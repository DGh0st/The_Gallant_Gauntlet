#include "ProjectileShooter.h"
#include "Defines.h" 

ProjectileShooter::ProjectileShooter(sf::Texture & healthBarForegroundTexture, sf::Texture & healthBarBackgroundTexture, 
	sf::Texture & projectileShooterTexture, sf::Texture & weaponTexture, sf::Texture & projectileTextureA, sf::Texture & projectileTextureB,
	float projectileSpeed, float projectileReloadTime, float timeAfterShot, bool fromWeapon) : 
	Character(healthBarForegroundTexture, healthBarBackgroundTexture), projectileTextureA(projectileTextureA),
	projectileTextureB(projectileTextureB), projectileSpeed(projectileSpeed), projectileReloadTime(projectileReloadTime),
	timeAfterShot(timeAfterShot), fromWeapon(fromWeapon) {
	playerSprite = sf::Sprite(projectileShooterTexture);
	playerSprite.setOrigin(playerSprite.getGlobalBounds().width / 2, playerSprite.getGlobalBounds().height / 2);
	weaponSprite = sf::Sprite(weaponTexture);
	weaponSprite.setOrigin(weaponSprite.getGlobalBounds().width / 2, weaponSprite.getGlobalBounds().height / 2);
	weaponCircle = sf::CircleShape(50.0f);
	weaponCircle.setOrigin(weaponCircle.getRadius(), weaponCircle.getRadius());
	projectiles = std::vector<Projectile *>(0);
}

void ProjectileShooter::draw(sf::RenderWindow & window) {
	window.draw(weaponSprite);
	Character::draw(window);
}

void ProjectileShooter::shoot(sf::RenderWindow & window) {
	//if shooting and timeAfterShot goes by, mark isShooting as false
	if (isShooting && clockProjectile.getElapsedTime().asSeconds() > timeAfterShot) {
		isShooting = false;
		clockProjectile.restart();
	}
	//shoot if left mouse click and projectileReloadTime went by
	if (isPlayer && sf::Mouse::isButtonPressed(sf::Mouse::Left) && clockReload.getElapsedTime().asSeconds() > projectileReloadTime) {
		isShooting = true;
		uniqueProjectilesCounter++;
		Projectile * projectile;
		if (!fromWeapon) { //shoot projectile from playerSprite
			projectile = new Projectile(window, projectileTextureA, playerSprite);
		}
		else { //shoot projectile from weaponSprite
			projectile = new Projectile(window, projectileTextureA, weaponSprite);
		}
		projectiles.insert(projectiles.begin(), projectile); //store projectile
		clockReload.restart();
		clockProjectile.restart();
	}
}

bool ProjectileShooter::collisionPP(sf::CircleShape & player) {
	for (int i = 0; i < projectiles.size(); i++) {
		sf::FloatRect projectileBox = projectiles[i]->projectileSprite.getGlobalBounds();
		//if not swinging, we don't want collision to register so return false
		//if sword already intersected, return false to prevent multiple intersections on one swing
		//if swordBox and rectangle encompassing player circle don't intersect, no collision possible (light, quick check)
		if (!projectileBox.intersects(player.getGlobalBounds())) {
			return false;
		}
		//full collision check between sword and circle of player
		float circleX = player.getPosition().x;
		float closestX;
		if (circleX < projectileBox.left) { //circle to left of rect
			closestX = projectileBox.left;
		}
		else if (circleX > projectileBox.left + projectileBox.width) { //circle to right of rect
			closestX = projectileBox.left + projectileBox.width;
		}
		else {
			closestX = circleX;
		}

		float circleY = player.getPosition().y;
		float closestY;
		if (circleY < projectileBox.top) { //circle above rect
			closestY = projectileBox.top;
		}
		else if (circleY > projectileBox.top + projectileBox.height) { //circle below rect
			closestY = projectileBox.top + projectileBox.height;
		}
		else {
			closestY = circleY;
		}

		float dx = (circleX - closestX) * (circleX - closestX);
		float dy = (circleY - closestY) * (circleY - closestY);
		bool ret = sqrt(dx + dy) < player.getRadius();
		if (ret) {
			projectiles.erase(projectiles.begin() + i);
			return true;
		}
	}
	return false;
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
	return ((projectile->projectileSprite.getPosition().y + projectile->projectileSprite.getLocalBounds().height) >= playerSprite.getPosition().y - windowSize.y / 2.0f) //up
		&& ((projectile->projectileSprite.getPosition().y - projectile->projectileSprite.getLocalBounds().height) <= playerSprite.getPosition().y + windowSize.y / 2.0f) //down
		&& ((projectile->projectileSprite.getPosition().x + projectile->projectileSprite.getLocalBounds().width) >= playerSprite.getPosition().x - windowSize.x / 2.0f) //left
		&& ((projectile->projectileSprite.getPosition().x - projectile->projectileSprite.getLocalBounds().width) <= playerSprite.getPosition().x + windowSize.x / 2.0f); //right
}

void ProjectileShooter::moveProjectile(Projectile * projectile) {
	float dx = projectile->mousePos.x - projectile->startPos.x;
	float dy = projectile->mousePos.y - projectile->startPos.y;

	float norm = sqrt(dx*dx + dy*dy);
	dx = projectileSpeed * dx / norm;
	dy = projectileSpeed * dy / norm;
	projectile->projectileSprite.move(dx, dy);
}

void ProjectileShooter::setWeapon(sf::RenderWindow & window) {
	weaponCircle.setPosition(playerSprite.getPosition());
	weaponCircle.setRotation(playerSprite.getRotation());
	if (!isShooting) { //default position at side of player
		float x = weaponCircle.getPosition().x + (weaponCircle.getRadius())*cos(weaponCircle.getRotation()* (PI / 180) + PI / 3);
		float y = weaponCircle.getPosition().y + (weaponCircle.getRadius())*sin(weaponCircle.getRotation()* (PI / 180) + PI / 3);
		weaponSprite.setPosition(x, y);

		weaponSprite.setRotation(weaponCircle.getRotation() - 15);
	}
	else if (fromWeapon) { //position while shooting (and projectiles should come from weapon)
		float x = weaponCircle.getPosition().x + (weaponCircle.getRadius())*cos(weaponCircle.getRotation()* (PI / 180) + PI / 4);
		float y = weaponCircle.getPosition().y + (weaponCircle.getRadius())*sin(weaponCircle.getRotation()* (PI / 180) + PI / 4);
		weaponSprite.setPosition(x, y);

		if (isPlayer) {
			sf::Vector2i mousePos = (sf::Vector2i)window.mapPixelToCoords(sf::Mouse::getPosition(window));
			sf::Vector2i weaponPos = (sf::Vector2i)weaponSprite.getPosition();
			sf::Vector2i diff = mousePos - weaponPos;
			float angle = (float)atan2(diff.y, diff.x) * (180 / PI);
			weaponSprite.setRotation(angle);
		}
	}
	else { //position while shooting (and projectiles should come from player)
		float x = weaponCircle.getPosition().x + (weaponCircle.getRadius())*cos(weaponCircle.getRotation()* (PI / 180) + 2 * PI);
		float y = weaponCircle.getPosition().y + (weaponCircle.getRadius())*sin(weaponCircle.getRotation()* (PI / 180) + 2 * PI);
		weaponSprite.setPosition(x, y);

		weaponSprite.setRotation(weaponCircle.getRotation() - 75);
	}
}

//--PROJECTILE--

ProjectileShooter::Projectile::Projectile() {

}

ProjectileShooter::Projectile::Projectile(sf::RenderWindow & window, sf::Texture & projectileTexture, sf::Sprite startSprite) {
	mousePos = (sf::Vector2i)window.mapPixelToCoords(sf::Mouse::getPosition(window));
	projectileSprite = sf::Sprite(projectileTexture);
	projectileSprite.setOrigin(projectileSprite.getGlobalBounds().width / 2, projectileSprite.getGlobalBounds().height / 2); //set origin to center

	//set startPos forward by dx,dy (makes projectile come from front of startSprite, rather than middle)
	startPos = startSprite.getPosition();
	float dx = mousePos.x - startPos.x;
	float dy = mousePos.y - startPos.y;
	float norm = sqrt(dx*dx + dy*dy);
	dx = 50 * dx / norm;
	dy = 50 * dy / norm;
	startPos = sf::Vector2f(startPos.x + dx, startPos.y + dy);
	projectileSprite.setPosition(startPos);

	//rotate projectile to face mouse
	sf::Vector2i projectilePos = (sf::Vector2i)projectileSprite.getPosition();
	sf::Vector2i diff = mousePos - projectilePos;
	float angle = (float)atan2(diff.y, diff.x) * (180 / PI);
	projectileSprite.setRotation(angle);
}

ProjectileShooter::Projectile::Projectile(sf::Vector2i mousePos, sf::Texture & projectileTexture, sf::Sprite startSprite) {
	this->mousePos = mousePos;
	projectileSprite = sf::Sprite(projectileTexture);
	projectileSprite.setOrigin(projectileSprite.getGlobalBounds().width / 2, projectileSprite.getGlobalBounds().height / 2); //set origin to center

	//set startPos forward by dx,dy (makes projectile come from front of startSprite, rather than middle)
	startPos = startSprite.getPosition();
	float dx = mousePos.x - startPos.x;
	float dy = mousePos.y - startPos.y;
	float norm = sqrt(dx*dx + dy*dy);
	dx = 50 * dx / norm;
	dy = 50 * dy / norm;
	startPos = sf::Vector2f(startPos.x + dx, startPos.y + dy);
	projectileSprite.setPosition(startPos);

	//rotate projectile to face mouse
	sf::Vector2i projectilePos = (sf::Vector2i)projectileSprite.getPosition();
	sf::Vector2i diff = mousePos - projectilePos;
	float angle = (float)atan2(diff.y, diff.x) * (180 / PI);
	projectileSprite.setRotation(angle);
}