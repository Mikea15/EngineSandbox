#pragma once

#include <glm/glm.hpp>

struct CameraSnapshot
{
	float fov;
	glm::vec3 position;
	float horizontalRotation;
	float verticalRotation;

	CameraSnapshot& operator-=(const CameraSnapshot& rhs)
	{
		fov -= rhs.fov;
		position -= rhs.position;
		horizontalRotation -= rhs.horizontalRotation;
		verticalRotation -= rhs.verticalRotation;
		return *this;
	}

	CameraSnapshot& operator-(const CameraSnapshot& rhs)
	{
		fov -= rhs.fov;
		position -= rhs.position;
		horizontalRotation -= rhs.horizontalRotation;
		verticalRotation -= rhs.verticalRotation;
		return *this;
	}

	CameraSnapshot& operator+(const CameraSnapshot& rhs)
	{
		fov += rhs.fov;
		position += rhs.position;
		horizontalRotation += rhs.horizontalRotation;
		verticalRotation += rhs.verticalRotation;
		return *this;
	}

	CameraSnapshot& operator*=(const CameraSnapshot& rhs)
	{
		fov *= rhs.fov;
		position *= rhs.position;
		horizontalRotation *= rhs.horizontalRotation;
		verticalRotation *= rhs.verticalRotation;
		return *this;
	}

	CameraSnapshot& operator*(float rhs)
	{
		fov *= rhs;
		position *= rhs;
		horizontalRotation *= rhs;
		verticalRotation *= rhs;
		return *this;
	}
};

class CameraSnapshotInterpolator
{
public:
	static CameraSnapshot Interpolate(CameraSnapshot a, CameraSnapshot b, float progress)
	{
		return a + (b - a) * progress;
	}
};
