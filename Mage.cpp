#include "Defines.h"
#include "Mage.h"

Mage::Mage(sf::Texture & mageTexture, sf::Texture & staffTexture, sf::Texture & fireballTextureA, sf::Texture & fireballTextureB, float moveSpeed,
	float fireballSpeed, float fireballReloadTime, float timeAfterShot, float slowSpeed, bool fromWeapon) : ProjectileShooter(mageTexture, 
	staffTexture, fireballTextureA, fireballTextureB, fireballSpeed, fireballReloadTime, timeAfterShot, fromWeapon), moveSpeed(moveSpeed),
	slowSpeed(slowSpeed) {
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

sf::Packet Mage::chainDataToPacket(sf::Packet & packet, std::string value) {
	return (packet << value << "Mage" << maxHealth << health << damage << playerSprite.getPosition().x << playerSprite.getPosition().y << playerSprite.getRotation() << isShooting) ? packet : sf::Packet();
}

sf::Packet Mage::extractPacketToData(sf::Packet & packet) {
	sf::Vector2f pos;
	float rotation;
	packet >> maxHealth >> health >> damage >> pos.x >> pos.y >> rotation >> isShooting;
	playerSprite.setPosition(pos);
	playerSprite.setRotation(rotation);
	return packet;
}
