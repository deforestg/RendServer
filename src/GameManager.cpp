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
	autoIncrementId = 1;
	gamestate = new Tick();
	gamestate->ticker = 0;

	numPlayers = 0;
	numAlivePlayers = 0;
	players = new Player*[MAX_PLAYERS];

	this->ioService = &ioService;
    tcpServer = new TcpServer(ioService, this);
    udpServer = new UdpServer(ioService, this);
    srand(time(NULL));
}

/**
 * starts main game thread
 */
void GameManager::Start() {
	if (pthread_mutex_init(&playerLock, NULL) != 0) {
		cout << "mutex init failed" << endl;
		exit(-1);
	}

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
	int sleep;

	for (;;) {
		gettimeofday(&start, NULL);

		if (gamestate->ticker++ % RATE == 0) {	// check every second for timeouts
			CheckTimeouts(&start);
		}

		gettimeofday(&end, NULL);
		sleep = (1000/RATE) - (end.tv_usec - start.tv_usec);
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
			cout << "removing player " << (int)players[i]->getData()->id << endl;
			RemovePlayer(i);
		}
	}
}

/**
 * keeps the players array coalesced
 */
void GameManager::RemovePlayer(int index) {

	pthread_mutex_lock(&playerLock);

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

	pthread_mutex_unlock(&playerLock);
}

/**
 * keeps the players array as alive first, then dead
 */
void GameManager::KillPlayer(int index) {

	pthread_mutex_lock(&playerLock);

	cout << "killing player " << (int)players[index]->getData()->id << endl;
	players[index]->kill();
	numAlivePlayers--;

	if (index >= numAlivePlayers) { // already ordered
		pthread_mutex_unlock(&playerLock);
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

	pthread_mutex_unlock(&playerLock);
}

JoinMessage GameManager::Respawn(string ip, char playerId) {
	JoinMessage j;
	int index = -1;

	pthread_mutex_lock(&playerLock);

	for (int i = 0; i < numPlayers; i++) {
		if (players[i]->getIp().compare(ip) == 0 && players[i]->getData()->id == playerId) {
			index = i;
			break;
		}
	}

	if (index == -1) {	// not found
		pthread_mutex_unlock(&playerLock);
		return j;
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

	j.playerId = playerId;
	j.spawnpoint = rand() % 10;

	pthread_mutex_unlock(&playerLock);

	return j;
}

void GameManager::SwapPlayers(int index1, int index2) {
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
 * players join over tcp
 * @see TcpServer
 */
JoinMessage GameManager::AcceptJoin(string ip) {

	pthread_mutex_lock(&playerLock);

	JoinMessage j;

	if (numPlayers == MAX_PLAYERS) {
		cout << "connection refused" << endl;
		pthread_mutex_unlock(&playerLock);
		return j;
	}

	players[numPlayers] = new Player(autoIncrementId, ip, &gamestate->playersData[numPlayers]);

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


	j.playerId = autoIncrementId++;
	j.spawnpoint = rand() % 10;

	pthread_mutex_unlock(&playerLock);

	return j;
}

GameManager::~GameManager() {
	delete tcpServer;
	delete udpServer;
	delete players;
	pthread_mutex_destroy(&playerLock);
}

