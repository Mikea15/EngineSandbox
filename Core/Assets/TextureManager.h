#pragma once

#include <iostream>
#include <queue>
#include <algorithm>
#include <string>
#include <functional>

#include "../Systems/Rendering/Texture.h"

struct TextureLoadData
{
	bool HasData() const { return m_dataPtr != nullptr; }

	std::string m_path;

	int m_width = 0;
	int m_height = 0;
	int m_channels = 0;

	unsigned char* m_dataPtr = nullptr;
};

struct HDRTextureLoadData
{
	bool HasData() const { return m_dataPtr != nullptr; }

	std::string m_path;

	int m_width = 0;
	int m_height = 0;
	int m_channels = 0;

	float* m_dataPtr = nullptr;
};

class TextureManager
{
public:
	TextureManager() = default;

	void LoadTexture(const std::string& path, TextureLoadData& outData, bool flipVertically = true);
	float* LoadHeightMap(const std::string& path);
	void LoadHDRTexture(const std::string& path, HDRTextureLoadData& outData, bool flipVertically = true);

	Texture GenerateTexture(TextureLoadData textureData, TextureType type, bool useGammaCorrection);

	Texture GenerateHDRTexture(HDRTextureLoadData textureData, TextureType type);

	Texture GenerateCubemapTexture(const std::string& cubemapName, std::vector<TextureLoadData>& textureData);

	Texture FindTexture(const std::string& path) const;
	
private:
	std::unordered_map<size_t, Texture> m_textureMap;
};