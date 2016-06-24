#pragma once

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
	Mage(sf::Texture & mageTexture, sf::Texture & fireballTextureA, sf::Texture & fireballTextureB, float moveSpeed = 1.5f,
		float fireballSpeed = 0.5f, float fireballReloadTime = 0.3f, float timeAfterShot = 1.0f, float slowSpeed = 0.0f);
	// move
	void move(const sf::RenderWindow & window, const sf::Keyboard::Key releasedKey);
	// chain packets of type Knight
	sf::Packet chainDataToPacket(sf::Packet & packet, std::string value);
	// extract packets of type Knight
	sf::Packet extractPacketToData(sf::Packet & packet);

private:
	float moveSpeed;
	float slowSpeed;
};

#endif