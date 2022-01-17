#include <iostream>
#include <chrono>
#include <thread>
#include <stdio.h>

#if defined(_WIN32) || defined(_WIN64)
#include <windows.h >
#endif

// #include <filesystem>

#include "rapidjson/document.h"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <glm/gtc/matrix_transform.hpp>

#ifdef GetObject
#undef GetObject
#endif

#include "Vulkan/vk_engine.h"
#include "Game/GameManager.h"


using dmilliseconds = std::chrono::duration<double, std::milli>;
const std::string SHADER_PATH = "resources/shaders/";


int main()
{
	VulkanEngine ve;
	GameManager gm;

	ve.init_vulkan();
	gm.init(ve);

	const auto MS_PER_FRAME = std::chrono::duration<double, std::milli> (16.7);
	char frameTimeOutput[50], fpsOutput[50];



	const double alpha = 0.95; // how fast to move towards newer avg
	double avgFrameTime = 16.0;
	double avgFPS = 60.0;

	#if defined(_WIN32) || defined(_WIN64)
	DWORD waitStatus;
	HANDLE dwChangeHandle;
	dwChangeHandle = FindFirstChangeNotification(SHADER_PATH.c_str(), FALSE, FILE_NOTIFY_CHANGE_LAST_WRITE);
	#endif
	
	while (!glfwWindowShouldClose(ve._window))
	{
		#if defined(_WIN32) || defined(_WIN64)
		waitStatus = WaitForSingleObject(dwChangeHandle, 0);

		if(waitStatus == WAIT_OBJECT_0)
		{
			FindCloseChangeNotification(dwChangeHandle);
			std::array<char, 512> buffer;
			std::string result;
			std::string cmd = "resources\\shaders\\compile.bat";
			std::unique_ptr<FILE, decltype(&_pclose)> pipe(_popen(cmd.c_str(), "r"), _pclose);
			// if (!pipe) {
			//     throw std::runtime_error("failed to send shell command for shader reload");
			// }
			while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
			    result += buffer.data();
			}
			std::cout << "Updated shaders" << std::endl;
			ve.recreateGraphicsPipeline();
			dwChangeHandle = FindFirstChangeNotification(SHADER_PATH.c_str(), FALSE, FILE_NOTIFY_CHANGE_LAST_WRITE);
		}
		#endif

		auto startTime = glfwGetTime();
		// keep running
		glfwPollEvents();
		
		gm.update();
		// ve.drawSprite(50, 50, 84, 84);
		
		sprintf(frameTimeOutput, "Avg Time: %.2fms", avgFrameTime);
		sprintf(fpsOutput, "Avg FPS:  %.1f", avgFPS);
		
		ve.drawText(5, 15, (char*)frameTimeOutput);
		ve.drawText(5, 31, (char*)fpsOutput);

		ve.drawFrame();
		if (glfwGetKey(ve._window, GLFW_KEY_ESCAPE)  ==  GLFW_PRESS)
		{
			glfwSetWindowShouldClose(ve._window, GLFW_TRUE);
		}

		auto endTime = glfwGetTime();
		double timeSpentS = (endTime - startTime);
		avgFrameTime = alpha * avgFrameTime + (1.0 - alpha) * timeSpentS * 1000;
		avgFPS = alpha * avgFPS + (1.0 - alpha) * 1.0 / timeSpentS;
		
	 	// auto timeSpent = std::chrono::duration<double>(endTime - startTime);
		// auto timeToSleep = MS_PER_FRAME - timeSpent;
		// std::cout << "hello" << std::endl;
		// std::this_thread::sleep_for(timeToSleep);
	}
	#if defined(_WIN32) || defined(_WIN64)
	FindCloseChangeNotification(dwChangeHandle);
	#endif
	ve.shutdown_vulkan();

	// wait for input so I can read my validation errors :)
	system("pause");
	return 0;
}