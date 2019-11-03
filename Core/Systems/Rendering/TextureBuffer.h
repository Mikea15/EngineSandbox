#pragma once

#include <gl/glew.h>

class TextureBuffer
{
public:
	static void CreateFrameBuffer(int width, int height, unsigned int& frameBufferId, unsigned int& textureId, unsigned int& depthTextureId);
	static void CreateDepthMapFBO(int width, int height, unsigned int& frameBufferId, unsigned int& textureId);

	static void CreateDepthCubeMap(int width, int height, unsigned int& frameBufferId, unsigned int& textureId);
};