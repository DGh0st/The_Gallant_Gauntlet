#include "Client.h"
#include "Mage.h"
#include "Ranger.h"
#include "Knight.h"

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

void Client::receivePackets(sf::UdpSocket & socket, sf::Texture & rangerTexture, sf::Texture & mageTexture, sf::Texture & knightTexture, sf::Texture & arrowTexture, sf::Texture & fireballA, sf::Texture & fireballB, sf::Texture & swordTexture, sf::Texture & bowTexture, sf::Texture & staffTexture, sf::Texture & healthBarForegroundTexture, sf::Texture & healthBarBackgroundTexture) {
	// send "join game" packet to the server
	sf::Packet joinServerPacket;
	joinServerPacket.clear();
	joinServerPacket << "join game";
	sendPacket(socket, joinServerPacket);
	// setup variables and run receive
	sf::Packet packet;
	sf::IpAddress senderIp;
	unsigned short senderPort;
	bool wasServerClosed = false;
	running = true;

	while (running) {
		packet.clear();
		int rec = socket.receive(packet, senderIp, senderPort);
		if (rec == sf::Socket::Done) {
			// manage packet data
			sf::Packet copyPacket(packet);
			char *data = (char *)copyPacket.getData();
			copyPacket >> data;
			if (strcmp(data, "success") == 0) {
				copyPacket >> clientIDfromServer;
				copyPacket >> timeLeftInGame >> gameNotInProgress;
				printf("[Client] joined server as %s\n", clientIDfromServer.c_str());
				continue; // connected to server successfully
			}
			else if (strcmp(data, "left game") == 0) {
				// remove the player (that left) from the list of players
				std::string leftPlayerId;
				copyPacket >> leftPlayerId;
				printf("[Client] %s left game\n", leftPlayerId.c_str());
				for (int i = 0; i < otherPlayers.size(); i++) {
					if (leftPlayerId == otherPlayers[i].userID) {
						delete otherPlayers[i].userCharacter;
						otherPlayers.erase(otherPlayers.begin() + i);
						break;
					}
				}
				continue;
			}
			else if (strcmp(data, "closed") == 0) {
				printf("[Client] server closed so returning to main menu in 3 seconds...\n");
				sf::sleep(sf::seconds(3));
				wasServerClosed = true;
				break;
			}
			std::string senderId;
			packet >> senderId;
			std::string fighterName;
			packet >> fighterName;
			if (senderId == clientIDfromServer) {
				if (fighterName == "Knight") {
					Knight(healthBarForegroundTexture, healthBarBackgroundTexture, knightTexture, swordTexture).extractPacketToData(packet);
				}
				else if (fighterName == "Mage") {
					Mage(healthBarForegroundTexture, healthBarBackgroundTexture, mageTexture, staffTexture, fireballA, fireballB).extractPacketToData(packet);
				}
				else if (fighterName == "Ranger") {
					Ranger(healthBarForegroundTexture, healthBarBackgroundTexture, rangerTexture, bowTexture, arrowTexture, arrowTexture).extractPacketToData(packet);
				}
				packet >> timeLeftInGame >> gameNotInProgress;
				continue;
			}
			int i;
			for (i = 0; i < otherPlayers.size(); i++) {
				if (senderId == otherPlayers[i].userID) {
					if (fighterName == "Knight") {
						((Knight *)(otherPlayers[i].userCharacter))->extractPacketToData(packet);
					}
					else if (fighterName == "Mage") {
						((Mage *)(otherPlayers[i].userCharacter))->extractPacketToData(packet);
					}
					else if (fighterName == "Ranger") {
						((Ranger *)(otherPlayers[i].userCharacter))->extractPacketToData(packet);
					}
					packet >> timeLeftInGame >> gameNotInProgress;
					break;
				}
			}
			if (i == otherPlayers.size() && senderId != "") {
				PlayerData userData;
				userData.userID = senderId;
				userData.fighterClass = fighterName;
				if (fighterName == "Knight") {
					userData.userCharacter = new Knight(healthBarForegroundTexture, healthBarBackgroundTexture, knightTexture, swordTexture, 0.2f);
				}
				else if (fighterName == "Mage") {
					userData.userCharacter = new Mage(healthBarForegroundTexture, healthBarBackgroundTexture, mageTexture, staffTexture, fireballA, fireballB, 1.0f, 0.5f, 0.3f, 0.3f);
				}
				else if (fighterName == "Ranger") {
					userData.userCharacter = new Ranger(healthBarForegroundTexture, healthBarBackgroundTexture, rangerTexture, bowTexture, arrowTexture, arrowTexture, 0.3f, 0.5f, 0.3f, 0.3f);
				}
				userData.userCharacter->extractPacketToData(packet);
				packet >> timeLeftInGame >> gameNotInProgress;
				otherPlayers.push_back(userData);
				printf("[Client] %s joined game\n", userData.userID.c_str());
			}
		}
	}
	// send "left game" packet to the server if needed
	if (!wasServerClosed) {
		sf::Packet leftServerPacket;
		leftServerPacket.clear();
		leftServerPacket << "left game";
		sendPacket(socket, leftServerPacket);
	}
	// unbind port
	socket.unbind();
}

void Client::stopReceivingPackets() {
	running = false;
}

void Client::checkCollisions(Character * player) {
	for (int i = 0; i < otherPlayers.size(); i++) {
		if (otherPlayers[i].fighterClass == "Knight") {
			if (((Knight *)(otherPlayers[i].userCharacter))->collisionSP(player->getCollisionCircle())) {
				printf("[Client] Colliding\n");
				player->takeDamage(((Knight *)(otherPlayers[i].userCharacter))->getDamage());
			}
		}
		else if (otherPlayers[i].fighterClass == "Mage") {

		}
		else if (otherPlayers[i].fighterClass == "Ranger") {

		}
	}
}

void Client::draw(sf::RenderWindow & window) {
	for (int i = 0; i < otherPlayers.size(); i++) {
		if (otherPlayers[i].fighterClass == "Knight") {
			((Knight *)(otherPlayers[i].userCharacter))->swingSword();
			((Knight *)(otherPlayers[i].userCharacter))->draw(window);
		}
		else if (otherPlayers[i].fighterClass == "Mage") {
			((Mage *)(otherPlayers[i].userCharacter))->shoot(window);
			((Mage *)(otherPlayers[i].userCharacter))->setWeapon(window);
			((Mage *)(otherPlayers[i].userCharacter))->drawProjectiles(window);
			((Mage *)(otherPlayers[i].userCharacter))->draw(window);
		}
		else if (otherPlayers[i].fighterClass == "Ranger") {
			((Ranger *)(otherPlayers[i].userCharacter))->shoot(window);
			((Ranger *)(otherPlayers[i].userCharacter))->setWeapon(window);
			((Ranger *)(otherPlayers[i].userCharacter))->drawProjectiles(window);
			((Ranger *)(otherPlayers[i].userCharacter))->draw(window);
		}
	}
}

std::string Client::getClientId() {
	return clientIDfromServer;
}

bool Client::isGameInProgress() {
	return !gameNotInProgress;
}