#include "framework.h"
#include "Game/items.h"
#include "Game/collision/collide_room.h"
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
#include "Game/misc.h"

bool VonCroyPassedWaypoints[128];
BITE_INFO VonCroyBite = { 0, 35, 130, 18 };

#define SWAPMESHFLAGS_VON_CROY	0x40080
#define VON_CROY_FLAG_JUMP		6

enum VonCroyState
{
	VON_CROY_STATE_IDLE = 1,
	VON_CROY_STATE_WALK = 2,
	VON_CROY_STATE_RUN = 3,
	VON_CROY_STATE_START_MONKEY = 4,
	VON_CROY_STATE_MONKEY = 5,
	VON_CROY_STATE_TOGGLE_KNIFE = 6,
	VON_CROY_STATE_LOOK_BEFORE_JUMP = 7,

	VON_CROY_STATE_CALL_LARA_1 = 13,
	VON_CROY_STATE_CALL_LARA_2 = 14,
	VON_CROY_STATE_JUMP = 15,
	VON_CROY_STATE_JUMP_2_BLOCKS = 16,

	VON_CROY_STATE_ENABLE_TRAP = 20,
	VON_CROY_STATE_KNIFE_ATTACK_HIGH = 21,
	VON_CROY_STATE_LOOK_BACK_LEFT = 22,

	VON_CROY_STATE_STEP_DOWN_HIGH = 26,
	VON_CROY_STATE_GRAB_LADDER = 27,
	VON_CROY_STATE_CLIMB_LADDER_RIGHT = 28,

	VON_CROY_STATE_LADDER_CLIMB_UP = 30,
	VON_CROY_STATE_KNIFE_ATTACK_LOW = 31,

	VON_CROY_STATE_JUMP_BACK = 34,
	VON_CROY_STATE_LOOK_BACK_RIGHT = 35
};

// TODO
enum VonCroyAnim
{
	VON_CROY_ANIM_STEP_DOWN_HIGH = 36,

	VON_CROY_ANIM_CLIMB_UP_AFTER_JUMP = 52
};

void InitialiseVonCroy(short itemNumber)
{
	auto* item = &g_Level.Items[itemNumber];
	
	ClearItem(itemNumber);
	
	item->Animation.AnimNumber = Objects[item->ObjectNumber].animIndex + 11;
	item->Animation.FrameNumber = g_Level.Anims[item->Animation.AnimNumber].frameBase;
	item->Animation.TargetState = VON_CROY_STATE_TOGGLE_KNIFE;
	item->Animation.ActiveState = VON_CROY_STATE_TOGGLE_KNIFE;
	item->SwapMeshFlags = SWAPMESHFLAGS_VON_CROY;

	memset(VonCroyPassedWaypoints, 0, 128);
}

void VonCroyControl(short itemNumber)
{
	auto* item = &g_Level.Items[itemNumber];

	if (!CreatureActive(itemNumber))
		return;

	auto* creature = GetCreatureInfo(item);
	auto* obj = &Objects[item->ObjectNumber];

	short angle = 0;
	short tilt = 0;
	short joint0 = 0;
	short joint1 = 0;
	short joint2 = 0;
	short joint3 = 0;

	// Check whether Von Croy can jump 1 or 2 blocks.

	int x = item->Pose.Position.x;
	int z = item->Pose.Position.z;

	int dx = 808 * phd_sin(item->Pose.Orientation.y);
	int dz = 808 * phd_cos(item->Pose.Orientation.y);

	x += dx;
	z += dz;
	int height1 = GetCollision(x, item->Pose.Position.y, z, item->RoomNumber).Position.Floor;

	x += dx;
	z += dz;
	int height2 = GetCollision(x, item->Pose.Position.y, z, item->RoomNumber).Position.Floor;

	x += dx;
	z += dz;
	int height3 = GetCollision(x, item->Pose.Position.y, z, item->RoomNumber).Position.Floor;

	x += dx;
	z += dz;
	auto probe = GetCollision(x, item->Pose.Position.y, z, item->RoomNumber);
	int height4 = probe.Position.Floor;

	bool canJump1block = true;
	if (item->BoxNumber == LaraItem->BoxNumber ||
		item->Pose.Position.y >= height1 - CLICK(1.5f) ||
		item->Pose.Position.y >= height2 + CLICK(1) ||
		item->Pose.Position.y <= height2 - CLICK(1))
	{
		canJump1block = false;
	}

	bool canJump2blocks = true;
	if (item->BoxNumber == LaraItem->BoxNumber ||
		item->Pose.Position.y >= height1 - CLICK(1.5f) ||
		item->Pose.Position.y >= height2 - CLICK(1.5f) ||
		item->Pose.Position.y >= height3 + CLICK(1) ||
		item->Pose.Position.y <= height3 - CLICK(1))
	{
		canJump2blocks = false;
	}

	bool canJump3blocks = true;
	if (item->BoxNumber == LaraItem->BoxNumber ||
		item->Pose.Position.y >= height1 - CLICK(1.5f) ||
		item->Pose.Position.y >= height2 - CLICK(1.5f) ||
		item->Pose.Position.y >= height3 - CLICK(1.5f) ||
		item->Pose.Position.y >= height4 + CLICK(1) ||
		item->Pose.Position.y <= height4 - CLICK(1))
	{
		canJump3blocks = false;
	}

	// Von Croy must follow Lara and navigate with ID_AI_FOLLOW objects
	item->AIBits = FOLLOW;
	GetAITarget(creature);


	// Try to find a possible enemy or target
	ItemInfo* foundTarget = NULL;

	if (Lara.Location <= creature->LocationAI)
	{
		int minDistance = INT_MAX;
		int distance;
		auto* targetCreature = ActiveCreatures[0];

		for (int i = 0; i < ActiveCreatures.size(); i++)
		{
			targetCreature = ActiveCreatures[i];

			if (targetCreature->ItemNumber == NO_ITEM ||
				targetCreature->ItemNumber == itemNumber ||
				g_Level.Items[targetCreature->ItemNumber].ObjectNumber == ID_VON_CROY ||
				g_Level.Items[targetCreature->ItemNumber].ObjectNumber == ID_GUIDE)
			{
				continue;
			}

			auto* currentItem = &g_Level.Items[targetCreature->ItemNumber];

			if (abs(currentItem->Pose.Position.y - item->Pose.Position.y) <= CLICK(2))
			{
				dx = currentItem->Pose.Position.x - item->Pose.Position.x;
				dz = currentItem->Pose.Position.z - item->Pose.Position.z;

				if (abs(dx) < SECTOR(5) && abs(dz) < SECTOR(5))
				{
					distance = pow(dx, 2) + pow(dz, 2);
					if (distance < minDistance)
					{
						creature->ReachedGoal = false;
						foundTarget = currentItem;
						minDistance = distance;
					}
				}
			}
		}
	}

	// If a target is found, it becomes the enemy.
	ItemInfo* enemy = creature->Enemy;
	if (foundTarget != 0)
		creature->Enemy = foundTarget;

	AI_INFO AI;

	// HACK: Even the most advanced zone in TR must have a step height of 1024, so we need to recreate zones when step difference is higher.
	if (item->Animation.AnimNumber == Objects[item->ObjectNumber].animIndex + VON_CROY_ANIM_STEP_DOWN_HIGH ||
		item->Animation.AnimNumber == Objects[item->ObjectNumber].animIndex + VON_CROY_ANIM_CLIMB_UP_AFTER_JUMP)
	{
		short oldRoom = item->RoomNumber;
		item->Pose.Position.x += dx;
		item->Pose.Position.z += dz;

		GetFloor(item->Pose.Position.x, item->Pose.Position.y, item->Pose.Position.z, &item->RoomNumber);

		if (item->Animation.FrameNumber == g_Level.Anims[item->Animation.AnimNumber].frameBase + 1)
			CreateZone(item);
		
		CreatureAIInfo(item, &AI);

		item->RoomNumber = oldRoom;
		item->Pose.Position.x -= dx;
		item->Pose.Position.z -= dz;
	}
	else
		CreatureAIInfo(item, &AI);

	GetCreatureMood(item, &AI, VIOLENT);
	CreatureMood(item, &AI, VIOLENT);

	AI_INFO laraAI;
	if (creature->Enemy == LaraItem)
		memcpy(&laraAI, &AI, sizeof(AI_INFO));
	else
	{
		dx = LaraItem->Pose.Position.x - item->Pose.Position.x;
		dz = LaraItem->Pose.Position.z - item->Pose.Position.z;
		laraAI.angle = phd_atan(dz, dx) - item->Pose.Orientation.y;

		laraAI.ahead = true;
		if (laraAI.angle <= -ANGLE(90) || laraAI.angle >= ANGLE(90.0f))
			laraAI.ahead = false;

		laraAI.enemyFacing = laraAI.angle - LaraItem->Pose.Position.x + -ANGLE(180.0f);

		int distance = 0;
		if (dz > SECTOR(31.25f) || dz < -SECTOR(31.25f) || dx > SECTOR(31.25f) || dx < -SECTOR(31.25f))
			laraAI.distance = INT_MAX;
		else
			laraAI.distance = pow(dx, 2) + pow(dz, 2);

		dx = abs(dx);
		dz = abs(dz);

		int dy = item->Pose.Position.y - LaraItem->Pose.Position.y;
		short rot2 = 0;

		if (dx <= dz)
			laraAI.xAngle = phd_atan(dz + (dx / 2), dy);
		else
			laraAI.xAngle = phd_atan(dx + (dz / 2), dy);
	}

	if (abs(laraAI.angle) < ANGLE(33.75f) && laraAI.distance < pow(1024, 2))
		laraAI.bite = true;
	else
		laraAI.bite = false;

	angle = CreatureTurn(item, creature->MaxTurn);

	if (foundTarget != NULL)
	{
		creature->Enemy = enemy;
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
	bool flags;

	TENLog("State:" + std::to_string(item->Animation.ActiveState), LogLevel::Info);
	
	switch (item->Animation.ActiveState)
	{
	case VON_CROY_STATE_IDLE:
		creature->LOT.IsMonkeying = false;
		creature->LOT.IsJumping = false;
		creature->MaxTurn = 0;
		creature->Flags = 0;
		joint3 = AI.angle / 2;

		if (AI.ahead && item->AIBits & FOLLOW)
		{
			joint1 = AI.angle / 2;
			joint2 = AI.xAngle;
		}

		if (item->AIBits & GUARD)
		{
			joint3 = AIGuard(creature);
			item->Animation.TargetState = 0;
			break;
		}

		if (item->AIBits & MODIFY)
		{
			item->Animation.TargetState = VON_CROY_STATE_IDLE;
			if (item->Floor > (item->Pose.Position.y + CLICK(3)))
				item->AIBits &= ~MODIFY;

			break;
		}

		if (canJump3blocks || item->ItemFlags[2] == VON_CROY_FLAG_JUMP)
		{
			if (item->ItemFlags[2] != VON_CROY_FLAG_JUMP && !canJump2blocks)
				item->Animation.TargetState = VON_CROY_STATE_JUMP_BACK;
			else
				item->Animation.TargetState = VON_CROY_STATE_RUN;
			
			break;
		}
		else if (canJump1block || canJump2blocks)
		{
			item->Animation.AnimNumber = Objects[item->ObjectNumber].animIndex + 22;
			item->Animation.FrameNumber = g_Level.Anims[item->Animation.AnimNumber].frameBase;
			item->Animation.ActiveState = VON_CROY_STATE_JUMP;
			creature->MaxTurn = 0;
			creature->LOT.IsJumping = true;

			if (!canJump2blocks && !canJump3blocks)
				item->Animation.TargetState = VON_CROY_STATE_JUMP;
			else
				item->Animation.TargetState = VON_CROY_STATE_JUMP_2_BLOCKS;
			
			break;
		}

		if (creature->MonkeySwingAhead)
		{
			probe = GetCollision(item->Pose.Position.x, item->Pose.Position.y, item->Pose.Position.z, probe.RoomNumber);
			if (probe.Position.Ceiling == probe.Position.Floor - 1536)
			{
				if (item->SwapMeshFlags == SWAPMESHFLAGS_VON_CROY)
					item->Animation.TargetState = VON_CROY_STATE_TOGGLE_KNIFE;
				else
					item->Animation.TargetState = VON_CROY_STATE_START_MONKEY;

				break;
			}
		}
		else
		{
			if (creature->Enemy && creature->Enemy->HitPoints > 0 && AI.distance < pow(1024, 2) && creature->Enemy != LaraItem &&
				creature->Enemy->ObjectNumber != ID_AI_FOLLOW)
			{
				if (AI.bite)
				{
					if (enemy->HitPoints > 0 && AI.ahead)
					{
						if (abs(enemy->Pose.Position.y - item->Pose.Position.y + CLICK(2)) < CLICK(2))
							item->Animation.TargetState = VON_CROY_STATE_KNIFE_ATTACK_HIGH;
						else
							item->Animation.TargetState = VON_CROY_STATE_KNIFE_ATTACK_LOW;

						break;
					}
				}
			}
		}

		item->Animation.TargetState = VON_CROY_STATE_WALK;
		break;

	case VON_CROY_STATE_WALK:
		creature->MaxTurn = ANGLE(7.0f);
		creature->LOT.IsMonkeying = false;
		creature->LOT.IsJumping = false;
		creature->Flags = 0;

		if (laraAI.ahead)
			joint3 = laraAI.angle;
		else if (AI.ahead)
			joint3 = AI.angle;
		
		if (canJump1block || canJump2blocks || canJump3blocks)
		{
			item->Animation.TargetState = VON_CROY_STATE_IDLE;
			creature->MaxTurn = 0;
			break;
		}

		if (creature->ReachedGoal && creature->MonkeySwingAhead)
		{
			item->Animation.TargetState = VON_CROY_STATE_IDLE;
			break;
		}

		if (creature->ReachedGoal)
		{
			if (!creature->Enemy->Flags)
			{
				item->AIBits = FOLLOW;
				item->ItemFlags[3]++;
				creature->ReachedGoal = false;
				creature->Enemy = NULL;
				break;
			}

			item->Animation.TargetState = VON_CROY_STATE_IDLE;
			break;
		}
		else
		{
			if (Lara.Location >= item->ItemFlags[3])
			{
				if (!foundTarget || AI.distance >= SECTOR(2048) && (item->SwapMeshFlags & 0x40000 || AI.distance >= SECTOR(8216)))
				{
					if (creature->Enemy == LaraItem)
					{
						if (AI.distance >= SECTOR(4096))
						{
							if (AI.distance > SECTOR(16384))
								item->Animation.TargetState = VON_CROY_STATE_RUN;
						}
						else
							item->Animation.TargetState = VON_CROY_STATE_IDLE;
					}
					else if (Lara.Location > item->ItemFlags[3] && laraAI.distance > SECTOR(4096))
						item->Animation.TargetState = VON_CROY_STATE_RUN;
				}
				else
					item->Animation.TargetState = VON_CROY_STATE_IDLE;
			}
			else
				item->Animation.TargetState = VON_CROY_STATE_IDLE;
		}

		if (AI.bite)
		{
			if (AI.distance < pow(SECTOR(1), 2))
			{
				item->Animation.TargetState = VON_CROY_STATE_IDLE;
				break;
			}
		}

		if (creature->Mood == MoodType::Attack &&
			!(creature->JumpAhead) &&
			AI.distance > pow(SECTOR(1), 2))
		{
			item->Animation.TargetState = VON_CROY_STATE_RUN;
		}

		break;

	case VON_CROY_STATE_RUN:
		if (AI.ahead)
			joint3 = AI.angle;

		if (item->ItemFlags[2] == VON_CROY_FLAG_JUMP)
		{
			item->Animation.TargetState = VON_CROY_STATE_JUMP_2_BLOCKS;
			creature->MaxTurn = 0;
			break;
		}

		creature->MaxTurn = ANGLE(11.0f);
		tilt = abs(angle) / 2;

		if (AI.distance < pow(SECTOR(2), 2) || Lara.Location < creature->LocationAI)
		{
			item->Animation.TargetState = VON_CROY_STATE_IDLE;
			break;
		}

		if (creature->ReachedGoal)
		{
			if (!enemy->Flags)
			{
				item->AIBits = FOLLOW;
				creature->ReachedGoal = false;
				creature->Enemy = NULL;
				break;
			}

			item->Animation.TargetState = VON_CROY_STATE_IDLE;
			break;
		}

		if (canJump1block ||
			canJump2blocks ||
			canJump3blocks ||
			item->AIBits & FOLLOW ||
			creature->MonkeySwingAhead ||
			creature->JumpAhead ||
			AI.distance < pow(SECTOR(1), 2))
		{
			item->Animation.TargetState = VON_CROY_STATE_IDLE;
			break;
		}

		if (AI.distance < pow(SECTOR(1), 2))
		{
			item->Animation.TargetState = VON_CROY_STATE_WALK;
			break;
		}

		break;

	case VON_CROY_STATE_START_MONKEY:
		creature->MaxTurn = 0;

		if (item->BoxNumber != creature->LOT.TargetBox && creature->MonkeySwingAhead) 
			item->Animation.TargetState = VON_CROY_STATE_MONKEY;
		else
		{
			probe = GetCollision(item->Pose.Position.x, item->Pose.Position.y, item->Pose.Position.z, probe.RoomNumber);
			if (probe.Position.Ceiling == probe.Position.Floor - 1536)
				item->Animation.TargetState = VON_CROY_STATE_IDLE;
		}

		break;

	case VON_CROY_STATE_MONKEY:
		creature->MaxTurn = ANGLE(6.0f);
		creature->LOT.IsMonkeying = true;
		creature->LOT.IsJumping = true;

		if (item->BoxNumber == creature->LOT.TargetBox || !creature->MonkeySwingAhead)
		{
			probe = GetCollision(item->Pose.Position.x, item->Pose.Position.y, item->Pose.Position.z, probe.RoomNumber);
			if (probe.Position.Ceiling == (probe.Position.Floor - SECTOR(1.5f)))
				item->Animation.TargetState = VON_CROY_STATE_START_MONKEY;
		}

		break;

	case VON_CROY_STATE_TOGGLE_KNIFE:
		if (item->Animation.FrameNumber == g_Level.Anims[item->Animation.AnimNumber].frameBase) 
		{
			if (!(item->SwapMeshFlags & SWAPMESHFLAGS_VON_CROY))
				item->SwapMeshFlags |= SWAPMESHFLAGS_VON_CROY;
			else 
				item->SwapMeshFlags &= ~SWAPMESHFLAGS_VON_CROY;
		}

		break;

	case VON_CROY_STATE_LOOK_BEFORE_JUMP:
		if (item->Animation.FrameNumber == g_Level.Anims[item->Animation.AnimNumber].frameBase)
		{
			item->Pose = enemy->Pose;

			if (item->ItemFlags[2] == VON_CROY_FLAG_JUMP)
			{
				creature->MaxTurn = 0;
				item->Animation.ActiveState = VON_CROY_STATE_JUMP;
				item->Animation.TargetState = VON_CROY_STATE_JUMP_2_BLOCKS;
				item->Animation.AnimNumber = Objects[item->ObjectNumber].animIndex + 22;
				item->Animation.FrameNumber = g_Level.Anims[item->Animation.AnimNumber].frameBase;
				creature->LOT.IsJumping = true;
			}

			item->AIBits = FOLLOW;
			creature->ReachedGoal = false;
			creature->Enemy = NULL;
			creature->LocationAI++;
		}

		break;

	case VON_CROY_STATE_JUMP_2_BLOCKS:
		if (item->Animation.AnimNumber == Objects[item->ObjectNumber].animIndex + 25 ||
			item->Animation.FrameNumber > g_Level.Anims[item->Animation.AnimNumber].frameBase + 5)
		{
			creature->LOT.IsJumping = true;
			//if (canJump3blocks)
			//	item->itemFlags[2] = VON_CROY_FLAG_JUMP;
		}
		else if (canJump1block)
			item->Animation.TargetState = VON_CROY_STATE_JUMP;

		if (item->ItemFlags[2] == VON_CROY_FLAG_JUMP)
			item->Animation.TargetState = 33;

		break;

	case VON_CROY_STATE_ENABLE_TRAP:
		if (item->Animation.FrameNumber == g_Level.Anims[item->Animation.AnimNumber].frameBase)
			item->Pose = enemy->Pose;
		else if (item->Animation.FrameNumber == g_Level.Anims[item->Animation.AnimNumber].frameBase + 120)
		{
			TestTriggers(
				creature->AITarget->Pose.Position.x,
				creature->AITarget->Pose.Position.y,
				creature->AITarget->Pose.Position.z,
				creature->AITarget->RoomNumber,
				true);

			creature->ReachedGoal = false;
			creature->Enemy = NULL;
			item->AIBits = FOLLOW;
			creature->LocationAI++;
		}

		break;

	case VON_CROY_STATE_KNIFE_ATTACK_HIGH:
		creature->MaxTurn = 0;
		ClampRotation(&item->Pose, AI.angle, ANGLE(6.0f));

		if (AI.ahead) 
		{
			joint2 = AI.angle / 2;
			joint1 = AI.xAngle / 2;
			joint0 = joint2;
		}

		if (!creature->Flags && enemy != NULL) 
		{
			if (item->Animation.FrameNumber > g_Level.Anims[item->Animation.AnimNumber].frameBase + 20 &&
				item->Animation.FrameNumber > g_Level.Anims[item->Animation.AnimNumber].frameBase + 45) 
			{
				if (abs(item->Pose.Position.x - enemy->Pose.Position.x) < CLICK(2) &&
					abs(item->Pose.Position.y - enemy->Pose.Position.y) < CLICK(2) &&
					abs(item->Pose.Position.z - enemy->Pose.Position.z) < CLICK(2))
				{
					enemy->HitPoints -= 40;
					if (enemy->HitPoints <= 0)
						item->AIBits = FOLLOW;
					
					enemy->HitStatus = true;
					creature->Flags = 1;
					CreatureEffect2(item, &VonCroyBite, 2, -1, DoBloodSplat);
				}
			}
		}

		break;

	case VON_CROY_STATE_LOOK_BACK_LEFT:
	case VON_CROY_STATE_LOOK_BACK_RIGHT:
		creature->MaxTurn = 0;

		if (item->ItemFlags[2] == 0)
			ClampRotation(&item->Pose, laraAI.angle, ANGLE(2.8f));
		else
			ClampRotation(&item->Pose, enemy->Pose.Orientation.y - item->Pose.Orientation.y, ANGLE(2.8f));
		
		break;

	case VON_CROY_STATE_GRAB_LADDER:
		creature->MaxTurn = 0;
		creature->LOT.IsJumping = true;

		/*if (!creature->reachedGoal) 
			item->TargetState = VON_CROY_STATE_CLIMB_LADDER_RIGHT;
		else
		{*/
			item->Animation.TargetState = VON_CROY_STATE_LADDER_CLIMB_UP;
			creature->ReachedGoal = false;
			creature->Enemy = NULL;
			item->AIBits = FOLLOW;
			creature->LocationAI++;
		//}

		break;

	case VON_CROY_STATE_CLIMB_LADDER_RIGHT:
		creature->MaxTurn = 0;
		creature->LOT.IsJumping = true;
		break;

	case VON_CROY_STATE_KNIFE_ATTACK_LOW:
		if (AI.ahead)
		{
			joint2 = AI.angle / 2;
			joint1 = AI.xAngle / 2;
			joint0 = joint2;
		}

		creature->MaxTurn = 0;
		ClampRotation(&item->Pose, AI.angle, ANGLE(6.0f));

		if ((enemy == NULL || enemy->Flags != 0) ||
			item->Animation.FrameNumber <= g_Level.Anims[item->Animation.AnimNumber].frameBase + 21)
		{
			if (creature->Flags == 0 && enemy != NULL) 
			{
				if (item->Animation.FrameNumber > g_Level.Anims[item->Animation.AnimNumber].frameBase + 15 &&
					item->Animation.FrameNumber < g_Level.Anims[item->Animation.AnimNumber].frameBase + 26)
				{
					if (abs(item->Pose.Position.x - enemy->Pose.Position.x) < CLICK(2) &&
						abs(item->Pose.Position.y - enemy->Pose.Position.y) < CLICK(2) &&
						abs(item->Pose.Position.z - enemy->Pose.Position.z) < CLICK(2))
					{
						enemy->HitPoints -= 20;
						if (enemy->HitPoints <= 0)
							item->AIBits = FOLLOW;
						
						enemy->HitStatus = true;
						creature->Flags = 1;
						CreatureEffect2(item, &VonCroyBite, 2, -1, DoBloodSplat);
					}
				}
			}

			break;
		}

		TestTriggers(
			creature->AITarget->Pose.Position.x,
			creature->AITarget->Pose.Position.y,
			creature->AITarget->Pose.Position.z,
			creature->AITarget->RoomNumber,
			true);

		item->AIBits = FOLLOW;
		creature->ReachedGoal = false;
		creature->Enemy = NULL;
		creature->LocationAI++;

		break;

	case 32:
		creature->MaxTurn = 0;
		ClampRotation(&item->Pose, AI.angle / 2, ANGLE(6.0f));

		if (AI.ahead) 
		{
			joint0 = joint2;
			joint1 = AI.xAngle;
			joint2 = AI.angle / 2;
		}
		
		if (item->Animation.AnimNumber == Objects[item->ObjectNumber].animIndex + 47) 
		{
			if (item->Animation.FrameNumber != g_Level.Anims[item->Animation.AnimNumber].frameBase) 
				break;
		}
		else 
		{
			item->Animation.TargetState = VON_CROY_STATE_IDLE;

			if (GetRandomControl() & 0x1F)
				break;
		}

		item->AIBits = FOLLOW;
		creature->ReachedGoal = false;
		creature->Enemy = NULL;
		creature->LocationAI++;
		break;

	case 33:
		flags = true;
		if (item->Animation.AnimNumber != Objects[item->ObjectNumber].animIndex + 52 ||
			item->Animation.FrameNumber != g_Level.Anims[item->Animation.AnimNumber].frameBase) 
		{
			flags = false;
		}
		
		item->Animation.TargetState = VON_CROY_STATE_WALK;
		item->Animation.RequiredState = VON_CROY_STATE_RUN;
		item->ItemFlags[2] = 0;
		//if (sVar3 == -1) goto LAB_0041a991;
		if (!flags)
		{
			item->AIBits = FOLLOW;
			creature->ReachedGoal = false;
			creature->Enemy = NULL;
			creature->LocationAI++;
		}

		break;
	
	case VON_CROY_STATE_JUMP_BACK:
		item->ItemFlags[2] = VON_CROY_FLAG_JUMP;
		break;

	case 36:
	case 37:
		creature->MaxTurn = 0;
		MoveCreature3DPos(&item->Pose, &enemy->Pose, 15, enemy->Pose.Orientation.y-item->Pose.Orientation.y, 512);
		break;
	}

	CreatureTilt(item, tilt);
	CreatureJoint(item, 0, joint0);
	CreatureJoint(item, 1, joint1);
	CreatureJoint(item, 2, joint2);
	CreatureJoint(item, 3, joint3);
	
	if ((item->Animation.ActiveState < 15) && (item->Animation.ActiveState != 5)) 
	{
		switch (CreatureVault(itemNumber, angle, 2, 260)) 
		{
		case 2:
			item->Animation.AnimNumber = Objects[item->ObjectNumber].animIndex + 29;
			item->Animation.FrameNumber = g_Level.Anims[item->Animation.AnimNumber].frameBase;
			item->Animation.ActiveState = 19;
			creature->MaxTurn = 0;
			break;

		case 3:
			item->Animation.AnimNumber = Objects[item->ObjectNumber].animIndex + 28;
			item->Animation.FrameNumber = g_Level.Anims[item->Animation.AnimNumber].frameBase;
			item->Animation.ActiveState = 18;
			creature->MaxTurn = 0;
			break;

		case 4:
			item->Animation.AnimNumber = Objects[item->ObjectNumber].animIndex + 27;
			item->Animation.FrameNumber = g_Level.Anims[item->Animation.AnimNumber].frameBase;
			item->Animation.ActiveState = 17;
			creature->MaxTurn = 0;
			break;

		case 7:
			item->Animation.AnimNumber = Objects[item->ObjectNumber].animIndex + 37;
			item->Animation.FrameNumber = g_Level.Anims[item->Animation.AnimNumber].frameBase;
			item->Animation.ActiveState = 27;
			creature->MaxTurn = 0;
			break;

		case -7:
			item->Animation.AnimNumber = Objects[item->ObjectNumber].animIndex + 36;
			item->Animation.FrameNumber = g_Level.Anims[item->Animation.AnimNumber].frameBase;
			item->Animation.ActiveState = 26;
			creature->MaxTurn = 0;
			break;

		case -4:
			item->Animation.AnimNumber = Objects[item->ObjectNumber].animIndex + 35;
			item->Animation.FrameNumber = g_Level.Anims[item->Animation.AnimNumber].frameBase;
			item->Animation.ActiveState = 25;
			creature->MaxTurn = 0;
			break;

		case -3:
			item->Animation.AnimNumber = Objects[item->ObjectNumber].animIndex + 41;
			item->Animation.FrameNumber = g_Level.Anims[item->Animation.AnimNumber].frameBase;
			item->Animation.ActiveState = 24;
			creature->MaxTurn = 0;
			break;

		case -2:
			item->Animation.AnimNumber = Objects[item->ObjectNumber].animIndex + 42;
			item->Animation.FrameNumber = g_Level.Anims[item->Animation.AnimNumber].frameBase;
			item->Animation.ActiveState = 23;
			creature->MaxTurn = 0;
			break;
		}
	}
	else
		CreatureAnimation(itemNumber, angle, 0);
}