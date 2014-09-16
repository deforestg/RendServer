/*
 * Player.cpp
 *
 *  Created on: Aug 26, 2014
 *      Author: gdeforest
 */

#include "include/Player.h"

Player::Player(char id, string ip, PlayerData* playerData)
{
	this->ip = ip;
	data = playerData;
	data->id = id;
	data->health = 100;
	alive = 1;
	gettimeofday(&updated, NULL);
}

void Player::update(PlayerData* newData)
{
	char playerId = data->id;
	memcpy(data, newData, sizeof(PlayerData));
	data->id = playerId;	// client cannot change their id
	gettimeofday(&updated, NULL);
}

PlayerData* Player::getData()
{
	return data;
}

void Player::setData(PlayerData* playerData)
{
	data = playerData;
}

void Player::kill()
{
	alive = 0;
}

void Player::spawn()
{
	alive = 1;
	data->health = 100;
}
