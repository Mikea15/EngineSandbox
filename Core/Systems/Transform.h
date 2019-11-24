#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>

#define GLM_ENABLE_EXPERIMENTAL

#include <glm/gtx/transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

#include "Rendering/Gizmo.h"
#include "Rendering/Shader.h"

class Transform
{
public:
	Transform();
	Transform(const Transform& copy);

	void SetPosition(const glm::vec3& position);
	void SetRotation(const glm::vec4& rotation);
	void SetOrientation(const glm::quat& orientation);
	void SetScale(float scale);
	void SetScale(const glm::vec3& scale);

	const glm::vec3 GetLocalPosition();
	const glm::vec4 GetLocalRotation();
	const glm::quat GetOrientation();
	const glm::vec3 GetLocalScale();

	const glm::vec3 GetWorldPosition();
	const glm::vec3 GetWorldScale();

	const glm::mat4 GetTransform();
	const glm::mat4 GetPrevTransform();

	void UpdateTransform();

	void Translate(const glm::vec3& deltaPosition);
	void RotateLocal(const glm::vec3& axis, float degrees);

	const glm::vec3& GetUp() { return m_orientation * glm::vec3(0, 1, 0); }
	const glm::vec3& GetRight() { return m_orientation * glm::vec3(1, 0, 0); }
	const glm::vec3& GetForward() { return m_orientation * glm::vec3(0, 0, 1); }

	void RenderGizmo(Shader shader);

private:
	glm::mat4 m_transform;
	glm::mat4 m_prevTransform;

	glm::vec3 m_position;
	glm::vec4 m_rotation;
	glm::vec3 m_scale;

	glm::quat m_orientation;

	bool m_isDirty;
};

