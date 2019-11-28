#version 330 core
out vec4 FragColor;

#include "common.frag"

void main()
{
    float colValue = clamp(1.0 - sin(time * 2.0f), 0.0, 1.0f);
    FragColor = vec4(colValue, colValue * 0.5, 0.3f, 1.0); // set alle 4 vector values to 1.0
}