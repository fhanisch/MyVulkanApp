#ifndef MY_VULKAN_ENGINE_H
#define MY_VULKAN_ENGINE_H

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
VkPipeline pipeline1, pipeline2, pipeline3;
VkCommandPool commandPool;
VkCommandBuffer *pCommandBuffers;
VkSemaphore semaphoreImageAvailable;
VkSemaphore semaphoreRenderingDone;
VkQueue queue;
VkBuffer vertexBuffer;
VkBuffer indexBuffer;
VkBuffer uniformBuffer, uniformBuffer2, uniformBuffer3;
VkBuffer uniformStagingBuffer;
VkDeviceMemory vertexBufferDeviceMemory;
VkDeviceMemory indexBufferDeviceMemory;
VkDeviceMemory uniformBufferDeviceMemory, uniformBufferDeviceMemory2, uniformBufferDeviceMemory3;
VkDeviceMemory uniformStagingBufferMemory;
VkDescriptorPool descriptorPool;
VkDescriptorSet descriptorSet, descriptorSet2, descriptorSet3;

#endif // !MY_VULKAN_ENGINE_H

