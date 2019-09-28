#ifndef CORE_H
#define CORE_H

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "hashmap.h"

#define SYMBOL_MAX  32


//This is the main List structure
typedef struct List {
	struct List*  next;
	void*  data;
}List;


//This is the layered environment structure
typedef struct Environment {
	struct Environment* outer;
	List* data;
}Environment;

//Each cons shell is tagged with the lowest pointer bit set to 1, everything else is set to 0
//Before accessing cons car and cdr we need to untag the pointer to read from memory correctly
#define is_hashmap(x) (((uintptr_t)x & 0x4) == 0x4)
#define is_vector(x)  (((uintptr_t)x & 0x3) == 0x3)
#define is_number(x)  (((uintptr_t)x & 0x2) == 0x2)
#define is_pair(x)    (((uintptr_t)x & 0x1) == 0x1)
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

//Handy list shortcuts
#define first(x)   car(x)
#define second(x)  car(cdr(x))
#define third(x)   car(cdr(cdr(x)))
#define fourth(x)  car(cdr(cdr(cdr(x))))
#define fifth(x)   car(cdr(cdr(cdr(cdr(x)))))

//Utility
void print_obj(List* ob, int head_of_list);
char* trim_quotes(char* s);
int vecLength(void** v);

//Core
List* cons(void* _car, void* _cdr);
void* intern(char* sym);
Environment* makeEnvironment(List* _data, Environment* _outer);

//Numbers 
double* symbol_to_number(char* sym);
double* value_to_number(double value);
double numVal(List* tagged_number);

//Define what is true and what is false
#define e_true     cons( intern("quote"), cons( intern("t"), 0))
#define e_false    0


#endif
