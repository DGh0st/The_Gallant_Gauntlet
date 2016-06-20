#ifndef DEFINES_H
#define DEFINES_H
#include <SFML/Network.hpp>

// all
#define windowDesignSize sf::Vector2u(1920, 1080) // UI designed for these resolutions
static sf::Vector2u windowSize(1920, 1080); // current window size
// main and character
#define moveUpKey sf::Keyboard::W // up
#define moveDownKey sf::Keyboard::S // down
#define moveLeftKey sf::Keyboard::A // left
#define moveRightKey sf::Keyboard::D // right
// character
#define PI 3.14159265f
#define playerSize 20.0f // size of the character
// Server and Client
#ifndef USERINFO
#define USERINFO

struct userInfo {
	std::string name = ""; // name of the user
	sf::IpAddress ip = sf::IpAddress::None; // ip used to connect to server
	unsigned short port = 1000; // port used to connect to server
};

#endif // !USERINFO
// main
enum screenTypes {
	title = 0, // title screen
	createServer = 1, // create a server screen
	joinServer = 2, // join a server screen
	ingame = 3, // in game screen
};

#endif // !DEFINES_H