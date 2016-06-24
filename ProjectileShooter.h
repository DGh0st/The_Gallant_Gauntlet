#pragma once

#ifndef ProjectileShooter_H
#define ProjectileShooter_H

#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>
#include "Character.h"

class ProjectileShooter : public Character {
public:
	struct Projectile {
		// default projectile constructor
		Projectile();
		// constructor for player
		Projectile(sf::RenderWindow & window, sf::Texture & projectileTexture, sf::Sprite playerSprite);
		// constructor for internet player
		Projectile(sf::Texture & projectileTexture, sf::Sprite playerSprite);
		//sprite of projectile, created using "projectileTexture"
		sf::Sprite projectileSprite;
		//start position based on player position
		sf::Vector2f startPos;
		//mouse position when projectile was created
		sf::Vector2i mousePos;
		bool trigger; //used for animation of projectile (switch between frames)
		sf::Clock clockAnim; //used for animation of projectile

	};
	//constructor that creates the projectileShooter
	//@param projectileShooterTexture: texture for projectileShooter
	//@param projectileTextureA: texture for first animation frame of projectile
	//@param projectileTextureB: texture for second animation frame of projectile (same texture as A if no animation for projectile)
	//@param projectileSpeed: speed of projectiles shot
	//@param projectileReloadTime: time before you can shoot another projectile
	ProjectileShooter(sf::Texture & projectileShooterTexture, sf::Texture & projectileTextureA, sf::Texture & projectileTextureB,
		float projectileSpeed = 0.25f, float projectileReloadTime = 0.3f, float timeAfterShot = 1.0f);
	//draws projectileShooter
	void draw(sf::RenderWindow & window);
	//shoots projectile on left click, and updates "isShooting" based on "timeAfterShot"
	void shoot(sf::RenderWindow & window);
	//draws and moves all projectiles
	void drawProjectiles(sf::RenderWindow & window);
protected:
	bool isShooting; //true if shot and time within "timeAfterShot" seconds
	float timeAfterShot; //amount of time you want slow to last
private:
	sf::Clock clockProjectile; //used for checking time since projectile was fired
	sf::Texture projectileTextureA; //texture for first animation frame of projectile
	sf::Texture projectileTextureB; //texture for second animation frame of projectile
	std::vector<Projectile*> projectiles; //stores all projectiles
	float projectileSpeed; //speed of projectiles shot
	float projectileReloadTime; //time before you can shoot another projectile

	//--PROJECTILE--
	//checks if projectile is on screen (called by "drawProjectiles")
	//@param projectile: projectile to check
	//@return bool: true if projectile is on screen, false if projectile is not on screen
	bool projectileOnScreen(Projectile * projectile);
	//called by "drawProjectiles" to move all projectiles forward
	void moveProjectile(Projectile * projectile);
	sf::Clock clockReload; //used for projectile shooting

};

#endif