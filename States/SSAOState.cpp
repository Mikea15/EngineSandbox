#include "SSAOState.h"

#include "Game.h"

SSAOState::SSAOState()
{

}

SSAOState::~SSAOState()
{

}

void SSAOState::Init(Game* game)
{
	DefaultState::Init(game);

	ent = Entity();
	
	std::shared_ptr<Model> model = m_assetManager->LoadModel("Data/Objects/sponza/sponza.obj");

	model->Initialize();
	ent.SetModel(*model.get());
	// ent.GetTransform().SetScale(glm::vec3(0.1f));

	// objectPositions.push_back(glm::vec3(-3.0, -3.0, -3.0));
	// objectPositions.push_back(glm::vec3(0.0, -3.0, -3.0));
	// objectPositions.push_back(glm::vec3(3.0, -3.0, -3.0));
	// objectPositions.push_back(glm::vec3(-3.0, -3.0, 0.0));
	// objectPositions.push_back(glm::vec3(0.0, -3.0, 0.0));
	// objectPositions.push_back(glm::vec3(3.0, -3.0, 0.0));
	// objectPositions.push_back(glm::vec3(-3.0, -3.0, 3.0));
	// objectPositions.push_back(glm::vec3(0.0, -3.0, 3.0));
	// objectPositions.push_back(glm::vec3(3.0, -3.0, 3.0));
	objectPositions.push_back(glm::vec3(0.0, 0.0, 0.0));

	// configure g-buffer framebuffer
	// ------------------------------
	glGenFramebuffers(1, &gBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);

	// position color buffer
	glGenTextures(1, &gPosition);
	glBindTexture(GL_TEXTURE_2D, gPosition);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, m_windowParams.Width, m_windowParams.Height, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gPosition, 0);

	// normal color buffer
	glGenTextures(1, &gNormal);
	glBindTexture(GL_TEXTURE_2D, gNormal);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, m_windowParams.Width, m_windowParams.Height, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gNormal, 0);

	// color + specular color buffer
	glGenTextures(1, &gAlbedo);
	glBindTexture(GL_TEXTURE_2D, gAlbedo);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_windowParams.Width, m_windowParams.Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gAlbedo, 0);

	// tell OpenGL which color attachments we'll use (of this framebuffer) for rendering 
	unsigned int attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
	glDrawBuffers(3, attachments);

	// create and attach depth buffer (renderbuffer)
	glGenRenderbuffers(1, &renderBufferObjectDepth);
	glBindRenderbuffer(GL_RENDERBUFFER, renderBufferObjectDepth);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, m_windowParams.Width, m_windowParams.Height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, renderBufferObjectDepth);
	// finally check if framebuffer is complete
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "Framebuffer not complete!\n";
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	ssaoFx.LoadShaders(shaderManager, m_windowParams.Width, m_windowParams.Height);
	ssaoFx.GenBuffers();

	shaderGeometryPass = shaderManager.LoadShader("ssao_geometry", "deferred/ssao_geometryBuffer.vert", "deferred/ssao_geometryBuffer.frag");

	shaderLightingPass = shaderManager.LoadShader("ssao_lighting", "screen/ssao.vert", "deferred/ssao_lighting.frag");
	shaderLightingPass.Use();
	shaderLightingPass.SetInt("gPosition", 0);
	shaderLightingPass.SetInt("gNormal", 1);
	shaderLightingPass.SetInt("gAlbedo", 2);
	shaderLightingPass.SetInt("ssao", 3);
}

void SSAOState::HandleInput(SDL_Event* event)
{
	DefaultState::HandleInput(event);
}

void SSAOState::Update(float deltaTime)
{
	DefaultState::Update(deltaTime);
}

void SSAOState::Render(float alpha)
{
	// configure transformation matrices
	glm::mat4 view = m_sceneCamera->GetCamera().GetView();
	glm::mat4 projection = m_sceneCamera->GetCamera().GetProjection();
	glm::vec3 cameraPosition = m_sceneCamera->GetCamera().GetPosition();
	
	// render
	// ------
	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// 1. geometry pass: render scene's geometry/color data into gbuffer
	// -----------------------------------------------------------------
	{
		glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		shaderGeometryPass.Use();
		shaderGeometryPass.SetMat4("projection", projection);
		shaderGeometryPass.SetMat4("view", view);

		for (unsigned int i = 0; i < objectPositions.size(); i++)
		{
			ent.GetTransform().SetPosition(objectPositions[i]);
			shaderGeometryPass.SetMat4("model", ent.GetTransform().GetModelMat());
			ent.GetModel().Draw(shaderGeometryPass);
		}
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	ssaoFx.Process(projection, gPosition, gNormal);
	
	// 2. lighting pass: calculate lighting by iterating over a screen filled quad pixel-by-pixel using the gbuffer's content.
	// -----------------------------------------------------------------------------------------------------------------------
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	shaderLightingPass.Use();

	const float constant = 1.0f; // note that we don't send this to the shader, we assume it is always 1.0 (in our case)
	const float linear = 0.09f;
	const float quadratic = 0.032f;

	// send light relevant uniforms
	// fill Light Struct
	shaderLightingPass.SetVec3("light.Position", glm::vec3(0, 0, 0)); // light position is from camera
	shaderLightingPass.SetVec3("light.Color", glm::vec3(1.0, 1.0, 1.0));

	shaderLightingPass.SetFloat("light.Linear", linear);
	shaderLightingPass.SetFloat("light.Quadratic", quadratic);

	shaderLightingPass.SetVec3("viewPos", cameraPosition);

	glActiveTexture(GL_TEXTURE0); glBindTexture(GL_TEXTURE_2D, gPosition);
	glActiveTexture(GL_TEXTURE1); glBindTexture(GL_TEXTURE_2D, gNormal);
	glActiveTexture(GL_TEXTURE2); glBindTexture(GL_TEXTURE_2D, gAlbedo);
	glActiveTexture(GL_TEXTURE3); glBindTexture(GL_TEXTURE_2D, ssaoFx.GetColorBufferBlur());

	Primitives::RenderQuad();

	// copy content of geometry depth buffer to default framebuffer depth buffer
	// this is so the lights are rendered with correct depth values, etc..
	glBindFramebuffer(GL_READ_FRAMEBUFFER, gBuffer);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

	// blit to default framebuffer. Note that this may or may not work as the internal formats of both the FBO and default framebuffer have to match.
	// the internal formats are implementation defined. This works on all of my systems, but if it doesn't on yours you'll likely have to write to the 		
	// depth buffer in another shader stage (or somehow see to match the default framebuffer's internal format with the FBO's internal format).
	glBlitFramebuffer(0, 0, m_windowParams.Width, m_windowParams.Height, 0, 0,
		m_windowParams.Width, m_windowParams.Height, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// should now render as if forward rendering?
	// render skybox last. but before transparent objects
	skyboxShader.Use();
	skyboxShader.SetMat4("projection", projection);
	skyboxShader.SetMat4("view", view);
	m_skybox.Draw(skyboxShader);
}

void SSAOState::RenderUI()
{
	DefaultState::RenderUI();

	ImGui::Begin("SSAO Settings");
	SSAO::Params ssaoParams = ssaoFx.GetParams();

	ImGui::Checkbox("Enable", &enableSSAO);
	ImGui::SliderFloat("Intensity", &ssaoParams.Intensity, 0.0f, 10.0f);
	ImGui::SliderInt("Kernel Size", &ssaoParams.KernelSize, 1, 256);
	ImGui::SliderFloat("Kernel Radius", &ssaoParams.Radius, 0, 10);
	ImGui::SliderFloat("Bias", &ssaoParams.Bias, 0.0f, 1.0f);
	ImGui::SliderInt("BlurSize", &ssaoParams.BlurSize, 0, 10);
	ImGui::End();

	ImGui::Begin("Show Buffers");

	if (ssaoFx.GetParams() != ssaoParams)
	{
		ssaoFx.SetParams(ssaoParams);
	}

	const float screenRatio = m_windowParams.Height / static_cast<float>(m_windowParams.Width);
	
	ImGui::Columns(2, NULL, false);

	const float width = ImGui::GetColumnWidth();
	const float height = width * screenRatio;

	ImGui::Text("Position"); ImGui::NextColumn();
	ImGui::Text("Normals"); ImGui::NextColumn();
	ImGui::Image((void*)(intptr_t)gPosition, ImVec2(width, height), ImVec2(0, 1), ImVec2(1, 0)); ImGui::NextColumn();
	ImGui::Image((void*)(intptr_t)gNormal, ImVec2(width, height), ImVec2(0, 1), ImVec2(1, 0)); ImGui::NextColumn();

	ImGui::Text("Color"); ImGui::NextColumn();
	ImGui::Text("SSAO"); ImGui::NextColumn();
	ImGui::Image((void*)(intptr_t)gAlbedo, ImVec2(width, height), ImVec2(0, 1), ImVec2(1, 0)); ImGui::NextColumn();
	ImGui::Image((void*)(intptr_t)ssaoFx.GetColorBufferBlur(), ImVec2(width, height), ImVec2(0, 1), ImVec2(1, 0)); ImGui::NextColumn();
	
	ImGui::End();
}

void SSAOState::Cleanup()
{

}
