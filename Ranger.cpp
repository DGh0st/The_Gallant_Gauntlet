#include "Defines.h"
#include "Ranger.h"

Ranger::Ranger(sf::Texture & rangerTexture, sf::Texture & arrowTextureA, sf::Texture & arrowTextureB, float moveSpeed,
	float arrowSpeed, float arrowReloadTime, float timeAfterShot, float slowSpeed) : ProjectileShooter(rangerTexture, arrowTextureA, arrowTextureB,
		arrowSpeed, arrowReloadTime, timeAfterShot), moveSpeed(moveSpeed), slowSpeed(slowSpeed) {
	charSpeed = moveSpeed;
}

void Ranger::move(const sf::RenderWindow & window, const sf::Keyboard::Key releasedKey) {
	if (isShooting) {
		charSpeed = slowSpeed;
	}
	else {
		charSpeed = moveSpeed;
	}
	Character::move(window, releasedKey);
}