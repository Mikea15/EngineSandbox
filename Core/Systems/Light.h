#pragma once

#include "Transform.h"

struct Light
{
	glm::vec3 Position;
	glm::vec3 Direction;
	glm::vec3 DiffuseColor;
	glm::vec3 SpecularColor;
	glm::vec3 AmbientColor;
	float Constant;
	float Linear;
	float Quadratic;
};

