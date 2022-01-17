#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

#include <vector>
#include <unordered_map>

#include "rapidjson/document.h"


#include "../Vulkan/vk_engine.h"

#ifdef GetObject
#undef GetObject
#endif

struct Enemy
{
	Sprite sprite;
	int currentHealth, maxHealth, armor;
};

struct BattleInfo 
{
	int currentTurn;
	Enemy* enemies;
};

struct MouseState
{
	double previousX, previousY;
	double x, y;
	bool down[2];
	int dragSpriteIndex;
};

struct Dice
{
	int value;
};

struct PlayerStats
{
	int currentHP;
	int maxHP;
	int diceCount;
	std::vector<Dice> dice;
};

class GameManager {
public:
	void init(VulkanEngine& ve);
	void update();
private:
	void loadData();
	void updateMouseInput();
	void updateSprites();
	void updateText();
	
	VulkanEngine* _vulkanEngine;
	std::vector<Texture> _textures;
	std::vector<Sprite> _sprites;
	std::unordered_map<std::string, Texture> _textureLookup;
	MouseState _mouseState;
	PlayerStats _stats;
};