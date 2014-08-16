/*
 * TcpConnection.cpp
 *
 *  Created on: Aug 14, 2014
 *      Author: gdeforest
 */

#include "include/TcpConnection.h"

std::string TcpConnection::makeDaytimeString()
{
  using namespace std; // For time_t, time and ctime;
  time_t now = time(0);
  return ctime(&now);
}

TcpConnection::pointer TcpConnection::create(boost::asio::io_service& ioService)
{
	return pointer(new TcpConnection(ioService));
}

void TcpConnection::Start()
{
	message = makeDaytimeString();

	boost::asio::async_write(socket, boost::asio::buffer(message),
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
