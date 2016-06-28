#include "Map.h"
#include "Character.h"
#include "ProjectileShooter.h"

Map::Map(sf::Texture & mapTexture, sf::Vector2f startPos) {
	//set up map sprite
	mapSprite = sf::Sprite(mapTexture);
	mapSprite.setOrigin(mapSprite.getGlobalBounds().width / 2.0f, mapSprite.getGlobalBounds().height / 2.0f);
	mapSprite.setPosition(startPos);

	//set up four main walls of map
	//leftRect
	leftRect = sf::RectangleShape(sf::Vector2f(40.0f, 1300.0f));
	leftRect.setOrigin(leftRect.getGlobalBounds().width / 2.0f, leftRect.getGlobalBounds().height / 2.0f);
	leftRect.setFillColor(sf::Color::White);
	leftRect.setPosition(sf::Vector2f(startPos.x - mapSprite.getGlobalBounds().width / 2.0f + 90.0f, startPos.y));
	//rightRect
	rightRect = sf::RectangleShape(sf::Vector2f(40.0f, 1300.0f));
	rightRect.setOrigin(rightRect.getGlobalBounds().width / 2.0f, rightRect.getGlobalBounds().height / 2.0f);
	rightRect.setFillColor(sf::Color::White);
	rightRect.setPosition(sf::Vector2f(startPos.x + mapSprite.getGlobalBounds().width / 2.0f - 90.0f, startPos.y));
	//topRect
	topRect = sf::RectangleShape(sf::Vector2f(1300.0f, 40.0f));
	topRect.setOrigin(topRect.getGlobalBounds().width / 2.0f, topRect.getGlobalBounds().height / 2.0f);
	topRect.setFillColor(sf::Color::White);
	topRect.setPosition(sf::Vector2f(startPos.x, startPos.y - mapSprite.getGlobalBounds().height / 2.0f + 90.0f));
	//bottomRect
	bottomRect = sf::RectangleShape(sf::Vector2f(1300.0f, 40.0f));
	bottomRect.setOrigin(bottomRect.getGlobalBounds().width / 2.0f, bottomRect.getGlobalBounds().height / 2.0f);
	bottomRect.setFillColor(sf::Color::White);
	bottomRect.setPosition(sf::Vector2f(startPos.x, startPos.y + mapSprite.getGlobalBounds().height / 2.0f - 90.0f));

	//sets position for each wall of each pillar
	setPosPillars(startPos);
}

void Map::setPosPillars(sf::Vector2f startPos) {
	for (int i = 0; i < 4; i++) {
		sf::Vector2f pillarStartL;
		sf::Vector2f pillarStartR;
		sf::Vector2f pillarStartT;
		sf::Vector2f pillarStartB;
		if (i == 0) { //top left pillar
			pillarStartL = sf::Vector2f(389.0f, 271.5f);
			pillarStartR = sf::Vector2f(248.0f, 271.5f);
			pillarStartT = sf::Vector2f(318.5f, 342.0f);
			pillarStartB = sf::Vector2f(318.5f, 201.0f);
		}
		else if (i == 1) { //top right pillar
			pillarStartL = sf::Vector2f(-209.0f, 278.5f);
			pillarStartR = sf::Vector2f(-350.0f, 278.5f);
			pillarStartT = sf::Vector2f(-279.5f, 349.0f);
			pillarStartB = sf::Vector2f(-279.5f, 208.0f);
		}
		else if (i == 2) { //bottom left pillar
			pillarStartL = sf::Vector2f(390.0f, -304.5f);
			pillarStartR = sf::Vector2f(249.0f, -304.5f);
			pillarStartT = sf::Vector2f(319.5f, -234.0f);
			pillarStartB = sf::Vector2f(319.5f, -374.0f);
		}
		else if (i == 3) { //bottom right pillar
			pillarStartL = sf::Vector2f(-212.0f, -303.5f);
			pillarStartR = sf::Vector2f(-353.0f, -303.5f);
			pillarStartT = sf::Vector2f(-282.5f, -233.0f);
			pillarStartB = sf::Vector2f(-282.5f, -374.0f);
		}
		for (int j = 0; j < 4; j++) {
			sf::RectangleShape & curRect = pillarRects[i * 4 + j];
			//curRect.setFillColor(sf::Color::White); add for drawing
			if (j == 0) { //left rect
				curRect = sf::RectangleShape(sf::Vector2f(15.0f, 137.5f));
				curRect.setOrigin(curRect.getGlobalBounds().width / 2.0f, curRect.getGlobalBounds().height / 2.0f);
				curRect.setPosition(sf::Vector2f(startPos.x - pillarStartL.x, startPos.y - pillarStartL.y));
			}
			else if (j == 1) { //right rect
				curRect = sf::RectangleShape(sf::Vector2f(15.0f, 137.5f));
				curRect.setOrigin(curRect.getGlobalBounds().width / 2.0f, curRect.getGlobalBounds().height / 2.0f);
				curRect.setPosition(sf::Vector2f(startPos.x - pillarStartR.x, startPos.y - pillarStartR.y));
			}
			else if (j == 2) { //top rect
				curRect = sf::RectangleShape(sf::Vector2f(138.0f, 15.0f));
				curRect.setOrigin(curRect.getGlobalBounds().width / 2.0f, curRect.getGlobalBounds().height / 2.0f);
				curRect.setPosition(sf::Vector2f(startPos.x - pillarStartT.x, startPos.y - pillarStartT.y));
			}
			else if (j == 3) { //bottom rect
				curRect = sf::RectangleShape(sf::Vector2f(138.0f, 15.0f));
				curRect.setOrigin(curRect.getGlobalBounds().width / 2.0f, curRect.getGlobalBounds().height / 2.0f);
				curRect.setPosition(sf::Vector2f(startPos.x - pillarStartB.x, startPos.y - pillarStartB.y));
			}
		}
	}
}

void Map::draw(sf::RenderWindow & window) {
	window.draw(mapSprite);
}

void Map::collisionMProj(ProjectileShooter ps) {

	std::vector<sf::CircleShape*> projectiles = ps.getProjectileCircles();
	for (int i = 0; i < projectiles.size(); i++) {
		sf::CircleShape & curProjectile = *(projectiles[i]);
		if (collisionMPHelper(curProjectile, leftRect)) {
			ps.setHitWall(i);
			continue;
		}
		else if (collisionMPHelper(curProjectile, rightRect)) {
			ps.setHitWall(i);
			continue;
		}
		else if (collisionMPHelper(curProjectile, topRect)) {
			ps.setHitWall(i);
			continue;
		}
		else if (collisionMPHelper(curProjectile, bottomRect)) {
			ps.setHitWall(i);
			continue;
		}
		for (int j = 0; j < 16; j++) {
			if (collisionMPHelper(curProjectile, pillarRects[j])) {
				ps.setHitWall(i);
				break;
			}
		}
	}
}

void Map::collisionMP(Character & player) {
	//four main walls of map
	bool hitLeft = collisionMPHelper(player.getCollisionCircle(), leftRect);
	bool hitRight = collisionMPHelper(player.getCollisionCircle(), rightRect);
	bool hitTop = collisionMPHelper(player.getCollisionCircle(), topRect);
	bool hitBottom = collisionMPHelper(player.getCollisionCircle(), bottomRect);
	//for every pillar, check each wall
	for (int i = 0; i < 4; i++) { //top left pillar, then top right, then bottom left, then bottom right
		for (int j = 0; j < 4; j++) {
			sf::RectangleShape & curRect = pillarRects[i + j * 4];
			if (i == 0) { //left wall
				if (hitRight) {
					break;
				}
				hitRight = collisionMPHelper(player.getCollisionCircle(), curRect);
			}
			else if (i == 1) { //right wall
				if (hitLeft) {
					break;
				}
				hitLeft = collisionMPHelper(player.getCollisionCircle(), curRect);
			}
			else if (i == 2) {
				if (hitBottom) { //top wall
					break;
				}
				hitBottom = collisionMPHelper(player.getCollisionCircle(), curRect);
			}
			else if (i == 3) { //bottom wall
				if (hitTop) {
					break;
				}
				hitTop = collisionMPHelper(player.getCollisionCircle(), curRect);
			}
		}
	}

	float moveX = 0;
	float moveY = 0;
	//check for walls on left or right
	if (hitLeft) { //there's a wall on left, so move right
		moveX = player.getCharSpeed();
	}
	else if (hitRight) { //there's a wall on right, so move left
		moveX = -player.getCharSpeed();
	}
	//check for walls above or below
	if (hitTop) { //there's a wall above, so move down
		moveY = player.getCharSpeed();
	}
	else if (hitBottom) { //there's a wall below, move up
		moveY = -player.getCharSpeed();
	}
	//move player back by amount they're moving (charSpeed) to prevent traveling through walls
	(player.getPlayerSprite())->move(sf::Vector2f(moveX, moveY));
}

bool Map::collisionMPHelper(sf::CircleShape & player, sf::RectangleShape mapRect) {
	sf::FloatRect mapBox = mapRect.getGlobalBounds();
	//if mapBox and circle encompassing player circle don't intersect, no collision possible (light, quick check)
	if (!mapBox.intersects(player.getGlobalBounds())) {
		return false;
	}
	//full collision check between map and circle of player
	float circleX = player.getPosition().x;
	float closestX;
	if (circleX < mapBox.left) { //circle to left of rect
		closestX = mapBox.left;
	}
	else if (circleX > mapBox.left + mapBox.width) { //circle to right of rect
		closestX = mapBox.left + mapBox.width;
	}
	else {
		closestX = circleX;
	}

	float circleY = player.getPosition().y;
	float closestY;
	if (circleY < mapBox.top) { //circle above rect
		closestY = mapBox.top;
	}
	else if (circleY > mapBox.top + mapBox.height) { //circle below rect
		closestY = mapBox.top + mapBox.height;
	}
	else {
		closestY = circleY;
	}

	float dx = (circleX - closestX) * (circleX - closestX);
	float dy = (circleY - closestY) * (circleY - closestY);
	return sqrt(dx + dy) < player.getRadius();
}