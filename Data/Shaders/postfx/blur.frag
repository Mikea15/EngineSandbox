#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D image;

uniform vec2 resolution;
uniform float radius;

uniform bool horizontal;
uniform bool gauss;
uniform float weight[5] = float[] (0.2270270270, 0.1945945946, 0.1216216216, 0.0540540541, 0.0162162162);

float normpdf(in float x, in float sigma)
{
	return 0.39894*exp(-0.5*x*x/(sigma*sigma))/sigma;
}

void main()
{
        //this will be our RGBA sum
	    vec4 sum = vec4(0.0);
    
	    //our original texcoord for this fragment
	    vec2 tc = TexCoords;
    
	    //the amount to blur, i.e. how far off center to sample from 
	    //1.0 -> blur by one pixel
	    //2.0 -> blur by two pixels, etc.
	    float blurH = radius/resolution.x;
        float blurV = radius/resolution.y; 
    
	    //the direction of our blur
	    //(1.0, 0.0) -> x-axis blur
	    //(0.0, 1.0) -> y-axis blur
	    float hstep = horizontal ? 1.0 : 0.0;
	    float vstep = horizontal ? 0.0 : 1.0;
    
	    //apply blurring, using a 9-tap filter with predefined gaussian weights
	    sum += texture2D(image, vec2(tc.x - 4.0*blurH*hstep, tc.y - 4.0*blurV*vstep)) * 0.0162162162;
	    sum += texture2D(image, vec2(tc.x - 3.0*blurH*hstep, tc.y - 3.0*blurV*vstep)) * 0.0540540541;
	    sum += texture2D(image, vec2(tc.x - 2.0*blurH*hstep, tc.y - 2.0*blurV*vstep)) * 0.1216216216;
	    sum += texture2D(image, vec2(tc.x - 1.0*blurH*hstep, tc.y - 1.0*blurV*vstep)) * 0.1945945946;
    
	    sum += texture2D(image, vec2(tc.x, tc.y)) * 0.2270270270;
    
	    sum += texture2D(image, vec2(tc.x + 1.0*blurH*hstep, tc.y + 1.0*blurV*vstep)) * 0.1945945946;
	    sum += texture2D(image, vec2(tc.x + 2.0*blurH*hstep, tc.y + 2.0*blurV*vstep)) * 0.1216216216;
	    sum += texture2D(image, vec2(tc.x + 3.0*blurH*hstep, tc.y + 3.0*blurV*vstep)) * 0.0540540541;
	    sum += texture2D(image, vec2(tc.x + 4.0*blurH*hstep, tc.y + 4.0*blurV*vstep)) * 0.0162162162;

	    //discard alpha for our simple demo, multiply by vertex color and return
	    FragColor = vec4(sum.rgb, 1.0);
}