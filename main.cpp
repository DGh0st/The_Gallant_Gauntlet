#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>
#include <thread>
#include <sstream>
#include "Defines.h"
#include "Character.h"
#include "Button.h"
#include "Textfield.h"
#include "Server.h"
#include "Client.h"
#include "ProjectileShooter.h"
#include "Ranger.h"
#include "Mage.h"
#include "Knight.h"

static screenTypes currentScreenDisplayed = title; // current displayed screen
bool screenReseted = false; // did leave game or server closed and screen returned to title
bool isSelectionScreenDisplayed = false; // is class selection overlay displayed currently

// info about server (from server side)
static Server runningServer; // holds the server itself, if running server on your pc
static bool isServerInitializedAndRunning = false;
std::thread serverThread; // thread server is running on

// info about server (from client side)
static userInfo serverInfo; // info about the server for the user
sf::UdpSocket clientSocket;
static Client runningClient = Client(clientSocket, serverInfo); // holds the client that connects to the server
std::thread clientThread; // thread client is running on

// get IP address and port from the user in joinScreen
static std::string connectToIPandPort = "";

// textures
sf::Texture rangerTexture, mageTexture, knightTexture, arrowTexture, fireballA, fireballB, swordTexture, bowTexture, staffTexture, healthBarForegroundTexture, healthBarBackgroundTexture;

// holds class of player (current and next respawn). 0 = knight, 1 = ranger, 2 = mage
classTypes respawnClass = knight; //default knight class to start
classTypes currentClass = knight;

// player
Character* player;

// kill and death counter
static int kills = 0, deaths = 0; // kill and deaths

void runClient() {
	runningClient.receivePackets(clientSocket, rangerTexture, mageTexture, knightTexture, arrowTexture, fireballA, fireballB, swordTexture, bowTexture, staffTexture, healthBarForegroundTexture, healthBarBackgroundTexture);
	if (currentClass == knight) {
		delete (Knight *)player;
	}
	else if (currentClass == ranger) {
		delete (Ranger *)player;
	}
	else if (currentClass == mage) {
		delete (Mage *)player;
	}
	currentScreenDisplayed = title;
	screenReseted = true;
}

void onPlayClick() {
	currentScreenDisplayed = joinServer;
}

void onCreateGameClick() {
	if (!isServerInitializedAndRunning) {
		runningServer = Server(9000);
		serverThread = std::thread(&Server::runServer, &runningServer);
		isServerInitializedAndRunning = true;
	}
	if (serverInfo.name != "Server" || screenReseted) {
		serverInfo.ip = sf::IpAddress::getLocalAddress();
		serverInfo.name = "Server";
		serverInfo.port = runningServer.getPort();
		runningClient = Client(clientSocket, serverInfo);
		if (screenReseted) {
			clientThread.join();
		}
		clientThread = std::thread(&runClient);
	}
	currentScreenDisplayed = createServer;
}

void onJoinGameClickFromClient() {
	if (connectToIPandPort.size() < 9) {
		// string shorter than 0.0.0.0:1
		return;
	}
	else if (connectToIPandPort.find_first_of(":", 0) == std::string::npos) {
		// port not included
		return;
	}
	if (serverInfo.name != "Server" || screenReseted) {
		// parse ip and port
		size_t offset = connectToIPandPort.find_first_of(":", 0);
		serverInfo.ip = sf::IpAddress(connectToIPandPort.substr(0, offset));
		serverInfo.name = "Server";
		serverInfo.port = (unsigned short)atoi(connectToIPandPort.substr(offset + 1, connectToIPandPort.length() - offset).c_str());
		// create client to join server
		runningClient = Client(clientSocket, serverInfo);
		if (screenReseted) {
			clientThread.join();
		}
		clientThread = std::thread(&runClient);
	}
	// reset kills and deaths
	kills = 0;
	deaths = 0;
	currentScreenDisplayed = ingame;
}

void onJoinGameClickFromServer() {
	// reset kills and deaths
	kills = 0;
	deaths = 0;
	currentScreenDisplayed = ingame;
}

void onKnightClick() {
	respawnClass = knight;
	isSelectionScreenDisplayed = false;
}

void onRangerClick() {
	respawnClass = ranger;
	isSelectionScreenDisplayed = false;
}

void onMageClick() {
	respawnClass = mage;
	isSelectionScreenDisplayed = false;
}

int main() {
	sf::RenderWindow window(sf::VideoMode(800, 500), "Game 2");
	windowSize = window.getSize();

	sf::Font font;
	font.loadFromFile("fonts/times.ttf");
	rangerTexture.loadFromFile("textures/ranger.png");
	mageTexture.loadFromFile("textures/mage.png");
	knightTexture.loadFromFile("textures/knight.png");
	arrowTexture.loadFromFile("textures/arrow.png");
	fireballA.loadFromFile("textures/fireballA.png");
	fireballB.loadFromFile("textures/fireballB.png");
	swordTexture.loadFromFile("textures/sword.png");
	bowTexture.loadFromFile("textures/bow.png");
	staffTexture.loadFromFile("textures/staff.png");
	healthBarForegroundTexture.loadFromFile("textures/healthBarForeground.png");
	healthBarBackgroundTexture.loadFromFile("textures/healthBarBackground.png");

	player = (Character*)(new Knight(healthBarForegroundTexture, healthBarBackgroundTexture, knightTexture, swordTexture, 0.2f)); //default knight class to start
	player->setIsPlayer(true);

	sf::View playerView = sf::View(player->getCenter(), (sf::Vector2f)windowSize);
	sf::View normalView = window.getView();

	sf::Keyboard::Key releasedKey = sf::Keyboard::Unknown;

	Textfield joinTF(font, sf::Vector2f(650.0f, 50.0f));
	Button playButton, createButton, joinButton, knightButton, rangerButton, mageButton;

	while (window.isOpen()) {
		releasedKey = sf::Keyboard::Unknown; // reset released

		sf::Event event;
		while (window.pollEvent(event)) {
			if (event.type == sf::Event::Closed || (event.type == sf::Event::KeyReleased && event.key.code == sf::Keyboard::Escape)) {
				runningClient.stopReceivingPackets();
				sf::Packet leavingPacket;
				leavingPacket.clear();
				leavingPacket << "left game";
				runningClient.sendPacket(clientSocket, leavingPacket);
				runningServer.stopServer();
				window.close();
			}
			else if (currentScreenDisplayed == ingame && event.type == sf::Event::KeyReleased) {
				sf::Keyboard::Key tempKey = event.key.code;
				if (tempKey == moveUpKey || tempKey == moveDownKey || tempKey == moveLeftKey || tempKey == moveRightKey) {
					releasedKey = tempKey;
				}
			}

			if (currentScreenDisplayed == title) {
				playButton.handleEvent(event);
				createButton.handleEvent(event);
			}
			else if (currentScreenDisplayed == joinServer) {
				joinTF.handleEvent(event);
				joinButton.handleEvent(event);
			} 
			else if (currentScreenDisplayed == createServer) {
				joinButton.handleEvent(event);
			}
			else if (currentScreenDisplayed == ingame) {
				if (isSelectionScreenDisplayed) {
					knightButton.handleEvent(event);
					rangerButton.handleEvent(event);
					mageButton.handleEvent(event);
				}
				if (event.type == sf::Event::KeyReleased && event.key.code == classSelectionDisplayKey) {
					isSelectionScreenDisplayed = !isSelectionScreenDisplayed; // toogles
				}
			}
		}

		window.clear();

		if (currentScreenDisplayed == title) {
			window.setView(normalView);
			//title text
			sf::Text titleText("Title", font, 64U);
			titleText.setOrigin(titleText.getGlobalBounds().width / 2, titleText.getGlobalBounds().height / 2);
			titleText.setPosition(sf::Vector2f(windowSize.x / 2.0f, windowSize.y / 2.0f - 128.0f));
			window.draw(titleText);
			//play button
			sf::Text playText("Play", font, 32U);
			playText.setColor(sf::Color::Black);
			playButton = Button(playText, sf::Vector2f(100.0f, 75.0f), onPlayClick);
			playButton.setOrigin(playButton.getGlobalBounds().width / 2.0f, playButton.getGlobalBounds().height / 2.0f);
			playButton.setPosition(sf::Vector2f(windowSize.x / 2.0f, windowSize.y / 2.0f));
			playButton.draw(window);
			//create game button
			sf::Text createText("Create Game", font, 32U);
			createText.setColor(sf::Color::Black);
			createButton = Button(createText, sf::Vector2f(200.0f, 75.0f), onCreateGameClick);
			createButton.setOrigin(createButton.getGlobalBounds().width / 2.0f, createButton.getGlobalBounds().height / 2.0f);
			createButton.setPosition(sf::Vector2f(windowSize.x / 2.0f, windowSize.y / 2.0f + 128.0f));
			createButton.draw(window);
		}
		else if (currentScreenDisplayed == joinServer) {
			window.setView(normalView);
			// set joinTF's string to connectToIPandPort
			connectToIPandPort = joinTF.getString();
			//server address text
			sf::Text serverText("Server Address", font, 16);
			serverText.setOrigin(serverText.getGlobalBounds().width / 2.0f, serverText.getGlobalBounds().height / 2.0f);
			serverText.setPosition(windowSize.x / 2.0f, windowSize.y / 2.0f - serverText.getGlobalBounds().height * 4.0f);
			serverText.setColor(sf::Color::Cyan);
			//join text field
			joinTF.setOrigin(joinTF.getGlobalBounds().width / 2.0f, joinTF.getGlobalBounds().height / 2.0f);
			joinTF.setPosition(windowSize.x / 2.0f, windowSize.y / 2.0f);
			//join game button
			sf::Text joinText("Join Game", font);
			joinText.setColor(sf::Color::Black);
			joinButton = Button(joinText, sf::Vector2f(200.0f, 75.0f), onJoinGameClickFromClient);
			joinButton.setOrigin(joinButton.getGlobalBounds().width / 2.0f, joinButton.getGlobalBounds().height / 2.0f);
			joinButton.setPosition(windowSize.x / 2.0f, windowSize.y / 2.0f + joinButton.getGlobalBounds().height * 2.0f);
			// draw
			window.draw(serverText);
			joinTF.draw(window);
			joinButton.draw(window);
		}
		else if (currentScreenDisplayed == createServer) {
			window.setView(normalView);
			//ip and instructions on joining a server text
			std::ostringstream ipString;
			ipString << "Please use " << sf::IpAddress::getPublicAddress().toString() << ":" << runningServer.getPort() << " to connect to the server.";
			sf::Text ipText(ipString.str(), font, 20);
			ipText.setOrigin(ipText.getGlobalBounds().width / 2.0f, ipText.getGlobalBounds().height / 2.0f);
			ipText.setPosition(window.getSize().x / 2.0f, window.getSize().y / 2.0f - ipText.getGlobalBounds().height * 10.0f);
			ipText.setColor(sf::Color::Cyan);
			//join game button
			sf::Text joinText("Join Game", font);
			joinText.setColor(sf::Color::Black);
			joinButton = Button(joinText, sf::Vector2f(200.0f, 75.0f), onJoinGameClickFromServer);
			joinButton.setOrigin(joinButton.getGlobalBounds().width / 2.0f, joinButton.getGlobalBounds().height / 2.0f);
			joinButton.setPosition(sf::Vector2f(windowSize.x / 2.0f, windowSize.y / 2.0f));
			//draw
			joinButton.draw(window);
			window.draw(ipText);
		}
		else if (currentScreenDisplayed == ingame) {
			// center the view on player
			playerView.setCenter(player->getCenter());
			window.setView(playerView);
			// kills and deaths at top right corner text
			std::ostringstream killsDeathString;
			killsDeathString << "Kills: " << kills << " Deaths: " << deaths;
			sf::Text playerStats(killsDeathString.str(), font, 15U);
			playerStats.setOrigin(playerStats.getGlobalBounds().width, 0.0f);
			playerStats.setPosition(sf::Vector2f(player->getCenter().x + windowSize.x / 2.0f - 10.0f, player->getCenter().y - windowSize.y / 2.0f));
			// game time displayed in the center text
			std::ostringstream gameTimeString;
			int mins = (int)runningClient.timeLeftInGame / 60;
			int seconds = (int)runningClient.timeLeftInGame % 60;
			if (mins < 10) {
				gameTimeString << "0";
			}
			gameTimeString << mins << ":";
			if (seconds < 10) {
				gameTimeString << "0";
			}
			gameTimeString << seconds;
			//char gameTimeString[] = "00:00";
			//sprintf_s(gameTimeString, sizeof(gameTimeString), "%.2d:%.2d", (int)runningClient.timeLeftInGame / 60, (int)runningClient.timeLeftInGame % 60);
			sf::Text gameTimeText(gameTimeString.str(), font, 30U);
			gameTimeText.setOrigin(gameTimeText.getGlobalBounds().width / 2.0f, 0.0f);
			gameTimeText.setPosition(sf::Vector2f(player->getCenter().x, player->getCenter().y - windowSize.y / 2.0f));
			if (window.hasFocus() && !isSelectionScreenDisplayed && !player->getIsDead()) {
				if (runningClient.isGameInProgress()) {
					// check collision
					runningClient.checkCollisions(player, currentClass);
				}
				// player movement and send that data to server	
				sf::Packet packet;
				packet.clear();
				if (currentClass == knight) {
					((Knight*)player)->swingSword();
					((Knight*)player)->move(window, releasedKey);
					packet = ((Knight*)player)->chainDataToPacket(packet, runningClient.getClientId());
				}
				else if (currentClass == ranger) {
					((Ranger*)player)->move(window, releasedKey);
					((Ranger*)player)->shoot(window);
					((Ranger*)player)->setWeapon(window);
					packet = ((Ranger*)player)->chainDataToPacket(packet, runningClient.getClientId());
			    }
				else if (currentClass == mage) {
					((Mage*)player)->move(window, releasedKey);
					((Mage*)player)->shoot(window);
					((Mage*)player)->setWeapon(window);
					packet = ((Mage*)player)->chainDataToPacket(packet, runningClient.getClientId());
				}
				runningClient.sendPacket(clientSocket, packet);
			}
			// draw
			runningClient.draw(window);	
			if (currentClass == knight) {
				((Knight*)player)->draw(window);
			}
			else if (currentClass == ranger) {
				((Ranger*)player)->drawProjectiles(window);
				((Ranger*)player)->draw(window);
			}
			else if (currentClass == mage) {
				((Mage*)player)->drawProjectiles(window);
				((Mage*)player)->draw(window);
			}
			window.draw(playerStats);
			window.draw(gameTimeText);
			if (!runningClient.isGameInProgress()) {
				// Waiting for game to start text
				sf::Text waitingText("Waiting for game to being...", font, 15U);
				waitingText.setOrigin(waitingText.getGlobalBounds().width / 2.0f, 0.0f);
				waitingText.setPosition(sf::Vector2f(player->getCenter().x, gameTimeText.getGlobalBounds().top + gameTimeText.getGlobalBounds().height + waitingText.getGlobalBounds().height / 2.0f));
				// instructions to change class text
				sf::Text classSelectionText("Press 'H' to change class", font, 18U);
				classSelectionText.setOrigin(classSelectionText.getGlobalBounds().width / 2.0f, 0.0f);
				classSelectionText.setPosition(sf::Vector2f(player->getCenter().x, waitingText.getGlobalBounds().top + waitingText.getGlobalBounds().height + classSelectionText.getGlobalBounds().height / 2.0f));
				// draw
				window.draw(waitingText);
				window.draw(classSelectionText);
			}
			if (player->getIsDead() || (!runningClient.isGameInProgress() && currentClass != respawnClass && runningClient.timeLeftInGame > 5.0f)) {
				if (runningClient.isGameInProgress()) {
					window.setView(normalView);
					// backdrop
					sf::RectangleShape backdrop((sf::Vector2f) windowSize);
					backdrop.setFillColor(sf::Color(255, 0, 0, 150));
					window.draw(backdrop);
					// respawn text
					std::ostringstream respawnTimeString;
					respawnTimeString << "Respawning in " << respawnTime - player->getTimeAsDead() << " seconds";
					sf::Text respawnText(respawnTimeString.str(), font, 32U);
					respawnText.setOrigin(respawnText.getGlobalBounds().width / 2.0f, respawnText.getGlobalBounds().height / 2.0f);
					respawnText.setPosition(windowSize.x / 2.0f, windowSize.y / 2.0f);
					window.draw(respawnText);
					// class selection text
					sf::Text classSelectionText("Press 'H' to select class", font, 15U);
					classSelectionText.setOrigin(classSelectionText.getGlobalBounds().width / 2.0f, classSelectionText.getGlobalBounds().height / 2.0f);
					classSelectionText.setPosition(windowSize.x / 2.0f, windowSize.y / 2.0f + respawnText.getGlobalBounds().height);
					window.draw(classSelectionText);
				}
				// respawn when respawnTime spent as dead or in peace time
				if (player->getTimeAsDead() > respawnTime || !runningClient.isGameInProgress()) {
					if (currentClass == knight) {
						delete (Knight *)player;
					}
					else if (currentClass == ranger) {
						delete (Ranger *)player;
					}
					else if (currentClass == mage) {
						delete (Mage *)player;
					}
					if (respawnClass == knight) {
						player = (Character*)(new Knight(healthBarForegroundTexture, healthBarBackgroundTexture, knightTexture, swordTexture, 0.2f));
						currentClass = knight;
					}
					else if (respawnClass == ranger) {
						player = (Character*)(new Ranger(healthBarForegroundTexture, healthBarBackgroundTexture, rangerTexture, bowTexture, arrowTexture, arrowTexture, 0.3f, 0.5f, 0.3f, 0.3f));
						currentClass = ranger;
					}
					else if (respawnClass == mage) {
						player = (Character*)(new Mage(healthBarForegroundTexture, healthBarBackgroundTexture, mageTexture, staffTexture, fireballA, fireballB, 1.0f, 0.5f, 0.3f, 0.3f));
						currentClass = mage;
					}
					player->setIsPlayer(true);
					if (runningClient.isGameInProgress()) {
						deaths++;
					}
				}
			}
			if (isSelectionScreenDisplayed) {
				window.setView(normalView);
				//backdrop
				sf::RectangleShape backdrop((sf::Vector2f) windowSize);
				backdrop.setFillColor(sf::Color(0, 0, 0, 150));
				window.draw(backdrop);
				//title text
				sf::Text titleText("Class Selection", font, 64U);
				titleText.setOrigin(titleText.getGlobalBounds().width / 2, titleText.getGlobalBounds().height / 2);
				titleText.setPosition(sf::Vector2f(windowSize.x / 2.0f, windowSize.y / 2.0f - 200.0f));
				window.draw(titleText);
				//knight button
				sf::Text knightText("Knight", font, 32U);
				knightText.setColor(sf::Color::Black);
				knightButton = Button(knightText, sf::Vector2f(150.0f, 75.0f), onKnightClick);
				knightButton.setOrigin(knightButton.getGlobalBounds().width / 2.0f, knightButton.getGlobalBounds().height / 2.0f);
				knightButton.setPosition(sf::Vector2f(windowSize.x / 2.0f - 250.0f, windowSize.y / 2.0f + 150.0f));
				window.draw(knightText);
				knightButton.draw(window);
				//ranger button
				sf::Text rangerText("Ranger", font, 32U);
				rangerText.setColor(sf::Color::Black);
				rangerButton = Button(rangerText, sf::Vector2f(150.0f, 75.0f), onRangerClick);
				rangerButton.setOrigin(rangerButton.getGlobalBounds().width / 2.0f, rangerButton.getGlobalBounds().height / 2.0f);
				rangerButton.setPosition(sf::Vector2f(windowSize.x / 2.0f, windowSize.y / 2.0f + 150.0f));
				window.draw(rangerText);
				rangerButton.draw(window);
				//mage button
				sf::Text mageText("Mage", font, 32U);
				mageText.setColor(sf::Color::Black);
				mageButton = Button(mageText, sf::Vector2f(150.0f, 75.0f), onMageClick);
				mageButton.setOrigin(mageButton.getGlobalBounds().width / 2.0f, mageButton.getGlobalBounds().height / 2.0f);
				mageButton.setPosition(sf::Vector2f(windowSize.x / 2.0f + 250.0f, windowSize.y / 2.0f + 150.0f));
				window.draw(mageText);
				mageButton.draw(window);
				//knight sprite
				sf::Sprite knightSprite(knightTexture);
				knightSprite.setOrigin(knightSprite.getGlobalBounds().width / 2.0f, knightSprite.getGlobalBounds().height / 2.0f);
				knightSprite.setPosition(sf::Vector2f(windowSize.x / 2.0f - 250.0f, windowSize.y / 2.0f + 25.0f));
				knightSprite.setRotation(knightSprite.getRotation() + 90);
				window.draw(knightSprite);
				//ranger sprite
				sf::Sprite rangerSprite(rangerTexture);
				rangerSprite.setOrigin(rangerSprite.getGlobalBounds().width / 2.0f, rangerSprite.getGlobalBounds().height / 2.0f);
				rangerSprite.setPosition(sf::Vector2f(windowSize.x / 2.0f, windowSize.y / 2.0f + 25.0f));
				rangerSprite.setRotation(rangerSprite.getRotation() + 91);
				window.draw(rangerSprite);
				//mage sprite
				sf::Sprite mageSprite(mageTexture);
				mageSprite.setOrigin(mageSprite.getGlobalBounds().width / 2.0f, mageSprite.getGlobalBounds().height / 2.0f);
				mageSprite.setPosition(sf::Vector2f(windowSize.x / 2.0f + 250.0f, windowSize.y / 2.0f + 25.0f));
				mageSprite.setRotation(mageSprite.getRotation() + 100);
				window.draw(mageSprite);
			}
		}

		window.display();
	}

	if (isServerInitializedAndRunning) {
		runningServer.stopServer();
		isServerInitializedAndRunning = false;
		serverThread.join();
	}

	if (serverInfo.name == "Server") {
		runningClient.stopReceivingPackets();
		clientThread.join();
		serverInfo = userInfo();
	}

	return 0;
}