#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "MyVulkanEngine.h"

static const char engineName[] = "MyVulkanEngine";
static const VkFormat ourFormat = VK_FORMAT_B8G8R8A8_UNORM;

void assert(VkResult result, char *msg)
{
	if (result != VK_SUCCESS)
	{
		printf("Error %d: %s\n", result, msg);
		__debugbreak();
		exit(-1);
	}
}

void createInstance(const char *appName, const char **glfwExtensions, uint32_t glfwExtensionsCount)
{
	VkResult result;
	VkApplicationInfo appInfo;
	uint32_t layersCount;
	VkLayerProperties *pLayers;
	const char *ppValidationLayer[1];
	uint32_t extensionsCount;
	VkExtensionProperties *pExtensions;
	//const char *ppUsedExtensions[2]; --> GLFW erledigt das
	VkInstanceCreateInfo instanceInfo;
	//VkWin32SurfaceCreateInfoKHR surfaceCreateInfo; --> GLFW erledigt das
	//VkSurfaceKHR surface; --> GLFW erledigt das

	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pNext = NULL;
	appInfo.pApplicationName = appName;
	appInfo.applicationVersion = VK_MAKE_VERSION(0, 0, 0);
	appInfo.pEngineName = engineName;
	appInfo.engineVersion = VK_MAKE_VERSION(0, 0, 0);
	appInfo.apiVersion = 0;

	vkEnumerateInstanceLayerProperties(&layersCount, NULL);
	printf("Anzahl Layers: %u\n", layersCount);
	pLayers = malloc(sizeof(VkLayerProperties) * layersCount);
	vkEnumerateInstanceLayerProperties(&layersCount, pLayers);
	for (uint32_t i = 0; i < layersCount; i++)
	{
		printf("\nlayerName:             %s\n", pLayers[i].layerName);
		printf("specVersion:           %u\n", pLayers[i].specVersion);
		printf("implementationVersion: %u\n", pLayers[i].implementationVersion);
		printf("description:           %s\n", pLayers[i].description);
	}
	ppValidationLayer[0] = pLayers[12].layerName;
	printf("\nvalidationLayer: %s\n", ppValidationLayer[0]);

	vkEnumerateInstanceExtensionProperties(NULL, &extensionsCount, NULL);
	printf("\nAnzahl Extensions: %u\n", extensionsCount);
	pExtensions = malloc(sizeof(VkExtensionProperties) * extensionsCount);
	vkEnumerateInstanceExtensionProperties(NULL, &extensionsCount, pExtensions);
	for (uint32_t i = 0; i < extensionsCount; i++)
	{
		printf("\nExtension Name: %s\n", pExtensions[i].extensionName);
		printf("SpecVersion:    %u\n", pExtensions[i].specVersion);
	}

	/* --> GLFW erledigt das
	ppUsedExtensions[0] = "VK_KHR_surface"; --> GLFW erledigt das
	ppUsedExtensions[1] = VK_KHR_WIN32_SURFACE_EXTENSION_NAME; --> GLFW erledigt das
	*/

	instanceInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	instanceInfo.pNext = NULL;
	instanceInfo.flags = 0;
	instanceInfo.pApplicationInfo = &appInfo;
	instanceInfo.enabledLayerCount = 1;
	instanceInfo.ppEnabledLayerNames = ppValidationLayer;
	instanceInfo.enabledExtensionCount = glfwExtensionsCount;
	instanceInfo.ppEnabledExtensionNames = glfwExtensions;
	/* --> GLFW erledigt das
	instanceInfo.enabledExtensionCount = 2;
	instanceInfo.ppEnabledExtensionNames = ppUsedExtensions;
	*/

	result = vkCreateInstance(&instanceInfo, NULL, &instance);
	assert(result, "vkCreateInstance failed!");

	free(pLayers);
	free(pExtensions);
}

void getPhysicalDevices()
{
	VkResult result;
	uint32_t physicalDeviceCount;
	VkBool32 surfaceSupport;

	result = vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, NULL);
	assert(result, "vkEnumeratePhysicalDevices failed!");
	printf("Anzahl physiaklische Grafikkarten: %u\n\n", physicalDeviceCount);

	pPhysicalDevices = malloc(sizeof(VkPhysicalDevice) * physicalDeviceCount);
	result = vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, pPhysicalDevices);
	assert(result, "vkEnumeratePhysicalDevices failed!");

	for (uint32_t i = 0; i<physicalDeviceCount; i++)
		printStats(pPhysicalDevices[i]);

	result = vkGetPhysicalDeviceSurfaceSupportKHR(pPhysicalDevices[0], 0, surface, &surfaceSupport);
	assert(result, "vkGetPhysicalDeviceSurfaceSupportKHR failed!");
	if (!surfaceSupport)
	{
		printf("Error: Surface not Supported!\n");
		__debugbreak();
		exit(-1);
	}
}

void createLogicalDevice()
{
	VkResult result;
	VkDeviceQueueCreateInfo deviceQueueCreateInfo;
	const char *ppDeviceExtensions[1];
	VkDeviceCreateInfo deviceCreateInfo;
	VkPhysicalDeviceFeatures usedFeatures;
	float queuePrios[] = { 1.0f, 1.0f, 1.0f, 1.0f };

	deviceQueueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	deviceQueueCreateInfo.pNext = NULL;
	deviceQueueCreateInfo.flags = 0;
	deviceQueueCreateInfo.queueFamilyIndex = 0;
	deviceQueueCreateInfo.queueCount = 1;
	deviceQueueCreateInfo.pQueuePriorities = queuePrios;

	ppDeviceExtensions[0] = VK_KHR_SWAPCHAIN_EXTENSION_NAME;
	memset(&usedFeatures, 0, sizeof(VkPhysicalDeviceFeatures));

	deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	deviceCreateInfo.pNext = NULL;
	deviceCreateInfo.flags = 0;
	deviceCreateInfo.queueCreateInfoCount = 1;
	deviceCreateInfo.pQueueCreateInfos = &deviceQueueCreateInfo;
	deviceCreateInfo.enabledLayerCount = 0;
	deviceCreateInfo.ppEnabledLayerNames = NULL;
	deviceCreateInfo.enabledExtensionCount = 1;
	deviceCreateInfo.ppEnabledExtensionNames = ppDeviceExtensions;
	deviceCreateInfo.pEnabledFeatures = &usedFeatures;

	result = vkCreateDevice(pPhysicalDevices[0], &deviceCreateInfo, NULL, &device);
	assert(result, "vkCreateDevice failed!");

	vkGetDeviceQueue(device, 0, 0, &queue);
}

void createSwapchain(uint32_t width, uint32_t height)
{
	VkResult result;
	VkSwapchainCreateInfoKHR swapchainCreateInfo;
	VkExtent2D imageExtend = { width, height };

	swapchainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	swapchainCreateInfo.pNext = NULL;
	swapchainCreateInfo.flags = 0;
	swapchainCreateInfo.surface = surface;
	swapchainCreateInfo.minImageCount = 3; // ToDo: check if valid (civ)
	swapchainCreateInfo.imageFormat = ourFormat; // ToDo: check if valid (civ)
	swapchainCreateInfo.imageColorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR; // ToDo: check if valid (civ)
	swapchainCreateInfo.imageExtent = imageExtend;
	swapchainCreateInfo.imageArrayLayers = 1;
	swapchainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
	swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE; // ToDo: check if valid (civ)
	swapchainCreateInfo.queueFamilyIndexCount = 0;
	swapchainCreateInfo.pQueueFamilyIndices = NULL;
	swapchainCreateInfo.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
	swapchainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	swapchainCreateInfo.presentMode = VK_PRESENT_MODE_FIFO_KHR; // ToDo: check if valid (civ)
	swapchainCreateInfo.clipped = VK_TRUE;
	swapchainCreateInfo.oldSwapchain = VK_NULL_HANDLE;

	result = vkCreateSwapchainKHR(device, &swapchainCreateInfo, NULL, &swapchain);
	assert(result, "vkCreateSwapchainKHR failed!\n");
}

void createImageViews()
{
	VkResult result;
	VkImage *pSwapchainImages;
	VkImageViewCreateInfo imageViewCreateInfo;

	vkGetSwapchainImagesKHR(device, swapchain, &imagesInSwapChainCount, NULL);
	printf("Anzahl Images in Swapchain: %u\n", imagesInSwapChainCount);
	pSwapchainImages = malloc(sizeof(VkImage) * imagesInSwapChainCount);
	result = vkGetSwapchainImagesKHR(device, swapchain, &imagesInSwapChainCount, pSwapchainImages);
	assert(result, "vkGetSwapchainImagesKHR failed!\n");

	pImageViews = malloc(sizeof(VkImageView) * imagesInSwapChainCount);
	for (uint32_t i = 0; i < imagesInSwapChainCount; i++)
	{
		imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		imageViewCreateInfo.pNext = NULL;
		imageViewCreateInfo.flags = 0;
		imageViewCreateInfo.image = pSwapchainImages[i];
		imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		imageViewCreateInfo.format = ourFormat; // ToDo: check if valid (civ);
		imageViewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		imageViewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		imageViewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		imageViewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
		imageViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
		imageViewCreateInfo.subresourceRange.levelCount = 1;
		imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
		imageViewCreateInfo.subresourceRange.layerCount = 1;

		result = vkCreateImageView(device, &imageViewCreateInfo, NULL, &pImageViews[i]);
		assert(result, "vkCreateImageView failed!\n");
	}

	free(pSwapchainImages);
}

void createRenderPass()
{
	VkResult result;
	VkAttachmentDescription attachmentDescription;
	VkAttachmentDescription depthAttachment;
	VkAttachmentDescription attachments[2];
	VkAttachmentReference attachmentReference;
	VkAttachmentReference depthAttachmentRef;
	VkSubpassDescription subpathDescription;
	VkSubpassDependency subpassDependency;
	VkRenderPassCreateInfo renderPathCreateInfo;

	attachmentDescription.flags = 0;
	attachmentDescription.format = ourFormat;
	attachmentDescription.samples = VK_SAMPLE_COUNT_1_BIT;
	attachmentDescription.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	attachmentDescription.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	attachmentDescription.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	attachmentDescription.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	attachmentDescription.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	attachmentDescription.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

	depthAttachment.flags = 0;
	depthAttachment.format = VK_FORMAT_D32_SFLOAT;
	depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
	depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	depthAttachment.initialLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
	depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	attachmentReference.attachment = 0;
	attachmentReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	depthAttachmentRef.attachment = 1;
	depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	subpathDescription.flags = 0;
	subpathDescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpathDescription.inputAttachmentCount = 0;
	subpathDescription.pInputAttachments = NULL;
	subpathDescription.colorAttachmentCount = 1;
	subpathDescription.pColorAttachments = &attachmentReference;
	subpathDescription.pResolveAttachments = NULL;
	subpathDescription.pDepthStencilAttachment = &depthAttachmentRef;
	subpathDescription.preserveAttachmentCount = 0;
	subpathDescription.pPreserveAttachments = NULL;

	subpassDependency.srcSubpass = VK_SUBPASS_EXTERNAL;
	subpassDependency.dstSubpass = 0;
	subpassDependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	subpassDependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	subpassDependency.srcAccessMask = 0;
	subpassDependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
	subpassDependency.dependencyFlags = 0;

	attachments[0] = attachmentDescription;
	attachments[1] = depthAttachment;
	renderPathCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPathCreateInfo.pNext = NULL;
	renderPathCreateInfo.flags = 0;
	renderPathCreateInfo.attachmentCount = 2;
	renderPathCreateInfo.pAttachments = attachments;
	renderPathCreateInfo.subpassCount = 1;
	renderPathCreateInfo.pSubpasses = &subpathDescription;
	renderPathCreateInfo.dependencyCount = 1;
	renderPathCreateInfo.pDependencies = &subpassDependency;

	result = vkCreateRenderPass(device, &renderPathCreateInfo, NULL, &renderPath);
	assert(result, "vkCreateRenderPass failed!\n");
}

int loadShader(char **shaderStr, char *fileName)
{
	unsigned int filesize;
	FILE *file = fopen(fileName, "rb"); // ***** !!! Sehr Wichtig: hier muss das b bei "rb" übergeben werden !!! b steht für binäre Datei *****

	fseek(file, 0, SEEK_END);
	filesize = ftell(file);
	rewind(file);
	*shaderStr = (char*)malloc(filesize);
	fread(*shaderStr, 1, filesize, file);
	printf("\n%s geladen. Filesize: %d\n", fileName, filesize);
	fclose(file);

	return filesize;
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
	transitionImageLayout(depthImage, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);
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

void transitionImageLayout(VkImage image, VkImageLayout oldLayout, VkImageLayout newLayout)
{
	VkCommandBufferAllocateInfo commandBufferAllocateInfo;
	VkCommandBuffer cmdBuffer;
	VkCommandBufferBeginInfo beginInfo;
	VkSubmitInfo submitInfo;
	VkImageMemoryBarrier barrier;

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

	barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	barrier.pNext = 0;
	barrier.srcAccessMask = 0;
	barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
	barrier.oldLayout = oldLayout;
	barrier.newLayout = newLayout;
	barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.image = image;
	barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
	barrier.subresourceRange.baseMipLevel = 0;
	barrier.subresourceRange.levelCount = 1;
	barrier.subresourceRange.baseArrayLayer = 0;
	barrier.subresourceRange.layerCount = 1;

	vkCmdPipelineBarrier(cmdBuffer, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, 0, 0, NULL, 0, NULL, 1, &barrier);

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