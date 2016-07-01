#include "Defines.h"
#include "Knight.h"

Knight::Knight(sf::Texture & healthBarForegroundTexture, sf::Texture & healthBarBackgroundTexture, sf::Texture & knightTexture, 
	sf::Texture & swordTexture, float moveSpeed, int maxHealth, int damageMin, int damageMax) : Character(healthBarForegroundTexture, 
	healthBarBackgroundTexture,maxHealth, damageMin), damageMin(damageMin), damageMax(damageMax) {
	playerSprite = sf::Sprite(knightTexture);
	playerSprite.setOrigin(playerSprite.getGlobalBounds().width / 2, playerSprite.getGlobalBounds().height / 2);
	playerSprite.setPosition(sf::Vector2f((float)(rand() % (550 + 550 + 1) - 550), (float)(rand() % (550 + 550 + 1) - 550)));
	swordSprite = sf::Sprite(swordTexture);
	swordSprite.setOrigin((swordSprite.getGlobalBounds().width / 2)-30, swordSprite.getGlobalBounds().height / 2);
	charSpeed = moveSpeed;
	swordCircle = sf::CircleShape(60.0f);
	swordCircle.setOrigin(swordCircle.getRadius(), swordCircle.getRadius());
	//swordRect = sf::RectangleShape(sf::Vector2f(100.0f, 30.0f));
	swordCircles[0] = sf::CircleShape(16.0f);
	swordCircles[1] = sf::CircleShape(16.0f);
	swordCircles[2] = sf::CircleShape(16.0f);
	/*swordCircles[0].setFillColor(sf::Color::White); //for drawing sword collision circles
	swordCircles[1].setFillColor(sf::Color::White);
	swordCircles[2].setFillColor(sf::Color::White);*/
	swordCircles[0].setOrigin(0.0f, swordCircles[0].getRadius());
	swordCircles[1].setOrigin(-swordCircles[1].getGlobalBounds().width, swordCircles[1].getRadius());
	swordCircles[2].setOrigin(-swordCircles[2].getGlobalBounds().width * 2.0f, swordCircles[2].getRadius());
}

Knight::~Knight() {
	// nothing to clean up as of yet...
}

void Knight::draw(sf::RenderWindow & window) {
	swordCircles[0].setPosition(swordSprite.getPosition());
	swordCircles[1].setPosition(swordSprite.getPosition());
	swordCircles[2].setPosition(swordSprite.getPosition());
	swordCircles[0].setRotation(swordSprite.getRotation());
	swordCircles[1].setRotation(swordSprite.getRotation());
	swordCircles[2].setRotation(swordSprite.getRotation());
	/*window.draw(swordCircles[0]);
	window.draw(swordCircles[1]);
	window.draw(swordCircles[2]);*/
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
		uniqueSwingCounter++;
		isSwinging = true;
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
	sf::FloatRect circle1Box = swordCircles[0].getGlobalBounds();
	sf::FloatRect circle2Box = swordCircles[1].getGlobalBounds();
	sf::FloatRect circle3Box = swordCircles[2].getGlobalBounds();
	//if not swinging, we don't want collision to register so return false
	//if sword already intersected, return false to prevent multiple intersections on one swing (light, quick check)
	if (!isSwinging || swordIntersected) {
		return false;
	}
	//full collision check between sword circles and circle of player
	bool ret = false;
	sf::Vector2f playerCenter = sf::Vector2f(player.getGlobalBounds().left + player.getGlobalBounds().width / 2.0f, player.getGlobalBounds().top + player.getGlobalBounds().height / 2.0f);
	// circle 1
	if (circle1Box.intersects(player.getGlobalBounds())) {
		sf::Vector2f circle1Center = sf::Vector2f(circle1Box.left + circle1Box.width / 2.0f, circle1Box.top + circle1Box.height / 2.0f);
		float dx = (playerCenter.x - circle1Center.x) * (playerCenter.x - circle1Center.x);
		float dy = (playerCenter.y - circle1Center.y) * (playerCenter.y - circle1Center.y);
		ret = dx + dy < std::pow(player.getRadius() + swordCircles[0].getRadius(), 2);
		if (ret) {
			swordIntersected = true;
			return true;
		}
	}
	// circle 2
	if (circle2Box.intersects(player.getGlobalBounds())) {
		sf::Vector2f circle2Center = sf::Vector2f(circle2Box.left + circle2Box.width / 2.0f, circle2Box.top + circle2Box.height / 2.0f);
		float dx = (playerCenter.x - circle2Center.x) * (playerCenter.x - circle2Center.x);
		float dy = (playerCenter.y - circle2Center.y) * (playerCenter.y - circle2Center.y);
		ret = dx + dy < std::pow(player.getRadius() + swordCircles[1].getRadius(), 2);
		if (ret) {
			swordIntersected = true;
			return true;
		}
	}
	// circle 3
	if (circle3Box.intersects(player.getGlobalBounds())) {
		sf::Vector2f circle3Center = sf::Vector2f(circle3Box.left + circle3Box.width / 2.0f, circle3Box.top + circle3Box.height / 2.0f);
		float dx = (playerCenter.x - circle3Center.x) * (playerCenter.x - circle3Center.x);
		float dy = (playerCenter.y - circle3Center.y) * (playerCenter.y - circle3Center.y);
		ret = dx + dy < std::pow(player.getRadius() + swordCircles[2].getRadius(), 2);
		if (ret) {
			swordIntersected = true;
			return true;
		}
	}
	return false;
}

sf::Packet Knight::chainDataToPacket(sf::Packet & packet, std::string value) {
	return (packet << value << "Knight" << maxHealth << health << damage << playerSprite.getPosition().x << playerSprite.getPosition().y << playerSprite.getRotation() << uniqueSwingCounter) ? packet : sf::Packet();
}

sf::Packet Knight::extractPacketToData(sf::Packet & packet) {
	sf::Vector2f pos;
	float rotation;
	sf::Uint16 swingCount;
	packet >> maxHealth >> health >> damage >> pos.x >> pos.y >> rotation >> swingCount;
	playerSprite.setPosition(pos);
	playerSprite.setRotation(rotation);
	if (justAdded || swingCount < uniqueSwingCounter) {
		uniqueSwingCounter = swingCount;
		justAdded = false;
	}
	else if (swingCount > uniqueSwingCounter) {
		uniqueSwingCounter++;
		isSwinging = true;
		clockSwingTime.restart();
	}
	return packet;
}

sf::Int16 Knight::getDamage() {
	damage = rand() % (damageMax - damageMin + 1) + damageMin; // rand()%(max-min + 1) + min;
	return damage;
}