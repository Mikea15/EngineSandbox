#version 330 core
out vec4 FragColor;

in vec3 TexCoords;

uniform samplerCube skybox;

void main()
{   
	vec3 coords = TexCoords;
	coords.x = 1.0 - coords.x;
    FragColor = texture(skybox, TexCoords);
}