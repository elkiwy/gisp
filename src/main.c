/* This lisp dialect used "micro lisp" from A. Carl Douglas as a base.*/
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>


#include "core.h"

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
		while(index < SYMBOL_MAX - 1 && look != EOF && !is_space(look) && !is_parens(look)) {
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

		//Check if it's a number
		char* err;
		strtod((char*)exp, &err);
		if (*err == 0){
		  //printf("returning number\n");
			return exp;			
		}

		//Else return it as an atom
		//printf("returning null\n");
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
		List* extenv = env,* names = second(car(exp)),* vars = cdr(exp);
		for (  ; names ; names = cdr(names), vars = cdr(vars) )
			extenv = cons(cons(car(names),  cons(eval(car(vars), env), 0)), extenv);
		return eval (third(car(exp)), extenv);
	}
	puts("cannot evaluate expression\n");
	return 0;
}


List* numberToList(float num){
	int numint = (int)num;
	float numfrac = num - numint;
	char* str = malloc(32*sizeof(char));
	if (numfrac == 0){sprintf(str, "%i", numint);
	}else{            sprintf(str, "%f", num);}
	return (List*)str;
}

float numberOperation(char op, float a, float b){
  if (op=='+'){return a+b;}
  else if (op=='-'){return a-b;}
  else if (op=='*'){return a*b;}
  else if (op=='/'){return a/b;}
  else{return 0;}
}

List* applyOperationOnList(char op, List* list){
	printf("Applying operation %i on list\n", op);
	float res = atof((char*)first(list));
	List* current = list;
	while((current = cdr(current))){
	  res = numberOperation(op, res, atof((char*)car(current)));
	}
	return numberToList(res);
}

//Define function to be used in Gisp
List* fcons(List* a)    {  return cons(first(a), second(a));  }
List* fcar(List* a)     {  return car(first(a));  }
List* fcdr(List* a)     {  return cdr(first(a));  }
List* feq(List* a)      {  return first(a) == second(a) ? e_true : e_false;  }
List* fpair(List* a)    {  return is_pair(first(a))       ? e_true : e_false;  }
List* fatom(List* a)    {  return is_atom(first(a))       ? e_true : e_false;  }
List* fnull(List* a)    {  return first(a) == 0           ? e_true : e_false; }
List* freadobj(List* a) {  look = getchar(); gettoken(); return getobj();  }
List* fwriteobj(List* a){  print_obj(car(a), 1); puts(""); return e_true;  }

List* flist(List* a) {return a;}
List* fadd(List* a) {return applyOperationOnList('+', a);}
List* fsub(List* a) {return applyOperationOnList('-', a);}
List* fmul(List* a) {return applyOperationOnList('*', a);}
List* fdiv(List* a) {return applyOperationOnList('/', a);}



//Main program entry
int main(int argc, char* argv[]) {
  List* env = cons(cons(intern("car"), cons((void* )fcar, 0)),
              cons(cons(intern("cdr"), cons((void* )fcdr, 0)),
              cons(cons(intern("cons"), cons((void* )fcons, 0)),
              cons(cons(intern("eq?"), cons((void* )feq, 0)),
              cons(cons(intern("pair?"), cons((void* )fpair, 0)),
              cons(cons(intern("symbol?"), cons((void* )fatom, 0)),
              cons(cons(intern("null?"), cons((void* )fnull, 0)),
              cons(cons(intern("read"), cons((void* )freadobj, 0)),
              cons(cons(intern("write"), cons((void* )fwriteobj, 0)),
			  cons(cons(intern("+"), cons((void* )fadd, 0)),
			  cons(cons(intern("-"), cons((void* )fsub, 0)),
			  cons(cons(intern("*"), cons((void* )fmul, 0)),
			  cons(cons(intern("/"), cons((void* )fdiv, 0)),
              cons(cons(intern("list"), cons((void* )flist, 0)),
			  cons(cons(intern("null"), cons(0,0)), 0)))))))))))))));

  look = getchar();
  gettoken();
  List* result = eval(getobj(), env);
  print_obj( result, 1 );

  printf("\n");
  printf("\n");
  return 0;
}
