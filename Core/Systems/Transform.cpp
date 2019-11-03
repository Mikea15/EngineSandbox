#include "Transform.h"

Transform::Transform()
	: m_position(0.0f)
	, m_scale(1.0f)
{
	m_orientation = glm::quat(1.0f, 0, 0, 0);
}

Transform::Transform(const Transform& copy)
	: m_position(copy.m_position)
	, m_scale(copy.m_scale)
	, m_orientation(copy.m_orientation)
{
}

void Transform::SetPosition(const glm::vec3& position)
{
	m_position = position;
}

void Transform::SetScale(const glm::vec3& scale)
{
	m_scale = scale;
}

void Transform::SetOrientation(const glm::quat& orientation)
{
	m_orientation = orientation;
}

void Transform::Translate(const glm::vec3& deltaPosition)
{
	m_position += deltaPosition;
}

void Transform::RotateLocal(const glm::vec3& axis, float degrees)
{
	glm::quat rot = glm::angleAxis(glm::radians(degrees), axis);
	m_orientation = m_orientation * rot;
	m_orientation = glm::normalize(m_orientation);
}

void Transform::Scale(float delta)
{
	m_scale *= delta;
}

void Transform::Scale(const glm::vec3& axis, float delta)
{
	m_scale += axis * delta;
}

glm::mat4 Transform::GetModelMat()
{
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, m_position);
	model = model * glm::toMat4(m_orientation);
	model = glm::scale(model, m_scale);
	return model;
}

