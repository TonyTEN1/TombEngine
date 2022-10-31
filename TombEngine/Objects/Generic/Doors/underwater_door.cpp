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
#include "Math/Math.h"
#include "Game/misc.h"
#include "Objects/Generic/Doors/underwater_door.h"
#include "Game/collision/collide_item.h"

using namespace TEN::Input;

namespace TEN::Entities::Doors
{
	const auto UnderwaterDoorPos = Vector3i(-251, -540, -46);
	const InteractionBounds UnderwaterDoorBounds =
	{
		GameBoundingBox(
			-CLICK(1), CLICK(1),
			-SECTOR(1), 0,
			-SECTOR(1), 0
		),
		std::pair(
			EulerAngles(ANGLE(-80.0f), ANGLE(-80.0f), ANGLE(-80.0f)),
			EulerAngles(ANGLE(80.0f), ANGLE(80.0f), ANGLE(80.0f))
		)
	};

	void UnderwaterDoorCollision(short itemNumber, ItemInfo* laraItem, CollisionInfo* coll)
	{
		auto* laraInfo = GetLaraInfo(laraItem);
		auto* doorItem = &g_Level.Items[itemNumber];

		if (TrInput & IN_ACTION &&
			laraItem->Animation.ActiveState == LS_UNDERWATER_IDLE &&
			laraInfo->Control.WaterStatus == WaterStatus::Underwater &&
			doorItem->Status != ITEM_ACTIVE &&
			!doorItem->Animation.IsAirborne &&
			laraInfo->Control.HandStatus == HandStatus::Free ||
			laraInfo->Control.IsMoving && laraInfo->InteractedItem == itemNumber)
		{
			laraItem->Pose.Orientation.y ^= ANGLE(180.0f);

			if (TestPlayerEntityInteract(doorItem, laraItem, UnderwaterDoorBounds))
			{
				if (AlignPlayerToEntity(doorItem, laraItem, UnderwaterDoorPos))
				{
					SetAnimation(laraItem, LA_UNDERWATER_DOOR_OPEN);
					laraItem->Animation.Velocity.y = 0;
					doorItem->Status = ITEM_ACTIVE;

					AddActiveItem(itemNumber);

					doorItem->Animation.TargetState = LS_RUN_FORWARD;

					AnimateItem(doorItem);

					laraInfo->Control.IsMoving = false;
					laraInfo->Control.HandStatus = HandStatus::Busy;
				}
				else
					laraInfo->InteractedItem = itemNumber;

				laraItem->Pose.Orientation.y ^= ANGLE(180.0f);
			}
			else
			{
				if (laraInfo->Control.IsMoving && laraInfo->InteractedItem == itemNumber)
				{
					laraInfo->Control.IsMoving = false;
					laraInfo->Control.HandStatus = HandStatus::Free;
				}

				laraItem->Pose.Orientation.y ^= ANGLE(180.0f);
			}
		}
		else if (doorItem->Status == ITEM_ACTIVE)
			ObjectCollision(itemNumber, laraItem, coll);
	}
}
