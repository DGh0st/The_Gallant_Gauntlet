#ifndef MAP_H
#define MAP_H

#include <SFML/Graphics.hpp>
#include "Character.h"

class Map {
public:
	//constructor that creates the map
	//@param mapTexture: texture of map to draw
	//@param startPos: start position of map
	Map(sf::Texture & mapTexture, sf::Vector2f startPos);
	//collision between map and player
	//@param player: Character to test if intersecting with map
	void collisionMP(Character & player);
	//draw map
	void draw(sf::RenderWindow & window);
private:
	//set position of pillar walls based on start position
	void setPosPillars(sf::Vector2f startPos);

	//helper function that checks if circle of player intersects with map rectangle (effectively checks intersection for one wall)
	//@param player: circle to test if intersecting with mapRect
	//@param mapRect: wall to test if intersecting with player
	//@return: true if player and mapRect are intersecting, false otherwise
	bool collisionMPHelper(sf::CircleShape & player, sf::RectangleShape mapRect);

	sf::Sprite mapSprite; //sprite of map to draw
						  //four main walls of map
	sf::RectangleShape leftRect;
	sf::RectangleShape rightRect;
	sf::RectangleShape topRect;
	sf::RectangleShape bottomRect;
	//four walls for each pillar
	sf::RectangleShape pillarRects[16];
};

#endif