#ifndef BUTTON_H
#define BUTTON_H
#include <SFML/Graphics.hpp>
#include "Defines.h"

class Button : public sf::RectangleShape {
public:
	// constructor that auto resizes the button based on text
	// @param text: sfml text to be used on top of the button (note a copy of the sfml text is created for the button)
	// @param padding: used for the size of the button
	// @param func: function to call on button click (note a function needs to be passed in);
	Button(sf::Text & text = sf::Text(), sf::Vector2f & padding = sf::Vector2f(10.0f, 10.0f), void (*function)() = NULL);
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
	// set normal/hover/clicked color/texture
	inline void setNormalColor(sf::Color color) { normalColor = color; }
	inline void setHoverColor(sf::Color color) { hoverColor = color; }
	inline void setClickedColor(sf::Color color) { clickedColor = color; }
	inline void setNormalTexture(sf::Texture texture) { normalTexture = texture; }
	inline void setHoverTexture(sf::Texture texture) { hoverTexture = texture; }
	inline void setClickedTexture(sf::Texture texture) { clickedTexture = texture; }
	inline void setOnClickFunction(void (*func)()) { callback_function = func; }

private:
	sf::Text buttonText;
	sf::Color normalColor;
	sf::Color hoverColor;
	sf::Color clickedColor;
	sf::Texture normalTexture;
	sf::Texture hoverTexture;
	sf::Texture clickedTexture;
	void (*callback_function)() = NULL;
	bool hasFocus = false;
};

#endif // !BUTTON_H