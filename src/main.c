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

//Prepare variables for binary linked files
extern const char _binary_src_simplex_noise_gisp_start[];
extern const char _binary_src_simplex_noise_gisp_end[];
extern const char _binary_src_core_gisp_start[];
extern const char _binary_src_core_gisp_end[];
char* linkedFile = NULL;
int linkedFileIndex = 0;
int linkedFileSize = 0;

//Global variables to handle environment and input to parse
static int look; 
static int exitFlag = 0;
static char token[SYMBOL_MAX]; /* token*/
FILE* inputFile = 0;
char gispWorkingDir[4096];
Environment* global_env = 0;

//Memory Profiling variables
int vectorCount = 0;
int hashmapCount = 0;
int numberCount = 0;
int consCount = 0;
int environmentCounter_insert = 0;
int environmentCounter_search = 0;
double environmentCounter_searchTimeSum = 0;
double environmentCounter_searchTimeSum_hash = 0;

//Intern costant symbols
void* INTERN_quote	= 0;
void* INTERN_if		= 0;
void* INTERN_lambda	= 0;
void* INTERN_apply	= 0;
void* INTERN_def	= 0;
void* INTERN_defn	= 0;
void* INTERN_progn	= 0;
void* INTERN_let	= 0;
void* INTERN_reduce	= 0;
void* INTERN_map	= 0;
void* INTERN_mapv	= 0;
void* INTERN_doseq	= 0;
void* INTERN_profile= 0;

//Input parsing methods
int is_space(char x)  {
	return x == ' ' || x == '\n' || x == '\t';}
int is_parens(char x) {
	return x == '(' || x == ')' || x == '[' || x == ']' || x == '{' || x == '}';}

//Generic function to get the next char
char get_char(){
	//Read from linked file if it's set
	if (linkedFile){
		linkedFileIndex++;
		if (linkedFileIndex>=linkedFileSize) return EOF;
		return linkedFile[linkedFileIndex];
	//Read from input file
	}else if (inputFile){
		return fgetc(inputFile);		
	}

	//Return end of file if nothing of above is set
	return EOF;
}

//Read chars until the end of this line
void skipLine(){
	char c = get_char();
	while(c != EOF && c != '\n'){
		c = get_char();
	}
}

//Read char from input stream or from input file if provided
char read_char(){
	char c = get_char();
	while(c==';'){
		skipLine();
		c = get_char();
	}
	return c;
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
	//Parse list
	if (token[0] == '(') return getlist();
	//Parse vector
	if (token[0] == '[') return cons(intern("vector"), getlist());
	//Parse hashmap
	if (token[0] == '{') return cons(intern("hashmap"), getlist());
	//Parse lambda
	if (token[0] == '#'){
		if (look != '('){
			printf("ERROR: \"#\" should always be followed by a list. \"%c\" was found instead.", look); fflush(stdout);	
			return 0;
		}
		look = read_char();
		List* expr = getlist();
		List* lamda = cons(intern("lambda"), cons(cons(intern("%"), 0), cons(expr, 0)));
		return lamda;
	}
	//Else just return the symbol
	return intern(token);}
List* getlist() {
	//Get next token
	gettoken();
	//Check If i'm done with this list
	if (token[0] == ')' || token[0] == ']' || token[0] == '}') return 0;
	//If I'm not done get the full object from the current token and concatenate it to the rest of the list	
	List* tmp = getobj();
	return cons(tmp, getlist());
}

//Debug function to print the environment
void debug_printEnv(List* a, char* prefix){
	int i=0; while(a){printf("%s %d - %s\n", prefix, i, (char*)car(car(a))); a = cdr(a); i++;}
}




List* eval(List* exp, Environment* env);
List* apply_lambda(List* lambda, List* args, Environment* env){
	//bind names into env and eval body
	Environment* innerEnv = makeEnvironment(env);
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
	//printf("Eval %p in %p\n", (void*)exp, (void*)env);fflush(stdout);
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

		//Check if I have the symbol in the environment
		List* symbolValue = searchInEnvironment(exp, env);
		if (symbolValue != 0) return symbolValue;
		
		//Check if it's a string
		if (*((char*)exp) == '"'){return exp;}

		//Check if it's a keyword
		if (*((char*)exp) == ':'){return exp;}

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
	//(At this point we should be sure that the exp is a list)
	} else if (is_atom(first(exp))) { 
		// (quote X)
		if (first(exp) == INTERN_quote) {
			//Return the quoted element as it is
			//printf("returning quoted\n");
			return second(exp);

		// (if (cond) (success) (fail))
		} else if (first(exp) == INTERN_if) {
			if (eval(second(exp), env) != 0)
				return eval(third(exp), env);
			else
				return eval(fourth(exp), env);

		// (lambda (params) body)
		} else if (first(exp) == INTERN_lambda) {
			return exp;

		// (apply func args)
		} else if (first(exp) == INTERN_apply) { 
			List* args = evlist(cdr(cdr(exp)), env);
			args = car(args);
			return ((List* (*) (List*))eval(second(exp), env)) (args);

		// (def symbol sexp)
		}else if (first(exp) == INTERN_def){
			//printf("\n---found def expression \n");fflush(stdout);
			char* sym = second(exp);
			List* val = eval(third(exp), env);
			extendEnv(sym, val, env);
			//printf("== Done Defining %s ", second(exp)); print_obj(val, 1); printf("\n"); fflush(stdout);
			return val;

		// (defn symbol (params) sexp)
		}else if (first(exp) == INTERN_defn){
			char* sym = second(exp);
			List* lambda = cons(INTERN_lambda, cons(third(exp), cons(fourth(exp), 0)));
			extendEnv(sym, lambda, env);
			return lambda;

		// (progn exp1 exp2 ...)
		}else if (first(exp) == INTERN_progn){
			List *sexp = cdr(exp), *result = 0;	
			while (sexp){
				result = eval(first(sexp), env);
				sexp = cdr(sexp);
			}
			return result;

		// (let (binds) body)
		} else if (first(exp) == INTERN_let) {
			//Bind all the values
			Environment* innerEnv = makeEnvironment(env);
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


		/// (reduce function start list)
		}else if (first(exp) == INTERN_reduce){
			List* ret = eval(third(exp), env);
			List* l = eval(fourth(exp), env);

			//If I got a vector convert it into a list
			if (is_vector(l)){l = vecToList((Vector*)untag_vector(l));}

			if (is_pair(second(exp))){
				//Lambda
				while (l){
					List* args = cons(ret, cons(car(l), 0));
					List* r = apply_lambda(second(exp), args, env);
					ret = r;
					l = cdr(l);
				}
			}else{
				//Known function name
				void* f = eval(second(exp), env);
				while (l){
					List* args = cons(ret, cons(car(l), 0));
					List* r = ((List* (*) (List*))f)(args);
					ret = r;
					l = cdr(l);
				}
			}
			return ret;

		/// (map function list)
		}else if (first(exp) == INTERN_map){
			List* ret = 0;
			List* l = eval(third(exp), env);

			//If I got a vector convert it into a list
			if (is_vector(l)){l = vecToList((Vector*)untag_vector(l));}

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

		/// (mapv function list)
		}else if (first(exp) == INTERN_mapv){
			List* ret = 0;
			List* l = eval(third(exp), env);

			//If I got a vector convert it into a list
			if (is_vector(l)){l = vecToList((Vector*)untag_vector(l));}

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

			List* reversedList = freverse(cons(ret, 0)); 
			Vector* untagged_vec = listToVec(reversedList);
			return (List*)tag_vector(untagged_vec);

		/// (doseq (bind seq) body)
		}else if (first(exp) == INTERN_doseq){
			Environment* innerEnv = makeEnvironment(env);
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

		}else if (first(exp)== intern("exit")){
			printf("exiting \n");fflush(stdout);
			look = EOF;
			exitFlag = 1;
			return 0;

		/// (profile tag body)
		}else if (first(exp) == INTERN_profile){
			//Get the tag
			List* tag = eval(second(exp), env);

			//Eval and profile
			clock_t begin = clock();
			List* ret = 0;
			List* seq = cdr(cdr(exp));
			while(seq){
				ret = eval(car(seq), env);
				seq = cdr(seq);
			}
			clock_t end = clock();

			//Print the result
			printf("PROFILE for %s : %f seconds\n", (char*)tag, (double)(end - begin) / CLOCKS_PER_SEC);

			//Return the value
			return ret;

		//Keyword map member accessing
		}else if (*((char*)first(exp)) == ':'){
			List* obj = eval(second(exp), env);
			return fget(cons(obj, cons(first(exp), 0)));

		// (function args)
		} else { 
			//printf("-----Searching for symbol %s\n", (char*)first(exp));
			List* primop = eval(first(exp), env);

			//user defined lambda, arg list eval happens in binding  below
			if (is_pair(primop)) { 
				//printf("-----found lambda %s\n", (char*)first(exp));
				return eval( cons(primop, cdr(exp)), env );
			//Built-in primitive
			} else if (primop) { 
				//printf("-----found primitive %s\n", (char*)first(exp));
				List* result = ((List* (*) (List*))primop) (evlist(cdr(exp), env));
				return result;
			}
		}
	// ((lambda (params) body) args)
	} else if (car(car(exp)) == INTERN_lambda) {
		//printf("lambda found\n");

		//bind names into env and eval body
		return apply_lambda(car(exp), cdr(exp), env);
	}

	printf("cannot evaluate exp: %s %s in %p\n", (char*)exp, (char*)first(exp), env);
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

		if (exitFlag==0){look = read_char();
		}else{look = EOF;}
	}
	return result;
}

//Basics I/O operations
__attribute__((aligned(16))) List* fincludefile(List* a){
	char* path_rel = (char*)trim_quotes(first(a));
	char path_abs[1024];
	strcpy(path_abs, gispWorkingDir);
	strcat(path_abs, path_rel);
	FILE* originalFile = inputFile;
	inputFile = fopen(path_abs, "r");
	printf("Including source file \"%s\"...", (char*)path_abs);fflush(stdout);
	read_and_eval();
	inputFile = originalFile;
	printf("Done.\n");fflush(stdout);
	return 0;
}
__attribute__((aligned(16))) List* freadobj(List* a) {
	look = read_char();
	gettoken();
	return getobj();
}

__attribute__((aligned(16))) List* fwriteobj(List* a){
	fflush(stdout);
	List* current = a;
	while(current){
		print_obj(car(current), 1);
		current = cdr(current);
	}

	puts("");
	return e_true;
}



void includeLinkedBinaryFile(char* start, char* end){
	//Include gisp core
	linkedFile = start;
	linkedFileSize = end - start;
	printf("Including linked core.gisp size %d...", linkedFileSize);fflush(stdout);
	read_and_eval();
	printf("Done!\n");fflush(stdout);
	linkedFile = NULL;
	linkedFileIndex = 0;
}


//Main program entry
int main(int argc, char* argv[]) {
	printf("\n");
	if (argc>1){
		//Read the input file
		inputFile = fopen(argv[1], "r");
		if(realpath(argv[1], gispWorkingDir)==NULL){
			printf("Couldn't find input file '%s'.", argv[1]);exit(1);
		}
		printf("Reading from file \"%s\"\n", gispWorkingDir);

		//Remove the filename to get the working directory
		while (strlen(gispWorkingDir)>=1 && gispWorkingDir[strlen(gispWorkingDir)-1] != '/'){
			gispWorkingDir[strlen(gispWorkingDir)-1] = '\0';
		}
		printf("Working dir set on: \"%s\"\n", gispWorkingDir);
	}else{
		//Ask for input file
		printf("Please provide a .gisp file to read.\n");
		exit(1);
	}

	//Debug params
	int profile = 0;
	int memory = 0;
	if (argc>2){
		int i=2;
		while(i<argc){
			if (strcmp(argv[i], "--time")==0){
				profile = 1;	
			}else if (strcmp(argv[i], "--memory")==0){
				memory = 1;
			}
			i++;
		}
	}

	//Setup the profiling
	double time = 0.0;

	//Create the global environment
	clock_t begin = clock();
	global_env = makeEnvironment(NULL);
	extendEnv("null", 0, global_env);
	extendEnv("list",    (void*)flist, global_env);
	extendEnv("vector",  (void*)fvec, global_env);
	extendEnv("hashmap", (void*)fhashmap, global_env);
	extendEnv("count",   (void*)fcount, global_env);
	extendEnv("assoc",   (void*)fassoc, global_env);
	extendEnv("/",       (void*)fdiv, global_env);
	extendEnv("*",       (void*)fmul, global_env);
	extendEnv("+",       (void*)fadd, global_env);
	extendEnv("-",       (void*)fsub, global_env);
	extendEnv("bit-and", (void*)fbitAnd, global_env);
	extendEnv("mod",     (void*)fmodulo, global_env);
	extendEnv("pow",     (void*)fpow, global_env);
	extendEnv("sqrt",    (void*)fsqrt, global_env);
	extendEnv("log",     (void*)flog, global_env);
	extendEnv("log10",   (void*)flog10, global_env);
	extendEnv("atan2",   (void*)fatan2, global_env);
	extendEnv("rand",    (void*)frand, global_env);
	extendEnv("seed",    (void*)fseed, global_env);
	extendEnv("=",       (void*)feq, global_env);
	extendEnv("<",       (void*)flessThan, global_env);
	extendEnv(">",       (void*)fgreaterThan, global_env);
	extendEnv("sin",     (void*)fsin, global_env);
	extendEnv("cos",     (void*)fcos, global_env);
	extendEnv("dsin",    (void*)fdsin, global_env);
	extendEnv("dcos",    (void*)fdcos, global_env);
	extendEnv("deg",     (void*)fdeg, global_env);
	extendEnv("rad",     (void*)frad, global_env);
	extendEnv("int",     (void*)fint, global_env);
	extendEnv("floor",   (void*)ffloor, global_env);
	extendEnv("ceil",    (void*)fceil, global_env);
	extendEnv("min",     (void*)fminNum, global_env);
	extendEnv("max",     (void*)fmaxNum, global_env);
	extendEnv("range",   (void*)frange, global_env);
	extendEnv("reverse", (void*)freverse, global_env);
	extendEnv("concat",  (void*)fconcat, global_env);
	extendEnv("first",   (void*)ffirst, global_env);
	extendEnv("last",    (void*)flast, global_env);
	extendEnv("make-surface",   (void*)fsvg_surface, global_env);
	extendEnv("make-context",   (void*)fsvg_context, global_env);
	extendEnv("surface-status", (void*)fsvg_status, global_env);
	extendEnv("surface-clean",  (void*)fsvg_clean, global_env);
	extendEnv("line",           (void*)fsvg_line, global_env);
	extendEnv("surface-to-png", (void*)fsvg_to_png, global_env);
	extendEnv("str",     (void*)fstr, global_env);
	extendEnv("include", (void*)fincludefile, global_env);
	extendEnv("read",    (void*)freadobj, global_env);
	extendEnv("write",   (void*)fwriteobj, global_env);
	extendEnv("null?",   (void*)fnull, global_env);
	extendEnv("symbol?", (void*)fatom, global_env);
	extendEnv("pair?",   (void*)fpair, global_env);
	extendEnv("eq?",     (void*)feq, global_env);
	extendEnv("cons",    (void*)fcons, global_env);
	extendEnv("cdr",     (void*)fcdr, global_env);
	extendEnv("car",     (void*)fcar, global_env);
	extendEnv("get",     (void*)fget, global_env);

	//Intern all the macro strings
	INTERN_quote	= intern("quote");
	INTERN_if		= intern("if");
	INTERN_lambda	= intern("lambda");
	INTERN_apply	= intern("apply");
	INTERN_def		= intern("def");
	INTERN_defn		= intern("defn");
	INTERN_progn	= intern("progn");
	INTERN_let		= intern("let");
	INTERN_reduce	= intern("reduce");
	INTERN_map		= intern("map");
	INTERN_mapv		= intern("mapv");
	INTERN_doseq	= intern("doseq");
	INTERN_profile	= intern("profile");
	printf("\n");
	
	//Include gisp core
	includeLinkedBinaryFile((char*)_binary_src_core_gisp_start, (char*)_binary_src_core_gisp_end);
	includeLinkedBinaryFile((char*)_binary_src_simplex_noise_gisp_start, (char*)_binary_src_simplex_noise_gisp_end);

	//Evaluate everything 
	clock_t end_env = clock();
	List* result = read_and_eval();
	print_obj(result, 1);

	//Print the profiling
	clock_t end = clock();
	if (profile){
		printf("\n\nProfiling:\n");
		time += (double)(end_env - begin) / CLOCKS_PER_SEC;
		printf("Time elpased for setup %f\n", time);
		time += (double)(end - end_env) / CLOCKS_PER_SEC;
		printf("Time elpased for eval %f", time);
	}

	//Print memory usage and operations
	if (memory){
		printf("\n\nMemory Usage:\n");
		printf("Vector count: %d\n", vectorCount);
		printf("Cons count: %d\n", consCount);
		printf("Hashmap count: %d\n", hashmapCount);
		printf("Number count: %d\n", numberCount);
		printf("Environment insert count: %d\n", environmentCounter_insert);
		printf("Environment search count: %d\n", environmentCounter_search);
		printf("Environment search time: %f\n", environmentCounter_searchTimeSum);
		printf("Environment search  avg: %f\n", environmentCounter_searchTimeSum/(double)environmentCounter_search);
	}

	return 0;
}



