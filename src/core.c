#include "core.h"

//Cons shell
List* cons(void* _car, void* _cdr) {
	List* _pair = calloc( 1, sizeof (List) );
	_pair->data = _car;
	_pair->next = _cdr;
	return (List*) tag(_pair);
}

//Symbols interning
List* symbols = 0;
void* intern(char* sym) {
	List* _pair = symbols;
	for ( ; _pair ; _pair = cdr(_pair)){
		if (strncmp(sym, (char*) car(_pair), SYMBOL_MAX)==0){
		  return (void*)car(_pair);
		}
	}
	symbols = cons(strdup(sym), symbols);
	return (void*)car(symbols);
}



double* symbol_to_number(char* sym){
	double* ptr = malloc(sizeof(double));
	*ptr = strtod(sym, NULL);
	return (double*)tag_number(ptr);
}

double* value_to_number(double value){
	double* ptr = malloc(sizeof(double));
	*ptr = value;
	return (double*)tag_number(ptr);
}

double numVal(double* number){
	return *((double*)untag_number(number));
}
