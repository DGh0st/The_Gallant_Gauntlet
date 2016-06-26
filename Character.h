#ifndef CHARACTER_H
#define CHARACTER_H
#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>

class Character {
public:
	// constructor that creates the player
	// @param maxHealth: health that character starts with
	// @param damage: damage that the character can do to enemy
	Character(sf::Texture & healthBarForegroundTexture, sf::Texture & healthBarBackgroundTexture, int maxHealth = 100, int damage = 10, float divingAccuracy = 0.05f, float charSpeed = 1.5f, float diveSpeed = 25.0f, float diveResetTime = 0.5f, float diveResistance = 0.8f);
	// destructor
	~Character();
	// equal operator
	// @param other: the other character to copy from
	void operator=(const Character & other);
	// draw the character using sfml
	// @param window: the window to draw into
	void draw(sf::RenderWindow & window);
	// character movement
	void move(const sf::RenderWindow & window, const sf::Keyboard::Key releasedKey);
	// chain packets of type character
	sf::Packet chainDataToPacket(sf::Packet & packet, std::string value);
	// extract packets of type character
	sf::Packet extractPacketToData(sf::Packet & packet);
	// set isPlayer
	inline void setIsPlayer(bool isPlayer) { this->isPlayer = isPlayer; }
	// get position
	sf::Vector2f getCenter();
	// check if dead
	bool getIsDead();
	// get time as dead
	float getTimeAsDead();
	// get collision circle
	sf::CircleShape getCollisionCircle();
	// take damage
	sf::Int16 takeDamage(sf::Int16 damage);
	// get damage
	sf::Int16 getDamage();
protected:
	sf::Sprite playerSprite; //sprite of character
	float charSpeed; // normal character movement speed
	sf::Int16 maxHealth, health, damage; // health and damage
	bool isPlayer = false; // is the current Character the player or someone over the internet
	bool isDead = false; // is player dead
private:
	sf::CircleShape playerCircle = sf::CircleShape(); //circle used for player collisions
	sf::Clock diveResetTimer; // reset timer for diving in specific direction
	sf::Vector2f divingMovement; // movement of the character when diving
	sf::Keyboard::Key lastKey; // last key released (W, A, S, D) required for diving
	float divingAccuracy; // stop diving once movement is less than this
	float diveSpeed; // character dive speed
	float diveResetTime; // time required to reset the dive key (cooldown on diving)
	float diveResistance; // resistance when diving (simply to not make it jump in one frame)
	sf::Clock respawnTimer = sf::Clock(); // respawn timer
	sf::Sprite healthBarForegroundSprite, healthBarBackgroundSprite; // health bar
};

#endif // !CHARACTER_H