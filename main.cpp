#include <SFML/Graphics.hpp>
#include "character.h"
#include "Defines.h"
#include "Button.h"
#include "Textfield.h"

int main() {
	sf::RenderWindow window(sf::VideoMode(800, 500), "Demon Hunter"); //, sf::Style::Fullscreen);
	windowSize = window.getSize();

	sf::Font font;
	font.loadFromFile("fonts/times.ttf");
	Character player = Character();
	sf::Keyboard::Key releasedKey = sf::Keyboard::Unknown;
	
	userInfo serverInfo;

	while (window.isOpen()) {
		releasedKey = sf::Keyboard::Unknown; // reset released

		sf::Event event;
		while (window.pollEvent(event)) {
			if (event.type == sf::Event::Closed) {
				window.close();
			}
			else if (event.type == sf::Event::KeyReleased && event.key.code == sf::Keyboard::Escape) {
				window.close();
			}
			else if (currentScreenDisplayed == ingame && event.type == sf::Event::KeyReleased) {
				sf::Keyboard::Key tempKey = event.key.code;
				if (tempKey == moveUpKey || tempKey == moveDownKey || tempKey == moveLeftKey || tempKey == moveRightKey) {
					releasedKey = tempKey;
				}
			}
		}

		if (currentScreenDisplayed == ingame) {
			player.move(window, releasedKey);
		}

		window.clear();

		if (currentScreenDisplayed == ingame) {
			player.draw(window);
		}

		window.display();
	}

	return 0;
}