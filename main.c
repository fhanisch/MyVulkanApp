/*
	MyVulkanApp

	Date: 23.03.2017
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vulkan\vulkan.h>

VkInstance instance;
VkDevice device;

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

void printStats(VkPhysicalDevice device)
{
	VkPhysicalDeviceProperties properties;
	VkPhysicalDeviceFeatures features;
	VkPhysicalDeviceMemoryProperties memProp;
	uint32_t queueFamilyCount;
	VkQueueFamilyProperties *pQueueFamilyProperties;
	uint32_t width, height, depth;
	
	vkGetPhysicalDeviceProperties(device, &properties);
	vkGetPhysicalDeviceFeatures(device, &features);
	vkGetPhysicalDeviceMemoryProperties(device, &memProp);
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, NULL);
	pQueueFamilyProperties = malloc(sizeof(VkQueueFamilyProperties) * queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, pQueueFamilyProperties);

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

	free(pQueueFamilyProperties);
}

int main(int argc, char *argv[])
{
	VkApplicationInfo appInfo;
	uint32_t layersCount;
	VkLayerProperties *pLayers;
	const char *ppValidationLayer[1];
	uint32_t extensionsCount;
	VkExtensionProperties *pExtensions;
	VkInstanceCreateInfo instanceInfo;	
	VkResult result;
	uint32_t physicalDeviceCount;
	VkPhysicalDevice *pPhysicalDevices;
	VkDeviceQueueCreateInfo deviceQueueCreateInfo;
	VkDeviceCreateInfo deviceCreateInfo;
	VkPhysicalDeviceFeatures usedFeatures;
	float queuePrios[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	VkQueue queue;
	
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

	instanceInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	instanceInfo.pNext = NULL;
	instanceInfo.flags = 0;
	instanceInfo.pApplicationInfo = &appInfo;
	instanceInfo.enabledLayerCount = 1;
	instanceInfo.ppEnabledLayerNames = ppValidationLayer;
	instanceInfo.enabledExtensionCount = 0;
	instanceInfo.ppEnabledExtensionNames = NULL;

	result = vkCreateInstance(&instanceInfo, NULL, &instance);
	assert(result, "vkCreateInstance failed!");

	result = vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, NULL);
	assert(result, "vkEnumeratePhysicalDevices failed!");
	printf("Anzahl physiaklische Grafikkarten: %u\n\n", physicalDeviceCount);

	pPhysicalDevices = malloc(sizeof(VkPhysicalDevice) * physicalDeviceCount);
	result = vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, pPhysicalDevices);
	assert(result, "vkEnumeratePhysicalDevices failed!");

	for (uint32_t i=0; i<physicalDeviceCount; i++)
		printStats(pPhysicalDevices[i]);

	deviceQueueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	deviceQueueCreateInfo.pNext = NULL;
	deviceQueueCreateInfo.flags = 0;
	deviceQueueCreateInfo.queueFamilyIndex = 0;
	deviceQueueCreateInfo.queueCount = 1;
	deviceQueueCreateInfo.pQueuePriorities = queuePrios;

	memset(&usedFeatures, 0, sizeof(VkPhysicalDeviceFeatures));

	deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	deviceCreateInfo.pNext = NULL;
	deviceCreateInfo.flags = 0;
	deviceCreateInfo.queueCreateInfoCount = 1;
	deviceCreateInfo.pQueueCreateInfos = &deviceQueueCreateInfo;
	deviceCreateInfo.enabledLayerCount = 0;
	deviceCreateInfo.ppEnabledLayerNames = NULL;
	deviceCreateInfo.enabledExtensionCount = 0;
	deviceCreateInfo.ppEnabledExtensionNames = NULL;
	deviceCreateInfo.pEnabledFeatures = &usedFeatures;

	result = vkCreateDevice(pPhysicalDevices[0], &deviceCreateInfo, NULL, &device);
	assert(result, "vkCreateDevice failed!");

	vkGetDeviceQueue(device, 0, 0, &queue);

	vkDeviceWaitIdle(device);
	vkDestroyDevice(device, NULL);
	vkDestroyInstance(instance, NULL);

	free(pLayers);
	free(pExtensions);
	free(pPhysicalDevices);

	return 0;
}