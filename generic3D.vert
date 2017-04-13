#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 0) uniform UniformBufferObject {
    mat4 mModel;
    mat4 mView;
    mat4 mProj;
} ubo;


layout(location = 0) in vec3 pos;
layout(location = 1) in vec2 inTexCoords;

layout(location = 0) out vec2 texCoords;


out gl_PerVertex
{
	vec4 gl_Position;
};

void main()
{	
	texCoords = inTexCoords;
	gl_Position = ubo.mProj * ubo.mView * ubo.mModel * vec4(pos, 1.0);
}