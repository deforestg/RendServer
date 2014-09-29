/*
 * Player.cpp
 *
 *  Created on: Aug 26, 2014
 *      Author: gdeforest
 */

#include "include/Player.h"

/**
 * initialize player
 * @param char id
 * @param string ip
 * @param PlayerData* playerData
 * @param PlayerStatus* status
 */
Player::Player(char id, string ip, PlayerData* playerData, PlayerStatus* status)
{
	this->ip = ip;
	this->status = status;
	data = playerData;
	data->id = id;
	data->health = 100;
	alive = 1;
	gettimeofday(&updated, NULL);
}

/**
 * player id cannot change, everything else gets overwritten
 * @param PlayerData* playerData
 */
void Player::update(PlayerData* newData)
{
	char playerId = data->id;
	memcpy(data, newData, sizeof(PlayerData));
	data->id = playerId;	// cannot change id
	gettimeofday(&updated, NULL);
}

/**
 * @returns PlayerData*
 */
PlayerData* Player::getData()
{
	return data;
}

/**
 * @returns PlayerStatus*
 */
PlayerStatus* Player::getStatus()
{
	return status;
}

/**
 * @param PlayerData*
 */
void Player::setData(PlayerData* playerData)
{
	data = playerData;
}

/**
 * @param PlayerStatus*
 */
void Player::setStatus(PlayerStatus* playerStatus)
{
	status = playerStatus;
}

/**
 * kill player
 */
void Player::kill()
{
	alive = 0;
	data->health = 0;
	status->deaths++;
	gettimeofday(&updated, NULL);
}

/**
 * spawn player
 */
void Player::spawn()
{
	alive = 1;
	data->health = 100;
	gettimeofday(&updated, NULL);
}
