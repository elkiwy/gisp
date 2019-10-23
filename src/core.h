#ifndef CORE_H
#define CORE_H

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>

#include "hashmap.h"

#define SYMBOL_MAX  32


extern int vectorCount;
extern int hashmapCount;
extern int numberCount;
extern int consCount;

extern int environmentCounter_insert;
extern int environmentCounter_search;
extern double environmentCounter_searchTimeSum;
extern double environmentCounter_searchTimeSum_hash;

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

// ------------------------------------------------------------------
//Each cons shell is tagged with the lowest pointer bit set to 1, everything else is set to 0
//Before accessing cons car and cdr we need to untag the pointer to read from memory correctly
//This tecnique is used to tag numbers, vectors, and hashmaps too.
#define is_hashmap(x) (((uintptr_t)x & 0x4) == 0x4)
#define is_vector(x)  (((uintptr_t)x & 0x3) == 0x3)
#define is_number(x)  (((uintptr_t)x & 0x2) == 0x2)
#define is_pair(x)    (((uintptr_t)x & 0x2) != 0x2) && (((uintptr_t)x & 0x1) == 0x1)
#define is_atom(x)    (((uintptr_t)x & 0x1) == 0x0)
#define untag_hashmap(x) ((uintptr_t) x & ~0x4)
#define tag_hashmap(x)   ((uintptr_t) x | 0x4)
#define untag_vector(x)  ((uintptr_t) x & ~0x3)
#define tag_vector(x)    ((uintptr_t) x | 0x3)
#define untag_number(x)  ((uintptr_t) x & ~0x2)
#define tag_number(x)    ((uintptr_t) x | 0x2)
#define untag(x)         ((uintptr_t) x & ~0x1)
#define tag(x)           ((uintptr_t) x | 0x1)
#define car(x)     (((List*)untag(x))->data)
#define cdr(x)     (((List*)untag(x))->next)

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

// ------------------------------------------------------------------
//String utilities
char* trim_quotes(char* s);

// ------------------------------------------------------------------
//Hashmap utilites
map_t newHashmap();

// ------------------------------------------------------------------
//Vector utilities
void** newVec(int size);
void** listToVec(List* l);
void** copyVec(void** v);
List* vecToList(void** vec);
int vecLength(void** v);

// ------------------------------------------------------------------
//Numbers utilites
double* newNumber();
double* symbol_to_number(char* sym);
double* value_to_number(double value);
double numVal(List* tagged_number);

// ------------------------------------------------------------------
//Lisp core functions
List* cons(void* _car, void* _cdr);
void* intern(char* sym);
Environment* makeEnvironment(Environment* _outer);
void extendEnv(char* name, void* value, Environment* env);
void* searchInEnvironment(List* exp, Environment* env);

// ------------------------------------------------------------------
//Define what is true and what is false
#define e_true     cons( intern("quote"), cons( intern("t"), 0))
#define e_false    0


#endif
