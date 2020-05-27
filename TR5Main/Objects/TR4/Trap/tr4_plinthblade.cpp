#include "framework.h"
#include "tr4_plinthblade.h"
#include "level.h"
#include "control.h"

void PlinthBladeControl(short itemNum)
{
	ITEM_INFO* item = &Items[itemNum];

	if (!TriggerActive(item))
	{
		item->frameNumber = Anims[item->animNumber].frameBase;
	}
	else
	{
		short frameNumber = item->frameNumber - Anims[item->animNumber].frameBase;

		if (item->frameNumber == Anims[item->animNumber].frameEnd)
			item->itemFlags[3] = 0;
		else
			item->itemFlags[3] = 200;

		AnimateItem(item);
	}
}