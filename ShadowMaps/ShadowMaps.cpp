#include <iostream>
#include <memory>

#include "ShadowMaps.h"

#include "../Core/Game.h"

#include "../Core/Utils.h"
#include "../Core/Systems/Rendering/Primitives.h"

#include "../Core/Systems/Rendering/TextureBuffer.h"

#include "../Core/RenderPass.h"

#include <glm/glm.hpp>

int main(int argc, char* argv[])
{
	Game g;

	std::unique_ptr<ShadowMapState> state = std::make_unique<ShadowMapState>();
	g.SetState(state.get());
	return g.Execute();
}

// renderCube() renders a 1x1 3D cube in NDC.
// -------------------------------------------------
unsigned int cubeVAO = 0;
unsigned int cubeVBO = 0;
void renderCube()
{
	// initialize (if necessary)
	if (cubeVAO == 0)
	{
		float vertices[] = {
			// back face
			-1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
			 1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
			 1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 0.0f, // bottom-right         
			 1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
			-1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
			-1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 1.0f, // top-left
			// front face
			-1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
			 1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 0.0f, // bottom-right
			 1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
			 1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
			-1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 1.0f, // top-left
			-1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
			// left face
			-1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
			-1.0f,  1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-left
			-1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
			-1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
			-1.0f, -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-right
			-1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
			// right face
			 1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
			 1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
			 1.0f,  1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-right         
			 1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
			 1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
			 1.0f, -1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-left     
			// bottom face
			-1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
			 1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 1.0f, // top-left
			 1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
			 1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
			-1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f, // bottom-right
			-1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
			// top face
			-1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
			 1.0f,  1.0f , 1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
			 1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 1.0f, // top-right     
			 1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
			-1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
			-1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f  // bottom-left        
		};
		glGenVertexArrays(1, &cubeVAO);
		glGenBuffers(1, &cubeVBO);
		// fill buffer
		glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
		// link vertex attributes
		glBindVertexArray(cubeVAO);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}
	// render Cube
	glBindVertexArray(cubeVAO);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
}

// renderQuad() renders a 1x1 XY quad in NDC
// -----------------------------------------
unsigned int quadVAO = 0;
unsigned int quadVBO = 0;
void renderQuad()
{
	if (quadVAO == 0)
	{
		float quadVertices[] = {
			// positions        // texture Coords
			-1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
			-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
			 1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
			 1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
		};
		// setup plane VAO
		glGenVertexArrays(1, &quadVAO);
		glGenBuffers(1, &quadVBO);
		glBindVertexArray(quadVAO);
		glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	}
	glBindVertexArray(quadVAO);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);
}

unsigned int planeVAO = 0;
unsigned int planeVBO = 0;
void renderPlane()
{
	if (quadVAO == 0)
	{
		// set up vertex data (and buffer(s)) and configure vertex attributes
		// ------------------------------------------------------------------
		float planeVertices[] = {
			// positions            // normals         // texcoords
			 25.0f, -0.5f,  25.0f,  0.0f, 1.0f, 0.0f,  25.0f,  0.0f,
			-25.0f, -0.5f,  25.0f,  0.0f, 1.0f, 0.0f,   0.0f,  0.0f,
			-25.0f, -0.5f, -25.0f,  0.0f, 1.0f, 0.0f,   0.0f, 25.0f,

			 25.0f, -0.5f,  25.0f,  0.0f, 1.0f, 0.0f,  25.0f,  0.0f,
			-25.0f, -0.5f, -25.0f,  0.0f, 1.0f, 0.0f,   0.0f, 25.0f,
			 25.0f, -0.5f, -25.0f,  0.0f, 1.0f, 0.0f,  25.0f, 25.0f
		};
		// plane VAO
		glGenVertexArrays(1, &planeVAO);
		glGenBuffers(1, &planeVBO);
		glBindVertexArray(planeVAO);
		glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), planeVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
		glBindVertexArray(0);
	}
	glBindVertexArray(planeVAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
}

void renderScene(const Shader& shader)
{
	// floor
	glm::mat4 model = glm::mat4(1.0f);
	shader.SetMat4("model", model);
	renderPlane();
	//// cubes
	//model = glm::mat4(1.0f);
	//model = glm::translate(model, glm::vec3(0.0f, 1.5f, 2.0f));
	//model = glm::scale(model, glm::vec3(0.5f));
	//shader.SetMat4("model", model);
	//renderCube();
	//model = glm::mat4(1.0f);
	//model = glm::translate(model, glm::vec3(2.0f, 0.0f, 1.0f));
	//model = glm::scale(model, glm::vec3(0.5f));
	//shader.SetMat4("model", model);
	//renderCube();
	//model = glm::mat4(1.0f);
	//model = glm::translate(model, glm::vec3(-1.0f, 0.0f, 2.0f));
	//model = glm::rotate(model, glm::radians(60.0f), glm::normalize(glm::vec3(1.0, 0.0, 1.0)));
	//model = glm::scale(model, glm::vec3(0.25));
	//shader.SetMat4("model", model);
	//renderCube();
}

const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;
unsigned int depthMapFBO;
unsigned int depthMap;

void ShadowMapState::Init(Game* game)
{
	m_game = game;
	m_sdlHandler = game->GetSDLHandler();
	m_assetManager = game->GetAssetManager();
	m_sceneCamera = &game->GetSystemComponentManager()->GetComponent<SceneCameraComponent>();

	Camera& cam = m_sceneCamera->GetCamera();

	cam.Move(glm::vec3(0.0f, 2.0f, -2.0f));
	cam.LookAt(glm::vec3(0.0f, 0.0f, 0.0f));

	m_windowParams = game->GetWindowParameters();

	{
		// lights
		auto pointLight0 = std::make_shared<PointLight>();
		pointLight0->position = glm::vec4(1, 3, 1, 1);
		pointLight0->ambient = glm::vec3(0.1f, 0.1f, 0.1f);
		pointLight0->diffuse = glm::vec3(0.8f, 0.8f, 0.8f);
		pointLight0->specular = glm::vec3(1.0f, 1.0f, 1.0f);
		pointLight0->constant = 1.0f;
		pointLight0->linear = 0.09f;
		pointLight0->quadratic = 0.032f;

		m_directionalLight = std::make_shared<DirectionalLight>();
		m_directionalLight->direction = glm::vec3(-0.2f, -1.0f, -0.3f);
		m_directionalLight->ambient = glm::vec3(0.05f, 0.05f, 0.05f);
		m_directionalLight->diffuse = glm::vec3(0.4f, 0.4f, 0.4f);
		m_directionalLight->specular = glm::vec3(0.5f, 0.5f, 0.5f);

		m_spotLight = std::make_shared<SpotLight>();
		m_spotLight->position = glm::vec3(3.0f, 3.0f, 0.0f);
		m_spotLight->diffuse = glm::vec3(3.0f, 3.0f, 0.0f) - glm::vec3(0.0f);
		m_spotLight->ambient = glm::vec3(0.0f, 0.0f, 0.0f);
		m_spotLight->diffuse = glm::vec3(1.0f, 1.0f, 1.0f);
		m_spotLight->specular = glm::vec3(1.0f, 1.0f, 1.0f);
		m_spotLight->constant = 1.0f;
		m_spotLight->linear = 0.06f;
		m_spotLight->quadratic = 0.032f;
		m_spotLight->cutOff = 30.0f;
		m_spotLight->outerCutOff = 45.0f;

		m_sceneManager.AddLightSource(pointLight0);
		m_sceneManager.AddLightSource(m_directionalLight);
		m_sceneManager.AddLightSource(m_spotLight);
	}

	// configure depth map FBO
	// -----------------------
	glGenFramebuffers(1, &depthMapFBO);
	// create depth texture
	glGenTextures(1, &depthMap);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	float borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
	// attach depth texture as FBO's depth buffer
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// shader configuration
	// --------------------
	shader = m_assetManager->LoadShader("sm", "screen/shadow_mapping.vert", "screen/shadow_mapping.frag");
	simpleDepthShader = m_assetManager->LoadShader("smd", "screen/shadow_depth.vert", "screen/shadow_depth.frag");
	debugDepthQuad = m_assetManager->LoadShader("dq", "screen/debug_quad.vert", "screen/debug_quad.frag");

	depthMaterial.SetShader(simpleDepthShader);
	
	shader.Use();
	shader.SetInt("diffuseTexture", 0);
	shader.SetInt("shadowMap", 1);
	debugDepthQuad.Use();
	debugDepthQuad.SetInt("depthMap", 0);

	auto lightShader = m_assetManager->LoadShader("light", "color.vert", "unlit/white.frag");
	lightShader.Use();
	light.SetShader(lightShader);

	// lighting info
	// -------------
	lightPos = glm::vec3(-2.0f, 4.0f, -1.0f);

	auto model = m_assetManager->LoadModel("Data/Objects/nanosuit/nanosuit.obj");
	model->Initialize();
	model->SetShader(shader);
	// m_model->SetMaterialOverride(std::make_shared<Material>(m_defaultMat));

	std::shared_ptr<Entity> entity = std::make_shared<Entity>();
	entity->SetModel(*model);
	entity->GetTransform().SetScale(glm::vec3(0.1f));

	m_sceneManager.AddEntity(entity);
}

void ShadowMapState::HandleInput(SDL_Event* event)
{
	if (event->type == SDL_KEYDOWN)
	{
		switch (event->key.keysym.sym) {
		case SDLK_g:
		{
			Camera& cam = m_sceneCamera->GetCamera();

			// Update Spotlight Position and Direction;
			m_spotLight->position = cam.GetPosition();
			m_spotLight->direction = cam.GetForward();
		}
		break;
		case SDLK_t:
		{
			Camera& cam = m_sceneCamera->GetCamera();

			// Update Directional Light direction;
			m_directionalLight->direction = cam.GetForward();
		}
		break;
		default: break;

		}
	}
}

void ShadowMapState::Update(float deltaTime)
{
	m_sceneManager.Update(deltaTime);

	float time = m_game->GetTimeMS();
	// glm::vec2 pos = glm::vec2( * 2.0f, cos(time) * 2.0f) * 0.5f;
	lightPos = glm::vec3(sin(time) * 2.0f, 2.0f + sin(time) * 0.5f, cos(time) * 2.0f);

	// Camera& cam = m_sceneCamera->GetCamera();
	// m_spotLight->position = cam.GetPosition();
	// m_spotLight->direction = cam.GetForward();
}

void ShadowMapState::Render(float alpha)
{
	Camera& cam = m_sceneCamera->GetCamera();
	glm::mat4 projection = cam.GetProjection();
	glm::mat4 view = cam.GetView();
	glm::vec3 camPos = cam.GetPosition();

	// render
	// ------
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// 1. render depth of scene to texture (from light's perspective)
	// --------------------------------------------------------------
	glm::mat4 lightProjection, lightView;
	glm::mat4 lightSpaceMatrix;
	float near_plane = -10.0f, far_plane = 10.0f;
	// lightProjection = glm::perspective(glm::radians(45.0f), (GLfloat)SHADOW_WIDTH / (GLfloat)SHADOW_HEIGHT, near_plane, far_plane); // note that if you use a perspective projection matrix you'll have to change the light position as the current light position isn't enough to reflect the whole scene
	lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
	lightView = glm::lookAt(lightPos, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
	lightSpaceMatrix = lightProjection * lightView;
	// render scene from light's point of view
	simpleDepthShader.Use();
	simpleDepthShader.SetMat4("lightSpaceMatrix", lightSpaceMatrix);

	ShadowRenderPass shadowRenderPass;

	glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
		glClear(GL_DEPTH_BUFFER_BIT);

		shadowRenderPass.Render(cam, m_sceneManager.GetSceneLights(), m_sceneManager.GetSceneObjects(), depthMaterial);

		renderScene(simpleDepthShader);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// reset viewport
	glViewport(0, 0, m_windowParams.Width, m_windowParams.Height);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// 2. render scene as normal using the generated depth/shadow map  
	// --------------------------------------------------------------
	glViewport(0, 0, m_windowParams.Width, m_windowParams.Height);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	shader.Use();
	shader.SetMat4("projection", projection);
	shader.SetMat4("view", view);
	
	// set light uniforms
	shader.SetVec3("viewPos", camPos);
	shader.SetVec3("lightPos", lightPos);

	// shader.SetVec3("spotLight.position", m_spotLight->position);
	// shader.SetVec3("spotLight.direction", m_spotLight->direction);
	// shader.SetFloat("spotLight.constant", m_spotLight->constant);
	// shader.SetFloat("spotLight.linear", m_spotLight->linear);
	// shader.SetFloat("spotLight.quadratic", m_spotLight->quadratic);
	// shader.SetFloat("spotLight.cutOff", glm::cos(glm::radians(m_spotLight->cutOff)));
	// shader.SetFloat("spotLight.outerCutOff", glm::cos(glm::radians(m_spotLight->outerCutOff)));

	shader.SetMat4("lightSpaceMatrix", lightSpaceMatrix);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_assetManager->GetDefaultTexture().GetId());
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	
	renderScene(shader);

	m_sceneManager.Draw(cam);

	light.GetShader().Use();

	Transform pointLightTransform;
	pointLightTransform.SetPosition(lightPos);
	pointLightTransform.SetScale(glm::vec3(0.1f));
	light.SetMVP(pointLightTransform.GetModelMat(), view, projection);
	Primitives::RenderCube();

	// render Depth map to quad for visual debugging
	// ---------------------------------------------
	debugDepthQuad.Use();
	debugDepthQuad.SetFloat("near_plane", near_plane);
	debugDepthQuad.SetFloat("far_plane", far_plane);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	// renderQuad();
}

void ShadowMapState::RenderUI()
{
	m_sceneManager.RenderUI();
	m_sdlHandler->RenderUI();

	ImGui::Begin("Show Buffers");

	const float screenRatio = m_windowParams.Height / static_cast<float>(m_windowParams.Width);
	const float width = ImGui::GetColumnWidth();
	const float height = width * screenRatio;

	ImGui::Image((void*)(intptr_t)depthMap, ImVec2(width, height), 
		ImVec2(0, 1), ImVec2(1, 0)); ImGui::NextColumn();

	ImGui::End();

	ImGui::Begin("Directional Light Settings");

	DirectionalLight currentDirLightParams = *m_directionalLight;

	ImGui::Color3("Ambient", currentDirLightParams.ambient);
	ImGui::Color3("Diffuse", currentDirLightParams.diffuse);
	ImGui::Color3("Specular", currentDirLightParams.specular);

	*m_directionalLight = currentDirLightParams;

	ImGui::End();

	ImGui::Begin("SpotLight Settings");

	SpotLight currentSpotLightParams = *m_spotLight;
	ImGui::Checkbox("Update OnTick", &m_updateOnTick);

	ImGui::Color3("Ambient", currentSpotLightParams.ambient);
	ImGui::Color3("Diffuse", currentSpotLightParams.diffuse);
	ImGui::Color3("Specular", currentSpotLightParams.specular);
	ImGui::Separator();
	ImGui::SliderFloat("Constant", &currentSpotLightParams.constant, 0.0f, 10.0f);
	ImGui::SliderFloat("Linear", &currentSpotLightParams.linear, 0.0f, 1.0f);
	ImGui::SliderFloat("Quadratic", &currentSpotLightParams.quadratic, 0.0f, 5.0f, "%.5f");
	ImGui::Separator();

	float tempCutOff = currentSpotLightParams.cutOff;
	float tempOuterCutOff = currentSpotLightParams.outerCutOff;

	ImGui::SliderFloat("Inner CutOff", &tempCutOff, 0.0f, tempOuterCutOff);
	ImGui::SliderFloat("Outer CutOff", &tempOuterCutOff, tempCutOff, 90.0f);

	currentSpotLightParams.cutOff = tempCutOff;
	currentSpotLightParams.outerCutOff = tempOuterCutOff;

	*m_spotLight = currentSpotLightParams;

	ImGui::End();

	const float DISTANCE = 10.0f;
	static int corner = 1;
	ImVec2 window_pos = ImVec2((corner & 1) ? ImGui::GetIO().DisplaySize.x - DISTANCE : DISTANCE, (corner & 2) ? ImGui::GetIO().DisplaySize.y - DISTANCE : DISTANCE);
	ImVec2 window_pos_pivot = ImVec2((corner & 1) ? 1.0f : 0.0f, (corner & 2) ? 1.0f : 0.0f);
	if (corner != -1)
		ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, window_pos_pivot);
	ImGui::SetNextWindowBgAlpha(0.3f); // Transparent background
	static bool open = true;
	if (ImGui::Begin("Example: Simple overlay", &open, (corner != -1 ? ImGuiWindowFlags_NoMove : 0)
		| ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize
		| ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings
		| ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav))
	{
		ImGui::Text("Quick Info");
		ImGui::Separator();
		ImGui::Text("WASD to Move Camera. Q/E Up & Down. F1 - Release Mouse");
		ImGui::Text("C - Toggle Perspective/Orthographic Camera");
		ImGui::Text("F - Focus on Origin");
		ImGui::Text("R - Reset Camera Position");
		ImGui::Text("G - Set Spotlight Pos/Direction");
		ImGui::Text("T - Set Directional Light Direction");

		if (ImGui::BeginPopupContextWindow())
		{
			if (ImGui::MenuItem("Custom", NULL, corner == -1)) corner = -1;
			if (ImGui::MenuItem("Top-left", NULL, corner == 0)) corner = 0;
			if (ImGui::MenuItem("Top-right", NULL, corner == 1)) corner = 1;
			if (ImGui::MenuItem("Bottom-left", NULL, corner == 2)) corner = 2;
			if (ImGui::MenuItem("Bottom-right", NULL, corner == 3)) corner = 3;
			if (open && ImGui::MenuItem("Close")) open = false;
			ImGui::EndPopup();
		}
	}
	ImGui::End();
}

void ShadowMapState::Cleanup()
{

}
