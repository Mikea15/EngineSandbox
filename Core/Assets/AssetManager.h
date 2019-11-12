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

#include "../Systems/Material.h"
#include "../Systems/Model.h"
#include "../Systems/Rendering/Shader.h"
#include "../Systems/Rendering/Texture.h"

#include "TextureManager.h"

#include "AssimpImporter.h"

#include "ThreadSafeQueue.h"

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

	void Update();

	// Models
	std::shared_ptr<Model> LoadModel(const std::string& path);

	// Materials
	void LoadTexture(Material& material);

	// Shaders
	Shader LoadShader(const std::string& name, const std::string& vertPath, const std::string& fragPath, const std::string& geomFrag = "");
	
	// Textures
	Texture LoadTexture(const std::string& path, TextureType type = TextureType::DiffuseMap);
	void LoadTextureAsync(const std::string& path, Texture& outTexture);

	// Cubemaps
	unsigned int LoadCubemap(const std::string& cubemapName, const std::vector<std::string>& paths);
	
	unsigned int GetHDRTexture(const std::string& path);

	Texture GetDefaultTexture() const { return m_defaultTexture; }
	Material GetDefaultMaterial() const { return m_defaultMaterial; }
	Shader GetDefaultShader() const { return m_defaultShader; }

private:
	Properties m_properties;
	TextureManager m_textureManager;
	ShaderManager m_shaderManager;

	AssimpImporter m_assimpImporter;
	std::vector<TextureType> m_supportedTextureTypes;

	std::unordered_map<size_t, std::shared_ptr<Model>> m_modelsMap;

	Shader m_defaultShader;
	Material m_defaultMaterial;
	Texture m_defaultTexture;

	ThreadSafeQueue<TextureLoadJob> m_loadingTexturesQueue;
	ThreadSafeQueue<TextureLoadJob> m_processingTexturesQueue;

	std::vector<std::shared_ptr<Material>> m_materialCache;

	int m_maxThreads = 8;
	std::vector<std::thread> m_workerThreads;
	std::unordered_map<std::thread::id, std::string> m_threadNames;

	std::atomic<bool> m_loadingThreadActive;

	std::mutex m_outputMutex;

	// ---
	static const std::string s_assetDirectoryPath;
};

