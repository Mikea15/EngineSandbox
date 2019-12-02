#include "AssetManager.h"

#include <string>
#include <chrono>
#include <thread>
#include <future>

#include "Utils.h"

#define MULTITHREAD 0

const std::string AssetManager::s_mainAssetDirectory = "../../../../data/";
const std::string AssetManager::s_assetShaderDir = "Shaders/";
const std::string AssetManager::s_assetModelDir = "Objects/";
const std::string AssetManager::s_assetImagesDir = "Images/";

AssetManager::AssetManager()
	: m_loadingThreadActive(true)
	, m_fileWatcher(s_mainAssetDirectory, 2.0f)
{
	m_properties.m_gammaCorrection = false;
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

	m_fileWatcher.RegisterFileType(FileType::Shader_Vert, ".vert", [&](const std::string& filepath) {
		OnShaderFileUpdated(filepath);
	});
	m_fileWatcher.RegisterFileType(FileType::Shader_Frag, ".frag", [&](const std::string& filepath) {
		OnShaderFileUpdated(filepath);
	});
	m_fileWatcher.RegisterFileType(FileType::Shader_Geom, ".geom", [&](const std::string& filepath) {
		OnShaderFileUpdated(filepath);
	});

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
	m_defaultTexture = LoadTexture(s_mainAssetDirectory + s_assetImagesDir + "default.jpg");

	m_errorShader = LoadShader("base.vert", "error.frag");
	m_defaultShader = LoadShader("model_loading.vert", "model_loading.frag");
	m_wireframeShader = LoadShader("color.vert", "color.frag");

	m_defaultMaterial.AddTexture(m_defaultTexture);
	m_defaultMaterial.SetShader(m_defaultShader);
}

void AssetManager::LoaderThread()
{
	while (m_loadingThreadActive)
	{
		if (!m_loadingTexturesQueue.Empty())
		{
			TextureLoadJob assetJob;
			if (m_loadingTexturesQueue.TryPop(assetJob))
			{
				m_textureManager.LoadTexture(assetJob.texturePath, assetJob.loadedData);
				m_processingTexturesQueue.Push(assetJob);
			}
		}

		unsigned int workLoadSizeRemaining = m_loadingTexturesQueue.Size();
		{
			std::scoped_lock<std::mutex> lock(m_outputMutex);
			std::cout << "[AssetManager] Jobs Queue size: " << workLoadSizeRemaining << '\n';
		}

		const int msToWait = workLoadSizeRemaining > 0 ? 10 : 2000;
		std::this_thread::sleep_for(std::chrono::milliseconds(msToWait));
	}
}

void AssetManager::Update(float frameTime)
{
	// update filewatcher
	m_fileWatcher.Update(frameTime);

	if (!m_processingTexturesQueue.Empty())
	{
		TextureLoadJob assetJob;
		if (m_processingTexturesQueue.TryPop(assetJob))
		{
			// Generate OpenGL texture
			Texture outputTexture = m_textureManager.GenerateTexture(assetJob.loadedData, assetJob.textureType, m_properties.m_gammaCorrection);
			// Update Material
			// assetJob.materialOwner->AddTexture(outputTexture);
		}
	}
}

std::shared_ptr<Model> AssetManager::LoadModel(const std::string& path)
{
	std::string filePath = s_mainAssetDirectory + s_assetModelDir + path;
	std::string lowercasePath = Utils::Lowercase(filePath);
	size_t pathHash = Utils::Hash(lowercasePath);

	LoadedModelInfo loadedModelInfo = m_assimpImporter.LoadModel(lowercasePath);
	
	// Load All Materials.
	std::vector<Material> materials;
	for (MaterialInfo info : loadedModelInfo.materials)
	{
		Material mat = info.material;
		for (TextureInfo textureInfo : info.textures)
		{
			Texture texture = LoadTexture(textureInfo.path, textureInfo.type);
			mat.AddTexture(texture);
		}

		// Set default shader
		mat.SetShader(m_defaultShader);

		materials.push_back(mat);
	}

	// Cache Materials with Model Id.
	// Add material with loaded textures to model
	for (Material mat : materials)
	{
		loadedModelInfo.model.AddMaterial(mat);
		m_materialCache.emplace(mat.GetId(), mat);
	}

	// Cache Meshes with Model Id (?)
	for (Mesh mesh : loadedModelInfo.meshes)
	{
		loadedModelInfo.model.AddMesh(mesh);
	}

	// Initialize Model
	loadedModelInfo.model.Initialize();

	// Make Shared Model from Setup-Model
	std::shared_ptr<Model> model = std::make_shared<Model>(loadedModelInfo.model);
	m_modelsMap.emplace(pathHash, model);

#if 0
	for (Material material : materials)
	{
		m_materialCache.push_back(material);

		std::shared_ptr<Material> materialRef = m_materialCache.back();

		for (TextureType type : m_supportedTextureTypes)
		{
			std::vector<std::string> texturePaths = material->GetTexturePaths(type);

#if MULTITHREAD
			for (const std::string& path : texturePaths) 
			{
				auto textureJob = TextureLoadJob();
				textureJob.materialOwner = material;
				textureJob.texturePath = path;
				textureJob.textureType = type;

				m_loadingTexturesQueue.Push(textureJob);
			}
#else
			for (const std::string& path : texturePaths)
			{
				Texture texInfo = LoadTexture(path, type);
				material->AddTexture(texInfo);
			}
#endif
		}
	}
#endif
	std::cout << "[AssetManager] Model Finished loading\n";
	return model;
}

void AssetManager::LoadTexture(Material& material)
{
#if 0
	for (TextureType type : m_supportedTextureTypes)
	{
		auto texturePaths = material.GetTexturePaths(type);
		for (const std::string& path : texturePaths)
		{
			Texture texture = LoadTexture(path, type);
			material.AddTexture(texture);
		}
	}
#endif
}

Texture AssetManager::LoadTexture(const std::string& path, TextureType type)
{
	std::string filePath = path;
	std::string lowercasePath = Utils::Lowercase(filePath);

	const Texture texture = m_textureManager.FindTexture(lowercasePath);
	if (texture.IsValid())
	{
		return texture;
	}

	TextureLoadData textureData;
	m_textureManager.LoadTexture(lowercasePath, textureData);
	if (!textureData.HasData())
	{
		return Texture();
	}
	
	return m_textureManager.GenerateTexture(textureData, type, m_properties.m_gammaCorrection);
}

void AssetManager::RegisterMaterial(std::shared_ptr<Material> material)
{
#if 0
	m_materialCache.push_back(material);
#endif
}

void AssetManager::RegisterEntity(std::shared_ptr<Entity> entity)
{
	m_entityCache.push_back(entity);
}

Shader AssetManager::LoadShader(const std::string& vertPath, const std::string& fragPath)
{
	const std::string shaderPath = s_mainAssetDirectory + s_assetShaderDir;
	Shader shader = m_shaderManager.LoadShader(shaderPath, vertPath, fragPath);
	if (shader.HasValidProgram()) 
	{
		return shader;
	}
	return m_errorShader;
}

void AssetManager::LoadTextureAsync(const std::string& path, Texture& outTexture)
{
	std::string lowercasePath = Utils::Lowercase(path);

	const Texture texture = m_textureManager.FindTexture(lowercasePath);
	if (texture.IsValid())
	{
		outTexture = texture;
		return;
	}

	// std::cout << "[AssetManager] Creating Simple Texture Asset Job.\n";
	// SimpleTextureAssetJob job;
	// job.path = lowercasePath;
	// job.id = outId;
	// m_simpleTextureAssetJobQueue.Push(job);

	// return default texture while we load the intended one.
	// note: expand this to be handled by material, so we return
	// a temporary material instead. ( which in turn, has the temp tex )
	outTexture = m_defaultTexture;
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
	std::string lowercasePath = Utils::Lowercase(path);

	const Texture texture = m_textureManager.FindTexture(lowercasePath);
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

void AssetManager::OnShaderFileUpdated(const std::string& filepath)
{
	std::vector<Shader> shadersToUpdate = m_shaderManager.GetShaderFromPathDependency(filepath);
	for (Shader shader : shadersToUpdate) 
	{
		Shader updatedShader = m_shaderManager.NotifyShaderFileChanged(shader);
		// find materials that use this shader.
		for (auto& matInfo : m_materialCache)
		{
			if (matInfo.second.GetShader() == shader)
			{
				// Replace shader
				matInfo.second.SetShader(updatedShader);
			}
		}

		// find in all models, and replace all materials
		for (auto& modelInfo : m_modelsMap)
		{
			for (auto& modelMaterial : modelInfo.second->GetMaterials() ) 
			{
				if (modelMaterial.GetShader() == shader)
				{
					// Replace shader
					modelMaterial.SetShader(updatedShader);
				}
			}
		}

		// Loop All Entities for the materials.
		for (auto& entity : m_entityCache)
		{
			for (auto& modelMaterial : entity->GetModel().GetMaterials())
			{
				if (modelMaterial.GetShader() == shader)
				{
					// Replace shader
					modelMaterial.SetShader(updatedShader);
				}
			}
		}
		
	}
}



