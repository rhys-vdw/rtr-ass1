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

/** Generate triangle strip vertices for a plane. */
int generatePlaneVertices(Vertex **verticesPtr, float size, int divisions) {
	int i, j;
	float u, v, u1, v1;

	Vertex *vertices;

	int count = divisions * divisions * 4;
	*verticesPtr = reallocVertexArray(*verticesPtr, count);
	vertices = *verticesPtr;

	for (j = 0; j < divisions; j++) {
		v = j / (float) divisions;
		v1 = (j + 1) / (float) divisions;
		for (i = 0; i < divisions; i++) {
			u = i / (float) divisions;
			u1 = (i + 1) / (float) divisions;

			int index = ((j * divisions) + i) * 4;
			vertices[index + 0] = getPlaneVertex(size, u, v);
			vertices[index + 1] = getPlaneVertex(size, u, v1);

			vertices[index + 2] = getPlaneVertex(size, u1, v);
			vertices[index + 3] = getPlaneVertex(size, u1, v1);
		}
	}

	return count;
}

/** Generate triangle strip vertices for a torus. */
int generateTorusVertices(Vertex **verticesPtr, float R, float r, int slices, int stacks) {
	int i, j;
	float u, v, u1, v1;

	Vertex *vertices;

	int count = slices * stacks * 4;
	*verticesPtr = reallocVertexArray(*verticesPtr, count);
	vertices = *verticesPtr;

	for (j = 0; j < stacks; j++) {
		v = j / (float) stacks;
		v1 = (j + 1) / (float) stacks;
		for (i = 0; i < slices; i++) {
			u = i / (float) slices;
			u1 = (i + 1) / (float) slices;

			int index = ((j * slices) + i) * 4;
			vertices[index + 0] = getTorusVertex(R, r, u, v);
			vertices[index + 1] = getTorusVertex(R, r, u, v1);

			vertices[index + 2] = getTorusVertex(R, r, u1, v);
			vertices[index + 3] = getTorusVertex(R, r, u1, v1);
		}
	}

	return count;
}

int generateSphereVertices(Vertex **verticesPtr, float r, int slices, int stacks) {
	int i, j;
	float u, v, u1, v1;

	Vertex *vertices;

	int count = slices * stacks * 4;
	*verticesPtr = reallocVertexArray(*verticesPtr, count);
	vertices = *verticesPtr;

	for (j = 0; j < stacks; j++) {
		v = j / (float) stacks;
		v1 = (j + 1) / (float) stacks;
		for (i = 0; i < slices; i++) {
			u = i / (float) slices;
			u1 = (i + 1) / (float) slices;

			int index = ((j * slices) + i) * 4;
			vertices[index + 0] = getSphereVertex(r, u, v);
			vertices[index + 1] = getSphereVertex(r, u, v1);

			vertices[index + 2] = getSphereVertex(r, u1, v);
			vertices[index + 3] = getSphereVertex(r, u1, v1);
		}
	}

	return count;
}
