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
	autoIncrementId = 1;
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

void UdpServer::handleReceive(const boost::system::error_code& error, std::size_t bytes_transferred)
{
	string currentIp;
	boost::asio::ip::address addr = endpoint.address();

	if (addr.is_v4()) {
		currentIp = addr.to_v4().to_string();
	} else {
		currentIp = addr.to_v6().to_string();
	}

	Player* current = NULL;
	for (int i = 0; i < numPlayers; i++) {
		string playerIp = players[i]->getIp();

		PlayerData* player = (PlayerData*)(tick + sizeof(PlayerData)*i);

		if (playerIp.compare(currentIp) == 0 && player->id == players[i]->getData()->id) {	//found, now update
			current = players[i];

			if (bytes_transferred == sizeof(PlayerData)) {
				char playerId = players[i]->getData()->id;
				char* clientData = buffer.data();
				memcpy(player, clientData, sizeof(PlayerData));
				player->id = playerId;	// client cannot change their id
			} // TODO: incomplete packet, possibly later use what we can from it

			break;
		}
	}

	if (current == NULL) {
		if (numPlayers == MAX_PLAYERS) {
			// TODO: kill connection
			return;
		}

		players[numPlayers] = new Player(autoIncrementId++, currentIp);
		char* playerLocation = tick + sizeof(PlayerData)*numPlayers;
		PlayerData* newPlayer = players[numPlayers++]->getData();
		memcpy(playerLocation, newPlayer, sizeof(PlayerData));
	}

	if (!error || error == boost::asio::error::message_size)
	{
		const char* px = reinterpret_cast<const char*>(tick);

		socket->async_send_to(
			boost::asio::buffer(px, sizeof(PlayerData)*numPlayers),
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
