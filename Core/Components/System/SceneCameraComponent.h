#pragma once

#include "../../SystemComponentManager.h"

#include "../../Systems/Camera/Camera.h"
#include "../../Systems/Camera/FlyCamera.h"

#include "../../WindowParams.h"

class SceneCameraComponent
	: public SystemComponent
{
	CLASS_DECLARATION(SceneCameraComponent)

public:
	SceneCameraComponent();
	~SceneCameraComponent();

	void Initialize(Game* game) override;
	void HandleInput(SDL_Event* event) override;
	void PreUpdate(float frameTime) override;
	void Update(float deltaTime) override;
	void Render(float alpha) override;
	void RenderUI() override;
	void Cleanup() override;

	Camera& GetCamera() { return static_cast<Camera>(m_camera); }

private:
	WindowParams m_windowParams;

	FlyCamera m_camera;

	float m_cameraMovementSpeedBoostMult = 5.0f;

	glm::vec2 m_mousePosition;
	glm::vec2 m_mouseSensitivity;

	int m_fovInputChange = 0;
	float m_fovChange = 0.0f;
	float m_fovVelocity = 100.0f;
	float m_fovSensitivity = 1.0f;
	float m_fovDamping = 0.85f;

	bool m_inputGrabMouse = false;
	float m_inputMoveUp = 0.0f;
	float m_inputMoveRight = 0.0f;
	float m_inputMoveForward = 0.0f;

	bool m_inputEnableMovementBoost = false;
};