#include <SFML/Graphics.hpp>
#include "character.h"
#include "Defines.h"
#include "Button.h"
#include "Textfield.h"

void playScreen() {
	currentScreenDisplayed = joinServer;
}
void createServerScreen() {
	currentScreenDisplayed = createServer;
}
void ingameScreen() {
	currentScreenDisplayed = ingame;
}

int main() {
	sf::RenderWindow window(sf::VideoMode(800, 500), "Game 2"); //, sf::Style::Fullscreen);
	windowSize = window.getSize();

	sf::Font font;
	font.loadFromFile("fonts/times.ttf");
	Character player = Character();
	sf::Keyboard::Key releasedKey = sf::Keyboard::Unknown;
	
	Textfield joinTF(font,sf::Vector2f(650.0f,50.0f));
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
			joinTF.handleEvent(event);
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

		if (currentScreenDisplayed == title) {
			//title text
			sf::Text titleText("Title", font);
			titleText.setOrigin(titleText.getGlobalBounds().width/2,titleText.getGlobalBounds().height/2);
			titleText.setPosition(sf::Vector2f(windowSize.x/2, 10.0f));
			//play button
			sf::Text playText("Play", font);
			playText.setColor(sf::Color::Black);
			Button playButton(playText, sf::Vector2f(100.0f,75.0f), playScreen);
			playButton.setOrigin(playButton.getGlobalBounds().width / 2.0f, playButton.getGlobalBounds().height / 2.0f);
			playButton.setPosition(sf::Vector2f(windowSize.x/2, 100.0f));
			window.draw(titleText);
			playButton.draw(window);
			//create game button
			sf::Text createText("Create Game", font);
			createText.setColor(sf::Color::Black);
			Button createButton(createText, sf::Vector2f(200.0f, 75.0f), createServerScreen);
			createButton.setOrigin(createButton.getGlobalBounds().width / 2.0f, createButton.getGlobalBounds().height / 2.0f);
			createButton.setPosition(sf::Vector2f(windowSize.x / 2, 300.0f));
			window.draw(titleText);
			createButton.draw(window);
		}
		else if (currentScreenDisplayed == joinServer) {
			//server address text
			sf::Text serverText("Server Address", font, 16);
			serverText.setOrigin(serverText.getGlobalBounds().width / 2.0f, serverText.getGlobalBounds().height / 2.0f);
			serverText.setPosition(window.getSize().x / 2.0f, window.getSize().y / 2.0f - serverText.getGlobalBounds().height * 4.0f);
			serverText.setColor(sf::Color::Cyan);
			//join text field
			joinTF.setOrigin(joinTF.getGlobalBounds().width / 2.0f, joinTF.getGlobalBounds().height / 2.0f);
			joinTF.setPosition(window.getSize().x / 2.0f, window.getSize().y / 2.0f);
			//join game button
			sf::Text joinText("Join Game", font);
			joinText.setColor(sf::Color::Black);
			Button joinButton(joinText, sf::Vector2f(200.0f, 75.0f),ingameScreen);
			joinButton.setOrigin(joinButton.getGlobalBounds().width / 2.0f, joinButton.getGlobalBounds().height / 2.0f);
			joinButton.setPosition(sf::Vector2f(windowSize.x / 2, 400.0f));
			// draw
			window.draw(serverText);
			joinTF.draw(window);
			joinButton.draw(window);
		}
		else if (currentScreenDisplayed == createServer) {
			//ip and instructions on joining a server text
			sf::Text ipText("Want others to join your server? Have them use this IP address:123.123.123.123:5555", font, 20);
			ipText.setOrigin(ipText.getGlobalBounds().width / 2.0f, ipText.getGlobalBounds().height / 2.0f);
			ipText.setPosition(window.getSize().x / 2.0f, window.getSize().y / 2.0f - ipText.getGlobalBounds().height * 10.0f);
			ipText.setColor(sf::Color::Cyan);
			//join game button
			sf::Text joinText("Join Game", font);
			joinText.setColor(sf::Color::Black);
			Button joinButton(joinText, sf::Vector2f(200.0f, 75.0f), ingameScreen);
			joinButton.setOrigin(joinButton.getGlobalBounds().width / 2.0f, joinButton.getGlobalBounds().height / 2.0f);
			joinButton.setPosition(sf::Vector2f(windowSize.x / 2, 400.0f));
			//draw
			joinButton.draw(window);
			window.draw(ipText);
		}
		else if (currentScreenDisplayed == ingame) {
			player.draw(window);
		}

		window.display();
	}

	return 0;
}