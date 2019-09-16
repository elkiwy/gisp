#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include <time.h>

#include "hashmap.h"


#include "core.h"
#include "language.h"

//Handy debug method
#define debug(m,e) printf("%s:%d: %s:",__FILE__,__LINE__,m); print_obj(e,1); puts("");


//Static variables used for input parsing
static int look; 
static char token[SYMBOL_MAX]; /* token*/


FILE* inputFile = 0;
List* env_global = 0;

//Input parsing methods
int is_space(char x)  {
	return x == ' ' || x == '\n' || x == '\t';}
int is_parens(char x) {
	return x == '(' || x == ')' || x == '[' || x == ']' || x == '{' || x == '}';}

//Read char from input stream or from input file if provided
char read_char(){
	if (inputFile){
		char c = fgetc(inputFile);
		while(c == ';'){
			char* line = 0;
			size_t size;
			line = fgetln(inputFile, &size);
			c = fgetc(inputFile);
		}
		return c;
	}else{
		return getchar();
	}
}



static void gettoken() {
	int index = 0;
	while(is_space(look)) { look = read_char(); }

	if (is_parens(look)) {
		token[index++] = look;  look = read_char();
	} else {
		int in_quotes = 0;
		while(index < SYMBOL_MAX - 1 && look != EOF && ((!is_space(look) && !is_parens(look)) || in_quotes)) {
			if(look == '"'){in_quotes = !in_quotes;}
			token[index++] = look;  look = read_char();
		}
	}
	token[index] = '\0';
	//printf("returning token %s\n", token);
}


//Get list and get object function to help when parsing
// getobj() returns a List or an interned symbol
// getlist() returns an empty list or a cons with an object and a list
List* getlist();
void* getobj() {
	if (token[0] == '(') return getlist();
	if (token[0] == '[') return cons(intern("vector"), getlist());
	if (token[0] == '{') return cons(intern("hashmap"), getlist());
	return intern(token);}
List* getlist() {
	List* tmp;
	gettoken();
	if (token[0] == ')' || token[0] == ']' || token[0] == '}'){
		return 0;
	}
	tmp = getobj();
	return cons(tmp, getlist());
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
	//If is a tagged hashmap...
	if (is_hashmap(exp)){
		return exp;	

	//If is a tagged vector...
	}else if (is_vector(exp)){
		return exp;	

	//If is a number tagged...
	}else if (is_number(exp)){
		//printf("number found %s\n", (char*)exp);
		return exp;

	//If is an atom...
	}else if (is_atom(exp) ) {
		//printf("atom found %s\n", (char*)exp);
		//Check into global env
		List* temp_env = env_global;
		for ( ; temp_env != 0; temp_env = cdr(temp_env) ){
			if (exp == first(car(temp_env))){
				//printf("->Evaluated in global env as: ");
				//print_obj(second(car(temp_env)), 1);
				//printf("\n");
				return second(car(temp_env));}}

		//Check into local env
		temp_env = env;
		for ( ; temp_env != 0; temp_env = cdr(temp_env) ){
			if (exp == first(car(temp_env))){
				//printf("->Evaluated in local env as: ");
				//print_obj(second(car(temp_env)), 1);
				//printf("\n");
				return second(car(temp_env));}}

		//Check if it's a string
		if (*((char*)exp) == '"'){
			return exp;}

		//Check if it's a keyword
		if (*((char*)exp) == ':'){
			return exp;}

		//Check if it's a symbol representing a number (not tagged)
		char* err;
		double v = strtod((char*)exp, &err);
		if (*err == 0){
			//Convert it into a tagged number
			//printf("Converting to a number\n");
			return (List*)value_to_number(v);
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
			//printf("Searching for symbol %s\n", (char*)first(exp));
			List* primop = eval(first(exp), env);

			//user defined lambda, arg list eval happens in binding  below
			if (is_pair(primop)) { 
				//printf("found lambda %s\n", (char*)first(exp));
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














//Basics I/O operations
List* freadobj(List* a) { look = read_char(); gettoken(); return getobj();  }
List* fwriteobj(List* a){ print_obj(car(a), 1); puts(""); return e_true;  }

//Add a new function to an environment
List* extendEnv(char* name, void* func, List* env){
	return cons(cons(intern(name), cons(func, 0)), env);
}

//Main program entry
int main(int argc, char* argv[]) {
	//Check if I have a file to read
	printf("\n");
	if (argc>1){
		printf("Reading from file %s\n", argv[1]);
		inputFile = fopen(argv[1], "r");
	}

	//Setup the profiling
	double time = 0.0;

	//Create the global environment
	clock_t begin = clock();
	env_global = extendEnv("null", 0, 0);
	env_global = extendEnv("list",    (void*)flist, env_global);
	env_global = extendEnv("vector",  (void*)fvec, env_global);
	env_global = extendEnv("hashmap", (void*)fhashmap, env_global);

	env_global = extendEnv("/", (void*)fdiv, env_global);
	env_global = extendEnv("*", (void*)fmul, env_global);
	env_global = extendEnv("+", (void*)fadd, env_global);
	env_global = extendEnv("-", (void*)fsub, env_global);

	env_global = extendEnv("sin", (void*)fsin, env_global);
	env_global = extendEnv("cos", (void*)fcos, env_global);
	env_global = extendEnv("dsin", (void*)fdsin, env_global);
	env_global = extendEnv("dcos", (void*)fdcos, env_global);
	env_global = extendEnv("deg", (void*)fdeg, env_global);
	env_global = extendEnv("rad", (void*)frad, env_global);

	env_global = extendEnv("range", (void*)frange, env_global);
	env_global = extendEnv("reverse", (void*)freverse, env_global);

	env_global = extendEnv("surface",        (void*)fsvg_surface, env_global);
	env_global = extendEnv("context",        (void*)fsvg_context, env_global);
	env_global = extendEnv("surface-status", (void*)fsvg_status, env_global);
	env_global = extendEnv("surface-clean",  (void*)fsvg_clean, env_global);
	env_global = extendEnv("line",           (void*)fsvg_line, env_global);

	env_global = extendEnv("str",   (void*)fstr, env_global);
	env_global = extendEnv("read",  (void*)fread, env_global);
	env_global = extendEnv("write", (void*)fwrite, env_global);
	env_global = extendEnv("null?",   (void*)fnull, env_global);
	env_global = extendEnv("symbol?", (void*)fatom, env_global);
	env_global = extendEnv("pair?",   (void*)fpair, env_global);
	env_global = extendEnv("eq?",     (void*)feq, env_global);
	env_global = extendEnv("cons", (void*)fcons, env_global);

	env_global = extendEnv("map", (void*)fmap, env_global);
	env_global = extendEnv("cdr", (void*)fcdr, env_global);
	env_global = extendEnv("car", (void*)fcar, env_global);
	env_global = extendEnv("get", (void*)fget, env_global);

	clock_t end_env = clock();


	//Create an empty local environment
	List* env = 0;
	printf("\n");

	//Evaluate all the sexp as an implicit progn
	List* result = 0;
	look = read_char();
	while(look != EOF){
		gettoken();
		//Evaluate only if valid tokens
		if (strlen(token)>0)
			result = eval(getobj(), env);
		look = read_char();
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
