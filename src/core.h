#ifndef CORE_H
#define CORE_H

#include <stdlib.h>
#include <string.h>


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


List* cons(void* _car, void* _cdr);
void* intern(char* sym);



#endif
