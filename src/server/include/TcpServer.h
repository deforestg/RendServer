/*
 * TcpServer.h
 *
 *  Created on: Aug 14, 2014
 *      Author: gdeforest
 */

#ifndef TCPSERVER_H_
#define TCPSERVER_H_

#include <ctime>
#include <iostream>
#include <string>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/asio.hpp>

#include "../../include/GameManager.h"
#include "TcpConnection.h"

using boost::asio::ip::tcp;

class GameManager;

class TcpServer
{
	private:
		GameManager* gm;
		void startAccept();
		tcp::acceptor* acceptor;
		void handleAccept(TcpConnection::pointer newConnection, const boost::system::error_code& error);
	public:
		TcpServer(boost::asio::io_service& ioService);
};


#endif /* TCPSERVER_H_ */
