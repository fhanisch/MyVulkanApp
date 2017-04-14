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
	char *pVertShaderFileName;
	char *pFragShaderFileName;
	uint32_t stride;
	VkFormat vertexFormat;
	uint32_t posOffset;
	uint32_t texCoordsOffset;
} PipelineCreateInfo;

typedef struct {
	PipelineCreateInfo pipelineCreateInfo;
	UniformBufferObject ubo;
} RenderObject;

void initRenderObject(RenderObject *pObj, PipelineCreateInfo createInfo);

#endif // !RENDER_OBJECT_H