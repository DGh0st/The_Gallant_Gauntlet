#include "Defines.h"
#include "Mage.h"

Mage::Mage(sf::Texture & mageTexture, sf::Texture & fireballTextureA, sf::Texture & fireballTextureB, float moveSpeed, float fireballSpeed,
	float fireballReloadTime, float timeAfterShot, float slowSpeed) : ProjectileShooter(mageTexture, fireballTextureA, fireballTextureB,
	fireballSpeed, fireballReloadTime, timeAfterShot), moveSpeed(moveSpeed),slowSpeed(slowSpeed) {
	charSpeed = moveSpeed;
}

void Mage::move(const sf::RenderWindow & window, const sf::Keyboard::Key releasedKey) {
	if (isShooting) {
		charSpeed = slowSpeed;
	}
	else {
		charSpeed = moveSpeed;
	}
	Character::move(window, releasedKey);
}