/*
 * Player.cpp
 *
 *  Created on: Aug 26, 2014
 *      Author: gdeforest
 */

#include "include/Player.h"

Player::Player(char id, string ip)
{
	this->ip = ip;
	data = new PlayerData();
	data->id = id;
	data->health = 100;
	gettimeofday(&updated, NULL);
}

void Player::update(char* raw)
{
	char playerId = data->id;
	data = (PlayerData*)raw;
	data->id = playerId;	// client cannot change their id
	gettimeofday(&updated, NULL);
}

PlayerData* Player::getData()
{
	return data;
}
