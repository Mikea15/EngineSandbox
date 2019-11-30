#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

uniform vec3 viewPos;
uniform vec2 uvScale = vec2(1, 1);

#include "common.frag"

void main()
{
    // properties
    vec3 normal = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);

    // phase 1: directional light
    vec3 light = CalcDirLight(dirLight, normal, viewDir);

    // phase 2: point lights
    //for( int i = 0; i < NR_POINT_LIGHTS; ++i )
    {
        light += CalcPointLight(pointLights[0], normal, FragPos, viewDir);
    }

    // phase 3: spot light
    light += CalcSpotLight(spotLight, normal, FragPos, viewDir);

    FragColor = vec4(light, 1.0);
} 
