 /*
 * Player.h
 *
 *  Created on: Aug 26, 2014
 *      Author: gdeforest
 */

#ifndef PLAYER_H_
#define PLAYER_H_

#define MAX_PLAYERS 16

typedef struct {
	long ip;
	float position[3];
	float look[3];
	char health;
} PlayerData;

class Player
{
	public:
		Player(long ip);
		void update(char* raw);
		PlayerData* getData();
	private:
		PlayerData* data;
};

#endif /* PLAYER_H_ */
