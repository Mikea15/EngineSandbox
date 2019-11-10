#pragma once

#include <string>
#include <unordered_map>

#include <gl/glew.h>

enum class TextureType : int
{
	None = 0,
	DiffuseMap,
	NormalMap,
	SpecularMap,
	HeightMap,
	MetallicMap,
	AOMap,
	RoughnessMap,
	Count
};

struct TextureOp
{
	void SetTextureWrapping(unsigned int textureId)
	{
		glBindTexture(GL_TEXTURE_2D, textureId);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
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



