/*
 * PlayerManager.cpp
 *
 *  Created on: Sep 18, 2014
 *      Author: gdeforest
 */

#include "include/PlayerManager.h"

void PlayerManager::init(Tick* gamestate, ServerMessage* gameStatus, pthread_mutex_t* playerLock) {
	this->gamestate = gamestate;
	this->gameStatus = gameStatus;
	this->playerLock = playerLock;
	numPlayers = 0;
	numAlivePlayers = 0;
	players = new Player*[MAX_PLAYERS];
}

/**
 * @param string ip - player ip
 * @param char id - player id
 */
int PlayerManager::GetPlayerIndex(string ip, char id) {
	int index = -1;
	for (int i = 0; i < numPlayers; i++) {
		if (players[i]->getIp().compare(ip) == 0 && players[i]->getData()->id == id) {
			index = i;
			break;
		}
	}
	return index;
}

/**
 * internal convenience function
 * @param int index1
 * @param int index2
 */
void PlayerManager::SwapPlayers(int index1, int index2) {
	Player* temp = players[index1];
	players[index1] = players[index2];
	players[index2] = temp;
	// swap the actual data
	PlayerData tempData = gamestate->playersData[index1];
	gamestate->playersData[index1] = gamestate->playersData[index2];
	gamestate->playersData[index2] = tempData;
	// swap the status data
	PlayerStatus tempStatus = gameStatus->playersStatus[index1];
	gameStatus->playersStatus[index1] = gameStatus->playersStatus[index2];
	gameStatus->playersStatus[index2] = tempStatus;
	// relink the PlayerData
	players[index1]->setData(&gamestate->playersData[index1]);
	players[index2]->setData(&gamestate->playersData[index2]);
}

/**
 * checks for player timeouts by comparing last update timestamps to now
 * @param timeval* now
 */
void PlayerManager::CheckTimeouts(timeval* now) {
	for (int i = 0; i < numPlayers; i++) {
		timeval* updated = players[i]->LastUpdated();

		if (now->tv_sec - updated->tv_sec >= TIMEOUT) {
			cout << "timing out player " << (int)players[i]->getData()->id << endl;
			RemovePlayer(i);
		} else {
			gameStatus->playersStatus[i].ping = (now->tv_usec - updated->tv_usec)/1000;
		}
	}
}

/**
 * @param string ip - player ip
 * @param char id - player id
 */
void PlayerManager::AddPlayer(string ip, char id) {

	pthread_mutex_lock(playerLock);
	players[numPlayers] = new Player(id, ip, &gamestate->playersData[numPlayers], &gameStatus->playersStatus[numPlayers]);

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

/**
 * @param string ip - player ip
 * @param char id - player id
 * @return 0 on success, -1 on fail
 */
int PlayerManager::RemovePlayer(string ip, char playerId) {
	cout << "removing player " << (int)playerId << endl;

	int index = GetPlayerIndex(ip, playerId);

	if (index < 0) {
		return index;
	}

	RemovePlayer(index);

	return 0;
}

/**
 * keeps the players array coalesced
 * @param int index of the players array to remove
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
		gameStatus->playersStatus[index] = gameStatus->playersStatus[numPlayers];
		delete players[index];
		players[index] = players[numPlayers];
		// have to relink the PlayerData
		players[index]->setData(&gamestate->playersData[index]);
		players[index]->setStatus(&gameStatus->playersStatus[index]);
	}

	pthread_mutex_unlock(playerLock);
}

/**
 * keeps the players array as alive first, then dead
 * @param int index of the players array to kill
 */
void PlayerManager::KillPlayer(int index) {

	pthread_mutex_lock(playerLock);

	cout << "killing player " << (int)players[index]->getData()->id << endl;

	FindCulprit(players[index]);

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

/**
 * finds and rewards the culprit, if any
 * @param Player* shotPlayer
 */
void PlayerManager::FindCulprit(Player* shotPlayer) {
	PlayerData* data = shotPlayer->getData();
	if (data->shotBy > 0) {
		int index = GetPlayerIndex(shotPlayer->getIp(), data->id);

		if (index < 0) {
			return;
		}

		players[index]->getStatus()->kills++;
	}
}

/**
 * @param string ip - player ip
 * @param char id - player id
 */
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
			if (index != i) {	// if at the end of the array then no need to swap
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
