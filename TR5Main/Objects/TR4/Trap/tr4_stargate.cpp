#include "framework.h"
#include "tr4_stargate.h"
#include "level.h"
#include "control/control.h"
#include "Sound/sound.h"
#include "collide.h"
#include "sphere.h"
#include "lara.h"
#include "effects/effects.h"
#include "animation.h"
#include "items.h"

namespace TEN::Entities::TR4
{
	short StargateBounds[24] =
	{
		-512, 512, -1024, 
		-896, -96, 96, 
		-512, 512, -128, 
		0, -96, 96, 
		-512, -384, -1024, 
		0, -96, 96, 
		384, 512, -1024, 
		0, -96, 96
	};

	void StargateControl(short itemNum)
	{
		ITEM_INFO* item = &g_Level.Items[itemNum];
		item->itemFlags[3] = 50;

		if (TriggerActive(item))
		{
			SoundEffect(SFX_TR4_STARGATE_SWIRL, &item->pos, 0);
			item->itemFlags[0] = 0x36DB600;
			AnimateItem(item);
		}
		else
		{
			item->itemFlags[0] = 0;
		}
	}

	void StargateCollision(short itemNum, ITEM_INFO* l, COLL_INFO* c)
	{
		ITEM_INFO* item = &g_Level.Items[itemNum];

		if (item->status == ITEM_INVISIBLE)
			return;

		if (TestBoundsCollide(item, l, c->Setup.Radius))
		{
			for (int i = 0; i < 8; i++)
			{
				GlobalCollisionBounds.X1 = StargateBounds[3 * i + 0];
				GlobalCollisionBounds.Y1 = StargateBounds[3 * i + 1];
				GlobalCollisionBounds.Z1 = StargateBounds[3 * i + 2];

				if (TestWithGlobalCollisionBounds(item, l, c))
					ItemPushItem(item, l, c, 0, 2);
			}

			int result = TestCollision(item, l);
			if (result)
			{
				result &= item->itemFlags[0];
				int flags = item->itemFlags[0];

				if (result)
				{
					int j = 0;
					do
					{
						if (result & 1)
						{
							GlobalCollisionBounds.X1 = CreatureSpheres[j].x - CreatureSpheres[j].r - item->pos.xPos;
							GlobalCollisionBounds.Y1 = CreatureSpheres[j].y - CreatureSpheres[j].r - item->pos.yPos;
							GlobalCollisionBounds.Z1 = CreatureSpheres[j].z - CreatureSpheres[j].r - item->pos.zPos;
							GlobalCollisionBounds.X2 = CreatureSpheres[j].x + CreatureSpheres[j].r - item->pos.xPos;
							GlobalCollisionBounds.Y2 = CreatureSpheres[j].y + CreatureSpheres[j].r - item->pos.yPos;
							GlobalCollisionBounds.Z2 = CreatureSpheres[j].z + CreatureSpheres[j].r - item->pos.zPos;

							int oldX = LaraItem->pos.xPos;
							int oldY = LaraItem->pos.yPos;
							int oldZ = LaraItem->pos.zPos;

							if (ItemPushItem(item, l, c, flags & 1, 2))
							{
								if ((flags & 1) &&
									(oldX != LaraItem->pos.xPos 
									|| oldY != LaraItem->pos.yPos 
									|| oldZ != LaraItem->pos.zPos) &&
									TriggerActive(item))
								{
									DoBloodSplat((GetRandomControl() & 0x3F) + l->pos.xPos - 32,
										(GetRandomControl() & 0x1F) + CreatureSpheres[j].y - 16,
										(GetRandomControl() & 0x3F) + l->pos.zPos - 32,
										(GetRandomControl() & 3) + 2,
										2 * GetRandomControl(),
										l->roomNumber);
									LaraItem->hitPoints -= 100;
								}
							}
						}

						result /= 2;
						j++;
						flags /= 2;

					} while (result);
				}
			}
		}
	}
}