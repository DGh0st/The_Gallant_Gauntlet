#include "Defines.h"
#include "Ranger.h"

Ranger::Ranger(sf::Texture & rangerTexture, sf::Texture & bowTexture, sf::Texture & arrowTextureA, sf::Texture & arrowTextureB, float moveSpeed,
	float arrowSpeed, float arrowReloadTime, float timeAfterShot, float slowSpeed, bool fromWeapon) : ProjectileShooter(rangerTexture, bowTexture,
		arrowTextureA, arrowTextureB, arrowSpeed, arrowReloadTime, timeAfterShot, fromWeapon), moveSpeed(moveSpeed), slowSpeed(slowSpeed) {
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

sf::Packet Ranger::chainDataToPacket(sf::Packet & packet, std::string value) {
	sf::Vector2i mousePos = sf::Vector2i(-1, -1);
	if (projectiles.size() > 0) {
		mousePos = projectiles[0]->mousePos;
	}
	return (packet << value << "Ranger" << maxHealth << health << damage << playerSprite.getPosition().x << playerSprite.getPosition().y << playerSprite.getRotation() << isShooting << fromWeapon << weaponSprite.getRotation() << mousePos.x << mousePos.y << uniqueProjectilesCounter) ? packet : sf::Packet();
}

sf::Packet Ranger::extractPacketToData(sf::Packet & packet) {
	sf::Vector2f pos;
	float rotation;
	float angle;
	sf::Vector2i mousePos;
	sf::Uint64 projectilesCounter;
	packet >> maxHealth >> health >> damage >> pos.x >> pos.y >> rotation >> isShooting >> fromWeapon >> angle >> mousePos.x >> mousePos.y >> projectilesCounter;
	playerSprite.setPosition(pos);
	playerSprite.setRotation(rotation);
	weaponSprite.setRotation(angle);
	if (mousePos != sf::Vector2i(-1, -1) && projectilesCounter > uniqueProjectilesCounter) {
		uniqueProjectilesCounter++;
		Projectile * projectile;
		if (!fromWeapon) { //shoot projectile from playerSprite
			projectile = new Projectile(mousePos, projectileTextureA, playerSprite);
		}
		else { //shoot projectile from weaponSprite
			projectile = new Projectile(mousePos, projectileTextureA, weaponSprite);
		}
		projectiles.insert(projectiles.begin(), projectile); //store projectile
		clockReload.restart();
		clockProjectile.restart();
	}
	return packet;
}
