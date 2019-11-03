#include "InstancingState.h"

#include <algorithm>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>

#include "Game.h"

void InstancingState::Init(Game* game)
{
	DefaultState::Init(game);

	asteroidShader = shaderManager.LoadShader("asteroidInstancing", "instancing.vert", "lit/blinn_phong.frag");
	planetShader = shaderManager.LoadShader("planetShader", "lit/default_simple.vert", "unlit/blinn_phong_shadows.frag");
	
	rockEnt = Entity();
	std::shared_ptr<Model> model = m_assetManager->LoadModel("Data/Objects/rock/rock.obj");
	model->Initialize();
	model->SetShader(asteroidShader);
	rockEnt.SetModel(*model);
	
	planetEnt = Entity();
	std::shared_ptr<Model> model2 = m_assetManager->LoadModel("Data/Objects/test/test.fbx");
	model2->Initialize();
	model2->SetShader(asteroidShader);
	planetEnt.SetModel(*model2);
	planetEnt.GetTransform().SetPosition(glm::vec3(0.0f, 0.0f, 0.0f));
	planetEnt.GetTransform().Scale(1.0f);

	GenerateAsteroidBelt();
}

void InstancingState::HandleInput(SDL_Event* event)
{
	DefaultState::HandleInput(event);
}

void InstancingState::Update(float deltaTime)
{
	DefaultState::Update(deltaTime);
}

void InstancingState::Render(float alpha)
{
	// make sure we clear the framebuffer's content
	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	const Camera& camera = m_sceneCamera->GetCamera();

	// configure transformation matrices
	glm::mat4 view = camera.GetView();
	glm::mat4 projection = camera.GetProjection();

	// draw planet
	planetShader.Use();
	planetShader.SetMat4("projection", projection);
	planetShader.SetMat4("view", view);
	planetShader.SetMat4("model", planetEnt.GetTransform().GetModelMat());
	planetEnt.GetModel().Draw(planetShader);

	// draw meteorites
	asteroidShader.Use();
	asteroidShader.SetMat4("projection", projection);
	asteroidShader.SetMat4("view", view);
	rockEnt.GetModel().DrawInstanced(asteroidShader, asteroidCount);

	// render skybox last. but before transparent objects
	skyboxShader.Use();
	skyboxShader.SetMat4("projection", camera.GetProjection());
	skyboxShader.SetMat4("view", camera.GetView());
	m_skybox.Draw(skyboxShader);
}

void InstancingState::RenderUI()
{
	DefaultState::RenderUI();

	using namespace ImGui;

	Begin("Asteroid Belt");

	SliderFloat("Belt Radius", &radius, 15, 200);
	SliderFloat("Belt Offset", &offset, 15, 200);

	SliderInt("n#", &newAsteroidCount, 1, 1000000);
	if (Button("Generate Asteroid Belt"))
	{
		asteroidCount = newAsteroidCount;
		GenerateAsteroidBelt();
	}

	End();
}

void InstancingState::Cleanup()
{
	
}


void InstancingState::GenerateAsteroidBelt()
{
	modelMatrices.clear();
	modelMatrices.resize(asteroidCount);
	srand(14);
	
	for (unsigned int i = 0; i < asteroidCount; i++)
	{
		glm::mat4 model = glm::mat4(1.0f);
		// 1. translation: displace along circle with 'radius' in range [-offset, offset]
		float angle = (float)i / (float)asteroidCount * 360.0f;
		float displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
		float x = sin(angle) * radius + displacement;
		displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
		float y = displacement * 0.4f; // keep height of asteroid field smaller compared to width of x and z
		displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
		float z = cos(angle) * radius + displacement;
		model = glm::translate(model, glm::vec3(x, y, z));


		// 2. scale: Scale between 0.05 and 0.25f
		float scale = static_cast<float>(rand() % 20) / 100.0f + 0.05f;
		model = glm::scale(model, glm::vec3(scale));

		// 3. rotation: add random rotation around a (semi)randomly picked rotation axis vector
		float rotAngle = static_cast<float>(rand() % 360);
		model = glm::rotate(model, rotAngle, glm::vec3(0.4f, 0.6f, 0.8f));

		// 4. now add to list of matrices
		modelMatrices[i] = model;
	}

	// configure instanced array
	// -------------------------
	unsigned int buffer;
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, asteroidCount * sizeof(glm::mat4), &modelMatrices[0], GL_STATIC_DRAW);


	// set transformation matrices as an instance vertex attribute (with divisor 1)
	// note: we're cheating a little by taking the, now publicly declared, VAO of the model's mesh(es) and adding new vertexAttribPointers
	// normally you'd want to do this in a more organized fashion, but for learning purposes this will do.
	// -----------------------------------------------------------------------------------------------------------------------------------
	
	auto rockMeshes2 = rockEnt.GetModel().GetMeshes();
	for (unsigned int i = 0; i < rockMeshes2.size(); i++)
	{
		unsigned int VAO = rockMeshes2[i]->GetVAO();
		glBindVertexArray(VAO);
		// set attribute pointers for matrix (4 times vec4)
		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)0);
		glEnableVertexAttribArray(4);
		glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(sizeof(glm::vec4)));
		glEnableVertexAttribArray(5);
		glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(2 * sizeof(glm::vec4)));
		glEnableVertexAttribArray(6);
		glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(3 * sizeof(glm::vec4)));
		//
		glVertexAttribDivisor(3, 1);
		glVertexAttribDivisor(4, 1);
		glVertexAttribDivisor(5, 1);
		glVertexAttribDivisor(6, 1);

		glBindVertexArray(0);
	}
}