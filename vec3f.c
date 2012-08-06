#include <stdio.h>
#include "vec3f.h"

/** Print a vec3f to console in format '(x,y,z)'. */
void printVec3f(vec3f const * const v) {
	printf("(%f,%f,%f)", v->x, v->y, v->z);
}
