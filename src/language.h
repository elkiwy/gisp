#ifndef LANGUAGE_H
#define LANGUAGE_H

#include <stdlib.h>
#include <stdio.h>
#include "core.h"

//Math
List* fadd(List* a);
List* fsub(List* a);
List* fmul(List* a);
List* fdiv(List* a);

//List operations
List* fcons(List* a);
List* flist(List* a);
List* fcar(List* a);
List* fcdr(List* a);

//Tests and type checking
List* feq(List* a);
List* fpair(List* a);
List* fatom(List* a);
List* fnull(List* a);

#endif

