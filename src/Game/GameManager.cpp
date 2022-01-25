#include "GameManager.h"

void GameManager::init(VulkanEngine& vulkanEngine) 
{
	_vulkanEngine = &vulkanEngine;
	loadData();

	glfwGetCursorPos(_vulkanEngine->_window, &_mouseState.x, &_mouseState.y);
	startPlayerTurn();
}

void GameManager::loadData() 
{
	std::ifstream file("resources/data/config.json", std::ios::ate | std::ios::binary);
	if(!file.is_open())
	{
		std::cout << "failed to open file!";
	}

	std::string buffer;
	size_t fileSize = (size_t)file.tellg();
	buffer.resize(fileSize);

	file.seekg(0, std::ios::beg);
	file.read(&buffer[0], fileSize);
	file.close();

	rapidjson::Document config;

	rapidjson::ParseResult ok = config.Parse(buffer.c_str());
	if(!ok)
	{
		//std::cout << "ERROR Parsing config.json: " << rapidjson::GetParseErrorFunc(ok.Code()) << " " << ok.Offset() << std::endl;
	}
	// config.Parse(json);
	// rapidjson::Value& w = config["width"];

	// Player Starting Stats
	const rapidjson::Value& playerStats = config["player"];
	_stats.maxHP = _stats.currentHP = playerStats["maxHP"].GetInt();
	_stats.diceCount = playerStats["startingDice"].GetInt();
	// make starting dice
	for(int i = 0; i < _stats.diceCount; i++)
	{
		Dice d {.value = 6 };
		_stats.dice.push_back(d);
	}

	// Now we load the sprites!
	const rapidjson::Value& textures = config["textures"];
	_textures.resize(textures.Size());
	for(rapidjson::SizeType i = 0; i < textures.Size(); i++)
	{
		auto textureInfo = textures[i].GetObject();
		Texture t = _vulkanEngine->createTextureImage(textureInfo["path"].GetString());
		_textures.push_back(t);
		_textureLookup[textureInfo["name"].GetString()] = t;
	}
	_panelTexture = &_textureLookup["panel"];

	// Let's make some sprites!
	Texture *orcTexture = &_textureLookup["orc"];
	Sprite orc = {.position={600.0f, 30.0f}, .texture=orcTexture};
	_sprites.push_back(orc);


	// Event manager
	uint64_t someID = _eventManager.subscribe(EventTypeBattleStart, [](struct Event* e) {
		BattleStartEvent* be = static_cast<BattleStartEvent*>(e);
		std::cout << "Battle Start event triggered! Enemy: " << be->enemyType << std::endl;
	});
}

void GameManager::createSprite(std::string texture, glm::vec2 position, bool hoverable, bool draggable) 
{
	Texture *t = &_textureLookup[texture];
	Sprite sprite = {.position=position, .texture=t};
	_sprites.push_back(sprite);
	if(hoverable)
	{
		MouseInteractive mi = {.spriteIndex=static_cast<int>(_sprites.size() - 1), .draggable=draggable};
		_mouseInteractives.push_back(mi);
	}

}

void GameManager::createSprite(Texture* texture, glm::vec2 position, bool hoverable, bool draggable) 
{
	Sprite sprite = {.position=position, .texture=texture};
	_sprites.push_back(sprite);
	if(hoverable)
	{
		MouseInteractive mi = {.spriteIndex=static_cast<int>(_sprites.size() - 1), .draggable=draggable};
		_mouseInteractives.push_back(mi);
	}

}

void GameManager::update() 
{
	updateMouseInput();
	updateMouseInteractives();
	updatePanels();
	updateSprites();
	updateText();
}

void GameManager::updateMouseInput() 
{
	_mouseState.previousX = _mouseState.x;
	_mouseState.previousY = _mouseState.y;
	glfwGetCursorPos(_vulkanEngine->_window, &_mouseState.x, &_mouseState.y);
	int leftState = glfwGetMouseButton(_vulkanEngine->_window, GLFW_MOUSE_BUTTON_LEFT);
	int rightState = glfwGetMouseButton(_vulkanEngine->_window, GLFW_MOUSE_BUTTON_RIGHT);
	_mouseState.down[0] = GLFW_PRESS == leftState;
	_mouseState.down[1] = GLFW_PRESS == rightState;
	
	// Released drag on a sprite
	if(leftState == GLFW_RELEASE && _mouseState.dragSpriteIndex != -1)
	{
		BattleStartEvent e = {EventTypeBattleStart, 1};
		_eventManager.notify(EventTypeBattleStart, &e);
		_mouseState.dragSpriteIndex = -1;
	} 
}

void GameManager::updateMouseInteractives() 
{
	for(size_t i = 0; i < _mouseInteractives.size(); i++)
	{
		MouseInteractive mi = _mouseInteractives[i];
		Sprite& sprite = _sprites[mi.spriteIndex];
		// Mouse bounds checking 
		if(_mouseState.x >= sprite.position.x && _mouseState.y >= sprite.position.y)
		{
			int width = sprite.texture->width;
			int height = sprite.texture->height;

			if(_mouseState.x <= sprite.position.x + width && _mouseState.y <= sprite.position.y + height)
			{
				// We're at least hovering here
				if(mi.draggable && _mouseState.down[0])
				{
					if(_mouseState.dragSpriteIndex == i)
					{
						//  We're dragging this sprite!!
						sprite.position.x  += _mouseState.x - _mouseState.previousX;
						sprite.position.y += _mouseState.y - _mouseState.previousY;
					}
					else if(_mouseState.dragSpriteIndex == -1)
					{
						_mouseState.dragSpriteIndex = i;
					}
				}
			}
		}

	}
}

void GameManager::updatePanels() 
{
	for(size_t i = 0; i < _panels.size(); i++)
	{
		auto& panel = _panels[i];
		
		_vulkanEngine->drawSprite(panel);
	}
}

void GameManager::updateSprites() 
{
	for(size_t i = 0; i < _sprites.size(); i++)
	{
		auto& sprite = _sprites[i];
		
		_vulkanEngine->drawSprite(sprite);
	}
}

void GameManager::updateText() 
{
	
}

void GameManager::startPlayerTurn() 
{
	// Roll the dice
	std::uniform_int_distribution<> distrib(1, 6);
	for(size_t i = 0; i < _stats.dice.size(); i++)
	{
		int value = distrib(_rd);
		std::string strValue = std::to_string(value);		
		createSprite(strValue, {10.0f + 90.0f * i, 510.0f}, true, true);
	}
	createPanel({100.0f, 150.0f}, 250, 200, std::string("Place dice in here to attack"));
}

void GameManager::startEnemyTurn() 
{
	
}
void GameManager::updateEnemyTurn() 
{
	
}

void GameManager::createPanel(glm::vec2 position, int width, int height, std::string text) 
{
	_textures.push_back({.id=_panelTexture->id, .width=width, .height=height, .UV=_panelTexture->UV});
	Sprite panelSprite = {.position=position, .texture=&_textures.back()};
	_panels.push_back(panelSprite);
}