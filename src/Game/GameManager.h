#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

#include <vector>
#include <unordered_map>
#include <random>
#include <string>
#include <chrono>

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

struct MouseInteractive
{
	int spriteIndex;
	bool draggable;
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
	void updateMouseInteractives();
	void updatePanels();
	void updateSprites();
	void updateText();

	void startPlayerTurn();
	void startEnemyTurn();
	void updateEnemyTurn();

	void createSprite(std::string texture, glm::vec2 position, bool hoverable, bool draggable);
	void createSprite(Texture* texture, glm::vec2 position, bool hoverable, bool draggable);
	void createPanel(glm::vec2 position, int width, int height, std::string text);
	
	VulkanEngine* _vulkanEngine;
	
	std::vector<Texture> _textures;
	std::vector<Sprite> _sprites;
	std::vector<MouseInteractive> _mouseInteractives;
	std::vector<Sprite> _panels;

	std::unordered_map<std::string, Texture> _textureLookup;
	MouseState _mouseState;
	PlayerStats _stats;
	std::random_device _rd;
	std::mt19937 _gen;
	Texture* _panelTexture;
};