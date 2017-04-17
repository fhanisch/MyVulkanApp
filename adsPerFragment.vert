#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 0) uniform UniformBufferObject {
    mat4 mModel;
    mat4 mView;
    mat4 mProj;
} ubo;

layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec2 inTexCoords;
layout(location = 3) in vec3 inNormal;

layout(location = 0) out vec3 color;
layout(location = 1) out vec2 texCoords;
layout(location = 2) out vec3 vertexPosition;
layout(location = 3) out vec3 normalPosition;


out gl_PerVertex
{
	vec4 gl_Position;
};

void main()
{	
	color = inColor;
	texCoords = inTexCoords;

	vertexPosition = vec3(ubo.mView * ubo.mModel * vec4(pos, 1.0));
	gl_Position = ubo.mProj * vec4(vertexPosition, 1.0);

	normalPosition = vec3(transpose(inverse(ubo.mView * ubo.mModel)) * vec4(inNormal, 1.0));
}