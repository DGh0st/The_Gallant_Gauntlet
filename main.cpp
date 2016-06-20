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

static screenTypes currentScreenDisplayed = title; // current displayed screen

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

void runClient() {
	runningClient.receivePackets(clientSocket);
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
	currentScreenDisplayed = createServer;
}

void onJoinGameClickFromClient() {
	if (connectToIPandPort.size() < 12) {
		// string shorter than 0.0.0.0:1000
		return;
	}
	else if (connectToIPandPort.find_first_of(":", 0) == std::string::npos) {
		// port not included
		return;
	}
	else if (serverInfo.name != "Server") {
		// parse ip and port
		size_t offset = connectToIPandPort.find_last_of(":", 0);
		serverInfo.ip = sf::IpAddress(connectToIPandPort.substr(0, offset));
		serverInfo.name = "Server";
		serverInfo.port = (unsigned short)atoi(connectToIPandPort.substr(offset + 1, connectToIPandPort.length() - offset).c_str());
		// actually join server
		runningClient = Client(clientSocket, serverInfo);
		clientThread = std::thread(&runClient);
	}
	currentScreenDisplayed = ingame;
}

void onJoinGameClickFromServer() {
	if (serverInfo.name != "Server") {
		serverInfo.ip = sf::IpAddress::getPublicAddress();
		serverInfo.name = "Server";
		serverInfo.port = runningServer.getPort();
		runningClient = Client(clientSocket, serverInfo);
		clientThread = std::thread(&runClient);
	}
	currentScreenDisplayed = ingame;
}

int main() {
	sf::RenderWindow window(sf::VideoMode(800, 500), "Game 2");
	windowSize = window.getSize();

	sf::Font font;
	font.loadFromFile("fonts/times.ttf");
	Character player = Character();
	sf::Keyboard::Key releasedKey = sf::Keyboard::Unknown;
	
	Textfield joinTF(font, sf::Vector2f(650.0f, 50.0f));
	Button playButton, createButton, joinButton;

	while (window.isOpen()) {
		releasedKey = sf::Keyboard::Unknown; // reset released

		sf::Event event;
		while (window.pollEvent(event)) {
			if (event.type == sf::Event::Closed || (event.type == sf::Event::KeyReleased && event.key.code == sf::Keyboard::Escape)) {
				runningServer.stopServer();
				runningClient.stopReceivingPackets();
				window.close();
			}
			else if (currentScreenDisplayed == ingame && event.type == sf::Event::KeyReleased) {
				sf::Keyboard::Key tempKey = event.key.code;
				if (tempKey == moveUpKey || tempKey == moveDownKey || tempKey == moveLeftKey || tempKey == moveRightKey) {
					releasedKey = tempKey;
				}
			}
			joinTF.handleEvent(event);
			playButton.handleEvent(event);
			createButton.handleEvent(event);
			joinButton.handleEvent(event);
		}

		window.clear();

		if (currentScreenDisplayed == title) {
			//title text
			sf::Text titleText("Title", font, 64U);
			titleText.setOrigin(titleText.getGlobalBounds().width / 2, titleText.getGlobalBounds().height / 2);
			titleText.setPosition(sf::Vector2f(windowSize.x / 2.0f, windowSize.y / 2.0f - 128.0f));
			//play button
			sf::Text playText("Play", font, 32U);
			playText.setColor(sf::Color::Black);
			playButton = Button(playText, sf::Vector2f(100.0f, 75.0f), onPlayClick);
			playButton.setOrigin(playButton.getGlobalBounds().width / 2.0f, playButton.getGlobalBounds().height / 2.0f);
			playButton.setPosition(sf::Vector2f(windowSize.x / 2.0f, windowSize.y / 2.0f));
			window.draw(titleText);
			playButton.draw(window);
			//create game button
			sf::Text createText("Create Game", font, 32U);
			createText.setColor(sf::Color::Black);
			createButton = Button(createText, sf::Vector2f(200.0f, 75.0f), onCreateGameClick);
			createButton.setOrigin(createButton.getGlobalBounds().width / 2.0f, createButton.getGlobalBounds().height / 2.0f);
			createButton.setPosition(sf::Vector2f(windowSize.x / 2.0f, windowSize.y / 2.0f + 128.0f));
			window.draw(titleText);
			createButton.draw(window);
		}
		else if (currentScreenDisplayed == joinServer) {
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
			//ip and instructions on joining a server text
			/*char *ipString = "Please use 888.888.888:88888 to connect to the server...";
			if (isServerInitializedAndRunning && runningServer.getPort() != 0) {
				sprintf(ipString, "Please use %s:%d to connect to the server", sf::IpAddress::getPublicAddress().toString().c_str(), runningServer.getPort());
			}*/
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
			player.move(window, releasedKey);
			sf::Packet packet;
			packet = player.chainDataToPacket(packet);
			runningClient.sendPacket(clientSocket, packet);
			player.draw(window);
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