#pragma once
#define VK_USE_PLATFORM_WIN32_KHR
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>

#include <algorithm>
#include <vector>
#include <iostream>
#include <fstream>
#include <cassert>


// TODO: builder design for some of these init structs?
class VulkanEngine {
public:
	void init_vulkan();
	void shutdown_vulkan();

	GLFWwindow* _window;
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
	VkSurfaceFormatKHR chooseSwapSurfaceFormat();
	VkPresentModeKHR chooseSwapPresentMode();
	VkExtent2D chooseSwapExtent();
	void create_swapchain();
	void createImageViews();
	static std::vector<char> readFile(const std::string& filename);
	VkShaderModule createShaderModule(const std::vector<char>& code);
	void createGraphicsPipeline();


	uint32_t findQueueIndex(VkPhysicalDevice device, uint32_t flag);

	unsigned int graphicsFamilyIndex;
	// swapchain
	VkSwapchainKHR _swapChain;
	VkSurfaceFormatKHR _swapChainImageFormat;
	VkExtent2D _swapChainExtent;
	VkPresentModeKHR _swapChainPresentMode;
	std::vector<VkImage> _swapChainImages;
	std::vector<VkImageView> _swapChainImageViews;

	const std::vector<const char*> deviceExtensions = {
		VK_KHR_SWAPCHAIN_EXTENSION_NAME
	};
};