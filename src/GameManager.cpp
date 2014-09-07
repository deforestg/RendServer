/*
 * GameManager.cpp
 *
 *  Created on: Sep 2, 2014
 *      Author: gdeforest
 */

#include "include/GameManager.h"

/**
 * initialize game data and udp and tcp listeners
 */
GameManager::GameManager(boost::asio::io_service& ioService) {
	numPlayers = 0;
	autoIncrementId = 1;
	players = new Player*[MAX_PLAYERS];
	gameTicker = 0;

	this->ioService = &ioService;
    tcpServer = new TcpServer(ioService, this);
    udpServer = new UdpServer(ioService, this);
}

/**
 * starts main game thread
 */
void GameManager::Start() {
	pthread_t thread;
	if (pthread_create(&thread, NULL, &GameManager::RunHelper, this)){
		cout << "Error:unable to create thread" << endl;
		exit(-1);
	}
}

void* GameManager::RunHelper(void *context)
{
    ((GameManager *)context)->Run();
    return NULL;
}

void GameManager::Run() {
	timeval start, end;

	for (;;) {
		gettimeofday(&start, NULL);

		if (gameTicker++ % RATE == 0) {	// check every second for timeouts
			CheckTimeouts(&start);
		}

		gettimeofday(&end, NULL);
		int sleep = (1000/RATE) - (end.tv_usec - start.tv_usec);
		if (sleep > 0) {
			usleep(sleep * 1000);
		}
	}
}

/**
 * checks for player timeouts by comparing last update timestamps to now
 */
void GameManager::CheckTimeouts(timeval* now) {
	for (int i = 0; i < numPlayers; i++) {
		timeval* updated = players[i]->LastUpdated();

		if (now->tv_sec - updated->tv_sec >= TIMEOUT) {
			RemovePlayer(i);
		}
	}
}

/**
 * keeps the players array coalesced
 */
void GameManager::RemovePlayer(int index) {
	if (index == (numPlayers - 1)) {
		numPlayers--;
	} else {
		players[index] = players[numPlayers-- - 1];
	}
}

/**
 * called from the tcp server TODO: put in mutex, add leaves
 */
char GameManager::AcceptJoin(string ip) {

	pthread_mutex_lock(&playerLock);

	if (numPlayers == MAX_PLAYERS) {
		// TODO: kill connection
		return 0;
	}

	players[numPlayers] = new Player(++autoIncrementId, ip);
	char* playerLocation = gamestate + sizeof(PlayerData)*numPlayers;
	PlayerData* newPlayer = players[numPlayers++]->getData();
	memcpy(playerLocation, newPlayer, sizeof(PlayerData));

	pthread_mutex_unlock(&playerLock);

	return autoIncrementId;
}

GameManager::~GameManager() {
	delete tcpServer;
	delete udpServer;
	delete players;
}

