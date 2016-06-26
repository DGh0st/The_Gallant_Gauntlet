#include "Defines.h"
#include "Knight.h"

Knight::Knight(sf::Texture & healthBarForegroundTexture, sf::Texture & healthBarBackgroundTexture, sf::Texture & knightTexture, sf::Texture & swordTexture,float moveSpeed, int maxHealth, int damage) : Character(healthBarForegroundTexture, healthBarBackgroundTexture,maxHealth, damage) {
	playerSprite = sf::Sprite(knightTexture);
	playerSprite.setOrigin(playerSprite.getGlobalBounds().width / 2, playerSprite.getGlobalBounds().height / 2);
	swordSprite = sf::Sprite(swordTexture);
	swordSprite.setOrigin((swordSprite.getGlobalBounds().width / 2)-30, swordSprite.getGlobalBounds().height / 2);
	charSpeed = moveSpeed;
	swordCircle = sf::CircleShape(60.0f);
	swordCircle.setOrigin(swordCircle.getRadius(), swordCircle.getRadius());
	swordRect = sf::RectangleShape(sf::Vector2f(100.0f, 30.0f));
	swordRect.setFillColor(sf::Color::White);
	swordRect.setOrigin((swordRect.getGlobalBounds().width / 2.0f)-50.0f, swordRect.getGlobalBounds().height / 2.0f);
}

void Knight::draw(sf::RenderWindow & window) {
	swordRect.setPosition(swordSprite.getPosition());
	swordRect.setRotation(swordSprite.getRotation());
	window.draw(swordRect);
	window.draw(swordSprite);
	Character::draw(window);
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
		uniqueSwingCounter++;
		clockSwingTime.restart();
	}
	//if swinging and swingTime went by, then done swinging
	if (isSwinging && clockSwingTime.getElapsedTime().asSeconds() > swingTime) {
		isSwinging = false;
		swordIntersected = false;
	}
	else if (isSwinging) { //else if swinging, move swordSprite in arc and rotate slightly
		float x = swordCircle.getPosition().x + (swordCircle.getRadius())*cos(clockSwingTime.getElapsedTime().asSeconds() * 3 - PI / 3 - swordCircle.getRotation()*(PI / 180));
		float y = swordCircle.getPosition().y - (swordCircle.getRadius())*sin(clockSwingTime.getElapsedTime().asSeconds() * 3 - PI / 3 - swordCircle.getRotation()*(PI / 180));
		swordSprite.setPosition(x, y);
		swordSprite.setRotation(swordCircle.getRotation() + 15 - clockSwingTime.getElapsedTime().asSeconds() * 100.2f);
	}
}

bool Knight::collisionSP(sf::CircleShape & player) {
	sf::FloatRect swordBox = swordRect.getGlobalBounds();
	//if not swinging, we don't want collision to register so return false
	//if sword already intersected, return false to prevent multiple intersections on one swing
	//if swordBox and rectangle encompassing player circle don't intersect, no collision possible (light, quick check)
	if (!isSwinging || swordIntersected || !swordBox.intersects(player.getGlobalBounds())) {
		return false;
	}
	//full collision check between sword and circle of player
	float circleX = player.getPosition().x;
	float closestX;
	if (circleX < swordBox.left) { //circle to left of rect
		closestX = swordBox.left;
	}
	else if (circleX > swordBox.left + swordBox.width) { //circle to right of rect
		closestX = swordBox.left + swordBox.width;
	}
	else {
		closestX = circleX;
	}

	float circleY = player.getPosition().y;
	float closestY;
	if (circleY < swordBox.top) { //circle above rect
		closestY = swordBox.top;
	}
	else if (circleY > swordBox.top + swordBox.height) { //circle below rect
		closestY = swordBox.top + swordBox.height;
	}
	else {
		closestY = circleY;
	}

	float dx = (circleX - closestX) * (circleX - closestX);
	float dy = (circleY - closestY) * (circleY - closestY);
	bool ret = sqrt(dx + dy) < player.getRadius();
	if (ret) {
		swordIntersected = true;
	}
	return ret;
}

sf::Packet Knight::chainDataToPacket(sf::Packet & packet, std::string value) {
	return (packet << value << "Knight" << maxHealth << health << damage << playerSprite.getPosition().x << playerSprite.getPosition().y << playerSprite.getRotation() << isSwinging << uniqueSwingCounter) ? packet : sf::Packet();
}

sf::Packet Knight::extractPacketToData(sf::Packet & packet) {
	sf::Vector2f pos;
	float rotation;
	sf::Uint16 swingCount;
	packet >> maxHealth >> health >> damage >> pos.x >> pos.y >> rotation >> isSwinging >> swingCount;
	playerSprite.setPosition(pos);
	playerSprite.setRotation(rotation);
	if (swingCount > uniqueSwingCounter) {
		clockSwingTime.restart();
		uniqueSwingCounter++;
	}
	return packet;
}