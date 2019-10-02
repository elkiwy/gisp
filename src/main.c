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

//Global variables to handle environment and input to parse
static int look; 
static char token[SYMBOL_MAX]; /* token*/
FILE* inputFile = 0;
char* workingDir = "";
Environment* global_env = 0;

//Input parsing methods
int is_space(char x)  {
	return x == ' ' || x == '\n' || x == '\t';}
int is_parens(char x) {
	return x == '(' || x == ')' || x == '[' || x == ']' || x == '{' || x == '}';}


void skipLine(FILE* inputFile){
	char c = fgetc(inputFile);
	while(c != EOF && c != '\n'){
		c = fgetc(inputFile);
	}
	//if (c == '\n'){}
}

//Read char from input stream or from input file if provided
char read_char(){
	if (inputFile){
		//Read from the input file
		char c = fgetc(inputFile);
		while(c == ';'){
			skipLine(inputFile);
			c = fgetc(inputFile);
		}
		return c;
	}else{
		//Or read from stdin
		return getchar();
	}
}

//Get the next token from the input and store it into the token variable
static void gettoken() {
	int index = 0;

	//Skip all the white space
	while(is_space(look)) { look = read_char(); }

	if (is_parens(look)) {
		//Return just the parens
		token[index++] = look;  look = read_char();
	} else {
		//Return all the chars until a whitespace or a parens
		int in_quotes = 0;
		while(index < SYMBOL_MAX - 1 && look != EOF && ((!is_space(look) && !is_parens(look)) || in_quotes)) {
			if(look == '"'){in_quotes = !in_quotes;}
			token[index++] = look;  look = read_char();
		}
	}

	//Terminate the token string
	token[index] = '\0';
}

//Get list and get object function to help when parsing
// getobj() returns a List or an interned symbol
// getlist() returns an empty list or a cons with an object and a list
List* getlist();
void* getobj() {
	if (token[0] == '(') return getlist();
	if (token[0] == '[') return cons(intern("vector"), getlist());
	if (token[0] == '{') return cons(intern("hashmap"), getlist());
	if (token[0] == '#'){
		if (look != '('){
			printf("ERROR: \"#\" should always be followed by a list. \"%c\" was found instead.", look); fflush(stdout);	
			return 0;
		}
		look = read_char();
		List* expr = getlist();
		List* lam = cons(intern("lambda"), cons(cons(intern("%"), 0), cons(expr, 0)));
		return lam;
	}
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
		printf("%s %d - %s\n", prefix, i, (char*)car(car(a)));
		a = cdr(a);
		i++;
	}
}


//Add a new function to an environment
void extendEnv(char* name, void* value, Environment* env){
	//Add this symbol and value to the current environment level
	List* current = env->data;
	//printf("Adding \"%s\" to env %p and current %p\n", name, env, current); fflush(stdout);
	current = cons(cons(intern(name), cons(value, 0)), current);
	env->data = current;
}

List* eval(List* exp, Environment* env);
List* apply_lambda(List* lambda, List* args, Environment* env){
	//bind names into env and eval body
	Environment* innerEnv = makeEnvironment(NULL, env);
	List *names = second(lambda), *vars = args;
	for (  ; names ; names = cdr(names), vars = cdr(vars) ){
		char* sym = car(names);
		List* val = eval(car(vars), innerEnv);
		extendEnv(sym, val, innerEnv);
	}
	return eval(third(lambda), innerEnv);
}

//Eval functions
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
			//Bind all the values
			Environment* innerEnv = makeEnvironment(NULL, env);
			List *bindings = second(exp);
			while(bindings){
				char* sym = first(bindings);
				List* val = eval(second(bindings), innerEnv);
				extendEnv(sym, val, innerEnv);
				bindings = cdr(cdr(bindings));
				//printf("== Binded %s to ", (char*)sym); print_obj(val, 1); printf("\n");
			}

			//Evaluate all the body sexp in an implicit progn
			List* body = cdr(cdr(exp));
			List* result = 0;
			while(body){
				result = eval(car(body), innerEnv);
				body = cdr(body);}
			return result;


		/// (map function list)
		}else if (first(exp) == intern("map")){
			List* ret = 0;
			List* l = eval(third(exp), env);

			//If I got a vector convert it into a list
			if (is_vector(l)){l = vecToList((void**)untag_vector(l));}

			if (is_pair(second(exp))){
				//Lambda
				while (l){
					List* r = apply_lambda(second(exp), cons(car(l), 0), env);
					ret = cons(r, ret);
					l = cdr(l);
				}
			}else{
				//Known function name
				void* f = eval(second(exp), env);
				while (l){
					List* r = ((List* (*) (List*))f)(cons(car(l), 0));
					ret = cons(r, ret);
					l = cdr(l);
				}
			}
			return freverse(cons(ret, 0));

		/// (doseq (bind seq) body)
		}else if (first(exp) == intern("doseq")){
			Environment* innerEnv = makeEnvironment(NULL, env);
			char* sym = first(second(exp));
			List* seq = eval(second(second(exp)), env);
			List* body = cdr(cdr(exp));
			List* ret = 0;
			while(seq){
				//Update the symbol value
				extendEnv(sym, car(seq), innerEnv);
				//Eval the body and go to the next
				List* current = body;
				while(current){
					//Eval the body and go to the next
					ret = eval(car(current), innerEnv);
					current = cdr(current);
				}
				seq = cdr(seq);
			}
			return ret;

		//Keyword map member accessing
		}else if (*((char*)first(exp)) == ':'){
			List* obj = eval(second(exp), env);
			return fget(cons(obj, cons(first(exp), 0)));

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
		return apply_lambda(car(exp), cdr(exp), env);

	}
	printf("cannot evaluate expression %s\n", first(exp));
	return 0;
}



//Read the current input in stdin or the inputfile if present
List* read_and_eval(){
	List* result = 0;	
	look = read_char();
	while(look != EOF){
		gettoken();
		//Evaluate only if valid tokens
		if (strlen(token)>0)
			result = eval(getobj(), global_env);
		look = read_char();
	}
	return result;
}

//Basics I/O operations
List* fincludefile(List* a){
	char* path = (char*)trim_quotes(first(a));
	FILE* originalFile = inputFile;
	inputFile = fopen(path, "r");
	read_and_eval();
	inputFile = originalFile;
	return 0;
}
List* freadobj(List* a) {
	look = read_char();
	gettoken();
	return getobj();
}

List* fwriteobj(List* a){
	fflush(stdout);
	List* current = a;
	while(current){
		print_obj(car(current), 1);
		current = cdr(current);
	}

	puts("");
	return e_true;
}


//Main program entry
int main(int argc, char* argv[]) {
	//Check if I have a file to read
	printf("\n");
	if (argc>1){
		printf("Reading from file \"%s\"\n", argv[1]);
		inputFile = fopen(argv[1], "r");
		workingDir = argv[1];
		while (strlen(workingDir)>=1 && workingDir[strlen(workingDir)-1] != '/'){
			workingDir[strlen(workingDir)-1] = '\0';
		}
		printf("Working dir set on: \"%s\"\n", workingDir);
	}

	//Setup the profiling
	double time = 0.0;

	//Create the global environment
	clock_t begin = clock();
	global_env = makeEnvironment(NULL, NULL);
	extendEnv("null", 0, global_env);
	extendEnv("list",    (void*)flist, global_env);
	extendEnv("vector",  (void*)fvec, global_env);
	extendEnv("hashmap", (void*)fhashmap, global_env);
	extendEnv("count",   (void*)fcount, global_env);
	extendEnv("assoc",   (void*)fassoc, global_env);

	extendEnv("/", (void*)fdiv, global_env);
	extendEnv("*", (void*)fmul, global_env);
	extendEnv("+", (void*)fadd, global_env);
	extendEnv("-", (void*)fsub, global_env);
	extendEnv("mod",   (void*)fmodulo, global_env);
	extendEnv("pow",   (void*)fpow, global_env);
	extendEnv("sqrt",  (void*)fsqrt, global_env);
	extendEnv("atan2", (void*)fatan2, global_env);
	extendEnv("rand", (void*)frand, global_env);
	extendEnv("seed", (void*)fseed, global_env);
	extendEnv("=", (void*)feq, global_env);
	extendEnv("<", (void*)flessThan, global_env);
	extendEnv(">", (void*)fgreaterThan, global_env);

	extendEnv("sin", (void*)fsin, global_env);
	extendEnv("cos", (void*)fcos, global_env);
	extendEnv("dsin", (void*)fdsin, global_env);
	extendEnv("dcos", (void*)fdcos, global_env);
	extendEnv("deg", (void*)fdeg, global_env);
	extendEnv("rad", (void*)frad, global_env);
	extendEnv("int", (void*)fint, global_env);


	extendEnv("range", (void*)frange, global_env);
	extendEnv("reverse", (void*)freverse, global_env);
	extendEnv("concat", (void*)fconcat, global_env);
	extendEnv("first", (void*)ffirst, global_env);
	extendEnv("last", (void*)flast, global_env);

	extendEnv("make-surface",   (void*)fsvg_surface, global_env);
	extendEnv("make-context",   (void*)fsvg_context, global_env);
	extendEnv("surface-status", (void*)fsvg_status, global_env);
	extendEnv("surface-clean",  (void*)fsvg_clean, global_env);
	extendEnv("line",           (void*)fsvg_line, global_env);

	extendEnv("str",   (void*)fstr, global_env);

	extendEnv("include", (void*)fincludefile, global_env);
	extendEnv("read",    (void*)freadobj, global_env);
	extendEnv("write",   (void*)fwriteobj, global_env);

	extendEnv("null?",   (void*)fnull, global_env);
	extendEnv("symbol?", (void*)fatom, global_env);
	extendEnv("pair?",   (void*)fpair, global_env);
	extendEnv("eq?",     (void*)feq, global_env);
	extendEnv("cons", (void*)fcons, global_env);

	extendEnv("cdr", (void*)fcdr, global_env);
	extendEnv("car", (void*)fcar, global_env);
	extendEnv("get", (void*)fget, global_env);

	clock_t end_env = clock();


	printf("\n");

	//Evaluate everything 
	List* result = read_and_eval();
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



