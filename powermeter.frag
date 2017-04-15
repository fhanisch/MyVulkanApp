#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 color;
layout(location = 1) in vec2 texCoords;

layout(location = 0) out vec4 outColor;

const float pi = 3.141592653589793;

void main()
{
	vec4 farbe;
	float x = texCoords.x;
	float y = -texCoords.y;
	float R = sqrt(pow(abs(x),2.0)+pow(abs(y),2.0));
	float phi = asin(y/R);	

	if (R<=0.75 && R>=0.2)
	{
		farbe=vec4(0.0,0.0,1.0-R,1.0);
	}
	else if (R>0.75 && R<=1.0 && phi>=-pi/4.0 && phi<=pi/2.0 && x<=0.0)
	{
		farbe=vec4((phi+pi/2.0)/pi,1.0,0.0,1.0);
	}
	else if (R>0.75 && R<=1.0 && phi>=-pi/4.0 && phi<=pi/2.0 && x>=0.0)
	{
		farbe=vec4(1.0,(phi+pi/2.0)/pi,0.0,1.0);
	}	
	else
	{
		farbe=vec4(0.2,0.2,0.2,1.0);
	}

	outColor = farbe;
}