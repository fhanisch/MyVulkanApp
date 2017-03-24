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
	
	vkGetPhysicalDeviceProperties(device, &properties);
	vkGetPhysicalDeviceFeatures(device, &features);

	printf("ApiVersion:          %u.%u.%u\n", VK_VERSION_MAJOR(properties.apiVersion), VK_VERSION_MINOR(properties.apiVersion), VK_VERSION_PATCH(properties.apiVersion));
	printf("DriverVersion:       %u\n", properties.driverVersion);
	printf("VendorID:            %u\n", properties.vendorID);
	printf("DeviceID:            %u\n", properties.deviceID);
	printf("DeviceType:          %u\n", properties.deviceType);
	printf("DeviceName:          %s\n", properties.deviceName);
	//printf("%u\n", properties.pipelineCacheUUID);
	//properties.limits
	//properties.sparseProperties
	printf("Geometry Shader:     %d\n", features.geometryShader);
	printf("Tessellation Shader: %d\n", features.tessellationShader);
	printf("\n");
}

int main(int argc, char *argv[])
{
	VkApplicationInfo appInfo;
	VkInstanceCreateInfo instanceInfo;
	VkInstance instance;
	VkResult result;
	uint32_t physicalDeviceCount;
	VkPhysicalDevice *pPhysicalDevice;

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


	return 0;
}