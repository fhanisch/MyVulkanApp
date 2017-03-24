/*
	MyVulkanApp

	Date: 23.03.2017
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vulkan\vulkan.h>

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
		printf("Queue Family Number # %u\n", i);
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
	VkInstanceCreateInfo instanceInfo;
	VkInstance instance;
	VkResult result;
	uint32_t physicalDeviceCount;
	VkPhysicalDevice *pPhysicalDevice;
	VkDeviceQueueCreateInfo deviceQueueCreateInfo;
	VkDeviceCreateInfo deviceCreateInfo;
	VkPhysicalDeviceFeatures usedFeatures;
	VkDevice device;

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

	instanceInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	instanceInfo.pNext = NULL;
	instanceInfo.flags = 0;
	instanceInfo.pApplicationInfo = &appInfo;
	instanceInfo.enabledLayerCount = 0;
	instanceInfo.ppEnabledLayerNames = NULL;
	instanceInfo.enabledExtensionCount = 0;
	instanceInfo.ppEnabledExtensionNames = NULL;

	result = vkCreateInstance(&instanceInfo, NULL, &instance);
	assert(result, "vkCreateInstance failed!");

	result = vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, NULL);
	assert(result, "vkEnumeratePhysicalDevices failed!");
	printf("Anzahl physiaklische Grafikkarten: %u\n\n", physicalDeviceCount);

	pPhysicalDevice = malloc(sizeof(VkPhysicalDevice) * physicalDeviceCount);
	result = vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, pPhysicalDevice);
	assert(result, "vkEnumeratePhysicalDevices failed!");

	for (uint32_t i=0; i<physicalDeviceCount; i++)
		printStats(pPhysicalDevice[i]);

	deviceQueueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	deviceQueueCreateInfo.pNext = NULL;
	deviceQueueCreateInfo.flags = 0;
	deviceQueueCreateInfo.queueFamilyIndex = 0;
	deviceQueueCreateInfo.queueCount = 4;
	deviceQueueCreateInfo.pQueuePriorities = NULL;

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

	result = vkCreateDevice(pPhysicalDevice[0], &deviceCreateInfo, NULL, &device);
	assert(result, "vkCreateDevice failed!");

	return 0;
}