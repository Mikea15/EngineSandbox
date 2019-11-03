#include "IBLSpecState.h"

#include "Game.h"

#include <random>
#include <sstream>
#include <fstream>


#include "Systems/Material.h"

static float ccCamNearPlane = 1.0f;
static float ccCamFarPlane = 600.0f;
static float ccCamFov = 75.0f;
static float ccRotationSpeed = 10.0f;

IBLSpecState::IBLSpecState()
{

}

IBLSpecState::~IBLSpecState()
{

}

void IBLSpecState::Init(Game* game)
{
	DefaultState::Init(game);

	// set depth function to less than AND equal for skybox depth trick.
	//glDepthFunc(GL_LEQUAL);
	glEnable(GL_DEPTH_TEST);

	// enable seamless cubemap sampling for lower mip levels in the pre-filter map.
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

	m_qTree = QuadTree(glm::vec3(0.0f), 50.0f);
	m_oTree = Octree(glm::vec3(0.0f), 50.0f);

	debugInstanced = shaderManager.LoadShader("debugInstanced", "instancing.vert", "unlit/color.frag");
	pbrShader = shaderManager.LoadShader("pbrShader", "pbr/pbr.vert", "pbr/pbr.frag");
	// pbrShader = shaderManager.LoadShader("pbrShader", "model_loading.vert", "model_loading.frag");
	
	Shader equirectangularToCubemapShader = shaderManager.LoadShader("equirectangularToCubemapShader", "pbr/cubemap.vert", "pbr/equirectangular_to_cubemap.frag");
	Shader irradianceShader = shaderManager.LoadShader("irradianceShader", "pbr/cubemap.vert", "pbr/irradiance_convolution.frag");
	Shader prefilterShader = shaderManager.LoadShader("prefilterShader", "pbr/cubemap.vert", "pbr/prefilter.frag");
	Shader brdfShader = shaderManager.LoadShader("brdfShader", "pbr/brdf.vert", "pbr/brdf.frag");

	iblSkybox.SetShaders(irradianceShader, prefilterShader, brdfShader, equirectangularToCubemapShader);

	backgroundShader = shaderManager.LoadShader("backgroundShader", "pbr/background.vert", "pbr/background.frag");

	wireframeShader = shaderManager.LoadShader("wireframeSimple", "basic.vert", "unlit/color.frag");
	wireframeShader.Use();
	wireframeShader.SetVec3("Color", glm::vec3(0, 0, 1.0));

	pbrShader.Use();
	pbrShader.SetInt("irradianceMap", 0);
	pbrShader.SetInt("prefilterMap", 1);
	pbrShader.SetInt("brdfLUT", 2);

	pbrShader.SetInt("albedoMap", 3);
	pbrShader.SetInt("normalMap", 4);
	pbrShader.SetInt("metallicMap", 5);
	pbrShader.SetInt("roughnessMap", 6);
	pbrShader.SetInt("aoMap", 7);
	pbrShader.SetInt("heightMap", 8);

	backgroundShader.Use();
	backgroundShader.SetInt("environmentMap", 0);

	// load PBR material textures
	{
		// --------------------------
		// rusted iron
		iron.AddTexturePath(TextureType::DiffuseMap, "Data/Images/pbr/rusted_iron/albedo.png");
		iron.AddTexturePath(TextureType::NormalMap, "Data/Images/pbr/rusted_iron/normal.png");
		iron.AddTexturePath(TextureType::MetallicMap, "Data/Images/pbr/rusted_iron/metallic.png");
		iron.AddTexturePath(TextureType::RoughnessMap, "Data/Images/pbr/rusted_iron/roughness.png");
		iron.AddTexturePath(TextureType::AOMap, "Data/Images/pbr/rusted_iron/ao.png");
		iron.SetShader(wireframeShader);
		m_assetManager->LoadTexture(iron);

		// gold
		gold.AddTexturePath(TextureType::DiffuseMap, "Data/Images/pbr/gold/albedo.png");
		gold.AddTexturePath(TextureType::NormalMap, "Data/Images/pbr/gold/normal.png");
		gold.AddTexturePath(TextureType::MetallicMap, "Data/Images/pbr/gold/metallic.png");
		gold.AddTexturePath(TextureType::RoughnessMap, "Data/Images/pbr/gold/roughness.png");
		gold.AddTexturePath(TextureType::AOMap, "Data/Images/pbr/gold/ao.png");
		gold.SetShader(pbrShader);
		m_assetManager->LoadTexture(gold);

		// grass
		grass.AddTexturePath(TextureType::DiffuseMap, "Data/Images/pbr/grass/albedo.png");
		grass.AddTexturePath(TextureType::NormalMap, "Data/Images/pbr/grass/normal.png");
		grass.AddTexturePath(TextureType::MetallicMap, "Data/Images/pbr/grass/metallic.png");
		grass.AddTexturePath(TextureType::RoughnessMap, "Data/Images/pbr/grass/roughness.png");
		grass.AddTexturePath(TextureType::AOMap, "Data/Images/pbr/grass/ao.png");
		grass.SetShader(pbrShader);
		m_assetManager->LoadTexture(grass);

		// plastic
		plastic.AddTexturePath(TextureType::DiffuseMap, "Data/Images/pbr/plastic/albedo.png");
		plastic.AddTexturePath(TextureType::NormalMap, "Data/Images/pbr/plastic/normal.png");
		plastic.AddTexturePath(TextureType::MetallicMap, "Data/Images/pbr/plastic/metallic.png");
		plastic.AddTexturePath(TextureType::RoughnessMap, "Data/Images/pbr/plastic/roughness.png");
		plastic.AddTexturePath(TextureType::AOMap, "Data/Images/pbr/plastic/ao.png");
		plastic.SetShader(pbrShader);
		m_assetManager->LoadTexture(plastic);

		// wall
		wall.AddTexturePath(TextureType::DiffuseMap, "Data/Images/pbr/wall/albedo.png");
		wall.AddTexturePath(TextureType::NormalMap, "Data/Images/pbr/wall/normal.png");
		wall.AddTexturePath(TextureType::MetallicMap, "Data/Images/pbr/wall/metallic.png");
		wall.AddTexturePath(TextureType::RoughnessMap, "Data/Images/pbr/wall/roughness.png");
		wall.AddTexturePath(TextureType::AOMap, "Data/Images/pbr/wall/ao.png");
		wall.SetShader(pbrShader);
		m_assetManager->LoadTexture(wall);

		// marble
		marble.AddTexturePath(TextureType::DiffuseMap, "Data/Images/pbr/marble/albedo.png");
		marble.AddTexturePath(TextureType::NormalMap, "Data/Images/pbr/marble/normal.png");
		marble.AddTexturePath(TextureType::HeightMap, "Data/Images/pbr/marble/height.png");
		marble.AddTexturePath(TextureType::RoughnessMap, "Data/Images/pbr/marble/roughness.png");
		marble.AddTexturePath(TextureType::AOMap, "Data/Images/pbr/marble/ao.png");
		marble.SetShader(pbrShader);
		m_assetManager->LoadTexture(marble);

		// granite
		granite.AddTexturePath(TextureType::DiffuseMap, "Data/Images/pbr/cliffgranite/albedo.png");
		granite.AddTexturePath(TextureType::NormalMap, "Data/Images/pbr/cliffgranite/normal.png");
		granite.AddTexturePath(TextureType::HeightMap, "Data/Images/pbr/cliffgranite/height.png");
		granite.AddTexturePath(TextureType::RoughnessMap, "Data/Images/pbr/cliffgranite/roughness.png");
		granite.AddTexturePath(TextureType::AOMap, "Data/Images/pbr/cliffgranite/ao.png");
		granite.SetShader(pbrShader);
		m_assetManager->LoadTexture(granite);

		// leather
		leather.AddTexturePath(TextureType::DiffuseMap, "Data/Images/pbr/leather/albedo.png");
		leather.AddTexturePath(TextureType::NormalMap, "Data/Images/pbr/leather/normal.png");
		leather.AddTexturePath(TextureType::HeightMap, "Data/Images/pbr/leather/height.png");
		leather.AddTexturePath(TextureType::RoughnessMap, "Data/Images/pbr/leather/roughness.png");
		leather.AddTexturePath(TextureType::AOMap, "Data/Images/pbr/leather/ao.png");
		leather.SetShader(pbrShader);
		m_assetManager->LoadTexture(leather);
	}


	unsigned int skyboxTex = m_assetManager->GetHDRTexture("Data/Images/pbr/newport_loft.hdr");
	iblSkybox.GenBuffers(skyboxTex, 512);

	// initialize static shader uniforms before rendering
	// --------------------------------------------------
	glm::mat4 projection = m_sceneCamera->GetCamera().GetProjection();

	pbrShader.Use();
	pbrShader.SetMat4("projection", projection);

	backgroundShader.Use();
	backgroundShader.SetMat4("projection", projection);

	glViewport(0, 0, m_windowParams.Width, m_windowParams.Height);

	// create a 3d grid of cubes.
	gX = gY = gZ = 10;
	gY = 1;

	std::random_device rd; // obtain a random number from hardware
	std::mt19937 eng(rd()); // seed the generator
	std::uniform_int_distribution<> distr(-20, 20); // define the range

	for (int gXStart = -gX; gXStart < gX; gXStart++)
	{
		for (int gYStart = -gY; gYStart < gY; gYStart++)
		{
			for (int gZStart = -gZ; gZStart < gZ; gZStart++)
			{
				glm::vec3 v3 = glm::vec3(distr(eng), gY, distr(eng)) * gridSpacing;
				positions.push_back(v3);
				visible.push_back(ContainmentType::Disjoint);

				m_qTree.Insert(v3);
				m_oTree.Insert(v3);
			}
		}
	}

	// configure (floating point) framebuffers
	// ---------------------------------------
	glGenFramebuffers(1, &hdrFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);
	// create 2 floating point color buffers (1 for normal rendering, other for brightness treshold values)
	glGenTextures(1, &colorBuffer);
	glBindTexture(GL_TEXTURE_2D, colorBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, m_windowParams.Width, m_windowParams.Height, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);  // we clamp to the edge as the blur filter would otherwise sample repeated texture values!
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	// attach texture to framebuffer
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorBuffer, 0);


	// create and attach depth buffer (renderbuffer)
	glGenRenderbuffers(1, &rboDepth);
	glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, m_windowParams.Width, m_windowParams.Height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);

	// tell OpenGL which color attachments we'll use (of this framebuffer) for rendering 
	unsigned int attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
	glDrawBuffers(2, attachments);

	// finally check if framebuffer is complete
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "Framebuffer not complete!\n";
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	topDownCamera.SetFov(m_sceneCamera->GetCamera().GetFov());
	topDownCamera.SetNearFarPlane(
		m_sceneCamera->GetCamera().GetNearPlane(),
		m_sceneCamera->GetCamera().GetFarPlane()
	);

	
	cc.SetNearFarPlane(ccCamNearPlane, ccCamFarPlane);
	cc.SetFov(ccCamFov);

	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
	glDepthFunc(GL_LEQUAL);
	glDepthRange(0.0f, 1.0f);
}

void IBLSpecState::HandleInput(SDL_Event* event)
{
	DefaultState::HandleInput(event);
}

void IBLSpecState::Update(float deltaTime)
{
	DefaultState::Update(deltaTime);

	glm::vec3 camPos = m_sceneCamera->GetCamera().GetPosition();
	glm::vec3 camPosOverHead = camPos + glm::vec3(0, 1, 0) * 3.0f;

	if (includeCamPosIntoTrees)
	{
		camPushPosTime += deltaTime;
		if (camPushPosTime > includeFreq)
		{
			glm::vec3 positionToAdd = camPos + m_sceneCamera->GetCamera().GetForward() * 2.0f;
			positions.push_back(positionToAdd);
			m_qTree.Insert(positionToAdd);
			m_oTree.Insert(positionToAdd);
			visible.push_back(ContainmentType::Disjoint);
			camPushPosTime = 0.0f;
		}
	}

	cc.UpdateLookAt(glm::vec2(1.0f, 0.0f) * ccRotationSpeed * deltaTime);
	cc.Update(deltaTime);

	topDownCamera.SetPosition(glm::vec3(0.0f, 10.0f, 0.0f));
	topDownCamera.Update(deltaTime);
}

void IBLSpecState::Render(float alpha)
{
	glViewport(0, 0, m_windowParams.Width, m_windowParams.Height);

	vertexCountStats = 0;

	// configure transformation matrices
	glm::mat4 view = m_sceneCamera->GetCamera().GetView();
	glm::mat4 projection = m_sceneCamera->GetCamera().GetProjection();
	glm::vec3 cameraPosition = m_sceneCamera->GetCamera().GetPosition();

	// have this by default
	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
	glDepthFunc(GL_LEQUAL);
	glDepthRange(0.0f, 1.0f);

	// render
	// ------
	glClearColor(0.5f, 0.3f, 0.5f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if (drawSkybox)
	{
		iblSkybox.DrawSkybox(backgroundShader, view, projection);
		vertexCountStats += Primitives::cube.GetVertexCount();
	}
	
	if (showLines)
	{
		wireframeShader.Use();
		wireframeShader.SetMat4("view", view);
		wireframeShader.SetMat4("projection", projection);
		wireframeShader.SetVec3("Color", glm::vec3(0.1, 0.8, 0.2));

		Transform t;
		
		t.SetPosition(glm::vec3(0.0f));
		wireframeShader.SetVec3("Color", glm::vec3(0.0f, 0.0f, 0.0f));
		wireframeShader.SetMat4("model", t.GetModelMat());
		Primitives::RenderPoint(10);

		// red
		t.SetPosition(glm::vec3(1.0f, 0.0f, 0.0f));
		wireframeShader.SetVec3("Color", glm::vec3(1.0f, 0.0f, 0.0f));
		wireframeShader.SetMat4("model", t.GetModelMat());
		Primitives::RenderPoint(10);

		// green
		t.SetPosition(glm::vec3(0.0f, 1.0f, 0.0f));
		wireframeShader.SetVec3("Color", glm::vec3(0.0f, 1.0f, 0.0f));
		wireframeShader.SetMat4("model", t.GetModelMat());
		Primitives::RenderPoint(10);

		// blue
		t.SetPosition(glm::vec3(0.0f, 0.0f, 1.0f));
		wireframeShader.SetVec3("Color", glm::vec3(0.0f, 0.0f, 1.0f));
		wireframeShader.SetMat4("model", t.GetModelMat());
		Primitives::RenderPoint(10);


		t.SetPosition(glm::vec3(0.0f));
		wireframeShader.SetMat4("model", t.GetModelMat());
		wireframeShader.SetVec3("Color", glm::vec3(0.0f, 1.0f, 0.0f));
		Primitives::RenderLine(glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

		t.SetPosition(glm::vec3(0.0f));
		wireframeShader.SetMat4("model", t.GetModelMat());
		wireframeShader.SetVec3("Color", glm::vec3(1.0f, 0.0f, 0.0f));
		Primitives::RenderLine(glm::vec3(0.0f), glm::vec3(1.0f, 0.0f, 0.0f));

		t.SetPosition(glm::vec3(0.0f));
		wireframeShader.SetMat4("model", t.GetModelMat());
		wireframeShader.SetVec3("Color", glm::vec3(0.0f, 0.0f, 1.0f));
		Primitives::RenderLine(glm::vec3(0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		

		wireframeShader.SetMat4("model", t.GetModelMat());
		glm::vec3 ccP = cc.GetPosition();
		auto points = cc.GetBoundingFrustum().GetCorners();
		// near
		Primitives::RenderLine(points[0], points[3]);
		Primitives::RenderLine(points[1], points[0]);
		Primitives::RenderLine(points[2], points[1]);
		Primitives::RenderLine(points[3], points[2]);

		// far
		Primitives::RenderLine(points[4], points[7]);
		Primitives::RenderLine(points[5], points[4]);
		Primitives::RenderLine(points[6], points[5]);
		Primitives::RenderLine(points[7], points[6]);

		// near to far
		Primitives::RenderLine(points[0], points[4]);
		Primitives::RenderLine(points[1], points[5]);
		Primitives::RenderLine(points[2], points[6]);
		Primitives::RenderLine(points[3], points[7]);
	}

	if (showSpheresTextured)
	{
		// render scene, supplying the convoluted irradiance map to the final shader.
		// ------------------------------------------------------------------------------------------
		pbrShader.Use();
		pbrShader.SetMat4("view", view);
		pbrShader.SetMat4("projection", projection);
		pbrShader.SetVec3("camPos", cameraPosition);
		pbrShader.SetFloat("heightScale", 0.1f);

		// bind pre-computed IBL data
		iblSkybox.BindTextureMaps();

		// rusted iron
		iron.BindTextures();
		{
			Transform t;
			t.SetPosition(glm::vec3(-5.0, 0.0, 2.0));
			pbrShader.SetMat4("model", t.GetModelMat());
			Primitives::RenderSphere();
			vertexCountStats += Primitives::sphere.GetVertexCount();
		}

		// gold
		gold.BindTextures();
		{
			Transform t;
			t.SetPosition(glm::vec3(-3.0, 0.0, 2.0));
			pbrShader.SetMat4("model", t.GetModelMat());
			Primitives::RenderSphere();
			vertexCountStats += Primitives::sphere.GetVertexCount();
		}

		// grass
		grass.BindTextures();
		{
			Transform t;
			t.SetPosition(glm::vec3(-1.0, 0.0, 2.0));
			pbrShader.SetMat4("model", t.GetModelMat());
			Primitives::RenderSphere();
			vertexCountStats += Primitives::sphere.GetVertexCount();
		}

		// plastic
		plastic.BindTextures();
		{
			Transform t;
			t.SetPosition(glm::vec3(1.0, 0.0, 2.0));
			pbrShader.SetMat4("model", t.GetModelMat());
			Primitives::RenderSphere();
			vertexCountStats += Primitives::sphere.GetVertexCount();
		}

		// wall
		wall.BindTextures();
		{
			Transform t;
			t.SetPosition(glm::vec3(-5.0, 2.0, 2.0));
			pbrShader.SetMat4("model", t.GetModelMat());
			Primitives::RenderSphere();
			vertexCountStats += Primitives::sphere.GetVertexCount();
		}

		// marble
		marble.BindTextures();
		{
			Transform t;
			t.SetPosition(glm::vec3(-3.0, 2.0, 2.0));
			pbrShader.SetMat4("model", t.GetModelMat());
			Primitives::RenderSphere();
			vertexCountStats += Primitives::sphere.GetVertexCount();
		}

		// granite
		granite.BindTextures();
		{
			Transform t;
			t.SetPosition(glm::vec3(1.0, 2.0, 2.0));
			pbrShader.SetMat4("model", t.GetModelMat());
			Primitives::RenderSphere();
			vertexCountStats += Primitives::sphere.GetVertexCount();
		}

		// leather
		leather.BindTextures();
		{
			Transform t;
			t.SetPosition(glm::vec3(1.0, 2.0, 2.0));
			pbrShader.SetMat4("model", t.GetModelMat());
			Primitives::RenderSphere();
			vertexCountStats += Primitives::sphere.GetVertexCount();
		}
	}

	if (showSpheres)
	{
		// draw 3d grid of cubes.
		const unsigned int size = static_cast<unsigned int>(positions.size());
		vertexCountStats += Primitives::sphere.GetVertexCount() * size;

		for (unsigned int i = 0; i < size; ++i)
		{
			scratchTransform.SetPosition(positions[i]);
			pbrShader.SetMat4("model", scratchTransform.GetModelMat());

			visible[i] = cc.GetBoundingFrustum().Contains(BoundingBox(positions[i], 1.0f));
			if (visible[i] == ContainmentType::Disjoint)
			{
				wireframeShader.Use();
				wireframeShader.SetMat4("view", view);
				wireframeShader.SetMat4("projection", projection);
				wireframeShader.SetVec3("camPos", cameraPosition);
				wireframeShader.SetMat4("model", scratchTransform.GetModelMat());
				wireframeShader.SetVec3("Color", glm::vec3(0.1f, 0.2f, 0.5f));

				Primitives::RenderSphere(true);
			}
			else if (visible[i] == ContainmentType::Intersects)
			{
				wireframeShader.Use();
				wireframeShader.SetMat4("view", view);
				wireframeShader.SetMat4("projection", projection);
				wireframeShader.SetVec3("camPos", cameraPosition);
				wireframeShader.SetMat4("model", scratchTransform.GetModelMat());
				wireframeShader.SetVec3("Color", glm::vec3(1.0f, 1.0f, 1.0f));
				Primitives::RenderSphere(true);
			}
			else
			{
				pbrShader.Use();
				pbrShader.SetMat4("view", view);
				pbrShader.SetMat4("projection", projection);
				pbrShader.SetVec3("camPos", cameraPosition);
				pbrShader.SetMat4("model", scratchTransform.GetModelMat());
				Primitives::RenderSphere(false);
			}
		}
	}

	if (showQtree)
	{
		std::vector<Rect> quadTreeVis;
		m_qTree.GetAllBoundingBoxes(quadTreeVis);
		const unsigned int qSize = static_cast<unsigned int>(quadTreeVis.size());
		for (unsigned int i = 0; i < qSize; ++i)
		{
			Transform origin;
			auto pos2D = quadTreeVis[i].GetPosition();

			origin.SetPosition(glm::vec3(pos2D.x, 0.0f, pos2D.y));
			origin.RotateLocal(glm::vec3(1.0f, 0.0f, 0.0f), 90.0f);
			origin.SetScale(glm::vec3(quadTreeVis[i].GetSize()));

			wireframeShader.Use();
			wireframeShader.SetMat4("view", view);
			wireframeShader.SetMat4("projection", projection);
			wireframeShader.SetVec3("Color", glm::vec3(0.1, 0.8, 0.2));
			wireframeShader.SetMat4("model", origin.GetModelMat());
			Primitives::RenderQuad(true);
			vertexCountStats += Primitives::quad.GetVertexCount();
		}
	}

	if (showOctree)
	{
		std::vector<BoundingBox> ocTreeVis;
		m_oTree.GetAllBoundingBoxes(ocTreeVis);
		const unsigned int qSize = static_cast<unsigned int>(ocTreeVis.size());
		for (unsigned int i = 0; i < qSize; ++i)
		{
			Transform origin;
			auto pos = ocTreeVis[i].GetPosition();

			origin.SetPosition(pos);
			origin.SetScale(glm::vec3(ocTreeVis[i].GetSize()));

			wireframeShader.Use();
			wireframeShader.SetMat4("view", view);
			wireframeShader.SetMat4("projection", projection);
			wireframeShader.SetVec3("Color", glm::vec3(0.1, 0.8, 0.2));
			wireframeShader.SetMat4("model", origin.GetModelMat());
			Primitives::RenderCube(true);
			vertexCountStats += Primitives::cube.GetVertexCount();
		}
	}

	

	// render BRDF map to screen
	// brdfShader.Use();
	// Primitives::RenderQuad();

	// render top down view in wireframe
	
	if( renderTopDownBuffer )
	{
		// 1. render scene into floating point framebuffer
		// -----------------------------------------------
		glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);

		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

		// configure transformation matrices
		glm::mat4 view = topDownCamera.GetView();
		glm::mat4 projection = topDownCamera.GetProjection();
		glm::vec3 cameraPosition = topDownCamera.GetPosition();
		glm::mat4 ortho = topDownCamera.OrthographicMatrix();

		wireframeShader.Use();
		wireframeShader.SetMat4("view", view);
		wireframeShader.SetMat4("projection", ortho);

		// draw 3d grid of cubes.
		const unsigned int size = static_cast<unsigned int>(positions.size());
		for (unsigned int i = 0; i < size; ++i)
		{
			switch (visible[i])
			{
			case ContainmentType::Contains:
				wireframeShader.SetVec3("Color", glm::vec3(0.1f, 0.9f, 0.1f));
				break;
			case ContainmentType::Intersects:
				wireframeShader.SetVec3("Color", glm::vec3(1.0f, 1.0f, 1.0f));
				break;
			case ContainmentType::Disjoint:
				continue;
				wireframeShader.SetVec3("Color", glm::vec3(0.1f, 0.1f, 0.1f));
				break;
			default: break;
			}

			scratchTransform.SetPosition(positions[i]);
			wireframeShader.SetMat4("model", scratchTransform.GetModelMat());

			Primitives::RenderSphere(true);
			vertexCountStats += Primitives::sphere.GetVertexCount();
		}

		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
}

void IBLSpecState::RenderUI()
{
	DefaultState::RenderUI();

	ImGui::Begin("Rendering Stats");

	ImGui::Text("Vertex Count: %d", vertexCountStats);

	ImGui::End();


	ImGui::Begin("Camera Q/O Tree Magic");

	ImGui::Checkbox("Insert Camera Positions", &includeCamPosIntoTrees);
	ImGui::SliderFloat("Include Freq (s): %f", &includeFreq, 0.2f, 2.0f);

	ImGui::End();

	ImGui::Begin("Spatial Data Structures");

	ImGui::Checkbox("Show Quadtree", &showQtree);
	ImGui::Checkbox("Show Octree", &showOctree);

	ImGui::End();

	ImGui::Begin("IBL Scene Render Options");

	ImGui::Checkbox("Show TopDown Buffer", &renderTopDownBuffer);
	ImGui::Checkbox("Show Skybox", &drawSkybox);
	ImGui::Checkbox("Show Spheres", &showSpheres);
	ImGui::Checkbox("Show Spheres Textured", &showSpheresTextured);
	ImGui::Checkbox("Show Lines", &showLines);

	ImGui::End();
	

	ImGui::Begin("Show Buffers");
	const float screenRatio = m_windowParams.Height / static_cast<float>(m_windowParams.Width);

	ImGui::Columns(1, NULL, false);

	const float width = 250.0f;
	const float height = width * screenRatio;
	ImGui::Image((void*)(intptr_t)colorBuffer, ImVec2(width, height), ImVec2(0, 1), ImVec2(1, 0)); ImGui::NextColumn();

	ImGui::End();

	ImGui::Begin("CC Settings");

	ImGui::SliderFloat("Rotation Speed", &ccRotationSpeed, 0.0f, 100.0f);
	ImGui::SliderFloat("Near plane", &ccCamNearPlane, 0.1f, ccCamFarPlane);
	ImGui::SliderFloat("Far plane", &ccCamFarPlane, ccCamNearPlane, 1000.0f);
	ImGui::SliderFloat("FOV", &ccCamFov, 0.1f, 179.0f);

	cc.SetFov(ccCamFov);
	cc.SetNearFarPlane(ccCamNearPlane, ccCamFarPlane);

	ImGui::End();
}

void IBLSpecState::Cleanup()
{
	DefaultState::Cleanup();

}
