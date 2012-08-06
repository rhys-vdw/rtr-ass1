#ifndef MESH_GENERATION_H
#define MESH_GENERATION_H

#include "vertex.h"

/** Generate triangle strip vertices for a sphere. */
int generateSphereVertices(Vertex **verticesPtr, float r, int slices, int stacks);
/** Generate triangle strip vertices for a torus. */
int generateTorusVertices(Vertex **verticesPtr, float R, float r, int slices, int stacks);
/** Generate triangle strip vertices for a plane. */
int generatePlaneVertices(Vertex **verticesPtr, float size, int divisions);

#endif /* MESH_GENERATION_H */
