#include "language.h"


// --------------------------------------------
// Math operations

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
	double res = numVal(first(list));
	List* current = list;
	while((current = cdr(current))){
		res = numberOperation(op, res, numVal(car(current)));
	}
	return (List*)value_to_number(res);
}

List* fadd(List* a) {return applyOperationOnList('+', a);}
List* fsub(List* a) {return applyOperationOnList('-', a);}
List* fmul(List* a) {return applyOperationOnList('*', a);}
List* fdiv(List* a) {return applyOperationOnList('/', a);}

List* fsin(List* a) {return (List*)value_to_number(sin(numVal(first(a))));}
List* fcos(List* a) {return (List*)value_to_number(cos(numVal(first(a))));}
List* fdsin(List* a) {return (List*)value_to_number(sin(numVal(first(a)) * DEG_TO_RAD));}
List* fdcos(List* a) {return (List*)value_to_number(cos(numVal(first(a)) * DEG_TO_RAD));}

List* frad(List* a) {return (List*)value_to_number(numVal(first(a)) * DEG_TO_RAD);}
List* fdeg(List* a) {return (List*)value_to_number(numVal(first(a)) * RAD_TO_DEG);}
List* fint(List* a)   {return (List*)value_to_number((int)numVal(first(a)));}
List* ffloor(List* a) {return (List*)value_to_number((int)numVal(first(a)));}
List* fceil(List* a)  {return (List*)value_to_number(1+(int)numVal(first(a)));}

List* fminNum(List* a)  {
	List* arg = car(a);

	if(is_pair(arg)){
		List* l = arg;
		double n1 = numVal(first(l));
		double min = n1;
		while(l){
			n1 = numVal(first(l));
			if(n1 < min){min = n1;}
			l = cdr(l);
		}
		return (List*)value_to_number(min);

	}else if(is_vector(arg)){
		//TODO
	}
	return 0;
}

List* fmaxNum(List* a)  {
	List* arg = car(a);

	if(is_pair(arg)){
		List* l = arg;
		double n1 = numVal(first(l));
		double max = n1;
		while(l){
			n1 = numVal(first(l));
			if(n1 > max){max = n1;}
			l = cdr(l);
		}
		return (List*)value_to_number(max);

	}else if(is_vector(arg)){
		//TODO
	}
	return 0;
}


List* fpow(List* a)   {return (List*)value_to_number(pow(numVal(first(a)), numVal(second(a))));}
List* fmodulo(List* a){return (List*)value_to_number((int)numVal(first(a)) % (int)numVal(second(a)));}
List* fsqrt(List* a)  {return (List*)value_to_number(sqrt(numVal(first(a))));}
List* flog(List* a)    {return (List*)value_to_number(log(numVal(first(a))));}
List* flog10(List* a)  {return (List*)value_to_number(log10(numVal(first(a))));}
List* fatan2(List* a) {return (List*)value_to_number(atan2(numVal(first(a)), numVal(second(a))));}

List* fbitAnd(List* a){return (List*)value_to_number((int)numVal(first(a)) & (int)numVal(second(a)));}

List* flessThan(List* a)    {return numVal(first(a)) < numVal(second(a)) ? e_true : e_false;}
List* fgreaterThan(List* a) {return numVal(first(a)) > numVal(second(a)) ? e_true : e_false;}


List* frange(List* a){
	double min = 0, max = 0; 
	if (cdr(a)){
		min = numVal(first(a));
		max = numVal(second(a));
	}else{
		max = numVal(first(a));
	}
	List* ret = 0;
	for(int i = max-1; i>=min; i--){
		ret = cons(value_to_number(i), ret);
	}
	return ret;
}


List* fseed(List* a){
	int seed = 0;
	if (a){ seed = (int)numVal(first(a));
	}else{  seed = time(NULL);}
	srand(seed);
	return 0;
}



/// (rand min max) ;min is inclusive, max is exclusive
/// (rand max) ;max is exclusive
List* frand(List* a){
	List* arg1 = first(a);
	int min = 0;
	int max = 0;
	if (cdr(a)){
		//Min - max
		min = numVal(arg1);
		max = numVal(second(a));
	}else{
		//Max only
		max = numVal(arg1);
	}
	return (List*)value_to_number(randInt(min, max));
}






// ---------------------------------------------
// List operations
List* fcons(List* a) { return cons(first(a), second(a)); }
List* flist(List* a) { return a;}
List* fcar(List* a)  { return car(first(a)); }
List* fcdr(List* a)  { return cdr(first(a)); }

List* freverse(List* a) {
	List* ret = 0;
	List* l = first(a);
	while(l){
		ret = cons(car(l), ret);
		l = cdr(l);
	}
	return ret;	
}



Vector* concatVec(Vector* v1, Vector* v2){
	int size1 = v1->size;
	int size2 = v2->size;
	int tot = size1+size2;
	Vector* vecObj = newVec(tot);
	void** data1 = v1->data;
	void** data2 = v2->data;
	void** data = vecObj->data;

	int i = 0;
	while(i<size1){ data[i] = data1[i]; i++;}
	while(i<tot){   data[i] = data2[i-size1]; i++;}
	data[tot] = 0;
	return vecObj;
}


List* fconcat(List* a) {
	List* v1 = first(a);
	if(is_vector(v1)){
		Vector* untagged_result = (Vector*)untag_vector(v1);
		List* current = cdr(a);
		while(current){
			Vector* untagged_item = (Vector*)untag_vector(car(current));
			untagged_result = concatVec(untagged_result, untagged_item);
			current = cdr(current);
		}
		return (List*)tag_vector(untagged_result);
	}else{
		printf("Concat not yet implemented with type: %p", v1);
		return 0;
	}
}

List* ffirst(List* a){
	return first(first(a));
}

List* flast(List* a){
	List* l = first(a);
	List* current = l;
	List* last = 0;
	while(current){
		last = car(current);
		current = cdr(current);
	}
	return last;
}


// ---------------------------------------------
// Test and type checking
List* feq(List* a)   {
	List* v1 = first(a);
	List* v2 = second(a);
	if (is_number(v1) && is_number(v2)){
		double x1 = numVal(v1);	
		double x2 = numVal(v2);	
		return x1 == x2 ? e_true : e_false;
	}else{
		return first(a) == second(a) ? e_true : e_false;
	}
}
List* fpair(List* a) { return is_pair(first(a))     ? e_true : e_false; }
List* fatom(List* a) { return is_atom(first(a))     ? e_true : e_false; }
List* fnull(List* a) { return first(a) == 0         ? e_true : e_false; }



// ---------------------------------------------
// Strings
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



// ---------------------------------------------
// Data structures
int listLength(List* a){
	int n = 0;
	List* current = a;
	while(current){n++; current = cdr(current);}
	return n;
}

/// (vector values...)
List* fvec(List* a){
	Vector* vec = listToVec(a);
	return (List*)tag_vector(vec);
}

/// (hashmap key val ...)
List* fhashmap(List* a){
	map_t map = newHashmap();
	List* current = a;
	while(current){
		char* key = first(current);
		List* val = second(current);
		hashmap_put(map, key, val);
		current = cdr(cdr(current));
	}
	return (List*)tag_hashmap(map);
}

/// (get m k)
List* fget(List* a){
	List* seq = first(a);
	if(is_pair(seq)){
		seq = fvec(seq);
	}

	//Map
	if (is_hashmap(seq)){	
		map_t map = (map_t)untag_hashmap(seq);
		char* key = (char*)second(a);
		List* value = 0;
		hashmap_get(map, key, (any_t)&value);
		return value;
	//Vector
	}else if (is_vector(seq)){
		Vector* vec = (Vector*)untag_vector(seq);
		int size = vec->size;
		int pos = (int)numVal(second(a));
		if (pos >= size){
			return 0;
		}else{
			void** data = vec->data;
			return data[pos];
		}
	}else{
		return 0;
	}
}


/// (count vec)
List* fcount(List* a){
	List* arg = first(a);
	if (is_vector(arg)){
		Vector* vec = (Vector*)untag_vector(arg);
		int size = vec->size;
		return (List*)value_to_number(size);
	}else{
		printf("%p not supported for count.", arg);
		return 0;
	}
}


/// (assoc coll key value)
List* fassoc(List* a){
	List* coll = first(a);
	List* key_val = cdr(a);

	if (is_vector(coll)){
		Vector* old = (Vector*)untag_vector(coll);
		Vector* new = copyVec(old);

		int size = old->size;
		while(key_val){
			int pos = (int)numVal(first(key_val));
			if (pos<size){
				void** data = new->data;
				data[pos] = second(key_val);
			}
			key_val = cdr(cdr(key_val));
		}
		return (List*)tag_vector(new);

	}else{
		printf("Assoc not yet implemented for pointer type %p", coll);
		return 0;
	}
}




// ---------------------------------------------
// Cairo 
List* fsvg_surface(List* a){
	float w = numVal(second(a));
	float h = numVal(third(a));
	char* n = first(a);
	n++;
	n[strlen(n)-1] = '\0';
	cairo_surface_t* surface = cairo_svg_surface_create(n, w, h);
	cairo_svg_surface_restrict_to_version(surface, 1);
	cairo_surface_set_fallback_resolution(surface, 72., 72.);
	return (List*)surface;
}

List* fsvg_status(List* a){
	printf("\nSURFACE STATUS: %s\n", cairo_status_to_string(cairo_surface_status(first(a))));
	fflush(stdout);
	return 0;
}

List* fsvg_context(List* a){
	cairo_t* context = cairo_create(first(a));
	cairo_set_line_width(context, 1);
	cairo_set_source_rgb(context, 0, 0, 0);
	return (List*)context;
}

List* fsvg_clean(List* a){
	cairo_surface_t* surface = first(a);
	cairo_t* context = second(a);
	cairo_surface_flush(surface);
	cairo_surface_finish(surface);
	cairo_surface_destroy(surface);
	cairo_destroy(context);
	return 0;
}

// (svg-line context pointA pointB)
List* fsvg_line(List* a){
	cairo_t* context = first(a);
	map_t p1 = (map_t)untag_hashmap(second(a));
	map_t p2 = (map_t)untag_hashmap(third(a));
	List *x1, *y1, *x2, *y2;
	hashmap_get(p1, ":x", (any_t)&x1);
	hashmap_get(p1, ":y", (any_t)&y1);
	hashmap_get(p2, ":x", (any_t)&x2);
	hashmap_get(p2, ":y", (any_t)&y2);
	cairo_move_to(context, numVal(x1), numVal(y1));
	cairo_line_to(context, numVal(x2), numVal(y2));
	cairo_stroke(context);
	return 0;
}

