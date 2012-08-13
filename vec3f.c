#include <stdio.h>
#include "vec3f.h"

/** Print a vec3f to console in format '(x,y,z)'. */
void printVec3f(vec3f const * const v) {
	printf("(%f,%f,%f)", v->x, v->y, v->z);
}

/** Multiply vec3f by scalar. */
vec3f vec3f_multiply(float s, const vec3f *v) {
	vec3f result = *v;
	result.x *= s;
	result.y *= s;
	result.z *= s;
	return result;
}

vec3f vec3f_add(const vec3f *a, const vec3f *b) {
	vec3f result = *a;
	result.x += b->x;
	result.y += b->y;
	result.z += b->z;
	return result;
}
