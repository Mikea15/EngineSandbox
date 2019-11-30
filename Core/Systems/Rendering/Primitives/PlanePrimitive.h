#pragma once

#include <GL/glew.h>

class PlanePrimitive
{
public:
	PlanePrimitive();

	void Init();
	void Draw();
	void DrawInstanced(unsigned int count);

	unsigned int GetVertexCount() const { return 4; }

private:
	unsigned int m_VAO;
	unsigned int m_VBO;
};