#include "ClientData.h"

sf::Packet & operator<<(sf::Packet & packet, const ClientData & cd) {
	return packet << cd.id << cd.data;
}

sf::Packet & operator>>(sf::Packet & packet, ClientData & cd) {
	return packet >> cd.id >> cd.data;
}