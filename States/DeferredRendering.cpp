#include "DeferredRendering.h"

#include "Game.h"

DeferredRendering::DeferredRendering()
{

}

DeferredRendering::~DeferredRendering()
{

}

void DeferredRendering::Init(Game* game)
{
	DefaultState::Init(game);

	shaderGeometryPass	= shaderManager.LoadShader("gBuff",		"deferred/gbuffer.vert",				"deferred/gbuffer.frag");
	shaderLightingPass	= shaderManager.LoadShader("lightPass",	"deferred/deferred_shading.vert",		"deferred/deferred_shading.frag");
	shaderLightBox		= shaderManager.LoadShader("lighBox",	"deferred/deferred_light_box.vert",		"deferred/deferred_light_box.frag");

	ent = Entity();
	std::shared_ptr<Model> model = m_assetManager->LoadModel("Data/Objects/sponza/sponza.obj");
	// std::shared_ptr<Model> model = m_assetManager->LoadModel("Data/Objects/buddha/buddha.obj");
	// std::shared_ptr<Model> model = m_assetManager->LoadModel("Data/Objects/dragon/dragon.obj");
	// std::shared_ptr<Model> model = m_assetManager->LoadModel("Data/Objects/lpshead/head.obj");
	model->Initialize();
	ent.SetModel(*model.get());
	ent.GetTransform().SetScale(glm::vec3(0.01f));

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
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gPosition, 0);
	// normal color buffer
	glGenTextures(1, &gNormal);
	glBindTexture(GL_TEXTURE_2D, gNormal);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, m_windowParams.Width, m_windowParams.Height, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gNormal, 0);
	// color + specular color buffer
	glGenTextures(1, &gAlbedoSpec);
	glBindTexture(GL_TEXTURE_2D, gAlbedoSpec);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_windowParams.Width, m_windowParams.Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gAlbedoSpec, 0);
	// tell OpenGL which color attachments we'll use (of this framebuffer) for rendering 
	unsigned int attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
	glDrawBuffers(3, attachments);

	// create and attach depth buffer (renderbuffer)
	glGenRenderbuffers(1, &rboDepth);
	glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, m_windowParams.Width, m_windowParams.Height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);

	std::cout << "gPosition buffer: " << gPosition << "\n";
	std::cout << "gNormal buffer: " << gNormal << "\n";
	std::cout << "gAlbedoSpec buffer: " << gAlbedoSpec << "\n";
	std::cout << "rboDepth buffer: " << rboDepth << "\n";

	// finally check if framebuffer is complete
	auto status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE)
	{
		std::cout << "Framebuffer not complete! Status: " << status << "\n";
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// setup lighting
	srand(static_cast<unsigned int>(time(NULL)));
	const float min = 0.0f;
	const float max = 5.0f;
	for (unsigned int i = 0; i < nLights; i++)
	{
		// calculate slightly random offsets
		float xPos = ((rand() % 100) / 100.0f) * max - min;
		float yPos = ((rand() % 100) / 100.0f) * max - min;
		float zPos = ((rand() % 100) / 100.0f) * max - min;
		lightPositions.push_back(glm::vec3(xPos, yPos, zPos));
		// also calculate random color
		float rColor = ((rand() % 100) / 200.0f) + 0.5f; // between 0.5 and 1.0
		float gColor = ((rand() % 100) / 200.0f) + 0.5f; // between 0.5 and 1.0
		float bColor = ((rand() % 100) / 200.0f) + 0.5f; // between 0.5 and 1.0
		lightColors.push_back(glm::vec3(rColor, gColor, bColor));
	}

	// setup shaders
	shaderLightingPass.Use();
	shaderLightingPass.SetInt("gPosition", 0);
	shaderLightingPass.SetInt("gNormal", 1);
	shaderLightingPass.SetInt("gAlbedoSpec", 2);
}

void DeferredRendering::HandleInput(SDL_Event* event)
{
	DefaultState::HandleInput(event);
}

void DeferredRendering::Update(float deltaTime)
{
	DefaultState::Update(deltaTime);
}

void DeferredRendering::Render(float alpha)
{
	PROFILE("Render");

	// configure transformation matrices
	glm::mat4 view = m_sceneCamera->GetCamera().GetView();
	glm::mat4 projection = m_sceneCamera->GetCamera().GetProjection();
	glm::vec3 cameraPosition = m_sceneCamera->GetCamera().GetPosition();

	// render
	// ------
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
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


	// 2. lighting pass: calculate lighting by iterating over a screen filled quad pixel-by-pixel using the gbuffer's content.
	// -----------------------------------------------------------------------------------------------------------------------
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	shaderLightingPass.Use();
	glActiveTexture(GL_TEXTURE0); glBindTexture(GL_TEXTURE_2D, gPosition);
	glActiveTexture(GL_TEXTURE1); glBindTexture(GL_TEXTURE_2D, gNormal);
	glActiveTexture(GL_TEXTURE2); glBindTexture(GL_TEXTURE_2D, gAlbedoSpec);
	
	// send light relevant uniforms
	for (unsigned int i = 0; i < lightPositions.size(); i++)
	{
		shaderLightingPass.SetVec3("lights[" + std::to_string(i) + "].Position", lightPositions[i]);
		shaderLightingPass.SetVec3("lights[" + std::to_string(i) + "].Color", lightColors[i]);
		// update attenuation parameters and calculate radius
		const float constant = lConstant; // note that we don't send this to the shader, we assume it is always 1.0 (in our case)
		const float linear = llinear;
		const float quadratic = lquadratic;
		shaderLightingPass.SetFloat("lights[" + std::to_string(i) + "].Linear", linear);
		shaderLightingPass.SetFloat("lights[" + std::to_string(i) + "].Quadratic", quadratic);
		// then calculate radius of light volume/sphere
		const float maxBrightness = std::fmaxf(std::fmaxf(lightColors[i].r, lightColors[i].g), lightColors[i].b);
		float radius = (-linear + std::sqrt(linear * linear - 4 * quadratic * (constant - (256.0f / 5.0f) * maxBrightness))) / (2.0f * quadratic);
		shaderLightingPass.SetFloat("lights[" + std::to_string(i) + "].Radius", radius);
	}
	shaderLightingPass.SetVec3("viewPos", cameraPosition);
	Primitives::RenderQuad();
	

	// 2.5. copy content of geometry's depth buffer to default framebuffer's depth buffer
	// this is so the lights are rendered with correct depth values, as if rendered in forward rendering fashion
	// ----------------------------------------------------------------------------------
	glBindFramebuffer(GL_READ_FRAMEBUFFER, gBuffer);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0); // write to default framebuffer
	
	// blit to default framebuffer. Note that this may or may not work as the internal formats of both the FBO and default framebuffer have to match.
	// the internal formats are implementation defined. This works on all of my systems, but if it doesn't on yours you'll likely have to write to the 		
	// depth buffer in another shader stage (or somehow see to match the default framebuffer's internal format with the FBO's internal format).
	glBlitFramebuffer(0, 0, m_windowParams.Width, m_windowParams.Height, 0, 0,
		m_windowParams.Width, m_windowParams.Height, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	
	// 3. render lights on top of scene
	// --------------------------------
	shaderLightBox.Use();
	shaderLightBox.SetMat4("projection", projection);
	shaderLightBox.SetMat4("view", view);
	Transform lightTransform;
	for (unsigned int i = 0; i < lightPositions.size(); i++)
	{
		lightTransform.SetPosition(lightPositions[i]);
		lightTransform.SetScale(glm::vec3(0.125f));
		shaderLightBox.SetMat4("model", lightTransform.GetModelMat());
		shaderLightBox.SetVec3("lightColor", lightColors[i]);
		Primitives::RenderSphere();
	}

	// render skybox last. but before transparent objects
	skyboxShader.Use();
	skyboxShader.SetMat4("projection", projection);
	skyboxShader.SetMat4("view", view);
	m_skybox.Draw(skyboxShader);
}

void DeferredRendering::RenderUI()
{
	DefaultState::RenderUI();

	ImGui::Begin("Light Settings");
	ImGui::SliderFloat("Constant", &lConstant, 0.0f, 10.0f);
	ImGui::SliderFloat("Linear", &llinear, 0.0f, 5.0f);
	ImGui::SliderFloat("Quadratic", &lquadratic, 0.01f, 10.0f);
	ImGui::End();
	ImGui::Begin("Show Buffers");

	const float screenRatio = m_windowParams.Height / static_cast<float>(m_windowParams.Width);

	ImGui::Columns(3, NULL, false);
	ImGui::Text("Position"); ImGui::NextColumn();
	ImGui::Text("Normals"); ImGui::NextColumn();
	ImGui::Text("Color"); ImGui::NextColumn();

	const float width = ImGui::GetColumnWidth();
	const float height = width * screenRatio;

	ImGui::Image((void*)(intptr_t)gPosition, ImVec2(width, height), ImVec2(0, 1), ImVec2(1, 0)); ImGui::NextColumn();
	ImGui::Image((void*)(intptr_t)gNormal, ImVec2(width, height), ImVec2(0, 1), ImVec2(1, 0)); ImGui::NextColumn();
	ImGui::Image((void*)(intptr_t)gAlbedoSpec, ImVec2(width, height), ImVec2(0, 1), ImVec2(1, 0)); ImGui::NextColumn();

	ImGui::End();
}

void DeferredRendering::Cleanup()
{

}
