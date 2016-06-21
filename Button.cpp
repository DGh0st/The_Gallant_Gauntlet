#include "Button.h"

Button::Button(sf::Text & text, sf::Vector2f & padding, void (*function)()) : sf::RectangleShape(), callback_function(function) {
	this->setSize(sf::Vector2f(buttonText.getGlobalBounds().width + padding.x, buttonText.getGlobalBounds().height + padding.y));
	buttonText = sf::Text(text);
	buttonText.setOrigin(buttonText.getGlobalBounds().width / 2.0f, buttonText.getGlobalBounds().height / 2.0f);
}

Button::~Button() {
	// nothing to destruct as of yet ...
}

void Button::operator=(const Button & other) {
	this->setSize(other.getSize());
	this->setOrigin(other.getOrigin());
	this->setFillColor(other.getFillColor());
	this->setScale(other.getScale());
	this->setPosition(other.getPosition());
	buttonText = sf::Text(other.buttonText);
	normalColor = other.normalColor;
	hoverColor = other.hoverColor;
	clickedColor = other.clickedColor;
	normalTexture = other.normalTexture;
	hoverTexture = other.hoverTexture;
	clickedTexture = other.clickedTexture;
	callback_function = other.callback_function;
}

void Button::draw(sf::RenderWindow & window) {
	window.draw(*this);
	buttonText.setPosition(sf::Vector2f(this->getGlobalBounds().left + this->getGlobalBounds().width / 2.0f, this->getGlobalBounds().top + this->getGlobalBounds().height / 2.0f));
	window.draw(buttonText);
}

void Button::handleEvent(sf::Event event) {
	if (event.type == sf::Event::MouseButtonPressed) {
		if (this->getGlobalBounds().contains((float) event.mouseButton.x, (float) event.mouseButton.y)) {
			if (event.mouseButton.button == sf::Mouse::Left) {
				this->setFillColor(clickedColor);
				this->setTexture(&clickedTexture);
				hasFocus = true;
				// call the on click function that they provided
			}
			else {
				this->setFillColor(hoverColor);
				this->setTexture(&hoverTexture);
				hasFocus = false;
			}
		}
		else {
			this->setFillColor(normalColor);
			this->setTexture(&normalTexture);
			hasFocus = false;
		}
	}
	else if (event.type == sf::Event::MouseButtonReleased) {
		if (event.mouseButton.button == sf::Mouse::Left && hasFocus) {
			callback_function();
		}
	}
}