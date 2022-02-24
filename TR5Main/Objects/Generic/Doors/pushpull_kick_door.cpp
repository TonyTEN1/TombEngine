#include "framework.h"
#include "Objects/Generic/Doors/generic_doors.h"
#include "Specific/level.h"
#include "Game/control/control.h"
#include "Game/control/box.h"
#include "Game/items.h"
#include "Game/control/lot.h"
#include "Game/gui.h"
#include "Specific/input.h"
#include "Game/pickup/pickup.h"
#include "Sound/sound.h"
#include "Game/animation.h"
#include "Game/collision/sphere.h"
#include "Game/Lara/lara_helpers.h"
#include "Game/Lara/lara_struct.h"
#include "Game/Lara/lara.h"
#include "Specific/trmath.h"
#include "Game/misc.h"
#include "Objects/Generic/Doors/pushpull_kick_door.h"
#include "Game/collision/collide_item.h"
#include "Game/itemdata/door_data.h"

namespace TEN::Entities::Doors
{
	enum STATES_PUSHPULL_KICK_DOOR
	{
		STATE_PUSHPULL_KICK_DOOR_OPEN = 0,
		STATE_PUSHPULL_KICK_DOOR_CLOSED = 1,
		STATE_PUSHPULL_KICK_DOOR_PUSH = 2,
		STATE_PUSHPULL_KICK_DOOR_PULL = 3
	};

	PHD_VECTOR PullDoorPos(-201, 0, 322);
	PHD_VECTOR PushDoorPos(201, 0, -702);
	PHD_VECTOR KickDoorPos(0, 0, -917);

	OBJECT_COLLISION_BOUNDS PushPullKickDoorBounds =
	{
		-384, 384,
		0, 0,
		-1024, 512,
		-ANGLE(10.0f), ANGLE(10.0f),
		-ANGLE(30.0f), ANGLE(30.0f),
		-ANGLE(10.0f), ANGLE(10.0f),
	};

	void PushPullKickDoorCollision(short itemNumber, ITEM_INFO* laraItem, COLL_INFO* coll)
	{
		auto* laraInfo = GetLaraInfo(laraItem);
		auto* doorItem = &g_Level.Items[itemNumber];

		if (TrInput & IN_ACTION &&
			laraItem->ActiveState == LS_IDLE &&
			laraItem->AnimNumber == LA_STAND_IDLE &&
			!laraItem->HitStatus &&
			doorItem->Status != ITEM_ACTIVE &&
			laraInfo->Control.HandStatus == HandStatus::Free ||
			laraInfo->Control.IsMoving && laraInfo->interactedItem == itemNumber)
		{
			bool pull = false;

			if (laraItem->RoomNumber == doorItem->RoomNumber)
			{
				doorItem->Position.yRot ^= ANGLE(180.0f);
				pull = true;
			}

			if (TestLaraPosition(&PushPullKickDoorBounds, doorItem, laraItem))
			{
				bool openTheDoor = false;

				if (pull)
				{
					if (MoveLaraPosition(&PullDoorPos, doorItem, laraItem))
					{
						SetAnimation(laraItem, LA_DOOR_OPEN_PULL);
						doorItem->TargetState = STATE_PUSHPULL_KICK_DOOR_PULL;
						openTheDoor = true;
					}
					else
						laraInfo->interactedItem = itemNumber;
				}
				else
				{
					if (doorItem->ObjectNumber >= ID_KICK_DOOR1)
					{
						if (MoveLaraPosition(&KickDoorPos, doorItem, laraItem))
						{
							SetAnimation(laraItem, LA_DOOR_OPEN_KICK);
							doorItem->TargetState = STATE_PUSHPULL_KICK_DOOR_PUSH;
							openTheDoor = true;
						}
						else
							laraInfo->interactedItem = itemNumber;
					}
					else
					{
						if (MoveLaraPosition(&PushDoorPos, doorItem, laraItem))
						{
							SetAnimation(laraItem, LA_DOOR_OPEN_PUSH);
							doorItem->TargetState = STATE_PUSHPULL_KICK_DOOR_PUSH;
							openTheDoor = true;
						}
						else
							laraInfo->interactedItem = itemNumber;
					}
				}

				if (openTheDoor)
				{
					AddActiveItem(itemNumber);

					laraItem->ActiveState = LS_MISC_CONTROL;
					laraItem->TargetState = LS_IDLE;
					laraInfo->Control.IsMoving = false;
					laraInfo->Control.HandStatus = HandStatus::Busy;
					doorItem->Status = ITEM_ACTIVE;
				}
			}
			else if (laraInfo->Control.IsMoving &&
				laraInfo->interactedItem == itemNumber)
			{
				laraInfo->Control.IsMoving = false;
				laraInfo->Control.HandStatus = HandStatus::Free;
			}

			if (pull)
				doorItem->Position.yRot ^= ANGLE(180.0f);
		}
		else if (doorItem->ActiveState <= STATE_PUSHPULL_KICK_DOOR_CLOSED)
			DoorCollision(itemNumber, laraItem, coll);
	}

	void PushPullKickDoorControl(short itemNumber)
	{
		auto* doorItem = &g_Level.Items[itemNumber];
		auto* doorData = (DOOR_DATA*)doorItem->Data;

		if (!doorData->opened)
		{
			OpenThatDoor(&doorData->d1, doorData);
			OpenThatDoor(&doorData->d2, doorData);
			OpenThatDoor(&doorData->d1flip, doorData);
			OpenThatDoor(&doorData->d2flip, doorData);
			doorData->opened = true;
		}

		AnimateItem(doorItem);
	}
}