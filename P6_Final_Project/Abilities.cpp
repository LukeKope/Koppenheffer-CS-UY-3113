#include "Abilities.h"

Abilities::Abilities(const std::string& moveName, int mpAmnt, int damageAmnt) : name(moveName), mp(mpAmnt), damage(damageAmnt) {}

const std::string& Abilities::getName() const {
	return name;
}

const int Abilities::getDamage() const {
	return damage;
}