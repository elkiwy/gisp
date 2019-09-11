/* This lisp dialect used "micro lisp" from A. Carl Douglas as a base.*/
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include <time.h>


#include "cairo.h"
#include "cairo-svg.h"

#include "core.h"
#include "language.h"

//Handy debug method
#define debug(m,e) printf("%s:%d: %s:",__FILE__,__LINE__,m); print_obj(e,1); puts("");


//Static variables used for input parsing
static int look; 
static char token[SYMBOL_MAX]; /* token*/


List* env_global = 0;

//Input parsing methods
int is_space(char x)  { return x == ' ' || x == '\n'; }
int is_parens(char x) { return x == '(' || x == ')'; }

static void gettoken() {
	int index = 0;
	while(is_space(look)) { look = getchar(); }

	if (is_parens(look)) {
		token[index++] = look;  look = getchar();
	} else {
		int in_quotes = 0;
		while(index < SYMBOL_MAX - 1 && look != EOF && ((!is_space(look) && !is_parens(look)) || in_quotes)) {
		    if(look == '"'){in_quotes = !in_quotes;}
			token[index++] = look;  look = getchar();
		}
	}
	token[index] = '\0';
}


//Get list and get object function to help when parsing
// getobj() returns a List or an interned symbol
// getlist() returns an empty list or a cons with an object and a list
List* getlist();
void* getobj() {
	if (token[0] == '(') return getlist();
	return intern(token);}
List* getlist() {
	List* tmp;
	gettoken();
	if (token[0] == ')') return 0;
	tmp = getobj();
	return cons(tmp, getlist());
}


//Prints a List object
void print_obj(List* ob, int head_of_list) {
	if (!is_pair(ob) ) {
		printf("%s", ob ? (char*) ob : "null" );
	} else {
		if (head_of_list) printf("(");
		print_obj(car(ob), 1);
		if (cdr(ob) != 0) {
			printf(" ");
			print_obj(cdr(ob), 0); //Change to 1 to enable the cons cells view
		}
		if (head_of_list) printf(")");
	}
}


void debug_printEnv(List* a, char* prefix){
  int i=0;
  while(a){
	printf("%s %d - %s\n", prefix, i, car(car(a)));
	a = cdr(a);
	i++;
  }
}


//Eval functions
List* eval(List* exp, List* env);
List* evlist(List* list, List* env) {
	List* head = 0,**args = &head;
	for ( ; list ; list = cdr(list)) {
		*args = cons(eval(car(list), env), 0);
		args = &((List*)untag(*args))->next;}
	return head;}
List* eval(List* exp, List* env) {
	//If is an atom...
	if (is_atom(exp) ) {
	    printf("atom found %s\n", (char*)exp);
		//Check into global env
		List* temp_env = env_global;
		for ( ; temp_env != 0; temp_env = cdr(temp_env) ){
			if (exp == first(car(temp_env))){
				printf("->Evaluated in global env as: ");
				print_obj(second(car(temp_env)), 1);
				printf("\n");
				return second(car(temp_env));}}

		//Check into local env
		temp_env = env;
		for ( ; temp_env != 0; temp_env = cdr(temp_env) ){
			if (exp == first(car(temp_env))){
				printf("->Evaluated in local env as: ");
				print_obj(second(car(temp_env)), 1);
				printf("\n");
				return second(car(temp_env));}}

		//Check if it's a string
		if (*((char*)exp) == '"'){
		  return exp;}

		//Check if it's a number
		char* err;
		strtod((char*)exp, &err);
		if (*err == 0){
			return exp;}

		//Else return it as an atom
		return 0;
	//Else if is a list with the first atom being an atom
	} else if (is_atom(first(exp))) { 
	  //printf("list with atom found %s\n", (char*) first(exp));
		// (quote X)
	    if (first(exp) == intern("quote")) {
			//Return the quoted element as it is
			//printf("returning quoted\n");
			return second(exp);

		// (if (cond) (success) (fail))
		} else if (first(exp) == intern("if")) {
			if (eval(second(exp), env) != 0)
				return eval(third(exp), env);
			else
				return eval(fourth(exp), env);

		// (lambda (params) body)
		} else if (first(exp) == intern("lambda")) {
			return exp; /* todo: create a closure and capture free vars*/

		// (apply func args)
		} else if (first(exp) == intern("apply")) { 
			List* args = evlist(cdr(cdr(exp)), env);
			args = car(args); /* assumes one argument and that it is a list*/
			return ((List* (*) (List*))eval(second(exp), env)) (args);

		// (def symbol sexp)
		}else if (first(exp) == intern("def")){
			List* value = eval(third(exp), env);
			env_global = cons(cons(second(exp), cons(value, 0)), env_global);
			return value;

		// (defn symbol (params) sexp)
		}else if (first(exp) == intern("defn")){
			List* lambda = cons(intern("lambda"), cons(third(exp), cons(fourth(exp), 0)));
			env_global = cons(cons(second(exp), cons(lambda, 0)), env_global);
			return lambda;

		// (progn exp1 exp2 ...)
		}else if (first(exp) == intern("progn")){
			List *sexp = cdr(exp), *result = 0;	
			while (sexp){
			    result = eval(first(sexp), env);
				sexp = cdr(sexp);
			}
			return result;

		// (let (binds) body)
		} else if (first(exp) == intern("let")) {
			List *extenv = env, *bindings = second(exp);
			while(bindings){
				extenv = cons(cons( first(bindings), cons(eval(second(bindings), env), 0)), extenv);
				bindings = cdr(cdr(bindings));
			}
			return eval(third(exp), extenv);

		// (function args)
		} else { 
		    printf("Searching for symbol %s\n", (char*)first(exp));
			List* primop = eval(first(exp), env);

            //user defined lambda, arg list eval happens in binding  below
			if (is_pair(primop)) { 
				printf("found lambda %s\n", (char*)first(exp));
				return eval( cons(primop, cdr(exp)), env );
			//Built-in primitive
			} else if (primop) { 
			    //printf("found primitive %s\n", (char*)first(exp));
				List* result = ((List* (*) (List*))primop) (evlist(cdr(exp), env));
				return result;
			}
		}
	// ((lambda (params) body) args)
	} else if (car(car(exp)) == intern("lambda")) {
	  //printf("lambda found\n");
	    //bind names into env and eval body
		List *extenv = env, *names = second(car(exp)), *vars = cdr(exp);
		for (  ; names ; names = cdr(names), vars = cdr(vars) ){
			extenv = cons(cons(car(names), cons(eval(car(vars), env), 0)), extenv);
		}
		return eval (third(car(exp)), extenv);
	}
	printf("cannot evaluate expression %s\n", first(exp));
	return 0;
}




List* fsvg_surface(List* a){
	float w = atof((char*)second(a));
	float h = atof((char*)third(a));
	char* n = first(a);
	n++;
	n[strlen(n)-1] = '\0';
	cairo_surface_t* surface = cairo_svg_surface_create(n, w, h);
    cairo_svg_surface_restrict_to_version (surface, 1);
    cairo_surface_set_fallback_resolution (surface, 72., 72.);
	return (List*)surface;
}

List* fsvg_status(List* a){
	printf("\nSURFACE STATUS: %s\n", cairo_status_to_string(cairo_surface_status(first(a))));
	fflush(stdout);
	return 0;
}

List* fsvg_context(List* a){
	cairo_t* context = cairo_create(first(a));
	return (List*)context;
}

List* fsvg_clean(List* a){
	cairo_surface_t* surface = first(a);
	cairo_t* context = second(a);
	cairo_surface_flush(surface);
	cairo_surface_finish(surface);
	cairo_surface_destroy(surface);
	cairo_destroy(context);
	return 0;
}


//Basics I/O operations
List* freadobj(List* a) { look = getchar(); gettoken(); return getobj();  }
List* fwriteobj(List* a){ print_obj(car(a), 1); puts(""); return e_true;  }

//Main program entry
int main(int argc, char* argv[]) {
    //Setup the profiling
	double time = 0.0;
	clock_t begin = clock();

	//Create the global environment
	env_global = cons(cons(intern("car"),	cons((void*)fcar,		0)),
				 cons(cons(intern("cdr"),	cons((void*)fcdr,		0)),
				 cons(cons(intern("cons"),	cons((void*)fcons,		0)),
				 cons(cons(intern("eq?"),	cons((void*)feq,		0)),
				 cons(cons(intern("pair?"),	cons((void*)fpair,		0)),
				 cons(cons(intern("symbol?"),cons((void*)fatom,		0)),
				 cons(cons(intern("null?"),	cons((void*)fnull,		0)),
				 cons(cons(intern("read"),	cons((void*)freadobj,	0)),
				 cons(cons(intern("write"),	cons((void*)fwriteobj,	0)),
				 cons(cons(intern("str"),	cons((void*)fstr,   	0)),
				 cons(cons(intern("svg-surface"),	cons((void*)fsvg_surface,   	0)),
				 cons(cons(intern("svg-context"),	cons((void*)fsvg_context,   	0)),
				 cons(cons(intern("svg-clean"),  	cons((void*)fsvg_clean,   	0)),
				 cons(cons(intern("svg-status"),  	cons((void*)fsvg_status,   	0)),
				 cons(cons(intern("+"),		cons((void*)fadd,		0)),
				 cons(cons(intern("-"),		cons((void*)fsub,		0)),
				 cons(cons(intern("*"),		cons((void*)fmul,		0)),
				 cons(cons(intern("/"),		cons((void*)fdiv,		0)),
				 cons(cons(intern("list"),	cons((void*)flist,		0)),
				 cons(cons(intern("null"),	cons(0,                 0)), 0))))))))))))))))))));
	clock_t end_env = clock();


	//Create an empty local environment
	List* env = 0;

	//Evaluate all the sexp as an implicit progn
	List* result = 0;
	look = getchar();
	while(look != EOF){
		gettoken();
		//Evaluate only if valid tokens
		if (strlen(token)>0)
			result = eval(getobj(), env);
		look = getchar();
	}

	//Print only the last result as a progn
	print_obj(result, 1);

	//Print the profiling
	clock_t end = clock();
	printf("\n\n");
	time += (double)(end_env - begin) / CLOCKS_PER_SEC;
	printf("Time elpased for setup %f\n", time);
	time += (double)(end - end_env) / CLOCKS_PER_SEC;
	printf("Time elpased for eval %f\n\n", time);

	return 0;
}
