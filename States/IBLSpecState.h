#pragma once

#include <random>

#include "DefaultState.h"

#include "Systems/Rendering/IBLSkybox.h"

#include "Systems/QuadTree.h"
#include "Systems/Octree.h"

class Material;

class IBLSpecState
	: public DefaultState
{
public:
	IBLSpecState();
	~IBLSpecState() override;

	void Init(Game* game) override;
	void HandleInput(SDL_Event* event) override;
	void Update(float deltaTime) override;
	void Render(float alpha = 1.0f) override;
	void RenderUI() override;
	void Cleanup() override;

private:
	QuadTree m_qTree;
	Octree m_oTree;
	bool showQtree = false;
	bool showOctree = false;

	Shader debugInstanced;
	Shader pbrShader;
	Shader backgroundShader;

	Shader wireframeShader;

	Material iron;
	Material gold;
	Material grass;
	Material plastic;
	Material wall;
	Material marble;
	Material granite;
	Material leather;

	Cube cube;

	IBLSkybox iblSkybox;

	unsigned int vertexCountStats = 0;

	int gX, gY, gZ;
	int gridSize = 50;
	float gridSpacing = 2.5f;
	std::vector<glm::vec3> positions;
	std::vector<ContainmentType> visible;
	Transform scratchTransform;

	unsigned int hdrFBO;
	unsigned int colorBuffer;
	unsigned int rboDepth;
	Camera topDownCamera;

	Camera cc;

	bool includeCamPosIntoTrees = false;
	float includeFreq = 0.3f;
	float camPushPosTime = 0.0f;

	bool renderTopDownBuffer = false;
	bool drawSkybox = true;
	bool showSpheres = false;
	bool showSpheresTextured = true;
	bool showLines = false;
};

