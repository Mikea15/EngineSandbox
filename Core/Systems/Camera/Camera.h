#pragma once

#define GLM_ENABLE_EXPERIMENTAL

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/compatibility.hpp>

#include <vector>

#include "../Geometry/BoundingFrustum.h"

#include "../Transform.h"

class Camera
{
public:
	struct Params
	{
		float m_fov = 70.0f;
		float m_aspectRatio = 16.0f / 9.0f;
		float m_nearPlane = 0.01f;
		float m_farPlane = 1000.0f;

		bool m_isPerspective = false;

		bool operator==(const Params& rhs) {
			if (m_fov != rhs.m_fov) return false;
			if (m_aspectRatio != rhs.m_aspectRatio) return false;
			if (m_nearPlane != rhs.m_nearPlane) return false;
			if (m_farPlane != rhs.m_farPlane) return false;
			if (m_isPerspective != rhs.m_isPerspective) return false;
			return true;
		}

		bool operator!=(const Params& rhs) {
			return !(*this == rhs);
		}
	};

	Camera();
	Camera(glm::vec3 pos, glm::vec3 forward = glm::vec3(0, 0, -1), glm::vec3 up = glm::vec3(0, 1, 0));
	~Camera() = default;

	void Update(float deltaTime);

	void SetPerspective(float fov, float aspect, float near, float far);
	void SetOrthographics(float left, float right, float top, float bottom, float near, float far);

	void UpdateView();

	const glm::vec3& GetPosition() const { return m_position; }
	void SetPosition(const glm::vec3& position) { m_position = position; }

	float GetFov() const { return m_params.m_fov; }
	void SetFov(float fov);

	float GetNearPlane() const { return m_params.m_nearPlane; }
	float GetFarPlane() const { return m_params.m_farPlane; }
	void SetNearFarPlane(float nearPlane, float farPlane);

	float GetAspectRatio() const { return m_params.m_aspectRatio; }
	void SetAspectRatio(float ratio);

	bool IsInFieldOfView(const glm::vec3& position) const;

	const glm::mat4& GetView() const { return m_view; }
	const glm::mat4& GetProjection() const { return m_projection; }
	const glm::mat4& GetViewProjection() const { return m_projection * m_view; }

	const glm::vec3& GetUp() const { return m_up; }
	const glm::vec3& GetForward() const { return m_forward; }
	const glm::vec3& GetRight() const { return m_right; }

	bool IsOrthographic() const { return m_params.m_isPerspective; }


	const BoundingFrustum& GetBoundingFrustum() const { return m_frustum; }

	const Params& GetParams() const { return m_params; }
	void SetParams(const Params& params);

protected:
	glm::mat4 m_view;
	glm::mat4 m_projection;

	glm::vec3 m_position;
	glm::vec3 m_forward;
	glm::vec3 m_up;
	glm::vec3 m_right;

	Params m_params;

	BoundingFrustum m_frustum;

	static const float s_minFov;
	static const float s_maxFov;
	static const float s_maxPitchAngle;
};

