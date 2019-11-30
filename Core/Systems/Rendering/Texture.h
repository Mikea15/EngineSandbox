#pragma once

#include <string>
#include <unordered_map>

#include <gl/glew.h>

enum class TextureType : int
{
	None			= -1,
	DiffuseMap		= 0,
	NormalMap		= 1,
	SpecularMap		= 2,
	HeightMap		= 3,
	MetallicMap		= 4,
	AOMap			= 5,
	RoughnessMap	= 6,


	Count
};

static std::string GetTextureTypeName(TextureType type)
{
	static const std::unordered_map<TextureType, std::string> TextureTypeToName = {
		{ TextureType::DiffuseMap,		"diffuse" },
		{ TextureType::NormalMap,		"normal" },
		{ TextureType::SpecularMap,		"specular" },
		{ TextureType::HeightMap,		"height" },
		{ TextureType::MetallicMap,		"metallic" },
		{ TextureType::AOMap,			"ao" },
		{ TextureType::RoughnessMap,	"roughness" }
	};

	auto find = TextureTypeToName.find(type);
	if (find != TextureTypeToName.end())
	{
		return find->second;
	}
	return "InvalidTextureName";
}

static GLenum GetTextureSlot(TextureType type)
{
	static const std::unordered_map<TextureType, GLenum> TextureTypeToSlot = {
		{ TextureType::DiffuseMap,		GL_TEXTURE0 },
		{ TextureType::NormalMap,		GL_TEXTURE1 },
		{ TextureType::SpecularMap,		GL_TEXTURE2 },
		{ TextureType::HeightMap,		GL_TEXTURE3 },
		{ TextureType::MetallicMap,		GL_TEXTURE4 },
		{ TextureType::AOMap,			GL_TEXTURE5 },
		{ TextureType::RoughnessMap,	GL_TEXTURE6 }
	};

	auto find = TextureTypeToSlot.find(type);
	if (find != TextureTypeToSlot.end())
	{
		return find->second;
	}
	return GL_TEXTURE0;
}

struct TextureOp
{
	void SetTextureWrapping(unsigned int textureId)
	{
		glBindTexture(GL_TEXTURE_2D, textureId);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	void SetTextureFiltering(unsigned int textureId)
	{
		glBindTexture(GL_TEXTURE_2D, textureId);

		// GL_NEAREST, GL_LINEAR ( bilinear filtering )
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


		// MipMap Low to High Transition filter.
		// GL_NEAREST_MIPMAP_NEAREST
		// GL_LINEAR_MIPMAP_NEAREST
		// GL_NEAREST_MIPMAP_LINEAR
		// GL_LINEAR_MIPMAP_LINEAR
		// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

	}

	void GenerateMipMaps(unsigned int textureId)
	{
		// glGenerateMipMaps();
	}
};

class Texture
{
public:
	Texture()
		: m_id(InvalidId)
		, m_width(0)
		, m_height(0)
		, m_channels(0)
		, m_path("")
		, m_type(TextureType::DiffuseMap)
	{ }

	Texture(unsigned int id , int width, int height, int channels
		, const std::string& path , TextureType type = TextureType::DiffuseMap)
		: m_id(id)
		, m_width(width)
		, m_height(height)
		, m_channels(channels)
		, m_path(path)
		, m_type(type)
	{

	}


	bool IsValid() const { return m_id != InvalidId; }

	unsigned int GetId() const { return m_id; }
	TextureType GetType() const { return m_type; }

	int GetWidth() const { return m_width; }
	int GetHeight() const { return m_height; }
	int GetChannels() const { return m_channels; }
	const std::string& GetPath() const { return m_path; }

	static const unsigned int InvalidId = UINT_MAX;

private:
	unsigned int m_id;
	TextureType m_type;

	int m_width = 0;
	int m_height = 0;
	int m_channels = 0;
	std::string m_path;
};



