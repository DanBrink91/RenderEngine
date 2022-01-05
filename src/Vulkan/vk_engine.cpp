#include "vk_engine.h"

#define GLM_ENABLE_EXPERIMENTAL
#include "glm/ext.hpp"
#include <glm/gtc/matrix_transform.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#define STB_TRUETYPE_IMPLEMENTATION
#include <stb_truetype.h>

void VulkanEngine::init_vulkan()
{

	if (!glfwInit())
	{
		std::cerr << "couldn't init glfw" << std::endl;
	}
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

	_window = glfwCreateWindow(_windowWidth, _windowHeight, "Vulkan!", NULL, NULL);
	if(!_window)
	{
		std::cerr << "couldn't init glfw window" << std::endl;
		glfwTerminate();
	}
	glfwSetWindowUserPointer(_window, this);
	glfwSetFramebufferSizeCallback(_window, handleResize);

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

	drawSprite(200, 200, 512, 512);
	// drawSprite(100, 100, 64*3, 64*2);
	//createFontTexture();
	
	// stbtt_aligned_quad q;
	// float x, y;
	// const char* text = "Hello World!";

 // 	stbtt_GetBakedQuad(cdata, 512,512, *text, &x,&y,&q,1);//1=opengl & d3d10+,0=d3d9
 // 	std::cout << q.x0 << ", " << q.y0 << std::endl;
 	// glTexCoord2f(q.s0,q.t0); glVertex2f(q.x0,q.y0);
 	// glTexCoord2f(q.s1,q.t0); glVertex2f(q.x1,q.y0);
 	// glTexCoord2f(q.s1,q.t1); glVertex2f(q.x1,q.y1);
 	// glTexCoord2f(q.s0,q.t1); glVertex2f(q.x0,q.y1);

	find_physical_device();
	create_device();
	createSwapChain();
	createRenderPass();
	createDescriptorSetLayout();
	createIndexBuffer();
	createBuffers();
	createGraphicsPipeline();
	createFramebuffers();
	createCommandPool();
	createTextureImage();
	createTextureImageView();
	createTextureSampler();
	createDescriptorPool();
	createDescriptorSets();
	drawText(50, 50, "Hello World!");
	createCommandBuffers();
	createSyncObjects();
}

void VulkanEngine::drawSprite(int x, int y, int width, int height)
{
	float screenSpaceLeft = (float(x) / _windowWidth) * 2.0 - 1.0;
	float screenSpaceRight = (float(x + width) / _windowWidth) * 2.0 - 1.0;

	float screenSpaceTop = (float(y) / _windowHeight) * 2.0 - 1.0;
	float screenSpaceBottom = (float(y + height) / _windowHeight) * 2.0 - 1.0;


	int indexStart = _spriteVertices.size();

	_spriteVertices.push_back({{screenSpaceRight, screenSpaceTop}, {1.0, 0.0}});
	_spriteVertices.push_back({{screenSpaceRight , screenSpaceBottom}, {1.0, 1.0}});
	_spriteVertices.push_back({{screenSpaceLeft, screenSpaceBottom}, {0.0, 1.0}});
	_spriteVertices.push_back({{screenSpaceLeft, screenSpaceTop}, {0.0, 0.0}});
	
	_indices.push_back((uint16_t)indexStart);
	_indices.push_back((uint16_t)indexStart + 1);
	_indices.push_back((uint16_t)indexStart + 2);
	_indices.push_back((uint16_t)indexStart + 3);
	_indices.push_back((uint16_t)indexStart);
	_indices.push_back((uint16_t)indexStart + 2);
}

void VulkanEngine::drawText(float x, float y, char* text) 
{
	 while (*text) {
      if (*text >= 32 && *text < 128) {
         stbtt_aligned_quad q;
         stbtt_GetBakedQuad(cdata, 512,512, *text-32, &x,&y,&q,1);//1=opengl & d3d10+,0=d3d9
         std::cout << q.x0/512 << " ," << q.y0/512 << std::endl;
      //    glTexCoord2f(q.s0,q.t0); glVertex2f(q.x0,q.y0);
      //    glTexCoord2f(q.s1,q.t0); glVertex2f(q.x1,q.y0);
      //    glTexCoord2f(q.s1,q.t1); glVertex2f(q.x1,q.y1);
      //    glTexCoord2f(q.s0,q.t1); glVertex2f(q.x0,q.y1);
      }
      ++text;
   }
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
		uint32_t presentQueueFamilyIndex = findQueueIndex(physicalDevice, VK_QUEUE_GRAPHICS_BIT);

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
	VkPhysicalDeviceFeatures deviceFeatures{};
	deviceFeatures.samplerAnisotropy = VK_TRUE;

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
	device_info.pEnabledFeatures = &deviceFeatures;

	VkResult res = vkCreateDevice(_chosenGPU, &device_info, NULL, &_device);
	assert(res == VK_SUCCESS);

	vkGetDeviceQueue(_device, graphicsFamilyIndex, 0, &_presentQueue);
	vkGetDeviceQueue(_device, graphicsFamilyIndex, 0, &_graphicsQueue);
}

void VulkanEngine::createCommandPool()
{
	VkCommandPoolCreateInfo cmd_pool_info = {};
	cmd_pool_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	cmd_pool_info.pNext = nullptr;
	cmd_pool_info.queueFamilyIndex = graphicsFamilyIndex;
	cmd_pool_info.flags = 0;
	VkResult result = vkCreateCommandPool(_device, &cmd_pool_info, nullptr, &_pool);
	assert(result == VK_SUCCESS);
}

void VulkanEngine::createCommandBuffers()
{
	_commandBuffers.resize(_swapChainFramebuffers.size());
	VkCommandBufferAllocateInfo cmd = {};
	cmd.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	cmd.pNext = nullptr;
	cmd.commandPool = _pool;
	cmd.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	cmd.commandBufferCount = (uint32_t)_commandBuffers.size();
	VkResult result = vkAllocateCommandBuffers(_device, &cmd, _commandBuffers.data());
	assert(result == VK_SUCCESS);

	// Do some drawing stuff??
	for (size_t i = 0; i < _commandBuffers.size(); i++) {
	    VkCommandBufferBeginInfo beginInfo{};
	    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	    beginInfo.flags = 0; // Optional
	    beginInfo.pInheritanceInfo = nullptr; // Optional

	    if (vkBeginCommandBuffer(_commandBuffers[i], &beginInfo) != VK_SUCCESS) {
	        throw std::runtime_error("failed to begin recording command buffer!");
	    }
	    VkRenderPassBeginInfo renderPassInfo{};
	    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	    renderPassInfo.renderPass = _renderPass;
	    renderPassInfo.framebuffer = _swapChainFramebuffers[i];
	    renderPassInfo.renderArea.offset = {0, 0};
	    renderPassInfo.renderArea.extent = _swapChainExtent;

	    VkClearValue clearColor = {{{0.0f, 0.0f, 0.0f, 1.0f}}};
	    renderPassInfo.clearValueCount = 1;
	    renderPassInfo.pClearValues = &clearColor;
	    vkCmdBeginRenderPass(_commandBuffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
	    	vkCmdBindPipeline(_commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, _graphicsPipeline);
		  		vkCmdBindIndexBuffer(_commandBuffers[i], _indexBuffer, 0, VK_INDEX_TYPE_UINT16);
	    		vkCmdBindDescriptorSets(_commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, _pipelineLayout, 0, 1, &_descriptorSets[i], 0, nullptr);
	    			vkCmdDrawIndexed(_commandBuffers[i], static_cast<uint32_t>(_indices.size()), 1, 0, 0, 0);
	    			//vkCmdDraw(_commandBuffers[i], 3, 1, 0, 0);
	    vkCmdEndRenderPass(_commandBuffers[i]);

	    if (vkEndCommandBuffer(_commandBuffers[i]) != VK_SUCCESS) {
	        throw std::runtime_error("failed to record command buffer!");
	    }
	}
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


void VulkanEngine::createSwapChain()
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
		_swapChainImageViews[i] = createImageView(_swapChainImages[i], _swapChainImageFormat.format);
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

void VulkanEngine::createRenderPass()
{
	VkAttachmentDescription colorAttachment{};
   	colorAttachment.format = _swapChainImageFormat.format;
   	colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
   	colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
   	colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
   	colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
   	colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
   	colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
   	colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

   	VkAttachmentReference colorAttachmentRef{};
   	colorAttachmentRef.attachment = 0;
   	colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

   	VkSubpassDescription subpass{};
   	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
   	subpass.colorAttachmentCount = 1;
   	subpass.pColorAttachments = &colorAttachmentRef;

   	VkSubpassDependency dependency{};
   	dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
   	dependency.dstSubpass = 0;
   	dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
   	dependency.srcAccessMask = 0;
   	dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
   	dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

   	VkRenderPassCreateInfo renderPassInfo{};
   	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
   	renderPassInfo.attachmentCount = 1;
   	renderPassInfo.pAttachments = &colorAttachment;
   	renderPassInfo.subpassCount = 1;
   	renderPassInfo.pSubpasses = &subpass;
   	renderPassInfo.dependencyCount = 1;
   	renderPassInfo.pDependencies = &dependency;

   	if (vkCreateRenderPass(_device, &renderPassInfo, nullptr, &_renderPass) != VK_SUCCESS) {
   	    throw std::runtime_error("failed to create render pass!");
   	}
}

void VulkanEngine::createDescriptorSetLayout()
{
	VkDescriptorSetLayoutBinding vertexUvLayoutBinding{};
   	vertexUvLayoutBinding.binding = 0;
   	vertexUvLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
   	vertexUvLayoutBinding.descriptorCount = 1;
   	vertexUvLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    vertexUvLayoutBinding.pImmutableSamplers = nullptr;
    vertexUvLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

	VkDescriptorSetLayoutBinding samplerLayoutBinding{};
	samplerLayoutBinding.binding = 1;
	samplerLayoutBinding.descriptorCount = 1;
	samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	samplerLayoutBinding.pImmutableSamplers = nullptr;
	samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;    

	std::array<VkDescriptorSetLayoutBinding, 2> bindings = {vertexUvLayoutBinding, samplerLayoutBinding};


   	VkDescriptorSetLayoutCreateInfo layoutInfo{};
   	layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
   	layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
   	layoutInfo.pBindings = bindings.data();

   	if (vkCreateDescriptorSetLayout(_device, &layoutInfo, nullptr, &_descriptorSetLayout) != VK_SUCCESS) {
   	    throw std::runtime_error("failed to create descriptor set layout!");
   	}
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
	
	VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
	vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	vertexInputInfo.vertexBindingDescriptionCount = 0;
	vertexInputInfo.pVertexBindingDescriptions = nullptr; // Optional
	vertexInputInfo.vertexAttributeDescriptionCount = 0;
	vertexInputInfo.pVertexAttributeDescriptions = nullptr; // Optional

	VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
	inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	inputAssembly.primitiveRestartEnable = VK_FALSE;

	VkViewport viewport{};
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = (float) _swapChainExtent.width;
	viewport.height = (float) _swapChainExtent.height;
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;

	VkRect2D scissor{};
	scissor.offset = {0, 0};
	scissor.extent = _swapChainExtent;

	VkPipelineViewportStateCreateInfo viewportState{};
	viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewportState.viewportCount = 1;
	viewportState.pViewports = &viewport;
	viewportState.scissorCount = 1;
	viewportState.pScissors = &scissor;

	VkPipelineRasterizationStateCreateInfo rasterizer{};
	rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rasterizer.depthClampEnable = VK_FALSE;
	rasterizer.rasterizerDiscardEnable = VK_FALSE;
	rasterizer.polygonMode = VK_POLYGON_MODE_FILL; // could draw as lines or points here for debugging
	rasterizer.lineWidth = 1.0f;
	rasterizer.cullMode = VK_CULL_MODE_NONE;
	rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
	rasterizer.depthBiasEnable = VK_FALSE;
	rasterizer.depthBiasConstantFactor = 0.0f; // Optional
	rasterizer.depthBiasClamp = 0.0f; // Optional
	rasterizer.depthBiasSlopeFactor = 0.0f; // Optional

	VkPipelineMultisampleStateCreateInfo multisampling{};
	multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multisampling.sampleShadingEnable = VK_FALSE;
	multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
	multisampling.minSampleShading = 1.0f; // Optional
	multisampling.pSampleMask = nullptr; // Optional
	multisampling.alphaToCoverageEnable = VK_FALSE; // Optional
	multisampling.alphaToOneEnable = VK_FALSE; // Optional

	VkPipelineColorBlendAttachmentState colorBlendAttachment{};
	colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
	colorBlendAttachment.blendEnable = VK_FALSE;
	colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
	colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
	colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD; // Optional
	colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
	colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
	colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD; // Optional

	VkPipelineColorBlendStateCreateInfo colorBlending{};
	colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	colorBlending.logicOpEnable = VK_FALSE;
	colorBlending.logicOp = VK_LOGIC_OP_COPY; // Optional
	colorBlending.attachmentCount = 1;
	colorBlending.pAttachments = &colorBlendAttachment;
	colorBlending.blendConstants[0] = 0.0f; // Optional
	colorBlending.blendConstants[1] = 0.0f; // Optional
	colorBlending.blendConstants[2] = 0.0f; // Optional
	colorBlending.blendConstants[3] = 0.0f; // Optional

	VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
	pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutInfo.setLayoutCount = 1;
	pipelineLayoutInfo.pSetLayouts = &_descriptorSetLayout;
	pipelineLayoutInfo.pushConstantRangeCount = 0; // Optional
	pipelineLayoutInfo.pPushConstantRanges = nullptr; // Optional

	if (vkCreatePipelineLayout(_device, &pipelineLayoutInfo, nullptr, &_pipelineLayout) != VK_SUCCESS) {
	    throw std::runtime_error("failed to create pipeline layout!");
	}

	VkGraphicsPipelineCreateInfo pipelineInfo{};
	pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipelineInfo.stageCount = 2;
	pipelineInfo.pStages = shaderStages;
	pipelineInfo.pVertexInputState = &vertexInputInfo;
	pipelineInfo.pInputAssemblyState = &inputAssembly;
	pipelineInfo.pViewportState = &viewportState;
	pipelineInfo.pRasterizationState = &rasterizer;
	pipelineInfo.pMultisampleState = &multisampling;
	pipelineInfo.pDepthStencilState = nullptr; // Optional
	pipelineInfo.pColorBlendState = &colorBlending;
	pipelineInfo.pDynamicState = nullptr; // Optional
	pipelineInfo.layout = _pipelineLayout;
	pipelineInfo.renderPass = _renderPass;
	pipelineInfo.subpass = 0;
	pipelineInfo.basePipelineHandle = VK_NULL_HANDLE; // Optional
	pipelineInfo.basePipelineIndex = -1; // Optional

	if (vkCreateGraphicsPipelines(_device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &_graphicsPipeline) != VK_SUCCESS) {
	    throw std::runtime_error("failed to create graphics pipeline!");
	}

	vkDestroyShaderModule(_device, fragShaderModule, nullptr);
	vkDestroyShaderModule(_device, vertShaderModule, nullptr);
}

void VulkanEngine::createFramebuffers()
{
	_swapChainFramebuffers.resize(_swapChainImageViews.size());
	for (size_t i = 0; i < _swapChainImageViews.size(); i++) {
	    VkImageView attachments[] = {
	        _swapChainImageViews[i]
	    };

	    VkFramebufferCreateInfo framebufferInfo{};
	    framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
	    framebufferInfo.renderPass = _renderPass;
	    framebufferInfo.attachmentCount = 1;
	    framebufferInfo.pAttachments = attachments;
	    framebufferInfo.width = _swapChainExtent.width;
	    framebufferInfo.height = _swapChainExtent.height;
	    framebufferInfo.layers = 1;

	    if (vkCreateFramebuffer(_device, &framebufferInfo, nullptr, &_swapChainFramebuffers[i]) != VK_SUCCESS) {
	        throw std::runtime_error("failed to create framebuffer!");
	    }
	}
}

void VulkanEngine::createSyncObjects()
{
	_imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
	_renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
	_inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);
	_imagesInFlight.resize(_swapChainImages.size(), VK_NULL_HANDLE);

	VkSemaphoreCreateInfo semaphoreInfo{};
	semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

 	VkFenceCreateInfo fenceInfo{};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;


	for(size_t i = 0; i  < MAX_FRAMES_IN_FLIGHT; i++)
	{
		if (vkCreateSemaphore(_device, &semaphoreInfo, nullptr, &_imageAvailableSemaphores[i]) != VK_SUCCESS ||
		    vkCreateSemaphore(_device, &semaphoreInfo, nullptr, &_renderFinishedSemaphores[i]) != VK_SUCCESS ||
		    vkCreateFence(_device, &fenceInfo, nullptr, &_inFlightFences[i]) != VK_SUCCESS) {

		    throw std::runtime_error("failed to create semaphores!");
		}
	}
}

void VulkanEngine::createFontTexture() 
{
	unsigned char temp_bitmap[512*512];
	auto ttf_buffer = readFile("resources/fonts/Roboto-Regular.ttf");
   	stbtt_BakeFontBitmap(reinterpret_cast<const unsigned char*>(ttf_buffer.data()),0, 16.0, temp_bitmap,512,512, 32,96, cdata); // no guarantee this fits!
   	//glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, 512,512, 0, GL_ALPHA, GL_UNSIGNED_BYTE, temp_bitmap);
}

void VulkanEngine::createTextureImage()
{
	unsigned char temp_bitmap[512*512];
	auto ttf_buffer = readFile("resources/fonts/Roboto-Regular.ttf");
   	auto res = stbtt_BakeFontBitmap(reinterpret_cast<const unsigned char*>(ttf_buffer.data()),0, 16.0, temp_bitmap,512,512, 32,96, cdata); // no guarantee this fits!
   	std::cout << sizeof(temp_bitmap) << std::endl;

	// int texWidth, texHeight, texChannels;
 //    stbi_uc* pixels = stbi_load("resources/sprites/diceRed.png", &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
    VkDeviceSize imageSize = 512 * 512; //texWidth * texHeight * 4;

    // if (!pixels) 
    //     throw std::runtime_error("failed to load texture image!");
    
    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;

    createBuffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);
    void* data;
	vkMapMemory(_device, stagingBufferMemory, 0, imageSize, 0, &data);
	    memcpy(data, temp_bitmap, static_cast<size_t>(imageSize));
	vkUnmapMemory(_device, stagingBufferMemory);
	// stbi_image_free(pixels);

	VkImageCreateInfo imageInfo{};
	imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	imageInfo.imageType = VK_IMAGE_TYPE_2D;
	imageInfo.extent.width = 512;//static_cast<uint32_t>(texWidth);
	imageInfo.extent.height = 512;//static_cast<uint32_t>(texHeight);
	imageInfo.extent.depth = 1;
	imageInfo.mipLevels = 1;
	imageInfo.arrayLayers = 1;
	imageInfo.format = VK_FORMAT_R8_SRGB;
	imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
	imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	imageInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
	imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
	imageInfo.flags = 0; // Optional

	if (vkCreateImage(_device, &imageInfo, nullptr, &_textureImage) != VK_SUCCESS) {
    	throw std::runtime_error("failed to create image!");
	}

	VkMemoryRequirements memRequirements;
	vkGetImageMemoryRequirements(_device, _textureImage, &memRequirements);

	VkMemoryAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;
	allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

	if (vkAllocateMemory(_device, &allocInfo, nullptr, &_textureImageMemory) != VK_SUCCESS) 
	{
	    throw std::runtime_error("failed to allocate image memory!");
	}

	vkBindImageMemory(_device, _textureImage, _textureImageMemory, 0);
	transitionImageLayout(_textureImage, VK_FORMAT_R8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
	copyBufferToImage(stagingBuffer, _textureImage, 512, 512);

	//copyBufferToImage(stagingBuffer, _textureImage, static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight));
	transitionImageLayout(_textureImage, VK_FORMAT_R8_SRGB, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

	vkDestroyBuffer(_device, stagingBuffer, nullptr);
    vkFreeMemory(_device, stagingBufferMemory, nullptr);
}

VkCommandBuffer VulkanEngine::beginSingleTimeCommands() 
{
    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool = _pool;
    allocInfo.commandBufferCount = 1;

    VkCommandBuffer commandBuffer;
    vkAllocateCommandBuffers(_device, &allocInfo, &commandBuffer);

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    vkBeginCommandBuffer(commandBuffer, &beginInfo);

    return commandBuffer;
}

void VulkanEngine::endSingleTimeCommands(VkCommandBuffer commandBuffer) 
{
    vkEndCommandBuffer(commandBuffer);

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;

    vkQueueSubmit(_graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(_graphicsQueue);

    vkFreeCommandBuffers(_device, _pool, 1, &commandBuffer);
}

void VulkanEngine::copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height) 
{
    VkCommandBuffer commandBuffer = beginSingleTimeCommands();

    VkBufferImageCopy region{};
    region.bufferOffset = 0;
    region.bufferRowLength = 0;
    region.bufferImageHeight = 0;

    region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    region.imageSubresource.mipLevel = 0;
    region.imageSubresource.baseArrayLayer = 0;
    region.imageSubresource.layerCount = 1;

    region.imageOffset = {0, 0, 0};
    region.imageExtent = {
        width,
        height,
        1
    };

    vkCmdCopyBufferToImage(
        commandBuffer,
        buffer,
        image,
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        1,
        &region
    );
    endSingleTimeCommands(commandBuffer);
}

void VulkanEngine::transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout) 
{
	VkCommandBuffer commandBuffer = beginSingleTimeCommands();

	VkImageMemoryBarrier barrier{};
	barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	barrier.oldLayout = oldLayout;
	barrier.newLayout = newLayout;
	barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.image = image;
	barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	barrier.subresourceRange.baseMipLevel = 0;
	barrier.subresourceRange.levelCount = 1;
	barrier.subresourceRange.baseArrayLayer = 0;
	barrier.subresourceRange.layerCount = 1;
	barrier.srcAccessMask = 0; // TODO
	barrier.dstAccessMask = 0; // TODO

	VkPipelineStageFlags sourceStage;
	VkPipelineStageFlags destinationStage;

	if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
	    barrier.srcAccessMask = 0;
	    barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

	    sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
	    destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
	} else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
	    barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
	    barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

	    sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
	    destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
	} else {
	    throw std::invalid_argument("unsupported layout transition!");
	}

	vkCmdPipelineBarrier(
	    commandBuffer,
	    sourceStage, destinationStage,
	    0,
	    0, nullptr,
	    0, nullptr,
	    1, &barrier
	);
	endSingleTimeCommands(commandBuffer);
}
void VulkanEngine::updateBuffers(uint32_t currentImage)
{
	void* data;
	vkMapMemory(_device, _vertexBuffersMemory[currentImage], 0, sizeof(SpriteVertexData), 0, &data);
	   memcpy(data, _spriteVertices.data(), sizeof(SpriteVertexData)  * _spriteVertices.size());
	vkUnmapMemory(_device, _vertexBuffersMemory[currentImage]);
}

void VulkanEngine::drawFrame()
{
	vkWaitForFences(_device, 1, &_inFlightFences[currentFrame], VK_TRUE, UINT64_MAX);

	uint32_t imageIndex;
	VkResult result = vkAcquireNextImageKHR(_device, _swapChain, UINT64_MAX, _imageAvailableSemaphores[currentFrame], VK_NULL_HANDLE, &imageIndex);


	if (result == VK_ERROR_OUT_OF_DATE_KHR) {
	    recreateSwapChain();
	    return;
	} else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
	   std::cout << "failed to acquire swap chain image!" << std::endl;;
	}
	// Check if a previous frame is using this image (i.e. there is its fence to wait on)
    if (_imagesInFlight[imageIndex] != VK_NULL_HANDLE) {
        vkWaitForFences(_device, 1, &_imagesInFlight[imageIndex], VK_TRUE, UINT64_MAX);
    }
    // Mark the image as now being in use by this frame
    _imagesInFlight[imageIndex] = _inFlightFences[currentFrame];

    updateBuffers(imageIndex);

	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

	VkSemaphore waitSemaphores[] = { _imageAvailableSemaphores[currentFrame] };
	VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = waitSemaphores;
	submitInfo.pWaitDstStageMask = waitStages;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &_commandBuffers[imageIndex];

	VkSemaphore signalSemaphores[] = { _renderFinishedSemaphores[currentFrame] };
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = signalSemaphores;

	vkResetFences(_device, 1, &_inFlightFences[currentFrame]);
	if (vkQueueSubmit(_graphicsQueue, 1, &submitInfo, _inFlightFences[currentFrame]) != VK_SUCCESS) {
	    throw std::runtime_error("failed to submit draw command buffer!");
	}

	VkPresentInfoKHR presentInfo{};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = signalSemaphores;	
	VkSwapchainKHR swapChains[] = {_swapChain};
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = swapChains;
	presentInfo.pImageIndices = &imageIndex;
	
	result = vkQueuePresentKHR(_presentQueue, &presentInfo);
	if (result == VK_ERROR_OUT_OF_DATE_KHR || framebufferResized) {
		framebufferResized = false;
	    recreateSwapChain();
	} else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
	   std::cout << "failed to acquire swap chain image!" << std::endl;;
	}
	currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}

VkImageView VulkanEngine::createImageView(VkImage image, VkFormat format) 
{
	VkImageViewCreateInfo viewInfo{};
	viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	viewInfo.image = image;
	viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
	viewInfo.format = format;
	viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	viewInfo.subresourceRange.baseMipLevel = 0;
	viewInfo.subresourceRange.levelCount = 1;
	viewInfo.subresourceRange.baseArrayLayer = 0;
	viewInfo.subresourceRange.layerCount = 1;

	VkImageView imageView;
	if (vkCreateImageView(_device, &viewInfo, nullptr, &imageView) != VK_SUCCESS) {
	    throw std::runtime_error("failed to create texture image view!");
	}

	return imageView;
}

void VulkanEngine::createTextureImageView() 
{
	_textureImageView = createImageView(_textureImage, VK_FORMAT_R8_SRGB);
}

void VulkanEngine::createTextureSampler() 
{
	VkPhysicalDeviceProperties properties{};
	vkGetPhysicalDeviceProperties(_chosenGPU, &properties);

	VkSamplerCreateInfo samplerInfo{};
	samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	samplerInfo.magFilter = VK_FILTER_LINEAR;
	samplerInfo.minFilter = VK_FILTER_LINEAR;
	samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerInfo.anisotropyEnable = VK_TRUE;
	samplerInfo.maxAnisotropy = properties.limits.maxSamplerAnisotropy;;
	samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
	samplerInfo.unnormalizedCoordinates = VK_FALSE;
	samplerInfo.compareEnable = VK_FALSE;
	samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
	samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
	samplerInfo.mipLodBias = 0.0f;
	samplerInfo.minLod = 0.0f;
	samplerInfo.maxLod = 0.0f;

	if (vkCreateSampler(_device, &samplerInfo, nullptr, &_textureSampler) != VK_SUCCESS) 
	{
	    throw std::runtime_error("failed to create texture sampler!");
	}
}

uint32_t VulkanEngine::findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) 
{
	VkPhysicalDeviceMemoryProperties memProperties;
	vkGetPhysicalDeviceMemoryProperties(_chosenGPU, &memProperties);

	for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
	    if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
	        return i;
	    }
	}

	throw std::runtime_error("failed to find suitable memory type!");
}

void VulkanEngine::createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory) 
{
	VkBufferCreateInfo bufferInfo{};
	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.size = size;
	bufferInfo.usage = usage;
	bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	if (vkCreateBuffer(_device, &bufferInfo, nullptr, &buffer) != VK_SUCCESS) {
	    throw std::runtime_error("failed to create buffer!");
	}

	VkMemoryRequirements memRequirements;
	vkGetBufferMemoryRequirements(_device, buffer, &memRequirements);

	VkMemoryAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;
	allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties);

	if (vkAllocateMemory(_device, &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS) {
	    throw std::runtime_error("failed to allocate buffer memory!");
	}

	vkBindBufferMemory(_device, buffer, bufferMemory, 0);
}
void VulkanEngine::createIndexBuffer()
{
	VkDeviceSize bufferSize = sizeof(uint16_t) * _indices.size();
	createBuffer(bufferSize, VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, _indexBuffer, _indexBufferMemory);

	void* data;
	vkMapMemory(_device, _indexBufferMemory, 0, sizeof(uint16_t) * _indices.size(), 0, &data);
	   memcpy(data, _indices.data(), sizeof(uint16_t) * _indices.size());
	vkUnmapMemory(_device, _indexBufferMemory);
}

void VulkanEngine::createBuffers()
{
	VkDeviceSize bufferSize = sizeof(SpriteVertexData) * _spriteVertices.size();

	_vertexBuffers.resize(_swapChainImages.size());
	_vertexBuffersMemory.resize(_swapChainImages.size());

	for (size_t i = 0; i < _swapChainImages.size(); i++) {
	   createBuffer(bufferSize, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, _vertexBuffers[i], _vertexBuffersMemory[i]);
	}
}

void VulkanEngine::createDescriptorPool()
{
	std::array<VkDescriptorPoolSize, 2> poolSizes{};
	poolSizes[0].type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
	poolSizes[0].descriptorCount = static_cast<uint32_t>(_swapChainImages.size());
	poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	poolSizes[1].descriptorCount = static_cast<uint32_t>(_swapChainImages.size());



	VkDescriptorPoolCreateInfo poolInfo{};
	poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());;
	poolInfo.pPoolSizes = poolSizes.data();
	poolInfo.maxSets = static_cast<uint32_t>(_swapChainImages.size());

	if (vkCreateDescriptorPool(_device, &poolInfo, nullptr, &_descriptorPool) != VK_SUCCESS) {
	    throw std::runtime_error("failed to create descriptor pool!");
	}
}

void VulkanEngine::createDescriptorSets()
{
	std::vector<VkDescriptorSetLayout> layouts(_swapChainImages.size(), _descriptorSetLayout);
	
	VkDescriptorSetAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.descriptorPool = _descriptorPool;
	allocInfo.descriptorSetCount = static_cast<uint32_t>(_swapChainImages.size());
	allocInfo.pSetLayouts = layouts.data();

	_descriptorSets.resize(_swapChainImages.size());
	if (vkAllocateDescriptorSets(_device, &allocInfo, _descriptorSets.data()) != VK_SUCCESS) {
	    throw std::runtime_error("failed to allocate descriptor sets!");
	}
	for (size_t i = 0; i < _swapChainImages.size(); i++) 
	{
		VkDescriptorBufferInfo bufferInfo{};
	    bufferInfo.buffer = _vertexBuffers[i];
	    bufferInfo.offset = 0;
	    bufferInfo.range = sizeof(SpriteVertexData) * _spriteVertices.size();

    	VkDescriptorImageInfo imageInfo{};
       	imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
      	imageInfo.imageView = _textureImageView;
      	imageInfo.sampler = _textureSampler;

	    std::array<VkWriteDescriptorSet, 2> descriptorWrites{};

        descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrites[0].dstSet = _descriptorSets[i];
        descriptorWrites[0].dstBinding = 0;
        descriptorWrites[0].dstArrayElement = 0;
        descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
        descriptorWrites[0].descriptorCount = 1;
        descriptorWrites[0].pBufferInfo = &bufferInfo;

        descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrites[1].dstSet = _descriptorSets[i];
        descriptorWrites[1].dstBinding = 1;
        descriptorWrites[1].dstArrayElement = 0;
        descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        descriptorWrites[1].descriptorCount = 1;
        descriptorWrites[1].pImageInfo = &imageInfo;

        vkUpdateDescriptorSets(_device, static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
	}
}

void VulkanEngine::cleanupSwapChain() 
{
	for (auto framebuffer : _swapChainFramebuffers) {
		vkDestroyFramebuffer(_device, framebuffer, nullptr);
	}

	vkDestroyPipeline(_device, _graphicsPipeline, nullptr);
	vkDestroyPipelineLayout(_device, _pipelineLayout, nullptr);
	vkDestroyRenderPass(_device, _renderPass, nullptr);
	for(auto imageView : _swapChainImageViews)
	{
		vkDestroyImageView(_device, imageView, nullptr);
	}
	vkDestroySwapchainKHR(_device, _swapChain, nullptr);
	

	vkDestroyBuffer(_device, _indexBuffer, nullptr);
	vkFreeMemory(_device, _indexBufferMemory, nullptr);
	for (size_t i = 0; i < _swapChainImages.size(); i++) {
        vkDestroyBuffer(_device, _vertexBuffers[i], nullptr);
        vkFreeMemory(_device, _vertexBuffersMemory[i], nullptr);
    }

    vkDestroyDescriptorPool(_device, _descriptorPool, nullptr);
}

void VulkanEngine::handleResize(GLFWwindow* window, int width, int height)
{
	auto app = reinterpret_cast<VulkanEngine*>(glfwGetWindowUserPointer(window));
    app->framebufferResized = true;
}

void VulkanEngine::recreateSwapChain()
{
	int width = 0, height = 0;
	do {
		glfwGetFramebufferSize(_window, &width, &height);
		glfwWaitEvents();
	} while(width == 0 && height == 0);

	vkDeviceWaitIdle(_device);

	cleanupSwapChain();

	createSwapChain();
	createImageViews();
	createRenderPass();
	createDescriptorSetLayout();
	createGraphicsPipeline();
	createFramebuffers();
	createBuffers();
	createDescriptorPool();
	createDescriptorSets();
	createCommandBuffers();
}

void VulkanEngine::shutdown_vulkan()
{
	vkDeviceWaitIdle(_device);
	
	cleanupSwapChain();

	vkDestroySampler(_device, _textureSampler, nullptr);
	vkDestroyImageView(_device, _textureImageView, nullptr);

	vkDestroyImage(_device, _textureImage, nullptr);
	vkFreeMemory(_device, _textureImageMemory, nullptr);

	vkDestroyDescriptorSetLayout(_device, _descriptorSetLayout, nullptr);
	for(size_t i = 0; i  < MAX_FRAMES_IN_FLIGHT; i++)
	{
		vkDestroySemaphore(_device, _renderFinishedSemaphores[i], nullptr);
	   	vkDestroySemaphore(_device, _imageAvailableSemaphores[i], nullptr);
	   	vkDestroyFence(_device, _inFlightFences[i], nullptr);
	}

	vkDestroyCommandPool(_device, _pool, nullptr);
	vkDestroyDevice(_device, nullptr);
	vkDestroySurfaceKHR(_instance, _surface, nullptr);
	vkDestroyInstance(_instance, nullptr);
	glfwDestroyWindow(_window);
	glfwTerminate();
}