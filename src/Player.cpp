/*
 * Player.cpp
 *
 *  Created on: Aug 26, 2014
 *      Author: gdeforest
 */

#include "include/Player.h"

Player::Player(long ip)
{
	data = new PlayerData();
	data->ip = ip;
	data->health = 100;
}

void Player::update(char* raw)
{
	data = (PlayerData*)raw;
}

PlayerData* Player::getData()
{
	return data;
}
