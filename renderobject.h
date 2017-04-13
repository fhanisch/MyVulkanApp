#ifndef RENDER_OBJECT_H
#define RENDER_OBJECT_H

typedef struct {
	mat4 mModel;
	mat4 mView;
	mat4 mProj;
} UniformBufferObject;

typedef struct {
	vec2 pos;
	vec2 texCoords;
} Vertex;

typedef struct {
	vec3 pos;
	vec2 texCoords;
} Vertex3D;

typedef struct {
	UniformBufferObject ubo;
} RenderObject;

#endif // !RENDER_OBJECT_H