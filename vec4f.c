#include <stdio.h>
#include "vec4f.h"

/** Print a vec4f to console in format '(x,y,z)'. */
void printVec4f(vec4f const * const v) {
	printf("(%f,%f,%f,%f)", v->x, v->y, v->z, v->w);
}
