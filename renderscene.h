#ifndef RENDER_SCENE_H
#define RENDER_SCENE_H

PipelineCreateInfo createInfo1 = { .pVertShaderFileName = "vs_generic.spv", "fs_powermeter.spv", sizeof(Vertex), VK_FORMAT_R32G32_SFLOAT, offsetof(Vertex,pos), offsetof(Vertex, texCoords) };
PipelineCreateInfo createInfo2 = { .pVertShaderFileName = "vs_generic.spv", "fs_generic.spv", sizeof(Vertex), VK_FORMAT_R32G32_SFLOAT, offsetof(Vertex,pos), offsetof(Vertex, texCoords) };
PipelineCreateInfo createInfo3 = { .pVertShaderFileName = "vs_generic3D.spv", "fs_generic.spv", sizeof(Vertex3D), VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex3D,pos), offsetof(Vertex3D, texCoords) };

#endif // !RENDER_SCENE_H

