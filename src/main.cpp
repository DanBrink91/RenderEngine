#include <iostream>
#include <chrono>
#include <thread>
#include <stdio.h>

#include <filesystem>
#include <unordered_map>

#include "rapidjson/document.h"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <glm/gtc/matrix_transform.hpp>

#include "Vulkan/vk_engine.h"

using dmilliseconds = std::chrono::duration<double, std::milli>;
const std::string SHADER_PATH = "resources/shaders/";
std::unordered_map<std::string, std::filesystem::file_time_type> shaderPaths;

void shaderFileChanged(std::filesystem::path shaderSourceFile, VulkanEngine& ve)
{
    std::string filePath = shaderSourceFile.string();
    std::string compiledPath = shaderSourceFile.replace_extension(".spv").string();
    std::array<char, 128> buffer;
    std::string result;
    std::string cmd = "glslc.exe " + filePath + " -o " +  compiledPath;
    std::unique_ptr<FILE, decltype(&_pclose)> pipe(_popen(cmd.c_str(), "r"), _pclose);
    if (!pipe) {
        throw std::runtime_error("failed to send shell command for shader reload");
    }
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
        result += buffer.data();
    }
    std::cout << result << std::endl;
    ve.recreateGraphicsPipeline();
}

int main()
{
	std::ifstream file("resources/data/config.json", std::ios::ate);
	if(!file.is_open())
	{
		std::cout << "failed to open file!";
	}
	size_t fileSize = (size_t)file.tellg();


	std::vector<char> buffer(fileSize);
	file.seekg(0);
	file.read(buffer.data(), fileSize);
	file.close();

	rapidjson::Document config;
	char* json = reinterpret_cast<char*>(buffer.data());
	if(config.ParseInsitu(json).HasParseError())
	{
		std::cout << "ERROR!!!!" << std::endl;
	}
	config.Parse(json);
	// rapidjson::Value& w = config["width"];
	std::cout << config["width"].GetInt() << std::endl;
	VulkanEngine ve;
	const auto MS_PER_FRAME = std::chrono::duration<double, std::milli> (16.7);

	ve.init_vulkan();
	char frameTimeOutput[16];
	while (!glfwWindowShouldClose(ve._window))
	{
		for (auto& file : std::filesystem::recursive_directory_iterator(SHADER_PATH)) 
		{
		    if (file.path().extension().string() == ".spv") continue;
		    auto last_write_time = std::filesystem::last_write_time(file);

		    auto filePath = file.path().string();
		    auto keyExists = shaderPaths.find(filePath) != shaderPaths.end();
		    if (keyExists)
		    {
		        // File was written to since we last checked
		        if (last_write_time != shaderPaths[filePath])
		        {
		            std::cout << filePath << " was changed!" << std::endl;
		            shaderPaths[filePath] = last_write_time;
		            
		            shaderFileChanged(file.path(), ve);
		        }
		    }
		    else 
		    {
		        shaderPaths[filePath] = last_write_time;
		    }
		}
		auto startTime = glfwGetTime();
		// keep running
		glfwPollEvents();
		
		
		ve.drawSprite(50, 50, 64*3, 64*2);
		// std::snprintf()
		ve.drawText(300, 300, (char*)"Hello World!");
		//ve.drawText(50, 50, (char*)frameTimeOutput);

		ve.drawFrame();
		if (glfwGetKey(ve._window, GLFW_KEY_ESCAPE)  ==  GLFW_PRESS)
		{
			glfwSetWindowShouldClose(ve._window, GLFW_TRUE);
		}

		auto endTime = glfwGetTime();
		auto timeSpent = std::chrono::duration<double>(endTime - startTime);
		//std::cout << endTime - startTime << std::endl;
		auto timeToSleep = MS_PER_FRAME -timeSpent;

		//std::this_thread::sleep_for(timeToSleep);
		
	}
	ve.shutdown_vulkan();

	// wait for input so I can read my validation errors :)
	system("pause");
	return 0;
}