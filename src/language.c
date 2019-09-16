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
	double res = numVal((double*)first(list));
	List* current = list;
	while((current = cdr(current))){
		res = numberOperation(op, res, numVal((double*)car(current)));
	}
	return (List*)value_to_number(res);
}

//Compose the callable functions
List* fadd(List* a) {return applyOperationOnList('+', a);}
List* fsub(List* a) {return applyOperationOnList('-', a);}
List* fmul(List* a) {return applyOperationOnList('*', a);}
List* fdiv(List* a) {return applyOperationOnList('/', a);}


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

List* fmap(List* a){
	List* l = second(a);
	List* ret = 0;
	while (l){
		List* val = car(l);
		List* r = ((List* (*) (List*))first(a))(cons(val, 0));
		ret = cons(r, ret);
		l = cdr(l);
	}
	return freverse(cons(ret, 0));
}


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



// ---------------------------------------------
// Data structures
int listLength(List* a){
	int n = 0;
	List* current = a;
	while(current){n++; current = cdr(current);}
	return n;
}

int vecLength(void** v){
	int n = 0;
	while(v[n]){n++;}
	return n;
}

/// (vector values...)
List* fvec(List* a){
	int n = listLength(a);
	void** vec = malloc(sizeof(void*) * (n+1));
	List* current = a;
	int i = 0;
	while(current){
		vec[i] = first(current);	
		current = cdr(current);
		i++;
	}
	vec[n] = 0;
	return (List*)tag_vector(vec);
}

/// (hashmap key val ...)
List* fhashmap(List* a){
	map_t map = hashmap_new();
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
	if (is_hashmap(first(a))){	
		map_t map = (map_t)untag_hashmap(first(a));
		char* key = (char*)second(a);
		List* value = 0;
		hashmap_get(map, key, (any_t)&value);
		return value;

	}else if (is_vector(first(a))){
		void** vec = (void**)untag_vector(first(a));
		int pos = (int)numVal(second(a));
		int size = vecLength(vec);
		if (pos >= size){
			return 0;
		}else{
			return vec[pos];
		}
	}else{
		return 0;
	}
}


// ---------------------------------------------
// Cairo 
List* fsvg_surface(List* a){
	float w = numVal((double*)second(a));
	float h = numVal((double*)third(a));
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
	double *x1, *y1, *x2, *y2;
	hashmap_get(p1, ":x", (any_t)&x1);
	hashmap_get(p1, ":y", (any_t)&y1);
	hashmap_get(p2, ":x", (any_t)&x2);
	hashmap_get(p2, ":y", (any_t)&y2);
	cairo_move_to(context, numVal(x1), numVal(y1));
	cairo_line_to(context, numVal(x2), numVal(y2));
	cairo_stroke(context);
	return 0;
}

