/*
 * UdpServer.h
 *
 *  Created on: Aug 15, 2014
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
using boost::asio::ip::udp;

#ifndef UDPSERVER_H_
#define UDPSERVER_H_

class UdpServer
{
	public:
	UdpServer(boost::asio::io_service& ioService);
	private:
		int loadLength;
		int* testLoad;
		udp::socket* socket;
		udp::endpoint endpoint;
		boost::array<int, 512> buffer;
		void startReceive();
		void handleReceive(const boost::system::error_code& error, std::size_t /*bytes_transferred*/);
		void handleSend(
			const boost::system::error_code& /*error*/,
			std::size_t /*bytes_transferred*/
		);
};


#endif /* UDPSERVER_H_ */
