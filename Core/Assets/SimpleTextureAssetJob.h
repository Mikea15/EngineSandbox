#pragma once

#include <string>
#include "../Systems/Rendering/Texture.h"

#include "TextureManager.h"

struct SimpleTextureAssetJob
{
	std::string path = "";
	unsigned int* id = nullptr;
};

struct SimpleTextureAssetJobResult
{
	TextureLoadData textureLoadData;
	unsigned int* id = nullptr;
};
