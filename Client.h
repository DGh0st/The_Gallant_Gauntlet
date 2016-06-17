#pragma once
#include <SFML/Network.hpp>
#include "Defines.h"

class Client {
public:
	// constructor
	Client(userInfo server);
	// destructor
	~Client();
	// equal operator
	void operator=(const Client & other);
	// send packet
	void sendPacket(sf::Packet & packet);
	// receive packets if any
	void receivePackets();

private:
	sf::UdpSocket clientSocket; // client socket (UDP)
	unsigned short port; // port for the client
	userInfo server; // server ip and port
};

