#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>

#define GLM_ENABLE_EXPERIMENTAL

#include <glm/gtx/transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>


class Transform
{
public:
	Transform();
	Transform(const Transform& copy);

	void SetPosition(const glm::vec3& position);
	const glm::vec3& GetPosition() const { return m_position; }

	void SetScale(const glm::vec3& scale);
	const glm::vec3& GetScale() const { return m_scale; }

	void SetOrientation(const glm::quat& orientation);
	const glm::quat& GetOrientation() const { return m_orientation; }

	void Translate(const glm::vec3& deltaPosition);
	void RotateLocal(const glm::vec3& axis, float degrees);

	void Scale(float delta);
	void Scale(const glm::vec3& axis, float delta);

	const glm::vec3& GetUp() { return m_orientation * glm::vec3(0, 1, 0); }
	const glm::vec3& GetRight() { return m_orientation * glm::vec3(1, 0, 0); }
	const glm::vec3& GetForward() { return m_orientation * glm::vec3(0, 0, -1); }

	glm::mat4 GetModelMat();

private:
	glm::vec3 m_position;
	glm::vec3 m_scale;
	glm::quat m_orientation;
};

