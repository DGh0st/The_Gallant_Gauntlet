#pragma once

#ifndef RANGER_H
#define RANGER_H

#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>
#include "Character.h"

class Ranger : public Character {
public:
	struct Arrow {
		//default arrow constructor
		Arrow();
		//constructor
		Arrow(sf::RenderWindow & window, sf::Texture & arrowTexture);
		//sprite of arrow, created using "arrowTexture"
		sf::Sprite arrowSprite;
		//start position based on player position
		sf::Vector2f startPos;
		//mouse position when arrow was created
		sf::Vector2i mousePos;
	};
	//constructor that creates the ranger
	//@param arrowTexture: texture to use for arrows
	//@param arrowSpeed: speed of arrows shot
	Ranger(sf::Texture & rangerTexture, sf::Texture & arrowTexture, float arrowSpeed = 10.0f);
	//draws ranger
	void draw(sf::RenderWindow & window);
	//shoots arrow on left click
	void shoot(sf::RenderWindow & window);
	//draws and moves all arrows
	void drawArrows(sf::RenderWindow & window);
private:
	sf::Sprite rangerSprite; //sprite of ranger
	float arrowSpeed; //speed of arrows shot
	std::vector<Arrow*> arrows; //stores all arrows

								//--ARROW--
								//creates arrows, sets position and rotation, and inserts into "arrows" vector
	void createArrow(sf::RenderWindow & window);
	//checks if arrow is on screen (called by "drawArrows")
	//@param arrow: arrow to check
	//@return bool: true if arrow is on screen, false if arrow is not on screen
	bool Ranger::arrowOnScreen(Arrow * arrow);
	//called by "drawArrows" to move all arrows forward
	void moveArrow(Arrow * arrow);
	sf::Texture arrowTexture; //texture for arrow
	sf::Clock clock; //used for arrow shooting

};

#endif