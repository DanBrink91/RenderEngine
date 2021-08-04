#include <iostream>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <glm/gtc/matrix_transform.hpp>

#include "Vulkan/vk_engine.h"

int main()
{
	VulkanEngine ve;

	std::cout << "Hello World!" << std::endl;

	if (!glfwInit())
	{
		return -1;
	}
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

	GLFWwindow* window = glfwCreateWindow(640, 480, "Vulkan!", NULL, NULL);
	if(!window)
	{
		glfwTerminate();
		return -1;
	}

	ve.init_vulkan();

	while (!glfwWindowShouldClose(window))
	{
		// keep running
		glfwPollEvents();
		if (glfwGetKey(window, GLFW_KEY_ESCAPE)  ==  GLFW_PRESS)
		{
			glfwSetWindowShouldClose(window, GLFW_TRUE);
		}
	}

	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}