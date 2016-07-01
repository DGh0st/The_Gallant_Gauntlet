#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>
#include <SFML/Audio.hpp>
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
#include "Map.h"

static screenTypes currentScreenDisplayed = title; // current displayed screen
bool screenReseted = false; // did leave game or server closed and screen returned to title
bool isSelectionScreenDisplayed = false; // is class selection overlay displayed currently
bool isEscapeScreenDisplayed = false; // is escape overlay displayed currently
bool shouldCloseWindow = false; // should close window because exit was clicked or not

// info about server (from server side)
static Server runningServer; // holds the server itself, if running server on your pc
static bool isServerInitializedAndRunning = false;
std::thread serverThread; // thread server is running on

// info about server (from client side)
static userInfo serverInfo; // info about the server for the user
sf::UdpSocket clientSocket;
static Client runningClient = Client(clientSocket, serverInfo); // holds the client that connects to the server
std::thread clientThread; // thread client is running on
bool didRestartTimeoutTimer = false;
sf::Clock timeoutTimer; // timer for stopping connection look up once it reaches timeout seconds

// get IP address and port from the user in joinScreen
static std::string connectToIPandPort = "";

// textures, sound, and music
sf::Texture rangerTexture, mageTexture, knightTexture, arrowTexture, fireballA, fireballB, swordTexture, bowTexture, staffTexture, healthBarForegroundTexture, healthBarBackgroundTexture, backgroundTexture, mapTexture, titleTexture, buttonTexture;
sf::Image icon;
sf::SoundBuffer takeDamageSoundBuffer, doDamageKSoundBuffer, doDamageRSoundBuffer, doDamageMSoundBuffer;
sf::Music titleMusic,ingameMusic;

// holds class of player (current and next respawn). 0 = knight, 1 = ranger, 2 = mage
classTypes respawnClass = knight; //default knight class to start
classTypes currentClass = knight;
sf::Clock classChangeCooldown;

// player
Character *player = NULL;

// counters
int kills = 0, deaths = 0; // kill and deaths


// map
Map* map;

void runClient() {
	runningClient.receivePackets(clientSocket, kills, rangerTexture, mageTexture, knightTexture, arrowTexture, fireballA, fireballB, swordTexture, bowTexture, staffTexture, healthBarForegroundTexture, healthBarBackgroundTexture);
	screenReseted = true;
	isSelectionScreenDisplayed = false;
	isEscapeScreenDisplayed = false;
	if (runningClient.serverConnectionStatus == 2) {
		currentScreenDisplayed = title;
		runningClient.serverConnectionStatus = 0;
	}
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
		// string shorter than 0.0.0.0:0
		return;
	}
	else if (connectToIPandPort.find_first_of(":", 0) == std::string::npos) {
		// port not included
		return;
	}
	if (serverInfo.name != "Server" || screenReseted || runningClient.serverConnectionStatus <= 1) {
		if ((screenReseted || runningClient.serverConnectionStatus <= 1) && serverInfo.name == "Server") {
			// stop client and try again
			runningClient.stopReceivingPackets();
			clientThread.join();
		}
		// parse ip and port
		size_t offset = connectToIPandPort.find_first_of(":", 0);
		serverInfo.ip = sf::IpAddress(connectToIPandPort.substr(0, offset));
		serverInfo.name = "Server";
		serverInfo.port = (unsigned short)atoi(connectToIPandPort.substr(offset + 1, connectToIPandPort.length() - offset).c_str());
		// create client to join server
		runningClient = Client(clientSocket, serverInfo);
		runningClient.serverConnectionStatus = 0;
		clientThread = std::thread(&runClient);
	}
	didRestartTimeoutTimer = true;
	timeoutTimer.restart();
}

void onJoinGameClickFromServer() {
	// reset kills and deaths
	kills = 0;
	deaths = 0;
	currentScreenDisplayed = ingame;
	titleMusic.stop();
	ingameMusic.play();
}

void onBackClickFromClient() {
	currentScreenDisplayed = title;
}

void onBackClickFromServer() {
	runningClient.stopReceivingPackets();
	sf::Packet leavingPacket;
	leavingPacket.clear();
	leavingPacket << "left game";
	runningClient.sendPacket(clientSocket, leavingPacket);
	if (isServerInitializedAndRunning) {
		runningServer.stopServer();
		serverThread.join();
		isServerInitializedAndRunning = false;
	}
	currentScreenDisplayed = title;
}

void onKnightClick() {
	if (classChangeCooldown.getElapsedTime().asSeconds() < classChangeCooldownTime) {
		return;
	}
	respawnClass = knight;
	isSelectionScreenDisplayed = false;
	classChangeCooldown.restart();
}

void onRangerClick() {
	if (classChangeCooldown.getElapsedTime().asSeconds() < classChangeCooldownTime) {
		return;
	}
	respawnClass = ranger;
	isSelectionScreenDisplayed = false;
	classChangeCooldown.restart();
}

void onMageClick() {
	if (classChangeCooldown.getElapsedTime().asSeconds() < classChangeCooldownTime) {
		return;
	}
	respawnClass = mage;
	isSelectionScreenDisplayed = false;
	classChangeCooldown.restart();
}

void onExitClickFromTitle() {
	runningClient.stopReceivingPackets();
	sf::Packet leavingPacket;
	leavingPacket.clear();
	leavingPacket << "left game";
	runningClient.sendPacket(clientSocket, leavingPacket);
	if (isServerInitializedAndRunning) {
		runningServer.stopServer();
		serverThread.join();
		isServerInitializedAndRunning = false;
	}
	if (serverInfo.name == "Server") {
		clientThread.join();
		serverInfo = userInfo();
	}
	shouldCloseWindow = true;
}

void onExitClickFromGame() {
	runningClient.stopReceivingPackets();
	sf::Packet leavingPacket;
	leavingPacket.clear();
	leavingPacket << "left game";
	runningClient.sendPacket(clientSocket, leavingPacket);
	if (isServerInitializedAndRunning) {
		runningServer.stopServer();
		serverThread.join();
		isServerInitializedAndRunning = false;
	}
	currentScreenDisplayed = title;
	titleMusic.play();
}

int main() {
	sf::RenderWindow window(sf::VideoMode(1600, 900), "Game 2", sf::Style::Close | sf::Style::Titlebar);
	windowSize = window.getSize();
	icon.loadFromFile("textures/icon.png");
	window.setIcon(32, 32, icon.getPixelsPtr());
	srand((unsigned int)time(NULL));

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
	backgroundTexture.loadFromFile("textures/titleBackground.png");
	mapTexture.loadFromFile("textures/map.png");
	titleTexture.loadFromFile("textures/title.png");
	buttonTexture.loadFromFile("textures/button.png");
	takeDamageSoundBuffer.loadFromFile("sounds/takeDamage.wav");
	doDamageKSoundBuffer.loadFromFile("sounds/swordHit.wav");
	doDamageRSoundBuffer.loadFromFile("sounds/arrowHit.wav");
	doDamageMSoundBuffer.loadFromFile("sounds/fireballHit.wav");
	titleMusic.openFromFile("sounds/titleMusic.ogg");
	ingameMusic.openFromFile("sounds/ingameMusic.ogg");
	titleMusic.setLoop(true);
	ingameMusic.setLoop(true);
	titleMusic.play();
	sf::Sound takeDamageSound(takeDamageSoundBuffer);
	sf::Sound doDamageKSound(doDamageKSoundBuffer);
	sf::Sound doDamageRSound(doDamageRSoundBuffer);
	sf::Sound doDamageMSound(doDamageMSoundBuffer);

	sf::RectangleShape backgroundRect((sf::Vector2f)windowSize);
	backgroundRect.setTexture(&backgroundTexture);

	sf::View playerView;
	sf::View normalView = window.getView();

	map = new Map(mapTexture, sf::Vector2f(0, 0));

	player = (Character*)(new Knight(healthBarForegroundTexture, healthBarBackgroundTexture, knightTexture, swordTexture, 0.8f, 125, 22, 37)); //default knight class to start
	if (player != NULL) {
		player->setIsPlayer(true);
		playerView = sf::View(player->getCenter(), (sf::Vector2f)windowSize);
	}


	sf::Keyboard::Key releasedKey = sf::Keyboard::Unknown;

	Textfield joinTF(font, sf::Vector2f(310.0f, 50.0f));
	Button playButton, createButton, joinButton, joinLocalButton, stopServerButton, knightButton, rangerButton, mageButton, backButton, exitButton, leaveButton;
	
	// -- title screen --
	//title text
	sf::Sprite titleSprite(titleTexture);
	titleSprite.setOrigin(titleSprite.getGlobalBounds().width / 2.0f, titleSprite.getGlobalBounds().height / 2.0f);
	//play button
	sf::Text playText("Play", font, 32U);
	playText.setColor(sf::Color::Black);
	playButton = Button(playText, sf::Vector2f(100.0f, 70.0f), onPlayClick);
	playButton.setOrigin(playButton.getGlobalBounds().width / 2.0f, playButton.getGlobalBounds().height / 2.0f);
	playButton.setTexture(&buttonTexture);
	//create game button
	sf::Text createText("Create Game", font, 32U);
	createText.setColor(sf::Color::Black);
	createButton = Button(createText, sf::Vector2f(200.0f, 70.0f), onCreateGameClick);
	createButton.setOrigin(createButton.getGlobalBounds().width / 2.0f, createButton.getGlobalBounds().height / 2.0f);
	createButton.setTexture(&buttonTexture);
	// exit button
	sf::Text exitText("Exit", font, 32U);
	exitText.setColor(sf::Color::Black);
	exitButton = Button(exitText, sf::Vector2f(100.0f, 70.0f), onExitClickFromTitle);
	exitButton.setOrigin(exitButton.getGlobalBounds().width / 2.0f, exitButton.getGlobalBounds().height / 2.0f);
	exitButton.setTexture(&buttonTexture);

	// -- Join Server Screen --
	// back button
	sf::Text backText("< Back", font, 16U);
	backText.setColor(sf::Color::Black);
	backButton = Button(backText, sf::Vector2f(80.0f, 30.0f), onBackClickFromClient);
	backButton.setOrigin(0.0f, 0.0f);
	backButton.setTexture(&buttonTexture);
	//server address text
	sf::Text serverText("Server Address", font, 24U);
	serverText.setOrigin(serverText.getGlobalBounds().width / 2.0f, serverText.getGlobalBounds().height / 2.0f);
	serverText.setColor(sf::Color::White);
	//join text field
	joinTF.setOrigin(joinTF.getGlobalBounds().width / 2.0f, joinTF.getGlobalBounds().height / 2.0f);
	//join game button
	sf::Text joinText("Join Game", font, 32U);
	joinText.setColor(sf::Color::Black);
	joinButton = Button(joinText, sf::Vector2f(200.0f, 75.0f), onJoinGameClickFromClient);
	joinButton.setOrigin(joinButton.getGlobalBounds().width / 2.0f, joinButton.getGlobalBounds().height / 2.0f);
	joinButton.setTexture(&buttonTexture);
	// attempting to connect... string
	sf::Text connecting("Attempting to connect...", font, 16U);
	connecting.setOrigin(connecting.getGlobalBounds().width / 2.0f, connecting.getGlobalBounds().height / 2.0f);
	connecting.setColor(sf::Color::White);
	// failed to connect... string
	sf::Text failedConnection("Failed to connect to the server", font, 16U);
	failedConnection.setOrigin(failedConnection.getGlobalBounds().width / 2.0f, connecting.getGlobalBounds().height / 2.0f);
	failedConnection.setColor(sf::Color::White);

	// -- Create Server Screen --
	// back button
	sf::Text stopServerText("< Stop Server", font, 16U);
	stopServerText.setColor(sf::Color::Black);
	stopServerButton = Button(stopServerText, sf::Vector2f(120.0f, 32.0f), onBackClickFromServer);
	stopServerButton.setOrigin(0.0f, 0.0f);
	stopServerButton.setTexture(&buttonTexture);
	//join game button
	sf::Text joinLocalText("Join Game", font);
	joinLocalText.setColor(sf::Color::Black);
	joinLocalButton = Button(joinLocalText, sf::Vector2f(200.0f, 75.0f), onJoinGameClickFromServer);
	joinLocalButton.setOrigin(joinLocalButton.getGlobalBounds().width / 2.0f, joinLocalButton.getGlobalBounds().height / 3.2f);
	joinLocalButton.setTexture(&buttonTexture);

	// -- peace mode in game screen --
	// Waiting for game to start text
	sf::Text waitingText("Waiting for game to begin...", font, 15U);
	waitingText.setOrigin(waitingText.getGlobalBounds().width / 2.0f, 0.0f);
	// instructions to change class text
	sf::Text classSelectionText("Press 'H' to change class", font, 18U);
	classSelectionText.setOrigin(classSelectionText.getGlobalBounds().width / 2.0f, 0.0f);

	// -- death in game screen --
	// backdrop
	sf::RectangleShape deathBackdrop((sf::Vector2f) windowSize);
	deathBackdrop.setFillColor(sf::Color(255, 0, 0, 150));
	// class selection text
	sf::Text classSelectionInstructionsText("Press 'H' to select class", font, 15U);
	classSelectionInstructionsText.setOrigin(classSelectionInstructionsText.getGlobalBounds().width / 2.0f, classSelectionInstructionsText.getGlobalBounds().height / 2.0f);

	// -- class selection overlay in game screen --
	//backdrop
	sf::RectangleShape selectionbackdrop((sf::Vector2f) windowSize);
	selectionbackdrop.setFillColor(sf::Color(0, 0, 0, 150));
	//title text
	sf::Text classTitleText("Class Selection", font, 64U);
	classTitleText.setOrigin(classTitleText.getGlobalBounds().width / 2, classTitleText.getGlobalBounds().height / 2);
	classTitleText.setPosition(sf::Vector2f(windowSize.x / 2.0f, windowSize.y / 2.0f - classTitleText.getGlobalBounds().height * 4.0f));
	//knight button
	sf::Text knightText("Knight", font, 32U);
	knightText.setColor(sf::Color::Black);
	knightButton = Button(knightText, sf::Vector2f(150.0f, 75.0f), onKnightClick);
	knightButton.setOrigin(knightButton.getGlobalBounds().width / 2.0f, knightButton.getGlobalBounds().height / 2.0f);
	knightButton.setPosition(sf::Vector2f(windowSize.x / 2.0f - knightButton.getGlobalBounds().width * 2.0f, windowSize.y / 2.0f + knightButton.getGlobalBounds().height * 2.0f));
	knightButton.setTexture(&buttonTexture);
	//ranger button
	sf::Text rangerText("Ranger", font, 32U);
	rangerText.setColor(sf::Color::Black);
	rangerButton = Button(rangerText, sf::Vector2f(150.0f, 75.0f), onRangerClick);
	rangerButton.setOrigin(rangerButton.getGlobalBounds().width / 2.0f, rangerButton.getGlobalBounds().height / 2.0f);
	rangerButton.setPosition(sf::Vector2f(windowSize.x / 2.0f, windowSize.y / 2.0f + rangerButton.getGlobalBounds().height * 2.0f));
	rangerButton.setTexture(&buttonTexture);
	//mage button
	sf::Text mageText("Mage", font, 32U);
	mageText.setColor(sf::Color::Black);
	mageButton = Button(mageText, sf::Vector2f(150.0f, 75.0f), onMageClick);
	mageButton.setOrigin(mageButton.getGlobalBounds().width / 2.0f, mageButton.getGlobalBounds().height / 2.0f);
	mageButton.setPosition(sf::Vector2f(windowSize.x / 2.0f + mageButton.getGlobalBounds().width * 2.0f, windowSize.y / 2.0f + mageButton.getGlobalBounds().height * 2.0f));
	mageButton.setTexture(&buttonTexture);
	//knight sprite
	sf::Sprite knightSprite(knightTexture);
	knightSprite.setOrigin(knightSprite.getGlobalBounds().width / 2.0f, knightSprite.getGlobalBounds().height / 2.0f);
	knightSprite.setRotation(knightSprite.getRotation() + 90);
	knightSprite.setPosition(sf::Vector2f(windowSize.x / 2.0f - knightButton.getGlobalBounds().width * 2.0f, windowSize.y / 2.0f + knightSprite.getGlobalBounds().height / 4.0f));
	//ranger sprite
	sf::Sprite rangerSprite(rangerTexture);
	rangerSprite.setOrigin(rangerSprite.getGlobalBounds().width / 2.0f, rangerSprite.getGlobalBounds().height / 2.0f);
	rangerSprite.setRotation(rangerSprite.getRotation() + 91);
	rangerSprite.setPosition(sf::Vector2f(windowSize.x / 2.0f, windowSize.y / 2.0f + rangerSprite.getGlobalBounds().height / 4.0f));
	//mage sprite
	sf::Sprite mageSprite(mageTexture);
	mageSprite.setOrigin(mageSprite.getGlobalBounds().width / 2.0f, mageSprite.getGlobalBounds().height / 2.0f);
	mageSprite.setRotation(mageSprite.getRotation() + 100);
	mageSprite.setPosition(sf::Vector2f(windowSize.x / 2.0f + mageButton.getGlobalBounds().width * 2.0f, windowSize.y / 2.0f + mageSprite.getGlobalBounds().height / 4.0f));


	// -- exit overlay in game screen --
	// backdrop
	sf::RectangleShape exitBackdrop(sf::Vector2f(windowSize.x / 4.0f, windowSize.y / 4.0f));
	exitBackdrop.setFillColor(sf::Color(0, 0, 0, 150));
	exitBackdrop.setPosition(windowSize.x * 3.0f / 8.0f, windowSize.y * 3.0f / 8.0f);
	//title text
	sf::Text exitTitleText("Are you sure?", font, 64U);
	exitTitleText.setOrigin(exitTitleText.getGlobalBounds().width / 2, exitTitleText.getGlobalBounds().height / 2);
	// exit button
	sf::Text leaveText("Leave Game", font, 32U);
	leaveText.setColor(sf::Color::Black);
	leaveButton = Button(leaveText, sf::Vector2f(200.0f, 70.0f), onExitClickFromGame);
	leaveButton.setOrigin(leaveButton.getGlobalBounds().width / 2.0f, leaveButton.getGlobalBounds().height / 2.0f);
	leaveButton.setTexture(&buttonTexture);

	while (window.isOpen()) {
		releasedKey = sf::Keyboard::Unknown; // reset released

		sf::Event event;
		while (window.pollEvent(event)) {
			if (event.type == sf::Event::Closed) {
				onExitClickFromTitle();
				window.close();
			}
			else if (event.type == sf::Event::KeyPressed) {
				if (event.key.alt && event.key.code == sf::Keyboard::F4) {
					onExitClickFromTitle();
					window.close();
				}
			}
			else if (currentScreenDisplayed == ingame && event.type == sf::Event::KeyReleased) {
				sf::Keyboard::Key tempKey = event.key.code;
				if (tempKey == moveUpKey || tempKey == moveDownKey || tempKey == moveLeftKey || tempKey == moveRightKey) {
					releasedKey = tempKey;
				}
				else if (tempKey == sf::Keyboard::Escape) {
					isEscapeScreenDisplayed = !isEscapeScreenDisplayed;
				}
			}

			if (currentScreenDisplayed == title) {
				playButton.handleEvent(event);
				createButton.handleEvent(event);
				exitButton.handleEvent(event);
				if (shouldCloseWindow) { // set to true when exitButton's handleEvent callback_function is called
					window.close();
				}
			}
			else if (currentScreenDisplayed == joinServer) {
				joinTF.handleEvent(event);
				joinButton.handleEvent(event);
				backButton.handleEvent(event);
			} 
			else if (currentScreenDisplayed == createServer) {
				joinLocalButton.handleEvent(event);
				stopServerButton.handleEvent(event);
			}
			else if (currentScreenDisplayed == ingame) {
				if (isSelectionScreenDisplayed) {
					knightButton.handleEvent(event);
					rangerButton.handleEvent(event);
					mageButton.handleEvent(event);
				}
				if (isEscapeScreenDisplayed) {
					leaveButton.handleEvent(event);
				}
				if (event.type == sf::Event::KeyReleased && event.key.code == classSelectionDisplayKey) {
					isSelectionScreenDisplayed = !isSelectionScreenDisplayed; // toogles
				}
			}
		}

		window.clear();

		if (currentScreenDisplayed == title) {
			if (ingameMusic.getStatus() == sf::Sound::Playing) {
				ingameMusic.stop();
				titleMusic.play();
			}
			window.setView(normalView);
			// set positions
			titleSprite.setPosition(sf::Vector2f(windowSize.x / 2.0f, windowSize.y / 4.7f));
			playButton.setPosition(sf::Vector2f(windowSize.x / 2.0f, windowSize.y / 1.7f - playButton.getGlobalBounds().height));
			createButton.setPosition(sf::Vector2f(windowSize.x / 2.0f, playButton.getPosition().y + playButton.getGlobalBounds().height * 1.5f));
			exitButton.setPosition(sf::Vector2f(windowSize.x / 2.0f, createButton.getPosition().y + createButton.getGlobalBounds().height * 1.5f));
			// draw
			window.draw(backgroundRect);
			window.draw(titleSprite);
			playButton.draw(window);
			createButton.draw(window);
			exitButton.draw(window);
		}
		else if (currentScreenDisplayed == joinServer) {
			window.setView(normalView);
			// set joinTF's string to connectToIPandPort
			connectToIPandPort = joinTF.getString();
			backButton.setPosition(10.0f, 10.0f);
			serverText.setPosition(windowSize.x / 2.0f, windowSize.y / 2.0f - serverText.getGlobalBounds().height * 4.0f);
			joinTF.setPosition(windowSize.x / 2.0f, windowSize.y / 2.0f);
			joinButton.setPosition(windowSize.x / 2.0f, windowSize.y / 2.0f + joinTF.getGlobalBounds().height * 2.0f);
			connecting.setPosition(windowSize.x / 2.0f, windowSize.y / 2.0f + joinTF.getGlobalBounds().height);
			failedConnection.setPosition(windowSize.x / 2.0f, windowSize.y / 2.0f + joinTF.getGlobalBounds().height);
			// draw
			window.draw(backgroundRect);
			if (runningClient.serverConnectionStatus == 2) { // succesfully connected
				// reset kills and deaths
				kills = 0;
				deaths = 0;
				currentScreenDisplayed = ingame;
				titleMusic.stop();
				ingameMusic.play();
			}
			else if (runningClient.serverConnectionStatus == 0 && didRestartTimeoutTimer) { // waiting to connect
				if (timeoutTimer.getElapsedTime().asSeconds() < timeout) {
					window.draw(connecting);
				}
				else {
					runningClient.serverConnectionStatus = 1;
					runningClient.stopReceivingPackets();
				}
			}
			else if (runningClient.serverConnectionStatus == 1) { // failed to connect
				window.draw(failedConnection);
			}
			window.draw(serverText);
			joinTF.draw(window);
			joinButton.draw(window);
			backButton.draw(window);
		}
		else if (currentScreenDisplayed == createServer) {
			window.setView(normalView);
			// set positions
			stopServerButton.setPosition(10.0f, 10.0f);
			//ip and instructions on joining a server text
			std::ostringstream ipString;
			ipString << "Please use " << sf::IpAddress::getPublicAddress().toString() << ":" << runningServer.getPort() << " to connect to the server.";
			sf::Text ipText(ipString.str(), font, 25);
			ipText.setOrigin(ipText.getGlobalBounds().width / 2.0f, ipText.getGlobalBounds().height / 2.0f);
			ipText.setColor(sf::Color::White);
			ipText.setPosition(window.getSize().x / 2.0f, window.getSize().y / 2.0f - ipText.getGlobalBounds().height * 10.0f);
			joinLocalButton.setPosition(sf::Vector2f(windowSize.x / 2.0f, windowSize.y / 2.0f));
			//draw
			window.draw(backgroundRect);
			joinLocalButton.draw(window);
			window.draw(ipText);
			stopServerButton.draw(window);
		}
		else if (currentScreenDisplayed == ingame && player != NULL) {
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
			sf::Text gameTimeText(gameTimeString.str(), font, 30U);
			gameTimeText.setOrigin(gameTimeText.getGlobalBounds().width / 2.0f, 0.0f);
			gameTimeText.setPosition(sf::Vector2f(player->getCenter().x, player->getCenter().y - windowSize.y / 2.0f));
			map->collisionMP(*player);
			// check collision
			if (currentClass == knight) {
				runningClient.checkCollisions(player, currentClass, clientSocket, takeDamageSound, doDamageKSound);
			}
			else if (currentClass == ranger) {
				runningClient.checkCollisions(player, currentClass, clientSocket, takeDamageSound, doDamageRSound);
			}
			else if (currentClass == mage) {
				runningClient.checkCollisions(player, currentClass, clientSocket, takeDamageSound, doDamageMSound);
			}
			if (window.hasFocus() && !isSelectionScreenDisplayed && !player->getIsDead() && !isEscapeScreenDisplayed) {
				// player movement
				if (currentClass == knight) {
					((Knight*)player)->swingSword();
					((Knight*)player)->move(window, releasedKey);
				}
				else if (currentClass == ranger) {
					((Ranger*)player)->move(window, releasedKey);
					((Ranger*)player)->shoot(window);
					((Ranger*)player)->setWeapon(window);
			    }
				else if (currentClass == mage) {
					((Mage*)player)->move(window, releasedKey);
					((Mage*)player)->shoot(window);
					((Mage*)player)->setWeapon(window);
				}
			}
			// send that data to server
			sf::Packet packet;
			packet.clear();
			// draw
			map->draw(window);
			runningClient.draw(window, player->getCenter(), map);
			if (currentClass == knight) {
				packet = ((Knight*)player)->chainDataToPacket(packet, runningClient.getClientId());
				((Knight*)player)->draw(window);
			}
			else if (currentClass == ranger) {
				map->collisionMProj(*(ProjectileShooter*)player);
				packet = ((Ranger*)player)->chainDataToPacket(packet, runningClient.getClientId());
				((Ranger*)player)->drawProjectiles(window);
				((Ranger*)player)->draw(window);
			}
			else if (currentClass == mage) {
				map->collisionMProj(*(ProjectileShooter*)player);
				packet = ((Mage*)player)->chainDataToPacket(packet, runningClient.getClientId());
				((Mage*)player)->drawProjectiles(window);
				((Mage*)player)->draw(window);
			}
			runningClient.sendPacket(clientSocket, packet);
			window.draw(playerStats);
			window.draw(gameTimeText);
			if (!runningClient.isGameInProgress()) {
				waitingText.setPosition(sf::Vector2f(player->getCenter().x, gameTimeText.getGlobalBounds().top + gameTimeText.getGlobalBounds().height + waitingText.getGlobalBounds().height / 2.0f));
				classSelectionText.setPosition(sf::Vector2f(player->getCenter().x, waitingText.getGlobalBounds().top + waitingText.getGlobalBounds().height + classSelectionText.getGlobalBounds().height / 2.0f));
				// draw
				window.draw(waitingText);
				window.draw(classSelectionText);
				// reset kills and deaths and 5 seconds left in peace mode
				if (runningClient.timeLeftInGame <= 5.0f) {
					kills = 0;
					deaths = 0;
				}
			}
			if (player->getIsDead() || (!runningClient.isGameInProgress() && currentClass != respawnClass && runningClient.timeLeftInGame > 5.0f)) {
				if (runningClient.isGameInProgress()) {
					window.setView(normalView);
					// respawn text and set positions
					std::ostringstream respawnTimeString;
					respawnTimeString << "Respawning in " << respawnTime - player->getTimeAsDead() << " seconds";
					sf::Text respawnText(respawnTimeString.str(), font, 32U);
					respawnText.setOrigin(respawnText.getGlobalBounds().width / 2.0f, respawnText.getGlobalBounds().height / 2.0f);
					respawnText.setPosition(windowSize.x / 2.0f, windowSize.y / 2.0f);
					classSelectionInstructionsText.setPosition(windowSize.x / 2.0f, windowSize.y / 2.0f + respawnText.getGlobalBounds().height);
					// draw
					window.draw(deathBackdrop);
					window.draw(respawnText);
					window.draw(classSelectionInstructionsText);
				}
				// respawn when respawnTime spent as dead or in peace time
				if (player->getTimeAsDead() > respawnTime || !runningClient.isGameInProgress()) {
					if (respawnClass == knight) {
						if (currentClass == knight) {
							delete (Knight *)player;
							player = NULL;
						}
						else if (currentClass == ranger) {
							delete (Ranger *)player;
							player = NULL;
						}
						else if (currentClass == mage) {
							delete (Mage *)player;
							player = NULL;
						}
						player = (Character*)(new Knight(healthBarForegroundTexture, healthBarBackgroundTexture, knightTexture, swordTexture, 0.8f, 125, 22, 37));
						currentClass = knight;
					}
					else if (respawnClass == ranger) {
						if (currentClass == knight) {
							delete (Knight *)player;
							player = NULL;
						}
						else if (currentClass == ranger) {
							delete (Ranger *)player;
							player = NULL;
						}
						else if (currentClass == mage) {
							delete (Mage *)player;
							player = NULL;
						}
						player = (Character*)(new Ranger(healthBarForegroundTexture, healthBarBackgroundTexture, rangerTexture, bowTexture, arrowTexture, arrowTexture, 3.0f, 0.7f, 0.3f, 0.3f, 110, 16, 22, 0.7f));
						currentClass = ranger;
					}
					else if (respawnClass == mage) {
						if (currentClass == knight) {
							delete (Knight *)player;
							player = NULL;
						}
						else if (currentClass == ranger) {
							delete (Ranger *)player;
							player = NULL;
						}
						else if (currentClass == mage) {
							delete (Mage *)player;
							player = NULL;
						}
						player = (Character*)(new Mage(healthBarForegroundTexture, healthBarBackgroundTexture, mageTexture, staffTexture, fireballA, fireballB, 2.0f, 0.7f, 0.3f, 0.0f, 80, 25, 35, 0.6f));
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
				//draw
				window.draw(selectionbackdrop);
				window.draw(classTitleText);
				// cooldown timer
				if (classChangeCooldown.getElapsedTime().asSeconds() < classChangeCooldownTime) {
					std::ostringstream cooldownString;
					cooldownString << "must wait " << classChangeCooldownTime - classChangeCooldown.getElapsedTime().asSeconds() << " seconds before changing class again";
					sf::Text classChangeCooldownText(cooldownString.str(), font, 15U);
					classChangeCooldownText.setOrigin(classChangeCooldownText.getGlobalBounds().width / 2.0f, classChangeCooldownText.getGlobalBounds().height / 2.0f);
					classChangeCooldownText.setPosition(classTitleText.getPosition() + sf::Vector2f(0.0f, classTitleText.getGlobalBounds().height + classChangeCooldownText.getGlobalBounds().height / 2.0f));
					//draw
					window.draw(classChangeCooldownText);
				}
				//draw
				knightButton.draw(window);
				rangerButton.draw(window);
				mageButton.draw(window);
				window.draw(knightSprite);
				window.draw(rangerSprite);
				window.draw(mageSprite);
			}
			if (isEscapeScreenDisplayed) {
				window.setView(normalView);
				exitTitleText.setPosition(sf::Vector2f(windowSize.x / 2.0f, windowSize.y / 2.0f - exitTitleText.getCharacterSize()));
				leaveButton.setPosition(sf::Vector2f(windowSize.x / 2.0f, exitTitleText.getPosition().y + exitTitleText.getGlobalBounds().height * 1.5f));
				//draw
				window.draw(exitBackdrop);
				window.draw(exitTitleText);
				leaveButton.draw(window);
			}
		}

		window.display();
	}

	delete map;
	map = NULL;
	if (currentClass == knight) {
		delete (Knight *)player;
		player = NULL;
	}
	else if (currentClass == ranger) {
		delete (Ranger *)player;
		player = NULL;
	}
	else if (currentClass == mage) {
		delete (Mage *)player;
		player = NULL;
	}

	return EXIT_SUCCESS;
}