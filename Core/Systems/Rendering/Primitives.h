#pragma once

#include <vector>

#include "Primitives/Quad.h"
#include "Primitives/Cube.h"
#include "Primitives/Sphere.h"

class Primitives
{
public:
	static void RenderPoint(int size = 5);
	static void RenderLine(const glm::vec3& start, const glm::vec3& end);
	static void RenderCube(bool wireframe = false, bool instanced = false, unsigned int count = 0);
	static void RenderQuad(bool wireframe = false, bool instanced = false, unsigned int count = 0);
	static void RenderSphere(bool wireframe = false, bool instanced = false, unsigned int count = 0);

	static Cube cube;
	static Quad quad;
	static Sphere sphere;

	static unsigned int indexCount;
	static unsigned int pointVao, pointVbo;
	static unsigned int lineVao, lineVbo;
};