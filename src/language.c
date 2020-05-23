#include "language.h"


// --------------------------------------------
///=Core: Math operations

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
	List* current = cdr(list);
	while(notNil(current)){
		res = numberOperation(op, res, numVal(car(current)));
		current = cdr(current);
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

///~Calculate the abs of a radiant value
///&abs
///#Number
///@1n
///!1Number
__attribute__((aligned(16))) List* fabsval(List* a) {return (List*)value_to_number(fabs(numVal(first(a))));}
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
__attribute__((aligned(16))) List* fceil(List* a)  {return (List*)value_to_number(ceil(numVal(first(a))));}

///~Get the minimum value of all the arguments
///&min
///#Number
///@1n
///!1Number
///@2...
///!2Numbers
__attribute__((aligned(16))) List* fminNum(List* a)  {
	double min = numVal(first(a));
	List* current = cdr(a);
	while(notNil(current)){
		double val = numVal(car(current));
		if (val<min){min = val;}
		current=cdr(current);
	}

	return (List*)value_to_number(min);
}

///~Get the maximum value of all the arguments
///&max
///#Number
///@1n
///!1Number
///@2...
///!2Numbers
__attribute__((aligned(16))) List* fmaxNum(List* a)  {
	double max = numVal(first(a));
	List* current = cdr(a);
	while(notNil(current)){
		double val = numVal(car(current));
		if (val>max){max = val;}
		current=cdr(current);
	}
	return (List*)value_to_number(max);
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
///~Get the sign of a number
///&sign
///#Number
///@1n
///!1Number
__attribute__((aligned(16))) List* fsign(List* a){
	double n = numVal(first(a));
	if (n>0){return (List*)value_to_number(1);}
	if (n<0){return (List*)value_to_number(-1);}
	if (n==0){return (List*)value_to_number(0);}
	return e_nil;
}
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
///~Compare to number
///&<=
///#Bool
///@1a
///!1Number
///@2b
///!2Number
__attribute__((aligned(16))) List* flessOrEqThan(List* a) {return numVal(first(a)) <= numVal(second(a)) ? e_true : e_false;}
///~Compare to number
///&>=
///#Bool
///@1a
///!1Number
///@2b
///!2Number
__attribute__((aligned(16))) List* fgreaterOrEqThan(List* a) {return numVal(first(a)) >= numVal(second(a)) ? e_true : e_false;}


///~Create a range list with all the number between min (default 0 if omitted) and a max
///&range
///#List
///@1min
///!1Number
///@2max
///!2Number
__attribute__((aligned(16))) List* frange(List* a){
	double min = 0, max = 0; 
	if (notNil(cdr(a))){
		min = numVal(first(a));
		max = numVal(second(a));
	}else{
		max = numVal(first(a));
	}
	List* ret = e_nil;
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
	if (notNil(a)){seed = (int)numVal(first(a));
	}else{
		struct timeval t;
		gettimeofday(&t, NULL);
		seed = t.tv_usec;
	}
	srand(seed);
	return e_nil;
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
	if (notNil(cdr(a))){
		//Min - max
		min = numVal(arg1);
		max = numVal(second(a));
	}else{
		//Max only
		max = numVal(arg1);
	}
	return (List*)value_to_number(randInt(min, max));
}

///~Get a random number weighted within a gaussian distribution
///&gauss
///#Number
///@1center
///!1Number
///@2amplitude
///!2Number
__attribute__((aligned(16))) List* fgauss(List* a){
	double center = numVal(first(a));
	double ampl   = numVal(second(a));

	double x = (double)rand() / RAND_MAX;
	double y = (double)rand() / RAND_MAX;
	double z = sqrt(-2 * log(x)) * cos(2 * M_PI * y);
	return (List*)value_to_number((z * ampl) + center);
}













#define RANDOM_SEED 0

#define NUMBER_OF_SWAPS 400

const double P_SUPPLY[] = {
	151, 160, 137, 91, 90, 15, 131, 13, 201, 95, 96, 53, 194, 233, 7, 225, 140, 36,
	103, 30, 69, 142, 8, 99, 37, 240, 21, 10, 23, 190, 6, 148, 247, 120, 234, 75, 0,
	26, 197, 62, 94, 252, 219, 203, 117, 35, 11, 32, 57, 177, 33, 88, 237, 149, 56,
	87, 174, 20, 125, 136, 171, 168, 68, 175, 74, 165, 71, 134, 139, 48, 27, 166,
	77, 146, 158, 231, 83, 111, 229, 122, 60, 211, 133, 230, 220, 105, 92, 41, 55,
	46, 245, 40, 244, 102, 143, 54, 65, 25, 63, 161, 1, 216, 80, 73, 209, 76, 132, 187,
	208, 89, 18, 169, 200, 196, 135, 130, 116, 188, 159, 86, 164, 100, 109, 198, 173, 186,
	3, 64, 52, 217, 226, 250, 124, 123, 5, 202, 38, 147, 118, 126, 255, 82, 85, 212, 207,
	206, 59, 227, 47, 16, 58, 17, 182, 189, 28, 42, 223, 183, 170, 213, 119, 248, 152,
	2, 44, 154, 163, 70, 221, 153, 101, 155, 167, 43, 172, 9, 129, 22, 39, 253, 19, 98,
	108, 110, 79, 113, 224, 232, 178, 185, 112, 104, 218, 246, 97, 228, 251, 34, 242,
	193, 238, 210, 144, 12, 191, 179, 162, 241, 81, 51, 145, 235, 249, 14, 239, 107,
	49, 192, 214, 31, 181, 199, 106, 157, 184, 84, 204, 176, 115, 121, 50, 45, 127,
	4, 150, 254, 138, 236, 205, 93, 222, 114, 67, 29, 24, 72, 243, 141, 128, 195, 78,
    66, 215, 61, 156, 180};

#define P_SUPPLY_SIZE 256



typedef struct simplex_octave{
	double* octavePerm;
	double* octavePermMod12;
	double frequency;
	double amplitude;
} simplex_octave;

typedef struct simplex_generator{
	simplex_octave** octaves;
	int octaves_count;
} simplex_generator;

///~Create e simplex noise generator
///&simplex-noise
///#Generator
///@1largest_feature
///!1Number
///@2persistance
///!2Number
///@3seed
///!3Number
__attribute__((aligned(16))) List* fsimplex_noise(List* a){
	double largest_feature = numVal(first(a));
	double persistance     = numVal(second(a));
	double starting_seed   = numVal(third(a));

	srand(starting_seed);

	double octaves_count = ceil(log10(largest_feature) / log10(2));
	simplex_generator* generator = malloc(sizeof(simplex_generator));
	generator->octaves_count = (int)octaves_count;
	simplex_octave** octaves = malloc(sizeof(simplex_octave*) * octaves_count);
	for(int i=0; i<octaves_count; ++i){

		srand(randInt(0, 99999));

		int size = (P_SUPPLY_SIZE * 2);
		double* perm = malloc(sizeof(double) * size);
		for (int j=0;j<P_SUPPLY_SIZE; ++j){
			perm[j] = P_SUPPLY[j];	
		}

		for(int j=0; j<NUMBER_OF_SWAPS; ++j){
			int r1 = randInt(0, P_SUPPLY_SIZE);
			int r2 = randInt(0, P_SUPPLY_SIZE);
			double tmp = perm[r1];
			perm[r1] = perm[r2];
			perm[r2] = tmp;
		}

		for (int j=0;j<P_SUPPLY_SIZE; ++j){
			perm[j+P_SUPPLY_SIZE] = perm[j];	
		}
		
		double* permMod12 = malloc(sizeof(double) * size);
		for (int j=0; j<size; ++j){
			permMod12[j] = (int)perm[j] % 12;
		}

		octaves[i] = malloc(sizeof(simplex_octave));
		octaves[i]->octavePerm = perm;
		octaves[i]->octavePermMod12 = permMod12;
		octaves[i]->frequency = pow(2, i);
		octaves[i]->amplitude = pow(persistance, octaves_count - i);
	}
	generator->octaves = octaves;


	//DEBUG
	//printf("New generator:\n");
	//printf("octaves:%d\n", generator->octaves_count);
	//for(int i=0; i<octaves_count; ++i){
	//	printf("octave[%d]->freq:%f\n", i, generator->octaves[i]->frequency);
	//	printf("octave[%d]->ampl:%f\n", i, generator->octaves[i]->amplitude);
	//	printf("octave[%d]->perm:", i);
	//	for (int j=0;j<(P_SUPPLY_SIZE*2);++j){printf(" %d", (int)generator->octaves[i]->octavePerm[j]);}
	//	printf("octave[%d]->perm12:", i);
	//	for (int j=0;j<(P_SUPPLY_SIZE*2);++j){printf(" %d", (int)generator->octaves[i]->octavePermMod12[j]);}
	//}




	return (List*)generator;
}



double  pp1[3] = { 1,  1,  0};
double  pp2[3] = {-1,  1,  0};
double  pp3[3] = { 1, -1,  0};
double  pp4[3] = {-1, -1,  0};
double  pp5[3] = { 1,  0,  1};
double  pp6[3] = {-1,  0,  1};
double  pp7[3] = { 1,  0, -1};
double  pp8[3] = {-1,  0, -1};
double  pp9[3] = { 0,  1,  1};
double pp10[3] = { 0, -1,  1};
double pp11[3] = { 0,  1, -1};
double pp12[3] = { 0, -1, -1};
double* gradiens[12] = {pp1, pp2, pp3, pp4, pp5, pp6, pp7, pp8, pp9, pp10, pp11, pp12};

double F2 = 0.3660254038;
double G2 = 0.2113248654;


double contribution(int grad_index, double x, double y){
	double val = 0.5 - (x * x) - (y * y);
	if (val < 0){
		return 0;
	}else{
		val = val*val;
		return val * val * ((gradiens[grad_index][0] * x) + (gradiens[grad_index][1] * y));
	}
}

double noise(double* octavePerm, double* octavePermMod12, double xin, double yin){
	double s = (xin + yin) * F2;
	int i = floor(xin + s);
	int j = floor(yin + s);
	double t = (double)(i + j) * G2;
	double X0 = i - t;
	double Y0 = j - t;
	double x0 = xin - X0;
	double y0 = yin - Y0;
	int i1 = (x0 > y0) ? 1 : 0;
	int j1 = (x0 > y0) ? 0 : 1;
	double x1 = (x0 - (double)i1) + G2;
	double y1 = (y0 - (double)j1) + G2;
	double x2 = (x0 - 1.0) + (2.0 * G2);
	double y2 = (y0 - 1.0) + (2.0 * G2);
	int ii = i & 255;
	int jj = j & 255;
	int gi0 = octavePermMod12[ii + (int)octavePerm[jj]];
	int gi1 = octavePermMod12[ii + i1 + (int)octavePerm[jj + j1]];
	int gi2 = octavePermMod12[ii +  1 + (int)octavePerm[jj +  1]];
	double n0 = contribution(gi0, x0, y0);
	double n1 = contribution(gi1, x1, y1);
	double n2 = contribution(gi2, x2, y2);
	return (double)70 * (n0 + n1 + n2);
}

///~Extract a value from a previously defined generator at x and y
///&simplex-noise-value
///#Number
///@1simplex
///!1Generator
///@2x
///!2Number
///@3y
///!3Number
__attribute__((aligned(16))) List* fsimplex(List* a){
	simplex_generator* generator = (simplex_generator*)first(a);
	double x = numVal(second(a));
	double y = numVal(third(a));
	int octaves_count = generator->octaves_count;

	double result = 0;
	for(int i=0; i<octaves_count; ++i){
		simplex_octave* octave = generator->octaves[i];
		
		double freq = octave->frequency;
		double ampl = octave->amplitude;
		double* perm = octave->octavePerm;
		double* permMod12 = octave->octavePermMod12;

		result += ampl * noise(perm, permMod12, x/freq, y/freq);
	}

	return (List*)value_to_number(result);
}









///~Calculate the mandelbrot value of a specific point
///&mandelbrot-point
///#Number
///@1a
///!1Number
///@2b
///!2Number
///@3max-iterations
///!3Number
///@4infinity
///!4Number
__attribute__((aligned(16))) List* fmandelbrot(List* a){
	double realVal = numVal(first(a));
	double imaginaryVal = numVal(second(a));
	int maxIterations = (int)numVal(third(a));
	int infinity = (int)numVal(fourth(a));

	
	double currentReal = realVal;
	double currentImaginary = imaginaryVal;

	int n = 0;
	while(n<maxIterations){
		//printf("mandel iteration : %d of %d\n", n, maxIterations);fflush(stdout);
		double currentRealNew = ((currentReal * currentReal) - (currentImaginary * currentImaginary));
		double currentImaginaryNew = (2 * currentReal * currentImaginary);
		currentReal = currentRealNew + realVal;
		currentImaginary = currentImaginaryNew + imaginaryVal;
		if (currentReal + currentImaginary > infinity){
			break;
		}
		n++;
	}

	if (n==maxIterations){n = 0;}
	return (List*)value_to_number(n);
}

///~Rescale a value from a min and max to a newmin and a newmax
///&rescale
///#Number
///@1n
///!1Number
///@2min
///!2Number
///@3max
///!3Number
///@4newmin
///!4Number
///@5newmax
///!5Number
__attribute__((aligned(16))) List* frescale(List* a){
	double n = numVal(first(a));
	double currmin = numVal(second(a));
	double currmax = numVal(third(a));
	double newmin = numVal(fourth(a));
	double newmax = numVal(fifth(a));

	double ran1 = currmax - currmin;
	double ran2 = newmax - newmin;
	double perc = (n - currmin) / ran1;
	double result = newmin + (perc * ran2);
	return (List*)value_to_number(result);
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
__attribute__((aligned(16))) List* fcons(List* a) { return cons(objCopy(first(a)), objCopy(second(a))); }
///~Create a list structure
///&list
///#List
///@1a
///!1Any
///@2...
///!2Any
__attribute__((aligned(16))) List* flist(List* a) {return objCopy(a);}
///~Get the first element of a list
///&car
///#Any
///@1list
///!1List
__attribute__((aligned(16))) List* fcar(List* a)  { return objCopy(car(first(a))); }
///~Get all but the first element of a list
///&cdr
///#List
///@1list
///!1List
__attribute__((aligned(16))) List* fcdr(List* a)  { return objCopy(cdr(first(a))); }
///~Reverse a list structure
///&reverse
///#List
///@1list
///!1List
__attribute__((aligned(16))) List* freverse(List* a) {
	List* ret = e_nil;
	List* l = first(a);
	while(notNil(l)){
		ret = cons(objCopy(car(l)), ret);
		l = cdr(l);
	}
	return ret;	
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
	if(nil(second(a)) || nil(v1)){return objCopy(v1);}
	
	if(is_string(v1)){
		char* s1 = (char*)untag_string(v1);
		char* s2 = (char*)untag_string(second(a));
		int size = strlen(s1)+strlen(s2)+1;
		char* new = newStringFromSize(size);
		strcpy(new, s1);
		strcat(new, s2);
		return (List*)tag_string(new);

	}else if(is_vector(v1)){
		//Get the total size of the new vector to create
		List* current = a;
		int newSize = 0;
		while(notNil(current)){
			Vector* untagged_item = (Vector*)untag_vector(car(current));
			newSize += untagged_item->size;
			current = cdr(current);
		}

		//Fill all the data
		Vector* newvec = newVec(newSize);
		void** newdata = newvec->data;
		current = a;
		int index = 0;
		while(notNil(current)){
			Vector* untagged_item = (Vector*)untag_vector(car(current));
			void** olddata = untagged_item->data;
			for(int i=0; i<untagged_item->size; ++i){
				newdata[index] = objCopy(olddata[i]);
				index++;
			}
			current=cdr(current);
		}
		return (List*)tag_vector(newvec);

	}else if(is_pair(v1)){
		List* ret = objCopy(v1);
		List* last = listGetLastCons(ret);
		consSetNext(last, objCopy(second(a)));
		return ret;

	}else{
		printf("Concat not yet implemented with type: %p", v1);
		exit(1);
		return e_nil;
	}
}

///~Appends an item at the end of an existing collection
///&append
///#List/Vector
///@1a
///!1List/Vector
///@2b
///!2Any
__attribute__((aligned(16))) List* fappend(List* a) {
	List* v1 = first(a);
	if(nil(v1)){return cons(objCopy(second(a)), e_nil);}
	if(nil(second(a))){return objCopy(v1);}
	
	if(is_vector(v1)){
		Vector* vec = (Vector*)untag_vector(v1);
		void** data = vec->data;
		int size = vec->size;

		Vector* newvec = newVec(size+1);
		void** newdata = newvec->data;
		for(int i=0;i<size;++i){
			newdata[i] = objCopy(data[i]);
		}
		newdata[size] = objCopy(second(a));
		return (List*)tag_vector(newvec);

	}else if(is_pair(v1)){
		List* ret = objCopy(v1);
		List* prevlast = listGetLastCons(ret);
		List* newlast = cons(objCopy(second(a)),e_nil);
		consSetNext(prevlast, newlast);
		return ret;

	}else{
		printf("Append not yet implemented with type: %p", v1);
		exit(1);
		return e_nil;
	}
}


///~Insert an item at the front of an existing collection 
///&insert
///#List/Vector
///@1a
///!1List/Vector
///@2b
///!2Any
__attribute__((aligned(16))) List* finsert(List* a) {
	List* v1 = first(a);
	if(nil(v1)){return cons(objCopy(second(a)), e_nil);}
	if(nil(second(a))){return objCopy(v1);}
	
	if(is_vector(v1)){
		Vector* vec = (Vector*)untag_vector(v1);
		void** data = vec->data;
		int size = vec->size;

		Vector* newvec = newVec(size+1);
		void** newdata = newvec->data;
		for(int i=0;i<size;++i){
			newdata[i+1] = objCopy(data[i]);
		}
		newdata[0] = objCopy(second(a));
		return (List*)tag_vector(newvec);

	}else if(is_pair(v1)){
		List* ret = cons(objCopy(second(a)), objCopy(v1));
		return ret;

	}else{
		printf("Insert not yet implemented with type: %p", v1);
		exit(1);
		return e_nil;
	}
}




///~Get the first element of a list
///&first
///#Any
///@1a
///!1List
__attribute__((aligned(16))) List* ffirst(List* a){
	List* seq = first(a);
	if(nil(seq)){
		return e_nil;
	}else if (is_vector(seq)){
		Vector* vec = (Vector*)untag_vector(seq);
		int size = vec->size;
		if(size>0){
			void** data = vec->data;
			return objCopy(data[0]);
		}else{return e_nil;}
	}else if(is_pair(seq)){
		return objCopy(first(first(a)));
	}else{
		printf("first doesn't support this object.\n");fflush(stdout);
		exit(1);
		return e_nil;
	}
}

///~Get the last element of a list
///&last
///#Any
///@1a
///!1List
__attribute__((aligned(16))) List* flast(List* a){
	List* seq = first(a);
	if (nil(seq)){
		return e_nil;
	}else if (is_vector(seq)){
		Vector* vec = (Vector*)untag_vector(seq);
		int size = vec->size;
		if(size>0){
			void** data = vec->data;
			return objCopy(data[size-1]);
		}else{ return e_nil;}
	}else if (is_pair(seq)){
		List* current = seq;
		List* last = e_nil;
		while(notNil(current)){
			last = car(current);
			current = cdr(current);
		}
		return objCopy(last);

	}else{
		printf("last doesn't support this object.");fflush(stdout);
		exit(1);
		return e_nil;
	}
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
///~Checks if the first argument is a number
///&number?
///#Bool
///@1a
///!1Any
__attribute__((aligned(16))) List* fnumber(List* a) { return is_number(first(a)) ? e_true : e_false; }
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
///&nil?
///#Bool
///@1a
///!1Any
__attribute__((aligned(16))) List* fnull(List* a) { return (first(a) == e_false || first(a) == e_nil || first(a) == 0) ? e_true : e_false; }
///~Checks if the first argument is not null
///&not-nil?
///#Bool
///@1a
///!1Any
__attribute__((aligned(16))) List* fnotnull(List* a) { return (first(a) == e_false || first(a) == e_nil || first(a) == 0) ? e_false : e_true; }
///~Invert a boolean value
///&not
///#Bool
///@1a
///!1Any
__attribute__((aligned(16))) List* fnot(List* a) { return (first(a) == e_false || first(a) == e_nil)     ? e_true : e_false; }
///~Apply AND between two booleans
///&and
///#Bool
///@1a
///!1Boolean
///@2b
///!2Boolean
__attribute__((aligned(16))) List* fand(List* a) { return (first(a) == e_true && second(a) == e_true) ? e_true : e_false; }
///~Apply OR between two booleans
///&or
///#Bool
///@1a
///!1Boolean
///@2b
///!2Boolean
__attribute__((aligned(16))) List* forr(List* a) { return (first(a) == e_true || second(a) == e_true) ? e_true : e_false; }
///~Checks if the first argument is an empty collection
///&empty?
///#Bool
///@1a
///!1Any
__attribute__((aligned(16))) List* fempty(List* a) {
	List* coll = first(a);
	
	if(is_string(coll)){
		char* s = (char*)untag_string(coll);
		return strlen(s)==0 ? e_true : e_false;
	}else if(is_hashmap(coll)){
		map_t map = (map_t)untag_hashmap(coll);
		return hashmap_length(map)==0 ? e_true : e_false;
	}else if(is_vector(coll)){
		Vector* vec = (Vector*)untag_vector(coll);
		return vec->size == 0 ? e_true : e_false;
	}else if(is_pair(coll)){
		return (car(coll)==e_nil && cdr(coll)==e_nil) ? e_true : e_false;
	}else if(nil(coll)){
		return e_true;
	}else{
		printf("empty? doesn't currently support object %p.\n", coll);
		exit(1);
		return e_nil;
	}
}



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
	//Get all the strings
	char* strings[100];
	int stringsCount = 0;
	int totalSize = 0;
	List* tmp = a;
	while(notNil(tmp)){
		List* obj = car(tmp);
		char* objStr = objToString(obj, 1);
		if(objStr[0]=='"'){objStr = trim_quotes(objStr);}
		strings[stringsCount] = objStr;
		stringsCount++;
		totalSize += strlen(objStr);
		tmp = cdr(tmp);
	}

	//Join all the stirngs
	char* final = newStringFromSize(totalSize);
	strcpy(final, strings[0]);
	for(int i=1;i<stringsCount;++i){
		strcat(final, strings[i]);
	}

	for(int i=0;i<stringsCount;++i){
		free(strings[i]);
	}


	List* ret = (List*)tag_string(final);
	return ret;
}



// ---------------------------------------------
///=Core: Data structures
int listLength(List* a){
	int n = 0;
	List* current = a;
	while(notNil(current)){n++; current = cdr(current);}
	return n;
}

/// (vector values...)
///~Creates a vector
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


/// (to-vector list)
///~Convert a list into a vector
///&vector
///#Vector
///@1val
///!1List
__attribute__((aligned(16))) List* ftovec(List* a){
	Vector* vec = listToVec(first(a));
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
	while(notNil(current)){
		char* key = strdup(first(current));
		List* val = objCopy(second(current));
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
	//if(is_pair(seq)){
	//seq = fvec(seq);
	//}

	//Map
	if (is_hashmap(seq)){	
		map_t map = (map_t)untag_hashmap(seq);
		char* key = (char*)second(a);
		List* value = e_nil;
		hashmap_get(map, key, (any_t)&value);
		return objCopy(value);
	//Vector
	}else if (is_vector(seq)){
		Vector* vec = (Vector*)untag_vector(seq);
		int size = vec->size;
		int pos = (int)numVal(second(a));
		if (pos >= size){
			return e_nil;
		}else{
			void** data = vec->data;
			return objCopy(data[pos]);
		}
	//List
	}else if (is_pair(seq)){
		int size = listLength(seq);
		int pos = (int)numVal(second(a));
		if (pos >= size){
			return e_nil;
		}else{
			List* data = seq;	
			int i = 0;
			while(i<pos){
				data = cdr(data);
				i++;
			}
			return objCopy(car(data));
		}
	}else{
		return e_nil;
	}
}

/// (take n start coll)
///~Take n elements from a collection coll starting from start
///&take
///#List/Vector
///@1n
///!1Number
///@2start
///!2Number
///@3coll
///!3List/Vector
__attribute__((aligned(16))) List* ftake(List* a){
	List* coll = third(a);
	int n = (int)numVal(first(a)); 
	int start = (int)numVal(second(a)); 

	if(is_vector(coll)){
		Vector* vec = (Vector*)untag_vector(coll);
		void** data = vec->data;
		int size = vec->size;

		Vector* new = newVec(n);
		void** newdata = new->data;
		for (int i=0; i<n; ++i) {
			if(i+start >= size){
				newdata[i] = e_nil;
			}else{
				newdata[i] = objCopy(data[i+start]);
			}
		}

		return (List*)tag_vector(new);

	}else if(is_pair(coll)){
		List* ret = e_nil;
		List** tmp = &ret;

		List* current = coll;
		int index = 0;
		while(notNil(current)){
			if(index>=start && index<start+n){
				*tmp = cons(objCopy(first(current)), e_nil);
				tmp = &((List*)untag(*tmp))->next;
			}
			index++;
			current = cdr(current);
		}
		return ret;

	}else{
		printf("Take doesn't support currenct collection %p.\n", coll);
		exit(1);
		return e_nil;
	}
}

/// (drop n coll)
///~Drop n elements from a collection coll
///&drop
///#List/Vector
///@1n
///!1Number
///@2coll
///!2List/Vector
__attribute__((aligned(16))) List* fdrop(List* a){
	List* coll = second(a);
	int n = (int)numVal(first(a)); 

	if(is_vector(coll)){
		Vector* vec = (Vector*)untag_vector(coll);
		void** data = vec->data;
		int size = vec->size;

		Vector* new = newVec(size - n);
		void** newdata = new->data;
		for (int i=n; i<size; ++i) {
			newdata[i-n] = objCopy(data[i]);
		}

		return (List*)tag_vector(new);

	}else if(is_pair(coll)){
		List* ret = e_nil;
		List** tmp = &ret;

		List* current = coll;
		int index = 0;
		while(notNil(current)){
			if(index>=n){
				*tmp = cons(objCopy(first(current)), e_nil);
				tmp = &((List*)untag(*tmp))->next;
			}
			index++;
			current = cdr(current);
		}
		return ret;

	}else{
		printf("Take doesn't support currenct collection %p.\n", coll);
		exit(1);
		return e_nil;
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

	if (nil(arg)){
		return (List*)value_to_number(0);
	}else if (is_vector(arg)){
		Vector* vec = (Vector*)untag_vector(arg);
		int size = vec->size;
		return (List*)value_to_number(size);
	}else if(is_pair(arg)){
		List* l = (List*)untag(arg);
		int size = listLength(l);
		return (List*)value_to_number(size);
	}else{
		printf("%p not supported for count.", arg);
		exit(1);
		return e_nil;
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
		Vector* new = (Vector*)untag_vector(objCopy(coll));

		int size = old->size;
		while(notNil(key_val)){
			int pos = (int)numVal(first(key_val));
			if (pos<size){
				void** data = new->data;
				objFree(data[pos]);
				data[pos] = objCopy(second(key_val));
			}
			key_val = cdr(cdr(key_val));
		}
		return (List*)tag_vector(new);

	}else{
		printf("Assoc not yet implemented for pointer type %p", coll);
		return e_nil;
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
	char* filename = (char*)untag_string(first(a));

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
	return e_nil;
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
	return e_nil;
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
	return e_nil;
}

// (circle context center radius)
///~Draw a circle to a context
///&circle
///#void
///@1c
///!1Context
///@2center
///!2Point
///@3radius
///!3Number
__attribute__((aligned(16))) List* fsvg_circle(List* a){
	cairo_t* context = first(a);
	map_t c = (map_t)untag_hashmap(second(a));
	List *cx, *cy;
	hashmap_get(c, ":x", (any_t)&cx);
	hashmap_get(c, ":y", (any_t)&cy);
	double r = numVal(third(a));
	double x = numVal(cx);
	double y = numVal(cy);
	cairo_move_to(context, x+r, y);
	cairo_arc(context, x, y, r, 0, M_PI*2);
	cairo_stroke(context);
	return e_nil;
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
	char* filename = (char*)untag_string(second(a));//trim_quotes(second(a));
	char fullPath[4096];
	strcpy(fullPath, gispWorkingDir);
	strcat(fullPath, filename);
	printf("Saving image %s\n", fullPath);fflush(stdout);
	cairo_surface_write_to_png(surface, fullPath);
	return e_nil;
}










