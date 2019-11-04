#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D albedoMap;

void main()
{
    FragColor = vec4(texture(albedoMap, TexCoords).xyz, 1.0);
}