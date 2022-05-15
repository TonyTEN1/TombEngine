#include "framework.h"
#include "tr4_smallscorpion.h"
#include "Game/control/box.h"
#include "Game/items.h"
#include "Game/effects/effects.h"
#include "Specific/setup.h"
#include "Specific/level.h"
#include "Game/Lara/lara.h"
#include "Game/misc.h"
#include "Game/itemdata/creature_info.h"
#include "Game/control/control.h"

BITE_INFO SmallScorpionBiteInfo1 = { 0, 0, 0, 0 };
BITE_INFO SmallScorpionBiteInfo2 = { 0, 0, 0, 23 };

enum SmallScorionState
{
	SSCORPION_STATE_IDLE = 1,
	SSCORPION_STATE_WALK = 2,
	SSCORPION_STATE_RUN = 3,
	SSCORPION_STATE_ATTACK_1 = 4,
	SSCORPION_STATE_ATTACK_2 = 5,
	SSCORPION_STATE_DEATH_1 = 6,
	SSCORPION_STATE_DEATH_2 = 7
};

// TODO
enum SmallScorpionAnim
{

};

void InitialiseSmallScorpion(short itemNumber)
{
	auto* item = &g_Level.Items[itemNumber];

	ClearItem(itemNumber);

	item->Animation.AnimNumber = Objects[ID_SMALL_SCORPION].animIndex + 2;
	item->Animation.FrameNumber = g_Level.Anims[item->Animation.AnimNumber].frameBase;
	item->Animation.TargetState = SSCORPION_STATE_IDLE;
	item->Animation.ActiveState = SSCORPION_STATE_IDLE;
}

void SmallScorpionControl(short itemNumber)
{
	if (!CreatureActive(itemNumber))
		return;

	auto* item = &g_Level.Items[itemNumber];
	auto* creature = GetCreatureInfo(item);

	short angle = 0;
	short head = 0;
	short neck = 0;
	short tilt = 0;
	short joint0 = 0;
	short joint1 = 0;
	short joint2 = 0;
	short joint3 = 0;

	if (item->HitPoints <= 0)
	{
		item->HitPoints = 0;
		if (item->Animation.ActiveState != SSCORPION_STATE_DEATH_1 &&
			item->Animation.ActiveState != SSCORPION_STATE_DEATH_2)
		{
			item->Animation.AnimNumber = Objects[ID_SMALL_SCORPION].animIndex + 5;
			item->Animation.FrameNumber = g_Level.Anims[item->Animation.AnimNumber].frameBase;
			item->Animation.ActiveState = SSCORPION_STATE_DEATH_1;
		}
	}
	else
	{
		int dx = LaraItem->Pose.Position.x - item->Pose.Position.x;
		int dz = LaraItem->Pose.Position.z - item->Pose.Position.z;
		int laraDistance = dx * dx + dz * dz;

		if (item->AIBits & GUARD)
			GetAITarget(creature);
		else
			creature->Enemy = LaraItem;

		AI_INFO AI;
		CreatureAIInfo(item, &AI);

		GetCreatureMood(item, &AI, VIOLENT);
		CreatureMood(item, &AI, VIOLENT);

		angle = CreatureTurn(item, creature->MaxTurn);

		switch (item->Animation.ActiveState)
		{
		case SSCORPION_STATE_IDLE:
			creature->MaxTurn = 0;
			creature->Flags = 0;

			if (AI.distance > pow(341, 2))
				item->Animation.TargetState = SSCORPION_STATE_WALK;
			else if (AI.bite)
			{
				creature->MaxTurn = ANGLE(6.0f);
				if (GetRandomControl() & 1)
					item->Animation.TargetState = SSCORPION_STATE_ATTACK_1;
				else
					item->Animation.TargetState = SSCORPION_STATE_ATTACK_2;
			}
			else if (!AI.ahead)
				item->Animation.TargetState = SSCORPION_STATE_RUN;
			
			break;

		case SSCORPION_STATE_WALK:
			creature->MaxTurn = ANGLE(6.0f);

			if (AI.distance >= pow(341, 2))
			{
				if (AI.distance > pow(213, 2))
					item->Animation.TargetState = SSCORPION_STATE_RUN;
			}
			else
				item->Animation.TargetState = SSCORPION_STATE_IDLE;
			
			break;

		case SSCORPION_STATE_RUN:
			creature->MaxTurn = ANGLE(8.0f);

			if (AI.distance < pow(341, 2))
				item->Animation.TargetState = SSCORPION_STATE_IDLE;
			
			break;

		case SSCORPION_STATE_ATTACK_1:
		case SSCORPION_STATE_ATTACK_2:
			creature->MaxTurn = 0;

			if (abs(AI.angle) >= ANGLE(6.0f))
			{
				if (AI.angle >= 0)
					item->Pose.Orientation.y += ANGLE(6.0f);
				else
					item->Pose.Orientation.y -= ANGLE(6.0f);
			}
			else
				item->Pose.Orientation.y += AI.angle;
			
			if (!creature->Flags)
			{
				if (item->TouchBits & 0x1B00100)
				{
					if (item->Animation.FrameNumber > g_Level.Anims[item->Animation.AnimNumber].frameBase + 20 &&
						item->Animation.FrameNumber < g_Level.Anims[item->Animation.AnimNumber].frameBase + 32)
					{
						Lara.PoisonPotency += 2;
						LaraItem->HitPoints -= 20;
						LaraItem->HitStatus = true;

						short rotation;
						BITE_INFO* biteInfo;
						if (item->Animation.ActiveState == SSCORPION_STATE_ATTACK_1)
						{
							rotation = item->Pose.Orientation.y + -ANGLE(180.0f);
							biteInfo = &SmallScorpionBiteInfo1;
						}
						else
						{
							rotation = item->Pose.Orientation.y + -ANGLE(180.0f);
							biteInfo = &SmallScorpionBiteInfo2;
						}

						CreatureEffect2(item, biteInfo, 3, rotation, DoBloodSplat);
						creature->Flags = 1;
					}
				}
			}

			break;
		}
	}

	CreatureAnimation(itemNumber, angle, 0);
}