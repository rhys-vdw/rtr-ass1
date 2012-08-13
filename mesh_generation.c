#include <stdlib.h>
#include <math.h>
#include "mesh_generation.h"

static const float pi = 3.14159265f;

/**
 * Reallocate given Vertex array to allow for specified number of Vertex
 * structs.
 */
static Vertex *reallocVertexArray(Vertex *vertices, unsigned int length) {
	return (Vertex *) realloc(vertices, length * sizeof(Vertex));
}

static Vertex getPlaneVertex(float size, float u, float v) {
	Vertex vertex;

	vertex.pos.x = u * size;
	vertex.pos.y = v * size;
	vertex.pos.z = 0.0f;

	vertex.normal.x = 0.0f;
	vertex.normal.y = 0.0f;
	vertex.normal.z = 1.0f;

	return vertex;
}

/**
 * Returns a vertex struct with normal and position for torus of specified
 * parameters.
 */
static Vertex getTorusVertex(float R, float r, float u, float v) {
	Vertex vertex;

	/* Use maths rather than physics spherical coordinate convention */
	float theta = u * 2.0f * pi;
	float phi = v * 2.0f * pi;

	float x = (R + r * sin(phi)) * cos(theta);
	float y = (R + r * sin(phi)) * sin(theta);
	float z = r * cos(phi);
	
	vertex.normal.x = r * sin(phi) * cos(theta);
	vertex.normal.y = r * sin(phi) * sin(theta);
	vertex.normal.z = cos(phi);

	vertex.pos.x = x;
	vertex.pos.y = y;
	vertex.pos.z = z;

	return vertex;
}

/**
 * Returns a vertex struct with normal and position for sphere of specified
 * parameters.
 */ 
static Vertex getSphereVertex(float r, float u, float v) {
	Vertex vertex;

	/* Use maths rather than physics spherical coordinate convention */
	float theta = u * 2.0 * pi;
	float phi = v * pi;

	float x = r * cos(theta) * sin(phi);
	float y = r * sin(theta) * sin(phi);
	float z = r * cos(phi);
	
	vertex.normal.x = x / r;
	vertex.normal.y = y / r;
	vertex.normal.z = z / r;

	vertex.pos.x = x;
	vertex.pos.y = y;
	vertex.pos.z = z;

	return vertex;
}

/** Generate vertices for a plane. */
int generatePlaneVertices(Vertex **verticesPtr, float size, int slices,
		int stacks) {
	int i, j;
	int rows = stacks + 1;
	int cols = slices + 1;

	int count = rows * cols;
	*verticesPtr = reallocVertexArray(*verticesPtr, count);
	Vertex *vertices = *verticesPtr;

	for (j = 0; j < rows; j++) {
		float v = j / (float) slices;
		for (i = 0; i < cols; i++) {
			float u = i / (float) stacks;
			int index = j * cols + i;
			vertices[index] = getPlaneVertex(size, u, v);
		}
	}

	return count;
}

/** Generate vertices for a plane. */
int generateTorusVertices(Vertex **verticesPtr, float R, float r, int slices,
		int stacks) {
	int i, j;
	int rows = stacks + 1;
	int cols = slices + 1;

	int count = rows * cols;
	*verticesPtr = reallocVertexArray(*verticesPtr, count);
	Vertex *vertices = *verticesPtr;

	for (j = 0; j < rows; j++) {
		float v = j / (float) stacks;
		for (i = 0; i < cols; i++) {
			float u = i / (float) slices;
			int index = j * cols + i;
			vertices[index] = getTorusVertex(R, r, u, v);
		}
	}

	return count;
}

/** Generate vertices for a plane. */
int generateSphereVertices(Vertex **verticesPtr, float r, int slices,
		int stacks) {
	int i, j;
	int rows = stacks + 1;
	int cols = slices + 1;

	int count = rows * cols;
	*verticesPtr = reallocVertexArray(*verticesPtr, count);
	Vertex *vertices = *verticesPtr;

	for (j = 0; j < rows; j++) {
		float v = j / (float) stacks;
		for (i = 0; i < cols; i++) {
			float u = i / (float) slices;
			int index = j * cols + i;
			vertices[index] = getSphereVertex(r, u, v);
		}
	}

	return count;
}

int generateIndices(int **indicesPtr, int slices, int stacks) {
	int i, j;

	int cols = slices + 1;
	int count = (cols * stacks) * 2;
	*indicesPtr = realloc(*indicesPtr, count * sizeof(int));
	int *indices = *indicesPtr;

	for (j = 0; j < stacks; j++) {
		for (i = 0; i < cols; i++) {
			int index = ((j * cols) + i) * 2;
			indices[index + 0] = j * cols + i;
			indices[index + 1] = (j + 1) * cols + i;
		}
	}

	return count;
}
