#include "BoundingFrustum.h"

#include <iostream>
#include <vector>

#include "Plane.h"
#include "BoundingBox.h"

BoundingFrustum::BoundingFrustum()
{}

BoundingFrustum::BoundingFrustum(const glm::mat4& viewProjection)
{
	UpdateViewProjectionMatrix(viewProjection);
}

BoundingFrustum::~BoundingFrustum()
{}

void BoundingFrustum::UpdateViewProjectionMatrix(const glm::mat4& viewProjection)
{
	viewProj = viewProjection;
	CreatePlanes();
	CreateCorners();
}

bool BoundingFrustum::Intersects(const BoundingBox& aabb) const
{
	return Contains(aabb) != ContainmentType::Disjoint;
}

bool BoundingFrustum::Intersects(const BoundingFrustum& frustum) const
{
	return Contains(frustum) != ContainmentType::Disjoint;
}

PlaneIntersectionType BoundingFrustum::Intersects(const Plane& plane) const
{
	PlaneIntersectionType pit = plane.Intersects(corners[0]);
	for (int i = 1; i < corners.size(); ++i)
	{
		if (plane.Intersects(corners[i]) != pit)
		{
			pit = PlaneIntersectionType::Intersecting;
		}
	}
	return pit;
}

ContainmentType BoundingFrustum::Contains(const glm::vec3& point) const
{
	for (unsigned int i = 0; i < 6; ++i)
	{
		if (PlaneHelper::ClassifyPoint(point, planes[i]) > 0)
		{
			return ContainmentType::Disjoint;
		}
	}
	return ContainmentType::Contains;
}

ContainmentType BoundingFrustum::Contains(const BoundingBox& aabb) const
{
	bool intersects = false;
	for (unsigned int i = 0; i < 6; ++i)
	{
		PlaneIntersectionType planeIntersectionType = aabb.Intersects(planes[i]);
		switch (planeIntersectionType)
		{
		case PlaneIntersectionType::Front:
			return ContainmentType::Disjoint;
		case PlaneIntersectionType::Intersecting:
			intersects = true;
			break;
		}
	}

	return intersects ? ContainmentType::Intersects : ContainmentType::Contains;
}

ContainmentType BoundingFrustum::Contains(const BoundingFrustum& frustum) const
{
	bool intersects = false;
	for (unsigned int i = 0; i < 6; ++i)
	{
		PlaneIntersectionType planeIntersectionType = frustum.Intersects(planes[i]);
		switch (planeIntersectionType)
		{
		case PlaneIntersectionType::Front:
			return ContainmentType::Disjoint;
		case PlaneIntersectionType::Intersecting:
			intersects = true;
			break;
		}
	}
	return intersects ? ContainmentType::Intersects : ContainmentType::Contains;

	return ContainmentType();
}

void BoundingFrustum::CreateCorners()
{
	corners.resize(8);

	corners[0] = IntersectionPoint(planes[0], planes[2], planes[4]); // Near, Left, Top
	corners[1] = IntersectionPoint(planes[0], planes[3], planes[4]); // Near, Right, Top
	corners[2] = IntersectionPoint(planes[0], planes[3], planes[5]); // Near, Right, Bottom
	corners[3] = IntersectionPoint(planes[0], planes[2], planes[5]); // Near, Left, Bottom
	corners[4] = IntersectionPoint(planes[1], planes[2], planes[4]); // Far, Left, Top
	corners[5] = IntersectionPoint(planes[1], planes[3], planes[4]); // Far, Right, Top
	corners[6] = IntersectionPoint(planes[1], planes[3], planes[5]); // Far, Right, Bottom
	corners[7] = IntersectionPoint(planes[1], planes[2], planes[5]); // Far, Left, Bottom
}

void BoundingFrustum::CreatePlanes()
{
	planes.resize(6);

	planes[0] = Plane(-viewProj[0][2], -viewProj[1][2], -viewProj[2][2], -viewProj[3][2]);
	planes[1] = Plane(viewProj[0][2] - viewProj[0][3], viewProj[1][2] - viewProj[1][3], viewProj[2][2] - viewProj[2][3], viewProj[3][2] - viewProj[3][3]);
	planes[2] = Plane(-viewProj[0][3] - viewProj[0][0], -viewProj[1][3] - viewProj[1][0], -viewProj[2][3] - viewProj[2][0], -viewProj[3][3] - viewProj[3][0]);
	planes[3] = Plane(viewProj[0][0] - viewProj[0][3], viewProj[1][0] - viewProj[1][3], viewProj[2][0] - viewProj[2][3], viewProj[3][0] - viewProj[3][3]);
	planes[4] = Plane(viewProj[0][1] - viewProj[0][3], viewProj[1][1] - viewProj[1][3], viewProj[2][1] - viewProj[2][3], viewProj[3][1] - viewProj[3][3]);
	planes[5] = Plane(-viewProj[0][3] - viewProj[0][1], -viewProj[1][3] - viewProj[1][1], -viewProj[2][3] - viewProj[2][1], -viewProj[3][3] - viewProj[3][1]);

	planes[0].Normalize(); 	// Near
	planes[1].Normalize();	// Far
	planes[2].Normalize();	// Left
	planes[3].Normalize();	// Right
	planes[4].Normalize();	// Top
	planes[5].Normalize();	// Bottom
}

glm::vec3 BoundingFrustum::IntersectionPoint(const Plane & a, const Plane & b, const Plane & c)
{
	// Formula used
	//    d1 ( N2 * N3 ) + d2 ( N3 * N1 ) + d3 ( N1 * N2 )
	//P = ------------------------------------------------
	//    N1 . ( N2 * N3 )
	//
	// Note: N refers to the normal, d refers to the displacement. '.' means dot product. '*' means cross product

	glm::vec3 xbc = glm::cross(b.normal, c.normal);
	const float f = -glm::dot(a.normal, xbc);

	glm::vec3 v1 = xbc * a.d;

	glm::vec3 xca = glm::cross(c.normal, a.normal);
	glm::vec3 v2 = xca * b.d;

	glm::vec3 xab = glm::cross(a.normal, b.normal);
	glm::vec3 v3 = xca * c.d;

	return glm::vec3(
		(v1.x + v2.x + v3.x) / f,
		(v1.y + v2.y + v3.y) / f,
		(v1.z + v2.z + v3.z) / f
	);
}
