#include "Textfield.h"

Textfield::Textfield(sf::Font & font, sf::Vector2f & size, sf::Vector2f & padding, unsigned int letterSize) : sf::RectangleShape(size + padding), padding(padding) {
	inputText = sf::Text(string, font, letterSize);
	inputText.setOrigin(-padding.x, (float) letterSize / 2.0f);
	inputText.setColor(sf::Color::Black);
	cursor = sf::RectangleShape(sf::Vector2f(3.0f, (float) letterSize));
	cursor.setOrigin(0.0f, (float) letterSize / 2.0f);
	cursor.setFillColor(sf::Color::Black);
	blinkCursorTimer.restart();
}

Textfield ::~Textfield() {
	// nothing to delete as of yet ...
}

void Textfield::operator=(const Textfield & other) {
	string = other.string;
	inputText = other.inputText;
	cursor = other.cursor;
	blinkCursorTimer = other.blinkCursorTimer;
	hasFocus = other.hasFocus;
	padding = other.padding;
}

void Textfield::draw(sf::RenderWindow & window) {
	window.draw(*this);
	inputText.setString(string);
	inputText.setPosition(this->getGlobalBounds().left, this->getGlobalBounds().top + this->getGlobalBounds().height / 2.1f);
	window.draw(inputText);
	cursor.setPosition(inputText.getGlobalBounds().left + inputText.getGlobalBounds().width, this->getGlobalBounds().top + this->getGlobalBounds().height / 2.0f);
	if (hasFocus && (int) blinkCursorTimer.getElapsedTime().asSeconds() % 2 == 0) {
		window.draw(cursor);
	}
}

void Textfield::handleEvent(sf::Event event) {
	if (hasFocus && event.type == sf::Event::TextEntered) {
		if ((char)event.text.unicode == 8) { // backspace
			if (string.size() > 0) {
				string.pop_back();
			}
		}
		else if ((char)event.text.unicode >= 32 && (char)event.text.unicode < 127) { // everything else that can be displayed
			if (cursor.getGlobalBounds().left + cursor.getGlobalBounds().width < this->getGlobalBounds().left + this->getGlobalBounds().width - 2.0f * padding.x) {
				string.push_back((char)event.text.unicode);
			}
		}
		blinkCursorTimer.restart();
	}
	else if (event.type == sf::Event::MouseButtonPressed) {
		if (this->getGlobalBounds().contains(event.mouseButton.x, event.mouseButton.y)) {
			hasFocus = true;
		}
		else {
			hasFocus = false;
		}
	}
}


void Textfield::setTextColor(sf::Color color) {
	inputText.setColor(color);
}

void Textfield::setCursorColor(sf::Color color) {
	cursor.setFillColor(color);
}