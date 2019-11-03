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
	void Draw(Material& material);
	void Draw(const Shader& shader);
	void DrawInstanced(const Shader& shader, int instanceCount);

	std::vector<std::shared_ptr<Mesh>>& GetMeshes() { return m_meshes; }
	unsigned int GetMeshCount() const { return static_cast<unsigned int>(m_meshes.size()); }

	void AddMesh(std::shared_ptr<Mesh> mesh);

private:
	std::vector<std::shared_ptr<Mesh>> m_meshes;
	int i;
};
