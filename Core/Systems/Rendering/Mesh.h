#pragma once

#include <string>
#include <iostream>
#include <vector>

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Vertex.h"
#include "Texture.h"
#include "../Material.h"

class Shader;

class Mesh 
{
public:
	Mesh();
	Mesh(const Mesh& copy);
	Mesh(Mesh&& move);

	Mesh& operator=(const Mesh& assign);

	~Mesh();

	void SetVertices(const std::vector<VertexInfo>& vertices);
	const std::vector<VertexInfo>& GetVerticesConst() const { return m_vertices; }
	std::vector<VertexInfo>& GetVertices() { return m_vertices; }

	void SetIndices(const std::vector<unsigned int>& indices);
	const std::vector<unsigned int>& GetIndicesConst() const { return m_indices; }
	std::vector<unsigned int>& GetIndices() { return m_indices; }

	void SetName(const std::string& name) { m_name = name; }
	const std::string& GetName() const { return m_name; }
	
	void SetMaterial(std::shared_ptr<Material> material) { m_material = material; }
	std::shared_ptr<Material> GetMaterial() { return m_material; }

	void CreateBuffers();

	void Draw();
	void Draw(Material& material);
	void Draw(const Shader& shader);
	void DrawInstanced(const Shader& shader, int instanceCount);

	const unsigned int GetVAO() const { return m_VAO; }
	const unsigned int GetVBO() const { return m_VBO; }
	const unsigned int GetEBO() const { return m_EBO; }

	const unsigned int GetId() const { return m_id; }

private:
	std::string m_name;
	std::shared_ptr<Material> m_material;

	std::vector<VertexInfo> m_vertices;
	std::vector<unsigned int> m_indices;

	unsigned int m_VAO; // vertex array object
	unsigned int m_VBO; // vertex buffer object
	unsigned int m_EBO; // element buffer object

	unsigned int m_id;
	bool m_isReady;

	static unsigned int ID;
};
