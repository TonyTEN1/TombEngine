#include "items.h"
#include "..\Global\global.h"
#include "effect2.h"
#include <stdio.h>

void __cdecl ClearItem(__int16 itemNum)
{
	ITEM_INFO* item = &Items[itemNum];
	item->data = NULL;
	item->collidable = true;
}

void __cdecl KillItem(__int16 itemNum)
{
	if (InItemControlLoop)
	{
		ItemNewRooms[2 * ItemNewRoomNo] = itemNum | 0x8000;
		ItemNewRoomNo++;
	}
	else
	{
		ITEM_INFO* item = &Items[itemNum];

		DetatchSpark(itemNum, 128);

		item->active = false;
		item->reallyActive = false;

		if (NextItemActive == itemNum)
		{
			NextItemActive = item->nextActive;
		}
		else if (NextItemActive != NO_ITEM)
		{
			__int16 linknum;
			for (linknum = Items[NextItemActive].nextActive; linknum != NO_ITEM; linknum = Items[linknum].nextActive)
			{
				if (linknum == itemNum)
				{
					Items[linknum].nextActive = item->nextActive;
					break;
				}
			}
		}

		if (item->roomNumber != 255)
		{
			if (Rooms[item->roomNumber].itemNumber == itemNum)
			{
				Rooms[item->roomNumber].itemNumber = item->nextItem;
			}
			else if (Rooms[item->roomNumber].itemNumber != -1)
			{
				__int16 linknum;
				for (linknum = Items[Rooms[item->roomNumber].itemNumber].nextItem; linknum != NO_ITEM; linknum = Items[linknum].nextItem)
				{
					if (linknum == itemNum)
					{
						Items[Rooms[item->roomNumber].itemNumber].nextItem = item->nextItem;
						break;
					}
				}
			}
		}

		if (item == Lara.target)
			Lara.target = NULL;

		if (itemNum >= LevelItems)
		{
			item->nextItem = NextItemFree;
			NextItemFree = itemNum;
		}
		else
		{
			item->flags |= IFLAG_KILLED;
		}
	}
}

void __cdecl RemoveAllItemsInRoom(__int16 roomNumber, __int16 objectNumber)
{
	ROOM_INFO* room = &Rooms[roomNumber];
	__int16 currentItemNum = room->itemNumber;

	while (currentItemNum != NO_ITEM)
	{
		ITEM_INFO* item = &Items[currentItemNum];

		if (item->objectNumber == objectNumber)
		{
			RemoveActiveItem(currentItemNum);
			item->status = ITEM_INACTIVE;
			item->flags &= 0xC1;
		}

		currentItemNum = item->nextItem;
	}
}

void __cdecl AddActiveItem(__int16 itemNumber)
{
	ITEM_INFO* item = &Items[itemNumber];

	item->flags |= 0x20;

	if (Objects[item->objectNumber].control == NULL)
	{
		item->status = ITEM_INACTIVE;
		return;
	}

	if (!item->active)
	{
		item->active = TRUE;
		item->nextActive = NextItemActive;
		NextItemActive = itemNumber;
	}
}

void __cdecl ItemNewRoom(__int16 itemNumber, __int16 roomNumber)
{
	if (InItemControlLoop)
	{
		ItemNewRooms[ItemNewRoomNo] = itemNumber;
		ItemNewRooms[ItemNewRoomNo + 1] = roomNumber;
		ItemNewRoomNo++;
	}
	else
	{
		ITEM_INFO* item = &Items[itemNumber];

		if (item->roomNumber != 255)
		{
			ROOM_INFO* r = &Rooms[item->roomNumber];

			if (r->itemNumber == itemNumber)
			{
				r->itemNumber = item->nextItem;
			}
			else if (r->itemNumber != NO_ITEM)
			{
				for (__int16 linknum = Items[r->itemNumber].nextItem; linknum != -1; linknum = Items[linknum].nextItem)
				{
					if (linknum == itemNumber)
					{
						Items[r->itemNumber].nextItem = item->nextItem;
						break;
					}
				}
			}
		}

		item->roomNumber = roomNumber;
		item->nextItem = Rooms[roomNumber].itemNumber;
		Rooms[roomNumber].itemNumber = itemNumber;
	}
}

void __cdecl EffectNewRoom(__int16 fxNumber, __int16 roomNumber)
{
	if (InItemControlLoop)
	{
		ItemNewRooms[ItemNewRoomNo] = fxNumber;
		ItemNewRooms[ItemNewRoomNo + 1] = roomNumber;
		ItemNewRoomNo++;
	}
	else
	{
		FX_INFO* fx = &Effects[fxNumber];
		ROOM_INFO* r = &Rooms[fx->roomNumber];

		if (r->fxNumber == fxNumber)
		{
			r->fxNumber = fx->nextFx;
		}
		else if (r->fxNumber != NO_ITEM)
		{
			__int16 linknum;
			for (linknum = Effects[r->fxNumber].nextFx; linknum != -1; linknum = Effects[linknum].nextFx)
			{
				if (linknum == fxNumber)
				{
					Effects[r->fxNumber].nextFx = fx->nextFx;
					break;
				}
			}
		}

		fx->roomNumber = roomNumber;
		fx->nextFx = Rooms[roomNumber].fxNumber;
		Rooms[roomNumber].fxNumber = fxNumber;
	}
}

void __cdecl KillEffect(__int16 fxNumber)
{
	if (InItemControlLoop)
	{
		ItemNewRooms[2 * ItemNewRoomNo] = fxNumber | 0x8000;
		ItemNewRoomNo++;
	}
	else
	{
		FX_INFO* fx = &Effects[fxNumber];

		DetatchSpark(fxNumber, 128);

		if (NextFxActive == fxNumber)
		{
			NextFxActive = fx->nextActive;
		}
		else if (NextFxActive != NO_ITEM)
		{
			for (__int16 linknum = Effects[NextFxActive].nextActive; linknum != NO_ITEM; linknum = Effects[linknum].nextActive)
			{
				if (linknum == fxNumber)
				{
					Effects[linknum].nextActive = fx->nextActive;
					break;
				}
			}
		}

		if (Rooms[fx->roomNumber].fxNumber == fxNumber)
		{
			Rooms[fx->roomNumber].fxNumber = fx->nextFx;
		}
		else if (Rooms[fx->roomNumber].fxNumber != NO_ITEM)
		{
			for (__int16 linknum = Effects[Rooms[fx->roomNumber].fxNumber].nextFx; linknum != NO_ITEM; linknum = Effects[linknum].nextFx)
			{
				if (linknum == fxNumber)
				{
					Effects[Rooms[fx->roomNumber].fxNumber].nextFx = fx->nextFx;
					break;
				}
			}
		}

		fx->nextFx = NextFxFree;
		NextFxFree = fxNumber;
	}
}

__int16 __cdecl CreateNewEffect(__int16 roomNum) 
{
	__int16 fxNumber = NextFxFree;

	if (NextFxFree != NO_ITEM)
	{
		FX_INFO* fx = &Effects[NextFxFree];
		NextFxFree = fx->objectNumber;
		ROOM_INFO* r = &Rooms[roomNum];
		fx->roomNumber = roomNum;
		fx->nextFx = r->fxNumber;
		r->fxNumber = fxNumber;
		fx->nextActive = NextFxActive;
		NextFxActive = fxNumber;
		fx->shade = GRAY555;
	}

	return fxNumber;
}

void __cdecl InitialiseFXArray(__int32 allocmem)
{
	if (allocmem)
		Effects = (FX_INFO*)GameMalloc(24 * sizeof(FX_INFO));

	FX_INFO* fx = Effects;
	NextFxActive = NO_ITEM;
	NextFxFree = 0;
	for (__int32 i = 1; i <= 24; i++)
	{
		fx->nextFx = i++;
		++fx;
	}
	fx->nextFx = -1;
}

void __cdecl RemoveDrawnItem(__int16 itemNum) 
{
	ITEM_INFO* item = &Items[itemNum];

	if (Rooms[item->roomNumber].itemNumber == itemNum)
	{
		Rooms[item->roomNumber].itemNumber = item->nextItem;
	}
	else if (Rooms[item->roomNumber].itemNumber != NO_ITEM)
	{
		for (__int16 linknum = Items[Rooms[item->roomNumber].itemNumber].nextItem; linknum != NO_ITEM; linknum = Items[linknum].nextItem)
		{
			if (linknum == itemNum)
			{
				Items[linknum].nextItem = Items[itemNum].nextItem;
				break;
			}
		}
	}
}

void __cdecl RemoveActiveItem(__int16 itemNum) 
{
	if (Items[itemNum].active)
	{
		Items[itemNum].active = false;

		if (NextItemActive == itemNum)
		{
			NextItemActive = Items[itemNum].nextActive;
		}
		else if (NextItemActive != NO_ITEM)
		{
			for (__int16 linknum = Items[NextItemActive].nextActive; linknum != NO_ITEM; linknum = Items[linknum].nextActive)
			{
				if (linknum == itemNum)
				{
					Items[linknum].nextActive = Items[itemNum].nextActive;
					break;
				}
			}
		}
	}
}

void __cdecl InitialiseItem(__int16 itemNum) 
{
	ITEM_INFO* item = &Items[itemNum];

	item->animNumber = Objects[item->objectNumber].animIndex;
	item->frameNumber = Anims[item->animNumber].frameBase;

	item->requiredAnimState = 0;
	item->goalAnimState = Anims[item->animNumber].currentAnimState;
	item->currentAnimState = Anims[item->animNumber].currentAnimState;

	item->pos.zRot = 0;
	item->pos.xRot = 0;

	item->fallspeed = 0;
	item->speed = 0;

	item->itemFlags[3] = 0;
	item->itemFlags[2] = 0;
	item->itemFlags[1] = 0;
	item->itemFlags[0] = 0;

	item->active = false;
	item->status = ITEM_INACTIVE;
	item->gravityStatus = false;
	item->hitStatus = false;
	item->collidable = true;
	item->lookedAt = false;
	item->dynamicLight = false;
	item->poisoned = false;
	item->aiBits = 0;
	item->reallyActive = false;

	item->timer = 0;

	item->hitPoints = Objects[item->objectNumber].hitPoints;

	if (item->objectNumber == ID_HK_ITEM ||
		item->objectNumber == ID_HK_AMMO_ITEM ||
		item->objectNumber == ID_CROSSBOW_ITEM ||
		item->objectNumber == ID_REVOLVER_ITEM)
	{
		item->meshBits = 1;
	}
	else
	{
		item->meshBits = 0xFFFFFFFF;
	}

	item->touchBits = 0;
	item->afterDeath = 0;
	item->firedWeapon = 0;
	item->data = NULL;

	if (item->flags & IFLAG_INVISIBLE)
	{
		item->flags &= ~IFLAG_INVISIBLE;
		item->status = ITEM_INVISIBLE;
	}
	else if (Objects[item->objectNumber].intelligent)
	{
		item->status = ITEM_INVISIBLE;
	}

	if ((item->flags & IFLAG_ACTIVATION_MASK) == IFLAG_ACTIVATION_MASK)
	{
		item->flags &= ~IFLAG_ACTIVATION_MASK;
		item->flags |= IFLAG_REVERSE;
		AddActiveItem(itemNum);
		item->status = ITEM_ACTIVE;
	}

	ROOM_INFO* r = &Rooms[item->roomNumber];

	item->nextItem = r->itemNumber;
	r->itemNumber = itemNum;

	FLOOR_INFO* floor = &XZ_GET_SECTOR(r, item->pos.xPos - r->x, item->pos.zPos - r->z);
	item->floor = floor->floor * 256;
	item->boxNumber = floor->box;

	if (Objects[item->objectNumber].initialise != NULL)
	{
		Objects[item->objectNumber].initialise(itemNum);
	}

	//item->unk_thing = 0;
}

__int16 __cdecl CreateItem()
{
	__int16 itemNum = 0;

	if (NextItemFree == -1) return NO_ITEM;

	itemNum = NextItemFree;
	Items[NextItemFree].flags = 0;
	NextItemFree = Items[NextItemFree].nextItem;

	return itemNum;
}

void __cdecl InitialiseItemArray(__int32 numitems) 
{
	ITEM_INFO* item = &Items[LevelItems];

	NextItemActive = NO_ITEM;
	NextItemFree = LevelItems;

	if (LevelItems + 1 < numitems)
	{
		for (__int32 i = LevelItems + 1; i < numitems; i++, item++)
		{
			item->nextItem = i;
			item->active = false;
		}
	}

	item->nextItem = NO_ITEM;
}

__int16 __cdecl SpawnItem(ITEM_INFO* item, __int16 objectNumber)
{
	__int16 itemNumber = CreateItem();
	if (itemNumber != NO_ITEM)
	{
		ITEM_INFO* spawn = &Items[itemNumber];

		spawn->objectNumber = objectNumber;
		spawn->roomNumber = item->roomNumber;
		memcpy(&spawn->pos, &item->pos, sizeof(PHD_3DPOS));

		InitialiseItem(itemNumber);

		spawn->status = ITEM_INACTIVE;
		spawn->shade = 0x4210;
	}

	return itemNumber;
}

__int32 __cdecl GlobalItemReplace(__int16 search, __int16 replace)
{
	__int32 changed = 0;
	for (__int32 i = 0; i < NumberRooms; i++)
	{
		ROOM_INFO* room = &Rooms[i];
		for (__int16 itemNumber = room->itemNumber; itemNumber != NO_ITEM; itemNumber = Items[itemNumber].nextItem)
		{
			if (Items[itemNumber].objectNumber == search)
			{
				Items[itemNumber].objectNumber = replace;
				changed++;
			}
		}
	}

	return changed;
}

void Inject_Items()
{
	/*INJECT(0x00440840, CreateItem);
	INJECT(0x00440D10, AddActiveItem);
	INJECT(0x00440620, KillItem);
	INJECT(0x00440DA0, ItemNewRoom);
	INJECT(0x004412F0, EffectNewRoom);
	INJECT(0x00441180, KillEffect);
	INJECT(0x00441080, InitialiseFXArray);
	INJECT(0x004410F0, CreateNewEffect);
	INJECT(0x00440B60, RemoveActiveItem);
	INJECT(0x00440C40, RemoveDrawnItem);
	INJECT(0x004408B0, InitialiseItem);
	INJECT(0x00440840, ClearItem);
	INJECT(0x00440590, InitialiseItemArray);*/
}
