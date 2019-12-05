#pragma once

#include <string>
#include <iostream>
#include <vector>
#include <memory>
#include <unordered_map>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Assets/AssetId.h"

#include "Systems/Rendering/Mesh.h"
#include "Renderer/Light.h"

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
	void SetShader(Shader* shader);

	void Draw(const glm::mat4& model, const glm::mat4& view, const glm::mat4& projection);
	void Draw(const glm::mat4& model, const glm::mat4& view, const glm::mat4& projection, Material material);
	void Draw(const glm::mat4& model, const glm::mat4& view, const glm::mat4& projection, const Shader* shader);

	void AddMesh(Mesh mesh);
	std::vector<Mesh>& GetMeshes() { return m_meshes; }
	unsigned int GetMeshCount() const { return static_cast<unsigned int>(m_meshes.size()); }

	void AddMaterial(Material material);
	std::vector<Material>& GetMaterials() { return m_materials; }
	unsigned int GetMaterialCount() const { return static_cast<unsigned int>(m_materials.size()); }

	void SetMaterialOverride(Material material) { m_materialOverride = std::move(material); }

	void ApplyLight(ILight& light);

	AssetId GetId() const { return m_id; }
	bool IsValid() const { return m_id.IsValid(); }

private:
	AssetId m_id;

	std::vector<Mesh> m_meshes;
	std::vector<Material> m_materials;

	Material m_materialOverride;
};
