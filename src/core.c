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
		Vector* vec = (void*)untag_vector(ob);
		void** data = vec->data;
		int size = vec->size;

		printf("[ ");
		for(int i = 0; i<size; i++){
			print_obj(data[i], 0); printf(" ");
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
	return (rand() % (max-min)) + min;
}

// ------------------------------------------------------------------
//Lisp core
map_t symbols_hashmap = 0;
void* intern(char* sym) {
	if (symbols_hashmap == 0)
		symbols_hashmap = hashmap_new();
	
	void* value = 0;
	hashmap_get(symbols_hashmap, sym, (any_t)&value);
	if(value){
		return value;
	}else{
		void* newVal = strdup(sym);
		hashmap_put(symbols_hashmap, newVal, newVal);
		return newVal;
	}
}

List* cons(void* _car, void* _cdr) {
	consCount++;
	List* _pair = malloc( sizeof(List) );
	_pair->data = _car;
	_pair->next = _cdr;
	return (List*) tag(_pair);
}

Environment* makeEnvironment(Environment* _outer) {
	Environment* env = malloc( sizeof(Environment) );
	env->hashData = hashmap_new();
	env->outer = (void*)_outer;
	return env;
}

//Add a new function to an environment
void extendEnv(char* name, void* value, Environment* env){
	environmentCounter_insert++;
	//Add this symbol and value to the current environment level
	hashmap_put(env->hashData, name, value);
}

void* searchInEnvironment(List* exp, Environment* env){
	environmentCounter_search++;
	clock_t start = clock();

	Environment* currentEnv = env;
	while(currentEnv){
		//Search in the hash
		void* value = 0;
		hashmap_get(currentEnv->hashData, (char*)exp, (any_t)&value);
		if (value){
			clock_t end = clock(); environmentCounter_searchTimeSum += (double)(end - start) / CLOCKS_PER_SEC;
			return value;
		}

		//Search it into the outer level
		currentEnv = currentEnv->outer;
	}

	clock_t end = clock(); environmentCounter_searchTimeSum += (double)(end - start) / CLOCKS_PER_SEC;
	return 0;
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

Vector* newVec(int _size){
	vectorCount++;
	Vector* _vec = malloc(sizeof(Vector));
	_vec->data = calloc(_size + 1, sizeof(void*));
	_vec->size = _size;
	return (Vector*)_vec;
}

Vector* listToVec(List* l){
	//Count elements
	int n = 0;
	List* items = l;
	while(items){n++; items = cdr(items);}

	//Create the vector
	Vector* vec = newVec(n);
	void** data = vec->data;
	List* current = l;
	int i = 0;
	while(current){
		data[i] = first(current);	
		current = cdr(current);
		i++;
	}
	data[n] = 0;
	return vec;
}

Vector* copyVec(Vector* v){
	int s = v->size;
	Vector* new = newVec(s);
	void** data = new->data;
	memcpy(new->data, v->data, s*8);
	data[s] = 0;
	return new;
}

List* vecToList(Vector* vec){
	int size = vec->size;
	void** data = vec->data;
	List* l = 0;
	for(int i=size-1; i>=0; i--){
		l = cons(data[i], l);
	}
	return l;
}



