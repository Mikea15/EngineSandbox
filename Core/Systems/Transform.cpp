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
	m_isDirty = true;
}

void Transform::SetRotation(const glm::vec4& rotation)
{
	m_rotation = rotation;
	m_isDirty = true;
}

void Transform::SetScale(const glm::vec3& scale)
{
	m_scale = scale;
	m_isDirty = true;
}

void Transform::SetScale(float scale)
{
	m_scale = glm::vec3(scale);
	m_isDirty = true;
}

const glm::vec3 Transform::GetLocalPosition()
{
	return m_position;
}

const glm::vec4 Transform::GetLocalRotation()
{
	return m_rotation;
}

const glm::quat Transform::GetOrientation()
{
	return m_orientation;
}

const glm::vec3 Transform::GetLocalScale()
{
	return m_scale;
}

const glm::vec3 Transform::GetWorldPosition()
{
	glm::mat4 transform = GetTransform();
	glm::vec4 pos = transform * glm::vec4(m_position, 1.0f);
	return pos;
}

const glm::vec3 Transform::GetWorldScale()
{
	glm::mat4 transform = GetTransform();
	glm::vec3 scale = glm::vec3(transform[0][0], transform[1][1], transform[2][2]);
	if (scale.x < 0.0f) scale.x *= 1.0f;
	if (scale.y < 0.0f) scale.y *= 1.0f;
	if (scale.z < 0.0f) scale.z *= 1.0f;
	return scale;
}

const glm::mat4 Transform::GetTransform()
{
	if (m_isDirty)
	{
		UpdateTransform();
	}
	return m_transform;
}

const glm::mat4 Transform::GetPrevTransform()
{
	return m_prevTransform;
}

void Transform::UpdateTransform()
{
	m_prevTransform = m_transform;

	if (m_isDirty)
	{
		m_transform = glm::translate(m_position);
		m_transform = glm::scale(m_transform, m_scale);
		// m_transform = glm::rotate(m_transform, glm::vec3(m_rotation), m_rotation.a);

		m_isDirty = false;
	}
}

void Transform::SetOrientation(const glm::quat& orientation)
{
	m_orientation = orientation;
}

void Transform::Translate(const glm::vec3& deltaPosition)
{
	SetPosition(m_position + deltaPosition);
}

void Transform::RotateLocal(const glm::vec3& axis, float degrees)
{
	glm::quat rot = glm::angleAxis(glm::radians(degrees), axis);
	m_orientation = m_orientation * rot;
	m_orientation = glm::normalize(m_orientation);
}

void Transform::RenderGizmo(Shader shader)
{
	Gizmo::Render(shader, GetTransform(), GetLocalPosition(), GetForward(), GetUp(), GetRight());
}

