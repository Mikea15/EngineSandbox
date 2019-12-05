

#include "FlyCamera.h"

FlyCamera::FlyCamera(glm::vec3 pos, glm::vec3 forward /*= glm::vec3(0, 0, -1)*/, glm::vec3 up /*= glm::vec3(0, 1, 0)*/)
	: Camera(pos, forward, up)
	, m_yaw(0.0f)
	, m_pitch(0.0f)
	, m_targetPitch(0.0f)
	, m_targetYaw(0.0f)
{
	m_worldUp = up;
	m_targetPosition = pos;
}

void FlyCamera::Update(float deltaTime)
{
	Camera::Update(deltaTime);

	m_position = glm::lerp(m_position, m_targetPosition, glm::clamp(deltaTime * m_damping, 0.0f, 1.0f));
	m_yaw = glm::lerp(m_yaw, m_targetYaw, glm::clamp(deltaTime * m_damping * 2.0f, 0.0f, 1.0f));
	m_pitch = glm::lerp(m_pitch, m_targetPitch, glm::clamp(deltaTime * m_damping * 2.0f, 0.0f, 1.0f));

	// calculate new cartesian basis vectors from yaw/pitch pair:
	const float radian = glm::radians(1.0f);

	glm::vec3 forward(
		cos(radian * m_pitch) * cos(radian * m_yaw),
		sin(radian * m_pitch),
		cos(radian * m_pitch) * sin(radian * m_yaw)
	);

	m_forward = glm::normalize(forward);
	m_right = glm::normalize(glm::cross(m_forward, m_worldUp));
	m_up = glm::cross(m_right, m_forward);

	UpdateView();
}

void FlyCamera::InputMouse(glm::vec2 delta)
{
	m_targetYaw		+= delta.x * m_mouseSensitivity;
	m_targetPitch	+= delta.y * m_mouseSensitivity;

	if (m_targetYaw == 0.0f) m_targetYaw = 0.01f;
	if (m_targetPitch == 0.0f) m_targetPitch = 0.01f;

	if (m_targetPitch > s_maxPitchAngle)
	{
		m_targetPitch = s_maxPitchAngle;
	}

	if (m_targetPitch < -s_maxPitchAngle)
	{
		m_targetPitch = -s_maxPitchAngle;
	}
}

void FlyCamera::InputKey(float deltaTime, glm::vec3 moveInput, bool boostSpeed)
{
	const float velocity = m_speed * (boostSpeed ? m_boostSpeedFactor : 1.0f) * deltaTime;

	glm::vec3 movement = m_right * moveInput.x + m_up * moveInput.y + m_forward * moveInput.z;

	m_targetPosition = m_targetPosition + movement * velocity;
}
	

void FlyCamera::InputScroll(float delta)
{
	m_params.m_fov += delta;
	if (m_params.m_fov < s_minFov)
	{
		m_params.m_fov = s_minFov;
	}
	else if (m_params.m_fov > s_maxFov)
	{
		m_params.m_fov = s_maxFov;
	}
}
