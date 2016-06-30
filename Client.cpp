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
	// stop server
	stopReceivingPackets();
	for (int i = 0; i < otherPlayers.size(); i++) {
		delete otherPlayers[i].userCharacter;
		otherPlayers[i].userCharacter = NULL;
	}
}

void Client::operator=(const Client & other) {
	port = other.port;
	server = other.server;
	otherPlayers = std::vector<PlayerData>(0);
	if (other.running) {
		other.~Client();
	}
}

void Client::sendPacket(sf::UdpSocket & socket, sf::Packet & packet) {
	if (socket.send(packet, server.ip, server.port) != sf::Socket::Done) {
		// failed to send packet
	}
}

void Client::receivePackets(sf::UdpSocket & socket, int & kills, sf::Texture & rangerTexture, sf::Texture & mageTexture, sf::Texture & knightTexture, sf::Texture & arrowTexture, sf::Texture & fireballA, sf::Texture & fireballB, sf::Texture & swordTexture, sf::Texture & bowTexture, sf::Texture & staffTexture, sf::Texture & healthBarForegroundTexture, sf::Texture & healthBarBackgroundTexture) {
	socket.setBlocking(false);
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
	Knight knight(healthBarForegroundTexture, healthBarBackgroundTexture, knightTexture, swordTexture);
	Mage mage(healthBarForegroundTexture, healthBarBackgroundTexture, mageTexture, staffTexture, fireballA, fireballB);
	Ranger ranger(healthBarForegroundTexture, healthBarBackgroundTexture, rangerTexture, bowTexture, arrowTexture, arrowTexture);

	while (running) {
		packet.clear();
		int rec = socket.receive(packet, senderIp, senderPort);
		if (rec == sf::Socket::Done) {
			// manage packet data
			std::string data;
			packet >> data;
			if (strcmp(data.c_str(), "success") == 0) {
				packet >> clientIDfromServer >> timeLeftInGame >> gameNotInProgress;
				serverConnectionStatus = 2;
				printf("[Client] joined server as %s\n", clientIDfromServer.c_str());
				continue; // connected to server successfully
			}
			else if (serverConnectionStatus <= 1) {
				serverConnectionStatus = 1;
				continue;
			}
			else if (strcmp(data.c_str(), "kill") == 0) {
				std::string killerId;
				std::string killedId;
				packet >> killerId >> killedId >> timeLeftInGame >> gameNotInProgress;
				if (killerId == clientIDfromServer) {
					kills++;
				}
				for (int i = 0; i < otherPlayers.size(); i++) {
					if (killedId == otherPlayers[i].userID) {
						otherPlayers[i].isViable = false;
						break;
					}
				}
				continue;
			}
			else if (strcmp(data.c_str(), "left game") == 0) {
				// remove the player (that left) from the list of players
				std::string leftPlayerId;
				packet >> leftPlayerId;
				printf("[Client] %s left game\n", leftPlayerId.c_str());
				for (int i = 0; i < otherPlayers.size(); i++) {
					if (leftPlayerId == otherPlayers[i].userID) {
						otherPlayers[i].isViable = false;
						break;
					}
				}
				continue;
			}
			else if (strcmp(data.c_str(), "closed") == 0) {
				printf("[Client] server closed so returning to main menu in 3 seconds...\n");
				sf::sleep(sf::seconds(3));
				wasServerClosed = true;
				break;
			}
			std::string fighterName;
			packet >> fighterName;
			if (data == clientIDfromServer) {
				if (fighterName == "Knight") {
					knight.extractPacketToData(packet);
				}
				else if (fighterName == "Mage") {
					mage.extractPacketToData(packet);
				}
				else if (fighterName == "Ranger") {
					ranger.extractPacketToData(packet);
				}
				packet >> timeLeftInGame >> gameNotInProgress;
				continue;
			}
			int i;
			for (i = 0; i < otherPlayers.size(); i++) {
				if (data == otherPlayers[i].userID && otherPlayers[i].isViable) {
					if (fighterName == otherPlayers[i].fighterClass) {
						if (fighterName == "Knight" && otherPlayers[i].userCharacter != NULL) {
							((Knight *)(otherPlayers[i].userCharacter))->extractPacketToData(packet);
						}
						else if (fighterName == "Mage" && otherPlayers[i].userCharacter != NULL) {
							((Mage *)(otherPlayers[i].userCharacter))->extractPacketToData(packet);
						}
						else if (fighterName == "Ranger" && otherPlayers[i].userCharacter != NULL) {
							((Ranger *)(otherPlayers[i].userCharacter))->extractPacketToData(packet);
						}
					}
					else if (fighterName != otherPlayers[i].fighterClass && data != "") {
						otherPlayers[i].isViable = false;
						PlayerData userData;
						userData.userID = data;
						userData.fighterClass = fighterName;
						if (fighterName == "Knight") {
							userData.userCharacter = (Character *)new Knight(healthBarForegroundTexture, healthBarBackgroundTexture, knightTexture, swordTexture, 0.8f, 100, 90);
						}
						else if (fighterName == "Mage") {
							userData.userCharacter = (Character *)new Mage(healthBarForegroundTexture, healthBarBackgroundTexture, mageTexture, staffTexture, fireballA, fireballB, 0.7f, 3.0f, 0.7f, 0.3f);
						}
						else if (fighterName == "Ranger") {
							userData.userCharacter = (Character *)new Ranger(healthBarForegroundTexture, healthBarBackgroundTexture, rangerTexture, bowTexture, arrowTexture, arrowTexture, 0.7f, 3.0f, 0.7f, 0.3f);
						}
						userData.userCharacter->justAdded = true;
						userData.userCharacter->extractPacketToData(packet);
						packet >> timeLeftInGame >> gameNotInProgress;
						otherPlayers.push_back(userData);
					}
					packet >> timeLeftInGame >> gameNotInProgress;
					break;
				}
			}
			if (i == otherPlayers.size() && data != "") {
				PlayerData userData;
				userData.userID = data;
				userData.fighterClass = fighterName;
				if (fighterName == "Knight") {
					userData.userCharacter = (Character *)new Knight(healthBarForegroundTexture, healthBarBackgroundTexture, knightTexture, swordTexture, 0.8f, 100, 90);
				}
				else if (fighterName == "Mage") {
					userData.userCharacter = (Character *)new Mage(healthBarForegroundTexture, healthBarBackgroundTexture, mageTexture, staffTexture, fireballA, fireballB, 0.7f, 3.0f, 0.7f, 0.3f);
				}
				else if (fighterName == "Ranger") {
					userData.userCharacter = (Character *)new Ranger(healthBarForegroundTexture, healthBarBackgroundTexture, rangerTexture, bowTexture, arrowTexture, arrowTexture, 0.7f, 3.0f, 0.7f, 0.3f);
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

void Client::checkCollisions(Character * player, classTypes currentClass, sf::UdpSocket & socket, sf::Sound & takeDamageSound, sf::Sound & doDamageSound) {
	for (int i = 0; i < otherPlayers.size(); i++) {
		if (otherPlayers[i].isViable) {
			if (otherPlayers[i].fighterClass == "Knight") {
				if (otherPlayers[i].userCharacter != NULL && player != NULL && ((Knight *)(otherPlayers[i].userCharacter))->collisionSP(player->getCollisionCircle())) {
					if (!gameNotInProgress && !player->getIsDead() && player->takeDamage(((Knight *)(otherPlayers[i].userCharacter))->getDamage()) <= 0) {
						sf::Packet killPacket;
						killPacket.clear();
						killPacket << "kill" << otherPlayers[i].userID;
						sendPacket(socket, killPacket);
					}
					takeDamageSound.play();
					damageTakenVisualEffectTimer.restart();
				}
			}
			else if (otherPlayers[i].fighterClass == "Mage") {
				if (otherPlayers[i].userCharacter != NULL && player != NULL && ((Mage  *)(otherPlayers[i].userCharacter))->collisionPP(player->getCollisionCircle())) {
					if (!gameNotInProgress && !player->getIsDead() && player->takeDamage(((Mage *)(otherPlayers[i].userCharacter))->getDamage()) <= 0) {
						sf::Packet killPacket;
						killPacket.clear();
						killPacket << "kill" << otherPlayers[i].userID << clientIDfromServer;
						sendPacket(socket, killPacket);
					}
					takeDamageSound.play();
					damageTakenVisualEffectTimer.restart();
				}
			}
			else if (otherPlayers[i].fighterClass == "Ranger") {
				if (otherPlayers[i].userCharacter != NULL && player != NULL && ((Ranger  *)(otherPlayers[i].userCharacter))->collisionPP(player->getCollisionCircle())) {
					if (!gameNotInProgress && !player->getIsDead() && player->takeDamage(((Ranger *)(otherPlayers[i].userCharacter))->getDamage()) <= 0) {
						sf::Packet killPacket;
						killPacket.clear();
						killPacket << "kill" << otherPlayers[i].userID;
						sendPacket(socket, killPacket);
					}
					takeDamageSound.play();
					damageTakenVisualEffectTimer.restart();
				}
			}
			if (currentClass == knight && (((Knight *)player)->collisionSP(otherPlayers[i].userCharacter->getCollisionCircle()))) {
				doDamageSound.play();
			}
			else if (currentClass == mage && (((Mage *)player)->collisionPP(otherPlayers[i].userCharacter->getCollisionCircle()))) {
				doDamageSound.play();
			}
			else if (currentClass == ranger && (((Ranger *)player)->collisionPP(otherPlayers[i].userCharacter->getCollisionCircle()))) {
				doDamageSound.play();
			}
		}
	}
}

void Client::draw(sf::RenderWindow & window, sf::Vector2f playerPosition, Map *map) {
	if (damageTakenVisualEffectTimer.getElapsedTime().asSeconds() < damageTakenVisualTime) {
		sf::RectangleShape frontdrop((sf::Vector2f) windowSize);
		frontdrop.setFillColor(sf::Color(255, 0, 0, 25));
		frontdrop.setPosition(playerPosition - sf::Vector2f(windowSize.x / 2.0f, windowSize.y / 2.0f));
		window.draw(frontdrop);
	}
	for (int i = 0; i < otherPlayers.size(); i++) {
		if (!otherPlayers[i].isViable || otherPlayers[i].userID.find_first_of("Player") == std::string::npos || isDuplicate(otherPlayers[i].userID, i + 1)) {
			if (otherPlayers[i].fighterClass == "Knight" && otherPlayers[i].userCharacter != NULL) {
				delete (Knight *)(otherPlayers[i].userCharacter);
				otherPlayers[i].userCharacter = NULL;
			}
			else if (otherPlayers[i].fighterClass == "Mage" && otherPlayers[i].userCharacter != NULL) {
				delete (Mage *)(otherPlayers[i].userCharacter);
				otherPlayers[i].userCharacter = NULL;
			}
			else if (otherPlayers[i].fighterClass == "Ranger" && otherPlayers[i].userCharacter != NULL) {
				delete (Ranger *)(otherPlayers[i].userCharacter);
				otherPlayers[i].userCharacter = NULL;
			}
			otherPlayers.erase(otherPlayers.begin() + i);
			i--;
		}
		else {
			if (otherPlayers[i].fighterClass == "Knight" && otherPlayers[i].userCharacter != NULL) {
				((Knight *)(otherPlayers[i].userCharacter))->swingSword();
				((Knight *)(otherPlayers[i].userCharacter))->draw(window);
			}
			else if (otherPlayers[i].fighterClass == "Mage" && otherPlayers[i].userCharacter != NULL) {
				((Mage *)(otherPlayers[i].userCharacter))->shoot(window);
				map->collisionMProj(*(ProjectileShooter*)otherPlayers[i].userCharacter);
				((Mage *)(otherPlayers[i].userCharacter))->setWeapon(window);
				((Mage *)(otherPlayers[i].userCharacter))->drawProjectiles(window);
				((Mage *)(otherPlayers[i].userCharacter))->draw(window);
			}
			else if (otherPlayers[i].fighterClass == "Ranger" && otherPlayers[i].userCharacter != NULL) {
				((Ranger *)(otherPlayers[i].userCharacter))->shoot(window);
				map->collisionMProj(*(ProjectileShooter*)otherPlayers[i].userCharacter);
				((Ranger *)(otherPlayers[i].userCharacter))->setWeapon(window);
				((Ranger *)(otherPlayers[i].userCharacter))->drawProjectiles(window);
				((Ranger *)(otherPlayers[i].userCharacter))->draw(window);
			}
		}
	}
}

std::string Client::getClientId() {
	return clientIDfromServer;
}

bool Client::isGameInProgress() {
	return !gameNotInProgress;
}

bool Client::isDuplicate(std::string name, int start) {
	for (; start < otherPlayers.size(); start++) {
		if (otherPlayers[start].userID == name) {
			return true;
		}
	}
	return false;
}