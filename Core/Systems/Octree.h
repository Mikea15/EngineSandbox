#pragma once

#include <memory>
#include <vector>

#include <glm/glm.hpp>

#include "Geometry/BoundingBox.h"

class BoundingFrustum;


class Octree
{
public:
	Octree();
	Octree(const glm::vec3& position, float halfSize);
	~Octree();

	void Subdivide();
	bool Insert(const glm::vec3& pos);
	void Search(const BoundingBox& aabb, std::vector<glm::vec3>& outResult);
	void Search(const BoundingFrustum& frustum, std::vector<glm::vec3>& outResult);

	void GetAllBoundingBoxes(std::vector<BoundingBox>& outResult);

private:
	glm::vec3 m_position;
	float m_halfSize;
	BoundingBox m_bounds;

	glm::vec3 m_storePos;

	std::shared_ptr<Octree> m_upFrontLeft;
	std::shared_ptr<Octree> m_upFrontRight;
	std::shared_ptr<Octree> m_upBackLeft;
	std::shared_ptr<Octree> m_upBackRight;
	std::shared_ptr<Octree> m_downFrontLeft;
	std::shared_ptr<Octree> m_downFrontRight;
	std::shared_ptr<Octree> m_downBackLeft;
	std::shared_ptr<Octree> m_downBackRight;
};

