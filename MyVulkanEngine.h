#ifndef MY_VULKAN_ENGINE_H
#define MY_VULKAN_ENGINE_H

#include <vulkan\vulkan.h>
#include "renderobject.h"

//globale Variablen im gesamten Programm --> external linkage --> entspricht dem Schlüsselwort 'extern'
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
VkDeviceMemory vertexBufferDeviceMemory;
VkDeviceMemory indexBufferDeviceMemory;
VkDescriptorPool descriptorPool;
VkDescriptorSet descriptorSet, descriptorSet2;
VkImage depthImage;
VkDeviceMemory depthImageMemory;
VkImageView depthImageView;

//Functions
void assert(VkResult result, char *msg);
void createInstance(const char *appName, const char **glfwExtensions, uint32_t glfwExtensionsCount);
void getPhysicalDevices();
void createLogicalDevice();
void createSwapchain(uint32_t width, uint32_t height);
void createImageViews();
void createDescriptorSetLayout();
void createPipelineLayout();
void createRenderPass();
void createGraphicsPipeline(uint32_t width, uint32_t height, PipelineCreateInfo *pPipelineCreateInfo, VkPipeline *pPipeline);
void createShaderModule(char *filename, VkShaderModule *shaderModule);
int loadShader(char **shaderStr, char *fileName);
VkVertexInputBindingDescription getBindingDescription(uint32_t stride);
VkDescriptorSetLayoutBinding getDescriptorSetLayoutBinding(VkShaderStageFlags stageFlags);
VkVertexInputAttributeDescription getAttributeDescription(uint32_t location, uint32_t offset, VkFormat format);
uint32_t findMemoryTypeIndex(uint32_t typeFilter, VkMemoryPropertyFlags properties);
void printStats(VkPhysicalDevice physDevice);
void createDepthResources(uint32_t width, uint32_t height);
void createImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage *pImage, VkDeviceMemory *pImageMemory);
void createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, VkImageView *pImageView);
void transitionImageLayout(VkImage image, VkImageLayout oldLayout, VkImageLayout newLayout);
void shutdownVulkan();

#endif // !MY_VULKAN_ENGINE_H

