#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

uniform vec3 viewPos;
uniform vec2 uvScale = vec2(1, 1);

#include "common.frag"

float near = 0.1; 
float far  = 10.0; 

float LinearizeDepth(float depth)
{
	float z = depth * 2.0 - 1.0;
	return (2 * near * far) / (far + near - z * (far - near));
}

void main()
{    
	vec3 color = texture(material.diffuse, TexCoords).rgb;

	// ambient
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * vec3(0.0, 0.0, 0.6);
  	
    // diffuse 
    vec3 normal = normalize(Normal);
    vec3 lightDir = normalize(vec3(5.0, 15.0, -10.0) - FragPos);
    float diff = 0.5 + max(dot(normal, lightDir), 0.0);
    vec3 diffuse = diff * vec3(1.0, 1.0, 1.0);

	// specular
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, normal);
    
	float spec = 0.0;
    vec3 halfwayDir = normalize(lightDir + viewDir);  
    spec = pow(max(dot(normal, halfwayDir), 0.0), 64.0);
	
    vec3 specular = spec * vec3(1.0, 1.0, 1.0) * texture(material.specular, TexCoords).rgb;
	vec3 lighting = (ambient + (1.0) * (diffuse + specular)) * color;    
    
    FragColor = vec4(lighting, 1.0);
}