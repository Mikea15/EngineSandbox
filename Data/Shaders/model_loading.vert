#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

#include "common.vert"

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    vs_out.TexCoords = aTexCoords;    
    gl_Position = projection * view * model * vec4(aPos, 1.0);
}
