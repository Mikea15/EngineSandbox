#pragma once

#define GLM_ENABLE_EXPERIMENTAL

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/quaternion.hpp>

#include <vector>

#include "../Geometry/BoundingFrustum.h"

#include "CameraSnapshot.h"
#include "../Transform.h"

class Camera
{
public:
	struct Params
	{
		float m_fov = 70.0f;
		float m_nearPlane = 0.01f;
		float m_farPlane = 1000.0f;
		float m_aspectRatio = 16.0f / 9.0f;
		float m_orthoSize = 5.0f;

		bool m_isOrtho = false;

		bool operator==(const Params& rhs) {
			if (m_fov != rhs.m_fov) return false;
			if (m_nearPlane != rhs.m_nearPlane) return false;
			if (m_farPlane != rhs.m_farPlane) return false;
			if (m_aspectRatio != rhs.m_aspectRatio) return false;
			if (m_orthoSize != rhs.m_orthoSize) return false;
			if (m_isOrtho != rhs.m_isOrtho) return false;
			return true;
		}

		bool operator!=(const Params& rhs) {
			return !(*this == rhs);
		}
	};

	Camera();
	~Camera() = default;

	void Update(float deltaTime);
	void UpdateFov(float delta);
	void UpdateLookAt(const glm::vec2& mouseMovement);

	void Move(const glm::vec3& movement);

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

	const glm::mat4& GetView() const { return m_viewMatrix; }
	const glm::mat4& GetProjection() const { return m_projectionMatrix; }
	const glm::mat4& GetViewProjection() const { return m_projectionMatrix * m_viewMatrix; }
	const glm::mat4& OrthographicMatrix() const { return m_orthographicMatrix; }

	const glm::vec3& GetUp() const { return m_up; }
	const glm::vec3& GetForward() const { return m_forward; }
	const glm::vec3& GetRight() const { return m_right; }

	void ToggleOrthographicCamera();
	bool IsOrthographic() const { return m_params.m_isOrtho; }

	void LookAt(const glm::vec3& position);
	
	void NormalizeAngles();
		
	const BoundingFrustum& GetBoundingFrustum() const { return m_frustum; }

	const Params& GetParams() const { return m_params; }
	void SetParams(const Params& params);

	CameraSnapshot SaveCameraSnapshot();
	void InterpolateTo(CameraSnapshot b, float time);

private:
	glm::mat4 m_viewMatrix;
	glm::mat4 m_projectionMatrix;
	glm::mat4 m_orthographicMatrix;

	glm::vec3 m_forward;
	glm::vec3 m_up;
	glm::vec3 m_right;

	glm::vec3 m_position;

	float m_horizontalAngle;
	float m_verticalAngle;

	Params m_params;

	BoundingFrustum m_frustum;

	CameraSnapshot m_CSFrom;
	CameraSnapshot m_CSTo;
	float m_interpolationTime;
	float m_interpolationCurrentTime;
	bool m_isInterpolating = false;

private:
	static const float s_minFov;
	static const float s_maxFov;
	static const float s_maxVerticalAngle;
};

