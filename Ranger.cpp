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
	return (packet << value << "Ranger" << maxHealth << health << damage << playerSprite.getPosition().x << playerSprite.getPosition().y << playerSprite.getRotation() << isShooting) ? packet : sf::Packet();
}

sf::Packet Ranger::extractPacketToData(sf::Packet & packet) {
	sf::Vector2f pos;
	float rotation;
	packet >> maxHealth >> health >> damage >> pos.x >> pos.y >> rotation >> isShooting;
	playerSprite.setPosition(pos);
	playerSprite.setRotation(rotation);
	return packet;
}
