#pragma once

#include <memory>
#include <vector>

#include <glm/glm.hpp>

#include "Geometry/Rect.h"



class QuadTree
{
public:
	QuadTree();
	QuadTree(const glm::vec2& origin, float halfSize);
	~QuadTree();

	void Subdivide();
	bool Insert(const glm::vec3& pos);
	void Search(const Rect& range, std::vector<glm::vec3>& outResult);

	void GetAllBoundingBoxes(std::vector<Rect>& outResult);

private:
	glm::vec2 m_position;
	float m_halfSize;
	Rect m_bounds;

	glm::vec3 m_storePos;

	std::shared_ptr<QuadTree> m_topLeft;
	std::shared_ptr<QuadTree> m_topRight;
	std::shared_ptr<QuadTree> m_bottomLeft;
	std::shared_ptr<QuadTree> m_bottomRight;
};

