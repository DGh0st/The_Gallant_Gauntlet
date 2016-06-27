#ifndef BUTTON_H
#define BUTTON_H
#include <SFML/Graphics.hpp>
#include "Defines.h"

class Button : public sf::RectangleShape {
public:
	// constructor that auto resizes the button based on text
	// @param text: sfml text to be used on top of the button (note a copy of the sfml text is created for the button)
	// @param size: used for the size of the button
	// @param func: function to call on button click (note a function needs to be passed in);
	Button(sf::Text & text = sf::Text(), sf::Vector2f & size = sf::Vector2f(10.0f, 10.0f), void (*function)() = NULL);
	// destructor
	~Button();
	// equal operator
	// @param other: the object to copy from
	void operator=(const Button & other);
	// draw button
	// @param window: the window to draw inside
	void draw(sf::RenderWindow & window);
	// handle button click event (optional method as it will otherwise make use of draw)
	// @param event: the event to handle
	void handleEvent(sf::Event event);
	// set on click callback function
	inline void setOnClickFunction(void (*func)()) { callback_function = func; }

private:
	sf::Text buttonText;
	void (*callback_function)() = NULL;
	bool hasFocus = false;
};

#endif // !BUTTON_H