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

		for (int i = 0; i < gm->GetNumPlayers(); i++) {
			string playerIp = players[i]->getIp();

			if (playerIp.compare(currentIp) == 0 && update->id == players[i]->getData()->id) {	//found, now update
				current = players[i];

				if (bytes_transferred == sizeof(PlayerData)) {
					players[i]->update(update);
				}

				break;
			}
		}

		if (current == NULL) {	// not formally joined so stop this now
			startReceive();
			return;
		}

		const char* px = reinterpret_cast<const char*>(tick);

		socket->async_send_to(
			boost::asio::buffer(px, sizeof(int) + sizeof(PlayerData)*gm->GetNumPlayers()),
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
