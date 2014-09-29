/*
 * GameManager.cpp
 *
 *  Created on: Sep 2, 2014
 *      Author: gdeforest
 */

#include "include/GameManager.h"

/**
 * initialize game data, player manager, and udp and tcp listeners
 * @param boost::asio::io_service& ioService
 */
void GameManager::init(boost::asio::io_service& ioService) {
	this->ioService = &ioService;
	autoIncrementId = 1;
	gamestate = new Tick();
	gamestate->ticker = 0;
	gameStatus = new ServerMessage();

    pm = &PlayerManager::GetInstance();
    pm->init(gamestate, gameStatus, &playerLock);

    tcpServer = new TcpServer(ioService);
    udpServer = new UdpServer(ioService);
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

void* GameManager::RunHelper(void *context) {
    ((GameManager *)context)->Run();
    return NULL;
}

/**
 * main game loop, 128 tick
 */
void GameManager::Run() {
	timeval start, end;
	int sleep;

	for (;;) {
		gettimeofday(&start, NULL);

		if (gamestate->ticker++ % RATE == 0) {	// check every second for timeouts
			pm->CheckTimeouts(&start);
		}

		gettimeofday(&end, NULL);
		sleep = (1000/RATE) - (end.tv_usec - start.tv_usec);
		if (sleep > 0) {
			usleep(sleep * 1000);
		}
	}
}

/**
 * @param string ip
 * @param char playerId
 * @return ServerMessage*
 */
ServerMessage* GameManager::Leave(string ip, char playerId) {
	pm->RemovePlayer(ip, playerId);

	return gameStatus;
}

/**
 * @param string ip
 * @param char playerId
 * @return ServerMessage*
 */
ServerMessage* GameManager::Respawn(string ip, char playerId) {
	if (pm->Spawn(ip, playerId) < 0) {	// not found
		gameStatus->playerId = 0;
		return gameStatus;
	}

	gameStatus->type = RESPAWN;
	gameStatus->playerId = playerId;
	gameStatus->spawnpoint = rand() % 10;

	return gameStatus;
}

/**
 * players join over tcp
 * @see TcpServer
 * @return ServerMessage*
 */
ServerMessage* GameManager::AcceptJoin(string ip) {
	if (pm->GetNumPlayers() == MAX_PLAYERS) {
		cout << "connection refused" << endl;
		gameStatus->playerId = 0;
		return gameStatus;
	}

	pm->AddPlayer(ip, autoIncrementId);

	gameStatus->playerId = autoIncrementId++;
	gameStatus->spawnpoint = rand() % 10;

	return gameStatus;
}

GameManager::~GameManager() {
	delete tcpServer;
	delete udpServer;
	pthread_mutex_destroy(&playerLock);
}

