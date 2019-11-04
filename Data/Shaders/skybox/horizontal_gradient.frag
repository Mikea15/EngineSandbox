#version 330 core
out vec4 FragColor;

in vec3 TexCoords;

uniform vec3 Color1;
uniform vec3 Color2;
uniform vec3 UpVector;
uniform float Intensity;
uniform float Exponent;

void main()
{
    float d = dot( normalize(TexCoords), UpVector ) * 0.5f + 0.5f;

    FragColor = vec4( mix(Color1, Color2, pow(d, Exponent)) * Intensity, 0.0);
}
