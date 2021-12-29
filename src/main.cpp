#include <iostream>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <glm/gtc/matrix_transform.hpp>

#include "Vulkan/vk_engine.h"

int main()
{
	VulkanEngine ve;


	ve.init_vulkan();

	while (!glfwWindowShouldClose(ve._window))
	{
		// keep running
		glfwPollEvents();
		ve.drawFrame();
		if (glfwGetKey(ve._window, GLFW_KEY_ESCAPE)  ==  GLFW_PRESS)
		{
			glfwSetWindowShouldClose(ve._window, GLFW_TRUE);
		}
	}
	ve.shutdown_vulkan();

	// wait for input so I can read my validation errors :)
	system("pause");
	return 0;
}