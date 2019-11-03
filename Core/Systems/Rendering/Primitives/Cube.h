#pragma once

#include <GL/glew.h>

class Cube
{
public:
	Cube();

	void Init();
	void Draw();
	void DrawInstanced(unsigned int count);

	unsigned int GetVertexCount() const { return 36; }

private:
	unsigned int m_VAO;
	unsigned int m_VBO;
};