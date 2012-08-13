#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include <stdbool.h>
#include "mesh_generation.h"

static const float pi = 3.14159265f;

/* torus size */
static float torus_R = 1.0f;
static float torus_r = 0.5f;

/* sphere size */
static float sphere_r = 1.0f;

/* plane size */
static float plane_w = 1.0f;
static float plane_h = 1.0f;

/* wave properties */
static float wave_frequency = 30.00f;
static float wave_amplitude = 0.05f;

/** Set properties for torus. */
void setTorusOptions(float R, float r) {
	torus_R = R;
	torus_r = r;
}

/** Set properties for sphere. */
void setSphereOptions(float r) {
	sphere_r = r;
}

/** Set properties for plane. */
void setPlaneOptions(float w, float h) {
	plane_w = w;
	plane_h = w;
}

/** Set amplitude for wave. */
void setWaveAmplitude(float amplitude) {
	wave_amplitude = amplitude;
}

/** Set frequency for wave. */
void setWaveFrequency(float frequency) {
	wave_frequency = frequency;
}

float getWaveAmplitude() {
	return wave_amplitude;
}

float getWaveFrequency() {
	return wave_frequency;
}

/**
 * Reallocate given Vertex array to allow for specified number of Vertex
 * structs.
 */
static Vertex *reallocVertexArray(Vertex *vertices, unsigned int length) {
	return (Vertex *) realloc(vertices, length * sizeof(Vertex));
}

static void applyWaveToVertex(Vertex *vertex, float u, float v) {
	float y = wave_amplitude * sin(wave_frequency * u * pi)* cos(wave_frequency * v* pi); 
	vec3f change = vec3f_multiply(y, &vertex->normal);
	vertex->pos = vec3f_add(&vertex->pos, &change);
}

static Vertex getPlaneVertex(float u, float v) {
	Vertex vertex;

	vertex.pos.x = u * plane_w;
	vertex.pos.y = v * plane_h;
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
static Vertex getTorusVertex(float u, float v) {
	Vertex vertex;

	/* Use maths rather than physics spherical coordinate convention */
	float theta = u * 2.0f * pi;
	float phi = v * 2.0f * pi;

	float x = (torus_R + torus_r * sin(phi)) * cos(theta);
	float y = (torus_R + torus_r * sin(phi)) * sin(theta);
	float z = torus_r * cos(phi);
	
	vertex.normal.x = torus_r * sin(phi) * cos(theta);
	vertex.normal.y = torus_r * sin(phi) * sin(theta);
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
static Vertex getSphereVertex(float u, float v) {
	Vertex vertex;

	/* Use maths rather than physics spherical coordinate convention */
	float theta = u * 2.0 * pi;
	float phi = v * pi;

	float x = sphere_r * cos(theta) * sin(phi);
	float y = sphere_r * sin(theta) * sin(phi);
	float z = sphere_r * cos(phi);
	
	vertex.normal.x = x / sphere_r;
	vertex.normal.y = y / sphere_r;
	vertex.normal.z = z / sphere_r;

	vertex.pos.x = x;
	vertex.pos.y = y;
	vertex.pos.z = z;

	return vertex;
}

typedef Vertex (*VertexFptr) (float, float);

VertexFptr vertexFptrs[] = {
	&getSphereVertex,
	&getTorusVertex,
	&getPlaneVertex };

/** Generate triange strip vertices for a shape. */
int generateVertices(Vertex **verticesPtr, int meshType, int slices,
		int stacks) {
	int i, j;
	int rows = stacks + 1;
	int cols = slices + 1;

	int count = rows * cols;
	*verticesPtr = reallocVertexArray(*verticesPtr, count);
	Vertex *vertices = *verticesPtr;

	VertexFptr vertexFptr = vertexFptrs[meshType];

	for (j = 0; j < rows; j++) {
		float v = j / (float) stacks;
		for (i = 0; i < cols; i++) {
			float u = i / (float) slices;
			int index = j * cols + i;
			vertices[index] = vertexFptr(u, v);
			applyWaveToVertex(&vertices[index], u, v);
		}
	}

	return count;
}

int generateIndices(int **indicesPtr, int slices, int stacks) {
	int i, j;

	/* rows and columns of vertices (posts) */
	int cols = slices + 1;

	/* two degenerate vertices per row */
	int degenerates = stacks * 2;

	/* total vertices */
	int count = (cols * stacks) * 2 + degenerates;

	/* allocate array */
	*indicesPtr = realloc(*indicesPtr, count * sizeof(int));
	int *indices = *indicesPtr;

	/* initialize */
	int index = 0;
	for (j = 0; j < stacks; j++) {
		indices[index] = j * cols;
		index++;
		for (i = 0; i < cols; i++) {
			indices[index + 0] = j * cols + i;
			indices[index + 1] = (j + 1) * cols + i;
			index += 2;
		}
		indices[index] = indices[index - 1];
		index++;
	}

	return count;
}
