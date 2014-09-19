/*
 * UdpServer.h
 *
 *  Created on: Aug 15, 2014
 *      Author: gdeforest
 */

#ifndef UDPSERVER_H_
#define UDPSERVER_H_

#include <ctime>
#include <iostream>
#include <string>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/asio.hpp>

#include "../../include/GameManager.h"
#include "../../include/PlayerManager.h"

using namespace std;
using boost::asio::ip::udp;

class GameManager;
class PlayerManager;

class UdpServer
{
	public:
		UdpServer(boost::asio::io_service& ioService, GameManager* gm);
	private:
		GameManager* gm;
		PlayerManager* pm;
		Tick *tick;
		udp::socket* socket;
		udp::endpoint endpoint;
		boost::array<char, 512> buffer;
		void startReceive();
		void handleReceive(const boost::system::error_code& error, std::size_t /*bytes_transferred*/);
		void handleSend(
			const boost::system::error_code& /*error*/,
			std::size_t /*bytes_transferred*/
		);
};


#endif /* UDPSERVER_H_ */
