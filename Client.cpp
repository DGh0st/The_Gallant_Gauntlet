#include "Client.h"

Client::Client(userInfo server) : server(server) {
	// setup port stuff
	if (clientSocket.bind(sf::Socket::AnyPort) != sf::Socket::Done) {
		// failed to bind
	}
	port = clientSocket.getLocalPort();
	// send "join game" packet to the server
	sf::Packet joinServerPacket;
	joinServerPacket << "join game";
	sendPacket(joinServerPacket);
}

Client::~Client() {
	// unbind port and delete server info
	clientSocket.unbind();
	server = userInfo();
	// send "left game" packet to the server
	sf::Packet leftServerPacket;
	leftServerPacket << "left game";
	sendPacket(leftServerPacket);
}

void Client::operator=(const Client & other) {
	port = other.port;
	server = other.server;
	other.~Client();
	if (clientSocket.bind(port) != sf::Socket::Done) {
		// failed to bind
	}
}

void Client::sendPacket(sf::Packet & packet) {
	if (clientSocket.send(packet, server.ip, server.port) != sf::Socket::Done) {
		// failed to send packet
	}
}

void Client::receivePackets() {
	sf::Packet packet;
	sf::IpAddress senderIp;
	unsigned short senderPort;

	while (server.ip != sf::IpAddress::None) {
		int rec = clientSocket.receive(packet, senderIp, senderPort);
		if (senderIp != server.ip || senderPort != server.port) {
			continue; // got packet from wrong server so just ignore it
		}
		if (rec == sf::Socket::Done) {
			// manage packet data
		}
	}
}
