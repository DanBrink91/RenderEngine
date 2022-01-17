#include "GameManager.h"

void GameManager::init(VulkanEngine& vulkanEngine) 
{
	_vulkanEngine = &vulkanEngine;
	loadData();

	glfwGetCursorPos(_vulkanEngine->_window, &_mouseState.x, &_mouseState.y);
}

void GameManager::update() 
{
	updateMouseInput();
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
		_mouseState.dragSpriteIndex = -1;
	} 
}

void GameManager::updateSprites() 
{
	for(size_t i = 0; i < _sprites.size(); i++)
	{
		auto& sprite = _sprites[i];
		// Mouse bounds checking 
		if(_mouseState.x >= sprite.position.x && _mouseState.y >= sprite.position.y)
		{
			int width = sprite.texture->width;
			int height = sprite.texture->height;

			if(_mouseState.x <= sprite.position.x + width && _mouseState.y <= sprite.position.y + height)
			{
				// We're at least hovering here
				if(_mouseState.down[0])
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
		
		_vulkanEngine->drawSprite(sprite);
	}
}

void GameManager::updateText() 
{
	
}

void GameManager::loadData() 
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
		std::cout << "ERROR Parsing config.json" << std::endl;
	}
	config.Parse(json);
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
		// const rapidjson::Value& filePath = texture["path"].GetString();
		Texture t = _vulkanEngine->createTextureImage(textureInfo["path"].GetString());
		_textures.push_back(t);
		_textureLookup[textureInfo["name"].GetString()] = t;
	}

	// Let's make some sprites!
	Texture *orcTexture = &_textureLookup["orc"];
	Sprite orc = {.position={30.0f, 30.0f}, .texture=orcTexture};
	_sprites.push_back(orc);
}