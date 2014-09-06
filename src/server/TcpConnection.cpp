/*
 * TcpConnection.cpp
 *
 *  Created on: Aug 14, 2014
 *      Author: gdeforest
 */

#include "include/TcpConnection.h"

TcpConnection::pointer TcpConnection::create(boost::asio::io_service& ioService)
{
	return pointer(new TcpConnection(ioService));
}

void TcpConnection::Start(char* message, int loadLength)
{
//	std::cout << socket.remote_endpoint().address().to_string();
//	std::cout << std::endl;

	const char* px = reinterpret_cast<const char*>(message);

	boost::asio::async_write(
		socket, boost::asio::buffer(px, loadLength),
		boost::bind(
			&TcpConnection::handleWrite,
			shared_from_this(),
			boost::asio::placeholders::error,
			boost::asio::placeholders::bytes_transferred
		)
	);
}

TcpConnection::TcpConnection(boost::asio::io_service& ioService) : socket(ioService)
{
}

void TcpConnection::handleWrite(const boost::system::error_code& /*error*/, size_t /*bytes_transferred*/)
{
}
