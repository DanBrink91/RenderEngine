#include "vk_engine.h"

void VulkanEngine::init_vulkan()
{
	VkApplicationInfo appInfo = {};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName = "Vulkan Testing";
	appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.pEngineName = "Brink Engine";
	appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.apiVersion = VK_API_VERSION_1_2;

	VkInstanceCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &appInfo;
	createInfo.enabledExtensionCount = 0;
	createInfo.ppEnabledExtensionNames = NULL;

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
}

// TODO: Pick from different devices instead of just picking the first one
void VulkanEngine::find_physical_device()
{
	//  FIND PHYSICAL
	uint32_t deviceCount;
	VkResult result = vkEnumeratePhysicalDevices(_instance, &deviceCount, nullptr);

	assert(result == VK_SUCCESS);
	assert(deviceCount >= 1);
	std::cout << deviceCount << " devices found!" << std::endl;
	
	std::vector<VkPhysicalDevice> physicalDevices(deviceCount);
	result = vkEnumeratePhysicalDevices(_instance, &deviceCount, physicalDevices.data());
	assert(result == VK_SUCCESS);
	_chosenGPU = physicalDevices[0];
}

void VulkanEngine::create_device()
{
	// CREATE QUEUE
	VkPhysicalDeviceProperties physicalProperties = {};
	vkGetPhysicalDeviceProperties(_chosenGPU, &physicalProperties);

	uint32_t queuefamilyCount;
	vkGetPhysicalDeviceQueueFamilyProperties(_chosenGPU, &queuefamilyCount, nullptr);
	std::cout << queuefamilyCount << " queue families were found" << std::endl;

	std::vector<VkQueueFamilyProperties> queueFamilyProperties(queuefamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(_chosenGPU, &queuefamilyCount, queueFamilyProperties.data());

	float priorities[] = { 1.0f };
	VkDeviceQueueCreateInfo queueCreateInfo = {};
	queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	queueCreateInfo.pNext = nullptr;
	queueCreateInfo.flags = 0;

	bool found = false;
	for(unsigned int i = 0; i < queuefamilyCount; i++)
	{
		// VK_QUEUE_GRAPHICS_BIT
		// VK_QUEUE_COMPUTE_BIT
		// VK_QUEUE_TRANSFER_BIT
		// VK_QUEUE_SPARSE_BINDING_BIT
		if(queueFamilyProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
		{
			found = true;
			graphicsFamilyIndex = i;
			queueCreateInfo.queueFamilyIndex = i;
			break;
		}
	}
	if (!found)
	{
		std::cout << "Failed to find graphics queue!" << std::endl;
	}

	queueCreateInfo.queueCount = 1;
	queueCreateInfo.pQueuePriorities = &priorities[0];

	// CREATE LOGICAL DEVICE
	VkDeviceCreateInfo device_info = {};
	device_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	device_info.pNext = NULL;
	device_info.queueCreateInfoCount = 1;
	device_info.pQueueCreateInfos = &queueCreateInfo;
	device_info.enabledExtensionCount = 0;
	device_info.ppEnabledExtensionNames = NULL;
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