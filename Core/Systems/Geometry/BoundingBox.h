#pragma once

#include <vector>

#include <glm/glm.hpp>

#include "GeomDefines.h"

class Plane;
class BoundingFrustum;

class BoundingBox
{
public:
	BoundingBox();
	BoundingBox(const glm::vec3& origin, float halfSize);
	BoundingBox(const glm::vec3& origin, const glm::vec3& halfSize);
	BoundingBox(const glm::vec3& origin, const glm::vec3& min, const glm::vec3& max);
	~BoundingBox();

	ContainmentType Contains(const BoundingBox& box) const;
	ContainmentType Contains(const BoundingFrustum& frustum) const;
	ContainmentType Contains(const glm::vec3& point) const;

	//! Intersects BoundingSphere
	bool Intersects(const glm::vec3& point, float r) const;
	bool Intersects(const BoundingBox& aabb) const;
	bool Intersects(const BoundingFrustum& frustum) const;
	PlaneIntersectionType Intersects(const Plane& plane) const;

	const glm::vec3& GetPosition() const { return m_origin; }
	const float GetSize() const { return m_halfSize; }
	const float GetExtent() const { return fabsf(m_max.x - m_min.x); }

private:
	float m_halfSize;
	glm::vec3 m_origin;
	glm::vec3 m_min;
	glm::vec3 m_max;

	std::vector<glm::vec3> m_points;
};

