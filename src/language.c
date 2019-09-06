#include "language.h"


// --------------------------------------------
// Math operations

///Convert a float number into a numeric atomic value (untagged List ptr)
List* numberToList(float num){
	int numint = (int)num;
	float numfrac = num - numint;
	char* str = malloc(32*sizeof(char));
	if (numfrac == 0){sprintf(str, "%i", numint);
	}else{            sprintf(str, "%f", num);}
	return (List*)str;
}

///Apply an operation between two floats
float numberOperation(char op, float a, float b){
  if (op=='+'){return a+b;}
  else if (op=='-'){return a-b;}
  else if (op=='*'){return a*b;}
  else if (op=='/'){return a/b;}
  else{return 0;}
}

///Apply an operation to all the element of the list
List* applyOperationOnList(char op, List* list){
	float res = atof((char*)first(list));
	List* current = list;
	while((current = cdr(current))){
	  res = numberOperation(op, res, atof((char*)car(current)));
	}
	return numberToList(res);
}

//Compose the callable functions
List* fadd(List* a) {return applyOperationOnList('+', a);}
List* fsub(List* a) {return applyOperationOnList('-', a);}
List* fmul(List* a) {return applyOperationOnList('*', a);}
List* fdiv(List* a) {return applyOperationOnList('/', a);}



// ---------------------------------------------
// List operations
List* fcons(List* a) { return cons(first(a), second(a)); }
List* flist(List* a) { return a;}
List* fcar(List* a)  { return car(first(a)); }
List* fcdr(List* a)  { return cdr(first(a)); }



// ---------------------------------------------
// Test and type checking
List* feq(List* a)   { return first(a) == second(a) ? e_true : e_false; }
List* fpair(List* a) { return is_pair(first(a))     ? e_true : e_false; }
List* fatom(List* a) { return is_atom(first(a))     ? e_true : e_false; }
List* fnull(List* a) { return first(a) == 0         ? e_true : e_false; }



