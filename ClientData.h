#ifndef CLIENTDATA_H
#define CLIENTDATA_H
#include <SFML/Network.hpp>

struct ClientData {
	int id = 0;
	char *data;
};

sf::Packet & operator<<(sf::Packet & packet, const ClientData & cd);

sf::Packet & operator>>(sf::Packet & packet, ClientData & cd);

#endif // !CLIENTDATA_H