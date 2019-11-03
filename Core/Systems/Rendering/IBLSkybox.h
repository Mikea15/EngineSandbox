#pragma once

#include <gl/glew.h>
#include <vector>

#include "../Transform.h"
#include "Primitives.h"
#include "Primitives/Cube.h"
#include "Shader.h"

class IBLSkybox
{
public:
	IBLSkybox();
	~IBLSkybox();

	void SetShaders(Shader irradiance, Shader prefilter, Shader brdf, Shader cubemap);
	void GenBuffers(unsigned int texture, unsigned int size);

	void BindTextureMaps();

	void DrawSkybox(Shader shader, const glm::mat4& view, const glm::mat4& projection);

private:
	unsigned int captureFBO;
	unsigned int captureRBO;

	unsigned int hdrTexture;
	unsigned int envCubemap;

	glm::mat4 captureProjection;
	std::vector<glm::mat4> captureViews;

	unsigned int irradianceMap;
	unsigned int prefilterMap;
	unsigned int brdfLUTTexture;

	Cube cube;

	Shader equirectangularToCubemapShader;
	Shader irradianceShader;
	Shader prefilterShader;
	Shader brdfShader;
};

