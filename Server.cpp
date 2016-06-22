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
	printf("[Server] Starting on %u\n", port);
	sf::UdpSocket serverSocket; // server socket (UDP)
	uniqueConnectionCount = 0;
	// bind port
	while (serverSocket.bind(port) != sf::Socket::Done) {
		port = sf::Socket::AnyPort;
	}
	port = serverSocket.getLocalPort();
	sf::Packet dataPacket;
	sf::IpAddress senderIp;
	unsigned short senderPort;
	
	while (running) {
		dataPacket.clear();
		//printf("[Server] Waiting for data... %zu vs %d\n", connections.size(), uniqueConnectionCount);
		int rec = serverSocket.receive(dataPacket, senderIp, senderPort);
		//printf("[Server] Received data...\n");
		if (rec == sf::Socket::Done) {
			// maintain connections based on data received (join or left)
			char *data = (char *)dataPacket.getData();
			dataPacket >> data;
			userInfo user;
			if (strcmp(data, "left game") == 0) {
				for (int i = 0; i < connections.size(); i++) {
					if (connections[i].ip == senderIp && connections[i].port == senderPort) {
						connections.erase(connections.begin() + i);
					}
				}
				continue;
			}
			else if (strcmp(data, "join game") == 0) {
				user.id = uniqueConnectionCount;
				user.ip = senderIp;
				user.port = senderPort;
				uniqueConnectionCount++;
				connections.push_back(user);
				sf::Packet successPacket;
				successPacket.clear();
				successPacket << "success" << user.id;
				if (serverSocket.send(successPacket, senderIp, senderPort)) {
					// failed sending "success"
				}
				continue;
			}
			// send data to every connection
			for (int i = 0; i < connections.size(); i++) {
				if (connections[i].ip != senderIp && connections[i].port != senderPort) {
					printf("[Server] Sending data...\n");
					if (serverSocket.send(dataPacket, connections[i].ip, connections[i].port) != sf::Socket::Done) {
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