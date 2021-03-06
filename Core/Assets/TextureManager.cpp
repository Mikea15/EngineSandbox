
#include "TextureManager.h"

#include <GL/glew.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "../Utils.h"

void TextureManager::LoadTexture(const std::string& path, TextureLoadData& outData, bool flipVertically)
{
	std::cout << "[TextureManager] Loading " << path << " ";

	stbi_set_flip_vertically_on_load(flipVertically);
	// stbi_hdr_to_ldr_gamma(2.2f);
	// stbi_hdr_to_ldr_scale(1.0f);

	int height = 0, width = 0, channels = 0;
	unsigned char* data = stbi_load(path.c_str(), &width, &height, &channels, 0);

	if (data != nullptr)
	{
		outData.m_path = path;
		outData.m_height = height;
		outData.m_width = width;
		outData.m_channels = channels;
		outData.m_dataPtr = data;

		std::cout << "[ok]\n";
	}
	else
	{
		std::cout << "[fail]\n";
	}
}

float* TextureManager::LoadHeightMap(const std::string& path)
{
	std::cout << "[TextureManager] Loading " << path << "\n";

	stbi_set_flip_vertically_on_load(true);

	int width = 0, height = 0, channels = 0;
	return stbi_loadf(path.c_str(), &width, &height, &channels, 0);
}

void TextureManager::LoadHDRTexture(const std::string& path, HDRTextureLoadData& outData, bool flipVertically)
{
	std::cout << "[TextureManager] Loading " << path << "\n";

	stbi_set_flip_vertically_on_load(flipVertically);

	int width = 0, height = 0, channels = 0;
	float* data = stbi_loadf(path.c_str(), &width, &height, &channels, 0);
	if (data != nullptr)
	{
		outData.m_path = path;
		outData.m_height = height;
		outData.m_width = width;
		outData.m_channels = channels;
		outData.m_dataPtr = data;
	}
}

Texture TextureManager::GenerateTexture(TextureLoadData textureData, TextureType type, bool useGammaCorrection)
{
	std::cout << "[TextureManager] Generating Texture " << textureData.m_path << "\n";

	GLenum internalFormat = 0;
	GLenum dataFormat = 0;

	const bool gammaCorrect = useGammaCorrection && type == TextureType::DiffuseMap;

	if (textureData.m_channels == 1)
	{
		internalFormat = dataFormat = GL_RED;
	}
	else if (textureData.m_channels == 3)
	{
		internalFormat = gammaCorrect ? GL_SRGB : GL_RGB;
		dataFormat = GL_RGB;
	}
	else if (textureData.m_channels == 4)
	{
		internalFormat = gammaCorrect ? GL_SRGB_ALPHA : GL_RGBA;
		dataFormat = GL_RGBA;
	}

	unsigned int texId = 0;
	glGenTextures(1, &texId);
	glBindTexture(GL_TEXTURE_2D, texId);
	glTexImage2D(GL_TEXTURE_2D, 0, internalFormat,
		textureData.m_width,
		textureData.m_height,
		0, dataFormat, GL_UNSIGNED_BYTE,
		textureData.m_dataPtr);

	glGenerateMipmap(GL_TEXTURE_2D);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	stbi_image_free(textureData.m_dataPtr);

	Texture texture = Texture(texId, textureData.m_width, textureData.m_height, textureData.m_channels,
		textureData.m_path, type);
	if (texture.IsValid())
	{
		const size_t pathHash = Utils::Hash(texture.GetPath());
		m_textureMap.emplace(pathHash, texture);
		return texture;
	}

	return Texture();
}

Texture TextureManager::GenerateHDRTexture(HDRTextureLoadData textureData, TextureType type)
{
	std::cout << "[TextureManager] Generating Texture " << textureData.m_path << "\n";

	unsigned int texId = 0;
	glGenTextures(1, &texId);

	glBindTexture(GL_TEXTURE_2D, texId);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F,
		textureData.m_width,
		textureData.m_height,
		0, GL_RGB, GL_FLOAT,
		textureData.m_dataPtr
	);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	stbi_image_free(textureData.m_dataPtr);

	Texture texture = Texture(texId, textureData.m_width, textureData.m_height, textureData.m_channels,
		textureData.m_path, type);
	if (texture.IsValid())
	{
		const size_t pathHash = Utils::Hash(texture.GetPath());
		m_textureMap.emplace(pathHash, texture);
		return texture;
	}

	return Texture();
}

Texture TextureManager::GenerateCubemapTexture(const std::string& cubemapName, std::vector<TextureLoadData>& textureData)
{
	unsigned int texId = 0;
	glGenTextures(1, &texId);
	glBindTexture(GL_TEXTURE_CUBE_MAP, texId);

	const unsigned int size = static_cast<unsigned int>(textureData.size());
	for (unsigned int i = 0; i < size; ++i)
	{
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB,
			textureData[i].m_width,
			textureData[i].m_height,
			0, GL_RGB, GL_UNSIGNED_BYTE,
			textureData[i].m_dataPtr);

		stbi_image_free(textureData[i].m_dataPtr);
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	Texture texture = Texture(texId, textureData[0].m_width, textureData[0].m_height, textureData[0].m_channels,
		textureData[0].m_path);
	if (texture.IsValid())
	{
		const size_t pathHash = Utils::Hash(cubemapName);
		m_textureMap.emplace(pathHash, texture);
		return texture;
	}

	return Texture();
}

Texture TextureManager::FindTexture(const std::string& path) const
{
	const size_t pathHash = std::hash<std::string>{}(path);
	const auto findIt = m_textureMap.find(pathHash);
	if (findIt != m_textureMap.end())
	{
		return findIt->second;
	}
	return Texture();
}
