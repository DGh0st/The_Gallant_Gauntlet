#include <math.h>
#include <iostream>
#include <fstream>
#include <stdlib.h>

#include "Character.h"
#include "Defines.h"

Character::Character(int maxHealth, int damage, float divingAccuracy, float charSpeed, float diveSpeed, float diveResetTime, float diveResistance) : maxHealth(maxHealth), health(maxHealth), damage(damage), divingAccuracy(divingAccuracy), charSpeed(charSpeed), diveSpeed(diveSpeed), diveResetTime(diveResetTime), diveResistance(diveResistance) {
	divingMovement = sf::Vector2f(0.0f, 0.0f);
	lastKey = sf::Keyboard::Unknown;
	diveResetTimer.restart();
	playerCircle = sf::CircleShape(46.0f);
	playerCircle.setOrigin(playerCircle.getRadius(), playerCircle.getRadius());
}

Character::~Character() {
	// nothing to clean up as of yet...
}

void Character::operator=(const Character & other) {
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
	window.draw(playerSprite);	
}

void Character::move(const sf::RenderWindow & window, const sf::Keyboard::Key releasedKey) {
	// increase death counter if needed
	if (isPlayer && health <= 0) {
		deaths++;
	}
	// player rotation
	sf::Vector2i mousePos = (sf::Vector2i)window.mapPixelToCoords(sf::Mouse::getPosition(window));
	sf::Vector2i charPos = (sf::Vector2i)playerSprite.getPosition();
	sf::Vector2i diff = mousePos - charPos;
	float angle = (float)atan2(diff.y, diff.x) * (180 / PI);
	playerSprite.setRotation(angle);
	// dive/roll in progress if needed
	if (divingMovement != sf::Vector2f(0.0f, 0.0f)) {
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
		playerSprite.setPosition(playerSprite.getPosition() + sf::Vector2f(0.0f, -charSpeed)); // up
	}
	if (sf::Keyboard::isKeyPressed(moveDownKey)) {
		playerSprite.setPosition(playerSprite.getPosition() + sf::Vector2f(0.0f, charSpeed)); // down
	}
	if (sf::Keyboard::isKeyPressed(moveLeftKey)) {
		playerSprite.setPosition(playerSprite.getPosition() + sf::Vector2f(-charSpeed, 0.0f)); // left
	}
	if (sf::Keyboard::isKeyPressed(moveRightKey)) {
		playerSprite.setPosition(playerSprite.getPosition() + sf::Vector2f(charSpeed, 0.0f)); // right
	}

	//keep player circle on top of player
	playerCircle.setPosition(playerSprite.getPosition());
}

sf::Vector2f Character::getCenter() {
	return sf::Vector2f(playerSprite.getGlobalBounds().left + playerSprite.getGlobalBounds().width / 2.0f, playerSprite.getGlobalBounds().top + playerSprite.getGlobalBounds().height / 2.0f);
}

sf::Packet Character::chainDataToPacket(sf::Packet & packet, std::string value) {
	return (packet << value << "Character" << maxHealth << health << damage << playerSprite.getPosition().x << playerSprite.getPosition().y << playerSprite.getRotation()) ? packet : sf::Packet();
}

sf::Packet Character::extractPacketToData(sf::Packet & packet) {
	sf::Vector2f pos;
	float rotation;
	packet >> maxHealth >> health >> damage >> pos.x >> pos.y >> rotation;
	playerSprite.setPosition(pos);
	playerSprite.setRotation(rotation);
	return packet;
}