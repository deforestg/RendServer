/*
 * UdpServer.cpp
 *
 *  Created on: Aug 15, 2014
 *      Author: gdeforest
 */

#include "include/UdpServer.h"

UdpServer::UdpServer(boost::asio::io_service& ioService) {
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

void UdpServer::handleReceive(const boost::system::error_code& error, std::size_t /*bytes_transferred*/)
{
	if (!error || error == boost::asio::error::message_size)
	{
		const void* data = (const void*)100;

		socket->async_send_to(
			boost::asio::buffer(data, sizeof(data)),
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
	boost::shared_ptr<std::string> /*message*/,
	const boost::system::error_code& /*error*/,
	std::size_t /*bytes_transferred*/
) {
}
