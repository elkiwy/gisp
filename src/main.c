/* This lisp dialect used "micro lisp" from A. Carl Douglas as a base.*/
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include <time.h>

#include "core.h"
#include "language.h"

//Handy debug method
#define debug(m,e) printf("%s:%d: %s:",__FILE__,__LINE__,m); print_obj(e,1); puts("");


//Static variables used for input parsing
static int look; 
static char token[SYMBOL_MAX]; /* token*/

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
	  //printf("atom found %s\n", (char*)exp);
		//Check if is a known symbol
		for ( ; env != 0; env = cdr(env) ){
		  if (exp == first(car(env))){
			printf("Found symbol %s in environment\n", (char*)exp);	
			return second(car(env));
		  }
		}

		//Check if it's a string
		if (*((char*)exp) == '"'){
		  return exp;
		}

		//Check if it's a number
		char* err;
		strtod((char*)exp, &err);
		if (*err == 0){
			return exp;			
		}

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
		//} else if (first(exp) == intern("lambda")) {
			//return exp; /* todo: create a closure and capture free vars*/

		// (apply func args)
		} else if (first(exp) == intern("apply")) { 
			List* args = evlist(cdr(cdr(exp)), env);
			args = car(args); /* assumes one argument and that it is a list*/
			return ((List* (*) (List*))eval(second(exp), env)) (args);

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
		    printf("searching for symbol %s\n", (char*)first(exp));
			List* primop = eval(first(exp), env);
            //user defined lambda, arg list eval happens in binding  below
			//if (is_pair(primop)) { 
			//	printf("found lambda %s\n", (char*)first(exp));
			//	return eval( cons(primop, cdr(exp)), env );
			//Built-in primitive
			//} else
			if (primop) { 
				printf("found primitive %s\n", (char*)first(exp));
				List* result = ((List* (*) (List*))primop) (evlist(cdr(exp), env));
				return result;
			}else{
				printf("didn't find anything for %s\n", (char*)first(exp));
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
	puts("cannot evaluate expression\n");
	return 0;
}

char* trim_quotes(char* s){
	s++;
	s[strlen(s)-1] = '\0';
	return s;
}


List* fstr(List* a) {
	char* str1 = (char*)first(a);
	char* str2 = (char*)second(a);
	str1 = trim_quotes(str1);
	str2 = trim_quotes(str2);

	char* str_res;
	str_res = malloc(strlen(str1)+strlen(str2)+1); 
	strcpy(str_res, str1); 
	strcat(str_res, str2); 

	return (List*)str_res;
}


//Basics I/O operations
List* freadobj(List* a) { look = getchar(); gettoken(); return getobj();  }
List* fwriteobj(List* a){ print_obj(car(a), 1); puts(""); return e_true;  }

//Main program entry
int main(int argc, char* argv[]) {

	double time = 0.0;
	clock_t begin = clock();

	List* env = cons(cons(intern("car"),	cons((void*)fcar,		0)),
				cons(cons(intern("cdr"),	cons((void*)fcdr,		0)),
				cons(cons(intern("cons"),	cons((void*)fcons,		0)),
				cons(cons(intern("eq?"),	cons((void*)feq,		0)),
				cons(cons(intern("pair?"),	cons((void*)fpair,		0)),
				cons(cons(intern("symbol?"),cons((void*)fatom,		0)),
				cons(cons(intern("null?"),	cons((void*)fnull,		0)),
				cons(cons(intern("read"),	cons((void*)freadobj,	0)),
				cons(cons(intern("write"),	cons((void*)fwriteobj,	0)),
				cons(cons(intern("str"),	cons((void*)fstr,   	0)),
				cons(cons(intern("+"),		cons((void*)fadd,		0)),
				cons(cons(intern("-"),		cons((void*)fsub,		0)),
				cons(cons(intern("*"),		cons((void*)fmul,		0)),
				cons(cons(intern("/"),		cons((void*)fdiv,		0)),
				cons(cons(intern("list"),	cons((void*)flist,		0)),
				cons(cons(intern("null"),	cons(0,                 0)), 0))))))))))))))));

	clock_t end_env = clock();

	look = getchar();
	gettoken();
	List* result = eval(getobj(), env);
	print_obj( result, 1 );
	

	clock_t end = clock();

	printf("\n\n");
	time += (double)(end_env - begin) / CLOCKS_PER_SEC;
	printf("Time elpased for setup %f\n", time);
	time += (double)(end - end_env) / CLOCKS_PER_SEC;
	printf("Time elpased for eval %f\n\n", time);

	return 0;
}
