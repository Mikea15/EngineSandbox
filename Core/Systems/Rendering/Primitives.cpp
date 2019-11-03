#include "Primitives.h"

#include <glm/glm.hpp>
#include <gl/glew.h>

Cube Primitives::cube = Cube();
Quad Primitives::quad = Quad();
Sphere Primitives::sphere = Sphere();

unsigned int Primitives::indexCount = 0;
unsigned int Primitives::pointVao = 0;
unsigned int Primitives::pointVbo = 0;
unsigned int Primitives::lineVao = 0;
unsigned int Primitives::lineVbo = 0;


void Primitives::RenderPoint(int size)
{
	if (pointVao == 0)
	{
		float point[3] = { 0.0f, 0.0f, 0.0f };
		// Setup
		glGenVertexArrays(1, &pointVao);
		glGenBuffers(1, &pointVbo);

		// fill buffer
		glBindBuffer(GL_ARRAY_BUFFER, pointVbo);
		glBufferData(GL_ARRAY_BUFFER, 3 * sizeof(float), &point, GL_STATIC_DRAW);

		// link vertex attributes
		glBindVertexArray(pointVao);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	}
	glPointSize(static_cast<GLfloat>(size));
	glBindVertexArray(pointVao);
	glDrawArrays(GL_POINTS, 0, 1);
	glBindVertexArray(0);
}

void Primitives::RenderLine(const glm::vec3& start, const glm::vec3& end)
{
	//if (lineVao == 0)
	//{
		float line[6] = { 
			start.x, start.y, start.z,
			end.x, end.y, end.z 
		};

		// Setup
		glGenVertexArrays(1, &lineVao);
		glGenBuffers(1, &lineVbo);

		// fill buffer
		glBindBuffer(GL_ARRAY_BUFFER, lineVbo);
		glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(float), &line, GL_STATIC_DRAW);

		// link vertex attributes
		glBindVertexArray(lineVao);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	//}

	glLineWidth(1);
	glBindVertexArray(lineVao);
	glDrawArrays(GL_LINES, 0, 2);
	glBindVertexArray(0);
}

void Primitives::RenderCube(bool wireframe, bool instanced, unsigned int count)
{
	if (wireframe)
	{
		glLineWidth(1);
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}

	cube.Draw();

	if (wireframe)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
}

void Primitives::RenderQuad(bool wireframe, bool instanced, unsigned int count)
{
	if (wireframe)
	{
		glLineWidth(1);
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}

	if (!instanced)
	{
		quad.Draw();
	}
	else
	{
		quad.DrawInstanced(count);
	}

	if (wireframe)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
}

void Primitives::RenderSphere(bool wireframe, bool instanced, unsigned int count)
{
	if (wireframe)
	{
		glLineWidth(1);
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}

	if (!instanced)
	{
		sphere.Draw();
	}
	else
	{
		sphere.DrawInstanced(count);
	}

	
	if (wireframe)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
}
