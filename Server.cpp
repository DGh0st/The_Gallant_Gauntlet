#include "Server.h"

Server::Server(unsigned short port) : port(port) {
	// clean up list of connections
	connections = std::vector<userInfo>(0);
	// start server
	running = true;
}

Server::~Server() {
	// stop server
	stopServer();
}

void Server::operator=(const Server & other) {
	port = other.port;
	connections = other.connections;
	running = other.running;
	// unbind the port
	other.~Server();
}

void Server::runServer() {
	sf::UdpSocket serverSocket; // server socket (UDP)
	uniqueConnectionCount = 0;
	// bind port
	while (serverSocket.bind(port) != sf::Socket::Done) {
		port = sf::Socket::AnyPort;
	}
	port = serverSocket.getLocalPort();
	printf("[Server] Started on %s:%u\n", sf::IpAddress::getPublicAddress().toString().c_str(), port);
	sf::Packet dataPacket;
	sf::IpAddress senderIp;
	unsigned short senderPort;
	
	while (running) {
		dataPacket.clear();
		int rec = serverSocket.receive(dataPacket, senderIp, senderPort);
		if (rec == sf::Socket::Done) {
			// maintain connections based on data received (join or left)
			sf::Packet copyPacket(dataPacket);
			char *data = (char *)copyPacket.getData();
			copyPacket >> data;
			if (strcmp(data, "left game") == 0) {
				int i;
				for (i = 0; i < connections.size(); i++) {
					if (connections[i].ip == senderIp && connections[i].port == senderPort) {
						printf("[Server] %s left game\n", connections[i].name.c_str());
						break;
					}
				}
				if (i < connections.size()) {
					// construct left game packet
					sf::Packet leftGamePacket;
					leftGamePacket.clear();
					leftGamePacket << "left game" << connections[i].name;
					// send player left game to everyone
					for (int i = 0; i < connections.size(); i++) {
						if (serverSocket.send(leftGamePacket, connections[i].ip, connections[i].port)) {
							// failed sending leftGamePacket
						}
					}
					// erase from connections
					connections.erase(connections.begin() + i);
				}
				continue;
			}
			else if (strcmp(data, "join game") == 0) {
				uniqueConnectionCount++;
				userInfo user;
				user.name = "Player" + std::to_string(uniqueConnectionCount);
				user.ip = senderIp;
				user.port = senderPort;
				connections.push_back(user);
				printf("[Server] %s joined game\n", user.name.c_str());
				// time left in game
				float timeLeft = preMatchTime;
				if (uniqueConnectionCount == 1) {
					gameTimer.restart();
				}
				if (connections.size() >= 2 && !isInPeaceMode) {
					timeLeft = (totalMatchTime - gameTimer.getElapsedTime().asSeconds());
					if (timeLeft <= 0) {
						isInPeaceMode = true; // go into peace mode once game ended
						gameTimer.restart();
					}
				}
				else if (isInPeaceMode) {
					timeLeft = (preMatchTime - gameTimer.getElapsedTime().asSeconds());
					if (timeLeft <= 0) {
						if (connections.size() >= 2) {
							isInPeaceMode = false; // go into fight mode once peace time ended
						}
						gameTimer.restart();
					}
				}
				// send "success" with user.name and time left in game
				sf::Packet successPacket;
				successPacket.clear();
				successPacket << "success" << user.name << timeLeft << isInPeaceMode;
				if (serverSocket.send(successPacket, senderIp, senderPort)) {
					// failed sending "success"
				}
				continue;
			}
			float timeLeft = preMatchTime;
			if (connections.size() >= 2 && !isInPeaceMode) {
				timeLeft = (totalMatchTime - gameTimer.getElapsedTime().asSeconds());
				if (timeLeft <= 0) {
					isInPeaceMode = true; // go into peace mode once game ended
					gameTimer.restart();
				}
			}
			else if (isInPeaceMode) {
				timeLeft = (preMatchTime - gameTimer.getElapsedTime().asSeconds());
				if (timeLeft <= 0) {
					if (connections.size() >= 2) {
						isInPeaceMode = false; // go into fight mode once peace time ended
					}
					gameTimer.restart();
				}
			}
			dataPacket << timeLeft << isInPeaceMode; 
			// send data to every connection
			for (int i = 0; i < connections.size(); i++) {
				if (serverSocket.send(dataPacket, connections[i].ip, connections[i].port) != sf::Socket::Done) {
					// Error sending packet
				}
			}
		}
	}
	// send "closed" to every connection
	sf::Packet closedPacket;
	closedPacket.clear();
	closedPacket << "closed";
	for (int i = 0; i < connections.size(); i++) {
		if (serverSocket.send(closedPacket, connections[i].ip, connections[i].port) != sf::Socket::Done) {
			// Error sending packet
		}
	}
	// unbind port
	serverSocket.unbind();
}

void Server::stopServer() {
	running = false;
}

unsigned int Server::getPort() {
	return port;
}