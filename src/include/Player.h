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
	char shotBy;
} PlayerData;

typedef struct {
	short kills;
	short deaths;
	short ping;
} PlayerStatus;

class Player
{
	protected:
		timeval updated;
		PlayerData* data;
		PlayerStatus* status;
		string ip;// 45 chars (bytes) is max ipv6 length
		char alive;
	public:
		Player(char id, string ip, PlayerData* dataStruct, PlayerStatus* status);
		void update(PlayerData* newData);
		void setData(PlayerData* playerData);
		void setStatus(PlayerStatus* playerData);
		void kill();
		void spawn();
		char isAlive() { return alive; };
		PlayerData* getData();
		PlayerStatus* getStatus();
		string getIp() { return ip; }
		timeval* LastUpdated() { return &updated; }
};

#endif /* PLAYER_H_ */
