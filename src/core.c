#include "core.h"

// ------------------------------------------------------------------
//Generic utilities
char* objToString(List* ob, int head_of_list){
	int bufferSize = 1024*1024; 
	char* buffer = malloc(sizeof(char)*bufferSize);
	buffer[0] = '\0';
	char* target = buffer;
	//printf("\e[100m%p\e[49m", ob);fflush(stdout);
	if(is_string(ob)){
		char* untagged = (char*)untag_string(ob);
		target += sprintf(target, "\"%s\"", untagged);

	}else if(is_hashmap(ob)){
		//printf("printing hashmap\n");fflush(stdout);
		map_t map = (map_t)untag_hashmap(ob);
		int size = hashmap_length(map);
		char** keys = malloc(sizeof(void*)*size);
		void** values = malloc(sizeof(void*)*size);
		hashmap_keys_and_values(map, keys, values);

		target += sprintf(target, "{ ");
		for (int i=0; i<size; i++){
			target += sprintf(target, "%s ", (char*)keys[i]);
			char* str = objToString((List*)values[i], 0);
			target += sprintf(target, "%s", str);
			free(str);
			target += sprintf(target, " ");
		}
		target += sprintf(target, "}");
		free(keys);
		free(values);

	}else if(is_vector(ob)){
		//printf("printing vector\n");fflush(stdout);
		Vector* vec = (void*)untag_vector(ob);
		void** data = vec->data;
		int size = vec->size;

		target += sprintf(target, "[");
		for(int i = 0; i<size; i++){
			char* str = objToString(data[i], 0);
			target += sprintf(target, "%s", str);
			free(str);
			if(i<size-1){target += sprintf(target, " ");}
		}
		target += sprintf(target, "]");

	}else if(is_number(ob)){
		//printf("printing number %p\n", (void*)ob);fflush(stdout);
		double num = numVal(ob);
		if ((num - (int)num) == 0){
			target += sprintf(target, "%i", (int)num);
		}else{
			target += sprintf(target, "%f", num);
		}

	} else if (is_pair(ob)){
		//printf("printing pair\n");fflush(stdout);
		if (head_of_list){
			target += sprintf(target, "(");
		}
		char* str = objToString(car(ob), 1);
		target += sprintf(target, "%s", str);
		free(str);
		if (cdr(ob) != 0) {
			target += sprintf(target, " ");
			char* str2 = objToString(cdr(ob), 0);
			target += sprintf(target, "%s", str2); //Change to 1 to enable the cons cells view
			free(str2);
		}
		if (head_of_list){
			target += sprintf(target, ")");
		}

	}else{
		//printf("printing atom\n");fflush(stdout);
		target += sprintf(target, "%s", ob ? (char*) ob : "null" );
	}
	return buffer;
}

void print_obj(List* ob, int head_of_list) {
	//printf("\e[100m%p\e[49m", ob);fflush(stdout);
	char* str = objToString(ob, head_of_list);
	printf("%s\n", str);
	free(str);
	//if(is_string(ob)){
	//	char* untagged = (char*)untag_string(ob);
	//	printf("\"%s\"", untagged);

	//}else if(is_hashmap(ob)){
	//	//printf("printing hashmap\n");fflush(stdout);
	//	map_t map = (map_t)untag_hashmap(ob);
	//	int size = hashmap_length(map);
	//	char** keys = malloc(sizeof(void*)*size);
	//	void** values = malloc(sizeof(void*)*size);
	//	hashmap_keys_and_values(map, keys, values);

	//	printf("{ ");fflush(stdout);
	//	for (int i=0; i<size; i++){
	//		printf("%s ", (char*)keys[i]);fflush(stdout);
	//		print_obj((List*)values[i], 0);
	//		printf(" ");fflush(stdout);
	//	}
	//	printf("}");fflush(stdout);
	//	free(keys);
	//	free(values);

	//}else if(is_vector(ob)){
	//	//printf("printing vector\n");fflush(stdout);
	//	Vector* vec = (void*)untag_vector(ob);
	//	void** data = vec->data;
	//	int size = vec->size;

	//	printf("[ ");fflush(stdout);
	//	for(int i = 0; i<size; i++){
	//		print_obj(data[i], 0); printf(" ");
	//	}
	//	printf("]");fflush(stdout);

	//}else if(is_number(ob)){
	//	//printf("printing number %p\n", (void*)ob);fflush(stdout);
	//	double num = numVal(ob);
	//	if ((num - (int)num) == 0){printf("%i", (int)num);fflush(stdout);
	//	}else{printf("%f", num);fflush(stdout);}

	//} else if (is_pair(ob)){
	//	//printf("printing pair\n");fflush(stdout);
	//	if (head_of_list) printf("(");fflush(stdout);
	//	print_obj(car(ob), 1);
	//	if (cdr(ob) != 0) {
	//		printf(" ");fflush(stdout);
	//		print_obj(cdr(ob), 0); //Change to 1 to enable the cons cells view
	//	}
	//	if (head_of_list) printf(")");fflush(stdout);

	//}else{
	//	printf("%s", ob ? (char*) ob : "null" );fflush(stdout);
	//}
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

	if (debugPrintAllocs){
		printf("\e[31m+++ cons %p", (void*)tag(_pair));
		printf(" (%p ", _car);
		printf(". %p) ", _cdr);
		debugPrintObj("", (void*)tag(_pair));
		printf("\e[39m");
		debug_addAllocation((void*)tag(_pair));
	}

	return (List*) tag(_pair);
}

Environment* makeEnvironment(Environment* _outer) {
	Environment* env = malloc( sizeof(Environment) );
	env->hashData = hashmap_new();
	env->outer = (void*)_outer;
	if (debugPrintAllocs){printf("\e[31m+++ environemnt %p\n\e[39m", env);}
	return env;
}

//Add a new function to an environment
void extendEnv(char* name, void* value, Environment* env){
	environmentCounter_insert++;

	//Add this symbol and value to the current environment level
	char* cellKey;
	void* cellData;
	int cellIndex;
	int found = hashmap_get_cell_data(env->hashData, name, &cellKey, &cellData, &cellIndex);
	if(found){
		objFree(cellData);
		void* hashmap_obj = objCopy(value);
		if(debugPrintInfo){printf("replacing \"%s\" (%p) with %p (original: %p)\n", cellKey, cellKey, hashmap_obj, value);fflush(stdout);}
		hashmap_put(env->hashData, cellKey, hashmap_obj);
		
	}else{
		char* hashmap_key = strdup(name);
		void* hashmap_obj = objCopy(value);
		if(debugPrintInfo){printf("adding \"%s\" (%p) as %p (original: %p)\n", hashmap_key, hashmap_key, hashmap_obj, value);fflush(stdout);}
		hashmap_put(env->hashData, hashmap_key, hashmap_obj);
	}
}

void* searchInEnvironment(List* name, Environment* env){
	if(debugPrintInfo){printf("==> Search in environment \"%s\"\n", (char*)name);fflush(stdout);}
	
	environmentCounter_search++;
	clock_t start = clock();

	Environment* currentEnv = env;
	while(currentEnv){
		//Search in the hash
		void* value = 0;
		hashmap_get(currentEnv->hashData, (char*)name, (any_t)&value);
		if (value){
			clock_t end = clock(); environmentCounter_searchTimeSum += (double)(end - start) / CLOCKS_PER_SEC;
			List* copy = objCopy(value);

			if (debugPrintInfo){
				printf("==> Found, returning copy %p of value:%p ", copy, value);fflush(stdout);
				debugPrintObj("", value);
			}
			return copy;
		}

		//Search it into the outer level
		currentEnv = currentEnv->outer;
	}

	clock_t end = clock(); environmentCounter_searchTimeSum += (double)(end - start) / CLOCKS_PER_SEC;
	//printf("++not found value:\"\"\n");fflush(stdout);
	return 0;
}


// ------------------------------------------------------------------
// String utilities
char* newStringFromText(char* text){
	char* ptr = strdup(text);
	if (ptr[0]=='"'){ptr=trim_quotes(ptr);}
	return ptr;
}

char* newStringFromSize(int n){
	char* ptr = malloc(sizeof(char)*(n+1));
	return ptr;
}

// ------------------------------------------------------------------
//Number utilities
double* newNumber(){
	numberCount++;
	double* ptr = malloc(sizeof(double));
	if(debugPrintAllocs){
		printf("\e[31m+++ number %p\n\e[39m", (void*)tag_number(ptr));fflush(stdout);
		debug_addAllocation((void*)tag_number(ptr));
	}

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
	if (s[0] == '"'){
		for(uint i=0;i<strlen(s)-2;++i){
			s[i] = s[i+1];
		}
		s[strlen(s)-2] = '\0';
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
	if(debugPrintAllocs){
		printf("\e[31m+++ vector %p\n\e[39m", (void*)tag_vector(_vec));fflush(stdout);
		debug_addAllocation((void*)tag_vector(_vec));
	}
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
		data[i] = objCopy(first(current));
		current = cdr(current);
		i++;
	}
	data[n] = 0;
	return vec;
}


List* vecToList(Vector* vec){
	int size = vec->size;
	void** data = vec->data;
	List* l = 0;
	for(int i=size-1; i>=0; i--){
		l = cons(objCopy(data[i]), l);
	}
	return l;
}





List* listGetLastCons(List* l){
	List* current = l;
	while(cdr(current)){
		current = cdr(current);
	}
	return current;
}



void consSetNext(List* l, List* _next){
	List* untagged = (List*)untag(l);
	untagged->next = _next;
}

void consSetData(List* l, void* _data){
	List* untagged = (List*)untag(l);
	untagged->data = _data;
}









void debugPrintObj(char* pre, List* obj){
	printf("%s", pre);print_obj(obj, 1);printf("\n");fflush(stdout);
}












void objFree(List* obj){
	if(is_string(obj)){
		if(debugPrintFrees){printf("\e[31m--- Freeing string: %p ", (void*)obj);fflush(stdout); print_obj(obj, 1); printf("\n\e[39m");fflush(stdout);}
		stringFree(obj);

	}else if(is_hashmap(obj)){
		if(debugPrintFrees){printf("\e[31m--- Freeing hashmap: %p ", (void*)obj);fflush(stdout); print_obj(obj, 1); printf("\n\e[39m");fflush(stdout);}
		hashmapFree(obj);

	}else if (is_vector(obj)){
		if(debugPrintFrees){printf("\e[31m--- Freeing vector: %p ", (void*)obj);fflush(stdout); print_obj(obj, 1); printf("\n\e[39m");fflush(stdout);}
		vectorFree(obj);

	}else if(is_number(obj)){
		if(debugPrintFrees){printf("\e[31m--- Freeing number: %p\n\e[39m", (void*)obj);fflush(stdout);}
		numberFree(obj);	

	}else if (is_pair(obj)){
		if(debugPrintFrees){printf("\e[31m--- Freeing cons: %p", (void*)obj);fflush(stdout); printf(" (next: %p)\n\e[39m", (void*)cdr(obj));fflush(stdout);}
		listFree(obj);
	}
}

void stringFree(List* string){
	if(debugPrintFrees){debug_removeAllocation(string);}
	free((char*)untag_string(string));	
}

void numberFree(List* number){
	if(debugPrintFrees){debug_removeAllocation(number);}
	free((double*)untag_number(number));	
}

void listFree(List* l){
	//If this cons has a cdr free that first
	if(cdr(l)){objFree(cdr(l));}//else{printf("!!Stopping\n");fflush(stdout);}

	//Free this cons data
	void* data = (void*)car(l);
	//printf("Freeing internal data: %p\n", data);fflush(stdout);
	objFree(data);

	//Free this cons
	if(debugPrintFrees){debug_removeAllocation(l);}
	free((List*)untag(l));
}

void listFreeOnlyCons(List* l){
	if(l==NULL)return;
	if(cdr(l)){listFreeOnlyCons(cdr(l));}
	consFree(l);
}


void consFree(List* c){
	if(debugPrintFrees){
		printf("\e[31m--- Freeing single cons: %p\n\e[39m", c);
		debug_removeAllocation(c);
	}
	free((List*)untag(c));
}



void hashmapFree(List* hashmap){
	map_t m = (map_t)untag_hashmap(hashmap);
	int size = hashmap_length(m);
	if(size>0){
		char* keys[size];
		void* data[size];
		hashmap_keys_and_values(m, keys, data);
		for (int i=0; i<size; ++i){
			free(keys[i]);
			objFree(data[i]);	
		}
	}
	hashmap_free(m);
}

void vectorFree(List* v){
	if(debugPrintFrees){
		printf("\e[31m--- Freeing vector : %p\n\e[39m", v);
		debug_removeAllocation(v);
	}

	Vector* vec = (Vector*)untag_vector(v);
	int s = vec->size;
	void** data = vec->data;
	for(int i=0;i<s;++i){
		objFree(data[i]);
	}
	free(vec->data);
	free(vec);
}

void environmentFree(Environment* env){
	if(debugPrintFrees){printf("\e[31m--- Freeing environemtn : %p\n\e[39m", env);}
	map_t m = env->hashData;
	int size = hashmap_length(m);
	if(size>0){
		char* keys[size];
		void* data[size];
		hashmap_keys_and_values(m, keys, data);
		for (int i=0; i<size; ++i){
			//printf("Freeing key %p size:%d\n", keys[i], size);
			free(keys[i]);
			objFree(data[i]);	
		}
	}
	hashmap_free(m);
	free(env);
}







List* objCopy(List* obj){
	if(debugPrintCopy){char* str = objToString(obj, 1); printf("\e[35m--- Copying obj: %p value: %s\n\e[39m", obj, str); free(str);}
	if(is_string(obj)){return stringCopy(obj);}
	if(is_hashmap(obj)){return hashmapCopy(obj);}
	if(is_vector(obj)){return vectorCopy(obj);}
	if(is_number(obj)){return numberCopy(obj);}
	if(is_pair(obj)){return listCopy(obj);}
	return obj;
}

List* stringCopy(List* s){
	char* untagged = (char*)untag_string(s);
	char* new = strdup(untagged);
	return (List*)tag_string(new);
}

List* vectorCopy(List* v){
	if(debugPrintCopy){printf("\e[35m--- Copying vector: %p\n\e[39m", v);}
	Vector* old = (Vector*)untag_vector(v);
	int s = old->size;
	Vector* new = newVec(s);
	void** data = new->data;
	void** oldd = old->data;
	for(int i=0;i<s;++i){
		data[i] = objCopy(oldd[i]);
	}
	data[s] = 0;
	if(debugPrintCopy){printf("\e[35m===> Copied vector %p", new); debugPrintObj(" : \e[39m", (List*)tag_vector(new));}
	return (List*)tag_vector(new);
}

List* hashmapCopy(List* hashmap){
	if(debugPrintCopy){printf("\e[35m--- Copying hashmap: %p\n\e[39m", hashmap);}

	//Extract old hashmap information
	map_t old = (map_t)untag_hashmap(hashmap);
	int size = hashmap_length(old);
	char* keys[size];
	void* data[size];
	hashmap_keys_and_values(old, keys, data);

	//Create and fill the new one
	map_t new = newHashmap();
	for(int i=0;i<size;++i){
		char* key = strdup(keys[i]);
		List* val = objCopy(data[i]);
		hashmap_put(new, key, val);
	}

	if(debugPrintCopy){printf("\e[35m===> Copied hashmap %p", new); debugPrintObj(" : \e[39m", (List*)tag_hashmap(new));}
	return (List*)tag_hashmap(new);
}


List* numberCopy(List* num){
	double* n = (double*)untag_number(num);	
	double* newN = newNumber();
	*newN = *n;
	return (List*)tag_number(newN);
}


List* listCopyOnlyCons(List* l){
	if(l==NULL){return 0;}
	List* current = l;
	List* newFirst = cons(0,0);
	List* last = newFirst;
	while(current){
		//Copy current data and create a new next
		List* untagged = (List*)untag(last);
		untagged->data = car(current);
		if(cdr(current)){untagged->next = cons(0, 0);}

		//Save current last one and iterate
		last = cdr(last);
		current = cdr(current);
	}
	//printf("\e[35m===> Copied list %p", newFirst);
	//debugPrintObj(" : \e[39m", newFirst);
	return newFirst;
}



List* listCopy(List* l){
	if(l==NULL){return 0;}
	List* current = l;
	List* newFirst = cons(0,0);
	List* last = newFirst;
	while(current){
		//Copy current data and create a new next
		List* untagged = (List*)untag(last);
		untagged->data = objCopy(car(current));
		if(cdr(current)){untagged->next = cons(0, 0);}

		if(debugPrintCopy){printf("+++ listCopy %p", last); printf(" (%p ", car(last)); printf(". %p) ", cdr(last)); debugPrintObj("", last);}

		//Save current last one and iterate
		last = cdr(last);
		current = cdr(current);
	}
	if(debugPrintCopy){printf("\e[35m===> Copied list %p", newFirst); debugPrintObj(" : ", newFirst); List* c = newFirst; while(c){printf("%p, ", c);c=cdr(c);} printf("\n\e[39m");}
	return newFirst;
}








void debug_addAllocation(void* p){
	for(int i=0;i<1024*1024; i++){
		if (allocations[i]==NULL){
			allocations[i] = p;
			return;
		}
	}
}

void debug_removeAllocation(void* p){
	for(int i=0;i<1024*1024; i++){
		if (allocations[i]==p){
			allocations[i] = NULL;	
			return;
		}
	}
}

void debug_printAllocations(){
	if(debugPrintAllocs){
		printf("\n\n!!!!Allocations:\n");
		for(int i=0;i<1024*1024; i++){
			if (allocations[i]!=NULL){
				printf("  ==> Allocations[%d] => %p\n", i, allocations[i]);
			}
		}
	}
}
