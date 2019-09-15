#ifndef LANGUAGE_H
#define LANGUAGE_H

#include <stdlib.h>
#include <stdio.h>
#include "core.h"
#include "cairo.h"
#include "cairo-svg.h"
#include "hashmap.h"

//Math
List* fadd(List* a);
List* fsub(List* a);
List* fmul(List* a);
List* fdiv(List* a);
List* frange(List* a);

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

//Strings
List* fstr(List* a);

//Data structure
List* fvec(List* a);
List* fhashmap(List* a);
List* fget(List* a);

//Cairo
List* fsvg_surface(List* a);
List* fsvg_status(List* a);
List* fsvg_context(List* a);
List* fsvg_clean(List* a);
List* fsvg_line(List* a);


#endif

