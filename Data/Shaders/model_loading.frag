#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D albedoMap;

float near = 0.1; 
float far  = 100.0; 

float LinearizeDepth(float depth)
{
	float z = depth * 2.0 - 1.0;
	return (2 * near * far) / (far + near - z * (far - near));
}

in VS_OUT
{
	vec2 TexCoords;
} fs_in;

void main()
{    
	// float depth = LinearizeDepth(gl_FragCoord.z) / far; // divide by far for demonstration
    // FragColor = vec4(vec3(depth), 1.0);
	
    FragColor = texture(albedoMap, TexCoords) * vec4(1.0, 1.0, 1.0, 1.0);
}