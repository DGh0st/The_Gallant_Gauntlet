#pragma once
#include <SFML/Graphics.hpp>

class Textfield : public sf::RectangleShape {
public:
	// constructor
	// @param font: font to use for the text
	// @param size: size of the textfield
	// @param padding: space between text and border
	// @param letterSize: size of a character
	Textfield(sf::Font & font = sf::Font(), sf::Vector2f & size = sf::Vector2f(100.0f, 50.0f), sf::Vector2f & padding = sf::Vector2f(10.0f, 10.0f), unsigned int letterSize = 30U);
	// destructor
	~Textfield();
	// equal operator
	// @param other: the object to copy from
	void operator=(const Textfield & other);
	// draw textfield
	// @param window: the window to draw inside
	void draw(sf::RenderWindow & window);
	// handle event
	// @param event: event to handle
	void handleEvent(sf::Event event);
	// set text color
	void setTextColor(sf::Color color);
	// set cursor color
	void setCursorColor(sf::Color color);
	// get input string
	inline std::string getString() { return string; }

private:
	std::string string = "";
	sf::Text inputText;
	sf::RectangleShape cursor;
	sf::Clock blinkCursorTimer;
	bool hasFocus = false;
	sf::Vector2f padding;
};