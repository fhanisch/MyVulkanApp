/*
	MyVulkanApp

	Date: 23.03.2017
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#define VK_USE_PLATFORM_WIN32_KHR --> GLFW erledigt das
#define GLFW_INCLUDE_VULKAN
#include <GLFW\glfw3.h>
//#include <vulkan\vulkan.h> --> wird durch GLFW includiert
#include "matrix.h"

typedef struct  {
	mat4 mModel;
	mat4 mView;
	mat4 mProj;
} UniformBufferObject;

typedef struct {
	vec2 pos[2];
	vec2 texCoords[2];
} Vertex;


VkInstance instance;
VkPhysicalDevice *pPhysicalDevices;
VkSurfaceKHR surface;
VkDevice device;
VkSwapchainKHR swapchain;
uint32_t imagesInSwapChainCount;
VkImageView *pImageViews;
VkFramebuffer *pFramebuffer;
VkShaderModule vertexShaderModule, fragmentShaderModule;
VkDescriptorSetLayout descriptorSetLayout;
VkPipelineLayout pipelineLayout;
VkRenderPass renderPath;
VkPipeline pipeline1, pipeline2;
VkCommandPool commandPool;
VkCommandBuffer *pCommandBuffers;
VkSemaphore semaphoreImageAvailable;
VkSemaphore semaphoreRenderingDone;
VkQueue queue;
VkBuffer vertexBuffer;
VkBuffer indexBuffer;
VkBuffer uniformBuffer;
VkDeviceMemory vertexBufferDeviceMemory;
VkDeviceMemory indexBufferDeviceMemory;
VkDeviceMemory uniformBufferDeviceMemory;
VkDescriptorPool descriptorPool;
VkDescriptorSet descriptorSet;
GLFWwindow *pWindow;

const uint32_t wndWidth = 1000;
const uint32_t wndHeight = 1000;
const VkFormat ourFormat = VK_FORMAT_B8G8R8A8_UNORM;

static const char appName[] = "MyVulkanApp";
static const char engineName[] = "MyVulkanEngine";

static Vertex vertices[] = { 
								{ {-1.0f,  1.0f}, { -1.0f,  1.0f } },
								{ {-1.0f, -1.0f}, { -1.0f, -1.0f } },
								{ { 1.0f, -1.0f}, {  1.0f, -1.0f } },
								{ { 1.0f,  1.0f}, {  1.0f,  1.0f } },
								{ { 0.0f, -1.0f}, {  0.0f, -1.0f } }
							};
							 
static uint16_t indices[] = { 0, 1, 2, 0, 2, 3 };


static UniformBufferObject ubo;

void assert(VkResult result, char *msg)
{
	if (result != VK_SUCCESS)
	{
		printf("Error %d: %s\n", result, msg);
		__debugbreak();
		exit(-1);
	}
}

VkVertexInputBindingDescription getBindingDescription()
{
	VkVertexInputBindingDescription vertexInputBindingDescription;

	vertexInputBindingDescription.binding = 0;
	vertexInputBindingDescription.stride = sizeof(Vertex);
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

VkVertexInputAttributeDescription getAttributeDescription(uint32_t location, uint32_t offset)
{
	VkVertexInputAttributeDescription vertexInputAttributeDescription;

	vertexInputAttributeDescription.location = location;
	vertexInputAttributeDescription.binding = 0;
	vertexInputAttributeDescription.format = VK_FORMAT_R32G32_SFLOAT;
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

void startGLFW()
{
	if (!glfwInit()) exit(-1);
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	pWindow = glfwCreateWindow(wndWidth, wndHeight, appName, NULL, NULL);
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

void createInstance()
{
	VkResult result;
	VkApplicationInfo appInfo;
	uint32_t layersCount;
	VkLayerProperties *pLayers;
	const char *ppValidationLayer[1];
	uint32_t extensionsCount;
	VkExtensionProperties *pExtensions;
	//const char *ppUsedExtensions[2]; --> GLFW erledigt das
	const char **glfwExtensions;
	uint32_t glfwExtensionsCount;
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

	glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionsCount);
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

void createSwapchain()
{
	VkResult result;
	VkSwapchainCreateInfoKHR swapchainCreateInfo;
	VkExtent2D imageExtend = { wndWidth, wndHeight };

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

	descriptorSetLayoutBinding = getDescriptorSetLayoutBinding();

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

	//Wichitg für Shader-Uniform varibalen!!!
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

void createRenderPass()
{
	VkResult result;
	VkAttachmentDescription attachmentDescription;
	VkAttachmentReference attachmentReference;
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

	attachmentReference.attachment = 0;
	attachmentReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	subpathDescription.flags = 0;
	subpathDescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpathDescription.inputAttachmentCount = 0;
	subpathDescription.pInputAttachments = NULL;
	subpathDescription.colorAttachmentCount = 1;
	subpathDescription.pColorAttachments = &attachmentReference;
	subpathDescription.pResolveAttachments = NULL;
	subpathDescription.pDepthStencilAttachment = NULL;
	subpathDescription.preserveAttachmentCount = 0;
	subpathDescription.pPreserveAttachments = NULL;

	subpassDependency.srcSubpass = VK_SUBPASS_EXTERNAL;
	subpassDependency.dstSubpass = 0;
	subpassDependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	subpassDependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	subpassDependency.srcAccessMask = 0;
	subpassDependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
	subpassDependency.dependencyFlags = 0;

	renderPathCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPathCreateInfo.pNext = NULL;
	renderPathCreateInfo.flags = 0;
	renderPathCreateInfo.attachmentCount = 1;
	renderPathCreateInfo.pAttachments = &attachmentDescription;
	renderPathCreateInfo.subpassCount = 1;
	renderPathCreateInfo.pSubpasses = &subpathDescription;
	renderPathCreateInfo.dependencyCount = 1;
	renderPathCreateInfo.pDependencies = &subpassDependency;

	result = vkCreateRenderPass(device, &renderPathCreateInfo, NULL, &renderPath);
	assert(result, "vkCreateRenderPass failed!\n");
}

void createGraphicsPipeline(char *pVertShaderFileNames, char *pFragShaderFileNmae, VkPipeline *pPipeline)
{
	VkResult result;
	VkPipelineShaderStageCreateInfo shaderStageCreateInfoVert, shaderStageCreateInfoFrag;
	VkPipelineShaderStageCreateInfo shaderStages[2];
	VkVertexInputBindingDescription vertexInputBindingDescription;
	VkVertexInputAttributeDescription vertexInputAttributeDescription[2];
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

	createShaderModule(pVertShaderFileNames, &vertexShaderModule);
	createShaderModule(pFragShaderFileNmae, &fragmentShaderModule);

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

	vertexInputBindingDescription = getBindingDescription();
	vertexInputAttributeDescription[0] = getAttributeDescription(0, offsetof(Vertex,pos));
	vertexInputAttributeDescription[1] = getAttributeDescription(1, offsetof(Vertex, texCoords));

	vertexInputCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	vertexInputCreateInfo.pNext = NULL;
	vertexInputCreateInfo.flags = 0;
	vertexInputCreateInfo.vertexBindingDescriptionCount = 1;
	vertexInputCreateInfo.pVertexBindingDescriptions = &vertexInputBindingDescription;
	vertexInputCreateInfo.vertexAttributeDescriptionCount = 2;
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
	rasterizationCreateInfo.cullMode = VK_CULL_MODE_BACK_BIT;
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
	pipelineCreateInfo.pDepthStencilState = NULL;
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

	pFramebuffer = malloc(sizeof(VkFramebuffer) * imagesInSwapChainCount);
	for (uint32_t i = 0; i < imagesInSwapChainCount; i++)
	{
		framebufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferCreateInfo.pNext = NULL;
		framebufferCreateInfo.flags = 0;
		framebufferCreateInfo.renderPass = renderPath;
		framebufferCreateInfo.attachmentCount = 1;
		framebufferCreateInfo.pAttachments = &(pImageViews[i]);
		framebufferCreateInfo.width = wndWidth;
		framebufferCreateInfo.height = wndHeight;
		framebufferCreateInfo.layers = 1;

		result = vkCreateFramebuffer(device, &framebufferCreateInfo, NULL, &(pFramebuffer[i]));
		assert(result, "vkCreateFramebuffer failed!\n");
	}
}

void createBuffer(VkBuffer *pBuffer, VkDeviceSize bufferSize, VkBufferUsageFlags usage, VkDeviceMemory *pMemory)
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
	memoryAllocateInfo.memoryTypeIndex = findMemoryTypeIndex(memoryRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

	result = vkAllocateMemory(device, &memoryAllocateInfo, NULL, pMemory);
	assert(result, "vkAllocateMemory failed!\n");

	vkBindBufferMemory(device, *pBuffer, *pMemory, 0);
}

void createVertexBuffer()
{
	VkDeviceSize bufferSize = sizeof(vertices);
	void *rawData;

	createBuffer(&vertexBuffer, bufferSize, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, &vertexBufferDeviceMemory);

	vkMapMemory(device, vertexBufferDeviceMemory, 0, bufferSize, 0, &rawData);
	memcpy(rawData, vertices, bufferSize);
	vkUnmapMemory(device, vertexBufferDeviceMemory);
}

void createIndexBuffer()
{
	VkDeviceSize bufferSize = sizeof(indices);
	void *rawData;

	createBuffer(&indexBuffer, bufferSize, VK_BUFFER_USAGE_INDEX_BUFFER_BIT, &indexBufferDeviceMemory);

	vkMapMemory(device, indexBufferDeviceMemory, 0, bufferSize, 0, &rawData);
	memcpy(rawData, indices, bufferSize);
	vkUnmapMemory(device, indexBufferDeviceMemory);
}

void createUniformBuffer()
{
	VkDeviceSize bufferSize = sizeof(UniformBufferObject);
	void *rawData;
	
	identity4(ubo.mModel);
	identity4(ubo.mView);
	identity4(ubo.mProj);
	//ubo.mModel[0][0] = 0.5f;

	createBuffer(&uniformBuffer, bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, &uniformBufferDeviceMemory);
	
	vkMapMemory(device, uniformBufferDeviceMemory, 0, bufferSize, 0, &rawData);
	memcpy(rawData, &ubo, bufferSize);
	vkUnmapMemory(device, uniformBufferDeviceMemory);
	
}

void updateUniformBuffer()
{
	VkDeviceSize bufferSize = sizeof(UniformBufferObject);
	void *rawData;
	mat4 Z, D;	
	
	dup4(D, ubo.mModel);
	getRotZ4(Z, -0.01f);
	mult4(ubo.mModel, Z, D);
	identity4(ubo.mView);
	identity4(ubo.mProj);

	vkMapMemory(device, uniformBufferDeviceMemory, 0, bufferSize, 0, &rawData);
	memcpy(rawData, &ubo, bufferSize);
	vkUnmapMemory(device, uniformBufferDeviceMemory);
}

void createDescriptorPool()
{
	VkResult result;
	VkDescriptorPoolSize poolSize;
	VkDescriptorPoolCreateInfo poolInfo;

	poolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	poolSize.descriptorCount = 1;

	poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	poolInfo.pNext = NULL;
	poolInfo.flags = 0;
	poolInfo.maxSets = 1;
	poolInfo.poolSizeCount = 1;
	poolInfo.pPoolSizes = &poolSize;

	result = vkCreateDescriptorPool(device, &poolInfo, NULL, &descriptorPool);
	assert(result, "vkCreateDescriptorPool failed!\n");
}

void createDescriptorSet()
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

	result = vkAllocateDescriptorSets(device, &allocInfo, &descriptorSet);
	assert(result, "vkAllocateDescriptorSets failed!\n");

	bufferInfo.buffer = uniformBuffer;
	bufferInfo.offset = 0;
	bufferInfo.range = sizeof(UniformBufferObject);

	descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	descriptorWrite.pNext = NULL;
	descriptorWrite.dstSet = descriptorSet;
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
	VkClearValue clearValue = { 0.0f, 0.0f, 1.0f, 1.0f };
	VkDeviceSize offsets[] = { 0 };

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
		renderPassBeginInfo.clearValueCount = 1;
		renderPassBeginInfo.pClearValues = &clearValue;

		vkCmdBeginRenderPass(pCommandBuffers[i], &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
		vkCmdBindPipeline(pCommandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline1);
		vkCmdBindVertexBuffers(pCommandBuffers[i], 0, 1, &vertexBuffer, offsets);
		vkCmdBindIndexBuffer(pCommandBuffers[i], indexBuffer, 0, VK_INDEX_TYPE_UINT16);
		vkCmdBindDescriptorSets(pCommandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &descriptorSet, 0, NULL);
		//vkCmdDraw(pCommandBuffers[i], 3, 1, 0, 0);
		vkCmdDrawIndexed(pCommandBuffers[i], sizeof(indices) / 2 , 1, 0, 0, 0);

		vkCmdBindPipeline(pCommandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline2);
		vkCmdBindDescriptorSets(pCommandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &descriptorSet, 0, NULL);
		vkCmdDrawIndexed(pCommandBuffers[i], sizeof(indices) / 2 - 3, 1, 0, 0, 0);

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
	createInstance();
	createSurface();
	getPhysicalDevices();		
	createLogicalDevice();		
	createSwapchain();
	createImageViews();
	createDescriptorSetLayout();
	createPipelineLayout();
	createRenderPass();
	createGraphicsPipeline("vs_generic.spv", "fs_powermeter.spv", &pipeline1);
	createGraphicsPipeline("vs_generic.spv", "fs_generic.spv", &pipeline2);
	createFramebuffer();	
	createVertexBuffer();
	createIndexBuffer();
	createUniformBuffer();
	createDescriptorPool();
	createDescriptorSet();
	createCommandPool();
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

void mainLoop()
{
	while (!glfwWindowShouldClose(pWindow))
	{
		glfwPollEvents();
		//updateUniformBuffer();
		drawFrame();
	}
}

void shutdownVulkan()
{
	vkDeviceWaitIdle(device);

	vkDestroyDescriptorPool(device, descriptorPool, NULL);
	vkDestroyDescriptorSetLayout(device, descriptorSetLayout, NULL);
	vkFreeMemory(device, uniformBufferDeviceMemory, NULL);
	vkFreeMemory(device, vertexBufferDeviceMemory, NULL);
	vkDestroyBuffer(device, uniformBuffer, NULL);
	vkDestroyBuffer(device, vertexBuffer, NULL);
	vkDestroySemaphore(device, semaphoreImageAvailable, NULL);
	vkDestroySemaphore(device, semaphoreRenderingDone, NULL);
	vkFreeCommandBuffers(device, commandPool, imagesInSwapChainCount, pCommandBuffers);
	free(pCommandBuffers);
	vkDestroyCommandPool(device, commandPool, NULL);
	for (uint32_t i = 0; i < imagesInSwapChainCount; i++)
	{
		vkDestroyFramebuffer(device, pFramebuffer[i], NULL);
	}
	free(pFramebuffer);
	vkDestroyPipeline(device, pipeline1, NULL);
	vkDestroyRenderPass(device, renderPath, NULL);
	for (uint32_t i = 0; i < imagesInSwapChainCount; i++)
	{
		vkDestroyImageView(device, pImageViews[i], NULL);
	}
	free(pImageViews);
	vkDestroyPipelineLayout(device, pipelineLayout, NULL);
	vkDestroyShaderModule(device, vertexShaderModule, NULL);
	vkDestroyShaderModule(device, fragmentShaderModule, NULL);
	vkDestroySwapchainKHR(device, swapchain, NULL);
	vkDestroyDevice(device, NULL);	
	vkDestroySurfaceKHR(instance, surface, NULL);
	free(pPhysicalDevices);
	vkDestroyInstance(instance, NULL);
}

void shutdownGLFW()
{
	glfwDestroyWindow(pWindow);
	glfwTerminate();
}

int main(int argc, char *argv[])
{
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

	startGLFW();
	setupVulkan();
	mainLoop();
	shutdownVulkan();
	shutdownGLFW();

	return 0;
}