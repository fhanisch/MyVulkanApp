#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 0) uniform UniformBufferObject {
    mat4 mModel;
    mat4 mView;
    mat4 mProj;
} ubo;

layout(location = 0) in vec2 mesh;

layout(location = 0) out vec3 color;
layout(location = 1) out vec2 texCoords;
layout(location = 2) out vec3 vertexPosition;
layout(location = 3) out vec3 normalPosition;


out gl_PerVertex
{
	vec4 gl_Position;
};

const float pi = 3.14159;
const float R1 = 5.0;
const float R2 = 4.8;
const float scale = 0.1;

void main()
{	
	float u = pi*(2.0*mesh.x-1.0);
	float v = pi*(2.0*mesh.y-1.0);
	vec3 vertex;
	vec3 normal;
	vec3 fu, fv;

	vertex.x = scale*(cos(u)*(R1+R2*cos(v))+pow(abs(v)/pi,20.0));  // pow(x,y) --> x kann keine negativen Werte annehmen!!!
	vertex.y = scale*(-2.3*log(1.0-v*0.3157)+6.0*sin(v)+2.0*cos(v));
	vertex.z = scale*(sin(u)*(R1+R2*cos(v))+0.25*cos(5.0*u));

	fu.x = -scale*sin(u)*(R1 + R2*cos(v));
	fu.y = 0.0;
	fu.z = -scale*((5.0*sin(5.0*u))/4.0 - cos(u)*(R1 + R2*cos(v)));

	fv.x = scale*((20.0*pow(abs(v),19.0)*sign(v))/pow(pi,20.0) - R2*cos(u)*sin(v)); // pow(x,y) --> x kann keine negativen Werte annehmen!!!
	fv.y = -scale*(2.0*sin(v) - 6.0*cos(v) + 72611.0/(100000.0*((3157.0*v)/10000.0 - 1.0)));
	fv.z = -R2*scale*sin(u)*sin(v);

	normal = -cross(fu,fv);

	color = vec3(0.0, 1.0, 0.0);
	texCoords = vec2(0.0, 0.0);

	vertexPosition = vec3(ubo.mView * ubo.mModel * vec4(vertex, 1.0));
	gl_Position = ubo.mProj * vec4(vertexPosition, 1.0);

	normalPosition = vec3(transpose(inverse(ubo.mView * ubo.mModel)) * vec4(normal, 1.0));
}