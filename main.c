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

VkInstance instance;
VkSurfaceKHR surface;
VkDevice device;
VkSwapchainKHR swapchain;
uint32_t imagesInSwapChainCount;
VkImageView *pImageViews;
VkShaderModule vertexShaderModule, fragmentShaderModule;
GLFWwindow *pWindow;

const uint32_t wndWidth = 600;
const uint32_t wndHeight = 600;

static const char appName[] = "MyVulkanApp";
static const char engineName[] = "MyVulkanEngine";

void assert(VkResult result, char *msg)
{
	if (result != VK_SUCCESS)
	{
		printf("Error %d: %s\n", result, msg);
		__debugbreak();
		exit(-1);
	}
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
	FILE *file = fopen(fileName, "r");

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

	free(pLayers);
	free(pExtensions);
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

void setupVulkan()
{	
	VkResult result;
	uint32_t physicalDeviceCount;
	VkPhysicalDevice *pPhysicalDevices;
	VkDeviceQueueCreateInfo deviceQueueCreateInfo;
	const char *ppDeviceExtensions[1];
	VkDeviceCreateInfo deviceCreateInfo;
	VkPhysicalDeviceFeatures usedFeatures;
	float queuePrios[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	VkQueue queue;
	VkBool32 surfaceSupport;
	VkSwapchainCreateInfoKHR swapchainCreateInfo;
	VkExtent2D imageExtend = { wndWidth, wndHeight };	
	VkImage *pSwapchainImages;
	VkImageViewCreateInfo imageViewCreateInfo;	
	VkPipelineShaderStageCreateInfo shaderStageCreateInfoVert, shaderStageCreateInfoFrag;
	VkPipelineShaderStageCreateInfo shaderStages[2];
	VkPipelineVertexInputStateCreateInfo vertexInputCreateInfo;
		
	createInstance();

	result = vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, NULL);
	assert(result, "vkEnumeratePhysicalDevices failed!");
	printf("Anzahl physiaklische Grafikkarten: %u\n\n", physicalDeviceCount);

	pPhysicalDevices = malloc(sizeof(VkPhysicalDevice) * physicalDeviceCount);
	result = vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, pPhysicalDevices);
	assert(result, "vkEnumeratePhysicalDevices failed!");

	for (uint32_t i = 0; i<physicalDeviceCount; i++)
		printStats(pPhysicalDevices[i]);

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

	result = vkGetPhysicalDeviceSurfaceSupportKHR(pPhysicalDevices[0], 0, surface, &surfaceSupport);
	assert(result, "vkGetPhysicalDeviceSurfaceSupportKHR failed!");
	if (!surfaceSupport)
	{
		printf("Error: Surface not Supported!\n");
		__debugbreak();
		exit(-1);
	}

	swapchainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	swapchainCreateInfo.pNext = NULL;
	swapchainCreateInfo.flags = 0;
	swapchainCreateInfo.surface = surface;
	swapchainCreateInfo.minImageCount = 3; // ToDo: check if valid (civ)
	swapchainCreateInfo.imageFormat = VK_FORMAT_B8G8R8A8_UNORM; // ToDo: check if valid (civ)
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
		imageViewCreateInfo.format = VK_FORMAT_B8G8R8A8_UNORM; // ToDo: check if valid (civ);
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

	createShaderModule("vert.spv", &vertexShaderModule);
	createShaderModule("frag.spv", &fragmentShaderModule);

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

	vertexInputCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	vertexInputCreateInfo.pNext = NULL;
	vertexInputCreateInfo.flags = 0;
	vertexInputCreateInfo.vertexBindingDescriptionCount = 0;
	vertexInputCreateInfo.pVertexBindingDescriptions = NULL;
	vertexInputCreateInfo.vertexAttributeDescriptionCount = 0;
	vertexInputCreateInfo.pVertexAttributeDescriptions = NULL;

	free(pSwapchainImages);	
	free(pPhysicalDevices);
}

void mainLoop()
{
	while (!glfwWindowShouldClose(pWindow))
	{
		glfwPollEvents();
	}
}

void shutdownVulkan()
{
	vkDeviceWaitIdle(device);

	for (uint32_t i = 0; i < imagesInSwapChainCount; i++)
	{
		vkDestroyImageView(device, pImageViews[i], NULL);
	}
	free(pImageViews);
	vkDestroyShaderModule(device, vertexShaderModule, NULL);
	vkDestroyShaderModule(device, fragmentShaderModule, NULL);
	vkDestroySwapchainKHR(device, swapchain, NULL);
	vkDestroyDevice(device, NULL);
	vkDestroySurfaceKHR(instance, surface, NULL);
	vkDestroyInstance(instance, NULL);
}

void shutdownGLFW()
{
	glfwDestroyWindow(pWindow);
}

int main(int argc, char *argv[])
{
	printf("=======================\n");
	printf("***** %s *****\n", appName);
	printf("=======================\n\n");

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