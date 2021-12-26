#include "vk_engine.h"

void VulkanEngine::init_vulkan()
{

	if (!glfwInit())
	{
		std::cerr << "couldn't init glfw" << std::endl;
	}
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

	 _window = glfwCreateWindow(640, 480, "Vulkan!", NULL, NULL);
	if(!_window)
	{
		std::cerr << "couldn't init glfw window" << std::endl;
		glfwTerminate();
	}

	const std::vector<const char*> validationLayers = {
		"VK_LAYER_KHRONOS_validation"
	};

	VkApplicationInfo appInfo = {};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName = "Vulkan Testing";
	appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.pEngineName = "Brink Engine";
	appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.apiVersion = VK_API_VERSION_1_2;

	VkInstanceCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	// Synchronization validation, resource extensive. Run by itself without other validation is the recommended way.	
	// VkValidationFeatureEnableEXT enables[] = {VK_VALIDATION_FEATURE_ENABLE_SYNCHRONIZATION_VALIDATION_EXT};
	// VkValidationFeaturesEXT features = {};
	// features.sType = VK_STRUCTURE_TYPE_VALIDATION_FEATURES_EXT;
	// features.enabledValidationFeatureCount = 1;
	// features.pEnabledValidationFeatures = enables;
	// createInfo.pNext = &features;

	createInfo.pApplicationInfo = &appInfo;
	
	uint32_t glfwExtensionCount = 0;
	const char** glfwExtensions;

	glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

	createInfo.enabledExtensionCount = glfwExtensionCount;
	createInfo.ppEnabledExtensionNames = glfwExtensions;
	createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
	createInfo.ppEnabledLayerNames = validationLayers.data();

	if (vkCreateInstance(&createInfo, NULL, &_instance) != VK_SUCCESS)
	{
		std::cerr << "failed to create instance!" << std::endl;
	}
	else {
		std::cout << "created instance" << std::endl;
	}
	find_physical_device();
	create_device();
	create_commandbuffer();
	create_swapchain();
	createGraphicsPipeline();
}

// TODO: Pick from different devices instead of just picking the first one
void VulkanEngine::find_physical_device()
{
	// Create surface
	VkResult err =  glfwCreateWindowSurface(_instance, _window, nullptr, &_surface);
	if(err)
	{
		std::cerr << "Failed to create window surface";
	}
	//  FIND PHYSICAL
	uint32_t deviceCount;
	VkResult result = vkEnumeratePhysicalDevices(_instance, &deviceCount, nullptr);

	assert(result == VK_SUCCESS);
	assert(deviceCount >= 1);
	std::cout << deviceCount << " device(s) found!" << std::endl;
	
	std::vector<VkPhysicalDevice> physicalDevices(deviceCount);
	result = vkEnumeratePhysicalDevices(_instance, &deviceCount, physicalDevices.data());
	assert(result == VK_SUCCESS);
	for(const auto& physicalDevice: physicalDevices)
	{
		_chosenGPU = physicalDevice;
		VkBool32 presentSupport = false;
		uint32_t graphicsQueueFamilyIndex = findQueueIndex(physicalDevice, VK_QUEUE_GRAPHICS_BIT);
		vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, graphicsQueueFamilyIndex, _surface, &presentSupport);
		if(presentSupport)
		{
			std::cout << "Found physical device!" << std::endl;
			graphicsFamilyIndex = graphicsQueueFamilyIndex;
			return;
		}
	}

	_chosenGPU = physicalDevices[0];
}

uint32_t VulkanEngine::findQueueIndex(VkPhysicalDevice device, uint32_t flag)
{
	uint32_t queuefamilyCount;
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queuefamilyCount, nullptr);

	std::vector<VkQueueFamilyProperties> queueFamilyProperties(queuefamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queuefamilyCount, queueFamilyProperties.data());

	for(uint32_t i = 0; i < queuefamilyCount; i++)
	{
		// VK_QUEUE_GRAPHICS_BIT
		// VK_QUEUE_COMPUTE_BIT
		// VK_QUEUE_TRANSFER_BIT
		// VK_QUEUE_SPARSE_BINDING_BIT
		if(queueFamilyProperties[i].queueFlags & flag)
		{
			return i;
		}
	}

	std::cout << "Failed to find queue!" << std::endl;
	return 0;
}

void VulkanEngine::create_device()
{
	// CREATE QUEUE
	VkPhysicalDeviceProperties physicalProperties = {};
	vkGetPhysicalDeviceProperties(_chosenGPU, &physicalProperties);

	float priorities[] = { 1.0f };
	VkDeviceQueueCreateInfo queueCreateInfo = {};
	queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	queueCreateInfo.pNext = nullptr;
	queueCreateInfo.flags = 0;

	queueCreateInfo.queueFamilyIndex = graphicsFamilyIndex;
	queueCreateInfo.queueCount = 1;
	queueCreateInfo.pQueuePriorities = &priorities[0];

	// CREATE LOGICAL DEVICE
	VkDeviceCreateInfo device_info = {};
	device_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	device_info.pNext = NULL;
	device_info.queueCreateInfoCount = 1;
	device_info.pQueueCreateInfos = &queueCreateInfo;
	device_info.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());;
	device_info.ppEnabledExtensionNames = deviceExtensions.data();
	device_info.enabledLayerCount = 0;
	device_info.ppEnabledLayerNames = NULL;
	device_info.pEnabledFeatures = NULL;

	VkResult res = vkCreateDevice(_chosenGPU, &device_info, NULL, &_device);
	assert(res == VK_SUCCESS);
}

void VulkanEngine::create_commandbuffer()
{
	VkCommandPoolCreateInfo cmd_pool_info = {};
	cmd_pool_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	cmd_pool_info.pNext = nullptr;
	cmd_pool_info.queueFamilyIndex = graphicsFamilyIndex;
	cmd_pool_info.flags = 0;
	VkResult result = vkCreateCommandPool(_device, &cmd_pool_info, nullptr, &_pool);
	assert(result == VK_SUCCESS);

	VkCommandBufferAllocateInfo cmd = {};
	cmd.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	cmd.pNext = nullptr;
	cmd.commandPool = _pool;
	cmd.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	cmd.commandBufferCount = 1;
	result = vkAllocateCommandBuffers(_device, &cmd, &_commandBuffer);
	assert(result == VK_SUCCESS);
}

VkSurfaceFormatKHR VulkanEngine::chooseSwapSurfaceFormat()
{
	uint32_t formatCount;
	vkGetPhysicalDeviceSurfaceFormatsKHR(_chosenGPU, _surface, &formatCount, nullptr);
	
	std::vector<VkSurfaceFormatKHR> availableFormats (formatCount);
	vkGetPhysicalDeviceSurfaceFormatsKHR(_chosenGPU, _surface, &formatCount, availableFormats.data());

	for (const auto& availableFormat : availableFormats) 
	{
		if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
		{
			return availableFormat;
		}
	}
	return availableFormats[0];
}

VkPresentModeKHR VulkanEngine::chooseSwapPresentMode()
{
	uint32_t presentModeCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(_chosenGPU, _surface, &presentModeCount, nullptr);

    std::vector<VkPresentModeKHR> availablePresentModes(presentModeCount);
    vkGetPhysicalDeviceSurfacePresentModesKHR(_chosenGPU, _surface, &presentModeCount, availablePresentModes.data());

	for (const auto& availablePresentMode : availablePresentModes) 
	{
        if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) 
        {
 	       return availablePresentMode;
        }
    }
	return VK_PRESENT_MODE_FIFO_KHR;	
}

VkExtent2D VulkanEngine::chooseSwapExtent()
{
    VkSurfaceCapabilitiesKHR capabilities;
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(_chosenGPU, _surface, &capabilities);
	
	if (capabilities.currentExtent.width != UINT32_MAX) 
	{
    	return capabilities.currentExtent;
	} 
	else 
	{
	    int width, height;
	    glfwGetFramebufferSize(_window, &width, &height);

	    VkExtent2D actualExtent = {
	        static_cast<uint32_t>(width),
	        static_cast<uint32_t>(height)
	    };

	    actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
	    actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

	    return actualExtent;
	}
}


void VulkanEngine::create_swapchain()
{
    VkSurfaceCapabilitiesKHR capabilities;
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(_chosenGPU, _surface, &capabilities);
	
	_swapChainImageFormat = chooseSwapSurfaceFormat();
	_swapChainExtent = chooseSwapExtent();
	_swapChainPresentMode = chooseSwapPresentMode();

	uint32_t imageCount = capabilities.minImageCount + 1;
	// 0 means there is no maximum
	if(capabilities.maxImageCount > 0 && imageCount > capabilities.maxImageCount)
		imageCount = capabilities.maxImageCount;
	
	VkSwapchainCreateInfoKHR createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	createInfo.surface = _surface;
	createInfo.minImageCount = imageCount;
	createInfo.imageFormat = _swapChainImageFormat.format;
	createInfo.imageColorSpace = _swapChainImageFormat.colorSpace;
	createInfo.imageExtent = _swapChainExtent;
	createInfo.imageArrayLayers = 1;
	createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

	// this next im just assume graphics queue and present queue use the same index, change this.
	createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
	// transform to be applied to images in the swap chain, currentTransform = no transform
	createInfo.preTransform = capabilities.currentTransform;
	// should alpha channel be used for blending with other windows, we choose to ignore it
	createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	
	createInfo.presentMode = _swapChainPresentMode;
	// clipped means we don't care about the color of pixels that are obsecured, like another window infront of them.
	// better performance by clipping unless we need to read those pixels?
	createInfo.clipped = VK_TRUE;
	createInfo.oldSwapchain = VK_NULL_HANDLE;

	if (vkCreateSwapchainKHR(_device, &createInfo, nullptr, &_swapChain) != VK_SUCCESS)
	{
		std::cerr << "unable to create swap chain!!";
	}

	vkGetSwapchainImagesKHR(_device, _swapChain, &imageCount, nullptr);
	_swapChainImages.resize(imageCount);
	vkGetSwapchainImagesKHR(_device, _swapChain, &imageCount, _swapChainImages.data());
	std::cout << "Using " << imageCount << " images for the swapchain!" << std::endl;
	createImageViews();
}	

void VulkanEngine::createImageViews()
{
	_swapChainImageViews.resize(_swapChainImages.size());

	for(size_t i = 0; i < _swapChainImages.size(); i++)
	{
		VkImageViewCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		createInfo.image = _swapChainImages[i];
		createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		createInfo.format = _swapChainImageFormat.format;
		createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		createInfo.subresourceRange.baseMipLevel = 0;
		createInfo.subresourceRange.levelCount = 1;
		createInfo.subresourceRange.baseArrayLayer = 0;
		createInfo.subresourceRange.layerCount = 1;

		if( vkCreateImageView(_device, &createInfo, nullptr, &_swapChainImageViews[i]) != VK_SUCCESS)
		{
			std::cerr << "Failed to create swapchain image views!";
		}
	}
}

std::vector<char> VulkanEngine::readFile(const std::string& filename)
{
	// ate = start at EOF, binary = read as binary
	std::ifstream file(filename, std::ios::ate | std::ios::binary);
	if(!file.is_open())
	{
		std::cerr << "failed to open file!";
	}
	size_t fileSize = (size_t)file.tellg();
	
	std::vector<char> buffer(fileSize);
	file.seekg(0);
	file.read(buffer.data(), fileSize);
	file.close();

	return buffer;
}

VkShaderModule VulkanEngine::createShaderModule(const std::vector<char>& code)
{
	VkShaderModuleCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	createInfo.codeSize = code.size();
	createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

	VkShaderModule shaderModule;
	if(vkCreateShaderModule(_device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS)
	{
		std::cerr << "failed to create shader module";
	}
	return shaderModule;
}

void VulkanEngine::createGraphicsPipeline()
{
	auto vertShaderCode = readFile("resources/shaders/vert.spv");
	auto fragShaderCode = readFile("resources/shaders/frag.spv");

	VkShaderModule vertShaderModule = createShaderModule(vertShaderCode);
	VkShaderModule fragShaderModule = createShaderModule(fragShaderCode);

	VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
	vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
	vertShaderStageInfo.module = vertShaderModule;
	vertShaderStageInfo.pName = "main";

	VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
	fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	fragShaderStageInfo.module = fragShaderModule;
	fragShaderStageInfo.pName = "main";

	VkPipelineShaderStageCreateInfo shaderStages[] = {vertShaderStageInfo,  fragShaderStageInfo};
	// fixed function stuff now
	

	vkDestroyShaderModule(_device, fragShaderModule, nullptr);
	vkDestroyShaderModule(_device, vertShaderModule, nullptr);
}

void VulkanEngine::shutdown_vulkan()
{
	for(auto imageView : _swapChainImageViews)
	{
		vkDestroyImageView(_device, imageView, nullptr);
	}
	vkDestroySwapchainKHR(_device, _swapChain, nullptr);
	vkDestroyCommandPool(_device, _pool, nullptr);
	vkDestroyDevice(_device, nullptr);
	glfwDestroyWindow(_window);
	glfwTerminate();
}