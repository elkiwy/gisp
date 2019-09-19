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


//Add a new function to an environment
void extendEnv(char* name, void* value, Environment* env){
	//Add this symbol and value to the current environment level
	List* current = env->data;
	printf("Adding \"%s\" to env %p and current %p\n", name, env, current); fflush(stdout);
	current = cons(cons(intern(name), cons(value, 0)), current);
	env->data = current;
}

//Eval functions
List* eval(List* exp, Environment* env);
List* evlist(List* list, Environment* env) {
	List* head = 0,**args = &head;
	for ( ; list ; list = cdr(list)) {
		*args = cons(eval(car(list), env), 0);
		args = &((List*)untag(*args))->next;}
	return head;}
List* eval(List* exp, Environment* env) {
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

		//Check into the environment
		Environment* currentEnv = env;
		while(currentEnv){
			List* currentSymbols = currentEnv->data;
			for ( ; currentSymbols != 0; currentSymbols = cdr(currentSymbols) ){
				if (exp == first(car(currentSymbols))){
					//printf("->Evaluated %s in local env as: ", exp); print_obj(second(car(currentSymbols)), 1); printf("\n");
					return second(car(currentSymbols));}}
			//Search it into the outer level
			currentEnv = currentEnv->outer;
		}



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
			return exp;

		// (apply func args)
		} else if (first(exp) == intern("apply")) { 
			List* args = evlist(cdr(cdr(exp)), env);
			args = car(args);
			return ((List* (*) (List*))eval(second(exp), env)) (args);

		// (def symbol sexp)
		}else if (first(exp) == intern("def")){
			char* sym = second(exp);
			List* val = eval(third(exp), env);
			extendEnv(sym, val, env);
			//printf("== Defining %s to ", second(exp)); print_obj(value, 1); printf("\n");
			return val;

		// (defn symbol (params) sexp)
		}else if (first(exp) == intern("defn")){
			char* sym = second(exp);
			List* lambda = cons(intern("lambda"), cons(third(exp), cons(fourth(exp), 0)));
			extendEnv(sym, lambda, env);
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
			//List *extenv = env, *bindings = second(exp);
			Environment* innerEnv = makeEnvironment(NULL, env);
			List *bindings = second(exp);
			while(bindings){
				char* sym = first(bindings);
				List* val = eval(second(bindings), innerEnv);
				extendEnv(sym, val, innerEnv);
				//printf("== Binded %s to ", (char*)sym); print_obj(val, 1); printf("\n");
				bindings = cdr(cdr(bindings));
			}
			return eval(third(exp), innerEnv);

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
		Environment* innerEnv = makeEnvironment(NULL, env);
		List *names = second(car(exp)), *vars = cdr(exp);
		for (  ; names ; names = cdr(names), vars = cdr(vars) ){
			char* sym = car(names);
			List* val = eval(car(vars), innerEnv);
			extendEnv(sym, val, innerEnv);
		}
		return eval(third(car(exp)), innerEnv);
	}
	printf("cannot evaluate expression %s\n", first(exp));
	return 0;
}


//Basics I/O operations
List* freadobj(List* a) { look = read_char(); gettoken(); return getobj();  }
List* fwriteobj(List* a){
	fflush(stdout);
	print_obj(car(a), 1);
	puts("");
	return e_true;
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
	Environment* env = makeEnvironment(NULL, NULL);
	clock_t begin = clock();
	extendEnv("null", 0, env);
	extendEnv("list",    (void*)flist, env);
	extendEnv("vector",  (void*)fvec, env);
	extendEnv("hashmap", (void*)fhashmap, env);

	extendEnv("/", (void*)fdiv, env);
	extendEnv("*", (void*)fmul, env);
	extendEnv("+", (void*)fadd, env);
	extendEnv("-", (void*)fsub, env);

	extendEnv("sin", (void*)fsin, env);
	extendEnv("cos", (void*)fcos, env);
	extendEnv("dsin", (void*)fdsin, env);
	extendEnv("dcos", (void*)fdcos, env);
	extendEnv("deg", (void*)fdeg, env);
	extendEnv("rad", (void*)frad, env);

	extendEnv("range", (void*)frange, env);
	extendEnv("reverse", (void*)freverse, env);

	extendEnv("make-surface",   (void*)fsvg_surface, env);
	extendEnv("make-context",   (void*)fsvg_context, env);
	extendEnv("surface-status", (void*)fsvg_status, env);
	extendEnv("surface-clean",  (void*)fsvg_clean, env);
	extendEnv("line",           (void*)fsvg_line, env);

	extendEnv("str",   (void*)fstr, env);
	extendEnv("read",  (void*)freadobj, env);
	extendEnv("write", (void*)fwriteobj, env);
	extendEnv("null?",   (void*)fnull, env);
	extendEnv("symbol?", (void*)fatom, env);
	extendEnv("pair?",   (void*)fpair, env);
	extendEnv("eq?",     (void*)feq, env);
	extendEnv("cons", (void*)fcons, env);

	extendEnv("map", (void*)fmap, env);
	extendEnv("cdr", (void*)fcdr, env);
	extendEnv("car", (void*)fcar, env);
	extendEnv("get", (void*)fget, env);

	clock_t end_env = clock();


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
