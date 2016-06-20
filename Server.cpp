#include "Server.h"

Server::Server(unsigned short port) {
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
	// bind port
	while (serverSocket.bind(port) != sf::Socket::Done) {
		port = sf::Socket::AnyPort;
	}
	port = serverSocket.getLocalPort();
	sf::Packet dataPacket;
	sf::IpAddress senderIp;
	unsigned short senderPort;
	
	while (running) {
		int rec = serverSocket.receive(dataPacket, senderIp, senderPort);
		if (rec == sf::Socket::Done) {
			// maintain connections based on data received (join or left)
			char *data = (char *)dataPacket.getData();
			userInfo user;
			printf(data);
			if (strcmp(data, "left game") == 0) {
				for (int i = 0; i < connections.size(); i++) {
					if (connections[i].ip == senderIp && connections[i].port == senderPort) {
						connections.erase(connections.begin() + i);
					}
				}
				continue;
			}
			else if (strcmp(data, "join game") == 0) {
				user.name = uniqueConnectionCount;
				user.ip = senderIp;
				user.port = senderPort;
				uniqueConnectionCount++;
				connections.push_back(user);
				continue;
			}
			else {
				for (int i = 0; i < connections.size(); i++) {
					if (connections[i].ip == senderIp && connections[i].port == senderPort) {
						user = connections[i];
					}
				}
			}
			sf::Packet resultPacket;
			resultPacket << user.name << dataPacket;
			// send data to every connection
			for (int i = 0; i < connections.size(); i++) {
				if (connections[i].ip != senderIp && connections[i].port != senderPort) {
					if (serverSocket.send(resultPacket, connections[i].ip, connections[i].port) != sf::Socket::Done) {
						// Error sending packet
					}
				}
			}
		}
	}
	serverSocket.unbind();
}

void Server::stopServer() {
	running = false;
}

unsigned int Server::getPort() {
	return port;
}