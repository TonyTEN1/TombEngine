#pragma once
#include "items.h"
struct COLL_INFO;
struct OBJECT_COLLISION_BOUNDS;

namespace TEN::Entities::Effects
{
	extern std::array<ItemStateFunction, 2> FlameStateFunctions;

	extern OBJECT_COLLISION_BOUNDS FireBounds;

	void InitialiseFlameEmitter(short itemNumber);
	void InitialiseFlameEmitter2(short itemNumber);
	void InitialiseFlameEmitter3(short itemNumber);
	void FlameEmitterControl(short itemNumber);
	void FlameEmitter2Control(short itemNumber);
	void FlameEmitter3Control(short itemNumber);
	void FlameControl(short fxNumber);
	void FlameEmitterCollision(short itemNumber, ITEM_INFO* l, COLL_INFO* coll);
}