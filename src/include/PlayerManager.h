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
		pthread_mutex_t* playerLock;
		Player** players;
		int numPlayers;
		int numAlivePlayers;
		Tick* gamestate;
		void RemovePlayer(int index);
		void SwapPlayers(int index1, int index2);
		PlayerManager(PlayerManager const&);		// Don't Implement.
        void operator = (PlayerManager const&);	// Don't implement
        PlayerManager() {};
		virtual ~PlayerManager();
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

		void init(Tick* gamestate, pthread_mutex_t* playerLock);
		static PlayerManager& GetInstance() {
			static PlayerManager pm;
			return pm;
		};
};

#endif /* PLAYERMANAGER_H_ */
