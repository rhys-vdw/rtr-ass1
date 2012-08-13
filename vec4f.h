#ifndef VEC4F_H
#define VEC4F_H

/* Three float vector. */
typedef struct {
	float x, y, z, w;
} vec4f;


/** Print a vec4f to console in format '(x,y,z)'. */
void printVec4f(vec4f const * const v);

#endif /* VEC4F_H */
