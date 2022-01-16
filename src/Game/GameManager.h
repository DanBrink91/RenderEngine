
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

#include <vector>

#include "rapidjson/document.h"


#include "../Vulkan/vk_engine.h"

#ifdef GetObject
#undef GetObject
#endif

struct Sprite 
{
	glm::vec2 position;
	int width, height;
	int texture;
};

struct Texture
{
	uint32_t id;
	glm::vec2 UV;
};

class GameManager {
public:
	void init(VulkanEngine& ve);
	void update();
private:
	void loadData();
	void updateSprites();
	void updateText();
	
	VulkanEngine* _vulkanEngine;
	std::vector<Texture> _textures;
	std::vector<Sprite> _sprites;
};