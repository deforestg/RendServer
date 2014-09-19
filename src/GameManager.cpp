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
	gm = this;
    pm = new PlayerManager(gamestate, &playerLock);

	autoIncrementId = 1;
	gamestate = new Tick();
	gamestate->ticker = 0;

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

void* GameManager::RunHelper(void *context) {
    ((GameManager *)context)->Run();
    return NULL;
}

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

ServerMessage GameManager::Leave(string ip, char playerId) {
	ServerMessage msg;

	pm->RemovePlayer(ip, playerId);

	return msg;
}

ServerMessage GameManager::Respawn(string ip, char playerId) {
	ServerMessage msg;

	if (pm->Spawn(ip, playerId) < 0) {	// not found
		return msg;
	}

	msg.type = RESPAWN;
	msg.playerId = playerId;
	msg.spawnpoint = rand() % 10;

	return msg;
}

/**
 * players join over tcp
 * @see TcpServer
 */
ServerMessage GameManager::AcceptJoin(string ip) {
	ServerMessage msg;

	if (pm->GetNumPlayers() == MAX_PLAYERS) {
		cout << "connection refused" << endl;
		return msg;
	}

	pm->AddPlayer(autoIncrementId, ip);

	msg.playerId = autoIncrementId++;
	msg.spawnpoint = rand() % 10;

	return msg;
}

GameManager::~GameManager() {
	delete tcpServer;
	delete udpServer;
	delete pm;
	pthread_mutex_destroy(&playerLock);
}

