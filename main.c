/*
	MyVulkanApp

	Date: 23.03.2017
*/

#include <stdio.h>
#include <stdlib.h>
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

int main(int argc, char *argv[])
{
	VkApplicationInfo appInfo;
	VkInstanceCreateInfo instanceInfo;
	VkInstance instance;
	VkResult result;

	printf("=======================\n");
	printf("***** %s *****\n", appName);
	printf("=======================\n\n");

	appInfo.sType				= VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pNext				= NULL;
	appInfo.pApplicationName	= appName;
	appInfo.applicationVersion	= VK_MAKE_VERSION(0, 0, 0);
	appInfo.pEngineName			= engineName;
	appInfo.engineVersion		= VK_MAKE_VERSION(0, 0, 0);
	appInfo.apiVersion			= 0;

	instanceInfo.sType						= VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	instanceInfo.pNext						= NULL;
	instanceInfo.flags						= 0;
	instanceInfo.pApplicationInfo			= &appInfo;
	instanceInfo.enabledLayerCount			= 0;
	instanceInfo.ppEnabledLayerNames		= NULL;
	instanceInfo.enabledExtensionCount		= 0;
	instanceInfo.ppEnabledExtensionNames	= NULL;

	result = vkCreateInstance(&instanceInfo, NULL, &instance);
	assert(result, "vkCreateInstance failed!");

	return 0;
}