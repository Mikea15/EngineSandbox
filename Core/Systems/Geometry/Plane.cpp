#include "Plane.h"

#include "BoundingBox.h"
#include "BoundingFrustum.h"

Plane::Plane()
	: normal(glm::vec3(0.0f)), d(0.0f)
{}

Plane::Plane(const glm::vec3& a, const glm::vec3& b, const glm::vec3& c)
{
	glm::vec3 ab = b - a;
	glm::vec3 ac = c - a;

	glm::vec3 cross = glm::cross(ab, ac);

	normal = glm::normalize(cross);
	d = -(glm::dot(normal, a));
}

Plane::Plane(float a, float b, float c, float d)
	: normal(glm::vec3(a, b, c)), d(d)
{}

Plane::Plane(const glm::vec3& pointOnPlane, const glm::vec3& normal)
{
	this->normal = normal;
	d = -(pointOnPlane.x * normal.x + pointOnPlane.y * normal.y + pointOnPlane.z * normal.z);
}

Plane::Plane(const Plane& copy)
	: normal(copy.normal), d(copy.d)
{ }

Plane::Plane(Plane&& move)
	: normal(move.normal), d(move.d)
{ }

Plane& Plane::operator=(const Plane & assign)
{
	normal = assign.normal;
	d = assign.d;
	return *this;
}

Plane::~Plane()
{ }

void Plane::Normalize()
{
	const float length = glm::length(normal);
	if (length > 0.0f)
	{
		const float factor = 1.0f / length;
		normal = normal * factor;
		d = d * factor;
	}
}

PlaneIntersectionType Plane::Intersects(const glm::vec3& point) const
{
	float distance = DotCoordinate(point);
	if (distance > 0) 
	{
		return PlaneIntersectionType::Front;
	}
	if (distance < 0) 
	{
		return PlaneIntersectionType::Back;
	}
	return PlaneIntersectionType::Intersecting;
}


PlaneIntersectionType Plane::Intersects(const BoundingBox& aabb) const
{
	return aabb.Intersects(*this);
}

PlaneIntersectionType Plane::Intersects(const BoundingFrustum& frustum) const
{
	return frustum.Intersects(*this);
}

float Plane::DotCoordinate(const glm::vec3& point) const
{
	return ((((normal.x * point.x) + (normal.y * point.y)) + (normal.z * point.z)) + d);
}

float Plane::DotNormal(const glm::vec3 & point) const
{
	return (((normal.x * point.x) + (normal.y * point.y)) + (normal.z * point.z));
}


