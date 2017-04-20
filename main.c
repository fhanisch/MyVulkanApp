/*
	MyVulkanApp

	Date: 23.03.2017
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <libusb-1.0\libusb.h>
#include <Windows.h> // notwendig für Threads --> Alternative evtl. pthreads
//#define VK_USE_PLATFORM_WIN32_KHR --> GLFW erledigt das
#define GLFW_INCLUDE_VULKAN
#include <GLFW\glfw3.h>
//#include <vulkan\vulkan.h> --> wird durch GLFW includiert
#include "MyVulkanEngine.h"
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

// internal linkage --> Definition static: https://de.wikipedia.org/wiki/Static_(Schl%C3%BCsselwort) --> ohne 'static' würde 'extern' entsprechen
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
static unsigned int meshIndicesLength;

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
	VkDeviceSize bufferSize;
	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;
	vec2 *mesh;
	unsigned int meshGridSize;
	void *rawData;

	createMeshGrid((float**)&mesh, &meshGridSize, 100, 100);
	bufferSize = sizeof(vertices) + sizeof(verticesPlane) + meshGridSize;

	createBuffer(&stagingBuffer, bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &stagingBufferMemory);

	vkMapMemory(device, stagingBufferMemory, 0, bufferSize, 0, &rawData);
	memcpy(rawData, &vertices, sizeof(vertices));
	memcpy((char*)rawData + sizeof(vertices), &verticesPlane, sizeof(verticesPlane));
	memcpy((char*)rawData + sizeof(vertices) + sizeof(verticesPlane), mesh, meshGridSize);
	vkUnmapMemory(device, stagingBufferMemory);

	createBuffer(&vertexBuffer, bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &vertexBufferDeviceMemory);

	copyBuffer(stagingBuffer, vertexBuffer, bufferSize);
	
	vkFreeMemory(device, stagingBufferMemory, NULL);
	vkDestroyBuffer(device, stagingBuffer, NULL);
}

void createIndexBuffer()
{
	VkDeviceSize bufferSize;
	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;
	uint16_t *meshIndices;	
	unsigned int meshIndicesSize;
	void *rawData;

	createMeshGridIndices(&meshIndices, &meshIndicesLength, &meshIndicesSize, 100, 100);
	bufferSize = sizeof(indices) + sizeof(indices2) + sizeof(indices_plane) + meshIndicesSize;

	createBuffer(&stagingBuffer, bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &stagingBufferMemory);

	vkMapMemory(device, stagingBufferMemory, 0, bufferSize, 0, &rawData);
	memcpy(rawData, indices, sizeof(indices));
	memcpy((char*)rawData + sizeof(indices), indices2, sizeof(indices2));
	memcpy((char*)rawData + sizeof(indices)+ sizeof(indices2), indices_plane, sizeof(indices_plane));
	memcpy((char*)rawData + sizeof(indices) + sizeof(indices2) + sizeof(indices_plane), meshIndices, meshIndicesSize);
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
	VkDeviceSize offsets3[] = { sizeof(vertices) + sizeof(verticesPlane) };

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
		
		/*
		vkCmdBindVertexBuffers(pCommandBuffers[i], 0, 1, &vertexBuffer, offsets);
		
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
		/*
		vkCmdBindPipeline(pCommandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline3);
		vkCmdBindVertexBuffers(pCommandBuffers[i], 0, 1, &vertexBuffer, offsets2);
		vkCmdBindIndexBuffer(pCommandBuffers[i], indexBuffer, sizeof(indices) + sizeof(indices2), VK_INDEX_TYPE_UINT16);
		vkCmdBindDescriptorSets(pCommandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &descriptorSet3, 0, NULL);		
		vkCmdDrawIndexed(pCommandBuffers[i], sizeof(indices_plane) / 2, 1, 0, 0, 0);
		*/
		vkCmdBindPipeline(pCommandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline3);
		vkCmdBindVertexBuffers(pCommandBuffers[i], 0, 1, &vertexBuffer, offsets3);
		vkCmdBindIndexBuffer(pCommandBuffers[i], indexBuffer, sizeof(indices) + sizeof(indices2) + sizeof(indices_plane), VK_INDEX_TYPE_UINT16);
		vkCmdBindDescriptorSets(pCommandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &descriptorSet3, 0, NULL);
		vkCmdDrawIndexed(pCommandBuffers[i], meshIndicesLength, 1, 0, 0, 0);
		
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
	createGraphicsPipeline(wndWidth, wndHeight, &createInfo1, &pipeline1);
	createGraphicsPipeline(wndWidth, wndHeight, &createInfo2, &pipeline2);
	//createGraphicsPipeline(wndWidth, wndHeight, &createInfo3, &pipeline3);
	//createGraphicsPipeline(wndWidth, wndHeight, &quadCreateInfo, &pipeline3);
	createGraphicsPipeline(wndWidth, wndHeight, &sphereCreateInfo, &pipeline3);
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

int getCtrlValuesThread(CtrlValues *pCtrlValues)
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
	hThread = CreateThread(NULL, 0, getCtrlValuesThread, &ctrlValues, 0, &threadID);

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