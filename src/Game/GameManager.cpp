#include "GameManager.h"

void GameManager::init(VulkanEngine& vulkanEngine) 
{
	_vulkanEngine = &vulkanEngine;
	loadData();
}

void GameManager::update() 
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
	std::cout << config["width"].GetInt() << std::endl;

	// Now we load the sprites!
	const rapidjson::Value& textures = config["textures"];
	for(rapidjson::SizeType i = 0; i < textures.Size(); i++)
	{
		auto texture = textures[i].GetObject();
		std::cout << texture["path"].GetString() << std::endl;
	}
}