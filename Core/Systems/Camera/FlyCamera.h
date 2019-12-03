
#pragma once

#include "Camera.h"


class FlyCamera
	: public Camera
{
public:
	FlyCamera() = default;
	FlyCamera(glm::vec3 pos, glm::vec3 forward = glm::vec3(0, 0, -1), glm::vec3 up = glm::vec3(0, 1, 0));

	void Update(float deltaTime);

	void InputMouse(glm::vec2 delta);
	void InputKey(float deltaTime, glm::vec3 moveInput, bool boostSpeed);
	void InputScroll(float delta);

private:
	float m_yaw;
	float m_pitch;

	float m_targetYaw;
	float m_targetPitch;

	float m_speed = 50.0f;
	float m_mouseSensitivity = 0.1f;
	float m_damping = 5.0f;

	glm::vec3 m_worldUp;
	glm::vec3 m_targetPosition;
};