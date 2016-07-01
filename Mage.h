#ifndef MAGE_H
#define MAGE_H

#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>
#include "ProjectileShooter.h"

class Mage : public ProjectileShooter {
public:
	//constructor that creates the mage
	//@param mageTexture: texture for mage
	//@param fireballTextureA: texture for first animation frame of fireball
	//@param fireballTextureB: texture for second animation frame of fireball (same texture as A if no animation for fireball)
	//@param fireballSpeed: speed of fireballs shot
	//@param fireballReloadTime: time before you can shoot another fireball
	//@param timeAfterShot: amount of time you want slowSpeed to last
	//@param slowSpeed: movement speed when shooting
	//@param maxHealth: health that mage starts with
	//@param damageMin: min fireball damage
	//@param damageMax: max fireball damage
	//@param moveSpeed: movement speed for mage
	//@param fromWeapon: whether fireball shoots from weapon (or from player)
	Mage(sf::Texture & healthBarForegroundTexture, sf::Texture & healthBarBackgroundTexture, sf::Texture & mageTexture, sf::Texture & staffTexture, 
		sf::Texture & fireballTextureA, sf::Texture & fireballTextureB, float fireballSpeed = 0.5f, float fireballReloadTime = 0.3f,
		float timeAfterShot = 1.0f, float slowSpeed = 0.0f, int maxHealth = 100, int damageMin = 10, int damageMax = 20, float moveSpeed = 1.5f,
		bool fromWeapon = true);
	//destructor for mage
	~Mage();
	//movement for mage
	void move(const sf::RenderWindow & window, const sf::Keyboard::Key releasedKey);
	// chain packets of type Mage
	sf::Packet chainDataToPacket(sf::Packet & packet, std::string value);
	// extract packets of type Mage
	sf::Packet extractPacketToData(sf::Packet & packet);
	//get damage for mage
	sf::Int16 getDamage();
private:
	float moveSpeed; //regular movement speed
	float slowSpeed; ////movement speed when shooting fireballs
	int damageMin; //min fireball damage
	int damageMax; //max fireball damage
};

#endif