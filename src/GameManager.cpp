/*
 * GameManager.cpp
 *
 *  Created on: Sep 2, 2014
 *      Author: gdeforest
 */

#include "include/GameManager.h"

GameManager::GameManager(boost::asio::io_service& ioService) {
	numPlayers = 0;
	autoIncrementId = 1;
	players = new Player*[MAX_PLAYERS];

	this->ioService = &ioService;
    tcpServer = new TcpServer(ioService, this);
    udpServer = new UdpServer(ioService, this);
}

void GameManager::start() {
}

char* GameManager::AcceptJoin(char* ip) {
	if (GetNumPlayers() == MAX_PLAYERS) {
		// TODO: kill connection
		return "stop";
	}

	players[numPlayers] = new Player(autoIncrementId++, ip);
//	char* playerLocation = tick + sizeof(PlayerData)*numPlayers;
//	PlayerData* newPlayer = players[numPlayers++]->getData();
//	memcpy(playerLocation, newPlayer, sizeof(PlayerData));

	return "go";
}

GameManager::~GameManager() {
	delete tcpServer;
	delete udpServer;
}

