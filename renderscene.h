#ifndef RENDER_SCENE_H
#define RENDER_SCENE_H

PipelineCreateInfo createInfo1 = {
	.pVertShaderFileName = "vs_generic.spv",
	.pFragShaderFileName = "fs_powermeter.spv",
	.stride = sizeof(Vertex),
	.vertexFormat = VK_FORMAT_R32G32_SFLOAT,
	.posOffset = offsetof(Vertex,pos),
	.colorOffset = offsetof(Vertex, color),
	.texCoordsOffset = offsetof(Vertex, texCoords),
	.normalOffset = 0
	};
PipelineCreateInfo createInfo2 = {
	.pVertShaderFileName = "vs_generic.spv",
	.pFragShaderFileName = "fs_generic.spv",
	.stride = sizeof(Vertex),
	.vertexFormat = VK_FORMAT_R32G32_SFLOAT,
	.posOffset = offsetof(Vertex,pos),
	.colorOffset = offsetof(Vertex, color),
	.texCoordsOffset = offsetof(Vertex, texCoords),
	.normalOffset = 0
	};
PipelineCreateInfo createInfo3 = {
	.pVertShaderFileName = "vs_generic3D.spv",
	.pFragShaderFileName = "fs_generic.spv",
	.stride = sizeof(Vertex3D),
	.vertexFormat = VK_FORMAT_R32G32B32_SFLOAT,
	.posOffset = offsetof(Vertex3D,pos),
	.colorOffset = offsetof(Vertex3D, color),
	.texCoordsOffset = offsetof(Vertex3D, texCoords),
	.normalOffset = 0
	};

PipelineCreateInfo quadCreateInfo = {
	.pVertShaderFileName = "vs_adsPerFragment.spv",
	.pFragShaderFileName = "fs_adsPerFragment.spv",
	.stride = sizeof(Vertex3D),
	.vertexFormat = VK_FORMAT_R32G32B32_SFLOAT,
	.posOffset = offsetof(Vertex3D,pos),
	.colorOffset = offsetof(Vertex3D, color),
	.texCoordsOffset = offsetof(Vertex3D, texCoords),
	.normalOffset = offsetof(Vertex3D, normal)
};

PipelineCreateInfo sphereCreateInfo = {
	.pVertShaderFileName = "vs_adsPerFragment_sphere.spv",
	.pFragShaderFileName = "fs_adsPerFragment.spv",
	.stride = sizeof(vec2),
	.vertexFormat = VK_FORMAT_R32G32_SFLOAT,
	.posOffset = 0,
	.colorOffset = 0,
	.texCoordsOffset = 0,
	.normalOffset = 0
};

#endif // !RENDER_SCENE_H

