#ifndef VEC3F_H
#define VEC3F_H

/* Three float vector. */
typedef struct {
	float x, y, z;
} vec3f;


/** Print a vec3f to console in format '(x,y,z)'. */
void printVec3f(vec3f const * const v);

/** Multiply vec3f by scalar. */
vec3f vec3f_multiply(float s, const vec3f *v);

/** Add two vec3fs. */
vec3f vec3f_add(const vec3f *a, const vec3f *b);

#endif /* VEC3F_H */
