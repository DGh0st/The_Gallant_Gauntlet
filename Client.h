#ifndef CLIENT_H
#define CLIENT_H
#include <SFML/Network.hpp>
#include "Defines.h"
#include "Character.h"

struct PlayerData {
	std::string userID;
	Character userCharacter;
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
	void receivePackets(sf::UdpSocket & socket);
	// stop receiving packets
	void stopReceivingPackets();
	// draw other players
	void draw(sf::RenderWindow & window);

private:
	unsigned short port; // port for the client
	userInfo server; // server ip and port
	std::vector<PlayerData> otherPlayers;
};

#endif // !CLIENT_H