#include "Defines.h"
#include "Knight.h"

Knight::Knight(sf::Texture & knightTexture, sf::Texture & swordTexture,float moveSpeed, int maxHealth, int damage) : Character(maxHealth, damage) {
	playerSprite = sf::Sprite(knightTexture);
	playerSprite.setOrigin(playerSprite.getGlobalBounds().width / 2, playerSprite.getGlobalBounds().height / 2);
	swordSprite = sf::Sprite(swordTexture);
	swordSprite.setOrigin((swordSprite.getGlobalBounds().width / 2)-30, swordSprite.getGlobalBounds().height / 2);
	charSpeed = moveSpeed;
	swordCircle = sf::CircleShape(60.0f);
	swordCircle.setOrigin(swordCircle.getRadius(), swordCircle.getRadius());
}

void Knight::drawSword(sf::RenderWindow & window) {
	window.draw(swordSprite);
}

void Knight::swingSword() {
	//have swordCircle copy playerSprite in position and rotation
	swordCircle.setPosition(playerSprite.getPosition());
	swordCircle.setRotation(playerSprite.getRotation());
	//if not swinging, keep sword by side of playerSprite (use swordCircle to prevent sword thrusting on 45° angles, while keeping size of playerSprite smaller than swordCircle)
	if (!isSwinging) {
		float x = swordCircle.getPosition().x + (swordCircle.getRadius())*cos(swordCircle.getRotation()* (PI / 180) + PI / 3);
		float y = swordCircle.getPosition().y + (swordCircle.getRadius())*sin(swordCircle.getRotation()* (PI / 180) + PI / 3);
		swordSprite.setPosition(x, y);
		swordSprite.setRotation(swordCircle.getRotation() + 15);
	}
	//if left click and swingTime(time swinging sword)+reloadTime(time before sword can swing again) went by, then swing sword
	if (isPlayer && sf::Mouse::isButtonPressed(sf::Mouse::Left) && (clockSwingTime.getElapsedTime().asSeconds() > reloadTime + swingTime)) {
		isSwinging = true;
		clockSwingTime.restart();
	}
	//if swinging and swingTime went by, then done swinging
	if (isSwinging && clockSwingTime.getElapsedTime().asSeconds() > swingTime) {
		isSwinging = false;
	}
	else if (isSwinging) { //else if swinging, move swordSprite in arc and rotate slightly
		float x = swordCircle.getPosition().x + (swordCircle.getRadius())*cos(clockSwingTime.getElapsedTime().asSeconds() * 3 - PI / 3 - swordCircle.getRotation()*(PI / 180));
		float y = swordCircle.getPosition().y - (swordCircle.getRadius())*sin(clockSwingTime.getElapsedTime().asSeconds() * 3 - PI / 3 - swordCircle.getRotation()*(PI / 180));
		swordSprite.setPosition(x, y);
		swordSprite.setRotation(swordCircle.getRotation() + 15 - clockSwingTime.getElapsedTime().asSeconds() * 100.2f);
	}
}

sf::Packet Knight::chainDataToPacket(sf::Packet & packet, std::string value) {
	return (packet << value << "Knight" << maxHealth << health << damage << playerSprite.getPosition().x << playerSprite.getPosition().y << playerSprite.getRotation() << isSwinging << clockSwingTime.getElapsedTime().asSeconds()) ? packet : sf::Packet();
}

sf::Packet Knight::extractPacketToData(sf::Packet & packet) {
	sf::Vector2f pos;
	float rotation;
	float clockTime;
	packet >> maxHealth >> health >> damage >> pos.x >> pos.y >> rotation >> isSwinging >> clockTime;
	playerSprite.setPosition(pos);
	playerSprite.setRotation(rotation);
	if (clockTime < 0.05f) {
		clockSwingTime.restart();
	}
	return packet;
}