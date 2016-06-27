#ifndef CLIENT_H
#define CLIENT_H
#include <SFML/Network.hpp>
#include <SFML/Audio.hpp>
#include "Defines.h"
#include "Character.h"

struct PlayerData {
	bool isViable = true;
	std::string userID;
	std::string fighterClass;
	Character *userCharacter;
};

class Client {
public:
	// constructor
	// @param socket: the client's socket to connect to server
	// @param server: the info about the server
	Client(sf::UdpSocket & socket, userInfo server = userInfo());
	// destructor
	~Client();
	// equal operator
	// @param other: the Client object to copy from
	void operator=(const Client & other);
	// send packet
	// @param socket: the client's socket to use to send packet
	// @param packet: the packet to send
	void sendPacket(sf::UdpSocket & socket, sf::Packet & packet);
	// receive packets if any
	// @param socket: the client's socket to receive packets on
	void receivePackets(sf::UdpSocket & socket, int & kills, sf::Texture & rangerTexture, sf::Texture & mageTexture, sf::Texture & knightTexture, sf::Texture & arrowTexture, sf::Texture & fireballA, sf::Texture & fireballB, sf::Texture & swordTexture, sf::Texture & bowTexture, sf::Texture & staffTexture, sf::Texture & healthBarForegroundTexture, sf::Texture & healthBarBackgroundTexture);
	// stop receiving packets
	void stopReceivingPackets();
	// draw other players
	void draw(sf::RenderWindow & window, sf::Vector2f playerPosition);
	// check collisions and send data over server if needed
	void checkCollisions(Character *player, classTypes currentClass, sf::UdpSocket & socket, sf::SoundBuffer & takeDamageSoundBuffer, sf::SoundBuffer & doDamageSoundBuffer);
	// get client id
	std::string getClientId();
	// get is game in progress or not
	bool isGameInProgress();

	float timeLeftInGame; // time left in game (in seconds)
	int serverConnectionStatus = 0; // 0 if haven't gotten response, 1 if failed, 2 if successfully connected
private:
	bool running;
	unsigned short port; // port for the client
	userInfo server; // server ip and port
	std::vector<PlayerData> otherPlayers;
	std::string clientIDfromServer; // id of the client gotten from the server
	bool gameNotInProgress; // is game currently in progress
	sf::Clock damageTakenVisualEffectTimer; // timer for damage taken visuals
};

#endif // !CLIENT_H