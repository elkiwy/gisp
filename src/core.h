#ifndef CORE_H
#define CORE_H

#include <stdlib.h>
#include <string.h>

#define SYMBOL_MAX  32

//This is the main List structure
typedef struct List {
	struct List*  next;
	void*  data;
}List;

//Each cons shell is tagged with the lowest pointer bit set to 1, everything else is set to 0
//Before accessing cons car and cdr we need to untag the pointer to read from memory correctly
#define is_pair(x) (((uintptr_t)x & 0x1) == 0x1)
#define is_atom(x) (((uintptr_t)x & 0x1) == 0x0)
#define untag(x)   ((uintptr_t) x & ~0x1)
#define tag(x)     ((uintptr_t) x | 0x1)
#define car(x)     (((List*)untag(x))->data)
#define cdr(x)     (((List*)untag(x))->next)

//Handy list shortcuts
#define first(x)   car(x)
#define second(x)  car(cdr(x))
#define third(x)   car(cdr(cdr(x)))
#define fourth(x)  car(cdr(cdr(cdr(x))))
#define fifth(x)   car(cdr(cdr(cdr(cdr(x)))))

List* cons(void* _car, void* _cdr);
void* intern(char* sym);

//Define what is true and what is false
#define e_true     cons( intern("quote"), cons( intern("t"), 0))
#define e_false    0


#endif