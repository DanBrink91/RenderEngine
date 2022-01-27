#include "Ability.h"

BaseAbility::BaseAbility() {}

// Strike
StrikeAbility::StrikeAbility() {}
void StrikeAbility::addHandlers(){}
void StrikeAbility::removeHandlers(){}

bool StrikeAbility::canUse()
{
	return usesPerTurn > 0;
}

void StrikeAbility::use()
{
	// Push damage to queue, with effect
}

// Block
BlockAbility::BlockAbility() {}
void BlockAbility::addHandlers(){}
void BlockAbility::removeHandlers(){}

bool BlockAbility::canUse()
{
	return usesPerTurn > 0;
}

void BlockAbility::use()
{
	// Push damage to queue, with effect
}

// Reroll
RerollAbility::RerollAbility() {}
void RerollAbility::addHandlers(){}
void RerollAbility::removeHandlers(){}

bool RerollAbility::canUse()
{
	return usesPerTurn > 0;
}

void RerollAbility::use()
{
	// Push damage to queue, with effect
}