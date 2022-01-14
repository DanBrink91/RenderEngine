#pragma once

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

#include "rapidjson/document.h"

#include "Vulkan/vk_engine.h"

struct Sprite 
{
	glm::vec2 position;
	int width, height;
	int texture;
};

class GameManager {
public:
	GameManager();
	void init();
	void update();
private:
	void loadData();
	VulkanEngine ve;
};