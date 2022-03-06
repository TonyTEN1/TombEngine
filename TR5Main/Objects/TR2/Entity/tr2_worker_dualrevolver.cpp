#include "framework.h"
#include "Objects/TR2/Entity/tr2_worker_dualrevolver.h"

#include "Game/control/box.h"
#include "Game/itemdata/creature_info.h"
#include "Game/items.h"
#include "Game/Lara/lara.h"
#include "Game/people.h"
#include "Specific/level.h"
#include "Specific/setup.h"

BITE_INFO workerDualGunL = { -2, 275, 23, 6 };
BITE_INFO workerDualGunR = { 2, 275, 23, 10 };

void WorkerDualGunControl(short itemNum)
{
	if (!CreatureActive(itemNum))
		return;

	ITEM_INFO* item;
	CREATURE_INFO* dual;
	AI_INFO info;
	short angle, head_x, head_y, torso_x, torso_y, tilt;

	item = &g_Level.Items[itemNum];
	dual = (CREATURE_INFO*)item->Data;
	angle = head_x = head_y = torso_x = torso_y = tilt = 0;

	if (item->HitPoints <= 0)
	{
		if (item->ActiveState != 11)
		{
			item->AnimNumber = Objects[item->ObjectNumber].animIndex + 32;
			item->FrameNumber = g_Level.Anims[item->AnimNumber].frameBase;
			item->ActiveState = 11;
		}
	}
	else if (LaraItem->HitPoints <= 0)
	{
		item->TargetState = 2;
	}
	else
	{
		CreatureAIInfo(item, &info);

		GetCreatureMood(item, &info, VIOLENT);
		CreatureMood(item, &info, VIOLENT);

		angle = CreatureTurn(item, dual->maximumTurn);

		switch (item->ActiveState)
		{
		case 1:
		case 2:
			dual->maximumTurn = 0;

			if (info.ahead)
			{
				head_y = info.angle;
				head_x = info.xAngle;
			}

			if (dual->mood == MoodType::Attack || LaraItem->HitPoints > 0)
			{
				if (Targetable(item, &info))
				{
					if (info.distance <= 0x900000)
						item->TargetState = 9;
					else
						item->TargetState = 3;
				}
				else
				{
					switch (dual->mood)
					{
					case MoodType::Attack:
						if (info.distance > 0x19000000 || !info.ahead)
							item->TargetState = 4;
						else
							item->TargetState = 3;
						break;
					case MoodType::Escape:
						item->TargetState = 4;
						break;
					case MoodType::Stalk:
						item->TargetState = 3;
						break;

					default:
						if (!info.ahead)
							item->TargetState = 3;
						break;
					}
				}
			}
			else
			{
				item->TargetState = 1;
			}
			break;
		case 3:
			dual->maximumTurn = ANGLE(3);

			if (info.ahead)
			{
				head_y = info.angle;
				head_x = info.xAngle;
			}

			if (Targetable(item, &info))
			{
				if (info.distance < 0x900000 || info.zoneNumber != info.enemyZone)
				{
					item->TargetState = 1;
				}
				else
				{
					if (info.angle >= 0)
						item->TargetState = 6;
					else
						item->TargetState = 5;
				}
			}

			if (dual->mood == MoodType::Escape)
			{
				item->TargetState = 4;
			}
			else if (dual->mood == MoodType::Attack || dual->mood == MoodType::Stalk)
			{
				if (info.distance > 0x19000000 || !info.ahead)
					item->TargetState = 4;
			}
			else if (LaraItem->HitPoints > 0)
			{
				if (info.ahead)
					item->TargetState = 1;
			}
			else
			{
				item->TargetState = 2;
			}
			break;
		case 4:
			dual->maximumTurn = ANGLE(6);

			if (info.ahead)
			{
				head_y = info.angle;
				head_x = info.xAngle;
			}

			tilt = angle / 4;

			if (Targetable(item, &info))
			{
				if (info.zoneNumber == info.enemyZone)
				{
					if (info.angle >= 0)
						item->TargetState = 6;
					else
						item->TargetState = 5;
				}
				else
				{
					item->TargetState = 3;
				}
			}
			else if (dual->mood == MoodType::Attack)
			{
				if (info.ahead && info.distance < 0x19000000)
					item->TargetState = 3;
			}
			else if (LaraItem->HitPoints > 0)
			{
				item->TargetState = 1;
			}
			else
			{
				item->TargetState = 2;
			}
			break;
		case 5:
			dual->flags = 0;

			if (info.ahead)
			{
				torso_y = info.angle;
				torso_x = info.xAngle;
			}

			if (Targetable(item, &info))
				item->TargetState = 7;
			else
				item->TargetState = 3;
			break;
		case 6:
			dual->flags = 0;

			if (info.ahead)
			{
				torso_y = info.angle;
				torso_x = info.xAngle;
			}

			if (Targetable(item, &info))
				item->TargetState = 8;
			else
				item->TargetState = 3;
			break;
		case 7:
			if (info.ahead)
			{
				torso_y = info.angle;
				torso_x = info.xAngle;
			}

			if (!dual->flags)
			{
				ShotLara(item, &info, &workerDualGunL, torso_y, 50);
				dual->flags = 1;
			}
			break;
		case 8:
			if (info.ahead)
			{
				torso_y = info.angle;
				torso_x = info.xAngle;
			}

			if (!dual->flags)
			{
				ShotLara(item, &info, &workerDualGunR, torso_y, 50);
				dual->flags = 1;
			}
			break;
		case 9:
			dual->flags = 0;

			if (info.ahead)
			{
				torso_y = info.angle;
				torso_x = info.xAngle;
			}

			if (Targetable(item, &info))
				item->TargetState = 10;
			else
				item->TargetState = 1;
			break;
		case 10:
			if (info.ahead)
			{
				torso_y = info.angle;
				torso_x = info.xAngle;
			}

			if (!dual->flags)
			{
				ShotLara(item, &info, &workerDualGunL, torso_y, 50);
				ShotLara(item, &info, &workerDualGunR, torso_y, 50);
				dual->flags = 1;
			}
			break;
		}
	}

	CreatureTilt(item, tilt);
	CreatureJoint(item, 0, torso_y);
	CreatureJoint(item, 1, torso_x);
	CreatureJoint(item, 2, head_y);
	CreatureJoint(item, 3, head_x);
	CreatureAnimation(itemNum, angle, tilt);
}