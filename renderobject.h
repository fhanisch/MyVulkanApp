#ifndef RENDER_OBJECT_H
#define RENDER_OBJECT_H

#include "matrix.h"

typedef struct {
	mat4 mModel;
	mat4 mView;
	mat4 mProj;
} UniformBufferObject;

typedef struct {
	vec2 pos;
	vec3 color;
	vec2 texCoords;
} Vertex;

typedef struct {
	vec3 pos;
	vec3 color;
	vec2 texCoords;
	vec3 normal;
} Vertex3D;

typedef struct {
	char *pVertShaderFileName;
	char *pFragShaderFileName;
	uint32_t stride;
	VkFormat vertexFormat;
	uint32_t posOffset;
	uint32_t colorOffset;
	uint32_t texCoordsOffset;
	uint32_t normalOffset;
} PipelineCreateInfo;

typedef struct {
	PipelineCreateInfo pipelineCreateInfo;
	UniformBufferObject ubo;
	VkPipeline pipeline;
	VkDeviceMemory deviceMemory;
	VkBuffer uniformBuffer;
	VkDescriptorSet uniformDescriptorSet;
} RenderObject;

void initRenderObject(RenderObject *pObj, PipelineCreateInfo createInfo);
void motion(RenderObject *pObj, mat4 rotX, mat4 rotY, mat4 rotZ, mat4 transT);

#endif // !RENDER_OBJECT_H