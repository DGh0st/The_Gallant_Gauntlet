#include "Client.h"

Client::Client(sf::UdpSocket & socket, userInfo server) : server(server) {
	otherPlayers = std::vector<PlayerData>(0);
	if (server.ip != sf::IpAddress::None) {
		// setup port stuff
		if (socket.bind(sf::Socket::AnyPort) != sf::Socket::Done) {
			// failed to bind
		}
		port = socket.getLocalPort();
	}
}

Client::~Client() {
	// delete server info
	stopReceivingPackets();
}

void Client::operator=(const Client & other) {
	port = other.port;
	server = other.server;
	other.~Client();
}

void Client::sendPacket(sf::UdpSocket & socket, sf::Packet & packet) {
	if (socket.send(packet, server.ip, server.port) != sf::Socket::Done) {
		// failed to send packet
	}
}

void Client::receivePackets(sf::UdpSocket & socket) {
	// send "join game" packet to the server
	sf::Packet joinServerPacket;
	joinServerPacket.clear();
	joinServerPacket << "join game";
	sendPacket(socket, joinServerPacket);
	// setup variables and run receive
	sf::Packet packet;
	sf::IpAddress senderIp;
	unsigned short senderPort;

	while (server.ip != sf::IpAddress::None) {
		packet.clear();
		int rec = socket.receive(packet, senderIp, senderPort);
		/*if (senderIp != server.ip || senderPort != server.port) {
			continue; // got packet from wrong server so just ignore it
		}*/
		if (rec == sf::Socket::Done) {
			// manage packet data
			char *data = (char *)packet.getData();
			packet >> data >> clientIDfromServer;
			if (strcmp(data, "success") == 0) {
				printf("[Client] received success\n");
				continue; // connected to server successfully
			}
			int senderId;
			packet >> senderId;
			int i;
			for (i = 0; i < otherPlayers.size(); i++) {
				if (senderId == otherPlayers[i].userID) {
					otherPlayers[i].userCharacter.extractPacketToData(packet);
					break;
				}
			}
			if (i == otherPlayers.size()) {
				PlayerData userData;
				userData.userID = senderId;
				userData.userCharacter = Character();
				userData.userCharacter.extractPacketToData(packet);
				otherPlayers.push_back(userData);
			}
		}
	}
	// send "left game" packet to the server
	sf::Packet leftServerPacket;
	leftServerPacket.clear();
	leftServerPacket << "left game";
	sendPacket(socket, leftServerPacket);
	// unbind port
	socket.unbind();
}

void Client::stopReceivingPackets() {
	server = userInfo();
}

void Client::draw(sf::RenderWindow & window) {
	for (int i = 0; i < otherPlayers.size(); i++) {
		otherPlayers[i].userCharacter.draw(window);
	}
}