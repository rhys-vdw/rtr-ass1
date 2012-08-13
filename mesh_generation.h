#ifndef MESH_GENERATION_H
#define MESH_GENERATION_H

#include <stdbool.h>
#include "vertex.h"

/** Meshes that can be generated. */
enum MeshTypes {
	SPHERE,
	TORUS,
	PLANE,
	NUM_MESH_TYPES
};

/** Set properties for different meshes. */
void setTorusOptions(float R, float r);
void setSphereOptions(float r);
void setPlaneOptions(float w, float h);

/** Set wave properties. */
void setWaveAmplitude(float amplitude);
void setWaveFrequency(float frequency);
float getWaveAmplitude();
float getWaveFrequency();
bool getWaveEnabled();
void setWaveEnabled(bool enabled);

/** Generate triange strip vertices for a shape. */
int generateVertices(Vertex **verticesPtr, int meshType, int slices, int stacks);

/** Generate a general-use array of indices for triangle strips. */
int generateIndices(int **indicesPtr, int slices, int stacks);

#endif /* MESH_GENERATION_H */
