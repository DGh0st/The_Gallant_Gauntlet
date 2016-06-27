#ifndef KNIGHT_H
#define KNIGHT_H

#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>
#include "Character.h"

class Knight : public Character {
public:
	//constructor that creates the knight
	Knight(sf::Texture & healthBarForegroundTexture, sf::Texture & healthBarBackgroundTexture, sf::Texture & knightTexture, sf::Texture & swordTexture, float moveSpeed = 1.5f, int maxHealth = 100.0f, int damage = 10.0f);
	//draws sword using "swordSprite"
	void draw(sf::RenderWindow & window);
	//swing and rotate sword
	void swingSword();
	//collision between sword and player
	//@param player: circle to test if intersecting with sword
	//@return: true if sword and player are intersecting, false otherwise
	bool collisionSP(sf::CircleShape & player);
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
	sf::CircleShape swordCircles[3]; //bounding circles for sword
	bool swordIntersected = false; //whether or not sword already hit something this swing
	sf::Uint16 uniqueSwingCounter = 0; // number of swings so far by player
};

#endif