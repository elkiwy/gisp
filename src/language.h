#ifndef LANGUAGE_H
#define LANGUAGE_H

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <sys/time.h>
#include <unistd.h>

#include "core.h"

#ifdef __APPLE__
#include "cairo.h"
#include "cairo-svg.h"
#elif __linux__
#include "cairo/cairo.h"
#include "cairo/cairo-svg.h"
#endif

#include "hashmap.h"

#define PI 3.14159265
#define DEG_TO_RAD PI/180.0
#define RAD_TO_DEG 180.0/PI


extern char gispWorkingDir[4096];

// ------------------------------------------------------------------
// Math
List* fadd(List* a);
List* fsub(List* a);
List* fmul(List* a);
List* fdiv(List* a);

// -----------------------------------------
//Math - Bit-wise operations
List* fbitAnd(List* a);

// -----------------------------------------
//Math - Trigonometry
List* fabsval(List* a);
List* fsin(List* a);
List* fcos(List* a);
List* fdsin(List* a);
List* fdcos(List* a);
List* fdeg(List* a);
List* frad(List* a);
List* fatan2(List* a);

// -----------------------------------------
//Math - Conversions
List* fint(List* a);
List* ffloor(List* a);
List* fceil(List* a);

// -----------------------------------------
//Math - Advanced operations
List* fpow(List* a);
List* fmodulo(List* a);
List* fsign(List* a);
List* fsqrt(List* a);
List* flog(List* a);
List* flog10(List* a);

// -----------------------------------------
//Math - Random
List* frand(List* a);
List* fgauss(List* a);
List* fseed(List* a);

// -----------------------------------------
//Math - Misc
List* frange(List* a);
List* fminNum(List* a);
List* fmaxNum(List* a);
List* fsimplex_noise(List* a);
List* fsimplex(List* a);
List* fmandelbrot(List* a);
List* frescale(List* a);



// ------------------------------------------------------------------
// Sequences 
List* fcons(List* a);
List* flist(List* a);
List* fcar(List* a);
List* fcdr(List* a);
List* fcount(List* a);

// -----------------------------------------
//Sequences - Accessing
List* ffirst(List* a);
List* flast(List* a);
List* fget(List* a);
List* ftake(List* a);
List* fdrop(List* a);

// -----------------------------------------
//Sequences - Manipulations
List* freverse(List* a);
List* fconcat(List* a);
List* fappend(List* a);
List* finsert(List* a);
List* fassoc(List* a);

// -----------------------------------------
//Sequences - Vectors
List* fvec(List* a);
List* ftovec(List* a);

// -----------------------------------------
//Sequences - Hashmaps
List* fhashmap(List* a);



// ------------------------------------------------------------------
// Conditions
List* feq(List* a);
List* flessThan(List* a);
List* fgreaterThan(List* a);
List* flessOrEqThan(List* a);
List* fgreaterOrEqThan(List* a);
List* fnot(List* a);
List* fand(List* a);
List* forr(List* a);

// -----------------------------------------
//Conditions - Type checking
List* fpair(List* a);
List* fnumber(List* a);
List* fatom(List* a);
List* fnull(List* a);
List* fnotnull(List* a);
List* fempty(List* a);



// ------------------------------------------------------------------
// Strings
List* fstr(List* a);



// ------------------------------------------------------------------
// Graphic objects
List* fpoint(List* a);
List* fpointx(List* a);
List* fpointy(List* a);
List* fprintPoint(List* a);
List* fpointvec(List* a);
List* fveclen(List* a);
List* fvecdir(List* a);
List* fpointDistance(List* a);
List* fpointAngle(List* a);
List* fpointBetween(List* a);
List* fpointMoveByVector(List* a);
List* fpointDraw(List* a);
List* fpointsDraw(List* a);
List* flinesDraw(List* a);

List* fline(List* a);
List* flineA(List* a);
List* flineB(List* a);
List* fdrawPath(List* a);
List* fpathSmooth(List* a);



// ------------------------------------------------------------------
// Cairo
typedef struct gisp_frame{
	cairo_t* context;
	cairo_surface_t* surface;
} gisp_frame;

typedef struct gisp_frames{
	gisp_frame** frames;
	int count;
	int active;
} gisp_frames;

List* fframes_make(List* a);
List* fframes_setActive(List* a);
List* fframes_save(List* a);
List* fframes_save_gif(List* a);
List* fframes_clean(List* a);


List* fsvg_surface(List* a);
List* fsvg_status(List* a);
List* fsvg_context(List* a);
List* fsvg_clean(List* a);
List* fsvg_line(List* a);
List* fsvg_circle(List* a);
List* fsvg_to_png(List* a);


List* fprintAddress(List* a);






#endif

