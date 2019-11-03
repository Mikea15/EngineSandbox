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

struct TextureInfo
{
	bool IsValid() const { return m_id != s_InvalidId; }

	unsigned int GetId() const { return m_id; }
	TextureType GetType() const { return m_textureType; }

	unsigned int m_id = s_InvalidId;
	TextureType m_textureType = TextureType::DiffuseMap;

	static const int s_InvalidId = -1;
};

class TextureManager
{
public:
	TextureManager() = default;

	void LoadTexture(const std::string& path, TextureLoadData& outData);

	void LoadHDRTexture(const std::string& path, HDRTextureLoadData& outData, bool flipVertically = false);

	TextureInfo GenerateTexture(TextureLoadData textureData, TextureType type, bool useGammaCorrection);

	TextureInfo GenerateHDRTexture(HDRTextureLoadData textureData, TextureType type);

	TextureInfo GenerateCubemapTexture(const std::string& cubemapName, std::vector<TextureLoadData>& textureData);

	TextureInfo FindTexture(const std::string& path) const;
	
private:
	std::unordered_map<size_t, TextureInfo> m_textureMap;
};