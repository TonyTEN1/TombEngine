#pragma once

#include "..\Global\global.h"

//#define TestCollision ((int (__cdecl*)(ITEM_INFO*, ITEM_INFO*)) 0x00479170)
#define GetJointAbsPosition ((int (__cdecl*)(ITEM_INFO*, PHD_VECTOR*, int)) 0x00479780)
//#define GetSpheres ((int (__cdecl*)(ITEM_INFO*, SPHERE*, int)) 0x00479380)

extern bool GotLaraSpheres;

int TestCollision(ITEM_INFO* item, ITEM_INFO* l);
int GetSpheres(ITEM_INFO* item, SPHERE* ptr, char worldSpace);

void Inject_Sphere();