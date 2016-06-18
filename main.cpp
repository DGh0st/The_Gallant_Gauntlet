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

		/*// parse ip and port
		size_t offset = connectToIPandPort.find_first_of(":", 0);
		serverInfo.ip = sf::IpAddress(connectToIPandPort.substr(0, offset));
		serverInfo.port = (unsigned short) atoi(connectToIPandPort.substr(offset + 1, connectToIPandPort.length() - offset).c_str());
		// actually join server (create a client object)
		currentScreenDisplayed = ingame;*/

		/*if (currentScreenDisplayed == title) {
			// need to design title
		}
		else if (currentScreenDisplayed == joinServer) {
			// join a server text
			sf::Text text1("Join a Server", font, 128);
			text1.setOrigin(text1.getGlobalBounds().width / 2.0f, text1.getGlobalBounds().height / 2.0f);
			text1.setPosition(window.getSize().x / 2.0f, window.getSize().y / 2.0f - text1.getGlobalBounds().height * 1.1f);
			text1.setColor(sf::Color::Blue);
			// ip format text
			sf::Text text2("Server Address", font, 16);
			text2.setOrigin(text2.getGlobalBounds().width / 2.0f, text2.getGlobalBounds().height / 2.0f);
			text2.setPosition(window.getSize().x / 2.0f, window.getSize().y / 2.0f - text2.getGlobalBounds().height * 2.0f);
			text2.setColor(sf::Color::Cyan);
			// ip and port entered
			sf::Text text(connectToIPandPort, font, 32);
			text.setOrigin(text.getGlobalBounds().width / 2.0f, text.getGlobalBounds().height / 2.0f);
			text.setPosition(window.getSize().x / 2.0f, window.getSize().y / 2.0f);
			text.setColor(sf::Color::Black);
			// white background
			sf::RectangleShape background(sf::Vector2f(300.0f, 40.0f));
			background.setOrigin(150.0f, 10.0f);
			background.setFillColor(sf::Color::White);
			background.setPosition(window.getSize().x / 2.0f, window.getSize().y / 2.0f);
			// ip and port entered
			sf::Text text3("Done", font, 24);
			text3.setOrigin(text3.getGlobalBounds().width / 2.0f, text3.getGlobalBounds().height / 2.0f);
			text3.setPosition(window.getSize().x / 2.0f, window.getSize().y / 2.0f + text3.getGlobalBounds().height * 3.0f);
			text3.setColor(sf::Color::Black);
			// button
			sf::RectangleShape button(sf::Vector2f(150.0f, 32.0f));
			button.setOrigin(75.0f, 8.0f);
			button.setFillColor(sf::Color::Green);
			button.setPosition(window.getSize().x / 2.0f, window.getSize().y / 2.0f + text3.getGlobalBounds().height * 2.8f);
			// draw
			window.draw(background);
			window.draw(button);
			window.draw(text);
			window.draw(text1);
			window.draw(text2);
			window.draw(text3);
		}
		else if (currentScreenDisplayed == createServer) {

		}
		else if (currentScreenDisplayed == ingame) {
			player.draw(window);
		}*/

		window.display();
	}

	return 0;
}