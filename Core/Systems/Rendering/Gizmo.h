
#pragma once

#include "Primitives.h"
#include "Renderer/Shader.h"

class Gizmo
{
public:
	static void Render(const Shader* shader, glm::mat4 model, glm::vec3 pos, glm::vec3 forward, glm::vec3 up, glm::vec3 right);
};
