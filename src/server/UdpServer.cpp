/*
 * UdpServer.cpp
 *
 *  Created on: Aug 15, 2014
 *      Author: gdeforest
 */

#include "include/UdpServer.h"

UdpServer::UdpServer(boost::asio::io_service& ioService, GameManager* gm) {
	this->gm = gm;
	tick = gm->GetGamestate();
	socket = new udp::socket(ioService, udp::endpoint(udp::v4(), 18206));
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
	if (!error || error == boost::asio::error::message_size)
	{
		string currentIp;
		boost::asio::ip::address addr = endpoint.address();

		if (addr.is_v4()) {
			currentIp = addr.to_v4().to_string();
		} else {
			currentIp = addr.to_v6().to_string();
		}

		Player** players = gm->GetPlayers();
		Player* current = NULL;

		PlayerData* update = (PlayerData*) buffer.data();

		pthread_mutex_t* playerLock = gm->GetPlayerLock();
		pthread_mutex_lock(playerLock);

		int numPlayers = gm->GetNumPlayers();

		for (int i = 0; i < numPlayers; i++) {
			string playerIp = players[i]->getIp();

			if (playerIp.compare(currentIp) == 0 && update->id == players[i]->getData()->id) {	//found, now update
				current = players[i];

				if (bytes_transferred == sizeof(PlayerData)) {
					players[i]->update(update);
				}

				break;
			}
		}
		pthread_mutex_unlock(playerLock);

		char* px;
		int sendLen;
		if (current == NULL) {	// not formally joined so send back nothing
			px = new char[1];
			sendLen = 1;
		} else {
			px = reinterpret_cast<char*>(tick);
			sendLen = sizeof(unsigned int) + sizeof(PlayerData)*numPlayers;
		}

		socket->async_send_to(
			boost::asio::buffer(px, sendLen),
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
