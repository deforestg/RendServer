/*
 * TcpConnection.h
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

using namespace std;
using boost::asio::ip::tcp;

#ifndef TCPCONNECTION_H_
#define TCPCONNECTION_H_

class TcpConnection : public boost::enable_shared_from_this<TcpConnection>
{
	public:
		void Start(int* testLoad, int loadLength);
		tcp::socket& getSocket() { return socket; }
		typedef boost::shared_ptr<TcpConnection> pointer;
		static pointer create(boost::asio::io_service& ioService);
	private:
		TcpConnection(boost::asio::io_service& ioService);
		tcp::socket socket;
		std::string message;
		void handleWrite(const boost::system::error_code& /*error*/, size_t /*bytes_transferred*/);
};


#endif /* TCPCONNECTION_H_ */
