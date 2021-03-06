#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <stdbool.h>

#include "gispCore.h"
#include "hashmap.h"
#include "core.h"
#include "language.h"

#include "language3D.h"

//Handy debug method
#define debug(m,e) printf("%s:%d: %s:",__FILE__,__LINE__,m); print_obj(e,1); puts("");

#define MAX_ARGS_PER_FUNCTION 64

typedef struct profile_struct{
	clock_t total;
	clock_t ref;
	int calls;
}profile_struct;

map_t profile_map = NULL;
void profile(char* name, int active){
	if (profile_map==NULL){profile_map = hashmap_new();}
	profile_struct* prof;
	if (hashmap_get(profile_map, name, (any_t)&prof) == MAP_MISSING){
		prof = malloc(sizeof(profile_struct));
		prof->ref = -1;
		prof->total = 0;
		prof->calls = 0;
		hashmap_put(profile_map, name, prof);
	}
	if (active){
		prof->calls += 1;
		if (prof->ref != -1){
			prof->total += clock() - prof->ref;
		}
		prof->ref = clock();
	}else{
		prof->total += clock() - prof->ref;
		prof->ref = -1;
	}
	hashmap_put(profile_map, name, prof);
}

void profile_print(){
	int size = hashmap_length(profile_map);
	if (size>0){
		char* keys[size];
		profile_struct* structs[size];
		hashmap_keys_and_values(profile_map, keys, (void**)structs);
		for (int i=0; i<size; ++i){
			double total = ((double)structs[i]->total)/CLOCKS_PER_SEC;
			double calls = (double)structs[i]->calls;
			double avg   = total/calls;
			printf("\nProfile_total[%s] = %f, calls: %d, avg: %f", keys[i], total, (int)calls, avg);
		}
	}
}




//Prepare variables for binary linked files
extern unsigned char src_gisp_core_simplex_noise_gisp[];
extern unsigned int src_gisp_core_simplex_noise_gisp_len;
extern unsigned char src_gisp_core_core_gisp[];
extern unsigned int src_gisp_core_core_gisp_len;
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
int debugPrintInfo = 0;
int debugPrintFrees = 0;
int debugPrintCopy = 0;
int debugPrintAllocs = 0;

//Memory Profiling variables
int vectorCount = 0;
int hashmapCount = 0;
int numberCount = 0;
int consCount = 0;
int environmentCounter_insert = 0;
int environmentCounter_search = 0;
double environmentCounter_searchTimeSum = 0;
double environmentCounter_searchTimeSum_hash = 0;
void* allocations[1024*1024];

//Helper for sort function
List* sort_function = 0;
Environment* sort_environment = 0;

//Intern costant symbols
void* INTERN_quote	= 0;
void* INTERN_if		= 0;
void* INTERN_cond	= 0;
void* INTERN_lambda	= 0;
void* INTERN_apply	= 0;
void* INTERN_def	= 0;
void* INTERN_defn	= 0;
void* INTERN_progn	= 0;
void* INTERN_let	= 0;
void* INTERN_reduce	= 0;
void* INTERN_filter	= 0;
void* INTERN_sort	= 0;
void* INTERN_map	= 0;
void* INTERN_mapv	= 0;
void* INTERN_doseq	= 0;
void* INTERN_profile= 0;
void* INTERN_nil    = 0;

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
		List* lamda = cons(intern("lambda"), cons(cons(intern("%"), e_nil), cons(expr, e_nil)));
		return lamda;
	}
	//Else just return the symbol
	return intern(token);}
List* getlist() {
	//Get next token
	gettoken();
	//Check If i'm done with this list
	if (token[0] == ')' || token[0] == ']' || token[0] == '}') return e_nil;
	//If I'm not done get the full object from the current token and concatenate it to the rest of the list	
	List* tmp = getobj();
	return cons(tmp, getlist());
}


//Debug function to print the environment
void debug_printEnv(Environment* a){
	printf("\n\nEnvironament %p\n", a);fflush(stdout);
	debug_printMap(a->hashData);
}




List* eval(List* exp, Environment* env, bool autoclean);
List* apply_lambda(List* lambda, List* args, Environment* env, bool autoclean){
	if(debugPrintInfo){printf("===>Applying lambda ");fflush(stdout); print_obj(lambda, 1);fflush(stdout); printf(" with args ");fflush(stdout); print_obj(args, 1);fflush(stdout); printf(" \n");fflush(stdout);}

	//bind names into env and eval body
	Environment* innerEnv = makeEnvironment(env);
	if(debugPrintInfo){printf("===>binding names\n ");fflush(stdout);}
	List *names = second(lambda), *vars = args;
	for (  ; notNil(names) ; names = cdr(names), vars = cdr(vars) ){
		if(nil(names) || nil(vars)){break;}
		if(debugPrintInfo){char* s = objToString(car(vars), 1); printf("==> binding %s to value %s\n", (char*)car(names), s);free(s);fflush(stdout);}

		char* sym = car(names);
		List* val = eval(car(vars), env, true);
		consSetData(vars, e_nil);
		extendEnv(sym, val, innerEnv);
		if(debugPrintInfo){char* s = objToString(val, 1);printf("==> binded %s to value %s\n", sym, s);free(s);fflush(stdout);}
		objFree(val);
	}

	//Eval every body sexp in an implicit progn
	if(debugPrintInfo){printf("===>evaluating lambda in own environemnt\n ");fflush(stdout);}
	List *sexp = cdr(cdr(lambda)), *result = 0;	
	while (notNil(sexp)){
		if(debugPrintInfo){debugPrintObj("   Evaluating lambda sexp : ", car(sexp));}
		if (notNil(result)){objFree(result);}
		result = eval(car(sexp), innerEnv, autoclean);
		if(autoclean){consSetData(sexp, e_nil);}
		sexp = cdr(sexp);
	}

	environmentFree(innerEnv);
	return result;
}


int sortComparison_lambda(const void* elem1, const void* elem2) {
    List* a = *((List**)elem1);
    List* b = *((List**)elem2);
	List* lambda = objCopy(sort_function);
	List* args = cons(objCopy(a), cons(objCopy(b), e_nil));
	List* r = apply_lambda(lambda, args, sort_environment, true);
	objFree(lambda); objFree(args);
	if(r == e_true){objFree(r); return -1;
	}else if(r == e_false){objFree(r); return 1;
	}else{objFree(r); return 0;}
}



//Eval functions
//List* evlist(List* list, Environment* env) {
//	if(debugPrintInfo){debugPrintObj("===> Evaluating arg list ", list);}
//	//printf("{%p}\n", (void*)list);fflush(stdout);
//	List* head = 0;
//	List** args = &head;
//	for ( ; list ; list = cdr(list)) {
//		//debugPrintObj("->Evaluating arg ", car(list));
//		*args = cons(eval(car(list), env), 0);
//		args = &((List*)untag(*args))->next;
//	}
//	//debugPrintObj("->returning head ", head);
//	//printf("\e[39m");
//	return head;
//}


///=Gisp Macros
List* eval(List* exp, Environment* env, bool autoclean) {
	//printf("Eval %p in %p\n", (void*)exp, (void*)env);fflush(stdout);
	//If is a tagged hashmap...
	if (nil(exp) || exp==INTERN_nil){
		return e_nil;	

	//if is a tagged object...
	}else if (is_object(exp)){
		return exp;	

	//if is a tagged string...
	}else if (is_string(exp)){
		return exp;	

	//If is a tagged hasmap...
	}else if (is_hashmap(exp)){
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
		//printf("atom found %s (%p)\n", (char*)exp, exp);

		if (strlen((char*)exp)==0){
			//Evaluating a custom object
			return exp;
		}

		//Check if I have the symbol in the environment
		List* symbolValue = searchInEnvironment(exp, env);
		if (symbolValue != 0) return symbolValue;
		
		//Check if it's a string
		if (*((char*)exp) == '"'){
			char* str = newStringFromText((char*)exp);
			return (List*)tag_string(str);
			//return exp;
		}

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
		return exp;
	//Else if is a list with the first atom being an atom
	//(At this point we should be sure that the exp is a list)
	} else if (is_atom(first(exp))) { 
		if(debugPrintInfo){printf("\e[95m%p : ", exp); debugPrintObj("\e[95mEvaluating expression:" , exp); printf("\e[39m");fflush(stdout);}

		// (quote X)
		if (first(exp) == INTERN_quote) {
			//Return the quoted element as it is
			//printf("returning quoted\n");
			return second(exp);

		// (if (cond) (success) (fail))
		///+If cond is true evaluates success, else evaluates fail.
		///&if
		///#Any
		///@1cond
		///!1Boolean
		///@2success
		///!2Any
		///@3fail
		///!3Any
		} else if (first(exp) == INTERN_if) {
			List* condition = eval(second(exp), env, true);
			consSetData(cdr(exp), e_nil);
			List* ret;
			if (condition != e_false){
				ret = eval(third(exp), env, true);
				consSetData(cdr(cdr(exp)), e_nil);
			}else{
				ret = eval(fourth(exp), env, true);
				consSetData(cdr(cdr(cdr(exp))), e_nil);
			}
			objFree(condition);
			objFree(exp);
			if(debugPrintInfo){printf("\e[96m%p : ", exp); debugPrintObj("\e[96mIf macro Evaluated to:" , ret); printf("\e[39m");fflush(stdout);}
			return ret;

		// (cond (cond1) (body1) ...)
		///+Takes couple of condition and body, evaluates conditions until finds something true and evaluates its paired body.
		///&cond
		///#Any
		///@1cond1
		///!1Boolean
		///@2body1
		///!2Any
		///@3...
		///!3Boolean
		///@4...
		///!4Any
		} else if (first(exp) == INTERN_cond) {
			List* ret = e_nil;
			int done = 0;
			List* condToEval = cdr(exp);
			while(done==0 && notNil(condToEval)){
				List* condition = eval(car(condToEval), env, true);
				consSetData(condToEval, e_nil);

				if (condition != e_false){
					List* expToEval = cdr(condToEval);
					ret = eval(car(expToEval), env, true);
					consSetData(expToEval, e_nil);
					done = 1;
				}else{
					condToEval = cdr(cdr(condToEval));
				}
			}

			objFree(exp);
			if(debugPrintInfo){printf("\e[96m%p : ", exp); debugPrintObj("\e[96mcond macro Evaluated to:" , ret); printf("\e[39m");fflush(stdout);}
			return ret;

		// (lambda (params) body)
		///+Creates a function with params and body
		///&lambda
		///#Function
		///@1params
		///!1List
		///@2body
		///!2Any
		} else if (first(exp) == INTERN_lambda) {
			if(debugPrintInfo){printf("\e[96m%p : ", exp); debugPrintObj("\e[96mlambda Evaluated to:" , exp); printf("\e[39m");fflush(stdout);}
			return exp;


		// (apply func args)
		///+Applies the function f on each element of args
		///&Any
		///#Function
		///@1f
		///!1Function
		///@2Sequence
		///!2args
		} else if (first(exp) == INTERN_apply) { 
			List* args = eval(car(cdr(cdr(exp))), env, true);
			consSetData(cdr(cdr(exp)), args);
			List* function = eval(second(exp), env, true);
			consSetData(cdr(exp), function);
			List* ret = ((List* (*) (List*))function) (args);
			objFree(exp);
			if(debugPrintInfo){printf("\e[96m%p : ", exp); debugPrintObj("\e[96mapply Evaluated to:" , ret); printf("\e[39m");fflush(stdout);}
			return ret;

		// (def symbol value)
		///+Defines a symbol with a value
		///&def
		///#Nil
		///@1symbol
		///!1Atom
		///@2value
		///!2Any
		}else if (first(exp) == INTERN_def){
			if(debugPrintInfo){printf("\n===> found def expression \n");fflush(stdout);}
			char* sym = second(exp);
			List* val = eval(third(exp), env, true);

			//Take global env
			Environment* globalEnv = env;
			while(globalEnv->outer != NULL){globalEnv = globalEnv->outer;}
			extendEnv(sym, val, globalEnv);

			//printf("== Done Defining %s ", sym); print_obj(val, 1); printf("\n"); fflush(stdout);
			consFree(cdr(cdr(exp)));
			consSetNext(cdr(exp), e_nil);
			objFree(exp);
			objFree(val);
			if(debugPrintInfo){printf("\e[96m%p : ", exp); debugPrintObj("def Evaluated to:" , e_nil); printf("\e[39m");fflush(stdout);}
			return e_nil;

		// (defn symbol (params) sexp)
		///+Defines a symbol with a function.
		///&defn
		///#Nil
		///@1symbol
		///!1Atom
		///@2params
		///!2List
		///@3body
		///!3Any
		}else if (first(exp) == INTERN_defn){
			if(debugPrintInfo){debugPrintObj("\n===> found defn expression ", exp);}
			//Get function name
			char* sym = second(exp);

			//Expand defn macro
			List* lambda = cons(INTERN_lambda, cons(third(exp), cdr(cdr(cdr(exp)))));
			if(debugPrintInfo){debugPrintObj("===> lambda: ", lambda);}

			//Export the function into the environment
			extendEnv(sym, lambda, env);

			//Cleanup memory and return
			consFree(cdr(lambda));
			consFree(lambda);
			objFree(exp);
			if(debugPrintInfo){printf("\e[96m%p : ", exp); debugPrintObj("defn Evaluated to:" , e_nil); printf("\e[39m");fflush(stdout);}
			return e_nil;

		// (progn exp1 exp2 ...)
		///+Evaluates multiple bodies and returns the value of the last one.
		///&progn
		///#Any
		///@1body1
		///!1Any
		///@2...
		///!2Any
		}else if (first(exp) == INTERN_progn){
			List* sexp = cdr(exp);
			List* ret = e_nil;	
			while (notNil(sexp)){
				if(notNil(ret)){objFree(ret);}
				ret = eval(first(sexp), env, true);
				consSetData(sexp, e_nil);
				sexp = cdr(sexp);
			}
			objFree(exp);
			if(debugPrintInfo){printf("\e[96m%p : ", exp); debugPrintObj("progn Evaluated to:" , ret); printf("\e[39m");fflush(stdout);}
			return ret;

		// (let (binds) body)
		///+Binds pairs of symbols and values, and then evaluates the body.
		///&let
		///#Any
		///@1binds
		///!1List
		///@2body
		///!2Any
		} else if (first(exp) == INTERN_let) {
			//Bind all the values
			Environment* innerEnv = makeEnvironment(env);
			List *bindings = second(exp);
			while(notNil(bindings)){
				char* sym = first(bindings);
				List* val = eval(second(bindings), innerEnv, true);
				consSetData(cdr(bindings), e_nil);
				extendEnv(sym, val, innerEnv);
				objFree(val);
				bindings = cdr(cdr(bindings));
				//printf("== Binded %s to ", (char*)sym); print_obj(val, 1); printf("\n");
			}

			//Evaluate all the body sexp in an implicit progn
			List* body = cdr(cdr(exp));
			List* result = e_nil;
			while(notNil(body)){
				if(notNil(result))objFree(result);
				result = eval(car(body), innerEnv, true);
				consSetData(body, e_nil);
				body = cdr(body);
			}

			objFree(exp);

			environmentFree(innerEnv);
			if(debugPrintInfo){printf("\e[96m%p : ", exp); debugPrintObj("let Evaluated to:" , result); printf("\e[39m");fflush(stdout);}
			return result;

		/// (reduce function start list)
		///+Evaluates function with each element of seq and the result of the evaluation of it on the previous element. Starting with start as first result.
		///&reduce
		///#Any
		///@1function
		///!1Function
		///@2start
		///!2Any
		///@3seq
		///!3Sequence
		}else if (first(exp) == INTERN_reduce){
			List* ret = eval(third(exp), env, true);
			consSetData(cdr(cdr(exp)), e_nil);
			List* seqFirst = eval(fourth(exp), env, true);
			List* seq = seqFirst;
			consSetData(cdr(cdr(cdr(exp))), e_nil);

			//If I got a vector convert it into a list
			if (is_vector(seq)){seq = vecToList((Vector*)untag_vector(seq));}

			List* function = eval(second(exp), env, true);
			consSetData(cdr(exp), env);
			if (is_pair(function)){
				//Lambda
				List* lambda = function;
				while (notNil(seq)){
					List* lambdaCopy = objCopy(lambda);
					List* lambdaArgs = cons(ret, cons(objCopy(car(seq)), e_nil));
					List* r = apply_lambda(lambdaCopy, lambdaArgs, env, true);
					objFree(lambdaCopy);
					objFree(lambdaArgs);
					ret = r;
					seq = cdr(seq);
				}
			}else{
				//Known function name
				void* f = function;
				while (notNil(seq)){
					List* args = cons(ret, cons(objCopy(car(seq)), e_nil));
					List* r = ((List* (*) (List*))f)(args);
					objFree(args);
					ret = r;
					seq = cdr(seq);
				}
			}
			objFree(function);
			objFree(seqFirst);
			objFree(exp);

			if(debugPrintInfo){printf("\e[96m%p : ", exp); debugPrintObj("reduce Evaluated to:" , ret); printf("\e[39m");fflush(stdout);}
			return ret;

		/// (map function seq)
		///+Evaluate the function on the sequence seq returning a list of evaluated elements.
		///&map
		///#List
		///@1function
		///!1Function
		///@2seq
		///!2Sequence
		}else if (first(exp) == INTERN_map){
			if(debugPrintInfo){debugPrintObj("===>Evaluating map ", exp);}

			//Get the sequence
			List* seqFirst = eval(third(exp), env, true);
			List* seq = seqFirst;
			consSetData(cdr(cdr(exp)), e_nil);

			//If I got a vector convert it into a list
			if (is_vector(seqFirst)){
				Vector* vec = (Vector*)untag_vector(seqFirst);
				List* list = vecToList(vec);
				objFree((List*)seqFirst);
				seqFirst = list;
				seq = list;
			}


			List* ret = e_nil;
			List* function = eval(second(exp), env, true);
			consSetData(cdr(exp), env);
			if (is_pair(function)){
				//Lambda
				while (notNil(seq)){
					List* lambdaArg = cons(objCopy(car(seq)), e_nil);
					List* r = apply_lambda(function, lambdaArg, env, false);
					objFree(lambdaArg);
					ret = cons(r, ret);
					seq = cdr(seq);
				}
			}else{
				//Known function name
				void* f = function;
				if(nil(f)){printf("\nERROR: \"%s\" is not a valid function for map. Exiting.\n\n", (char*)second(exp));fflush(stdout);exit(1);}
				while (notNil(seq)){
					List* functionArg = cons(objCopy(car(seq)), e_nil);
					List* r = ((List* (*) (List*))f)(functionArg);
					objFree(functionArg);
					ret = cons(r, ret);
					seq = cdr(seq);
				}
			}

			//Reverse the result list
			List* correct = e_nil;
			List* l = ret;
			while(notNil(l)){
				correct = cons(car(l), correct);
				l = cdr(l);
			}

			//Clean the previous result cons since we recycled the same objects during the reverse
			listFreeOnlyCons(ret);

			objFree(function);
			objFree(seqFirst);
			objFree(exp);

			if(debugPrintInfo){printf("\e[96m%p : ", exp); debugPrintObj("\e[96mmap Evaluated to:" , correct); printf("\e[39m");fflush(stdout);}

			return correct;


		///+Evaluate the function on the sequence seq returning a vector of evaluated elements.
		///&mapv
		///#Vector
		///@1function
		///!1Function
		///@2seq
		///!2Sequence
		}else if (first(exp) == INTERN_mapv){
			if(debugPrintInfo){debugPrintObj("===>Evaluating map ", exp);}

			//Get the sequence
			List* seqFirst = eval(third(exp), env, true);
			List* seq = seqFirst;
			consSetData(cdr(cdr(exp)), e_nil);

			//If I got a vector convert it into a list
			if (is_vector(seqFirst)){
				Vector* vec = (Vector*)untag_vector(seqFirst);
				List* list = vecToList(vec);
				objFree((List*)seqFirst);
				seqFirst = list;
				seq = list;
			}


			//Get the sequence size and prepare the result vector
			int size = 0;
			List* items = seq;
			while(notNil(items)){size++; items = cdr(items);}
			Vector* ret_vec = newVec(size);
			void** ret_vec_data = ret_vec->data;

			//Evaluate and fill the result vector
			List* function = eval(second(exp), env, true);
			consSetData(cdr(exp), env);
			if (is_pair(function)){
				//Lambda
				int i = 0;
				while (notNil(seq)){
					List* lambdaArg = cons(objCopy(car(seq)), e_nil);
					List* r = apply_lambda(function, lambdaArg, env, false);
					objFree(lambdaArg);
					ret_vec_data[i] = r;
					seq = cdr(seq);
					i++;
				}
			}else{
				//Known function name
				void* f = function;
				int i = 0;
				if(nil(f)){printf("\nERROR: \"%s\" is not a valid function for map. Exiting.\n\n", (char*)second(exp));fflush(stdout);exit(1);}
				while (notNil(seq)){
					List* functionArg = cons(objCopy(car(seq)), e_nil);
					List* r = ((List* (*) (List*))f)(functionArg);
					objFree(functionArg);
					ret_vec_data[i] = r;
					seq = cdr(seq);
					i++;
				}
			}

			objFree(function);
			objFree(seqFirst);
			objFree(exp);

			List* finalResult = (List*)tag_vector(ret_vec);
			if(debugPrintInfo){printf("\e[96m%p : ", exp); debugPrintObj("\e[96mmap Evaluated to:" , finalResult); printf("\e[39m");fflush(stdout);}

			return finalResult;

		/// (filter function seq)
		///+Filter a sequence seq evaluating function on each element of seq and returning only the ones evaluating the function to true.
		///&filter
		///#List
		///@1function
		///!1Function
		///@2seq
		///!2Sequence
		}else if (first(exp) == INTERN_filter){
			if(debugPrintInfo){debugPrintObj("===>Evaluating filter ", exp);}
			List* ret = e_nil;

			//Get the sequence
			List* seqFirst = eval(third(exp), env, true);
			List* seq = seqFirst;
			consSetData(cdr(cdr(exp)), e_nil);

			//If I got a vector convert it into a list
			if (is_vector(seqFirst)){
				Vector* vec = (Vector*)untag_vector(seqFirst);
				List* list = vecToList(vec);
				objFree((List*)seqFirst);
				seqFirst = list;
				seq = list;
			}

			List* function = eval(second(exp), env, true);
			consSetData(cdr(exp), env);
			if (is_pair(function)){
				//Lambda
				List* lambda = function;
				while (notNil(seq)){
					List* lambdaCopy = objCopy(lambda);
					List* lambdaArg = cons(objCopy(car(seq)), e_nil);
					List* r = apply_lambda(lambdaCopy, lambdaArg, env, true);
					objFree(lambdaCopy);
					objFree(lambdaArg);
					if(r==e_true){
						ret = cons(objCopy(car(seq)), ret);
					}
					seq = cdr(seq);
				}
			}else{
				//Known function name
				void* f = function;
				if(nil(f)){printf("\nERROR: \"%s\" is not a valid function for map. Exiting.\n\n", (char*)second(exp));fflush(stdout);exit(1);}
				while (notNil(seq)){
					List* functionArg = cons(objCopy(car(seq)), e_nil);
					List* r = ((List* (*) (List*))f)(functionArg);
					objFree(functionArg);
					if(r==e_true){
						ret = cons(objCopy(car(seq)), ret);
					}
					seq = cdr(seq);
				}
			}

			List* reversed = cons(ret, e_nil);
			List* correct = freverse(reversed);
			objFree(reversed);
			objFree(function);
			objFree(seqFirst);
			objFree(exp);

			if(debugPrintInfo){printf("\e[96m%p : ", exp); debugPrintObj("\e[96mfilter Evaluated to:" , correct); printf("\e[39m");fflush(stdout);}
			return correct;



		/// (sort function seq)
		///+Sort a sequence seq using function as comparator.
		///&sort
		///#List
		///@1function
		///!1Function
		///@2seq
		///!2Sequence
		}else if (first(exp) == INTERN_sort){
			if(debugPrintInfo){debugPrintObj("===>Evaluating sort ", exp);}
			List* ret = e_nil;

			//Get the sequence
			List* seq = eval(third(exp), env, true);
			consSetData(cdr(cdr(exp)), e_nil);
			if (is_pair(seq)){
				Vector* vecSeq = listToVec(seq);
				objFree(seq);
				seq = (List*)tag_vector(vecSeq);
			}

			List* function = eval(second(exp), env, true);
			consSetData(cdr(exp), env);
			if (is_pair(function)){
				//Lambda
				sort_function = function;
				List* newSeq = objCopy(seq);
				Vector* vecCopy = (Vector*)untag_vector(newSeq);
				void** data = vecCopy->data;
				sort_environment = env;
				qsort(data, vecCopy->size, sizeof(List*), sortComparison_lambda);
				ret = (List*)tag_vector(vecCopy);
			}

			objFree(function);
			objFree(seq);
			objFree(exp);

			if(debugPrintInfo){printf("\e[96m%p : ", exp); debugPrintObj("\e[96msort Evaluated to:" , ret); printf("\e[39m");fflush(stdout);}
			return ret;



		/// (doseq (bind seq) body)
		///+Providing a sequence and a symbol in binds, evaluates body multiple times on each element of the sequence binded to the symbol. Returns the value of the last evaluation.
		///&doseq
		///#Any
		///@1binds
		///!1List
		///@2body
		///!2Any
		}else if (first(exp) == INTERN_doseq){
			if(debugPrintInfo){debugPrintObj("===>Expanding doseq macro  ", exp);}
			Environment* innerEnv = makeEnvironment(env);
			char* sym = first(second(exp));

			//Create a copy of the seq expression and evaluate the copy so it free itself in eval
			List* seqCopy = objCopy(second(second(exp)));
			List* evaluatedSeq = eval(seqCopy, env, true);
			List* seqCurrent = evaluatedSeq;

			List* body = cdr(cdr(exp));
			List* ret = e_nil;
			while(notNil(seqCurrent)){
				//Update the symbol value
				extendEnv(sym, car(seqCurrent), innerEnv);

				//Eval the body and go to the next
				List* bodyCopy = objCopy(body);
				List* current = bodyCopy;
				while(notNil(current)){
					//Eval the body and go to the next
					if (notNil(ret)) objFree(ret);
					ret = eval(car(current), innerEnv, true);
					consSetData(current, e_nil);
					current = cdr(current);
				}
				objFree(bodyCopy);
				seqCurrent = cdr(seqCurrent);
			}

			objFree(evaluatedSeq);
			objFree(exp);
			environmentFree(innerEnv);
			if(debugPrintInfo){printf("\e[96m%p : ", exp); debugPrintObj("\e[96mdoseq Evaluated to:" , ret); printf("\e[39m");fflush(stdout);}
			return ret;

		}else if (first(exp)== intern("exit")){
			printf("exiting \n");fflush(stdout);
			look = EOF;
			exitFlag = 1;
			objFree(exp);
			return e_nil;

		/// (profile tag body)
		///+Evaluate body and prints the tag and how much time the evaluation took. Returns the value of the evaluation.
		///&profile
		///#Any
		///@1tag
		///!1String
		///@2body
		///!2Any
		}else if (first(exp) == INTERN_profile){
			//Get the tag
			List* tag = eval(second(exp), env, true);

			//Eval and profile
			clock_t begin = clock();
			List* ret = e_nil;
			List* seq = cdr(cdr(exp));
			while(notNil(seq)){
				if(notNil(ret)){objFree(ret);}
				ret = eval(car(seq), env, true);
				consSetData(seq, e_nil);
				seq = cdr(seq);
			}
			clock_t end = clock();

			//Print the result
			printf("PROFILE for %s : %f seconds\n", (char*)tag, (double)(end - begin) / CLOCKS_PER_SEC);
			objFree(tag);
			consSetData(cdr(exp), e_nil);
			objFree(exp);

			//Return the value
			if(debugPrintInfo){printf("\e[96m%p : ", exp); debugPrintObj("profile Evaluated to:" , ret); printf("\e[39m");fflush(stdout);}
			return ret;

		//Keyword map member accessing
		}else if (*((char*)first(exp)) == ':'){
			List* obj = eval(second(exp), env, true);
			consSetData(cdr(exp), e_nil);

			//Equivalent to (get hashmap keyword)
			List* expandedExp = cons(obj, cons(objCopy(first(exp)), e_nil));
			List* ret = fget(expandedExp);
			objFree(expandedExp);
			objFree(exp);
			if(debugPrintInfo){printf("\e[96m%p : ", exp); debugPrintObj("keyword access Evaluated to:" , ret); printf("\e[39m");fflush(stdout);}
			return ret;

		// (function args)
		} else { 
			List* primop = eval(first(exp), env, true);

			//user defined lambda, arg list eval happens in binding  below
			if (is_pair(primop)) { 
				if(debugPrintInfo){printf("===>Found user defined function, expanding it into a lambda expression. "); debugPrintObj("Exp:", exp); printf("   %p ", primop);fflush(stdout); debugPrintObj(" Primop ", primop);}

				//Es:
				//Gets an exp like: (point 1 2)
				//Reads "point" and evaluate it into (lambda (x y) (hashmap :x x :y y)) 
				//Put that into a cons with the arguments of the lambda
				//Evaluate it
				List* lambda = cons(primop, objCopy(cdr(exp)));
				List* result = eval(lambda, env, autoclean); 

				//objFree(lambda);
				if(autoclean){objFree(exp);}
				if(debugPrintInfo){printf("\e[96m%p : ", exp); debugPrintObj("\e[96muser defined function Evaluated to:" , result); printf("\e[39m");fflush(stdout);}
				return result;

			//Built-in primitive
			} else if (primop) { 
				if(debugPrintInfo){printf("====> %p", exp);fflush(stdout); debugPrintObj(" Evaluating ", exp);}

				List* argsToEval = cdr(exp);


				//Make a bunch of slots for the args value
				List args_stack[MAX_ARGS_PER_FUNCTION] = {0};
				List* args = (List*)tag(&args_stack[0]);
				int args_ind = 0;
				for ( ; notNil(argsToEval) ; argsToEval = cdr(argsToEval)) {
					//Take the current slot
					args_stack[args_ind].data = eval(car(argsToEval), env, autoclean);
					args_stack[args_ind].next = e_nil;

					//Link to previous cons
					if (args_ind>0){ args_stack[args_ind-1].next = (List*)tag(&args_stack[args_ind]); }

					//Autoclean if necessary and go next
					if(autoclean){consSetData(argsToEval, e_nil);}
					args_ind++;
				}

				//Evaluate expression with evaluated arguments
				List* result = ((List* (*) (List*))primop) (args);

				//Free the current expression
				if(autoclean){objFree(exp);}
				if(debugPrintInfo){printf("\e[96m%p : ", exp); debugPrintObj("primitive Evaluated to:" , result); printf("\e[39m");fflush(stdout);}
				return result;
			}
		}
	// ((lambda (params) body) args)
	} else if (is_pair(car(exp)) && car(car(exp)) == INTERN_lambda) {
		if(debugPrintInfo){printf("\e[95m%p : ", exp); debugPrintObj("\e[95mEvaluating lambda expression:" , exp); printf("\e[39m");fflush(stdout);}

		//bind names into env and eval body
		List* ret = apply_lambda(car(exp), cdr(exp), env, true);
		objFree(exp);

		if(debugPrintInfo){printf("\e[96m%p : ", exp); debugPrintObj("lambda Evaluated to:" , ret); printf("\e[39m");fflush(stdout);}
		return ret;

	// Else should be just a normal list
	} else if (is_pair(exp)){
		if(!debugPrintInfo){printf("\e[96m%p : ", exp); debugPrintObj("list Evaluated to itself:" , exp); printf("\e[39m");fflush(stdout);}
		return exp;
	}


	printf("ERROR: cannot evaluate exp: %p \"%s\" in %p\n", (char*)exp, (char*)first(exp), env);
	exit(1);
	return e_nil;
}



//Read the current input in stdin or the inputfile if present
List* read_and_eval(){
	List* result = e_nil;	
	look = read_char();
	while(look != EOF){
		gettoken();
		//Evaluate only if valid tokens
		if (strlen(token)>0){
			debug_printAllocations();
			if(debugPrintInfo){printf("\n\e[36m***> Reading\n");fflush(stdout);}
			List* obj = getobj();
			if(debugPrintInfo){
				debugPrintObj("Read: ", obj);
				printf("\e[39m"); fflush(stdout);}
			if(notNil(result)){objFree(result);result=e_nil;}
			result = eval(obj, global_env, true);
		}

		if (exitFlag==0){look = read_char();
		}else{look = EOF;}
	}

	if(debugPrintInfo){printf("\nEvaluation completed.\n\n");fflush(stdout);}
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
	List* result = read_and_eval();
	objFree(result);
	inputFile = originalFile;
	printf("Done.\n");fflush(stdout);
	return e_nil;
}
__attribute__((aligned(16))) List* freadobj(List* a) {
	look = read_char();
	gettoken();
	return getobj();
}

__attribute__((aligned(16))) List* fwriteobj(List* a){
	fflush(stdout);
	List* current = a;
	while(notNil(current)){
		print_obj(car(current), 1);
		current = cdr(current);
	}
	fflush(stdout);
	puts("");
	return e_true;
}



void includeLinkedBinaryFile(unsigned char* start, unsigned int len){
	//Include gisp core
	linkedFile = start;
	linkedFileSize = len;
	printf("Including linked core.gisp size %d...", linkedFileSize);fflush(stdout);
	List* result = read_and_eval();
	objFree(result);
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
	extendEnv("null",   e_nil, global_env);
	extendEnv("list",    (void*)flist, global_env);
	extendEnv("vector",  (void*)fvec, global_env);
	extendEnv("to-vector",(void*)ftovec, global_env);
	extendEnv("hashmap", (void*)fhashmap, global_env);
	extendEnv("count",   (void*)fcount, global_env);
	extendEnv("assoc",   (void*)fassoc, global_env);
	extendEnv("/",       (void*)fdiv, global_env);
	extendEnv("*",       (void*)fmul, global_env);
	extendEnv("+",       (void*)fadd, global_env);
	extendEnv("-",       (void*)fsub, global_env);
	extendEnv("bit-and", (void*)fbitAnd, global_env);
	extendEnv("mod",     (void*)fmodulo, global_env);
	extendEnv("sign",    (void*)fsign, global_env);
	extendEnv("pow",     (void*)fpow, global_env);
	extendEnv("sqrt",    (void*)fsqrt, global_env);
	extendEnv("log",     (void*)flog, global_env);
	extendEnv("log10",   (void*)flog10, global_env);
	extendEnv("atan2",   (void*)fatan2, global_env);
	extendEnv("rand",    (void*)frand, global_env);
	extendEnv("gauss",   (void*)fgauss, global_env);
	extendEnv("seed",    (void*)fseed, global_env);
	extendEnv("=",       (void*)feq, global_env);
	extendEnv("<",       (void*)flessThan, global_env);
	extendEnv(">",       (void*)fgreaterThan, global_env);
	extendEnv("<=",      (void*)flessOrEqThan, global_env);
	extendEnv(">=",      (void*)fgreaterOrEqThan, global_env);
	extendEnv("abs",     (void*)fabsval, global_env);
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
	extendEnv("append",  (void*)fappend, global_env);
	extendEnv("insert",  (void*)finsert, global_env);
	extendEnv("first",   (void*)ffirst, global_env);
	extendEnv("last",    (void*)flast, global_env);
	extendEnv("make-surface",   (void*)fsvg_surface, global_env);
	extendEnv("make-context",   (void*)fsvg_context, global_env);
	extendEnv("surface-status", (void*)fsvg_status, global_env);
	extendEnv("surface-clean",  (void*)fsvg_clean, global_env);
	extendEnv("draw-line",      (void*)fsvg_line, global_env);
	extendEnv("draw-circle",    (void*)fsvg_circle, global_env);
	extendEnv("surface-to-png", (void*)fsvg_to_png, global_env);

	extendEnv("frames-make",   (void*)fframes_make, global_env);
	extendEnv("frames-set",    (void*)fframes_setActive, global_env);
	extendEnv("frames-to-png", (void*)fframes_save, global_env);
	extendEnv("frames-to-gif", (void*)fframes_save_gif, global_env);
	extendEnv("frames-clean",  (void*)fframes_clean, global_env);

	extendEnv("str",     (void*)fstr, global_env);
	extendEnv("include", (void*)fincludefile, global_env);
	extendEnv("read",    (void*)freadobj, global_env);
	extendEnv("write",   (void*)fwriteobj, global_env);
	extendEnv("nil?",    (void*)fnull, global_env);
	extendEnv("not-nil?",(void*)fnotnull, global_env);
	extendEnv("not",     (void*)fnot, global_env);
	extendEnv("and",     (void*)fand, global_env);
	extendEnv("or",      (void*)forr, global_env);
	extendEnv("symbol?", (void*)fatom, global_env);
	extendEnv("pair?",   (void*)fpair, global_env);
	extendEnv("number?", (void*)fnumber, global_env);
	extendEnv("eq?",     (void*)feq, global_env);
	extendEnv("empty?",  (void*)fempty, global_env);
	extendEnv("cons",    (void*)fcons, global_env);
	extendEnv("cdr",     (void*)fcdr, global_env);
	extendEnv("car",     (void*)fcar, global_env);
	extendEnv("get",     (void*)fget, global_env);
	extendEnv("take",    (void*)ftake, global_env);
	extendEnv("drop",    (void*)fdrop, global_env);

	extendEnv("point",                (void*)fpoint, global_env);
	extendEnv("point-x",              (void*)fpointx, global_env);
	extendEnv("point-y",              (void*)fpointy, global_env);
	extendEnv("point-print",          (void*)fprintPoint, global_env);
	extendEnv("vec",                  (void*)fpointvec, global_env);
	extendEnv("vec-len",              (void*)fveclen, global_env);
	extendEnv("vec-dir",              (void*)fvecdir, global_env);
	extendEnv("point-distance",       (void*)fpointDistance, global_env);
	extendEnv("point-angle",          (void*)fpointAngle, global_env);
	extendEnv("point-between",        (void*)fpointBetween, global_env);
	extendEnv("point-move-by-vector", (void*)fpointMoveByVector, global_env);
	extendEnv("draw-point",           (void*)fpointDraw, global_env);
	extendEnv("draw-points",          (void*)fpointsDraw, global_env);
	extendEnv("draw-lines",           (void*)flinesDraw, global_env);

	extendEnv("line",   (void*)fline, global_env);
	extendEnv("line-a", (void*)flineA, global_env);
	extendEnv("line-b", (void*)flineB, global_env);
	extendEnv("draw-path", (void*)fdrawPath, global_env);
	extendEnv("path-smooth", (void*)fpathSmooth, global_env);


	extendEnv("simplex-noise",(void*)fsimplex_noise, global_env);
	extendEnv("simplex-noise-value",(void*)fsimplex, global_env);
	extendEnv("mandelbrot-point",(void*)fmandelbrot, global_env);
	extendEnv("rescale",         (void*)frescale, global_env);

	extendEnv("printAddress",   (void*)fprintAddress, global_env);


	/**
	 * Language 3D
	 * */
	//Groups
	extendEnv("make-group3D",             (void*)gisp_make_group3D, global_env);
	extendEnv("add-to-group3D",           (void*)gisp_add_to_group3D, global_env);

	//Materials
	extendEnv("make-material-lambertian", (void*)gisp_make_material_lambertian, global_env);
	extendEnv("make-material-light",      (void*)gisp_make_material_light, global_env);
	extendEnv("make-material-metal",      (void*)gisp_make_material_metal, global_env);
	extendEnv("make-material-dielectric", (void*)gisp_make_material_dielectric, global_env);

	//Objects
	extendEnv("flip-face",                (void*)gisp_flip_face3D, global_env);
	extendEnv("rect3D",                   (void*)gisp_rect3D, global_env);
	extendEnv("box3D",                    (void*)gisp_box3D, global_env);
	extendEnv("sphere3D",                 (void*)gisp_sphere3D, global_env);
	extendEnv("rotated3D",                (void*)gisp_rotated3D, global_env);
	extendEnv("translated3D",             (void*)gisp_translated3D, global_env);

	//Rendering
	extendEnv("camera3D",                 (void*)gisp_camera3D, global_env);
	extendEnv("render3D",                 (void*)gisp_render3D, global_env);


	/**
	 * Interns
	 * */
	//Intern all the macro strings
	INTERN_quote	= intern("quote");
	INTERN_if		= intern("if");
	INTERN_cond		= intern("cond");
	INTERN_lambda	= intern("lambda");
	INTERN_apply	= intern("apply");
	INTERN_def		= intern("def");
	INTERN_defn		= intern("defn");
	INTERN_progn	= intern("progn");
	INTERN_let		= intern("let");
	INTERN_reduce	= intern("reduce");
	INTERN_filter	= intern("filter");
	INTERN_sort		= intern("sort");
	INTERN_map		= intern("map");
	INTERN_mapv		= intern("mapv");
	INTERN_doseq	= intern("doseq");
	INTERN_profile	= intern("profile");
	INTERN_nil		= intern("nil");

	printf("\n");
	

	//Include gisp core
	debugPrintInfo = 0;
	debugPrintFrees = 0;
	debugPrintCopy = 0;
	debugPrintAllocs = 0;
	includeLinkedBinaryFile((unsigned char*)src_gisp_core_core_gisp, (unsigned int)src_gisp_core_core_gisp_len);
	//includeLinkedBinaryFile((unsigned char*)src_gisp_core_simplex_noise_gisp, (unsigned int)src_gisp_core_simplex_noise_gisp_len);

	//Evaluate everything 
	debugPrintInfo = 0;
	debugPrintFrees = 0;
	debugPrintCopy = 0;
	debugPrintAllocs = 0;
	clock_t end_env = clock();
	List* result = read_and_eval();
	print_obj(result, 1);printf("\n");fflush(stdout);
	debugPrintInfo = 0;
	debugPrintFrees = 0;
	debugPrintCopy = 0;
	debugPrintAllocs = 0;
	objFree(result);


	environmentFree(global_env);

	//debug_printEnv(global_env);
	debug_printAllocations();


	//Print the profiling
	clock_t end = clock();
	if (profile){
		printf("\nProfiling:\n");
		time += (double)(end_env - begin) / CLOCKS_PER_SEC;
		printf("Time elpased for setup %f\n", time);
		time += (double)(end - end_env) / CLOCKS_PER_SEC;
		printf("Time elpased for eval %f", time);


		profile_print();
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



