 /*
 * Player.h
 *
 *  Created on: Aug 26, 2014
 *      Author: gdeforest
 */

#ifndef PLAYER_H_
#define PLAYER_H_

#include <string.h>
#include <string>
#include <sys/time.h>

using namespace std;

typedef struct {
	char id;
	float position[3];
	float look[3];
	unsigned short playerState;
	char health;
	char weaponId;
	unsigned short weaponState;
	char pullingTrigger;
} PlayerData;

class Player
{
	protected:
		timeval updated;
		PlayerData* data;
		string ip;// 45 chars (bytes) is max ipv6 length
	public:
		Player(char id, string ip, PlayerData* dataStruct);
		void update(PlayerData* newData);
		PlayerData* getData();
		string getIp() { return ip; }
		timeval* LastUpdated() { return &updated; }
};

#endif /* PLAYER_H_ */
