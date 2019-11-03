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
#include "SimpleTextureAssetJob.h"

#include "ThreadSafeQueue.h"

struct TextureAssetJob
{
	std::shared_ptr<Material> material;
	TextureType textureType = TextureType::None;
	std::vector<std::string> resourcePaths;
	std::vector<TextureLoadData> texDatas;
};

struct TextureAssetJobResult
{
	std::shared_ptr<Material> material;
	TextureType textureType = TextureType::None;
	std::vector<TextureLoadData> textureInfos;
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
	void LoadShader();
	
	// Textures
	TextureInfo LoadTexture(const std::string& path, TextureType type = TextureType::DiffuseMap);
	void LoadTextureAsync(const std::string& path, unsigned int* outId);

	// Cubemaps
	unsigned int LoadCubemap(const std::string& cubemapName, const std::vector<std::string>& paths);
	
	unsigned int GetHDRTexture(const std::string& path);

	unsigned int GetDefaultTex() const { return m_defaultTex; }

private:
	std::vector<TextureLoadData> LoadTexturesFromAssetJob(TextureAssetJob& job);

	std::string GetLowercase(const std::string& path);

private:
	Properties m_properties;
	TextureManager m_textureManager;
	ShaderManager m_shaderManager;

	AssimpImporter m_assimpImporter;
	std::vector<TextureType> m_supportedTextureTypes;

	std::unordered_map<size_t, std::shared_ptr<Model>> m_modelsMap;

	unsigned int m_defaultTex;

	ThreadSafeQueue<SimpleTextureAssetJob> m_simpleTextureAssetJobQueue;
	ThreadSafeQueue<SimpleTextureAssetJobResult> m_simpleTextureAssetJobResultQueue;

	ThreadSafeQueue<TextureAssetJob> m_textureAssetJobQueue;
	ThreadSafeQueue<TextureAssetJobResult> m_textureAssetJobResultQueue;

	std::vector<std::shared_ptr<Mesh>> m_meshCache;
	std::vector<std::shared_ptr<Material>> m_materialCache;

	int m_maxThreads = 1;
	std::vector<std::thread> m_workerThreads;
	std::unordered_map<std::thread::id, std::string> m_threadNames;

	std::atomic<bool> m_loadingThreadActive;

	std::mutex m_outputMutex;

	// ---
	static const std::string s_assetDirectoryPath;
};

