/*
 * UdpServer.cpp
 *
 *  Created on: Aug 15, 2014
 *      Author: gdeforest
 */

#include "include/UdpServer.h"

UdpServer::UdpServer(boost::asio::io_service& ioService) {
	socket = new udp::socket(ioService, udp::endpoint(udp::v4(), 18206));
	numPlayers = 0;
	players = new Player*[MAX_PLAYERS];
	startReceive();
}

void UdpServer::startReceive()
{
	socket->async_receive_from(
		boost::asio::buffer(buffer),
		endpoint,
		boost::bind(
			&UdpServer::handleReceive,
			this,
			boost::asio::placeholders::error,
			boost::asio::placeholders::bytes_transferred
		)
	);
}

void UdpServer::handleReceive(const boost::system::error_code& error, std::size_t /*bytes_transferred*/)
{
	long ip = (long)endpoint.data();
	Player* current = NULL;
	for (int i = 0; i < numPlayers; i++) {
		if (players[i]->getData()->ip == ip) {	//found, now update
			current = players[i];

			char* playerLocation = tick + sizeof(PlayerData)*i;

			if (buffer.size() == sizeof(PlayerData)) {
				char* clientData = buffer.data();
				memcpy(&playerLocation, &clientData, sizeof(PlayerData));
			} // TODO: incomplete packet, possibly later use what we can from it
			break;
		}
	}

	if (current == NULL) {
		if (numPlayers == MAX_PLAYERS) {
			// TODO: kill connection
		}

		players[numPlayers] = new Player(ip);
		char* playerLocation = tick + sizeof(PlayerData)*numPlayers++;
		char* clientData = buffer.data();
		memcpy(&playerLocation, &clientData, sizeof(PlayerData));
	}

	if (!error || error == boost::asio::error::message_size)
	{
		const char* px = reinterpret_cast<const char*>(tick);

		socket->async_send_to(
			boost::asio::buffer(px, loadLength),
			endpoint,
			boost::bind(
				&UdpServer::handleSend,
				this,
				boost::asio::placeholders::error,
				boost::asio::placeholders::bytes_transferred
			)
		);

		startReceive();
	}
}

void UdpServer::handleSend(
	const boost::system::error_code& /*error*/,
	std::size_t /*bytes_transferred*/
) {
}
