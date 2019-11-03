#include "Octree.h"


#include "Geometry/BoundingFrustum.h"

Octree::Octree()
	: Octree(glm::vec3(0.0f), 0.0f)
{
}


Octree::Octree(const glm::vec3& position, float halfSize)
	: m_position(position), m_halfSize(halfSize)
	, m_upFrontLeft(nullptr)
	, m_upFrontRight(nullptr)
	, m_upBackLeft(nullptr)
	, m_upBackRight(nullptr)
	, m_downFrontLeft(nullptr)
	, m_downFrontRight(nullptr)
	, m_downBackLeft(nullptr)
	, m_downBackRight(nullptr)
{
	m_storePos = glm::vec3(0.0f);
	m_bounds = BoundingBox(m_position, halfSize);
}

Octree::~Octree()
{
}

void Octree::Subdivide()
{
	const glm::vec3 ufl = (m_position + (m_position + glm::vec3(-m_halfSize, m_halfSize, m_halfSize))) * 0.5f;
	const glm::vec3 ufr = (m_position + (m_position + glm::vec3(m_halfSize, m_halfSize, m_halfSize))) * 0.5f;
	const glm::vec3 ubl = (m_position + (m_position + glm::vec3(-m_halfSize, m_halfSize, -m_halfSize))) * 0.5f;
	const glm::vec3 ubr = (m_position + (m_position + glm::vec3(m_halfSize, m_halfSize, -m_halfSize))) * 0.5f;

	const glm::vec3 dfl = (m_position + (m_position + glm::vec3(-m_halfSize, -m_halfSize, m_halfSize))) * 0.5f;
	const glm::vec3 dfr = (m_position + (m_position + glm::vec3(m_halfSize, -m_halfSize, m_halfSize))) * 0.5f;
	const glm::vec3 dbl = (m_position + (m_position + glm::vec3(-m_halfSize, -m_halfSize, -m_halfSize))) * 0.5f;
	const glm::vec3 dbr = (m_position + (m_position + glm::vec3(m_halfSize, -m_halfSize, -m_halfSize))) * 0.5f;

	m_upFrontLeft = std::make_shared<Octree>(ufl, m_halfSize * 0.5f);
	m_upFrontRight = std::make_shared<Octree>(ufr, m_halfSize * 0.5f);
	m_upBackLeft = std::make_shared<Octree>(ubl, m_halfSize * 0.5f);
	m_upBackRight = std::make_shared<Octree>(ubr, m_halfSize * 0.5f);
	m_downFrontLeft = std::make_shared<Octree>(dfl, m_halfSize * 0.5f);
	m_downFrontRight = std::make_shared<Octree>(dfr, m_halfSize * 0.5f);
	m_downBackLeft = std::make_shared<Octree>(dbl, m_halfSize * 0.5f);
	m_downBackRight = std::make_shared<Octree>(dbr, m_halfSize * 0.5f);
}

bool Octree::Insert(const glm::vec3& position)
{
	if( m_bounds.Contains(position) == ContainmentType::Disjoint )
	{
		return false;
	}

	if (m_upFrontLeft == nullptr && m_storePos == glm::vec3(0.0f))
	{
		m_storePos = position;
		return true;
	}

	if (m_upFrontLeft == nullptr)
	{
		Subdivide();
	}

	if (m_upFrontLeft->Insert(position)) return true;
	if (m_upFrontRight->Insert(position)) return true;
	if (m_upBackLeft->Insert(position)) return true;
	if (m_upBackRight->Insert(position)) return true;
	if (m_downFrontLeft->Insert(position)) return true;
	if (m_downFrontRight->Insert(position)) return true;
	if (m_downBackLeft->Insert(position)) return true;
	if (m_downBackRight->Insert(position)) return true;

	return false;
}

void Octree::Search(const BoundingBox& aabb, std::vector<glm::vec3>& outResult)
{
	if (m_bounds.Contains(aabb) == ContainmentType::Disjoint)
	{
		return;
	}

	// check objects at this bounds level#
	if (aabb.Contains(m_storePos) != ContainmentType::Disjoint)
	{
		outResult.push_back(m_storePos);
	}

	if (m_upFrontLeft == nullptr)
	{
		return;
	}

	m_upFrontLeft->Search(aabb, outResult);
	m_upFrontRight->Search(aabb, outResult);
	m_upBackLeft->Search(aabb, outResult);
	m_upBackRight->Search(aabb, outResult);

	m_downFrontLeft->Search(aabb, outResult);
	m_downFrontRight->Search(aabb, outResult);
	m_downBackLeft->Search(aabb, outResult);
	m_downBackRight->Search(aabb, outResult);
}

void Octree::Search(const BoundingFrustum& frustum, std::vector<glm::vec3>& outResult)
{
	if (frustum.Contains(m_bounds) == ContainmentType::Disjoint)
	{
		return;
	}

	// check objects at this bounds level#
	// while no BoundingSphere, this BoundingBox is a Generous aproximation of the spheres.
	// if (frustum.Contains(BoundingBox(m_storePos, 1.0f)) != ContainmentType::Disjoint)
	if (frustum.Contains(m_storePos) != ContainmentType::Disjoint)
	{
		outResult.push_back(m_storePos);
	}

	if (m_upFrontLeft == nullptr)
	{
		return;
	}

	m_upFrontLeft->Search(frustum, outResult);
	m_upFrontRight->Search(frustum, outResult);
	m_upBackLeft->Search(frustum, outResult);
	m_upBackRight->Search(frustum, outResult);

	m_downFrontLeft->Search(frustum, outResult);
	m_downFrontRight->Search(frustum, outResult);
	m_downBackLeft->Search(frustum, outResult);
	m_downBackRight->Search(frustum, outResult);
}

void Octree::GetAllBoundingBoxes(std::vector<BoundingBox>& outResult)
{
	outResult.push_back(m_bounds);

	if (m_upFrontLeft == nullptr) return;

	m_upFrontLeft->GetAllBoundingBoxes(outResult);
	m_upFrontRight->GetAllBoundingBoxes(outResult);
	m_upBackLeft->GetAllBoundingBoxes(outResult);
	m_upBackRight->GetAllBoundingBoxes(outResult);

	m_downFrontLeft->GetAllBoundingBoxes(outResult);
	m_downFrontRight->GetAllBoundingBoxes(outResult);
	m_downBackLeft->GetAllBoundingBoxes(outResult);
	m_downBackRight->GetAllBoundingBoxes(outResult);
}
