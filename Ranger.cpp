#include "Ranger.h"
#include "Defines.h"

Ranger::Ranger(sf::Texture & rangerTexture,sf::Texture & arrowTexture, float arrowSpeed) : arrowTexture(arrowTexture),arrowSpeed(arrowSpeed) {
	//rangerSprite = sf::Sprite(rangerTexture);
	playerSprite = sf::Sprite(rangerTexture);
	playerSprite.setOrigin(playerSprite.getGlobalBounds().width / 2, playerSprite.getGlobalBounds().height / 2);
}

void Ranger::draw(sf::RenderWindow & window) {
	//window.draw(rangerSprite);
	window.draw(playerSprite);
}

void Ranger::shoot(sf::RenderWindow & window) {
	float shootWaitTime = 0.3f; //time before you can shoot another arrow (reload time)
	if (sf::Mouse::isButtonPressed(sf::Mouse::Left) && clock.getElapsedTime().asSeconds() > shootWaitTime) { //shoot if left mouse click and shootWaitTime went by (stops spam shooting)
		createArrow(window);
		clock.restart();
	}
}

void Ranger::createArrow(sf::RenderWindow & window) {
	Arrow * arrow = new Arrow(window,arrowTexture);
	arrow->arrowSprite = sf::Sprite(arrowTexture);
	arrow->startPos = playerSprite.getPosition();
	arrow->arrowSprite.setPosition(arrow->startPos);
	arrow->arrowSprite.setRotation(player.getRotation());
	arrows.insert(arrows.begin(), arrow); //store arrow
}

void Ranger::drawArrows(sf::RenderWindow & window) {
	for (int i = 0; i < arrows.size(); i++) {
		//if arrow exists and on screen
		if (arrows[i] != NULL && arrowOnScreen(arrows[i])) {
			window.draw(arrows[i]->arrowSprite);
			moveArrow(arrows[i]);
		}
		else {
			delete arrows[i];
			arrows[i] = NULL;
			arrows.erase(arrows.begin() + i);  //remove NULL from vector
		}
	}
}

bool Ranger::arrowOnScreen(Arrow * arrow) {
	return ((arrow->arrowSprite.getPosition().y + arrow->arrowSprite.getLocalBounds().height) >= 0) //up
		&& ((arrow->arrowSprite.getPosition().y - arrow->arrowSprite.getLocalBounds().height) <= windowSize.y) //down
		&& ((arrow->arrowSprite.getPosition().x + arrow->arrowSprite.getLocalBounds().width) >= 0) //left
		&& ((arrow->arrowSprite.getPosition().x - arrow->arrowSprite.getLocalBounds().width) <= windowSize.x); //right
}

void Ranger::moveArrow(Arrow * arrow) {
	float dx = arrow->mousePos.x - arrow->startPos.x;
	float dy = arrow->mousePos.y - arrow->startPos.y;

	float norm = sqrt(dx*dx + dy*dy);
	dx = arrowSpeed * dx / norm;
	dy = arrowSpeed * dy / norm;
	arrow->arrowSprite.move(dx, dy);
}

//--ARROW--

Ranger::Arrow::Arrow() {
	
}

Ranger::Arrow::Arrow(sf::RenderWindow & window, sf::Texture & arrowTexture) {
	arrowSprite = sf::Sprite(arrowTexture);
	arrowSprite.setOrigin(arrowSprite.getGlobalBounds().width / 2, arrowSprite.getGlobalBounds().height / 2); //set origin to center
	mousePos = sf::Mouse::getPosition(window);
}

