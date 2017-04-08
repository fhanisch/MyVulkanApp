#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 0) uniform UniformBufferObject {
    mat4 mModel;
    mat4 mView;
    mat4 mProj;
} ubo;


layout(location = 0) in vec2 pos;

out gl_PerVertex
{
	vec4 gl_Position;
};

void main()
{
	float x = 0.0;
	gl_Position = ubo.mProj * ubo.mView * ubo.mModel * vec4(pos, x, 1.0);
}