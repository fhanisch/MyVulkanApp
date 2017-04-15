#include <stdio.h>
#include <stdlib.h>
#include "MyVulkanEngine.h"

void assert(VkResult result, char *msg)
{
	if (result != VK_SUCCESS)
	{
		printf("Error %d: %s\n", result, msg);
		__debugbreak();
		exit(-1);
	}
}

VkVertexInputBindingDescription getBindingDescription(uint32_t stride)
{
	VkVertexInputBindingDescription vertexInputBindingDescription;

	vertexInputBindingDescription.binding = 0;
	vertexInputBindingDescription.stride = stride;
	vertexInputBindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

	return vertexInputBindingDescription;
}

VkDescriptorSetLayoutBinding getDescriptorSetLayoutBinding()
{
	VkDescriptorSetLayoutBinding descriptorSetLayoutBinding;

	descriptorSetLayoutBinding.binding = 0;
	descriptorSetLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	descriptorSetLayoutBinding.descriptorCount = 1;
	descriptorSetLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
	descriptorSetLayoutBinding.pImmutableSamplers = NULL;

	return descriptorSetLayoutBinding;
}

VkVertexInputAttributeDescription getAttributeDescription(uint32_t location, uint32_t offset, VkFormat format)
{
	VkVertexInputAttributeDescription vertexInputAttributeDescription;

	vertexInputAttributeDescription.location = location;
	vertexInputAttributeDescription.binding = 0;
	vertexInputAttributeDescription.format = format;
	vertexInputAttributeDescription.offset = offset;

	return vertexInputAttributeDescription;
}

uint32_t findMemoryTypeIndex(uint32_t typeFilter, VkMemoryPropertyFlags properties)
{
	VkPhysicalDeviceMemoryProperties physicalDeviceMemoryProperties;
	vkGetPhysicalDeviceMemoryProperties(pPhysicalDevices[0], &physicalDeviceMemoryProperties); //ToDo: civ

	for (uint32_t i = 0; i < physicalDeviceMemoryProperties.memoryTypeCount; i++)
	{
		if ((typeFilter & (1 << i)) && (physicalDeviceMemoryProperties.memoryTypes[i].propertyFlags & properties) == properties)
		{
			return i;
		}
	}
	printf("Found no correct memory type!\n");
	exit(-1);
}

void printStats(VkPhysicalDevice physDevice)
{
	VkPhysicalDeviceProperties properties;
	VkFormatProperties formatProperties;
	VkPhysicalDeviceFeatures features;
	VkPhysicalDeviceMemoryProperties memProp;
	uint32_t queueFamilyCount;
	VkQueueFamilyProperties *pQueueFamilyProperties;
	uint32_t width, height, depth;
	VkSurfaceCapabilitiesKHR surfaceCapabilities;
	uint32_t formatsCount;
	VkSurfaceFormatKHR *pSurfaceFormat;
	uint32_t presentationModeCount;
	VkPresentModeKHR *pPresentModes;

	vkGetPhysicalDeviceProperties(physDevice, &properties);
	vkGetPhysicalDeviceFeatures(physDevice, &features);
	vkGetPhysicalDeviceMemoryProperties(physDevice, &memProp);
	vkGetPhysicalDeviceQueueFamilyProperties(physDevice, &queueFamilyCount, NULL);
	pQueueFamilyProperties = malloc(sizeof(VkQueueFamilyProperties) * queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(physDevice, &queueFamilyCount, pQueueFamilyProperties);
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physDevice, surface, &surfaceCapabilities);
	vkGetPhysicalDeviceFormatProperties(physDevice, VK_FORMAT_D32_SFLOAT, &formatProperties);

	printf("ApiVersion:              %u.%u.%u\n", VK_VERSION_MAJOR(properties.apiVersion), VK_VERSION_MINOR(properties.apiVersion), VK_VERSION_PATCH(properties.apiVersion));
	printf("DriverVersion:           %u\n", properties.driverVersion);
	printf("VendorID:                %u\n", properties.vendorID);
	printf("DeviceID:                %u\n", properties.deviceID);
	printf("DeviceType:              %u\n", properties.deviceType);
	printf("DeviceName:              %s\n", properties.deviceName);
	printf("discreteQueuePriorities: %u\n", properties.limits.discreteQueuePriorities);
	//printf("%u\n", properties.pipelineCacheUUID);
	//properties.limits
	//properties.sparseProperties
	printf("Geometry Shader:       %d\n", features.geometryShader);
	printf("Tessellation Shader:   %d\n", features.tessellationShader);
	printf("Anzahl Queue-Familien: %u\n", queueFamilyCount);
	for (uint32_t i = 0; i < queueFamilyCount; i++)
	{
		printf("\nQueue Family Number # %u\n", i);
		printf("VK_QUEUE_GRAPHICS_BIT:       %d\n", (pQueueFamilyProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) != 0);
		printf("VK_QUEUE_COMPUTE_BIT:        %d\n", (pQueueFamilyProperties[i].queueFlags & VK_QUEUE_COMPUTE_BIT) != 0);
		printf("VK_QUEUE_TRANSFER_BIT:       %d\n", (pQueueFamilyProperties[i].queueFlags & VK_QUEUE_TRANSFER_BIT) != 0);
		printf("VK_QUEUE_SPARSE_BINDING_BIT: %d\n", (pQueueFamilyProperties[i].queueFlags & VK_QUEUE_SPARSE_BINDING_BIT) != 0);
		printf("Queue Count:                 %d\n", pQueueFamilyProperties[i].queueCount);
		printf("Timestamp Valid Bits:        %d\n", pQueueFamilyProperties[i].timestampValidBits);
		width = pQueueFamilyProperties[i].minImageTransferGranularity.width;
		height = pQueueFamilyProperties[i].minImageTransferGranularity.height;
		depth = pQueueFamilyProperties[i].minImageTransferGranularity.depth;
		printf("Min Image Timestamp Granularity: %u, %u, %u\n", width, height, depth);
	}
	printf("\n");

	printf("Surface Capabilities\n");
	printf("\tminImageCount:           %u\n", surfaceCapabilities.minImageCount);
	printf("\tmaxImageCount:           %u\n", surfaceCapabilities.maxImageCount);
	printf("\tcurrentExtent:           %u/%u\n", surfaceCapabilities.currentExtent.width, surfaceCapabilities.currentExtent.height);
	printf("\tminImageExtent:          %u/%u\n", surfaceCapabilities.minImageExtent.width, surfaceCapabilities.minImageExtent.height);
	printf("\tmaxImageExtent:          %u/%u\n", surfaceCapabilities.maxImageExtent.width, surfaceCapabilities.maxImageExtent.height);
	printf("\tmaxImageArrayLayers:     %u\n", surfaceCapabilities.maxImageArrayLayers);
	printf("\tsupportedTransforms:     %u\n", surfaceCapabilities.supportedTransforms);
	printf("\tcurrentTransform:        %u\n", surfaceCapabilities.currentTransform);
	printf("\tsupportedCompositeAlpha: %u\n", surfaceCapabilities.supportedCompositeAlpha);
	printf("\tsupportedUsageFlags:     %u\n", surfaceCapabilities.supportedUsageFlags);

	vkGetPhysicalDeviceSurfaceFormatsKHR(physDevice, surface, &formatsCount, NULL);
	printf("\nAnzahl Formate: %u\n", formatsCount);
	pSurfaceFormat = malloc(sizeof(VkSurfaceFormatKHR) * formatsCount);
	vkGetPhysicalDeviceSurfaceFormatsKHR(physDevice, surface, &formatsCount, pSurfaceFormat);
	for (uint32_t i = 0; i < formatsCount; i++)
	{
		printf("Format:    %u\n", pSurfaceFormat[i].format);
		printf("ColorSpace: %u\n", pSurfaceFormat[i].colorSpace);
	}

	vkGetPhysicalDeviceSurfacePresentModesKHR(physDevice, surface, &presentationModeCount, NULL);
	printf("\nAnzahl Presentation Modes: %u\n", presentationModeCount);
	pPresentModes = malloc(sizeof(VkPresentModeKHR) * presentationModeCount);
	vkGetPhysicalDeviceSurfacePresentModesKHR(physDevice, surface, &presentationModeCount, pPresentModes);
	for (uint32_t i = 0; i < presentationModeCount; i++)
	{
		printf("\tSupported presentation mode: %u\n", pPresentModes[i]);
	}

	free(pQueueFamilyProperties);
	free(pSurfaceFormat);
	free(pPresentModes);
}

void createDepthResources(uint32_t width, uint32_t height)
{
	VkFormat depthFormat;

	depthFormat = VK_FORMAT_D32_SFLOAT;
	createImage(width, height, depthFormat, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &depthImage, &depthImageMemory);
	createImageView(depthImage, depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT, &depthImageView);
}

void createImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage *pImage, VkDeviceMemory *pImageMemory)
{
	VkResult result;
	VkImageCreateInfo imageCreateInfo;
	VkMemoryRequirements memRequirements;
	VkMemoryAllocateInfo allocInfo;

	imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	imageCreateInfo.pNext = NULL;
	imageCreateInfo.flags = 0;
	imageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
	imageCreateInfo.format = format;
	imageCreateInfo.extent.width = width;
	imageCreateInfo.extent.height = height;
	imageCreateInfo.extent.depth = 1;
	imageCreateInfo.mipLevels = 1;
	imageCreateInfo.arrayLayers = 1;
	imageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
	imageCreateInfo.tiling = tiling;
	imageCreateInfo.usage = usage;
	imageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	imageCreateInfo.queueFamilyIndexCount = 0;
	imageCreateInfo.pQueueFamilyIndices = NULL;
	imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_PREINITIALIZED;	

	result = vkCreateImage(device, &imageCreateInfo, NULL, pImage);
	assert(result, "vkCreateImage() in createImage() failed!\n");

	vkGetImageMemoryRequirements(device, *pImage, &memRequirements);

	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.pNext = NULL;
	allocInfo.allocationSize = memRequirements.size;
	allocInfo.memoryTypeIndex = findMemoryTypeIndex(memRequirements.memoryTypeBits, properties);

	vkAllocateMemory(device, &allocInfo, NULL, pImageMemory);

	vkBindImageMemory(device, *pImage, *pImageMemory, 0);
}

void createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, VkImageView *pImageView)
{
	VkResult result;
	VkImageViewCreateInfo viewInfo;

	viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	viewInfo.pNext = NULL;
	viewInfo.flags = 0;
	viewInfo.image = image;
	viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
	viewInfo.format = format;
	viewInfo.components.r = 0;
	viewInfo.components.g = 0;
	viewInfo.components.b = 0;
	viewInfo.components.a = 0;
	viewInfo.subresourceRange.aspectMask = aspectFlags;
	viewInfo.subresourceRange.baseMipLevel = 0;
	viewInfo.subresourceRange.levelCount = 1;
	viewInfo.subresourceRange.baseArrayLayer = 0;
	viewInfo.subresourceRange.layerCount = 1;

	result = vkCreateImageView(device, &viewInfo, NULL, pImageView);
	assert(result, "vkCreateImageView() in createImageView() failed!\n");
}