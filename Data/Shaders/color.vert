#version 330 core
#include "common.vert"

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
	vs_out.Normal = mat3(transpose(inverse(model))) * aNormal;
	gl_Position = projection * view * model * vec4(aPos, 1.0);
}