#ifndef ProjectileShooter_H
#define ProjectileShooter_H

#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>
#include "Character.h"

class ProjectileShooter : public Character {
public:
	struct Projectile {
		//default projectile constructor
		Projectile();
		//constructor for player projectiles
		Projectile(sf::RenderWindow & window, sf::Texture & projectileTexture, sf::Sprite startSprite);
		//constructor for internet player projectiles
		Projectile(sf::Vector2i mousePos, sf::Texture & projectileTexture, sf::Sprite startSprite);
		//sprite of projectile, created using "projectileTexture"
		sf::Sprite projectileSprite;
		//start position based on player position
		sf::Vector2f startPos;
		//mouse position when projectile was created
		sf::Vector2i mousePos;
		bool trigger; //used for animation of projectile (switch between frames)
		sf::Clock clockAnim; //used for animation of projectile
		sf::CircleShape projectileCircle; // collision circle for projectile
		bool hitWall = false;
	};
	//constructor that creates the projectileShooter
	//@param projectileShooterTexture: texture for projectileShooter
	//@param weaponTexture: texture for weapon
	//@param projectileTextureA: texture for first animation frame of projectile
	//@param projectileTextureB: texture for second animation frame of projectile (same texture as A if no animation for projectile)
	//@param projectileSpeed: speed of projectiles shot
	//@param projectileReloadTime: time before you can shoot another projectile
	//@param timeAfterShot: amount of time you want slow to last
	//@param fromWeapon: whether projectile should come from weapon (otherwise comes from player)
	ProjectileShooter(sf::Texture & healthBarForegroundTexture, sf::Texture & healthBarBackgroundTexture, sf::Texture & projectileShooterTexture, 
		sf::Texture & weaponTexture, sf::Texture & projectileTextureA, sf::Texture & projectileTextureB, float projectileSpeed = 0.25f,
		float projectileReloadTime = 0.3f, float timeAfterShot = 1.0f, bool fromWeapon = false, int maxHealth = 100, int damage = 10, float charSpeed = 1.5f);
	//draws projectileShooter
	void draw(sf::RenderWindow & window);
	//shoots projectile on left click, and updates "isShooting" based on "timeAfterShot"
	void shoot(sf::RenderWindow & window);
	//collision between projectile and player
	//@param player: circle to test if intersecting with projectile
	//@return: true if sword and player are intersecting, false otherwise
	bool collisionPP(sf::CircleShape & player);
	//draws and moves all projectiles
	void drawProjectiles(sf::RenderWindow & window);
	//sets weapon of projectileShooter based on "fromWeapon"
	void setWeapon(sf::RenderWindow & window);
	//gets projectile collision circles for use in map collisions
	std::vector<sf::CircleShape*> getProjectileCircles();
	//make hitWall for projectiles[i] true so that the projectile is deleted
	void setHitWall(int i);
protected:
	bool isShooting = false; //true if shot and time within "timeAfterShot" seconds
	float timeAfterShot; //amount of time you want slow to last
	sf::Sprite weaponSprite; //sprite for weapon
	sf::CircleShape weaponCircle; //circle that weapon rotates around
	bool fromWeapon; //whether projectile shoots from weapon (or from player)
	sf::Texture projectileTextureA; //texture for first animation frame of projectile
	sf::Texture projectileTextureB; //texture for second animation frame of projectile
	std::vector<Projectile*> projectiles; //stores all projectiles
	sf::Clock clockProjectile; //used for checking time since projectile was fired
	sf::Clock clockReload; //used for projectile shooting
	sf::Uint16 uniqueProjectilesCounter = 0; // number of projectiles shot so far by player
private:
	float projectileSpeed; //speed of projectiles shot
	float projectileReloadTime; //time before you can shoot another projectile

	//--PROJECTILE--
	//checks if projectile is on screen (called by "drawProjectiles")
	//@param projectile: projectile to check
	//@return bool: true if projectile is on screen, false if projectile is not on screen
	bool projectileOnScreen(Projectile * projectile);
	//called by "drawProjectiles" to move all projectiles forward
	void moveProjectile(Projectile * projectile);

};

#endif