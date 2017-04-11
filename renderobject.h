#ifndef RENDER_OBJECT_H
#define RENDER_OBJECT_H

typedef struct {
	mat4 mModel;
	mat4 mView;
	mat4 mProj;
} UniformBufferObject;

typedef struct {
	vec2 pos[2];
	vec2 texCoords[2];
} Vertex;

typedef struct {
	UniformBufferObject ubo;
} RenderObject;

#endif // !RENDER_OBJECT_H