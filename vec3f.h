#ifndef VEC3F_H
#define VEC3F_H

/* Three float vector. */
typedef struct {
	float x, y, z;
} vec3f;


/** Print a vec3f to console in format '(x,y,z)'. */
void printVec3f(vec3f const * const v);

#endif /* VEC3F_H */
