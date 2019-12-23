#pragma once

#include "..\Global\global.h"

#define ElectricityWiresControl ((void (__cdecl*)(short)) 0x00442610)
#define InitialiseRomeHammer ((void (__cdecl*)(short)) 0x0043ECB0)
#define InitialiseDeathSlide ((void (__cdecl*)(short)) 0x0041CC70)
#define DeathSlideCollision ((void (__cdecl*)(short,ITEM_INFO*,COLL_INFO*)) 0x0041CCE0)
#define DeathSlideControl ((void (__cdecl*)(short)) 0x0041CE00)
#define RollingBallCollision ((void (__cdecl*)(short,ITEM_INFO*,COLL_INFO*)) 0x0048B6D0)
#define RollingBallControl ((void (__cdecl*)(short)) 0x0048AE60)
#define InitialiseTwoBlocksPlatform ((void (__cdecl*)(short)) 0x0043D5D0)
#define TwoBlocksPlatformControl ((void (__cdecl*)(short)) 0x0048BBB0)
#define TwoBlocksPlatformFloor ((void (__cdecl*)(ITEM_INFO*,int,int,int,int*)) 0x0048B9E0)
#define TwoBlocksPlatformCeiling ((void (__cdecl*)(ITEM_INFO*,int,int,int,int*)) 0x0048BA50)
#define KillAllTriggersControl ((void (__cdecl*)(short)) 0x00431030)
#define FallingCeilingCollision ((void (__cdecl*)(short,ITEM_INFO*,COLL_INFO*)) 0x004127C0)
#define FallingCeilingControl ((void (__cdecl*)(short)) 0x004899D0)
//#define InitialiseFallingBlock ((void (__cdecl*)(short)) 0x0043D330)
//#define FallingBlockCollision ((void (__cdecl*)(short,ITEM_INFO*,COLL_INFO*)) 0x00489750)
//#define FallingBlockControl ((void (__cdecl*)(short)) 0x00489820)
//#define FallingBlockFloor ((void (__cdecl*)(ITEM_INFO*,int,int,int,int*)) 0x00489910)
//#define FallingBlockCeiling ((void (__cdecl*)(ITEM_INFO*,int,int,int,int*)) 0x00489980)
#define InitialisePushableBlock ((void (__cdecl*)(short)) 0x0045E720)
#define PushableBlockControl ((void (__cdecl*)(short)) 0x0045EA30)
#define PushableBlockCollision ((void (__cdecl*)(short,ITEM_INFO*,COLL_INFO*)) 0x0045F570)
#define DartEmitterControl ((void (__cdecl*)(short)) 0x00489B30)
#define DrawDart ((void (__cdecl*)(ITEM_INFO*)) 0x004CBB10)
#define DartControl ((void (__cdecl*)(short)) 0x00489D60)
#define InitialiseFlameEmitter ((void (__cdecl*)(short)) 0x0043D370)
#define FlameEmitterCollision ((void (__cdecl*)(short,ITEM_INFO*,COLL_INFO*)) 0x00433B40)
//#define FlameEmitterControl ((void (__cdecl*)(short)) 0x00489F70)
#define InitialiseFlameEmitter2 ((void (__cdecl*)(short)) 0x0043D4E0)
//#define FlameEmitter2Control ((void (__cdecl*)(short)) 0x0048A3B0)
#define FlameEmitter3Control ((void (__cdecl*)(short)) 0x0048A570)
//#define FlameControl ((void (__cdecl*)(short)) 0x0048AB80)
#define InitialiseRopeTrap ((void (__cdecl*)()) 0x0046EE40)
#define GenSlot1Control ((void (__cdecl*)(short)) 0x00406580)
#define InitialiseGenSlot2 ((void (__cdecl*)(short)) 0x0043FD70)
#define GenSlot2Control ((void (__cdecl*)(short)) 0x00488710)
#define DrawGenSlot2 ((void (__cdecl*)(ITEM_INFO*)) 0x004CFF80)
#define InitialiseGenSlot3 ((void (__cdecl*)(short)) 0x004402E0)
#define InitialiseGenSlot4 ((void (__cdecl*)(short)) 0x00440440)
#define GenSlot4Control ((void (__cdecl*)(short)) 0x00486450)
#define InitialiseHighObject1 ((void (__cdecl*)(short)) 0x0043FC30)
#define HighObject1Control ((void (__cdecl*)(short)) 0x004067E0)
#define InitialisePortal ((void (__cdecl*)(short)) 0x0043FAA0)
#define PortalControl ((void (__cdecl*)(short)) 0x00401AEB)
#define DrawPortal ((void (__cdecl*)(ITEM_INFO*)) 0x004CFF80)
#define InitialiseWreckingBall ((void (__cdecl*)(short)) 0x0043EF20)
#define WreckingBallCollision ((void (__cdecl*)(short,ITEM_INFO*,COLL_INFO*)) 0x00441D50)
#define WreckingBallControl ((void (__cdecl*)(short)) 0x00441410)
#define DrawWreckingBall ((void (__cdecl*)(ITEM_INFO*)) 0x00441F50)
#define InitialiseVentilator ((void (__cdecl*)(short)) 0x0043F3D0)
#define VentilatorControl ((void (__cdecl*)(short)) 0x00405610)
#define InitialiseTeethSpike ((void (__cdecl*)(short)) 0x0043FBC0)
#define TeethSpikeControl ((void (__cdecl*)(short)) 0x0043FBC0)
#define DrawScaledSpike ((void (__cdecl*)(ITEM_INFO*)) 0x0043FBC0)

void LaraBurn();
void LavaBurn(ITEM_INFO* item);
void FlameControl(short fxNumber);
void FlameEmitter2Control(short itemNumber);
void FlameEmitterControl(short itemNumber);
void InitialiseTrapDoor(short itemNumber);
void TrapDoorCollision(short itemNumber, ITEM_INFO* l, COLL_INFO* coll);
void CeilingTrapDoorCollision(short itemNumber, ITEM_INFO* l, COLL_INFO* coll);
void FloorTrapDoorCollision(short itemNumber, ITEM_INFO* l, COLL_INFO* coll);
void TrapDoorControl(short itemNumber);
void CloseTrapDoor(ITEM_INFO* item);
void OpenTrapDoor(ITEM_INFO* item);
void InitialiseFallingBlock(short itemNumber);
void FallingBlockCollision(short itemNum, ITEM_INFO* l, COLL_INFO* coll);
void FallingBlockControl(short itemNumber);
void FallingBlockFloor(ITEM_INFO* item, int x, int y, int z, int* height);
void FallingBlockCeiling(ITEM_INFO* item, int x, int y, int z, int* height);
