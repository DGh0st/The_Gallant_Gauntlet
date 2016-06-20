#ifndef SERVER_H
#define SERVER_H
#include <SFML/Network.hpp>
#include "Defines.h"

class Server {
public:
	// constructor
	Server(unsigned short port = 0);
	// destructor
	~Server();
	// equal operator
	void operator=(const Server & other);
	// run server
	void runServer();
	// stop server
	void stopServer();
	// get port that the server is running on
	unsigned int getPort();

private:
	bool running = false; // server running or not
	unsigned short port; // port for the server
	std::vector<userInfo> connections; // list of connections (users)
	int uniqueConnectionCount = 0; // number of unique connections
};

#endif // !SERVER_H