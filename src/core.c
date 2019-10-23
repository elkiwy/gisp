#include "core.h"

// ------------------------------------------------------------------
//Generic utilities
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
		double num = numVal(ob);
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

int randInt(int min, int max){
	int range = max-min;
	return (rand() % range) + min;
}

// ------------------------------------------------------------------
//Lisp core
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

List* cons(void* _car, void* _cdr) {
	consCount++;
	List* _pair = calloc( 1, sizeof (List) );
	_pair->data = _car;
	_pair->next = _cdr;
	return (List*) tag(_pair);
}

Environment* makeEnvironment(List* _data, Environment* _outer) {
	Environment* env = calloc( 1, sizeof (Environment) );
	env->data = _data;
	env->outer = (void*)_outer;
	return env;
}

// ------------------------------------------------------------------
//Number utilities
double* newNumber(){
	numberCount++;
	double* ptr = malloc(sizeof(double));
	return ptr;
}

double* symbol_to_number(char* sym){
	double* ptr = newNumber();
	*ptr = strtod(sym, NULL);
	return (double*)tag_number(ptr);}
double* value_to_number(double value){
	double* ptr = newNumber();
	*ptr = value;
	return (double*)tag_number(ptr);}
double numVal(List* tagged_number){
	return *((double*)untag_number(tagged_number));
}

// ------------------------------------------------------------------
//String utilities
char* trim_quotes(char* s){
	if (s[0] == '"'){s++;}
	if (s[strlen(s)-1] == '"'){
		s[strlen(s)-1] = '\0';
	}
	return s;
}


// ------------------------------------------------------------------
//Hashmap utilites
map_t newHashmap(){
	hashmapCount++;
	map_t map = hashmap_new();
	return map;
}


// ------------------------------------------------------------------
//Vector utilities

void** newVec(int size){
	vectorCount++;
	void** vec = malloc(sizeof(void*) * size);
	return vec;
}

void** listToVec(List* l){
	//Count elements
	int n = 0;
	List* items = l;
	while(items){n++; items = cdr(items);}

	//Create the vector
	void** vec = newVec(n+1);
	List* current = l;
	int i = 0;
	while(current){
		vec[i] = first(current);	
		current = cdr(current);
		i++;
	}
	vec[n] = 0;
	return vec;
}

int vecLength(void** v){
	int n = 0;
	while(v[n]){
		n++;
	}
	return n;
}

void** copyVec(void** v){
	int size = vecLength(v);
	void** new = newVec(size+1);
	for(int i = 0; i<size; i++){
		new[i] = v[i];
	}
	new[size] = 0;
	return new;
}

List* vecToList(void** vec){
	int size = vecLength(vec);
	List* l = 0;
	for(int i=size-1; i>=0; i--){
		l = cons(vec[i], l);
	}
	return l;
}



