#ifndef MESH_GENERATION_H
#define MESH_GENERATION_H

#include "vertex.h"

/** Meshes that can be generated. */
enum MeshTypes {
	SPHERE,
	TORUS,
	PLANE,
	NUM_MESH_TYPES
};

/** Generate triangle strip vertices for a sphere. */
int generateSphereVertices(Vertex **verticesPtr, float r, int slices, int stacks);

/** Generate triangle strip vertices for a torus. */
int generateTorusVertices(Vertex **verticesPtr, float R, float r, int slices, int stacks);

/** Generate triangle strip vertices for a plane. */
int generatePlaneVertices(Vertex **verticesPtr, float size, int slices, int stacks);

/** Generate a general-use array of indices for triangle strips. */
int generateIndices(int **indicesPtr, int slices, int stacks);

#endif /* MESH_GENERATION_H */
