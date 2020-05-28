#pragma once
#include "global.h"
#include "box.h"

int ShotLara(ITEM_INFO* item, AI_INFO* info, BITE_INFO* gun, short extra_rotation, int damage);
short GunMiss(int x, int y, int z, short speed, short yrot, short roomNumber);
short GunHit(int x, int y, int z, short speed, short yrot, short roomNumber);
short GunShot(int x, int y, int z, short speed, short yrot, short roomNumber);
int Targetable(ITEM_INFO* item, AI_INFO* info);
int TargetVisible(ITEM_INFO* item, AI_INFO* info);
