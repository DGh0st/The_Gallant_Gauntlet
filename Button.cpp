#include "Button.h"

Button::Button(sf::Text & text, sf::Vector2f & padding, void (*function)()) : sf::RectangleShape(), callback_function(function) {
	buttonText = sf::Text(text);
	buttonText.setOrigin(buttonText.getGlobalBounds().width / 2.0f, buttonText.getGlobalBounds().height / 2.0f);
	this->setSize(sf::Vector2f(buttonText.getGlobalBounds().width + padding.x, buttonText.getGlobalBounds().height + padding.y));
}

Button::~Button() {
	// nothing to destruct as of yet ...
}

void Button::operator=(const Button & other) {
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
	sf::FloatRect buttonBounds = this->getGlobalBounds();
	buttonText.setPosition(sf::Vector2f(buttonBounds.left + buttonBounds.width / 2.0f, buttonBounds.top + buttonBounds.height / 2.0f));
	window.draw(buttonText);
	handleMouseMovement(window);
}

void Button::handleMouseMovement(sf::RenderWindow & window) {
	if (this->getGlobalBounds().contains((sf::Vector2f) sf::Mouse::getPosition(window))) {
		if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
			this->setFillColor(clickedColor);
			this->setTexture(&clickedTexture);
			// call the on click function that they provided
			callback_function();
		}
		else {
			this->setFillColor(hoverColor);
			this->setTexture(&hoverTexture);
		}
	}
	else {
		this->setFillColor(normalColor);
		this->setTexture(&normalTexture);
	}
}