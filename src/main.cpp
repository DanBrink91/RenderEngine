#include <iostream>
#include <chrono>
#include <stdio.h>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <glm/gtc/matrix_transform.hpp>

#include "Vulkan/vk_engine.h"

int main()
{
	VulkanEngine ve;


	ve.init_vulkan();
	char frameTimeOutput[16];
	auto previousTime = std::chrono::high_resolution_clock::now();
	while (!glfwWindowShouldClose(ve._window))
	{
		// keep running
		glfwPollEvents();
		
		ve.drawSprite(50, 50, 512, 512);
		// std::snprintf()
		ve.drawText(200, 200, (char*)"Hello World!");
		ve.drawText(50, 50, (char*)frameTimeOutput);

		ve.drawFrame();
		if (glfwGetKey(ve._window, GLFW_KEY_ESCAPE)  ==  GLFW_PRESS)
		{
			glfwSetWindowShouldClose(ve._window, GLFW_TRUE);
		}
		auto currentTime = std::chrono::high_resolution_clock::now();
		std::chrono::duration<double> diff = currentTime - previousTime;
		previousTime = currentTime;
		sprintf(frameTimeOutput, "FT: %.3f", diff.count());
		// totalTime += diff.count();
		// if (totalTime >= 1.0)
		// {
		// 	fps = frames;
		// 	totalTime = 0.0;
		// 	frames = 0.0;
		// 	std::cout << "FPS: " << fps << std::endl;
		// }
		// std::cout << diff.count() << std::endl;
	}
	ve.shutdown_vulkan();

	// wait for input so I can read my validation errors :)
	system("pause");
	return 0;
}