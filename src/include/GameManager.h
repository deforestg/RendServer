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
#define TIMEOUT 10		// seconds

#include <sys/time.h>
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>

#include "Player.h"

typedef struct {
	unsigned int ticker;
	PlayerData playersData[MAX_PLAYERS];
} Tick;
#include "Player.h"

typedef struct {
	char playerId;
	char spawnpoint;
} JoinMessage;

#include "../server/include/TcpServer.h"
#include "../server/include/UdpServer.h"

class TcpServer;
class UdpServer;

class GameManager {
	private:
		pthread_mutex_t playerLock;
		boost::asio::io_service* ioService;
		TcpServer* tcpServer;
		UdpServer* udpServer;
		Tick* gamestate;
		int numPlayers;
		int numAlivePlayers;
		char autoIncrementId;
		Player** players;
		void CheckTimeouts(timeval* now);
		static void* RunHelper(void *context);
		void Run();
		void SwapPlayers(int index1, int index2);
	public:
		Tick* GetGamestate() { return gamestate; }
		int GetTick() { return gamestate->ticker; }
		Player** GetPlayers() { return players; };
		int GetNumPlayers() { return numPlayers; };
		int GetNumAlivePlayers() { return numAlivePlayers; };
		pthread_mutex_t* GetPlayerLock() { return &playerLock; };
		void RemovePlayer(int index);
		void KillPlayer(int index);
		GameManager(boost::asio::io_service& ioService);
		void Start();
		JoinMessage Respawn(string ip, char playerId);
		JoinMessage AcceptJoin(string ip);
		virtual ~GameManager();
};




#endif /* GAMEMANAGER_H_ */
