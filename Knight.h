#pragma once

#ifndef KNIGHT_H
#define KNIGHT_H

#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>
#include "Character.h"

class Knight : public Character {
public:
	//constructor that creates the knight
	Knight(sf::Texture & knightTexture, sf::Texture & swordTexture,float moveSpeed = 1.5f, int maxHealth = 100.0f, int damage = 10.0f);
	//draws sword using "swordSprite"
	void drawSword(sf::RenderWindow & window);
	//swing and rotate sword
	void swingSword();
	// chain packets of type Knight
	sf::Packet chainDataToPacket(sf::Packet & packet, std::string value);
	// extract packets of type Knight
	sf::Packet extractPacketToData(sf::Packet & packet);
private:
	sf::Sprite swordSprite; //sprite for sword
	sf::CircleShape swordCircle; //circle that sword rotates around
	bool isSwinging = false; //whether sword is being swung
	float reloadTime = 0.1f; //how long before you can swing again
	float swingTime = 0.75; //how long the swing lasts
	sf::Clock clockSwingTime; //used for timing how long before you can swing again, timing how long swing lasts, and rotating sword based on times
};

#endif