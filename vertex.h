#ifndef VERTEX_H
#define VERTEX_H

#include "vec3f.h"

/** Holds information for a single mesh vertex. */
typedef struct {
	vec3f normal;
	vec3f pos;
} Vertex;

#endif /* VERTEX_H */
