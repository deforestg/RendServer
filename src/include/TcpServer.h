/*
 * TcpServer.h
 *
 *  Created on: Aug 14, 2014
 *      Author: gdeforest
 */

#include <ctime>
#include <iostream>
#include <string>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/asio.hpp>

#include "TcpConnection.h"

using namespace std;
using boost::asio::ip::tcp;

#ifndef TCPSERVER_H_
#define TCPSERVER_H_

class TcpServer
{
	public:
		TcpServer(boost::asio::io_service& ioService);
	private:
		int loadLength;
		int* testLoad;
		void startAccept();
		tcp::acceptor* acceptor;
		void handleAccept(TcpConnection::pointer newConnection, const boost::system::error_code& error);
};


#endif /* TCPSERVER_H_ */
