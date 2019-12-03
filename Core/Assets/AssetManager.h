#pragma once

#include <string>
#include <vector>
#include <queue>
#include <unordered_map>
#include <functional>

#include <thread>
#include <mutex>
#include <future>

#include "ShaderManager.h"
#include "FileWatcher.h"
#include "TextureManager.h"
#include "AssimpImporter.h"
#include "ThreadSafeQueue.h"

#include "Systems/Entity.h"
#include "Systems/Material.h"
#include "Systems/Model.h"
#include "Systems/Rendering/Shader.h"
#include "Systems/Rendering/Texture.h"

struct TextureLoadJob
{
	AssetId materialId;
	std::string texturePath;
	TextureType textureType = TextureType::DiffuseMap;
	TextureLoadData loadedData;
};

class Game;

class AssetManager
{
public:
	struct Properties
	{
		bool m_gammaCorrection;
		bool m_flipHDROnLoad;
	};

	AssetManager();
	~AssetManager();

	void Initialize();
	void LoaderThread( );
	void Update(float frameTime);

	// Models
	std::shared_ptr<Model> LoadModel(const std::string& path);

	void RegisterMaterial(std::shared_ptr<Material> material);
	void RegisterEntity(std::shared_ptr<Entity> entity);

	// Shaders
	Shader LoadShader(const std::string& vertPath, const std::string& fragPath);
	
	// Textures
	Texture LoadTexture(const std::string& path, TextureType type = TextureType::DiffuseMap);
	void LoadTexture(Material& material);
	void LoadTextureAsync(const std::string& path, Texture& outTexture);
	float* LoadHeightMapTexture(const std::string& path);

	// Cubemaps
	unsigned int LoadCubemap(const std::string& cubemapName, const std::vector<std::string>& paths);
	
	unsigned int GetHDRTexture(const std::string& path);

	Texture GetDefaultTexture() const { return m_defaultTexture; }
	Material GetDefaultMaterial() const { return m_defaultMaterial; }

	Shader GetDefaultShader() const { return m_defaultShader; }
	Shader GetWireframeShader() const { return m_wireframeShader; }
	Shader GetErrorShader() const { return m_errorShader; }

	ShaderManager* GetShaderManager() { return &m_shaderManager; }

	void OnShaderFileUpdated(const std::string& filepath);

private:
	Properties m_properties;

	TextureManager m_textureManager;
	ShaderManager m_shaderManager;

	AssimpImporter m_assimpImporter;
	std::vector<TextureType> m_supportedTextureTypes;

	std::unordered_map<size_t, std::shared_ptr<Model>> m_modelsMap;
	std::unordered_map<AssetId, Material> m_materialCache;
	std::vector<std::shared_ptr<Entity>> m_entityCache; // HACK:.... :(

	Shader m_errorShader;
	Shader m_wireframeShader;

	Shader m_defaultShader;
	Texture m_defaultTexture;
	Material m_defaultMaterial;

	ThreadSafeQueue<TextureLoadJob> m_loadingTexturesQueue;
	ThreadSafeQueue<TextureLoadJob> m_processingTexturesQueue;

	int m_maxThreads = 1;
	std::vector<std::thread> m_workerThreads;
	std::unordered_map<std::thread::id, std::string> m_threadNames;

	std::atomic<bool> m_loadingThreadActive;

	std::mutex m_outputMutex;

	FileWatcher m_fileWatcher;

	// ---
	static const std::string s_mainAssetDirectory;
	static const std::string s_assetShaderDir;
	static const std::string s_assetModelDir;
	static const std::string s_assetImagesDir;
};

