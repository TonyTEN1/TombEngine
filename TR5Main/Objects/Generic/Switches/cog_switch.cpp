#include "framework.h"
#include "cog_switch.h"
#include "control.h"
#include "input.h"
#include "lara.h"
#include "generic_switch.h"
#include "door.h"

namespace ten::entities::switches
{
	OBJECT_COLLISION_BOUNDS CogSwitchBounds =
	{
		-512, 512,
		0, 0,
		-1536, -512,
		-ANGLE(10), ANGLE(10),
		-ANGLE(30), ANGLE(30),
		-ANGLE(10), ANGLE(10)
	};
	PHD_VECTOR CogSwitchPos(0, 0, -856);

	void CogSwitchCollision(short itemNum, ITEM_INFO* l, COLL_INFO* coll)
	{
		ITEM_INFO* item = &g_Level.Items[itemNum];

		FLOOR_INFO* floor = GetFloor(item->pos.xPos, item->pos.yPos, item->pos.zPos, &item->roomNumber);
		GetFloorHeight(floor, item->pos.xPos, item->pos.yPos, item->pos.zPos);

		if (!TriggerIndex)
		{
			ObjectCollision(itemNum, l, coll);
			return;
		}

		short* trigger = TriggerIndex;
		for (int i = *TriggerIndex; (i & 0x1F) != 4; trigger++)
		{
			if (i < 0)
				break;
			i = trigger[1];
		}

		ITEM_INFO* target = &g_Level.Items[trigger[3] & 0x3FF];
		DOOR_DATA* door = (DOOR_DATA*)target->data;

		if (item->status == ITEM_NOT_ACTIVE)
		{
			if (!(item->flags & ONESHOT)
				&& (TrInput & IN_ACTION
					&& !Lara.gunStatus
					&& !item->gravityStatus
					&& l->currentAnimState == LS_STOP
					&& l->animNumber == LA_STAND_IDLE
					|| Lara.isMoving
					&& Lara.generalPtr == (void*)itemNum))
			{
				if (TestLaraPosition(&CogSwitchBounds, item, l))
				{
					if (MoveLaraPosition(&CogSwitchPos, item, l))
					{
						Lara.isMoving = false;
						Lara.headYrot = 0;
						Lara.headXrot = 0;
						Lara.torsoYrot = 0;
						Lara.torsoXrot = 0;
						Lara.gunStatus = LG_HANDS_BUSY;
						Lara.generalPtr = target;
						l->animNumber = LA_COGWHEEL_GRAB;
						l->goalAnimState = LS_COGWHEEL;
						l->currentAnimState = LS_COGWHEEL;
						l->frameNumber = g_Level.Anims[l->animNumber].frameBase;

						AddActiveItem(itemNum);
						item->goalAnimState = SWITCH_ON;
						item->status = ITEM_ACTIVE;

						if (door != NULL)
						{
							if (!door->opened)
							{
								AddActiveItem((target - g_Level.Items.data()));
								target->itemFlags[2] = target->pos.yPos;
								target->status = ITEM_ACTIVE;
							}
						}
					}
					else
					{
						Lara.generalPtr = (void*)itemNum;
					}
					return;
				}
				else if (Lara.isMoving && Lara.generalPtr == (void*)itemNum)
				{
					Lara.isMoving = false;
					Lara.gunStatus = LG_NO_ARMS;
				}
			}

			ObjectCollision(itemNum, l, coll);
		}
	}

	void CogSwitchControl(short itemNumber)
	{
		ITEM_INFO* item = &g_Level.Items[itemNumber];

		AnimateItem(item);

		if (item->currentAnimState == SWITCH_ON)
		{
			if (item->goalAnimState == SWITCH_ON && !(TrInput & IN_ACTION))
			{
				LaraItem->goalAnimState = LS_STOP;
				item->goalAnimState = SWITCH_OFF;
			}

			if (LaraItem->animNumber == LA_COGWHEEL_PULL)
			{
				if (LaraItem->frameNumber == g_Level.Anims[LaraItem->animNumber].frameBase + 10)
				{
					((ITEM_INFO*)Lara.generalPtr)->itemFlags[0] = COG_DOOR_TURN;
				}
			}
		}
		else
		{
			if (item->frameNumber == g_Level.Anims[item->animNumber].frameEnd)
			{
				item->currentAnimState = SWITCH_OFF;
				item->status = ITEM_NOT_ACTIVE;

				RemoveActiveItem(itemNumber);

				LaraItem->animNumber = LA_STAND_SOLID;
				LaraItem->frameNumber = g_Level.Anims[LaraItem->animNumber].frameBase;
				LaraItem->goalAnimState = LS_STOP;
				LaraItem->currentAnimState = LS_STOP;
				Lara.gunStatus = LG_NO_ARMS;
			}
		}
	}
}