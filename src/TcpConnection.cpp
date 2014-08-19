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

void TcpConnection::Start()
{
	int x[] = {100, 200, 300, 400, 500};
	const char* px = reinterpret_cast<const char*>(&x);

	boost::asio::async_write(socket, boost::asio::buffer(px, sizeof(x)),
	boost::bind(&TcpConnection::handleWrite, shared_from_this(),
	boost::asio::placeholders::error,
	boost::asio::placeholders::bytes_transferred));
}

TcpConnection::TcpConnection(boost::asio::io_service& ioService) : socket(ioService)
{
}

void TcpConnection::handleWrite(const boost::system::error_code& /*error*/, size_t /*bytes_transferred*/)
{
}
