
#include "Sphere.h"

Sphere::Sphere()
	: Sphere(32, 32)
{

}

Sphere::Sphere(unsigned int xDiv, unsigned int yDiv)
	: m_VAO(0)
	, m_VBO(0)
	, xDiv(xDiv)
	, yDiv(yDiv)
{

}

void Sphere::Init()
{
	const float PI = 3.14159265359f;
	for (unsigned int y = 0; y <= yDiv; ++y)
	{
		for (unsigned int x = 0; x <= xDiv; ++x)
		{
			const float xSegment = static_cast<float>(x / static_cast<float>(xDiv));
			const float ySegment = static_cast<float>(y / static_cast<float>(yDiv));

			const float xPos = std::cos(xSegment * 2.0f * PI) * std::sin(ySegment * PI);
			const float yPos = std::cos(ySegment * PI);
			const float zPos = std::sin(xSegment * 2.0f * PI) * std::sin(ySegment * PI);

			positions.push_back(glm::vec3(xPos, yPos, zPos));
			uv.push_back(glm::vec2(xSegment, ySegment));
			normals.push_back(glm::vec3(xPos, yPos, zPos));
		}
	}

	bool oddRow = false;
	for (unsigned int y = 0; y < yDiv; ++y)
	{
		if (!oddRow) // even rows: y == 0, y == 2; and so on
		{
			for (int x = 0; x <= xDiv; ++x)
			{
				indices.push_back(y       * (xDiv + 1) + x);
				indices.push_back((y + 1) * (xDiv + 1) + x);
			}
		}
		else
		{
			for ( int x = xDiv; x >= 0; --x)
			{
				indices.push_back((y + 1) * (xDiv + 1) + x);
				indices.push_back(y       * (xDiv + 1) + x);
			}
		}
		oddRow = !oddRow;
	}

	std::vector<float> data;
	const unsigned int posSize = static_cast<unsigned int>(positions.size());
	for (unsigned int i = 0; i < posSize; ++i)
	{
		data.push_back(positions[i].x);
		data.push_back(positions[i].y);
		data.push_back(positions[i].z);
		if (uv.size() > 0)
		{
			data.push_back(uv[i].x);
			data.push_back(uv[i].y);
		}
		if (normals.size() > 0)
		{
			data.push_back(normals[i].x);
			data.push_back(normals[i].y);
			data.push_back(normals[i].z);
		}
	}

	glGenVertexArrays(1, &m_VAO);

	glGenBuffers(1, &m_VBO);
	glGenBuffers(1, &m_EBO);

	glBindVertexArray(m_VAO);
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), &data[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
	
	GLsizei stride = (3 + 2 + 3) * sizeof(float);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride, (void*)(5 * sizeof(float)));
}

void Sphere::Draw()
{
	if (m_VAO == 0)
	{
		Init();
	}

	glBindVertexArray(m_VAO);
	glDrawElements(GL_TRIANGLE_STRIP, GetIndexCount(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

void Sphere::DrawInstanced(unsigned int count)
{
	if (m_VAO == 0)
	{
		Init();
	}

	glBindVertexArray(m_VAO);
	glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, count, GetIndexCount());
	glBindVertexArray(0);
}
