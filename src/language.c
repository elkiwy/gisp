#include "language.h"


// --------------------------------------------
// Math operations

///Convert a float number into a numeric atomic value (untagged List ptr)
List* numberToList(double num){
	int numint = (int)num;
	double numfrac = num - numint;
	char* str = malloc(32*sizeof(char));
	if (numfrac == 0){sprintf(str, "%i", numint);
	}else{            sprintf(str, "%f", num);}
	return (List*)str;
}

///Apply an operation between two floats
double numberOperation(char op, double a, double b){
	if (op=='+'){return a+b;}
	else if (op=='-'){return a-b;}
	else if (op=='*'){return a*b;}
	else if (op=='/'){return a/b;}
	else{return 0;}
}

///Apply an operation to all the element of the list
List* applyOperationOnList(char op, List* list){
	double res = numVal((double*)first(list));
	List* current = list;
	while((current = cdr(current))){
		res = numberOperation(op, res, numVal((double*)car(current)));
	}
	//return numberToList(res);
	return (List*)value_to_number(res);
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



// ---------------------------------------------
// Strings
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
