#version 330 core

out vec4 FragColor;

in vec3 TexCoords;

uniform vec3 TopColor;
uniform float TopExponent;
uniform vec3 HorizonColor;
uniform vec3 BottomColor;
uniform float BottomExponent;

uniform float SkyIntensity;
uniform vec3 SunColor;
uniform float SunIntensity;
uniform float SunAlpha;
uniform float SunBeta;
uniform float SunAzimuth;
uniform float SunAltitude;

void main()
{
    vec3 v = normalize(TexCoords);

    float p = v.y;
    float p1 = 1 - pow(min(1, 1-p), TopExponent);
    float p3 = 1 - pow(min(1, 1+p), BottomExponent);
    float p2 = 1 - p1 - p3;

    vec3 SunDirection = vec3( 
        cos(radians(SunAzimuth)) * sin(radians(SunAltitude)), 
        sin(radians(SunAzimuth)),
        cos(radians(SunAzimuth)) * cos(radians(SunAltitude))
    );

    vec3 skyColor = TopColor * p1 + HorizonColor * p2 + BottomColor * p3;
    vec3 sunColor = SunColor * min( pow( max(0, dot(v, SunDirection)), SunAlpha) * SunBeta, 1);

    FragColor = vec4( skyColor * SkyIntensity + sunColor * SunIntensity, 0.0 );
}