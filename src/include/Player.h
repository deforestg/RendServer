 /*
 * Player.h
 *
 *  Created on: Aug 26, 2014
 *      Author: gdeforest
 */

#ifndef PLAYER_H_
#define PLAYER_H_

#include <string>
#include <sys/time.h>

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
		timeval* LastUpdated() { return &updated; }
	protected:
		timeval updated;
		PlayerData* data;
		string ip;// 45 chars (bytes) is max ipv6 length
};

#endif /* PLAYER_H_ */
