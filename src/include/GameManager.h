/*
 * GameManager.h
 *
 *  Created on: Sep 2, 2014
 *      Author: gdeforest
 */

#ifndef GAMEMANAGER_H_
#define GAMEMANAGER_H_

#define RATE 128		// ticks/s
#define MAX_PLAYERS 16
#define TIMEOUT 2		// seconds

#include <sys/time.h>
#include <unistd.h>
#include <pthread.h>
#include "../server/include/TcpServer.h"
#include "../server/include/UdpServer.h"

class TcpServer;
class UdpServer;

class GameManager {
	private:
		char gamestate[sizeof(PlayerData)*MAX_PLAYERS];
		boost::asio::io_service* ioService;
		TcpServer* tcpServer;
		UdpServer* udpServer;
		Player** players;
		int numPlayers;
		char autoIncrementId;
		long gameTicker;
		void CheckTimeouts(timeval* now);
		void RemovePlayer(int index);
		static void* RunHelper(void *context);
		void Run();
	public:
		char* GetGamestate() { return gamestate; }
		GameManager(boost::asio::io_service& ioService);
		void Start();
		Player** GetPlayers() { return players; };
		int GetNumPlayers() { return numPlayers; };
		char AcceptJoin(string ip);
		virtual ~GameManager();
};

#endif /* GAMEMANAGER_H_ */
