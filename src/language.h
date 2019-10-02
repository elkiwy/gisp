#ifndef LANGUAGE_H
#define LANGUAGE_H

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>

#include "core.h"
#include "cairo.h"
#include "cairo-svg.h"
#include "hashmap.h"

#define PI 3.14159265
#define DEG_TO_RAD PI/180.0
#define RAD_TO_DEG 180.0/PI

//Math
List* fadd(List* a);
List* fsub(List* a);
List* fmul(List* a);
List* fdiv(List* a);
List* frange(List* a);

List* fsin(List* a);
List* fcos(List* a);
List* fdsin(List* a);
List* fdcos(List* a);
List* fdeg(List* a);
List* frad(List* a);
List* fint(List* a);

List* flessThan(List* a);
List* fgreaterThan(List* a);

List* fpow(List* a);
List* fmodulo(List* a);
List* fsqrt(List* a);
List* fatan2(List* a);
List* frand(List* a);
List* fseed(List* a);

//List operations
List* fcons(List* a);
List* flist(List* a);
List* fcar(List* a);
List* fcdr(List* a);
List* freverse(List* a);
List* fconcat(List* a);
List* ffirst(List* a);
List* flast(List* a);

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
List* fcount(List* a);
List* fassoc(List* a);

//Cairo
List* fsvg_surface(List* a);
List* fsvg_status(List* a);
List* fsvg_context(List* a);
List* fsvg_clean(List* a);
List* fsvg_line(List* a);


#endif

