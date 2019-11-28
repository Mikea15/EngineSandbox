#include "SSAO.h"


#include "Assets/ShaderManager.h"

int SSAO::s_MaxKernelSamples = 256;

SSAO::SSAO()
{
}


SSAO::~SSAO()
{
}

void SSAO::LoadShaders(ShaderManager& shaderManager, unsigned int width, unsigned int height)
{
	m_screenWidth = width;
	m_screenHeight = height;

	shaderSSAO = shaderManager.LoadShader("../../../../data/shaders/", "screen/ssao.vert", "screen/ssao.frag");
	shaderSSAO.Use();
	shaderSSAO.SetInt("gPosition", 0);
	shaderSSAO.SetInt("gNormal", 1);
	shaderSSAO.SetInt("texNoise", 2);
	shaderSSAO.SetInt("screenWidth", m_screenWidth);
	shaderSSAO.SetInt("screenHeight", m_screenHeight);

	shaderSSAOBlur = shaderManager.LoadShader("../../../../data/shaders/", "screen/ssao.vert", "screen/ssao_blur.frag");
	
	shaderSSAOBlur.Use();
	shaderSSAOBlur.SetInt("ssaoInput", 0);
	shaderSSAOBlur.SetVec2("resolution", glm::vec2(width, height));
}

void SSAO::GenBuffers()
{
	// also create framebuffer to hold SSAO processing stage 
	// -----------------------------------------------------
	glGenFramebuffers(1, &m_FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);
	// SSAO color buffer
	glGenTextures(1, &m_colorBuffer);
	glBindTexture(GL_TEXTURE_2D, m_colorBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, m_screenWidth, m_screenHeight, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_colorBuffer, 0);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "SSAO Framebuffer not complete!\n";

	
	glGenFramebuffers(1, &m_blurFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, m_blurFBO);
	// and blur stage
	glGenTextures(1, &m_colorBlurBuffer);
	glBindTexture(GL_TEXTURE_2D, m_colorBlurBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, m_screenWidth, m_screenHeight, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_colorBlurBuffer, 0);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "SSAO Blur Framebuffer not complete!\n";
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	GenSampleKernel();
	GenNoiseTexture();
}

void SSAO::GenSampleKernel()
{
	// generate sample kernel
	// ----------------------
	std::uniform_real_distribution<GLfloat> randomFloats(0.0, 1.0); // generates random floats between 0.0 and 1.0
	std::default_random_engine generator;

	if (m_params.KernelSize > s_MaxKernelSamples)
	{
		m_params.KernelSize = s_MaxKernelSamples;
	}

	shaderSSAO.Use();

	ssaoKernel.clear();
	for (unsigned int i = 0; i < static_cast<unsigned int>(m_params.KernelSize); ++i)
	{
		// scale samples s.t. they're more aligned to center of kernel
		float scale = static_cast<float>(i / m_params.KernelSize);
		
		// scale = Utils::Lerp(0.01f, 1.0f, scale * scale);

		glm::vec3 sample(
			randomFloats(generator) * 2.0f - 1.0f,
			randomFloats(generator) * 2.0f - 1.0f,
			randomFloats(generator)
		);

		sample = glm::normalize(sample) * randomFloats(generator) * scale;
		
		ssaoKernel.push_back(sample);
	}
}

void SSAO::GenNoiseTexture()
{
	std::uniform_real_distribution<GLfloat> randomFloats(0.0, 1.0); // generates random floats between 0.0 and 1.0
	std::default_random_engine generator;

	// generate noise texture
	// ----------------------
	const int noiseTexSize = 4;
	const int noiseTexSizeSq = 4 * 4;
	std::vector<glm::vec3> ssaoNoise;
	for (unsigned int i = 0; i < noiseTexSizeSq; i++)
	{
		glm::vec3 noise(randomFloats(generator) * 2.0 - 1.0, randomFloats(generator) * 2.0 - 1.0, 0.0f); // rotate around z-axis (in tangent space)
		ssaoNoise.push_back(noise);
	}

	glGenTextures(1, &m_noiseTexture);
	glBindTexture(GL_TEXTURE_2D, m_noiseTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, noiseTexSize, noiseTexSize, 0, GL_RGB, GL_FLOAT, &ssaoNoise[0]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
}

void SSAO::Process(const glm::mat4& projection, unsigned int gPosition, unsigned int gNormal)
{
	// 1.1. generate SSAO texture
	// ------------------------
	glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);
	glClear(GL_COLOR_BUFFER_BIT);
	
	shaderSSAO.Use();
	shaderSSAO.SetMat4("projection", projection);

	for (unsigned int i = 0; i < static_cast<unsigned int>(m_params.KernelSize); ++i)
	{
		shaderSSAO.SetVec3("samples[" + std::to_string(i) + "]", ssaoKernel[i]);
	}

	shaderSSAO.SetInt("kernelSize", m_params.KernelSize);
	shaderSSAO.SetFloat("radius", m_params.Radius);
	shaderSSAO.SetFloat("bias", m_params.Bias);
	shaderSSAO.SetFloat("intensity", m_params.Intensity);

	glActiveTexture(GL_TEXTURE0); glBindTexture(GL_TEXTURE_2D, gPosition);
	glActiveTexture(GL_TEXTURE1); glBindTexture(GL_TEXTURE_2D, gNormal);
	glActiveTexture(GL_TEXTURE2); glBindTexture(GL_TEXTURE_2D, m_noiseTexture);

	Primitives::RenderQuad();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// 1.2. blur SSAO texture to remove noise
	// ------------------------------------
	glBindFramebuffer(GL_FRAMEBUFFER, m_blurFBO);
	glClear(GL_COLOR_BUFFER_BIT);

	shaderSSAOBlur.Use();
	shaderSSAOBlur.SetInt("blurSize", m_params.BlurSize);
	// Send color buffer to blur phase.
	glActiveTexture(GL_TEXTURE0); glBindTexture(GL_TEXTURE_2D, m_colorBuffer);

	Primitives::RenderQuad();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
