#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <vector>
#include <iostream>
#include <cassert>

// TODO: builder design for some of these init structs?
class VulkanEngine {
public:
	void init_vulkan();

	VkInstance _instance;
	VkDebugUtilsMessengerEXT _debug_messenger;
	VkPhysicalDevice _chosenGPU;
	VkDevice _device;
	VkSurfaceKHR _surface;
	VkCommandPool _pool;
	VkCommandBuffer _commandBuffer;

private:
	void find_physical_device();
	void create_device();
	void create_commandbuffer();

	unsigned int graphicsFamilyIndex;
};