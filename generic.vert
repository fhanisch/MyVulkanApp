#version 450
#extension GL_ARB_separate_shader_objects : enable

out gl_PerVertex
{
	vec4 gl_Position;
};
/*
vec2 position[3] = vec2[]
(
	vec2(0.0, -0.5),
	vec2(0.5, 0.5),
	vec2(-0.5, 0.5)
);
*/

//vec3 x = vec3(0.0, 0.5, -0.5);

void main()
{
	//gl_Position = vec4(position[gl_VertexIndex], 0.0, 1.0);
	gl_Position = vec4(0.0, 0.0, 0.0, 1.0);
}