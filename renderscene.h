#ifndef RENDER_SCENE_H
#define RENDER_SCENE_H

PipelineCreateInfo createInfo1 = {
	.pVertShaderFileName = "vs_generic.spv",
	.pFragShaderFileName = "fs_powermeter.spv",
	sizeof(Vertex), VK_FORMAT_R32G32_SFLOAT, offsetof(Vertex,pos), offsetof(Vertex, color), offsetof(Vertex, texCoords)
	};
PipelineCreateInfo createInfo2 = {
	.pVertShaderFileName = "vs_generic.spv",
	.pFragShaderFileName = "fs_generic.spv",
	sizeof(Vertex), VK_FORMAT_R32G32_SFLOAT, offsetof(Vertex,pos), offsetof(Vertex, color), offsetof(Vertex, texCoords)
	};
PipelineCreateInfo createInfo3 = {
	.pVertShaderFileName = "vs_generic3D.spv",
	.pFragShaderFileName = "fs_generic.spv",
	sizeof(Vertex3D), VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex3D,pos), offsetof(Vertex3D, color), offsetof(Vertex3D, texCoords)
	};

#endif // !RENDER_SCENE_H

