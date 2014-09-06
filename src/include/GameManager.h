/*
 * GameManager.h
 *
 *  Created on: Sep 2, 2014
 *      Author: gdeforest
 */

#include "../server/include/TcpServer.h"
#include "../server/include/UdpServer.h"

#define MAX_PLAYERS 16

#ifndef GAMEMANAGER_H_
#define GAMEMANAGER_H_

class GameManager {
	private:
		boost::asio::io_service* ioService;
		TcpServer* tcpServer;
		UdpServer* udpServer;
		Player** players;
		int numPlayers;
		char autoIncrementId;
	public:
		GameManager(boost::asio::io_service& ioService);
		void start();
		Player** GetPlayers() { return players; };
		int GetNumPlayers() { return numPlayers; };
		char* AcceptJoin(char* ip);
		virtual ~GameManager();
};

#endif /* GAMEMANAGER_H_ */
