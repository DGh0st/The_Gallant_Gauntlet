#include <math.h>
#include <iostream>
#include <fstream>
#include <stdlib.h>

#include "Character.h"
#include "Defines.h"

Character::Character(int maxHealth, int damage, float divingAccuracy, float charSpeed, float diveSpeed, float diveResetTime, float diveResistance) : maxHealth(maxHealth), health(maxHealth), damage(damage), divingAccuracy(divingAccuracy), charSpeed(charSpeed), diveSpeed(diveSpeed), diveResetTime(diveResetTime), diveResistance(diveResistance) {
	player = sf::RectangleShape(sf::Vector2f(playerSize, playerSize));
	player.setFillColor(sf::Color::Green);
	player.setOrigin(playerSize / 2, playerSize / 2);
	player.setPosition(playerSize, playerSize);
	divingMovement = sf::Vector2f(0.0f, 0.0f);
	lastKey = sf::Keyboard::Unknown;
	diveResetTimer.restart();
}

Character::~Character() {
	// nothing to clean up as of yet...
}

void Character::operator=(const Character & other) {
	player = other.player;
	maxHealth = other.maxHealth;
	health = other.health;
	damage = other.damage;
	divingMovement = other.divingMovement;
	lastKey = other.lastKey;
	diveResetTimer = other.diveResetTimer;
	divingAccuracy = other.divingAccuracy;
	charSpeed = other.charSpeed;
	diveSpeed = other.diveSpeed;
	diveResetTime = other.diveResetTime;
	diveResistance = other.diveResistance;
}

void Character::draw(sf::RenderWindow & window) const {
	window.draw(player);
}

void Character::move(const sf::RenderWindow & window, const sf::Keyboard::Key releasedKey) {
	// player rotation
	sf::Vector2i mousePos = sf::Mouse::getPosition(window);
	sf::Vector2i charPos = (sf::Vector2i)player.getPosition();
	sf::Vector2i diff = mousePos - charPos;
	float angle = (float)atan2(diff.y, diff.x) * (180 / PI);
	player.setRotation(angle);
	playerSprite.setRotation(angle);
	// dive/roll in progress if needed
	if (divingMovement != sf::Vector2f(0.0f, 0.0f)) {
		player.move(divingMovement);
		playerSprite.move(divingMovement);
		divingMovement = sf::Vector2f(divingMovement.x * diveResistance, divingMovement.y * diveResistance);
		if (abs(divingMovement.x) < divingAccuracy && abs(divingMovement.y) < divingAccuracy) {
			divingMovement = sf::Vector2f(0.0f, 0.0f);
		}
		return;
	}
	// beginning of dive/roll
	if (releasedKey != sf::Keyboard::Unknown && diveResetTimer.getElapsedTime().asSeconds() < diveResetTime) {
		if (releasedKey == moveUpKey && lastKey == moveUpKey) {
			divingMovement = sf::Vector2f(0.0f, -diveSpeed); // up
		}
		else if (releasedKey == moveDownKey && lastKey == moveDownKey) {
			divingMovement = sf::Vector2f(0.0f, diveSpeed); // down
		}
		else if (releasedKey == moveLeftKey && lastKey == moveLeftKey) {
			divingMovement = sf::Vector2f(-diveSpeed, 0.0f); // left
		}
		else if (releasedKey == moveRightKey && lastKey == moveRightKey) {
			divingMovement = sf::Vector2f(diveSpeed, 0.0f); // right
		}
		lastKey = releasedKey;
		diveResetTimer.restart();
		return;
	}
	else if (diveResetTimer.getElapsedTime().asSeconds() >= diveResetTime) {
		lastKey = sf::Keyboard::Unknown;
		diveResetTimer.restart();
	}
	// normal movement
	if (sf::Keyboard::isKeyPressed(moveUpKey)) {
		player.move(sf::Vector2f(0.0f, -charSpeed)); // up
		playerSprite.move(sf::Vector2f(0.0f, -charSpeed)); // up
	}
	if (sf::Keyboard::isKeyPressed(moveDownKey)) {
		player.move(sf::Vector2f(0.0f, charSpeed)); // down
		playerSprite.move(sf::Vector2f(0.0f, charSpeed)); // down
	}
	if (sf::Keyboard::isKeyPressed(moveLeftKey)) {
		player.move(sf::Vector2f(-charSpeed, 0.0f)); // left
		playerSprite.move(sf::Vector2f(-charSpeed, 0.0f)); // left
	}
	if (sf::Keyboard::isKeyPressed(moveRightKey)) {
		player.move(sf::Vector2f(charSpeed, 0.0f)); // right
		playerSprite.move(sf::Vector2f(charSpeed, 0.0f)); // right
	}
}

sf::Packet Character::chainDataToPacket(sf::Packet & packet, std::string value) {
	return (packet << value << maxHealth << health << damage << player.getPosition().x << player.getPosition().y) ? packet : sf::Packet();
}

sf::Packet Character::extractPacketToData(sf::Packet & packet) {
	sf::Vector2f pos;
	packet >> maxHealth >> health >> damage >> pos.x >> pos.y;
	player.setPosition(pos);
	return packet;
}