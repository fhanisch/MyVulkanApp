/*
	MyVulkanApp

	Date: 23.03.2017
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <libusb-1.0\libusb.h>
#include <Windows.h> // notwendig f�r Threads --> Alternative evtl. pthreads
//#define VK_USE_PLATFORM_WIN32_KHR --> GLFW erledigt das
#define GLFW_INCLUDE_VULKAN
#include <GLFW\glfw3.h>
//#include <vulkan\vulkan.h> --> wird durch GLFW includiert
#include "MyVulkanEngine.h"
#include "matrix.h"
#include "renderobject.h"
#include "renderscene.h"

#define GAMECONTROLLER_VENDOR_ID 0x046d
#define GAMECONTROLLER_PRODUCT_ID 0xc218
#define HID_INTERFACE 0
#define ENDPOINT_ADDRESS 0x81

typedef struct {
	float lStickX;
	float lStickY;
	float rStickX;
	float rStickY;
} CtrlValues;

libusb_device_handle *hid_gamecontroller;

GLFWwindow *pWindow;

static const uint32_t wndWidth = 1000;
static const uint32_t wndHeight = 1000;

// internal linkage --> Definition static: https://de.wikipedia.org/wiki/Static_(Schl%C3%BCsselwort) --> ohne 'static' w�rde 'extern' entsprechen
static const char appName[] = "MyVulkanApp";

static Vertex vertices[] = { 
								{ {-1.0f,  1.0f}, { 1.0f, 0.0f, 1.0f }, { -1.0f,  1.0f } },
								{ {-1.0f, -1.0f}, { 1.0f, 0.0f, 1.0f }, { -1.0f, -1.0f } },
								{ { 1.0f, -1.0f}, { 1.0f, 0.0f, 1.0f }, {  1.0f, -1.0f } },
								{ { 1.0f,  1.0f}, { 1.0f, 0.0f, 1.0f }, {  1.0f,  1.0f } },
								{ { 0.0f, -1.0f}, { 1.0f, 0.0f, 1.0f }, {  0.0f, -1.0f } }
							};
							
static Vertex3D verticesPlane[] = {
	{ { -1.0f,  1.0f,  1.0f }, { 0.0f, 1.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f,  1.0f } },
	{ {  1.0f,  1.0f,  1.0f }, { 0.0f, 1.0f, 0.0f }, { 1.0f, 1.0f }, { 0.0f, 0.0f,  1.0f } },
	{ {  1.0f, -1.0f,  1.0f }, { 0.0f, 1.0f, 0.0f }, { 1.0f, 0.0f }, { 0.0f, 0.0f,  1.0f } },	
	{ { -1.0f, -1.0f,  1.0f }, { 0.0f, 1.0f, 0.0f }, { 0.0f, 1.0f }, { 0.0f, 0.0f,  1.0f } },

	{ {  1.0f, -1.0f,  1.0f }, { 1.0f, 1.0f, 0.0f }, { 0.0f, 0.0f }, { 1.0f, 0.0f,  0.0f } },
	{ {  1.0f,  1.0f,  1.0f }, { 1.0f, 1.0f, 0.0f }, { 1.0f, 1.0f }, { 1.0f, 0.0f,  0.0f } },
	{ {  1.0f,  1.0f, -1.0f }, { 1.0f, 1.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 0.0f,  0.0f } },
	{ {  1.0f, -1.0f, -1.0f }, { 1.0f, 1.0f, 0.0f }, { 0.0f, 1.0f }, { 1.0f, 0.0f,  0.0f } },

	{ { -1.0f,  1.0f, -1.0f }, { 1.0f, 0.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f, -1.0f } },
	{ {  1.0f,  1.0f, -1.0f }, { 1.0f, 0.0f, 0.0f }, { 1.0f, 1.0f }, { 0.0f, 0.0f, -1.0f } },
	{ {  1.0f, -1.0f, -1.0f }, { 1.0f, 0.0f, 0.0f }, { 1.0f, 0.0f }, { 0.0f, 0.0f, -1.0f } },
	{ { -1.0f, -1.0f, -1.0f }, { 1.0f, 0.0f, 0.0f }, { 0.0f, 1.0f }, { 0.0f, 0.0f, -1.0f } },

	{ { -1.0f, -1.0f,  1.0f }, { 1.0f, 0.0f, 1.0f }, { 0.0f, 0.0f }, { 1.0f, 0.0f,  0.0f } },
	{ { -1.0f,  1.0f,  1.0f }, { 1.0f, 0.0f, 1.0f }, { 1.0f, 1.0f }, { 1.0f, 0.0f,  0.0f } },
	{ { -1.0f,  1.0f, -1.0f }, { 1.0f, 0.0f, 1.0f }, { 1.0f, 0.0f }, { 1.0f, 0.0f,  0.0f } },
	{ { -1.0f, -1.0f, -1.0f }, { 1.0f, 0.0f, 1.0f }, { 0.0f, 1.0f }, { 1.0f, 0.0f,  0.0f } },
};

static uint16_t indices[] = { 0, 1, 2, 0, 2, 3 };
static uint16_t indices2[] = { 0, 4, 3 };
static uint16_t indices_plane[] = { 0, 1, 2, 2, 3, 0,
									4, 5, 6, 6, 7, 4,
									8, 9, 10, 10, 11, 8,
									12, 13, 14, 14, 15, 12
								};

static RenderObject obj1;
static RenderObject obj2;
static RenderObject obj3;

void startGLFW()
{
	if (!glfwInit()) exit(-1);
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	pWindow = glfwCreateWindow(wndWidth, wndHeight, appName, NULL, NULL);
}

void createSurface()
{
	VkResult result;

	result = glfwCreateWindowSurface(instance, pWindow, NULL, &surface);
	assert(result, "glfwCreateWindowSurface failed!");
	/* --> GLFW erledigt das
	surfaceCreateInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
	surfaceCreateInfo.pNext = NULL;
	surfaceCreateInfo.flags = 0;
	surfaceCreateInfo.hinstance = NULL;
	surfaceCreateInfo.hwnd = NULL;

	result = vkCreateWin32SurfaceKHR(instance, &surfaceCreateInfo, NULL, &surface);
	assert(result, "vkCreateWin32SurfaceKHR failed!");
	*/
}

void createShaderModule(char *filename, VkShaderModule *shaderModule)
{
	VkResult result;
	char *shaderCode;
	int filesize;
	VkShaderModuleCreateInfo shaderCreateInfo;

	filesize = loadShader(&shaderCode, filename);
	shaderCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	shaderCreateInfo.pNext = NULL;
	shaderCreateInfo.flags = 0;
	shaderCreateInfo.codeSize = filesize;
	shaderCreateInfo.pCode = (uint32_t*)shaderCode;

	result = vkCreateShaderModule(device, &shaderCreateInfo, NULL, shaderModule);
	assert(result, "vkCreateShaderModule failed!\n");
}

void createDescriptorSetLayout()
{
	VkResult result;
	VkDescriptorSetLayoutBinding descriptorSetLayoutBinding;
	VkDescriptorSetLayoutCreateInfo layoutInfo;

	descriptorSetLayoutBinding = getDescriptorSetLayoutBinding(VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT);

	layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	layoutInfo.pNext = NULL;
	layoutInfo.flags = 0;
	layoutInfo.bindingCount = 1;
	layoutInfo.pBindings = &descriptorSetLayoutBinding;

	result = vkCreateDescriptorSetLayout(device, &layoutInfo, NULL, &descriptorSetLayout);
	assert(result, "vkCreateDescriptorSetLayout failed!\n");
}

void createPipelineLayout()
{
	VkResult result;
	VkDescriptorSetLayout setLayouts[1];
	VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo;

	//Wichitg f�r Shader-Uniform varibalen!!!
	setLayouts[0] = descriptorSetLayout;
	pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutCreateInfo.pNext = NULL;
	pipelineLayoutCreateInfo.flags = 0;
	pipelineLayoutCreateInfo.setLayoutCount = 1;
	pipelineLayoutCreateInfo.pSetLayouts = setLayouts;
	pipelineLayoutCreateInfo.pushConstantRangeCount = 0;
	pipelineLayoutCreateInfo.pPushConstantRanges = NULL;

	result = vkCreatePipelineLayout(device, &pipelineLayoutCreateInfo, NULL, &pipelineLayout);
	assert(result, "vkCreatePipelineLayout failed!\n");
}

void createGraphicsPipeline(PipelineCreateInfo *pPipelineCreateInfo, VkPipeline *pPipeline)
{
	VkResult result;
	VkPipelineShaderStageCreateInfo shaderStageCreateInfoVert, shaderStageCreateInfoFrag;
	VkPipelineShaderStageCreateInfo shaderStages[2];
	VkVertexInputBindingDescription vertexInputBindingDescription;
	VkVertexInputAttributeDescription vertexInputAttributeDescription[4];
	VkPipelineVertexInputStateCreateInfo vertexInputCreateInfo;
	VkPipelineInputAssemblyStateCreateInfo inputAssemblyCreateInfo;
	VkViewport viewport;
	VkRect2D scissor;
	VkPipelineViewportStateCreateInfo viewportCreateInfo;
	VkPipelineRasterizationStateCreateInfo rasterizationCreateInfo;
	VkPipelineMultisampleStateCreateInfo multisampleCreateInfo;
	VkPipelineColorBlendAttachmentState colorBlendAttachment;
	VkPipelineColorBlendStateCreateInfo colorBlendCreateInfo;	
	VkGraphicsPipelineCreateInfo pipelineCreateInfo;
	VkPipelineDepthStencilStateCreateInfo depthStencilStateInfo;

	createShaderModule(pPipelineCreateInfo->pVertShaderFileName, &vertexShaderModule);
	createShaderModule(pPipelineCreateInfo->pFragShaderFileName, &fragmentShaderModule);

	shaderStageCreateInfoVert.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	shaderStageCreateInfoVert.pNext = NULL;
	shaderStageCreateInfoVert.flags = 0;
	shaderStageCreateInfoVert.stage = VK_SHADER_STAGE_VERTEX_BIT;
	shaderStageCreateInfoVert.module = vertexShaderModule;
	shaderStageCreateInfoVert.pName = "main";
	shaderStageCreateInfoVert.pSpecializationInfo = NULL;

	shaderStageCreateInfoFrag.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	shaderStageCreateInfoFrag.pNext = NULL;
	shaderStageCreateInfoFrag.flags = 0;
	shaderStageCreateInfoFrag.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	shaderStageCreateInfoFrag.module = fragmentShaderModule;
	shaderStageCreateInfoFrag.pName = "main";
	shaderStageCreateInfoFrag.pSpecializationInfo = NULL;

	shaderStages[0] = shaderStageCreateInfoVert;
	shaderStages[1] = shaderStageCreateInfoFrag;

	vertexInputBindingDescription = getBindingDescription(pPipelineCreateInfo->stride);
	vertexInputAttributeDescription[0] = getAttributeDescription(0, pPipelineCreateInfo->posOffset , pPipelineCreateInfo->vertexFormat);
	vertexInputAttributeDescription[1] = getAttributeDescription(1, pPipelineCreateInfo->colorOffset, VK_FORMAT_R32G32B32_SFLOAT);
	vertexInputAttributeDescription[2] = getAttributeDescription(2, pPipelineCreateInfo->texCoordsOffset, VK_FORMAT_R32G32_SFLOAT);
	vertexInputAttributeDescription[3] = getAttributeDescription(3, pPipelineCreateInfo->normalOffset, VK_FORMAT_R32G32B32_SFLOAT);

	vertexInputCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	vertexInputCreateInfo.pNext = NULL;
	vertexInputCreateInfo.flags = 0;
	vertexInputCreateInfo.vertexBindingDescriptionCount = 1;
	vertexInputCreateInfo.pVertexBindingDescriptions = &vertexInputBindingDescription;
	vertexInputCreateInfo.vertexAttributeDescriptionCount = 4;
	vertexInputCreateInfo.pVertexAttributeDescriptions = vertexInputAttributeDescription;

	inputAssemblyCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	inputAssemblyCreateInfo.pNext = NULL;
	inputAssemblyCreateInfo.flags = 0;
	inputAssemblyCreateInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	inputAssemblyCreateInfo.primitiveRestartEnable = VK_FALSE;

	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = (float)wndWidth;
	viewport.height = (float)wndHeight;
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;

	scissor.offset.x = 0;
	scissor.offset.y = 0;
	scissor.extent.width = wndWidth;
	scissor.extent.height = wndHeight;

	viewportCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewportCreateInfo.pNext = NULL;
	viewportCreateInfo.flags = 0;
	viewportCreateInfo.viewportCount = 1;
	viewportCreateInfo.pViewports = &viewport;
	viewportCreateInfo.scissorCount = 1;
	viewportCreateInfo.pScissors = &scissor;

	rasterizationCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rasterizationCreateInfo.pNext = NULL;
	rasterizationCreateInfo.flags = 0;
	rasterizationCreateInfo.depthClampEnable = VK_FALSE;
	rasterizationCreateInfo.rasterizerDiscardEnable = VK_FALSE;
	rasterizationCreateInfo.polygonMode = VK_POLYGON_MODE_FILL;
	rasterizationCreateInfo.cullMode = 0; // Front and Back werden gerendert
	rasterizationCreateInfo.frontFace = VK_FRONT_FACE_CLOCKWISE;
	rasterizationCreateInfo.depthBiasEnable = VK_FALSE;
	rasterizationCreateInfo.depthBiasConstantFactor = 0.0f;
	rasterizationCreateInfo.depthBiasClamp = 0.0f;
	rasterizationCreateInfo.depthBiasSlopeFactor = 0.0f;
	rasterizationCreateInfo.lineWidth = 1.0f;

	multisampleCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multisampleCreateInfo.pNext = NULL;
	multisampleCreateInfo.flags = 0;
	multisampleCreateInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
	multisampleCreateInfo.sampleShadingEnable = VK_FALSE;
	multisampleCreateInfo.minSampleShading = 1.0f;
	multisampleCreateInfo.pSampleMask = NULL;
	multisampleCreateInfo.alphaToCoverageEnable = VK_FALSE;
	multisampleCreateInfo.alphaToOneEnable = VK_FALSE;

	colorBlendAttachment.blendEnable = VK_TRUE;
	colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
	colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
	colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
	colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
	colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
	colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;
	colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;

	colorBlendCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	colorBlendCreateInfo.pNext = NULL;
	colorBlendCreateInfo.flags = 0;
	colorBlendCreateInfo.logicOpEnable = VK_FALSE;
	colorBlendCreateInfo.logicOp = VK_LOGIC_OP_NO_OP;
	colorBlendCreateInfo.attachmentCount = 1;
	colorBlendCreateInfo.pAttachments = &colorBlendAttachment;
	colorBlendCreateInfo.blendConstants[0] = 0.0f;
	colorBlendCreateInfo.blendConstants[1] = 0.0f;
	colorBlendCreateInfo.blendConstants[2] = 0.0f;
	colorBlendCreateInfo.blendConstants[3] = 0.0f;

	memset(&depthStencilStateInfo, 0, sizeof(VkPipelineDepthStencilStateCreateInfo));
	depthStencilStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
	depthStencilStateInfo.pNext = NULL;
	depthStencilStateInfo.flags = 0;
	depthStencilStateInfo.depthTestEnable = VK_TRUE;
	depthStencilStateInfo.depthWriteEnable = VK_TRUE;
	depthStencilStateInfo.depthCompareOp = VK_COMPARE_OP_LESS;
	depthStencilStateInfo.depthBoundsTestEnable = VK_FALSE;
	depthStencilStateInfo.stencilTestEnable = VK_FALSE;
	depthStencilStateInfo.minDepthBounds = 0.0f;
	depthStencilStateInfo.maxDepthBounds = 1.0f;

	pipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipelineCreateInfo.pNext = NULL;
	pipelineCreateInfo.flags = 0;
	pipelineCreateInfo.stageCount = 2;
	pipelineCreateInfo.pStages = shaderStages;
	pipelineCreateInfo.pVertexInputState = &vertexInputCreateInfo;
	pipelineCreateInfo.pInputAssemblyState = &inputAssemblyCreateInfo;
	pipelineCreateInfo.pTessellationState = NULL;
	pipelineCreateInfo.pViewportState = &viewportCreateInfo;
	pipelineCreateInfo.pRasterizationState = &rasterizationCreateInfo;
	pipelineCreateInfo.pMultisampleState = &multisampleCreateInfo;
	pipelineCreateInfo.pDepthStencilState = &depthStencilStateInfo;
	pipelineCreateInfo.pColorBlendState = &colorBlendCreateInfo;
	pipelineCreateInfo.pDynamicState = NULL;
	pipelineCreateInfo.layout = pipelineLayout;
	pipelineCreateInfo.renderPass = renderPath;
	pipelineCreateInfo.subpass = 0;
	pipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE;
	pipelineCreateInfo.basePipelineIndex = -1;

	result = vkCreateGraphicsPipelines(device, NULL, 1, &pipelineCreateInfo, NULL, pPipeline);
	assert(result, "vkCreateGraphicsPipelines failed!\n");
}

void createFramebuffer()
{
	VkResult result;
	VkFramebufferCreateInfo framebufferCreateInfo;
	VkImageView attachments[2];

	pFramebuffer = malloc(sizeof(VkFramebuffer) * imagesInSwapChainCount);
	for (uint32_t i = 0; i < imagesInSwapChainCount; i++)
	{
		attachments[0] = pImageViews[i];
		attachments[1] = depthImageView;
		framebufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferCreateInfo.pNext = NULL;
		framebufferCreateInfo.flags = 0;
		framebufferCreateInfo.renderPass = renderPath;
		framebufferCreateInfo.attachmentCount = 2;
		framebufferCreateInfo.pAttachments = attachments;
		framebufferCreateInfo.width = wndWidth;
		framebufferCreateInfo.height = wndHeight;
		framebufferCreateInfo.layers = 1;

		result = vkCreateFramebuffer(device, &framebufferCreateInfo, NULL, &(pFramebuffer[i]));
		assert(result, "vkCreateFramebuffer failed!\n");
	}
}

void createBuffer(VkBuffer *pBuffer, VkDeviceSize bufferSize, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkDeviceMemory *pMemory)
{
	VkResult result;
	VkBufferCreateInfo bufferCreateInfo;
	VkMemoryRequirements memoryRequirements;
	VkMemoryAllocateInfo memoryAllocateInfo;

	bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferCreateInfo.pNext = NULL;
	bufferCreateInfo.flags = 0;
	bufferCreateInfo.size = bufferSize;
	bufferCreateInfo.usage = usage;
	bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	bufferCreateInfo.queueFamilyIndexCount = 0;
	bufferCreateInfo.pQueueFamilyIndices = NULL;

	result = vkCreateBuffer(device, &bufferCreateInfo, NULL, pBuffer);
	assert(result, "vkCreateBuffer failed!\n");

	vkGetBufferMemoryRequirements(device, *pBuffer, &memoryRequirements);

	memoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	memoryAllocateInfo.pNext = NULL;
	memoryAllocateInfo.allocationSize = memoryRequirements.size;
	memoryAllocateInfo.memoryTypeIndex = findMemoryTypeIndex(memoryRequirements.memoryTypeBits, properties);

	result = vkAllocateMemory(device, &memoryAllocateInfo, NULL, pMemory);
	assert(result, "vkAllocateMemory failed!\n");

	vkBindBufferMemory(device, *pBuffer, *pMemory, 0);
}

void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size)
{
	VkCommandBufferAllocateInfo commandBufferAllocateInfo;
	VkCommandBuffer cmdBuffer;
	VkCommandBufferBeginInfo beginInfo;
	VkBufferCopy copyRegion;
	VkSubmitInfo submitInfo;

	commandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	commandBufferAllocateInfo.pNext = NULL;
	commandBufferAllocateInfo.commandPool = commandPool;
	commandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	commandBufferAllocateInfo.commandBufferCount = 1;

	vkAllocateCommandBuffers(device, &commandBufferAllocateInfo, &cmdBuffer);

	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.pNext = NULL;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
	beginInfo.pInheritanceInfo = NULL;

	vkBeginCommandBuffer(cmdBuffer, &beginInfo);

	copyRegion.srcOffset = 0;
	copyRegion.dstOffset = 0;
	copyRegion.size = size;
	vkCmdCopyBuffer(cmdBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

	vkEndCommandBuffer(cmdBuffer);

	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.pNext = NULL;
	submitInfo.waitSemaphoreCount = 0;
	submitInfo.pWaitSemaphores = NULL;
	submitInfo.pWaitDstStageMask = NULL;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &cmdBuffer;
	submitInfo.signalSemaphoreCount = 0;
	submitInfo.pSignalSemaphores = NULL;

	vkQueueSubmit(queue, 1, &submitInfo, VK_NULL_HANDLE);
	vkQueueWaitIdle(queue);

	vkFreeCommandBuffers(device, commandPool, 1, &cmdBuffer);

}

void createVertexBuffer()
{
	VkDeviceSize bufferSize = sizeof(vertices) + sizeof(verticesPlane);
	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;
	void *rawData;

	createBuffer(&stagingBuffer, bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &stagingBufferMemory);

	vkMapMemory(device, stagingBufferMemory, 0, bufferSize, 0, &rawData);
	memcpy(rawData, &vertices, bufferSize);
	memcpy((char*)rawData + sizeof(vertices), &verticesPlane, sizeof(verticesPlane));
	vkUnmapMemory(device, stagingBufferMemory);

	createBuffer(&vertexBuffer, bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &vertexBufferDeviceMemory);

	copyBuffer(stagingBuffer, vertexBuffer, bufferSize);
	
	vkFreeMemory(device, stagingBufferMemory, NULL);
	vkDestroyBuffer(device, stagingBuffer, NULL);
}

void createIndexBuffer()
{
	VkDeviceSize bufferSize = sizeof(indices) + sizeof(indices2) + sizeof(indices_plane);
	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;
	void *rawData;

	createBuffer(&stagingBuffer, bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &stagingBufferMemory);

	vkMapMemory(device, stagingBufferMemory, 0, bufferSize, 0, &rawData);
	memcpy(rawData, indices, sizeof(indices));
	memcpy((char*)rawData + sizeof(indices), indices2, sizeof(indices2));
	memcpy((char*)rawData + sizeof(indices)+ sizeof(indices2), indices_plane, sizeof(indices_plane));
	vkUnmapMemory(device, stagingBufferMemory);

	createBuffer(&indexBuffer, bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &indexBufferDeviceMemory);

	copyBuffer(stagingBuffer, indexBuffer, bufferSize);

	vkFreeMemory(device, stagingBufferMemory, NULL);
	vkDestroyBuffer(device, stagingBuffer, NULL);
}

void createUniformStagingBuffer()
{
	VkDeviceSize bufferSize = sizeof(UniformBufferObject);
	createBuffer(&uniformStagingBuffer, bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &uniformStagingBufferMemory);
}

void createUniformBuffer(UniformBufferObject *pUBO, VkBuffer *pUniformBuffer, VkDeviceMemory *pUniformBufferDeviceMemory)
{
	VkDeviceSize bufferSize = sizeof(UniformBufferObject);
	void *rawData;	

	vkMapMemory(device, uniformStagingBufferMemory, 0, bufferSize, 0, &rawData);
	memcpy(rawData, pUBO, bufferSize);
	vkUnmapMemory(device, uniformStagingBufferMemory);	

	createBuffer(pUniformBuffer, bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, pUniformBufferDeviceMemory);

	copyBuffer(uniformStagingBuffer, *pUniformBuffer, bufferSize);
}

void updateUniformBuffer()
{
	VkDeviceSize bufferSize = sizeof(UniformBufferObject);
	void *rawData;
	
	vkMapMemory(device, uniformStagingBufferMemory, 0, bufferSize, 0, &rawData);
	memcpy(rawData, &obj2.ubo.mModel, bufferSize);
	vkUnmapMemory(device, uniformStagingBufferMemory);

	copyBuffer(uniformStagingBuffer, uniformBuffer2, bufferSize);

	vkMapMemory(device, uniformStagingBufferMemory, 0, bufferSize, 0, &rawData);
	memcpy(rawData, &obj3.ubo.mModel, bufferSize);
	vkUnmapMemory(device, uniformStagingBufferMemory);

	copyBuffer(uniformStagingBuffer, uniformBuffer3, bufferSize);
}

void createDescriptorPool()
{
	VkResult result;
	VkDescriptorPoolSize poolSize;
	VkDescriptorPoolCreateInfo poolInfo;

	poolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	poolSize.descriptorCount = 3; // funktioniert auch mit 1 --> warum?

	poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	poolInfo.pNext = NULL;
	poolInfo.flags = 0;
	poolInfo.maxSets = 3;
	poolInfo.poolSizeCount = 1;
	poolInfo.pPoolSizes = &poolSize;

	result = vkCreateDescriptorPool(device, &poolInfo, NULL, &descriptorPool);
	assert(result, "vkCreateDescriptorPool failed!\n");
}

void createDescriptorSet(VkBuffer uniformBuffer, VkDescriptorSet *pDescriptorSet)
{
	VkResult result;
	VkDescriptorSetLayout layouts[] = { descriptorSetLayout };
	VkDescriptorSetAllocateInfo allocInfo;
	VkDescriptorBufferInfo bufferInfo;
	VkWriteDescriptorSet descriptorWrite;

	allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.pNext = NULL;
	allocInfo.descriptorPool = descriptorPool;
	allocInfo.descriptorSetCount = 1;
	allocInfo.pSetLayouts = layouts;

	result = vkAllocateDescriptorSets(device, &allocInfo, pDescriptorSet);
	assert(result, "vkAllocateDescriptorSets failed!\n");

	bufferInfo.buffer = uniformBuffer;
	bufferInfo.offset = 0;
	bufferInfo.range = sizeof(UniformBufferObject);

	descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	descriptorWrite.pNext = NULL;
	descriptorWrite.dstSet = *pDescriptorSet;
	descriptorWrite.dstBinding = 0;
	descriptorWrite.dstArrayElement = 0;
	descriptorWrite.descriptorCount = 1;
	descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	descriptorWrite.pImageInfo = NULL;
	descriptorWrite.pBufferInfo = &bufferInfo;
	descriptorWrite.pTexelBufferView = NULL;

	vkUpdateDescriptorSets(device, 1, &descriptorWrite, 0, NULL);
}

void createCommandPool()
{
	VkResult result;
	VkCommandPoolCreateInfo commandPoolCreateInfo;

	commandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	commandPoolCreateInfo.pNext = NULL;
	commandPoolCreateInfo.flags = 0;
	commandPoolCreateInfo.queueFamilyIndex = 0; // ToDo: civ - get correct queue with VK_QUEUE_GRAPHICS_BIT

	result = vkCreateCommandPool(device, &commandPoolCreateInfo, NULL, &commandPool);
	assert(result, "vkCreateCommandPool!\n");
}

void createCommandBuffer()
{
	VkResult result;
	VkCommandBufferAllocateInfo commandBufferAllocateInfo;
	VkCommandBufferBeginInfo commandBufferBeginInfo;
	VkRenderPassBeginInfo renderPassBeginInfo;
	VkClearValue clearValues[2];
	VkClearValue clearColor = { 0.0f, 0.0f, 1.0f, 1.0f };
	VkClearValue clearDepth = { 1.0f, 0 };
	VkDeviceSize offsets[] = { 0 };
	VkDeviceSize offsets2[] = { sizeof(vertices) };

	clearValues[0] = clearColor;
	clearValues[1] = clearDepth;
	commandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	commandBufferAllocateInfo.pNext = NULL;
	commandBufferAllocateInfo.commandPool = commandPool;
	commandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	commandBufferAllocateInfo.commandBufferCount = imagesInSwapChainCount;

	pCommandBuffers = malloc(sizeof(VkCommandBuffer)* imagesInSwapChainCount);
	result = vkAllocateCommandBuffers(device, &commandBufferAllocateInfo, pCommandBuffers);
	assert(result, "vkAllocateCommandBuffers!\n");

	commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	commandBufferBeginInfo.pNext = NULL;
	commandBufferBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
	commandBufferBeginInfo.pInheritanceInfo = NULL;

	for (uint32_t i = 0; i < imagesInSwapChainCount; i++)
	{
		result = vkBeginCommandBuffer(pCommandBuffers[i], &commandBufferBeginInfo);
		assert(result, "vkBeginCommandBuffer failed!\n");

		renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassBeginInfo.pNext = NULL;
		renderPassBeginInfo.renderPass = renderPath;
		renderPassBeginInfo.framebuffer = pFramebuffer[i];
		renderPassBeginInfo.renderArea.offset.x = 0;
		renderPassBeginInfo.renderArea.offset.y = 0;
		renderPassBeginInfo.renderArea.extent.width = wndWidth;
		renderPassBeginInfo.renderArea.extent.height = wndHeight;
		renderPassBeginInfo.clearValueCount = 2;
		renderPassBeginInfo.pClearValues = clearValues;

		vkCmdBeginRenderPass(pCommandBuffers[i], &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
		
		vkCmdBindVertexBuffers(pCommandBuffers[i], 0, 1, &vertexBuffer, offsets);
		/*
		vkCmdBindPipeline(pCommandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline2);
		vkCmdBindIndexBuffer(pCommandBuffers[i], indexBuffer, sizeof(indices), VK_INDEX_TYPE_UINT16);
		vkCmdBindDescriptorSets(pCommandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &descriptorSet2, 0, NULL);
		//vkCmdDrawIndexed(pCommandBuffers[i], sizeof(indices) / 2 - 3, 1, 0, 0, 0);
		vkCmdDrawIndexed(pCommandBuffers[i], sizeof(indices2) / 2, 1, 0, 0, 0);

		vkCmdBindPipeline(pCommandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline1);		
		vkCmdBindIndexBuffer(pCommandBuffers[i], indexBuffer, 0, VK_INDEX_TYPE_UINT16);
		vkCmdBindDescriptorSets(pCommandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &descriptorSet, 0, NULL);
		//vkCmdDraw(pCommandBuffers[i], 3, 1, 0, 0);
		vkCmdDrawIndexed(pCommandBuffers[i], sizeof(indices) / 2 , 1, 0, 0, 0);
		*/	
		vkCmdBindPipeline(pCommandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline3);
		vkCmdBindVertexBuffers(pCommandBuffers[i], 0, 1, &vertexBuffer, offsets2);
		vkCmdBindIndexBuffer(pCommandBuffers[i], indexBuffer, sizeof(indices) + sizeof(indices2), VK_INDEX_TYPE_UINT16);
		vkCmdBindDescriptorSets(pCommandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &descriptorSet3, 0, NULL);		
		vkCmdDrawIndexed(pCommandBuffers[i], sizeof(indices_plane) / 2, 1, 0, 0, 0);
		
		vkCmdEndRenderPass(pCommandBuffers[i]);

		result = vkEndCommandBuffer(pCommandBuffers[i]);
		assert(result, "vkEndCommandBuffer failed!\n");
	}
}

void createSemaphore()
{
	VkResult result;
	VkSemaphoreCreateInfo semaphoreCreateInfo;

	semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
	semaphoreCreateInfo.pNext = NULL;
	semaphoreCreateInfo.flags = 0;

	result = vkCreateSemaphore(device, &semaphoreCreateInfo, NULL, &semaphoreImageAvailable);
	assert(result, "vkCreateSemaphore failed!\n");
	result = vkCreateSemaphore(device, &semaphoreCreateInfo, NULL, &semaphoreRenderingDone);
	assert(result, "vkCreateSemaphore failed!\n");
}

void setupVulkan()
{	
	const char **glfwExtensions;
	uint32_t glfwExtensionsCount;

	identity4(obj1.ubo.mModel);
	identity4(obj1.ubo.mView);
	identity4(obj1.ubo.mProj);
	//obj1.ubo.mModel[0][0] = 0.5f;
	identity4(obj2.ubo.mModel);
	identity4(obj2.ubo.mView);
	identity4(obj2.ubo.mProj);
	obj2.ubo.mModel[0][0] = 0.25f;

	getTrans4(obj3.ubo.mModel, 0.0f, 0.0f, -7.0f);
	identity4(obj3.ubo.mView);
	identity4(obj3.ubo.mProj);
	getFrustum(obj3.ubo.mProj, 0.25f, 0.25f, 0.5f, 10.0f);

	glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionsCount);
	createInstance(appName, glfwExtensions, glfwExtensionsCount);
	createSurface();
	getPhysicalDevices();		
	createLogicalDevice();		
	createSwapchain(wndWidth, wndHeight);
	createImageViews();
	createDescriptorSetLayout();
	createPipelineLayout();
	createCommandPool();
	createDepthResources(wndWidth, wndHeight);
	createRenderPass();
	createGraphicsPipeline(&createInfo1, &pipeline1);
	createGraphicsPipeline(&createInfo2, &pipeline2);
	//createGraphicsPipeline(&createInfo3, &pipeline3);
	createGraphicsPipeline(&quadCreateInfo, &pipeline3);
	createFramebuffer();	
	createVertexBuffer();	
	createIndexBuffer();
	createUniformStagingBuffer();
	createUniformBuffer(&obj1.ubo, &uniformBuffer, &uniformBufferDeviceMemory);
	createUniformBuffer(&obj2.ubo, &uniformBuffer2, &uniformBufferDeviceMemory2);
	createUniformBuffer(&obj3.ubo, &uniformBuffer3, &uniformBufferDeviceMemory3);
	createDescriptorPool();
	createDescriptorSet(uniformBuffer, &descriptorSet);
	createDescriptorSet(uniformBuffer2, &descriptorSet2);
	createDescriptorSet(uniformBuffer3, &descriptorSet3);
	createCommandBuffer();
	createSemaphore();
}

void drawFrame()
{
	VkResult result;
	uint32_t imageIndex;
	VkSubmitInfo submitInfo;
	VkPipelineStageFlags waitStageMask[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
	VkPresentInfoKHR presentInfo;

	result = vkAcquireNextImageKHR(device, swapchain, UINT64_MAX, semaphoreImageAvailable, VK_NULL_HANDLE, &imageIndex);
	assert(result, "vkAcquireNextImageKHR failed!\n");

	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.pNext = NULL;
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = &semaphoreImageAvailable;
	submitInfo.pWaitDstStageMask = waitStageMask;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &(pCommandBuffers[imageIndex]);
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = &semaphoreRenderingDone;

	result = vkQueueSubmit(queue, 1, &submitInfo, VK_NULL_HANDLE);
	assert(result, "vkQueueSubmit failed!\n");

	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentInfo.pNext = NULL;
	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = &semaphoreRenderingDone;
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = &swapchain;
	presentInfo.pImageIndices = &imageIndex;
	presentInfo.pResults = NULL;

	result = vkQueuePresentKHR(queue, &presentInfo);
	assert(result, "vkQueuePresentKHR failed!\n");
}

int getCtrlValueThread(CtrlValues *pCtrlValues)
{
	int result;
	boolean quit = FALSE;
	int transferred;
	unsigned char gamectrlbuf[8];

	printf("Thread started.\n");

	memset(gamectrlbuf, 127, 4);

	while (!quit)
	{
		result = libusb_bulk_transfer(hid_gamecontroller, ENDPOINT_ADDRESS, gamectrlbuf, 8, &transferred, 1); // --> Performance checken!!! ggf. im Thread aufrufen
		if (result == 0 || result == -7)
		{
			pCtrlValues->lStickX = -((float)gamectrlbuf[0] / 255.0f * 2.0f - 1.0f) / 100.0f;
			pCtrlValues->lStickY = -((float)gamectrlbuf[1] / 255.0f * 2.0f - 1.0f) / 100.0f;
			pCtrlValues->rStickX = -((float)gamectrlbuf[2] / 255.0f * 2.0f - 1.0f) / 100.0f;
			pCtrlValues->rStickY = -((float)gamectrlbuf[3] / 255.0f * 2.0f - 1.0f) / 100.0f;

			//printf("%u\n", gamectrlbuf[2]);
			//printf("%0.2f\n", -((float)gamectrlbuf[2] / 255.0f * 2.0f - 1.0f));
		}
		else
		{
			printf("Transfer Error: %d\n", result);
			quit = TRUE;
		}
	}
	printf("Thread terminated!\n");

	return 0;
}

void mainLoop()
{
	DWORD threadID;
	DWORD threadExitCode;
	HANDLE hThread;
	CtrlValues ctrlValues;
	clock_t start_t, end_t, delta_t;
	uint32_t framecount = 0;
	boolean quit = FALSE;	
	mat4 rotY, rotZ, D, transT;

	memset(&ctrlValues, 0, sizeof(ctrlValues));
	hThread = CreateThread(NULL, 0, getCtrlValueThread, &ctrlValues, 0, &threadID);

	start_t = clock(); //FPS
	while (!glfwWindowShouldClose(pWindow) && !quit)
	{
		getRotY4(rotY, ctrlValues.rStickX);
		getRotZ4(rotZ, ctrlValues.rStickY);
		getTrans4(transT, ctrlValues.lStickX, 0.0f, ctrlValues.lStickY);
		
		dup4(D, obj2.ubo.mModel);
		mult4(obj2.ubo.mModel, rotZ, D);
		dup4(D, obj3.ubo.mView);
		mult4(obj3.ubo.mView, rotY, D);
		dup4(D, obj3.ubo.mView);
		mult4(obj3.ubo.mView, transT, D);

		framecount++;
		end_t = clock();
		delta_t = end_t - start_t;

		if (delta_t >= CLOCKS_PER_SEC)
		{
			printf("FPS = %u\n", framecount);
			start_t = clock();
			framecount = 0;
		}
		
		glfwPollEvents();
		updateUniformBuffer();
		drawFrame();
	}
	GetExitCodeThread(hThread, &threadExitCode);
	TerminateThread(hThread, threadExitCode);
}

void shutdownGLFW()
{
	glfwDestroyWindow(pWindow);
	glfwTerminate();
}

int init_hid(libusb_device_handle **hid_dev, unsigned int vendor_id, unsigned int product_id, char *deviceName)
{
	int ret;

	*hid_dev = libusb_open_device_with_vid_pid(NULL, vendor_id, product_id);
	if (!*hid_dev)
	{
		printf("USB %s wurde nicht gefunden!\n", deviceName);
		return -1;
	}
	printf("USB %s erkannt!\n", deviceName);

	//libusb_detach_kernel_driver(*hid_dev, HID_INTERFACE);

	ret = libusb_claim_interface(*hid_dev, HID_INTERFACE);
	if (ret < 0)
	{
		printf("Claim %s Interface fehlgeschlagen: %d!\n", deviceName, ret);
		return -1;
	}
	printf("USB %s interface claimed!\n", deviceName);

	return 0;
}

void close_hid(libusb_device_handle *hid_dev)

{

	libusb_release_interface(hid_dev, HID_INTERFACE);

	//libusb_attach_kernel_driver(hid_dev, HID_INTERFACE);

	libusb_close(hid_dev);

}

int main(int argc, char *argv[])
{
	int result;
	mat4 A, B;
	printf("=======================\n");
	printf("***** %s *****\n", appName);
	printf("=======================\n\n");

	//identity4(A);
	zero4(A);
	A[0][0] = 1.0f; A[0][1] = 2.0f; A[0][2] = 3.0f; A[0][3] = 4.0f;
	printMatrix4(A,"Matrix A");
	transpose4(B, A);
	printMatrix4(B, "Matrix B");

	if (argc > 1)
	{
		if (!strcmp(argv[1], "version"))
		{
			printf("Version: 0.0.0\n");
			exit(0);
		}
		else
		{
			printf("%s unbekanntes Argument!\n", argv[1]);
			exit(-1);
		}
	}

	result = libusb_init(NULL);
	assert(result, "libusb_init failed!\n");
	result = init_hid(&hid_gamecontroller, GAMECONTROLLER_VENDOR_ID, GAMECONTROLLER_PRODUCT_ID, "Game Controller");
	assert(result, "init_hid failed!\n");

	startGLFW();
	setupVulkan();
	mainLoop();
	shutdownVulkan();
	shutdownGLFW();

	close_hid(hid_gamecontroller);
	libusb_exit(NULL);

	return 0;
}