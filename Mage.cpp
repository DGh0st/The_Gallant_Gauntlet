#include "Defines.h"
#include "Mage.h"

Mage::Mage(sf::Texture & healthBarForegroundTexture, sf::Texture & healthBarBackgroundTexture, sf::Texture & mageTexture, 
	sf::Texture & staffTexture, sf::Texture & fireballTextureA, sf::Texture & fireballTextureB, float moveSpeed, float fireballSpeed,
	float fireballReloadTime, float timeAfterShot, float slowSpeed, bool fromWeapon) : 
	ProjectileShooter(healthBarForegroundTexture, healthBarBackgroundTexture, mageTexture, staffTexture, fireballTextureA, fireballTextureB, fireballSpeed, fireballReloadTime, timeAfterShot, fromWeapon), 
	moveSpeed(moveSpeed), slowSpeed(slowSpeed) {
	charSpeed = moveSpeed;
}

Mage::~Mage() {
	int i = 0;
	while (projectiles.size() > 0) {
		delete projectiles[i];
		projectiles[i] = NULL;
		projectiles.erase(projectiles.begin() + i);  //remove NULL from vector
	}
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
	sf::Vector2i mousePos = sf::Vector2i(-1, -1);
	if (projectiles.size() > 0) {
		mousePos = projectiles[0]->mousePos;
	}
	return (packet << value << "Mage" << maxHealth << health << damage << playerSprite.getPosition().x << playerSprite.getPosition().y << playerSprite.getRotation() << isShooting << fromWeapon << weaponSprite.getRotation() << mousePos.x << mousePos.y << uniqueProjectilesCounter) ? packet : sf::Packet();
}

sf::Packet Mage::extractPacketToData(sf::Packet & packet) {
	sf::Vector2f pos;
	float rotation;
	float angle;
	sf::Vector2i mousePos;
	sf::Uint16 projectilesCounter;
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
