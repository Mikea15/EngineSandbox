#include "AssetManager.h"

#include <algorithm>
#include <string>
#include <chrono>
#include <thread>
#include <future>

#define MULTITHREAD 1

const std::string AssetManager::s_assetDirectoryPath = "Data/";

AssetManager::AssetManager()
	: m_loadingThreadActive(true)
{
	m_properties.m_gammaCorrection = true;
	m_properties.m_flipHDROnLoad = true;

	m_supportedTextureTypes = {
		TextureType::DiffuseMap,
		TextureType::NormalMap,
		TextureType::SpecularMap,
		TextureType::MetallicMap,
		TextureType::RoughnessMap,
		TextureType::HeightMap,
		TextureType::AOMap,
	};

#if MULTITHREAD
	for (int i = 0; i < m_maxThreads; ++i)
	{
		m_workerThreads.push_back(std::thread(&AssetManager::LoaderThread, this));
	}

	m_threadNames.emplace(std::this_thread::get_id(), "Main");

	for (int i = 0; i < m_maxThreads; ++i)
	{
		std::string name = "Thread " + std::to_string(i);
		m_threadNames.emplace(m_workerThreads[i].get_id(), name);
	}
#endif
}

AssetManager::~AssetManager()
{
#if MULTITHREAD
	m_loadingThreadActive = false;
	for (int i = 0; i < m_maxThreads; ++i)
	{
		m_workerThreads[i].join();
	}
#endif
}

void AssetManager::Initialize()
{
	m_defaultTex = LoadTexture("Data/Images/default.jpg").GetId();
}

void AssetManager::LoaderThread()
{
	while (m_loadingThreadActive)
	{
		bool hasTextureAssetJob = false;
		bool hasSimpleTextureJob = false;

		TextureAssetJob textureAssetJob;
		if (m_textureAssetJobQueue.TryPop(textureAssetJob))
		{
			TextureAssetJobResult jobResult;
			jobResult.material = textureAssetJob.material;
			jobResult.textureInfos = LoadTexturesFromAssetJob(textureAssetJob);
			jobResult.textureType = textureAssetJob.textureType;

			m_textureAssetJobResultQueue.Push(jobResult);
		}

		SimpleTextureAssetJob simpleJob;
		if (m_simpleTextureAssetJobQueue.TryPop(simpleJob))
		{
			SimpleTextureAssetJobResult jobResult;
			jobResult.id = simpleJob.id;

			TextureLoadData textureLoadData;
			m_textureManager.LoadTexture(simpleJob.path, textureLoadData);

			jobResult.textureLoadData = textureLoadData;

			m_simpleTextureAssetJobResultQueue.Push(jobResult);
		}

		unsigned int workLoadSizeRemaining = m_textureAssetJobQueue.Size() + m_simpleTextureAssetJobQueue.Size();
		{
			std::scoped_lock<std::mutex> lock(m_outputMutex);
			std::cout << "[AssetManager] Jobs Remaining: " << workLoadSizeRemaining << '\n';
		}

		const int msToWait = workLoadSizeRemaining > 0 ? 10 : 2000;
		std::this_thread::sleep_for(std::chrono::milliseconds(msToWait));
	}
}

void AssetManager::Update()
{
	// Handle Simple Texture Job Results
	SimpleTextureAssetJobResult simpleTextureJobResult;
	if (m_simpleTextureAssetJobResultQueue.TryPop(simpleTextureJobResult))
	{
		TextureInfo textureInfo = m_textureManager.GenerateTexture(simpleTextureJobResult.textureLoadData, TextureType::DiffuseMap, m_properties.m_gammaCorrection);

		// this should update the pointer. must find a better way to return the value
		// maybe a better approach would be using a callback.
		*simpleTextureJobResult.id = textureInfo.GetId();
	}

	// Handle Texture Asset Job Results
	TextureAssetJobResult textureAssetJobResult;
	if (!m_textureAssetJobResultQueue.TryPop(textureAssetJobResult))
	{
		for (const TextureLoadData& textureLoadData : textureAssetJobResult.textureInfos)
		{
			TextureInfo texture = m_textureManager.GenerateTexture(textureLoadData, textureAssetJobResult.textureType, m_properties.m_gammaCorrection);
			textureAssetJobResult.material->AddTexture(texture);
		}
	}
}

std::shared_ptr<Model> AssetManager::LoadModel(const std::string& path)
{
	std::string lowercasePath = GetLowercase(path);
	size_t pathHash = std::hash<std::string>{}(lowercasePath);

	std::shared_ptr<Model> model = m_assimpImporter.LoadModel(lowercasePath);
	if (!model)
	{
		return std::shared_ptr<Model>();
	}

	m_modelsMap.emplace(pathHash, model);
	
	const std::vector<std::shared_ptr<Mesh>>& meshes = model->GetMeshes();
	const unsigned int meshCount = static_cast<unsigned int>(meshes.size());
	for (const std::shared_ptr<Mesh>& mesh : meshes)
	{
		const std::shared_ptr<Material>& material = mesh->GetMaterial();
		m_materialCache.push_back(material);

		std::shared_ptr<Material> materialRef = m_materialCache.back();

		for (TextureType textureType : m_supportedTextureTypes)
		{
			std::vector<std::string> texturePaths = material->GetTexturePaths(textureType);

#if MULTITHREAD
			TextureAssetJob job;
			job.material = materialRef;
			job.textureType = textureType;

			for (const std::string& path : texturePaths)
			{
				job.resourcePaths.push_back(path);
			}

			m_textureAssetJobQueue.Push(job);
#else
			for (const std::string& path : texturePaths)
			{
				TextureInfo texInfo = LoadTexture(path, textureType);
				material->AddTexture(texInfo);
			}
#endif

		}
		
		
		m_meshCache.push_back(mesh);
	}

	return model;
}

void AssetManager::LoadTexture(Material& material)
{
	for (TextureType type : m_supportedTextureTypes)
	{
		auto texturePaths = material.GetTexturePaths(type);
		for (const std::string& path : texturePaths)
		{
			TextureLoadData textureLoadData;
			m_textureManager.LoadTexture(path, textureLoadData);
			if (!textureLoadData.HasData())
			{
				continue;
			}
			
			TextureInfo texInfo = m_textureManager.GenerateTexture(textureLoadData, type, m_properties.m_gammaCorrection);
			if (!texInfo.IsValid())
			{
				continue;
			}

			material.AddTexture(texInfo);
		}
	}
}

TextureInfo AssetManager::LoadTexture(const std::string& path, TextureType type)
{
	std::string lowercasePath = path;
	GetLowercase(lowercasePath);

	const TextureInfo texture = m_textureManager.FindTexture(lowercasePath);
	if (texture.IsValid())
	{
		return texture;
	}

	TextureLoadData textureData;
	m_textureManager.LoadTexture(lowercasePath, textureData);
	if (!textureData.HasData())
	{
		std::cerr << "[AssetManager][Error][" << m_threadNames[std::this_thread::get_id()] << "] Texture: " << path << " failed to load.\n";
		return TextureInfo();
	}

	std::cout << "[AssetManager] Texture: " << path << " loaded.\n";
	
	return m_textureManager.GenerateTexture(textureData, type, m_properties.m_gammaCorrection);
}

void AssetManager::LoadTextureAsync(const std::string& path, unsigned int* outId)
{
	std::string lowercasePath = path;
	GetLowercase(lowercasePath);

	const TextureInfo texture = m_textureManager.FindTexture(lowercasePath);
	if (texture.IsValid())
	{
		*outId = texture.GetId();
		return;
	}

	std::cout << "[AssetManager] Creating Simple Texture Asset Job.\n";

	SimpleTextureAssetJob job;
	job.path = lowercasePath;
	job.id = outId;

	m_simpleTextureAssetJobQueue.Push(job);

	// return default texture while we load the intended one.
	// note: expand this to be handled by material, so we return
	// a temporary material instead. ( which in turn, has the temp tex )
	*outId = m_defaultTex;
}

unsigned int AssetManager::LoadCubemap(const std::string& cubemapName, const std::vector<std::string>& paths)
{
	unsigned int cubemapId = 0;
	const size_t nameHash = std::hash<std::string>{}(cubemapName);

	const unsigned int imageListSize = static_cast<unsigned int>(paths.size());
	std::vector<TextureLoadData> dataTextures;
	dataTextures.resize(imageListSize);

	bool success = true;
	for (unsigned int i = 0; i < imageListSize; ++i)
	{
		m_textureManager.LoadTexture(paths[i], dataTextures[i]);
		if (!dataTextures[i].HasData())
		{
			success = false;
			break;
		}
	}

	if (!success)
	{
		std::cerr << "[AssetManager] One of the textures could not be loaded.\n";
		return 0;
	}

	return m_textureManager.GenerateCubemapTexture(cubemapName, dataTextures).GetId();
}

unsigned int AssetManager::GetHDRTexture(const std::string& path)
{
	std::string lowercasePath = path;
	GetLowercase(lowercasePath);

	const TextureInfo texture = m_textureManager.FindTexture(lowercasePath);
	if (texture.IsValid())
	{
		std::cout << "[AssetManager][" << m_threadNames[std::this_thread::get_id()] << "] Texture found\n";
		return texture.GetId();
	}

	HDRTextureLoadData textureData;
	m_textureManager.LoadHDRTexture(lowercasePath, textureData);
	if (!textureData.HasData())
	{
		std::cerr << "[AssetManager][Error][" << m_threadNames[std::this_thread::get_id()] << "] Texture: " << path << " failed to load.\n";
		return 0;
	}

	return m_textureManager.GenerateHDRTexture(textureData, TextureType::DiffuseMap).GetId();
}

std::vector<TextureLoadData> AssetManager::LoadTexturesFromAssetJob(TextureAssetJob& job)
{
	std::vector<TextureLoadData> textureInfos;
	for (const auto& path : job.resourcePaths)
	{
		TextureLoadData textureInfo;
		m_textureManager.LoadTexture(path, textureInfo);
		if (textureInfo.HasData())
		{
			textureInfos.push_back(textureInfo);
		}
	}
	return textureInfos;
}

std::string AssetManager::GetLowercase(const std::string& path)
{
	std::string lowercasePath = path;
	std::transform(lowercasePath.begin(), lowercasePath.end(), lowercasePath.begin(), ::tolower);
	return lowercasePath;
}
