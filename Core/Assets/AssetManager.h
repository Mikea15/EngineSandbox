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

#include "Systems/Material.h"
#include "Systems/Model.h"
#include "Systems/Rendering/Shader.h"
#include "Systems/Rendering/Texture.h"


struct TextureLoadJob
{
	std::shared_ptr<Material> materialOwner;
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

	// Materials
	void LoadTexture(Material& material);

	// Shaders
	std::shared_ptr<Shader> LoadShader(const std::string& name, const std::string& vertPath, const std::string& fragPath, const std::string& geomPath = "");
	
	// Textures
	Texture LoadTexture(const std::string& path, TextureType type = TextureType::DiffuseMap);
	void LoadTextureAsync(const std::string& path, Texture& outTexture);

	// Cubemaps
	unsigned int LoadCubemap(const std::string& cubemapName, const std::vector<std::string>& paths);
	
	unsigned int GetHDRTexture(const std::string& path);

	Texture GetDefaultTexture() const { return m_defaultTexture; }
	Material GetDefaultMaterial() const { return m_defaultMaterial; }
	std::shared_ptr<Shader> GetDefaultShader() const { return m_defaultShader; }
	std::shared_ptr<Shader> GetWireframeShader() const { return m_wireframeShader; }

	ShaderManager* GetShaderManager() { return &m_shaderManager; }

private:
	Properties m_properties;
	TextureManager m_textureManager;
	ShaderManager m_shaderManager;

	AssimpImporter m_assimpImporter;
	std::vector<TextureType> m_supportedTextureTypes;

	std::unordered_map<size_t, std::shared_ptr<Model>> m_modelsMap;
	std::unordered_map<size_t, std::shared_ptr<Shader>> m_shaders;

	std::shared_ptr<Shader> m_defaultShader;
	std::shared_ptr<Shader> m_wireframeShader;
	Material m_defaultMaterial;
	Texture m_defaultTexture;

	ThreadSafeQueue<TextureLoadJob> m_loadingTexturesQueue;
	ThreadSafeQueue<TextureLoadJob> m_processingTexturesQueue;

	std::vector<std::shared_ptr<Material>> m_materialCache;

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

