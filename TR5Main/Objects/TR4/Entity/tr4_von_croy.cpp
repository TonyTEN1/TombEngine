#include "framework.h"
#include "Game/items.h"
#include "Game/control/box.h"
#include "Game/control/lot.h"
#include "Game/effects/effects.h"
#include "Game/effects/tomb4fx.h"
#include "Specific/setup.h"
#include "Specific/level.h"
#include "Game/Lara/lara.h"
#include "Sound/sound.h"
#include "Game/animation.h"
#include "Game/itemdata/creature_info.h"

#define STATE_VON_CROY_STOP						1
#define STATE_VON_CROY_WALK						2
#define STATE_VON_CROY_RUN						3
#define STATE_VON_CROY_START_MONKEY				4
#define STATE_VON_CROY_MONKEY					5
#define STATE_VON_CROY_TOGGLE_KNIFE				6
#define STATE_VON_CROY_LOOK_BEFORE_JUMP			7
#define STATE_VON_CROY_CALL_LARA1				13
#define STATE_VON_CROY_CALL_LARA2				14
#define STATE_VON_CROY_JUMP						15
#define STATE_VON_CROY_JUMP_2BLOCKS				16
#define STATE_VON_CROY_ENABLE_TRAP				20
#define STATE_VON_CROY_KNIFE_ATTACK_HIGH		21
#define STATE_VON_CROY_LOOK_BACK_LEFT			22
#define STATE_VON_CROY_STEP_DOWN_HIGH			26
#define STATE_VON_CROY_GRAB_LADDER				27
#define STATE_VON_CROY_CLIMB_LADDER_RIGHT		28
#define STATE_VON_CROY_LADDER_CLIMB_UP			30
#define STATE_VON_CROY_KNIFE_ATTACK_LOW			31
#define STATE_VON_CROY_JUMP_BACK				34
#define STATE_VON_CROY_LOOK_BACK_RIGHT			35

#define ANIMATION_VON_CROY_STEP_DOWN_HIGH		36
#define ANIMATION_VON_CROY_CLIMB_UP_AFTER_JUMP	52

#define SWAPMESHFLAGS_VON_CROY					0x40080

#define VON_CROY_FLAG_JUMP						6

bool VonCroyPassedWaypoints[128];
BITE_INFO VonCroyBite = { 0, 35, 130, 18 };

void InitialiseVonCroy(short itemNumber)
{
	ITEM_INFO* item = &g_Level.Items[itemNumber];
	
	ClearItem(itemNumber);
	
	item->AnimNumber = Objects[item->ObjectNumber].animIndex + 11;
	item->FrameNumber = g_Level.Anims[item->AnimNumber].frameBase;
	item->TargetState = STATE_VON_CROY_TOGGLE_KNIFE;
	item->ActiveState = STATE_VON_CROY_TOGGLE_KNIFE;
	item->SwapMeshFlags = SWAPMESHFLAGS_VON_CROY;

	memset(VonCroyPassedWaypoints, 0, 128);
}

void VonCroyControl(short itemNumber)
{
	ITEM_INFO* item = &g_Level.Items[itemNumber];

	if (!CreatureActive(itemNumber))
		return;

	CREATURE_INFO* creature = (CREATURE_INFO*)item->Data;
	OBJECT_INFO* obj = &Objects[item->ObjectNumber];

	short angle = 0;
	short joint3 = 0;
	short joint2 = 0;
	short joint1 = 0;
	short joint0 = 0;
	short tilt = 0;

	// check if Von Croy can jump 1 or 2 blocks

	int x = item->Position.xPos;
	int z = item->Position.zPos;

	int dx = 808 * phd_sin(item->Position.yRot);
	int dz = 808 * phd_cos(item->Position.yRot);

	x += dx;
	z += dz;

	short roomNumber = item->RoomNumber;
	FLOOR_INFO* floor = GetFloor(x, item->Position.yPos, z, &roomNumber);
	int height1 = GetFloorHeight(floor, x, item->Position.yPos, z);

	x += dx;
	z += dz;

	roomNumber = item->RoomNumber;
	floor = GetFloor(x, item->Position.yPos, z, &roomNumber);
	int height2 = GetFloorHeight(floor, x, item->Position.yPos, z);

	x += dx;
	z += dz;

	roomNumber = item->RoomNumber;
	floor = GetFloor(x, item->Position.yPos, z, &roomNumber);
	int height3 = GetFloorHeight(floor, x, item->Position.yPos, z);

	x += dx ;
	z += dz ;

	roomNumber = item->RoomNumber;
	floor = GetFloor(x, item->Position.yPos, z, &roomNumber);
	int height4 = GetFloorHeight(floor, x, item->Position.yPos, z);

	bool canJump1block;
	if (item->BoxNumber == LaraItem->BoxNumber
		|| item->Position.yPos >= height1 - 384
		|| item->Position.yPos >= height2 + 256
		|| item->Position.yPos <= height2 - 256)
		canJump1block = false;
	else
		canJump1block = true;

	bool canJump2blocks;
	if (item->BoxNumber == LaraItem->BoxNumber
		|| item->Position.yPos >= height1 - 384
		|| item->Position.yPos >= height2 - 384
		|| item->Position.yPos >= height3 + 256
		|| item->Position.yPos <= height3 - 256)
		canJump2blocks = false;
	else
		canJump2blocks = true;

	bool canJump3blocks;
	if (item->BoxNumber == LaraItem->BoxNumber
		|| item->Position.yPos >= height1 - 384
		|| item->Position.yPos >= height2 - 384
		|| item->Position.yPos >= height3 - 384
		|| item->Position.yPos >= height4 + 256
		|| item->Position.yPos <= height4 - 256)
		canJump3blocks = false;
	else
		canJump3blocks = true;

	// Von Croy must follow Lara and navigate with ID_AI_FOLLOW objects
	item->AIBits = FOLLOW;
	GetAITarget(creature);

	AI_INFO info;
	AI_INFO laraInfo;

	// Try to find a possible enemy or target
	ITEM_INFO* foundTarget = NULL;

	if (Lara.location <= item->LocationAI)
	{
		int minDistance = 0x7FFFFFFF;
		int distance;
		CREATURE_INFO* baddie = ActiveCreatures[0];

		for (int i = 0; i < ActiveCreatures.size(); i++)
		{
			baddie = ActiveCreatures[i];

			if (baddie->itemNum == NO_ITEM
				|| baddie->itemNum == itemNumber
				|| g_Level.Items[baddie->itemNum].ObjectNumber == ID_VON_CROY
				|| g_Level.Items[baddie->itemNum].ObjectNumber == ID_GUIDE)
				continue;

			ITEM_INFO* currentItem = &g_Level.Items[baddie->itemNum];
			if (abs(currentItem->Position.yPos - item->Position.yPos) <= 512)
			{
				dx = currentItem->Position.xPos - item->Position.xPos;
				dz = currentItem->Position.zPos - item->Position.zPos;

				if (abs(dx) < 5120 && abs(dz) < 5120)
				{
					distance = SQUARE(dx) + SQUARE(dz);
					if (distance < minDistance)
					{
						creature->reachedGoal = false;
						foundTarget = currentItem;
						minDistance = distance;
					}
				}
			}
		}
	}

	// If a target is found, that it becomes the enemy
	ITEM_INFO* enemy = creature->enemy;
	if (foundTarget != 0)
	{
		creature->enemy = foundTarget;
	}

	// HACK: even the most advanced zone in TR must have a step height of 1024, so we need to recreate zones when step difference is higher
	if (item->AnimNumber == Objects[item->ObjectNumber].animIndex + ANIMATION_VON_CROY_STEP_DOWN_HIGH
		|| item->AnimNumber == Objects[item->ObjectNumber].animIndex + ANIMATION_VON_CROY_CLIMB_UP_AFTER_JUMP)
	{
		short oldRoom = item->RoomNumber;
		item->Position.xPos += dx;
		item->Position.zPos += dz;

		GetFloor(item->Position.xPos, item->Position.yPos, item->Position.zPos, &item->RoomNumber);

		if (item->FrameNumber == g_Level.Anims[item->AnimNumber].frameBase + 1)
		{
			CreateZone(item);
		}
		CreatureAIInfo(item, &info);

		item->RoomNumber = oldRoom;
		item->Position.xPos -= dx;
		item->Position.zPos -= dz;
	}
	else
	{
		CreatureAIInfo(item, &info);
	}

	GetCreatureMood(item, &info, VIOLENT);
	CreatureMood(item, &info, VIOLENT);

	if (creature->enemy == LaraItem)
	{
		memcpy(&laraInfo, &info, sizeof(AI_INFO));
	}
	else
	{
		dx = LaraItem->Position.xPos - item->Position.xPos;
		dz = LaraItem->Position.zPos - item->Position.zPos;
		laraInfo.angle = phd_atan(dz, dx) - item->Position.yRot;

		laraInfo.ahead = true;
		if (laraInfo.angle <= -ANGLE(90) || laraInfo.angle >= ANGLE(90))
			laraInfo.ahead = false;

		laraInfo.enemyFacing = laraInfo.angle - LaraItem->Position.xPos + -ANGLE(180);
		int distance = 0;
		if (dz > 32000 || dz < -32000 || dx > 32000 || dx < -32000)
			laraInfo.distance = 0x7FFFFFFF;
		else
			laraInfo.distance = dx * dx + dz * dz;

		dx = abs(dx);
		dz = abs(dz);

		int dy = item->Position.yPos - LaraItem->Position.yPos;
		short rot2 = 0;

		if (dx <= dz)
			laraInfo.xAngle = phd_atan(dz + (dx / 2), dy);
		else
			laraInfo.xAngle = phd_atan(dx + (dz / 2), dy);
	}

	if (abs(laraInfo.angle) < 6144 && laraInfo.distance < SQUARE(1024))
		laraInfo.bite = true;
	else
		laraInfo.bite = false;

	angle = CreatureTurn(item, creature->maximumTurn);

	if (foundTarget != NULL)
	{
		creature->enemy = enemy;
		enemy = foundTarget;
	}

	// NOTE: I've removed here a bunch of if (Lara.location == X) 
	// made for making Von Croy wait for Lara in tutorial area

	/*if (!VonCroyPassedWaypoints[item->location] &&
		(((creature->reachedGoal 
			&& item->location == Lara.locationPad)
			|| item->triggerFlags > 0)
			|| (VonCroyPassedWaypoints[item->location] <= Lara.locationPad
				&& !VonCroyPassedWaypoints[Lara.locationPad])))
	{
		CreatureJoint(item, 0, laraInfo.angle >> 1);
		CreatureJoint(item, 1, laraInfo.angle >> 1);
		CreatureJoint(item, 2, laraInfo.angle >> 1);
		CreatureJoint(item, 3, laraInfo.angle >> 1);
		VonCroyAnimation(item, creature);
		return;
	}*/

	short rot = 0;
	int dy, height, ceiling, flags;

	TENLog("State:" + std::to_string(item->ActiveState), LogLevel::Info);
	
	switch (item->ActiveState)
	{
	case STATE_VON_CROY_STOP:
		creature->LOT.isMonkeying = false;
		creature->LOT.isJumping = false;
		creature->flags = 0;
		creature->maximumTurn = 0;
		joint3 = info.angle / 2;
		if (info.ahead && item->AIBits & FOLLOW)
		{
			joint1 = info.angle / 2;
			joint2 = info.xAngle;
		}

		if (item->AIBits & GUARD)
		{
			joint3 = AIGuard(creature);
			item->TargetState = 0;
			break;
		}

		if (item->AIBits & MODIFY)
		{
			item->TargetState = STATE_VON_CROY_STOP;
			if (item->Floor > item->Position.yPos + (STEP_SIZE * 3))
				item->AIBits &= ~MODIFY;
			break;
		}

		if (canJump3blocks || item->ItemFlags[2] == VON_CROY_FLAG_JUMP)
		{
			if (item->ItemFlags[2] != VON_CROY_FLAG_JUMP && !canJump2blocks)
			{
				item->TargetState = STATE_VON_CROY_JUMP_BACK;
			}
			else
			{
				item->TargetState = STATE_VON_CROY_RUN;
			}
			break;
		}
		else if (canJump1block || canJump2blocks)
		{
			creature->maximumTurn = 0;
			item->AnimNumber = Objects[item->ObjectNumber].animIndex + 22;
			item->FrameNumber = g_Level.Anims[item->AnimNumber].frameBase;
			item->ActiveState = STATE_VON_CROY_JUMP;
			creature->LOT.isJumping = true;

			if (!canJump2blocks && !canJump3blocks)
				item->TargetState = STATE_VON_CROY_JUMP;
			else
			{
				item->TargetState = STATE_VON_CROY_JUMP_2BLOCKS;
			}
			break;
		}

		if (creature->monkeyAhead)
		{
			floor = GetFloor(item->Position.xPos, item->Position.yPos, item->Position.zPos, &roomNumber);
			height = GetFloorHeight(floor, item->Position.xPos, item->Position.yPos, item->Position.zPos);
			if (GetCeiling(floor, item->Position.xPos, item->Position.yPos, item->Position.zPos) == height - 1536)
			{
				if (item->SwapMeshFlags == SWAPMESHFLAGS_VON_CROY)
					item->TargetState = STATE_VON_CROY_TOGGLE_KNIFE;
				else
					item->TargetState = STATE_VON_CROY_START_MONKEY;
				break;
			}
		}
		else
		{
			if (creature->enemy && creature->enemy->HitPoints > 0 && info.distance < SQUARE(1024) && creature->enemy != LaraItem 
				&& creature->enemy->ObjectNumber != ID_AI_FOLLOW)
			{
				if (info.bite)
				{
					if (enemy->HitPoints > 0 && info.ahead)
					{
						if (abs(enemy->Position.yPos - item->Position.yPos + 512) < 512)
							item->TargetState = STATE_VON_CROY_KNIFE_ATTACK_HIGH;
						else
							item->TargetState = STATE_VON_CROY_KNIFE_ATTACK_LOW;
						break;
					}
				}
			}
		}

		item->TargetState = STATE_VON_CROY_WALK;
		break;

	case STATE_VON_CROY_WALK:
		creature->LOT.isMonkeying = false;
		creature->LOT.isJumping = false;
		creature->maximumTurn = ANGLE(7);
		creature->flags = 0;

		if (laraInfo.ahead)
		{
			joint3 = laraInfo.angle;
		}
		else if (info.ahead)
		{
			joint3 = info.angle;
		}
		
		if (canJump1block || canJump2blocks || canJump3blocks)
		{
			creature->maximumTurn = 0;
			item->TargetState = STATE_VON_CROY_STOP;
			break;
		}

		if (creature->reachedGoal && creature->monkeyAhead)
		{
			item->TargetState = STATE_VON_CROY_STOP;
			break;
		}

		if (creature->reachedGoal)
		{
			if (!creature->enemy->Flags)
			{
				creature->reachedGoal = false;
				creature->enemy = NULL;
				item->AIBits = FOLLOW;
				item->ItemFlags[3]++;

				break;
			}
			item->TargetState = STATE_VON_CROY_STOP;
			break;
		}
		else
		{
			if (Lara.location >= item->ItemFlags[3])
			{
				if (!foundTarget || info.distance >= 0x200000 && (item->SwapMeshFlags & 0x40000 || info.distance >= 9437184))
				{
					if (creature->enemy == LaraItem)
					{
						if (info.distance >= 0x400000)
						{
							if (info.distance > 0x1000000)
							{
								item->TargetState = STATE_VON_CROY_RUN;
							}
						}
						else
						{
							item->TargetState = STATE_VON_CROY_STOP;
						}
					}
					else if (Lara.location > item->ItemFlags[3] && laraInfo.distance > 0x400000)
					{
						item->TargetState = STATE_VON_CROY_RUN;
					}
				}
				else
				{
					item->TargetState = STATE_VON_CROY_STOP;
				}
			}
			else
			{
				item->TargetState = STATE_VON_CROY_STOP;
			}
		}

		if (info.bite)
		{
			if (info.distance < SQUARE(1024))
			{
				item->TargetState = STATE_VON_CROY_STOP;
				break;
			}
		}

		if (creature->mood == MoodType::Attack &&
			!(creature->jumpAhead) &&
			info.distance > SQUARE(1024))
		{
			item->TargetState = STATE_VON_CROY_RUN;
		}
		break;

	case STATE_VON_CROY_RUN:
		if (info.ahead)
		{
			joint3 = info.angle;
		}

		if (item->ItemFlags[2] == VON_CROY_FLAG_JUMP)
		{
			creature->maximumTurn = 0;
			item->TargetState = STATE_VON_CROY_JUMP_2BLOCKS;
			break;
		}

		creature->maximumTurn = ANGLE(11);
		tilt = abs(angle) / 2;

		if (info.distance < SQUARE(2048) || Lara.location < item->LocationAI)
		{
			item->TargetState = STATE_VON_CROY_STOP;
			break;
		}

		if (creature->reachedGoal)
		{
			if (!enemy->Flags)
			{
				creature->reachedGoal = false;
				creature->enemy = NULL;
				item->AIBits = FOLLOW;

				break;
			}

			item->TargetState = STATE_VON_CROY_STOP;
			break;
		}

		if (canJump1block
			|| canJump2blocks
			|| canJump3blocks
			|| creature->monkeyAhead
			|| item->AIBits & FOLLOW
			|| info.distance < SQUARE(1024)
			|| creature->jumpAhead)
		{
			item->TargetState = STATE_VON_CROY_STOP;
			break;
		}

		if (info.distance < SQUARE(1024))
		{
			item->TargetState = STATE_VON_CROY_WALK;
			break;
		}

		break;

	case STATE_VON_CROY_START_MONKEY:
		creature->maximumTurn = 0;
		if (item->BoxNumber != creature->LOT.targetBox && creature->monkeyAhead) 
		{
			item->TargetState = STATE_VON_CROY_MONKEY;
		}
		else
		{
			floor = GetFloor(item->Position.xPos, item->Position.yPos, item->Position.zPos, &roomNumber);
			height = GetFloorHeight(floor, item->Position.xPos, item->Position.yPos, item->Position.zPos);
			ceiling = GetCeiling(floor, item->Position.xPos, item->Position.yPos, item->Position.zPos);
			if (ceiling == height - 1536)
				item->TargetState = STATE_VON_CROY_STOP;
		}

		break;

	case STATE_VON_CROY_MONKEY:
		creature->LOT.isMonkeying = true;
		creature->LOT.isJumping = true;
		creature->maximumTurn = ANGLE(6);

		if (item->BoxNumber == creature->LOT.targetBox || !creature->monkeyAhead)
		{
			floor = GetFloor(item->Position.xPos, item->Position.yPos, item->Position.zPos, &roomNumber);
			height = GetFloorHeight(floor, item->Position.xPos, item->Position.yPos, item->Position.zPos);
			if (GetCeiling(floor, item->Position.xPos, item->Position.yPos, item->Position.zPos) == height - 1536)
				item->TargetState = STATE_VON_CROY_START_MONKEY;
		}

		break;

	case STATE_VON_CROY_TOGGLE_KNIFE:
		if (item->FrameNumber == g_Level.Anims[item->AnimNumber].frameBase) 
		{
			if (!(item->SwapMeshFlags & SWAPMESHFLAGS_VON_CROY))
			{
				item->SwapMeshFlags |= SWAPMESHFLAGS_VON_CROY;
			}
			else 
			{
				item->SwapMeshFlags &= ~SWAPMESHFLAGS_VON_CROY;
			}
		}
		break;

	case STATE_VON_CROY_LOOK_BEFORE_JUMP:
		if (item->FrameNumber == g_Level.Anims[item->AnimNumber].frameBase)
		{
			item->Position.xPos = enemy->Position.xPos;
			item->Position.yPos = enemy->Position.yPos;
			item->Position.zPos = enemy->Position.zPos;
			item->Position.xRot = enemy->Position.xRot;
			item->Position.yRot = enemy->Position.yRot;
			item->Position.zRot = enemy->Position.zRot;

			if (item->ItemFlags[2] == VON_CROY_FLAG_JUMP)
			{
				creature->maximumTurn = 0;
				item->ActiveState = STATE_VON_CROY_JUMP;
				item->TargetState = STATE_VON_CROY_JUMP_2BLOCKS;
				item->AnimNumber = Objects[item->ObjectNumber].animIndex + 22;
				item->FrameNumber = g_Level.Anims[item->AnimNumber].frameBase;
				creature->LOT.isJumping = true;
			}

			creature->reachedGoal = false;
			creature->enemy = NULL;
			item->AIBits = FOLLOW;
			item->LocationAI++;
		}

		break;

	case STATE_VON_CROY_JUMP_2BLOCKS:
		if (item->AnimNumber == Objects[item->ObjectNumber].animIndex + 25
			|| item->FrameNumber > g_Level.Anims[item->AnimNumber].frameBase + 5)
		{
			creature->LOT.isJumping = true;
			//if (canJump3blocks)
			//	item->itemFlags[2] = VON_CROY_FLAG_JUMP;
		}
		else if (canJump1block)
		{
			item->TargetState = STATE_VON_CROY_JUMP;
		}

		if (item->ItemFlags[2] == VON_CROY_FLAG_JUMP)
		{
			item->TargetState = 33;
		}

		break;

	case STATE_VON_CROY_ENABLE_TRAP:
		if (item->FrameNumber == g_Level.Anims[item->AnimNumber].frameBase)
		{
			item->Position.xPos = enemy->Position.xPos;
			item->Position.yPos = enemy->Position.yPos;
			item->Position.zPos = enemy->Position.zPos;
			item->Position.xRot = enemy->Position.xRot;
			item->Position.yRot = enemy->Position.yRot;
			item->Position.zRot = enemy->Position.zRot;
		}
		else if (item->FrameNumber == g_Level.Anims[item->AnimNumber].frameBase + 120)
		{
			TestTriggers(
				creature->aiTarget->Position.xPos,
				creature->aiTarget->Position.yPos,
				creature->aiTarget->Position.zPos,
				creature->aiTarget->RoomNumber,
				true);

			creature->reachedGoal = false;
			creature->enemy = NULL;
			item->AIBits = FOLLOW;
			item->LocationAI++;
		}

		break;

	case STATE_VON_CROY_KNIFE_ATTACK_HIGH:
		if (info.ahead) 
		{
			joint2 = info.angle / 2;
			joint1 = info.xAngle / 2;
			joint0 = joint2;
		}

		creature->maximumTurn = 0;
		ClampRotation(&item->Position, info.angle, ANGLE(6));

		if (!creature->flags && enemy != NULL) 
		{
			if (item->FrameNumber > g_Level.Anims[item->AnimNumber].frameBase + 20 
				&& item->FrameNumber > g_Level.Anims[item->AnimNumber].frameBase + 45) 
			{
				if (abs(item->Position.xPos - enemy->Position.xPos) < 512
					&& abs(item->Position.yPos - enemy->Position.yPos) < 512
					&& abs(item->Position.zPos - enemy->Position.zPos) < 512)
				{
					enemy->HitPoints -= 40;
					if (enemy->HitPoints <= 0)
					{
						item->AIBits = FOLLOW;
					}
					enemy->HitStatus = true;
					creature->flags = 1;
					CreatureEffect2(item, &VonCroyBite, 2, -1, DoBloodSplat);
				}
			}
		}

		break;

	case STATE_VON_CROY_LOOK_BACK_LEFT:
	case STATE_VON_CROY_LOOK_BACK_RIGHT:
		creature->maximumTurn = 0;
		if (item->ItemFlags[2] == 0)
		{
			ClampRotation(&item->Position, laraInfo.angle, 512);
		}
		else
		{
			ClampRotation(&item->Position, enemy->Position.yRot - item->Position.yRot, 512);
		}
		break;

	case STATE_VON_CROY_GRAB_LADDER:
		creature->LOT.isJumping = true;
		creature->maximumTurn = 0;

		/*if (!creature->reachedGoal) 
		{
			item->TargetState = STATE_VON_CROY_CLIMB_LADDER_RIGHT;
		}
		else
		{*/
			item->TargetState = STATE_VON_CROY_LADDER_CLIMB_UP;
			creature->reachedGoal = false;
			creature->enemy = NULL;
			item->AIBits = FOLLOW;
			item->LocationAI++;
		//}
		break;

	case STATE_VON_CROY_CLIMB_LADDER_RIGHT:
		creature->LOT.isJumping = true;
		creature->maximumTurn = 0;
		break;

	case STATE_VON_CROY_KNIFE_ATTACK_LOW:
		if (info.ahead)
		{
			joint2 = info.angle / 2;
			joint1 = info.xAngle / 2;
			joint0 = joint2;
		}

		creature->maximumTurn = 0;
		ClampRotation(&item->Position, info.angle, ANGLE(6));

		if ((enemy == NULL
			|| enemy->Flags != 0) 
			|| item->FrameNumber <= g_Level.Anims[item->AnimNumber].frameBase + 21)
		{
			if (creature->flags == 0 && enemy != NULL) 
			{
				if (item->FrameNumber > g_Level.Anims[item->AnimNumber].frameBase + 15 
					&& item->FrameNumber < g_Level.Anims[item->AnimNumber].frameBase + 26)
				{
					if (abs(item->Position.xPos - enemy->Position.xPos) < 512
						&& abs(item->Position.yPos - enemy->Position.yPos) < 512
						&& abs(item->Position.zPos - enemy->Position.zPos) < 512)
					{
						enemy->HitPoints -= 20;
						if (enemy->HitPoints <= 0)
						{
							item->AIBits = FOLLOW;
						}
						enemy->HitStatus = true;
						creature->flags = 1;
						CreatureEffect2(item, &VonCroyBite, 2, -1, DoBloodSplat);
					}
				}
			}
			break;
		}

		TestTriggers(
			creature->aiTarget->Position.xPos,
			creature->aiTarget->Position.yPos,
			creature->aiTarget->Position.zPos,
			creature->aiTarget->RoomNumber,
			true);

		creature->reachedGoal = false;
		creature->enemy = NULL;
		item->AIBits = FOLLOW;
		item->LocationAI++;

		break;

	case 32:
		if (info.ahead) 
		{
			joint2 = info.angle / 2;
			joint1 = info.xAngle;
			joint0 = joint2;
		}
		
		creature->maximumTurn = 0;
		ClampRotation(&item->Position, info.angle / 2, ANGLE(6));

		if (item->AnimNumber == Objects[item->ObjectNumber].animIndex + 47) 
		{
			if (item->FrameNumber != g_Level.Anims[item->AnimNumber].frameBase) 
				break;
		}
		else 
		{
			if (GetRandomControl() & 0x1F)
				break;
			item->TargetState = STATE_VON_CROY_STOP;
		}

		creature->reachedGoal = false;
		creature->enemy = NULL;
		item->AIBits = FOLLOW;
		item->LocationAI++;

		break;

	case 33:
		flags = 1;
		if (item->AnimNumber != Objects[item->ObjectNumber].animIndex + 52
			|| item->FrameNumber != g_Level.Anims[item->AnimNumber].frameBase) 
		{
			flags = 0;
		}
		
		item->TargetState = STATE_VON_CROY_WALK;
		item->RequiredState = STATE_VON_CROY_RUN;
		item->ItemFlags[2] = 0;
		//if (sVar3 == -1) goto LAB_0041a991;
		if (!flags)
		{
			creature->reachedGoal = false;
			creature->enemy = NULL;
			item->AIBits = FOLLOW;
			item->LocationAI++;
		}

		break;
	
	case STATE_VON_CROY_JUMP_BACK:
		item->ItemFlags[2] = VON_CROY_FLAG_JUMP;
		break;

	case 36:
	case 37:
		creature->maximumTurn = 0;
		MoveCreature3DPos(&item->Position, &enemy->Position, 15, enemy->Position.yRot-item->Position.yRot, 512);
		break;

	}

	CreatureTilt(item, tilt);
	CreatureJoint(item, 0, joint0);
	CreatureJoint(item, 1, joint1);
	CreatureJoint(item, 2, joint2);
	CreatureJoint(item, 3, joint3);
	
	if ((item->ActiveState < 15) && (item->ActiveState != 5)) 
	{
		switch (CreatureVault(itemNumber, angle, 2, 260)) 
		{
		case 2:
			creature->maximumTurn = 0;
			item->AnimNumber = Objects[item->ObjectNumber].animIndex+29;
			item->ActiveState = 19;
			item->FrameNumber = g_Level.Anims[item->AnimNumber].frameBase;
			break;

		case 3:
			creature->maximumTurn = 0;
			item->AnimNumber = Objects[item->ObjectNumber].animIndex + 28;
			item->ActiveState = 18;
			item->FrameNumber = g_Level.Anims[item->AnimNumber].frameBase;
			break;

		case 4:
			creature->maximumTurn = 0;
			item->AnimNumber = Objects[item->ObjectNumber].animIndex + 27;
			item->ActiveState = 17;
			item->FrameNumber = g_Level.Anims[item->AnimNumber].frameBase;
			break;

		case 7:
			creature->maximumTurn = 0;
			item->AnimNumber = Objects[item->ObjectNumber].animIndex + 37;
			item->ActiveState = 27;
			item->FrameNumber = g_Level.Anims[item->AnimNumber].frameBase;
			break;

		case -7:
			creature->maximumTurn = 0;
			item->AnimNumber = Objects[item->ObjectNumber].animIndex + 36;
			item->ActiveState = 26;
			item->FrameNumber = g_Level.Anims[item->AnimNumber].frameBase;
			break;

		case -4:
			creature->maximumTurn = 0;
			item->AnimNumber = Objects[item->ObjectNumber].animIndex + 35;
			item->ActiveState = 25;
			item->FrameNumber = g_Level.Anims[item->AnimNumber].frameBase;
			break;

		case -3:
			creature->maximumTurn = 0;
			item->AnimNumber = Objects[item->ObjectNumber].animIndex + 41;
			item->ActiveState = 24;
			item->FrameNumber = g_Level.Anims[item->AnimNumber].frameBase;
			break;

		case -2:
			creature->maximumTurn = 0;
			item->AnimNumber = Objects[item->ObjectNumber].animIndex + 42;
			item->ActiveState = 23;
			item->FrameNumber = g_Level.Anims[item->AnimNumber].frameBase;
			break;

		}
	}
	else
	{
		CreatureAnimation(itemNumber, angle, 0);
	}
}