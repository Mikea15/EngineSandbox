
#include "QuadTree.h"

QuadTree::QuadTree()
	: QuadTree(glm::vec2(0.0f), 1.0f)
{
}

QuadTree::QuadTree(const glm::vec2& origin, float halfSize)
	: m_position(origin), m_halfSize(halfSize), m_storePos(0.0f)
	, m_topLeft(nullptr)
	, m_topRight(nullptr)
	, m_bottomLeft(nullptr)
	, m_bottomRight(nullptr)
{
	// point on quadTree plane
	m_bounds = Rect(m_position, halfSize);
}

QuadTree::~QuadTree()
{
}

void QuadTree::Subdivide()
{
	const glm::vec2 tl = (m_position + (m_position + glm::vec2(-m_halfSize, m_halfSize))) * 0.5f;
	const glm::vec2 tr = (m_position + (m_position + glm::vec2(m_halfSize, m_halfSize))) * 0.5f;
	const glm::vec2 bl = (m_position + (m_position + glm::vec2(-m_halfSize, -m_halfSize))) * 0.5f;
	const glm::vec2 br = (m_position + (m_position + glm::vec2(m_halfSize, -m_halfSize))) * 0.5f;

	m_topLeft = std::make_shared<QuadTree>(tl,		m_halfSize * 0.5f);
	m_topRight = std::make_shared<QuadTree>(tr,		m_halfSize * 0.5f);
	m_bottomLeft = std::make_shared<QuadTree>(bl,	m_halfSize * 0.5f);
	m_bottomRight = std::make_shared<QuadTree>(br,	m_halfSize * 0.5f);
}

bool QuadTree::Insert(const glm::vec3& pos)
{
	// use on the QuadTree Plane
	const glm::vec3 pos0 = glm::vec3(pos.x, 0.0f, pos.z);
	const glm::vec2 p = glm::vec2(pos.x, pos.z);
	if (!m_bounds.Intersect(p))
	{
		return false;
	}

	if (m_storePos == glm::vec3(0.0f) && m_topLeft == nullptr)
	{
		m_storePos = pos0;
		return true;
	}

	if (m_topLeft == nullptr)
	{
		Subdivide();
	}

	if (m_topLeft->Insert(pos0)) return true;
	if (m_topRight->Insert(pos0)) return true;
	if (m_bottomLeft->Insert(pos0)) return true;
	if (m_bottomRight->Insert(pos0)) return true;

	return false;
}

void QuadTree::Search(const Rect& range, std::vector<glm::vec3>& outResult)
{
	if (!m_bounds.Intersect(range))
	{
		return;
	}

	// check objects at this bounds level#
	if (range.Intersect(glm::vec2(m_storePos.x, m_storePos.z)))
	{
		outResult.push_back(m_storePos);
	}

	if (m_topLeft == nullptr)
	{
		return;
	}

	m_topLeft->Search(range, outResult);
	m_topRight->Search(range, outResult);
	m_bottomLeft->Search(range, outResult);
	m_bottomRight->Search(range, outResult);
}

void QuadTree::GetAllBoundingBoxes(std::vector<Rect>& outResult)
{
	outResult.push_back(m_bounds);

	if (m_topLeft == nullptr) return;

	m_topLeft->GetAllBoundingBoxes(outResult);
	m_topRight->GetAllBoundingBoxes(outResult);
	m_bottomLeft->GetAllBoundingBoxes(outResult);
	m_bottomRight->GetAllBoundingBoxes(outResult);
}

