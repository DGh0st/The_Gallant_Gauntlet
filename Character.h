#ifndef CHARACTER_H
#define CHARACTER_H
#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>

class Character {
public:
	// constructor that creates the player
	// @param maxHealth: health that character starts with
	// @param damage: damage that the character can do to enemy
	Character(int maxHealth = 100, int damage = 10, float divingAccuracy = 0.05f, float charSpeed = 1.5f, float diveSpeed = 25.0f, float diveResetTime = 0.5f, float diveResistance = 0.8f);
	// destructor
	~Character();
	// equal operator
	// @param other: the other character to copy from
	void operator=(const Character & other);
	// draw the character using sfml
	// @param window: the window to draw into
	void draw(sf::RenderWindow & window) const;
	// character movement
	void move(const sf::RenderWindow & window, const sf::Keyboard::Key releasedKey);
	// chain packets of type character
	sf::Packet chainDataToPacket(sf::Packet & packet);
	// extract packets of type character
	sf::Packet extractPacketToData(sf::Packet & packet);
protected:
	sf::RectangleShape player; // shape of the character
	sf::Sprite playerSprite; //sprite of character
	float charSpeed; // normal character movement speed
private:
	sf::Int16 maxHealth, health, damage; // health and damage
	sf::Clock diveResetTimer; // reset timer for diving in specific direction
	sf::Vector2f divingMovement; // movement of the character when diving
	sf::Keyboard::Key lastKey; // last key released (W, A, S, D) required for diving
	float divingAccuracy; // stop diving once movement is less than this
	float diveSpeed; // character dive speed
	float diveResetTime; // time required to reset the dive key (cooldown on diving)
	float diveResistance; // resistance when diving (simply to not make it jump in one frame)
};

#endif // !CHARACTER_H