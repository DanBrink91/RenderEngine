#include "Ability.h"

BaseAbility::BaseAbility() {}

// Strike
StrikeAbility::StrikeAbility() {}
void StrikeAbility::addHandlers(){}
void StrikeAbility::removeHandlers(){}

// Even
bool StrikeAbility::canUse(Player player, Dice usedDice)
{
	return usesPerTurn > 0 &&
	 usedDice.value % 2 == 0;
}

void StrikeAbility::use(Player player, Enemy target, Dice dice)
{
	// Push damage to queue, with effect
	BattleSystem.push_back(DamageAction(player, target, baseDamage));
}

// Block
BlockAbility::BlockAbility() {}
void BlockAbility::addHandlers(){}
void BlockAbility::removeHandlers(){}

// Odd
bool BlockAbility::canUse(Player player, Dice usedDice)
{
	return usesPerTurn > 0 &&
	usedDice.value  % 2 != 0;
}

void BlockAbility::use(Player player, Enemy target, Dice dice)
{
	// Push damage to queue, with effect
	BattleSystem.push_back(BlockAction(player, player, baseBlock));
}

// Reroll
RerollAbility::RerollAbility() {}
void RerollAbility::addHandlers(){}
void RerollAbility::removeHandlers(){}

bool RerollAbility::canUse(Player player, Dice usedDice)
{
	return usesPerTurn > 0;
}

void RerollAbility::use(Player player, Enemy target, Dice dice)
{
	// Push damage to queue, with effect
}