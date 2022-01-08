#pragma once
#define VK_USE_PLATFORM_WIN32_KHR
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>

#include <algorithm>
#include <array>
#include <vector>
#include <iostream>
#include <fstream>
#include <cassert>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>


#include <stb_truetype.h>

struct SpriteVertexData
{
	glm::vec2 position;
	glm::vec2 uv;
};

struct SpriteDrawData
{
	uint32_t textureIndex;
};

// TODO: builder design for some of these init structs?
class VulkanEngine {
public:
	void init_vulkan();
	void shutdown_vulkan();
	void drawFrame();
	void drawSprite(int x, int y, int width, int height);
	void drawText(float x, float y, char* text);

	GLFWwindow* _window;
	VkInstance _instance;
	VkDebugUtilsMessengerEXT _debug_messenger;
	VkPhysicalDevice _chosenGPU;
	VkQueue _presentQueue, _graphicsQueue;
	VkDevice _device;
	VkSurfaceKHR _surface;
	std::vector<VkCommandPool> _commandPools;
	std::vector<VkCommandBuffer> _commandBuffers;
	VkRenderPass _renderPass;
	VkDescriptorSetLayout _descriptorSetLayout;
	VkDescriptorPool _descriptorPool;
	std::vector<VkDescriptorSet> _descriptorSets;
	VkPipelineLayout _pipelineLayout;
	VkPipeline _graphicsPipeline;
	bool framebufferResized = false;

	const int _windowWidth = 800, _windowHeight = 600;

private:
	void cleanupSwapChain();
	void recreateSwapChain();
	void find_physical_device();
	void create_device();
	void createCommandPools();
	void createCommandBuffers();
	void updateCommandBuffer(uint32_t currentImage);
	VkSurfaceFormatKHR chooseSwapSurfaceFormat();
	VkPresentModeKHR chooseSwapPresentMode();
	VkExtent2D chooseSwapExtent();
	void createSwapChain();
	void createImageViews();
	static std::vector<char> readFile(const std::string& filename);
	VkShaderModule createShaderModule(const std::vector<char>& code);
	void createRenderPass();
	void createDescriptorSetLayout();
	void createGraphicsPipeline();
	void createFramebuffers();
	void createSyncObjects();
	VkCommandBuffer beginSingleTimeCommands();
	void endSingleTimeCommands(VkCommandBuffer commandBuffer);
	void copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);
	void transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);
	void createFontTexture();
	void createTextureImage();
	void createTextureImageViews();
	void createTextureSampler();
	VkImageView createImageView(VkImage image, VkFormat format);
	uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
	void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);
	void createIndexBuffer();
	void createBuffers();
	void createDescriptorPool();
	void createDescriptorSets();
	void updateBuffers(uint32_t currentImage);
	static void handleResize(GLFWwindow* window, int width, int height);

	const int MAX_FRAMES_IN_FLIGHT = 2;
	size_t currentFrame = 0;

	uint32_t findQueueIndex(VkPhysicalDevice device, uint32_t flag);

	unsigned int graphicsFamilyIndex;
	// swapchain
	VkSwapchainKHR _swapChain;
	VkSurfaceFormatKHR _swapChainImageFormat;
	VkExtent2D _swapChainExtent;
	VkPresentModeKHR _swapChainPresentMode;
	std::vector<VkImage> _swapChainImages;
	std::vector<VkImageView> _swapChainImageViews;
	std::vector<VkFramebuffer> _swapChainFramebuffers;

	std::vector<VkSemaphore> _imageAvailableSemaphores, _renderFinishedSemaphores;
	std::vector<VkFence> _inFlightFences;
	std::vector<VkFence> _imagesInFlight;

	std::vector<VkBuffer> _indexBuffers;
	std::vector<VkDeviceMemory>  _indexBuffersMemory;

	std::vector<VkBuffer> _vertexBuffers;
	std::vector<VkDeviceMemory> _vertexBuffersMemory;

	std::vector<VkImage> _textureImages;
	std::vector<VkDeviceMemory> _textureImagesMemory;
	std::vector<VkImageView> _textureImageViews;
	
	VkSampler _textureSampler;

	const std::vector<const char*> deviceExtensions = {
		VK_KHR_SWAPCHAIN_EXTENSION_NAME,
        VK_EXT_DESCRIPTOR_INDEXING_EXTENSION_NAME
	};

	std::vector<SpriteVertexData> _spriteVertices;

	std::vector<uint16_t> _indices;
	stbtt_bakedchar cdata[96]; // ASCII 32..126 is 95 glyphs

	const int MAX_SPRITES = 2048;
	const int MAX_INDICES = 2048;
	const int MAX_TEXTURES = 2048;

};