#ifndef RANGER_H
#define RANGER_H

#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>
#include "ProjectileShooter.h"

class Ranger : public ProjectileShooter {
public:
	//constructor that creates the ranger
	//@param rangerTexture: texture for ranger
	//@param bowTexture: texture for bow
	//@param arrowTextureA: texture for first animation frame of arrow
	//@param arrowTextureB: texture for second animation frame of arrow (same texture as A if no animation for arrow)
	//@param moveSpeed: movement speed of ranger
	//@param arrowSpeed: speed of arrows shot
	//@param arrowReloadTime: time before you can shoot another arrow
	//@param timeAfterShot: amount of time you want slowSpeed to last
	//@param slowSpeed: movement speed when shooting
	//@param fromWeapon: whether fireball shoots from weapon (or from player)
	Ranger(sf::Texture & healthBarForegroundTexture, sf::Texture & healthBarBackgroundTexture, sf::Texture & rangerTexture, 
		sf::Texture & bowTexture, sf::Texture & arrowTextureA, sf::Texture & arrowTextureB, float moveSpeed = 1.5f,	float arrowSpeed = 1.0f, 
		float arrowReloadTime = 0.3f, float timeAfterShot = 1.0f, float slowSpeed = 0.1f, bool fromWeapon = false);
	//movement for ranger
	void move(const sf::RenderWindow & window, const sf::Keyboard::Key releasedKey);
	// chain packets of type Knight
	sf::Packet chainDataToPacket(sf::Packet & packet, std::string value);
	// extract packets of type Knight
	sf::Packet extractPacketToData(sf::Packet & packet);

private:
	float moveSpeed; //regular movement speed
	float slowSpeed; //movement speed when shooting arrows
};

#endif
