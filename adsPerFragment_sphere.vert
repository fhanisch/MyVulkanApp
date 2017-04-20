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

void main()
{	
	float R=1.0;	

	float u = 2.0*pi*mesh.x;
	float v = pi*mesh.y;
	vec3 vertex;
	vec3 normal;
	//vec3 fu,fv;	

	vertex.x = R*sin(v)*cos(u);
	vertex.y = R*sin(v)*sin(u);
	vertex.z = R*cos(v);

	normal.x = sin(v)*cos(u);
	normal.y = sin(v)*sin(u);
	normal.z = cos(v);	

	/*
	fu.x=-sin(v)*sin(u);
	fu.y=sin(v)*cos(u);
	fu.z=0.0;

	fv.x=cos(v)*cos(u);
	fv.y=cos(v)*sin(u);
	fv.z=-sin(v);

	normal = -cross(fu,fv);
	*/

	color = vec3(0.0, 1.0, 0.0);
	texCoords = vec2(0.0, 0.0);

	vertexPosition = vec3(ubo.mView * ubo.mModel * vec4(vertex, 1.0));
	gl_Position = ubo.mProj * vec4(vertexPosition, 1.0);

	normalPosition = vec3(transpose(inverse(ubo.mView * ubo.mModel)) * vec4(normal, 1.0));
}