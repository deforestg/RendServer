/*
 * PlayerManager.h
 *
 *  Created on: Sep 18, 2014
 *      Author: gdeforest
 */

#ifndef PLAYERMANAGER_H_
#define PLAYERMANAGER_H_

#include <string>
#include "GameManager.h"

using namespace std;

class PlayerManager {
	private:
		static PlayerManager* pm;
		pthread_mutex_t* playerLock;
		Player** players;
		int numPlayers;
		int numAlivePlayers;
		Tick* gamestate;
		void RemovePlayer(int index);
		void SwapPlayers(int index1, int index2);
	public:
		void KillPlayer(int index); //TODO: make private, remove from UdpServer
		Player** GetPlayers() { return players; };
		void CheckTimeouts(timeval* now);
		void AddPlayer(char id, string ip);
		int RemovePlayer(string ip, char playerId);
		int KillPlayer(string ip, char playerId);
		int Spawn(string ip, char playerId);

		int GetPlayerIndex(string ip, char playerId);
		int GetNumPlayers() { return numPlayers; };
		int GetNumAlivePlayers() { return numAlivePlayers; };

		static PlayerManager* GetInstance() { return pm; };
		PlayerManager(Tick* gamestate, pthread_mutex_t* playerLock);
		virtual ~PlayerManager();
};

#endif /* PLAYERMANAGER_H_ */
