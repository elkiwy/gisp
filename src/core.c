#include "core.h"



//Prints a List object
void print_obj(List* ob, int head_of_list) {
	if(is_hashmap(ob)){
		map_t map = (map_t)untag_hashmap(ob);
		int size = hashmap_length(map);
		char** keys = malloc(sizeof(void*)*size);
		void** values = malloc(sizeof(void*)*size);
		hashmap_keys_and_values(map, keys, values);

		printf("{ ");
		for (int i=0; i<size; i++){
			printf("%s ", (char*)keys[i]);
			print_obj((List*)values[i], 0);
			printf(" ");
		}
		printf("}");
		free(keys);
		free(values);

	}else if(is_vector(ob)){
		void** vec = (void*)untag_vector(ob);
		int i = 0;
		printf("[ ");
		while(vec[i]!=0){
			print_obj(vec[i], 0); printf(" ");
			i++;
		}
		printf("]");

	}else if(is_number(ob)){
		double num = numVal((double*)ob);
		if ((num - (int)num) == 0){printf("%i", (int)num);
		}else{printf("%f", num);}

	} else if (is_pair(ob)){
		if (head_of_list) printf("(");
		print_obj(car(ob), 1);
		if (cdr(ob) != 0) {
			printf(" ");
			print_obj(cdr(ob), 0); //Change to 1 to enable the cons cells view
		}
		if (head_of_list) printf(")");

	}else{
		printf("%s", ob ? (char*) ob : "null" );
	}
}








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


