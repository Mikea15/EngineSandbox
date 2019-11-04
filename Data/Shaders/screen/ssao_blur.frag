#version 330 core
out float FragColor;
precision mediump float;

in vec2 TexCoords;

uniform sampler2D ssaoInput;
uniform vec2 resolution;

float normpdf(in float x, in float sigma)
{
	return 0.39894*exp(-0.5*x*x/(sigma*sigma))/sigma;
}

void main() 
{
	bool useGaussianApprox = true;
	if( useGaussianApprox )
	{
		//declare stuff
		const int mSize = 11;
		const int kSize = (mSize-1)/2;
		float kernel[mSize];
		float final_colour = 0.0;
		
		//create the 1-D kernel
		float sigma = 7.0;
		float Z = 0.0;

		for (int j = 0; j < mSize; ++j)
		{
			kernel[j] = normpdf(float(j), sigma);
			Z += kernel[j];
		}
		
		//read out the texels
		for (int i = -kSize; i <= kSize; ++i)
		{
			for (int j = -kSize; j <= kSize; ++j)
			{
				final_colour += kernel[kSize + j] * kernel[kSize + i] 
				* texture(ssaoInput, TexCoords.xy + vec2(float(i), float(j)) / resolution.xy ).r;
			}
		}
		
		FragColor = final_colour/(Z*Z);
	}
	else
	{
		vec2 texelSize = 1.0 / vec2(textureSize(ssaoInput, 0));
		float result = 0.0;
		for (int x = -2; x < 2; ++x) 
		{
			for (int y = -2; y < 2; ++y) 
			{
				vec2 offset = vec2(float(x), float(y)) * texelSize;
				result += texture(ssaoInput, TexCoords + offset).r;
			}
		}
		FragColor = result / (4.0 * 4.0);
	}
}  