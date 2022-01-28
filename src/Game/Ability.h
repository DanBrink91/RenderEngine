#pragma once
#include <string>


enum DiceRequirements
{
	ANY = 0,
	EVEN,
	ODD,
	GREATER_THAN,
	LESS_THAN
};

struct Dice
{
	int value;
};

class BaseAbility 
{
public:

	BaseAbility();
	virtual void addHandlers() = 0;
	virtual void removeHandlers() = 0;
	virtual bool canUse(Player player, Dice usedDice) = 0; // TODO needs a state parameter or at least Dice[]
	virtual void use(Player player, Enemy target, Dice dice) = 0;

	std::string name, description;
	// Panel Color
protected:
	int usesPerTurn = 1;
};

class StrikeAbility : BaseAbility 
{
public:
	StrikeAbility();
	void addHandlers();
	void removeHandlers();
	bool canUse(Player player, Dice usedDice);
	void use(Player player, Enemy target, Dice dice);
private:
	int baseDamage = 5;
	int usesPerTurn = 3;
};

class BlockAbility : BaseAbility
{
public:
	BlockAbility();
	void addHandlers();
	void removeHandlers();
	bool canUse(Player player, Dice usedDice);
	void use(Player player, Enemy target, Dice dice);
private:
	int baseBlock = 3;
	int usesPerTurn = 3;
};

class RerollAbility : BaseAbility
{
public:
	RerollAbility();
	void addHandlers();
	void removeHandlers();
	bool canUse(Player player, Dice usedDice);
	void use(Player player, Enemy target, Dice dice);
};
