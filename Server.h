#pragma once
#include <SFML/Network.hpp>
#include "Defines.h"

class Server {
public:
	// constructor
	Server(unsigned short port);
	// destructor
	~Server();
	// equal operator
	void operator=(const Server & other);
	// run server
	void runServer();

private:
	bool running; // server running or not
	sf::UdpSocket serverSocket; // server socket (UDP)
	unsigned short port; // port for the server
	std::vector<userInfo> connections; // list of connections (users)
};

