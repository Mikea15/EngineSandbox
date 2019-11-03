#pragma once

#include <glm/glm.hpp>
#include <GL/glew.h>

#include "GeomDefines.h"

class BoundingFrustum;
class BoundingBox;

class Plane
{
public:
	Plane();
	Plane(const glm::vec3& a, const glm::vec3& b, const glm::vec3& c);
	Plane(float a, float b, float c, float d);
	Plane(const glm::vec3& pointOnPlane, const glm::vec3& normal);
	
	Plane(const Plane& copy);
	Plane(Plane&& move);
	Plane& operator=(const Plane& assign);

	~Plane();

	void Normalize();

	PlaneIntersectionType Intersects(const glm::vec3& point) const;
	PlaneIntersectionType Intersects(const BoundingBox& aabb) const;
	PlaneIntersectionType Intersects(const BoundingFrustum& frustum) const;

	float DotCoordinate(const glm::vec3& point) const;
	float DotNormal(const glm::vec3& point) const;

	glm::vec3 normal;
	float d;
};

namespace PlaneHelper
{
	static float ClassifyPoint(const glm::vec3& p, const Plane& plane)
	{
		return p.x * plane.normal.x + p.y * plane.normal.y + p.z * plane.normal.z + plane.d;
	}

	static float PerpendicularDistance(const glm::vec3& p, const Plane& plane)
	{
		// dist = (ax + by + cz + d) / sqrt(a*a + b*b + c*c)
		return static_cast<float>(fabsf((plane.normal.x * p.x + plane.normal.y * p.y + plane.normal.z * p.z)
			/ sqrtf(plane.normal.x * plane.normal.x + plane.normal.y * plane.normal.y + plane.normal.z * plane.normal.z)));
	}
}
