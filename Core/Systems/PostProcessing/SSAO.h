#pragma once

#include <gl/glew.h>

#include <random>

#include "../Rendering/Shader.h"
#include "../Rendering/Primitives.h"

class ShaderManager;

class SSAO
{
public:
	struct Params
	{
		float Intensity = 1.0f;
		int KernelSize = 16;
		float Radius = 2.5f;
		float Bias = 0.05f;
		int BlurSize = 4;

		bool operator==(const Params& rhs) {
			if (Intensity != rhs.Intensity) return false;
			if (KernelSize != rhs.KernelSize) return false;
			if (Radius != rhs.Radius) return false;
			if (Bias != rhs.Bias) return false;
			if (BlurSize != rhs.BlurSize) return false;
			return true;
		}

		bool operator!=(const Params& rhs) {
			return !(*this == rhs);
		}
	};

	SSAO();
	~SSAO();

	void LoadShaders(ShaderManager& shaderManager, unsigned int width, unsigned int height);
	void GenBuffers();
	void GenSampleKernel();
	void GenNoiseTexture();

	void Process(const glm::mat4& projection, unsigned int gPosition, unsigned int gNormal);

	Params GetParams() const { return m_params; }
	void SetParams(Params params) { 
		m_params = params;
		GenSampleKernel();
		GenNoiseTexture();
	}

	unsigned int GetColorBuffer() const { return m_colorBuffer; }
	unsigned int GetColorBufferBlur() const { return m_colorBlurBuffer; }

private:
// ssao
	std::vector<glm::vec3> ssaoKernel;
	
	unsigned int m_FBO;
	unsigned int m_blurFBO;
	unsigned int m_colorBuffer;
	unsigned int m_colorBlurBuffer;
	unsigned int m_noiseTexture;

	unsigned int m_screenWidth;
	unsigned int m_screenHeight;

	Params m_params;

	Shader shaderSSAO;
	Shader shaderSSAOBlur;

	static int s_MaxKernelSamples;
};

