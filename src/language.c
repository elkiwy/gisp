#include "language.h"


// --------------------------------------------
///= Core: Math operations

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

///~Adds all the number arguments
///&+
///#Number
///@1n
///!1Number
///@2...
///!2Numbers
__attribute__((aligned(16))) List* fadd(List* a) {return applyOperationOnList('+', a);}
///~Subtracts all the number arguments
///&-
///#Number
///@1n
///!1Number
///@2...
///!2Numbers
__attribute__((aligned(16))) List* fsub(List* a) {return applyOperationOnList('-', a);}
///~Multiplies all the number arguments
///&*
///#Number
///@1n
///!1Number
///@2...
///!2Numbers
__attribute__((aligned(16))) List* fmul(List* a) {return applyOperationOnList('*', a);}
///~Divide all the number arguments
///&/
///#Number
///@1n
///!1Number
///@2...
///!2Numbers
__attribute__((aligned(16))) List* fdiv(List* a) {return applyOperationOnList('/', a);}

///~Calculate the sin of a radiant value
///&sin
///#Number
///@1n
///!1Number
__attribute__((aligned(16))) List* fsin(List* a) {return (List*)value_to_number(sin(numVal(first(a))));}
///~Calculate the cos of a radiant value
///&cos
///#Number
///@1n
///!1Number
__attribute__((aligned(16))) List* fcos(List* a) {return (List*)value_to_number(cos(numVal(first(a))));}
///~Calculate the sin of a degree value
///&dsin
///#Number
///@1n
///!1Number
__attribute__((aligned(16))) List* fdsin(List* a) {return (List*)value_to_number(sin(numVal(first(a)) * DEG_TO_RAD));}
///~Calculate the cos of a degree value
///&dcos
///#Number
///@1n
///!1Number
__attribute__((aligned(16))) List* fdcos(List* a) {return (List*)value_to_number(cos(numVal(first(a)) * DEG_TO_RAD));}

///~Transform a degree value into radians
///&rad
///#Number
///@1n
///!1Number
__attribute__((aligned(16))) List* frad(List* a) {return (List*)value_to_number(numVal(first(a)) * DEG_TO_RAD);}
///~Transform a radians value into degrees
///&deg
///#Number
///@1n
///!1Number
__attribute__((aligned(16))) List* fdeg(List* a) {return (List*)value_to_number(numVal(first(a)) * RAD_TO_DEG);}
///~Casts a number to an integer value
///&int
///#Number
///@1n
///!1Number
__attribute__((aligned(16))) List* fint(List* a)   {return (List*)value_to_number((int)numVal(first(a)));}
///~Get the floor value of a number
///&floor
///#Number
///@1n
///!1Number
__attribute__((aligned(16))) List* ffloor(List* a) {return (List*)value_to_number((int)numVal(first(a)));}
///~Get the ceil value of a number
///&ceil
///#Number
///@1n
///!1Number
__attribute__((aligned(16))) List* fceil(List* a)  {return (List*)value_to_number(1+(int)numVal(first(a)));}

///~Get the minimum value of all the arguments
///&min
///#Number
///@1n
///!1Number
///@2...
///!2Numbers
__attribute__((aligned(16))) List* fminNum(List* a)  {
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

///~Get the maximum value of all the arguments
///&max
///#Number
///@1n
///!1Number
///@2...
///!2Numbers
__attribute__((aligned(16))) List* fmaxNum(List* a)  {
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


///~Calculate the power of the first argument base with the second value as exponent
///&pow
///#Number
///@1base
///!1Number
///@2exp
///!2Number
__attribute__((aligned(16))) List* fpow(List* a)   {return (List*)value_to_number(pow(numVal(first(a)), numVal(second(a))));}
///~Calculate the modulo
///&mod
///#Number
///@1n
///!1Number
///@2div
///!2Number
__attribute__((aligned(16))) List* fmodulo(List* a){return (List*)value_to_number((int)numVal(first(a)) % (int)numVal(second(a)));}
///~Calculate the square root of a number
///&sqrt
///#Number
///@1n
///!1Number
__attribute__((aligned(16))) List* fsqrt(List* a)  {return (List*)value_to_number(sqrt(numVal(first(a))));}
///~Calculate the natural log of a number
///&log
///#Number
///@1n
///!1Number
__attribute__((aligned(16))) List* flog(List* a)    {return (List*)value_to_number(log(numVal(first(a))));}
///~Calculate the log base 10 of a number
///&log10
///#Number
///@1n
///!1Number
__attribute__((aligned(16))) List* flog10(List* a)  {return (List*)value_to_number(log10(numVal(first(a))));}
///~Calculate the atan2 of an X and Y value
///&atan2
///#Number
///@1y
///!1Number
///@2x
///!2Number
__attribute__((aligned(16))) List* fatan2(List* a) {return (List*)value_to_number(atan2(numVal(first(a)), numVal(second(a))));}
///~Bitwise and operator
///&bit-and
///#Number
///@1a
///!1Number
///@2b
///!2Number
__attribute__((aligned(16))) List* fbitAnd(List* a){return (List*)value_to_number((int)numVal(first(a)) & (int)numVal(second(a)));}
///~Compare to number
///&<
///#Bool
///@1a
///!1Number
///@2b
///!2Number
__attribute__((aligned(16))) List* flessThan(List* a)    {return numVal(first(a)) < numVal(second(a)) ? e_true : e_false;}
///~Compare to number
///&>
///#Bool
///@1a
///!1Number
///@2b
///!2Number
__attribute__((aligned(16))) List* fgreaterThan(List* a) {return numVal(first(a)) > numVal(second(a)) ? e_true : e_false;}


///~Create a range list with all the number between min (default 0 if omitted) and a max
///&range
///#List
///@1min
///!1Number
///@2max
///!2Number
__attribute__((aligned(16))) List* frange(List* a){
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


///~Sets the starting point for the random number generator. seed paramter is random if not provided.
///&seed
///#void
///@1seed
///!1Number
__attribute__((aligned(16))) List* fseed(List* a){
	int seed = 0;
	if (a){ seed = (int)numVal(first(a));
	}else{  seed = time(NULL);}
	srand(seed);
	return 0;
}



///~Get a random number between min inclusive (default 0) and max exclusive
///&rand
///#Number
///@1min
///!1Number
///@2max
///!2Number
__attribute__((aligned(16))) List* frand(List* a){
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
///=Core: List operations

///~Create a cons structure between two atoms
///&cons
///#List
///@1a
///!1Atom
///@2b
///!2Atom
__attribute__((aligned(16))) List* fcons(List* a) { return cons(first(a), second(a)); }
///~Create a list structure
///&list
///#List
///@1a
///!1Any
///@2...
///!2Any
__attribute__((aligned(16))) List* flist(List* a) { return a;}
///~Get the first element of a list
///&car
///#Any
///@1list
///!1List
__attribute__((aligned(16))) List* fcar(List* a)  { return car(first(a)); }
///~Get all but the first element of a list
///&cdr
///#List
///@1list
///!1List
__attribute__((aligned(16))) List* fcdr(List* a)  { return cdr(first(a)); }
///~Reverse a list structure
///&reverse
///#List
///@1list
///!1List
__attribute__((aligned(16))) List* freverse(List* a) {
	List* ret = 0;
	List* l = first(a);
	while(l){
		ret = cons(car(l), ret);
		l = cdr(l);
	}
	return ret;	
}



__attribute__((aligned(16))) Vector* concatVec(Vector* v1, Vector* v2){
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


///~Concatenate two structures together
///&concat
///#List/Vector
///@1a
///!1List/Vector
///@2b
///!2List/Vector
__attribute__((aligned(16))) List* fconcat(List* a) {
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

///~Get the first element of a list
///&first
///#Any
///@1a
///!1List
__attribute__((aligned(16))) List* ffirst(List* a){
	return first(first(a));
}

///~Get the last element of a list
///&last
///#Any
///@1a
///!1List
__attribute__((aligned(16))) List* flast(List* a){
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
///Core: Test and type checking
///~Checks if the first two element are equal
///&=
///#Bool
///@1a
///!1Any
///@2b
///!2Any
__attribute__((aligned(16))) List* feq(List* a)   {
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
///~Checks if the first argument is a pair
///&pair?
///#Bool
///@1a
///!1Any
__attribute__((aligned(16))) List* fpair(List* a) { return is_pair(first(a))     ? e_true : e_false; }
///~Checks if the first argument is an atom
///&symbol?
///#Bool
///@1a
///!1Any
__attribute__((aligned(16))) List* fatom(List* a) { return is_atom(first(a))     ? e_true : e_false; }
///~Checks if the first argument is null
///&null?
///#Bool
///@1a
///!1Any
__attribute__((aligned(16))) List* fnull(List* a) { return first(a) == 0         ? e_true : e_false; }



// ---------------------------------------------
///=Core: Strings
///~Concatenate strings together
///&str
///#String
///@1a
///!1Any
///@2...
///!2Any
__attribute__((aligned(16))) List* fstr(List* a) {
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
///=Core: Data structures
int listLength(List* a){
	int n = 0;
	List* current = a;
	while(current){n++; current = cdr(current);}
	return n;
}

/// (vector values...)
///~Convert a list into a vector
///&vector
///#Vector
///@1val
///!1Any
///@2...
///!2Any
__attribute__((aligned(16))) List* fvec(List* a){
	Vector* vec = listToVec(a);
	return (List*)tag_vector(vec);
}

/// (hashmap key val ...)
///~Create an hashmap structure from a set of keys and values
///&hashmap
///#HashMap
///@1key
///!1String/Keyword
///@2val
///!2Any
///@3...
///!3Any
__attribute__((aligned(16))) List* fhashmap(List* a){
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
///~Get an element from a structure m at key k
///&get
///#Vector/HashMap
///@1m
///!1Vector/HashMap
///@2k
///!2String/Keyword
__attribute__((aligned(16))) List* fget(List* a){
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
///~Counts the elements of a structure m
///&count
///#Number
///@1m
///!1Vector/HashMap
__attribute__((aligned(16))) List* fcount(List* a){
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


/// (assoc m key value)
///~Associate a value at the key in a structure m
///&assoc
///#Vector/HashMap
///@1m
///!1Vector/HashMap
///@2key
///!2String/Keyword
///@3value
///!3Any
__attribute__((aligned(16))) List* fassoc(List* a){
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
///=Core: Cairo 
///~Creates a new surface of width w and height h 
///&make-surface
///#Surface
///@1path
///!1String
///@2w
///!2Number
///@3h
///!3Number
__attribute__((aligned(16))) List* fsvg_surface(List* a){
	float w = numVal(second(a));
	float h = numVal(third(a));
	char* filename = trim_quotes(first(a));

	char fullPath[4096];
	strcpy(fullPath, gispWorkingDir);
	strcat(fullPath, filename);

	cairo_surface_t* surface = cairo_svg_surface_create(fullPath, w, h);
	cairo_svg_surface_restrict_to_version(surface, 1);
	cairo_surface_set_fallback_resolution(surface, 72., 72.);
	return (List*)surface;
}

///~Prints the status of a surface s
///&surface-status
///#void
///@1s
///!1Surface
__attribute__((aligned(16))) List* fsvg_status(List* a){
	printf("\nSURFACE STATUS: %s\n", cairo_status_to_string(cairo_surface_status(first(a))));
	fflush(stdout);
	return 0;
}

///~Create a context for a suraface
///&make-context
///#Context
///@1s
///!1Surface
__attribute__((aligned(16))) List* fsvg_context(List* a){
	cairo_t* context = cairo_create(first(a));
	cairo_set_source_rgb(context, 1, 1, 1);
	cairo_paint(context);
	cairo_set_line_width(context, 1);
	cairo_set_source_rgb(context, 0, 0, 0);
	return (List*)context;
}

///~Clean a surface destroying it
///&surface-clean
///#void
///@1s
///!1Surface
__attribute__((aligned(16))) List* fsvg_clean(List* a){
	cairo_surface_t* surface = first(a);
	cairo_t* context = second(a);
	cairo_surface_flush(surface);
	cairo_surface_finish(surface);
	cairo_surface_destroy(surface);
	cairo_destroy(context);
	return 0;
}


// (line context pointA pointB)
///~Draw a line to a context
///&line
///#void
///@1c
///!1Context
///@2p1
///!2Point
///@3p2
///!3Point
__attribute__((aligned(16))) List* fsvg_line(List* a){
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



///~Export a surface to a png file
///&surface-to-png
///#void
///@1s
///!1Surface
///@2name
///!2string
__attribute__((aligned(16))) List* fsvg_to_png(List* a){
	cairo_surface_t* surface = first(a);
	char* filename = trim_quotes(second(a));
	char fullPath[4096];
	strcpy(fullPath, gispWorkingDir);
	strcat(fullPath, filename);
	printf("Saving image %s\n", fullPath);fflush(stdout);
	cairo_surface_write_to_png(surface, fullPath);
	return 0;
}
