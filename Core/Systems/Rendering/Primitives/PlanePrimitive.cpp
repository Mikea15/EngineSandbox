#include "PlanePrimitive.h"

#include <GL/glew.h>

PlanePrimitive::PlanePrimitive()
	: m_VAO(0)
	, m_VBO(0)
{

}

void PlanePrimitive::Init()
{
	// set up vertex data (and buffer(s)) and configure vertex attributes
		// ------------------------------------------------------------------
	float planeVertices[] = {
		// positions            // normals         // texcoords
		 25.0f, 0.0f,  25.0f,  0.0f, 1.0f, 0.0f,  25.0f,  0.0f,
		-25.0f, 0.0f,  25.0f,  0.0f, 1.0f, 0.0f,   0.0f,  0.0f,
		-25.0f, 0.0f, -25.0f,  0.0f, 1.0f, 0.0f,   0.0f, 25.0f,

		 25.0f, 0.0f,  25.0f,  0.0f, 1.0f, 0.0f,  25.0f,  0.0f,
		-25.0f, 0.0f, -25.0f,  0.0f, 1.0f, 0.0f,   0.0f, 25.0f,
		 25.0f, 0.0f, -25.0f,  0.0f, 1.0f, 0.0f,  25.0f, 25.0f
	};
	// plane VAO
	glGenVertexArrays(1, &m_VAO);
	glGenBuffers(1, &m_VBO);
	glBindVertexArray(m_VAO);
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), planeVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glBindVertexArray(0);
}

void PlanePrimitive::Draw()
{
	if (m_VAO == 0)
	{
		Init();
	}
	glBindVertexArray(m_VAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
}

void PlanePrimitive::DrawInstanced(unsigned int count)
{
	if (m_VAO == 0)
	{
		Init();
	}
	glBindVertexArray(m_VAO);
	glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, count);
	glBindVertexArray(0);
}
