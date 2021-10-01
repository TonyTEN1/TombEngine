#include "framework.h"
#include "items.h"
#include "setup.h"
#include "level.h"
#include "lara.h"
#include "control/control.h"
#include "floordata.h"
#include "effects/effects.h"

using namespace TEN::Floordata;

void ClearItem(short itemNum)
{
	ITEM_INFO* item = &g_Level.Items[itemNum];
	ROOM_INFO* room = &g_Level.Rooms[item->roomNumber];

	item->collidable = true;
	item->data = nullptr;
	item->startPos = item->pos;
}

void KillItem(short itemNum)
{
	if (InItemControlLoop)
	{
		ItemNewRooms[2 * ItemNewRoomNo] = itemNum | 0x8000;
		ItemNewRoomNo++;
	}
	else// if (NextItemActive != NO_ITEM)
	{
		ITEM_INFO* item = &g_Level.Items[itemNum];

		DetatchSpark(itemNum, SP_ITEM);

		item->active = false;
		item->reallyActive = false;

		if (NextItemActive == itemNum)
		{
			NextItemActive = item->nextActive;
		}
		else
		{
			short linknum;
			for (linknum = NextItemActive; linknum != NO_ITEM; linknum = g_Level.Items[linknum].nextActive)
			{
				if (g_Level.Items[linknum].nextActive == itemNum)
				{
					g_Level.Items[linknum].nextActive = item->nextActive;
					break;
				}
			}
		}

		if (item->roomNumber != NO_ROOM)
		{
			if (g_Level.Rooms[item->roomNumber].itemNumber == itemNum)
			{
				g_Level.Rooms[item->roomNumber].itemNumber = item->nextItem;
			}
			else
			{
				short linknum;
				for (linknum = g_Level.Rooms[item->roomNumber].itemNumber; linknum != NO_ITEM; linknum = g_Level.Items[linknum].nextItem)
				{
					if (g_Level.Items[linknum].nextItem == itemNum)
					{
						g_Level.Items[linknum].nextItem = item->nextItem;
						break;
					}
				}
			}
		}

		if (item == Lara.target)
			Lara.target = NULL;

		if (itemNum >= g_Level.NumItems)
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

void RemoveAllItemsInRoom(short roomNumber, short objectNumber)
{
	ROOM_INFO* room = &g_Level.Rooms[roomNumber];
	short currentItemNum = room->itemNumber;

	while (currentItemNum != NO_ITEM)
	{
		ITEM_INFO* item = &g_Level.Items[currentItemNum];

		if (item->objectNumber == objectNumber)
		{
			RemoveActiveItem(currentItemNum);
			item->status = ITEM_NOT_ACTIVE;
			item->flags &= 0xC1;
		}

		currentItemNum = item->nextItem;
	}
}

void AddActiveItem(short itemNumber)
{
	ITEM_INFO* item = &g_Level.Items[itemNumber];

	item->flags |= 0x20;

	if (Objects[item->objectNumber].control == NULL)
	{
		item->status = ITEM_NOT_ACTIVE;
		return;
	}

	if (!item->active)
	{
		item->active = true;
		item->nextActive = NextItemActive;
		NextItemActive = itemNumber;
	}
}

void ItemNewRoom(short itemNumber, short roomNumber)
{
	if (InItemControlLoop)
	{
		ItemNewRooms[2 * ItemNewRoomNo] = itemNumber;
		ItemNewRooms[2 * ItemNewRoomNo + 1] = roomNumber;
		ItemNewRoomNo++;
	}
	else
	{
		ITEM_INFO* item = &g_Level.Items[itemNumber];

		if (item->roomNumber != NO_ROOM)
		{
			ROOM_INFO* r = &g_Level.Rooms[item->roomNumber];

			if (r->itemNumber == itemNumber)
			{
				r->itemNumber = item->nextItem;
			}
			else
			{
				for (short linknum = r->itemNumber; linknum != -1; linknum = g_Level.Items[linknum].nextItem)
				{
					if (g_Level.Items[linknum].nextItem == itemNumber)
					{
						g_Level.Items[linknum].nextItem = item->nextItem;
						break;
					}
				}
			}
		}

		item->roomNumber = roomNumber;
		item->nextItem = g_Level.Rooms[roomNumber].itemNumber;
		g_Level.Rooms[roomNumber].itemNumber = itemNumber;
	}
}

void EffectNewRoom(short fxNumber, short roomNumber)
{
	ItemNewRoom(fxNumber, roomNumber);
}

void KillEffect(short fxNumber)
{
	KillItem(fxNumber);
}

short CreateNewEffect(short roomNum, GAME_OBJECT_ID objectNum, PHD_3DPOS pos) 
{
	auto fxNumber = CreateItem();

	if (fxNumber != NO_ITEM)
	{
		auto& fx = g_Level.Items[fxNumber];
		fx.data = FX_INFO();
		fx.roomNumber = roomNum;
		fx.pos = pos;
		fx.objectNumber = objectNum;
		InitialiseItem(fxNumber);
		AddActiveItem(fxNumber);
	}
	return fxNumber;
}

void RemoveDrawnItem(short itemNum) 
{
	ITEM_INFO* item = &g_Level.Items[itemNum];

	if (g_Level.Rooms[item->roomNumber].itemNumber == itemNum)
	{
		g_Level.Rooms[item->roomNumber].itemNumber = item->nextItem;
	}
	else
	{
		for (short linknum = g_Level.Rooms[item->roomNumber].itemNumber; linknum != NO_ITEM; linknum = g_Level.Items[linknum].nextItem)
		{
			if (g_Level.Items[linknum].nextItem == itemNum)
			{
				g_Level.Items[linknum].nextItem = item->nextItem;
				break;
			}
		}
	}
}

void RemoveActiveItem(short itemNum) 
{
	if (g_Level.Items[itemNum].active)
	{
		g_Level.Items[itemNum].active = false;

		if (NextItemActive == itemNum)
		{
			NextItemActive = g_Level.Items[itemNum].nextActive;
		}
		else
		{
			for (short linknum = NextItemActive; linknum != NO_ITEM; linknum = g_Level.Items[linknum].nextActive)
			{
				if (g_Level.Items[linknum].nextActive == itemNum)
				{
					g_Level.Items[linknum].nextActive = g_Level.Items[itemNum].nextActive;
					break;
				}
			}
		}
	}
}

void InitialiseItem(short itemNum) 
{
	ITEM_INFO* item = &g_Level.Items[itemNum];

	item->animNumber = Objects[item->objectNumber].animIndex;
	item->frameNumber = g_Level.Anims[item->animNumber].frameBase;

	item->requiredAnimState = 0;
	item->goalAnimState = g_Level.Anims[item->animNumber].currentAnimState;
	item->currentAnimState = g_Level.Anims[item->animNumber].currentAnimState;

	item->pos.zRot = 0;
	item->pos.xRot = 0;

	item->fallspeed = 0;
	item->speed = 0;

	item->itemFlags[3] = 0;
	item->itemFlags[2] = 0;
	item->itemFlags[1] = 0;
	item->itemFlags[0] = 0;

	item->active = false;
	item->status = ITEM_NOT_ACTIVE;
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
		item->meshBits = -1;
	}

	item->touchBits = 0;
	item->afterDeath = false;
	item->firedWeapon = 0;
	item->swapMeshFlags = 0;

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

	ROOM_INFO* r = &g_Level.Rooms[item->roomNumber];

	item->nextItem = r->itemNumber;
	r->itemNumber = itemNum;

	FLOOR_INFO* floor = GetSector(r, item->pos.xPos - r->x, item->pos.zPos - r->z);
	item->floor = floor->FloorHeight(item->pos.xPos, item->pos.zPos);
	item->boxNumber = floor->Box;

	item->mutator.resize(Objects[item->objectNumber].nmeshes);
	for (int i = 0; i < item->mutator.size(); i++)
		item->mutator[i] = {};

	if (Objects[item->objectNumber].initialise != NULL)
		Objects[item->objectNumber].initialise(itemNum);
}

short CreateItem()
{
	short itemNum = 0;

	if (NextItemFree == -1) return NO_ITEM;

	itemNum = NextItemFree;
	g_Level.Items[NextItemFree].flags = 0;
	g_Level.Items[NextItemFree].luaName = "";
	NextItemFree = g_Level.Items[NextItemFree].nextItem;

	return itemNum;
}

void InitialiseItemArray(int numitems)
{
	ITEM_INFO* item = &g_Level.Items[g_Level.NumItems];

	NextItemActive = NO_ITEM;
	NextItemFree = g_Level.NumItems;

	if (g_Level.NumItems + 1 < numitems)
	{
		for (int i = g_Level.NumItems + 1; i < numitems; i++, item++)
		{
			item->nextItem = i;
			item->active = false;
		}
	}

	item->nextItem = NO_ITEM;
}

short SpawnItem(ITEM_INFO* item, GAME_OBJECT_ID objectNumber)
{
	short itemNumber = CreateItem();
	if (itemNumber != NO_ITEM)
	{
		ITEM_INFO* spawn = &g_Level.Items[itemNumber];

		spawn->objectNumber = objectNumber;
		spawn->roomNumber = item->roomNumber;
		memcpy(&spawn->pos, &item->pos, sizeof(PHD_3DPOS));

		InitialiseItem(itemNumber);

		spawn->status = ITEM_NOT_ACTIVE;
		spawn->shade = 0x4210;
	}

	return itemNumber;
}

int GlobalItemReplace(short search, GAME_OBJECT_ID replace)
{
	int changed = 0;
	for (int i = 0; i < g_Level.Rooms.size(); i++)
	{
		ROOM_INFO* room = &g_Level.Rooms[i];
		for (short itemNumber = room->itemNumber; itemNumber != NO_ITEM; itemNumber = g_Level.Items[itemNumber].nextItem)
		{
			if (g_Level.Items[itemNumber].objectNumber == search)
			{
				g_Level.Items[itemNumber].objectNumber = replace;
				changed++;
			}
		}
	}

	return changed;
}

void UpdateItemRoom(ITEM_INFO* item, int height)
{
	int x = item->pos.xPos;
	int y = height + item->pos.yPos;
	int z = item->pos.zPos;
	item->location = GetRoom(item->location, x, y, z);
	item->floor = GetFloorHeight(item->location, x, z).value_or(NO_HEIGHT);

	if (item->roomNumber != item->location.roomNumber)
		ItemNewRoom(FindItem(item), item->location.roomNumber);
}

std::vector<int> FindAllItems(short objectNumber)
{
	std::vector<int> itemList;

	for (int i = 0; i < g_Level.NumItems; i++)
	{
		if (g_Level.Items[i].objectNumber == objectNumber)
			itemList.push_back(i);
	}

	return itemList;
}

ITEM_INFO* FindItem(int object_number)
{
	ITEM_INFO* item;

	for (int i = 0; i < g_Level.NumItems; i++)
	{
		item = &g_Level.Items[i];

		if (item->objectNumber == object_number)
			return item;
	}

	return 0;
}

int FindItem(ITEM_INFO* item)
{
	if (item == LaraItem)
		return Lara.itemNumber;

	for (int i = 0; i < g_Level.NumItems; i++)
		if (item == &g_Level.Items[i])
			return i;

	return -1;
}
