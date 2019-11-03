#pragma once

#include <glm/glm.hpp>
#include <glm/mat4x4.hpp>

#include <GL/glew.h>

#include <vector>

#include "Plane.h"

class BoundingBox;

class BoundingFrustum
{
	enum
	{
		Top = 0,
		Bottom,
		Left,
		Right,
		Near,
		Far
	};

public:
	BoundingFrustum();
	// (view * projection)
	BoundingFrustum(const glm::mat4& viewProjection);

	~BoundingFrustum();

	void UpdateViewProjectionMatrix(const glm::mat4& viewProjection);

	bool Intersects(const BoundingBox& aabb) const;
	bool Intersects(const BoundingFrustum& frustum) const;
	PlaneIntersectionType Intersects(const Plane& plane) const;

	ContainmentType Contains(const glm::vec3& point) const;
	ContainmentType Contains(const BoundingBox& aabb) const;
	ContainmentType Contains(const BoundingFrustum& frustum) const;

	void CreateCorners();
	void CreatePlanes();

	glm::vec3 IntersectionPoint(const Plane& a, const Plane& b, const Plane& c);

	const std::vector<glm::vec3> GetCorners() const { return corners; }

private:
	glm::mat4 viewProj;
	std::vector<glm::vec3> corners;
	std::vector<Plane> planes;
};

