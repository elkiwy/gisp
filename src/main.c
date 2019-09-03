/* This lisp dialect used "micro lisp" from A. Carl Douglas as a base.*/
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

//Handy debug method
#define debug(m,e) printf("%s:%d: %s:",__FILE__,__LINE__,m); print_obj(e,1); puts("");

//This is the main List structure
typedef struct List {
	struct List*  next;
	void*  data;
} List;


//Global list of symbols
List* symbols = 0;


//Static variables used for input parsing
static int look; 
#define SYMBOL_MAX  32
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


//Each cons shell is tagged with the lowest pointer bit set to 1, everything else is set to 0
//Before accessing cons car and cdr we need to untag the pointer to read from memory correctly
#define is_pair(x) (((uintptr_t)x & 0x1) == 0x1)
#define is_atom(x) (((uintptr_t)x & 0x1) == 0x0)
#define untag(x)   ((uintptr_t) x & ~0x1)
#define tag(x)     ((uintptr_t) x | 0x1)
#define car(x)     (((List*)untag(x))->data)
#define cdr(x)     (((List*)untag(x))->next)

//Handy list shortcuts
#define first(x)   (car(x))
#define second(x)  (car(cdr(x)))
#define third(x)   (car(cdr(cdr(x))))
#define fourth(x)  (car(cdr(cdr(cdr(x)))))
#define fifth(x)   (car(cdr(cdr(cdr(cdr(x))))))

//Define what is true and what is false
#define e_true     cons( intern("quote"), cons( intern("t"), 0))
#define e_false    0

//Cons shell
List* cons(void* _car, void* _cdr) {
	List* _pair = calloc( 1, sizeof (List) );
	_pair->data = _car;
	_pair->next = _cdr;
	return (List*) tag(_pair);
}

//Symbols interning
void* intern(char* sym) {
	List* _pair = symbols;
	for ( ; _pair ; _pair = cdr(_pair))
		if (strncmp(sym, (char*) car(_pair), 32)==0) return car(_pair);
	symbols = cons(strdup(sym), symbols);
	return car(symbols);
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



// (list 1 2 3 4)
// (cons 1 (cons 2 (cons 3 (cons 4 0))))




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

//List* flist(List* a) {for (List* current = a; current!=0; current=cdr(current)) {}}


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
		//Check if is a known symbol
		for ( ; env != 0; env = cdr(env) ){
			if (exp == car(car(env)))  return car(cdr(car(env)));}

		//Check if it's a number
		char* err;
		strtod((char*)exp, &err);
		if (*err == 0){
			printf("returning number\n");
			return exp;			
		}

		//Else return it as an atom
		printf("returning null\n");
		return 0;
	//Else if is a list with the first atom being an atom
	} else if (is_atom(first(exp))) { 
	    printf("list with atom found %s\n", (char*) first(exp));
		// (quote X)
	    if (first(exp) == intern("quote")) {
			//Return the quoted element as it is
			printf("returning quoted\n");
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
			List* primop = eval(first(exp), env);
            //user defined lambda, arg list eval happens in binding  below
			if (is_pair(primop)) { 
				return eval( cons(primop, cdr(exp)), env );
			//Built-in primitive
			} else if (primop) { 
				List* result = ((List* (*) (List*))primop) (evlist(cdr(exp), env));
				printf("evaluated %s\n", (char*)first(exp));
				return result;
			}
		}
	// ((lambda (params) body) args)
	} else if (car(car(exp)) == intern("lambda")) {
		printf("lambda found\n");
	    //bind names into env and eval body
		List* extenv = env,* names = second(car(exp)),* vars = cdr(exp);
		for (  ; names ; names = cdr(names), vars = cdr(vars) )
			extenv = cons(cons(car(names),  cons(eval(car(vars), env), 0)), extenv);
		return eval (third(car(exp)), extenv);
	}
	puts("cannot evaluate expression");
	return 0;
}



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
              cons(cons(intern("null"), cons(0,0)), 0))))))))));
  look = getchar();
  gettoken();

  List* result = eval(getobj(), env);
  print_obj( result, 1 );
  printf("\n");
  printf("\n");
  return 0;
}
