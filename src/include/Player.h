 /*
 * Player.h
 *
 *  Created on: Aug 26, 2014
 *      Author: gdeforest
 */

#include <string>

#ifndef PLAYER_H_
#define PLAYER_H_

using namespace std;

typedef struct {
	char id;
	float position[3];
	float look[3];
	char health;
} PlayerData;

class Player
{
	public:
		Player(char id, string ip);
		void update(char* raw);
		PlayerData* getData();
		string getIp() { return ip; }
	protected:
		PlayerData* data;
		string ip;// 45 chars (bytes) is max ipv6 length
};

#endif /* PLAYER_H_ */
