
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
	, m_materials(copy.m_materials)
	, m_materialOverride(copy.m_materialOverride)
{
}

Model::Model(Model&& move)
	: m_meshes(move.m_meshes)
	, m_materials(move.m_materials)
	, m_materialOverride(move.m_materialOverride)
{
	
}

Model& Model::operator=(const Model& assign)
{
	m_meshes = assign.m_meshes;
	m_materials = assign.m_materials;
	m_materialOverride = assign.m_materialOverride;
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
	const unsigned int materialCount = static_cast<unsigned int>(m_materials.size());
	for (unsigned int i = 0; i < materialCount; ++i)
	{
		m_materials[i]->SetShader(shader);
	}
}

void Model::Draw(const glm::mat4& model, const glm::mat4& view, const glm::mat4& projection)
{
	const unsigned int meshCount = static_cast<unsigned int>(m_meshes.size());

	if (m_materialOverride != nullptr)
	{
		m_materialOverride->SetMVP(model, view, projection);
		m_materialOverride->BindTextures();
	}

	for (unsigned int i = 0; i < meshCount; ++i)
	{
		if (m_materialOverride != nullptr)
		{
			m_meshes[i]->Draw();
		}
		else
		{
			unsigned int matIndex = m_meshes[i]->GetMaterialIndex();
			const bool hasMaterial = matIndex >= 0 && matIndex < m_materials.size();
			if (hasMaterial)
			{
				Material& matRef = *m_materials[matIndex];

				matRef.SetMVP(model, view, projection);
				matRef.BindTextures();
				m_meshes[i]->Draw();
			}
		}
	}
}

void Model::Draw(Material& material)
{
	const unsigned int meshCount = static_cast<unsigned int>(m_meshes.size());
	for (unsigned int i = 0; i < meshCount; ++i)
	{
		material.BindTextures();
		m_meshes[i]->Draw();
	}
}

void Model::AddMesh(std::shared_ptr<Mesh> mesh)
{
	m_meshes.push_back(mesh);
}

void Model::AddMaterial(std::shared_ptr<Material> material)
{
	m_materials.push_back(material);
}
