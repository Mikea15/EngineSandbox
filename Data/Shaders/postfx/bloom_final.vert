#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoords;

out vec2 TexCoords;

uniform float renderScale;
uniform vec2 resolution;

void main()
{
    TexCoords = aTexCoords * renderScale;
    gl_Position = vec4(aPos, 1.0);
}