#version 330 core
#include "common.vert"

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoords;

void main()
{
    FragPos = vec3(model * vec4(aPos, 1.0));
    Normal = mat3(transpose(inverse(model))) * aNormal;
	TexCoords = aTexCoords;

    gl_Position = projection * view * vec4(FragPos, 1.0);
}
