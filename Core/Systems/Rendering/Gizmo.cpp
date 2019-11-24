
#include "Gizmo.h"

void Gizmo::Render(Shader shader, glm::mat4 model, glm::vec3 pos, glm::vec3 forward, glm::vec3 up, glm::vec3 right)
{
	shader.Use();
	shader.SetMat4("model", glm::mat4(1.0f));
	shader.SetVec3("color", glm::vec3(0, 0, 1));
	Primitives::RenderLine(pos, pos + forward);
	shader.SetVec3("color", glm::vec3(1, 0, 0));
	Primitives::RenderLine(pos, pos + right);
	shader.SetVec3("color", glm::vec3(0, 1, 0));
	Primitives::RenderLine(pos, pos + up);
}
