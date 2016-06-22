#include "ProjectileShooter.h"
#include "Defines.h"

ProjectileShooter::ProjectileShooter(sf::Texture & projectileShooterTexture, sf::Texture & projectileTextureA, sf::Texture & projectileTextureB,
	float projectileSpeed, float projectileReloadTime, float timeAfterShot) : projectileTextureA(projectileTextureA), projectileTextureB(projectileTextureB),
	projectileSpeed(projectileSpeed), projectileReloadTime(projectileReloadTime), timeAfterShot(timeAfterShot) {
	playerSprite = sf::Sprite(projectileShooterTexture);
	playerSprite.setOrigin(playerSprite.getGlobalBounds().width / 2, playerSprite.getGlobalBounds().height / 2);
}

void ProjectileShooter::draw(sf::RenderWindow & window) {
	window.draw(playerSprite);
}

void ProjectileShooter::shoot(sf::RenderWindow & window) {
	//if shooting and timeAfterShot goes by, mark isShooting as false
	if (isShooting && clockProjectile.getElapsedTime().asSeconds() > timeAfterShot) {
		isShooting = false;
		clockProjectile.restart();
	}
	//shoot if left mouse click and projectileReloadTime went by
	if (sf::Mouse::isButtonPressed(sf::Mouse::Left) && clockReload.getElapsedTime().asSeconds() > projectileReloadTime) {
		isShooting = true;
		Projectile * projectile = new Projectile(window, projectileTextureA, playerSprite);
		projectiles.insert(projectiles.begin(), projectile); //store projectile
		clockReload.restart();
		clockProjectile.restart();
	}
}

void ProjectileShooter::drawProjectiles(sf::RenderWindow & window) {
	for (int i = 0; i < projectiles.size(); i++) {
		//if projectile exists and on screen
		if (projectiles[i] != NULL && projectileOnScreen(projectiles[i])) {
			//animation for projectile
			if (projectiles[i]->clockAnim.getElapsedTime().asSeconds() > 0.2f) { //change frames every 0.2 seconds
				if (projectiles[i]->trigger) {
					projectiles[i]->projectileSprite.setTexture(projectileTextureA);
				}
				else {
					projectiles[i]->projectileSprite.setTexture(projectileTextureB);
				}
				projectiles[i]->trigger = !projectiles[i]->trigger;
				projectiles[i]->clockAnim.restart();
			}
			window.draw(projectiles[i]->projectileSprite);
			moveProjectile(projectiles[i]);
		}
		else { //otherwise off screen, so delete projectile
			delete projectiles[i];
			projectiles[i] = NULL;
			projectiles.erase(projectiles.begin() + i);  //remove NULL from vector
		}
	}
}

bool ProjectileShooter::projectileOnScreen(Projectile * projectile) {
	return ((projectile->projectileSprite.getPosition().y + projectile->projectileSprite.getLocalBounds().height) >= 0) //up
		&& ((projectile->projectileSprite.getPosition().y - projectile->projectileSprite.getLocalBounds().height) <= windowSize.y) //down
		&& ((projectile->projectileSprite.getPosition().x + projectile->projectileSprite.getLocalBounds().width) >= 0) //left
		&& ((projectile->projectileSprite.getPosition().x - projectile->projectileSprite.getLocalBounds().width) <= windowSize.x); //right
}

void ProjectileShooter::moveProjectile(Projectile * projectile) {
	float dx = projectile->mousePos.x - projectile->startPos.x;
	float dy = projectile->mousePos.y - projectile->startPos.y;

	float norm = sqrt(dx*dx + dy*dy);
	dx = projectileSpeed * dx / norm;
	dy = projectileSpeed * dy / norm;
	projectile->projectileSprite.move(dx, dy);
}

//--PROJECTILE--

ProjectileShooter::Projectile::Projectile() {

}

ProjectileShooter::Projectile::Projectile(sf::RenderWindow & window, sf::Texture & projectileTexture, sf::Sprite playerSprite) {
	mousePos = sf::Mouse::getPosition(window);
	projectileSprite = sf::Sprite(projectileTexture);
	projectileSprite.setOrigin(projectileSprite.getGlobalBounds().width / 2, projectileSprite.getGlobalBounds().height / 2); //set origin to center
	startPos = playerSprite.getPosition();
	projectileSprite.setPosition(startPos);
	projectileSprite.setRotation(playerSprite.getRotation());
}

