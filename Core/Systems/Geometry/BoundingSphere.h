#pragma once

#include <glm/glm.hpp>

class BoundingSphere
{
public:
	BoundingSphere();
	~BoundingSphere();



private:
	float m_radius;
	glm::vec3 m_origin;
};

