
#include "Model.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>


#include "Rendering/Vertex.h"
#include "Rendering/Shader.h"

#include "../Assets/AssetManager.h"

Model::Model()
{

}

Model::Model(const Model& copy)
	: m_meshes(copy.m_meshes)
{
}

Model::Model(Model&& move)
	: m_meshes(move.m_meshes)
{
	
}

Model& Model::operator=(const Model& assign)
{
	m_meshes = assign.m_meshes;
	return *this;
}

Model::~Model()
{
}

void Model::Initialize()
{
	const unsigned int meshCount = static_cast<unsigned int>(m_meshes.size());
	for (unsigned int i = 0; i < meshCount; ++i)
	{
		m_meshes[i]->CreateBuffers();
	}
}

void Model::SetShader(const Shader& shader)
{
	const unsigned int meshCount = static_cast<unsigned int>(m_meshes.size());
	for (unsigned int i = 0; i < meshCount; ++i)
	{
		m_meshes[i]->GetMaterial()->SetShader(shader);
	}
}

void Model::Draw(const glm::mat4& model, const glm::mat4& view, const glm::mat4& projection)
{
	const unsigned int meshCount = static_cast<unsigned int>(m_meshes.size());
	for (unsigned int i = 0; i < meshCount; ++i)
	{
		m_meshes[i]->GetMaterial()->SetMVP(model, view, projection);
		m_meshes[i]->Draw();
	}
}

void Model::Draw(Material& material)
{
	const unsigned int meshCount = static_cast<unsigned int>(m_meshes.size());
	for (unsigned int i = 0; i < meshCount; ++i)
	{
		m_meshes[i]->Draw(material);
	}
}

void Model::Draw(const Shader& shader)
{
	const unsigned int meshCount = static_cast<unsigned int>(m_meshes.size());
	for (unsigned int i = 0; i < meshCount; ++i)
	{
		m_meshes[i]->Draw(shader);
	}
}

void Model::DrawInstanced(const Shader& shader, int instanceCount)
{
	const unsigned int meshCount = static_cast<unsigned int>(m_meshes.size());
	for (unsigned int i = 0; i < meshCount; ++i)
	{
		m_meshes[i]->DrawInstanced(shader, instanceCount);
	}
}

void Model::AddMesh(std::shared_ptr<Mesh> mesh)
{
	m_meshes.push_back(mesh);
}
