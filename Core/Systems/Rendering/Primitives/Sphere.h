#pragma once

#include <vector>
#include <glm/glm.hpp>
#include <GL/glew.h>

class Sphere
{
public:
	Sphere();
	Sphere(unsigned int xDiv, unsigned int yDiv);

	void Init();
	void Draw();
	void DrawInstanced(unsigned int count);

	unsigned int GetIndexCount() const { return static_cast<unsigned int>(indices.size()); }
	unsigned int GetVertexCount() const { return static_cast<unsigned int>(GetIndexCount() * 3 / 4); }

	const unsigned int GetVAO() const { return m_VAO; }

private:
	unsigned int xDiv;
	unsigned int yDiv;

	unsigned int m_VAO;
	unsigned int m_VBO;
	unsigned int m_EBO;

	std::vector<glm::vec3> positions;
	std::vector<glm::vec2> uv;
	std::vector<glm::vec3> normals;
	std::vector<unsigned int> indices;
};