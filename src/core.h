#ifndef CORE_H
#define CORE_H

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>

#include "hashmap.h"

#define SYMBOL_MAX  128

extern int debugPrintInfo;
extern int debugPrintFrees;
extern int debugPrintCopy;
extern int debugPrintAllocs;

extern int vectorCount;
extern int hashmapCount;
extern int numberCount;
extern int consCount;

extern int environmentCounter_insert;
extern int environmentCounter_search;
extern double environmentCounter_searchTimeSum;
extern double environmentCounter_searchTimeSum_hash;

extern void* allocations[1024*1024];

//This is the main List structure
typedef struct List {
	struct List*  next;
	void*  data;
}List;


//This is the layered environment structure
typedef struct Environment {
	struct Environment* outer;
	map_t hashData;
}Environment;


//Custom Vector structure
typedef struct Vector {
	void* data;
	int size;
}Vector;




// ------------------------------------------------------------------
//Each cons shell is tagged with the lowest pointer bit set to 1, everything else is set to 0
//Before accessing cons car and cdr we need to untag the pointer to read from memory correctly
//This tecnique is used to tag numbers, vectors, and hashmaps too.
#define is_string(x)     (((uintptr_t)x & 0x5) == 0x5)
#define is_hashmap(x)    (((uintptr_t)x & 0x4) == 0x4)
#define is_vector(x)     (((uintptr_t)x & 0x3) == 0x3)
#define is_number(x)     (((uintptr_t)x & 0x2) == 0x2)
#define is_pair(x)       (((uintptr_t)x & 0x1) == 0x1) && (((uintptr_t)x & 0x2) != 0x2)
#define is_atom(x)       (((uintptr_t)x & 0x1) == 0x0) && (((uintptr_t)x & 0x2) != 0x2) && (((uintptr_t)x & 0x4) != 0x4)
#define untag_string(x)  ((uintptr_t) x & ~0x5)
#define tag_string(x)    ((uintptr_t) x | 0x5)
#define untag_hashmap(x) ((uintptr_t) x & ~0x4)
#define tag_hashmap(x)   ((uintptr_t) x | 0x4)
#define untag_vector(x)  ((uintptr_t) x & ~0x3)
#define tag_vector(x)    ((uintptr_t) x | 0x3)
#define untag_number(x)  ((uintptr_t) x & ~0x2)
#define tag_number(x)    ((uintptr_t) x | 0x2)
#define untag(x)         ((uintptr_t) x & ~0x1)
#define tag(x)           ((uintptr_t) x | 0x1)
#define car(x)           (((List*)untag(x))->data)
#define cdr(x)           (((List*)untag(x))->next)

// ------------------------------------------------------------------
//Handy list shortcuts
#define first(x)   car(x)
#define second(x)  car(cdr(x))
#define third(x)   car(cdr(cdr(x)))
#define fourth(x)  car(cdr(cdr(cdr(x))))
#define fifth(x)   car(cdr(cdr(cdr(cdr(x)))))

// ------------------------------------------------------------------
//Generic utility
void print_obj(List* ob, int head_of_list);
int randInt(int min, int max);
void debugPrintObj(char* pre, List* obj);
char* objToString(List* ob, int head_of_list);

// ------------------------------------------------------------------
//String utilities
char* trim_quotes(char* s);

// ------------------------------------------------------------------
//Hashmap utilites
map_t newHashmap();

// ------------------------------------------------------------------
//Vector utilities
Vector* newVec(int size);
Vector* listToVec(List* l);
List* vecToList(Vector* vec);

// ------------------------------------------------------------------
//Cons utilities
List* listGetLastCons(List* l);
void consSetNext(List* l, List* _next);
void consSetData(List* l, void* _data);

// ------------------------------------------------------------------
//Copy functions
List* objCopy(List* obj);
List* numberCopy(List* num);
List* listCopy(List* l);
List* hashmapCopy(List* hashmap);
List* vectorCopy(List* v);
List* stringCopy(List* s);

// ------------------------------------------------------------------
//Free functions
void consFree(List* c);
void listFree(List* l);
void listFreeOnlyCons(List* l);
void objFree(List* obj);
void numberFree(List* number);
void environmentFree(Environment* env);
void hashmapFree(List* hashmap);
void vectorFree(List* v);
void stringFree(List* s);

// ------------------------------------------------------------------
//debug utility
void debug_addAllocation(void* p);
void debug_removeAllocation(void* p);
void debug_printAllocations();

// ------------------------------------------------------------------
//Numbers utilites
double* newNumber();
double* symbol_to_number(char* sym);
double* value_to_number(double value);
double numVal(List* tagged_number);

// ------------------------------------------------------------------
//String utilites
char* newStringFromText(char* t);
char* newStringFromSize(int n);

// ------------------------------------------------------------------
//Lisp core functions
List* cons(void* _car, void* _cdr);
void* intern(char* sym);
Environment* makeEnvironment(Environment* _outer);
void extendEnv(char* name, void* value, Environment* env);
void* searchInEnvironment(List* exp, Environment* env);

// ------------------------------------------------------------------
//Define what is true and what is false
#define e_true     intern("true")
#define e_false    intern("nil")


#endif
