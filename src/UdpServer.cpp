/*
 * UdpServer.cpp
 *
 *  Created on: Aug 15, 2014
 *      Author: gdeforest
 */

#include "include/UdpServer.h"

UdpServer::UdpServer(boost::asio::io_service& ioService) {
	socket = new udp::socket(ioService, udp::endpoint(udp::v4(), 18206));

	loadLength = 250;
	testLoad = new int[loadLength];
	for (int i = 0; i < loadLength; i++) {
		testLoad[i] = i;
	}
	loadLength *= sizeof(int);

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

	int* data = (int*)buffer.data();
	for (unsigned i = 0; i < buffer.size(); i++) {
		std::cout << data[i] << " ";
	}
	std::cout << std::endl;
}

void UdpServer::handleReceive(const boost::system::error_code& error, std::size_t /*bytes_transferred*/)
{
	if (!error || error == boost::asio::error::message_size)
	{
		const char* px = reinterpret_cast<const char*>(testLoad);

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
