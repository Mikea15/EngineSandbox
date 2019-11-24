
#include "Camera.h"

#include <iostream>

#include "../Transform.h"


//must be less than 90 to avoid gimbal lock
const float Camera::s_maxPitchAngle = 85.0f;
const float Camera::s_minFov = 0.1f;
const float Camera::s_maxFov = 179.0f;

Camera::Camera()
	: m_view(1.0f)
	, m_projection(1.0f)
	, m_position(0.0f, 0.0f, 0.0f)
	, m_forward(0.0f, 0.0f, -1.0f)
	, m_up(0.0f, 1.0f, 0.0f)
	, m_right(1.0f, 0.0f, 0.0f)
{
	UpdateView();
	m_frustum = BoundingFrustum(m_view);
}

Camera::Camera(glm::vec3 pos, glm::vec3 forward, glm::vec3 up)
	: m_view(1.0f)
	, m_projection(1.0f)
	, m_position(pos)
	, m_forward(forward)
	, m_up(up)
	, m_right(1.0f, 0.0f, 0.0f)
{
	UpdateView();
	m_frustum = BoundingFrustum(m_view);
}

void Camera::Update(float deltaTime)
{
	m_frustum.Update(m_projection * m_view);
}

void Camera::SetPerspective(float fov, float aspect, float near, float far)
{
	m_params.m_isPerspective = true;
	m_params.m_fov = fov;
	m_params.m_aspectRatio = aspect;
	m_params.m_nearPlane = near;
	m_params.m_farPlane = far;

	m_projection = glm::perspective(fov, aspect, near, far);
}

void Camera::SetOrthographics(float left, float right, float top, float bottom, float near, float far)
{
	m_params.m_isPerspective = false;
	m_params.m_nearPlane = near;
	m_params.m_farPlane = far;
	m_projection = glm::ortho(left, right, bottom, top, near, far);
}

void Camera::UpdateView()
{
	m_view = glm::lookAt(m_position, m_position + m_forward, m_up);
}

void Camera::SetFov(float fov)
{
	if (fov < s_minFov)
	{
		fov = s_minFov;
	}
	else if (fov > s_maxFov)
	{
		fov = s_maxFov;
	}
	m_params.m_fov = fov;
}

void Camera::SetNearFarPlane(float nearPlane, float farPlane)
{
	m_params.m_nearPlane = nearPlane;
	m_params.m_farPlane = farPlane;
}

void Camera::SetAspectRatio(float ratio)
{
	m_params.m_aspectRatio = ratio;
}

bool Camera::IsInFieldOfView(const glm::vec3& position) const
{
	const glm::vec3 camToObj = position - m_position;
	const float distSq = static_cast<float>(camToObj.length() * camToObj.length());
	if (distSq > m_params.m_farPlane * m_params.m_farPlane)
	{
		return false;
	}
	if (distSq < m_params.m_nearPlane * m_params.m_nearPlane)
	{
		return false;
	}

	const glm::vec3 camToObjDir = glm::normalize(camToObj);
	const float dot = glm::dot(camToObjDir, m_forward);
	if (dot > 0)
	{
		const float angle = glm::degrees(acosf(dot));
		return angle <= m_params.m_fov;
	}
	return false;
}


void Camera::SetParams(const Params & params)
{
	if (m_params != params)
	{
		m_params = params;
	}
}
