#version 330 core
out vec4 FragColor;

in vec3 Normal;
in vec3 Position;
in float RefractionIndex;

uniform vec3 cameraPos;
uniform samplerCube skybox;

// Air		1.00
// Water	1.33
// Ice		1.309
// Glass	1.52
// Diamond	2.42

void main()
{    
    float ratio = 1.00 / RefractionIndex;
    vec3 I = normalize(Position - cameraPos);
    vec3 R = refract(I, normalize(Normal), ratio);
    FragColor = vec4(texture(skybox, R).rgb, 1.0);
}