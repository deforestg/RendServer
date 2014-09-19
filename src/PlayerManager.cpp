/*
 * PlayerManager.cpp
 *
 *  Created on: Sep 18, 2014
 *      Author: gdeforest
 */

#include "include/PlayerManager.h"

PlayerManager::PlayerManager(Tick* gamestate, pthread_mutex_t* playerLock) {
	pm = this;

	this->gamestate = gamestate;
	this->playerLock = playerLock;
	numPlayers = 0;
	numAlivePlayers = 0;
	players = new Player*[MAX_PLAYERS];
}

int PlayerManager::GetPlayerIndex(string ip, char playerId) {
	int index = -1;
	for (int i = 0; i < numPlayers; i++) {
		if (players[i]->getIp().compare(ip) == 0 && players[i]->getData()->id == playerId) {
			index = i;
			break;
		}
	}
	return index;
}

void PlayerManager::SwapPlayers(int index1, int index2) {
	Player* temp = players[index1];
	players[index1] = players[index2];
	players[index2] = temp;
	// swap the actual data
	PlayerData tempData = gamestate->playersData[index1];
	gamestate->playersData[index1] = gamestate->playersData[index2];
	gamestate->playersData[index2] = tempData;
	// relink the PlayerData
	players[index1]->setData(&gamestate->playersData[index1]);
	players[index2]->setData(&gamestate->playersData[index2]);
}

/**
 * checks for player timeouts by comparing last update timestamps to now
 */
void PlayerManager::CheckTimeouts(timeval* now) {

	for (int i = 0; i < numPlayers; i++) {
		timeval* updated = players[i]->LastUpdated();

		if (now->tv_sec - updated->tv_sec >= TIMEOUT) {
			cout << "removing player " << (int)players[i]->getData()->id << endl;
			RemovePlayer(i);
		}
	}
}

void PlayerManager::AddPlayer(char id, string ip) {

	pthread_mutex_lock(playerLock);
	players[numPlayers] = new Player(id, ip, &gamestate->playersData[numPlayers]);

	// swap player with first dead
	for (int i = 0; i < numPlayers; i++) {
		if (!players[i]->isAlive()) {
			cout << "swapping " << numPlayers << " and " << i << endl;
			SwapPlayers(numPlayers, i);
			break;
		}
	}

	numPlayers++;
	numAlivePlayers++;

	pthread_mutex_unlock(playerLock);
}

int PlayerManager::RemovePlayer(string ip, char playerId) {
	int index = GetPlayerIndex(ip, playerId);

	if (index < 0) {
		return index;
	}

	RemovePlayer(index);

	return 0;
}

/**
 * keeps the players array coalesced
 */
void PlayerManager::RemovePlayer(int index) {

	pthread_mutex_lock(playerLock);

	if (players[index]->isAlive()) {
		numAlivePlayers--;
	}

	if (index == (numPlayers - 1)) {
		numPlayers--;
		delete players[index];
	} else {
		numPlayers--;
		gamestate->playersData[index] = gamestate->playersData[numPlayers];
		delete players[index];
		players[index] = players[numPlayers];
		// have to relink the PlayerData
		players[index]->setData(&gamestate->playersData[index]);
	}

	pthread_mutex_unlock(playerLock);
}

/**
 * keeps the players array as alive first, then dead
 */
void PlayerManager::KillPlayer(int index) {

	pthread_mutex_lock(playerLock);

	cout << "killing player " << (int)players[index]->getData()->id << endl;
	players[index]->kill();
	numAlivePlayers--;

	if (index >= numAlivePlayers) { // already ordered
		pthread_mutex_unlock(playerLock);
		return;
	}

	// swap player with first alive
	for (int i = numAlivePlayers; i >= 0; i--) {
		if (players[i]->isAlive()) {
			if (index != i) {	// no need
				SwapPlayers(index, i);
			}
			break;
		}
	}

	pthread_mutex_unlock(playerLock);
}

int PlayerManager::Spawn(string ip, char playerId) {

	pthread_mutex_lock(playerLock);

	int index = GetPlayerIndex(ip, playerId);

	if (index < 0) {
		pthread_mutex_unlock(playerLock);
		return index;
	}

	cout << "respawning player " << (int)players[index]->getData()->id << endl;

	if (!players[index]->isAlive()) {
		numAlivePlayers++;
	}
	players[index]->spawn();

	// swap player with first dead
	for (int i = 0; i < numPlayers; i++) {
		if (!players[i]->isAlive()) {
			if (index != i) {	// no need
				SwapPlayers(index, i);
			}
			break;
		}
	}

	pthread_mutex_unlock(playerLock);
	return 0;
}

PlayerManager::~PlayerManager() {
	delete players;
}
