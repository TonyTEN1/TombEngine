#pragma once
#include "Objects/Utils/VehicleHelpers.h"

struct CollisionInfo;
struct ItemInfo;

namespace TEN::Entities::Vehicles
{
	void InitialiseSpeedboat(short itemNumber);

	void SpeedboatPlayerCollision(short itemNumber, ItemInfo* laraItem, CollisionInfo* coll);
	void SpeedboatControl(short itemNumber);
	bool SpeedboatUserControl(ItemInfo* speedboatItem, ItemInfo* laraItem);
	void SpeedboatAnimation(ItemInfo* speedboatItem, ItemInfo* laraItem, VehicleImpactDirection impactDirection);

	void DoSpeedboatMount(ItemInfo* speedboatItem, ItemInfo* laraItem, VehicleMountType mountType);
	bool TestSpeedboatDismount(ItemInfo* speedboatItem, int direction);
	void DoSpeedboatDismount(ItemInfo* speedboatItem, ItemInfo* laraItem);
	void DoSpeedboatImpact(ItemInfo* speedboatItem, ItemInfo* laraItem, VehicleImpactDirection impactDirection);

	VehicleImpactDirection SpeedboatDynamics(short itemNumber, ItemInfo* laraItem);
	void SpeedboatDoBoatShift(ItemInfo* speedboatItem, int itemNumber);
	void SpeedboatSplash(ItemInfo* speedboatItem, long verticalVelocity, long water);
}
