#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 0) uniform UniformBufferObject {
    mat4 mModel;
    mat4 mView;
    mat4 mProj;
} ubo;


layout(location = 0) in vec2 pos;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec2 inTexCoords;

layout(location = 0) out vec3 color;
layout(location = 1) out vec2 texCoords;


out gl_PerVertex
{
	vec4 gl_Position;
};

void main()
{
	float x = 0.0;
	color = inColor;
	texCoords = inTexCoords;
	gl_Position = ubo.mProj * ubo.mView * ubo.mModel * vec4(pos, x, 1.0);
}