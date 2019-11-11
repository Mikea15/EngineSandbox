#pragma once

#include <string>
#include <iostream>
#include <vector>
#include <memory>
#include <unordered_map>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Rendering/Mesh.h"
#include "Light.h"

class AssetManager;
class Shader;

class Model
{
public:
	Model();
	Model(const Model& copy);
	Model(Model&& move);
	Model& operator=(const Model& assign); 
	~Model();

	void Initialize();
	void SetShader(const Shader& shader);

	void Draw(const glm::mat4& model, const glm::mat4& view, const glm::mat4& projection);
	void Draw(const glm::mat4& model, const glm::mat4& view, const glm::mat4& projection, Material& material);

	void AddMesh(std::shared_ptr<Mesh> mesh);
	std::vector<std::shared_ptr<Mesh>>& GetMeshes() { return m_meshes; }
	unsigned int GetMeshCount() const { return static_cast<unsigned int>(m_meshes.size()); }

	void AddMaterial(std::shared_ptr<Material> material);
	std::vector<std::shared_ptr<Material>>& GetMaterials() { return m_materials; }
	unsigned int GetMaterialCount() const { return static_cast<unsigned int>(m_materials.size()); }

	void SetMaterialOverride(std::shared_ptr<Material> material) { m_materialOverride = std::move(material); }

	void ApplyLight(ILight& light);

private:
	std::vector<std::shared_ptr<Mesh>> m_meshes;
	std::vector<std::shared_ptr<Material>> m_materials;

	std::shared_ptr<Material> m_materialOverride;
};
