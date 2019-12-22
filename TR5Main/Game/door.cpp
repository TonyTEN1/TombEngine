#include "door.h"
#include "items.h"
#include "lot.h"
#include "objects.h"
#include "collide.h"
#include "Lara.h"
#include "inventory.h"
#include "draw.h"
#include "sphere.h"
#include "switch.h"
#include "misc.h"

PHD_VECTOR DoubleDoorPos = { 0, 0, 220 };
PHD_VECTOR PullDoorPos = { -201, 0, 322 };
PHD_VECTOR PushDoorPos = { 201, 0, -702 };
PHD_VECTOR KickDoorPos = { 0, 0, -917 };
PHD_VECTOR UnderwaterDoorPos = { -251, -540, -46 };
PHD_VECTOR CrowbarDoorPos = { -412, 0, 256 };

static short PushPullKickDoorBounds[12] =
{
	0xFE80, 0x0180, 0x0000, 0x0000, 0xFC00, 0x0200, 0xF8E4, 0x071C, 0xEAAC, 0x1554, 0xF8E4, 0x071C
};

static short UnderwaterDoorBounds[12] =
{
	0xFF00, 0x0100, 0xFC00, 0x0000, 0xFC00, 0x0000, 0xC720, 0x38E0, 0xC720, 0x38E0, 0xC720, 0x38E0
};

static short CrowbarDoorBounds[12] =
{
	0xFE00, 0x0200, 0xFC00, 0x0000, 0x0000, 0x0200, 0xC720, 0x38E0, 0xC720, 0x38E0, 0xC720, 0x38E0
};

int ClosedDoors[32];
byte LiftDoor;

extern byte SequenceUsed[6];
extern byte SequenceResults[3][3][3];
extern byte Sequences[3];
extern byte CurrentSequence;
extern PHD_VECTOR OldPickupPos;
extern Inventory* g_Inventory;

void SequenceDoorControl(short itemNumber) 
{
	ITEM_INFO* item = &Items[itemNumber];
	DOOR_DATA* door = (DOOR_DATA*)item->data;

	if (CurrentSequence == 3)
	{
		if (SequenceResults[Sequences[0]][Sequences[1]][Sequences[2]] == item->triggerFlags)
		{
			if (item->currentAnimState == 1)
				item->goalAnimState = 1;
			else
				item->goalAnimState = 0;

			TestTriggersAtXYZ(item->pos.xPos, item->pos.yPos, item->pos.zPos, item->roomNumber, TRUE, 0);
		}

		CurrentSequence = 4;
	}	

	if (item->currentAnimState == item->goalAnimState)
	{
		if (item->currentAnimState == 1)
		{
			if (!door->opened)
			{
				OpenThatDoor(&door->d1, door);
				OpenThatDoor(&door->d2, door);
				OpenThatDoor(&door->d1flip, door);
				OpenThatDoor(&door->d2flip, door);
				door->opened = TRUE;
				item->flags |= 0x3E;
			}
		}
		else
		{
			if (door->opened)
			{
				ShutThatDoor(&door->d1, door);
				ShutThatDoor(&door->d2, door);
				ShutThatDoor(&door->d1flip, door);
				ShutThatDoor(&door->d2flip, door);
				door->opened = FALSE;
				item->flags &= 0xC1;
			}
		}
	}

	AnimateItem(item);
}

void UnderwaterDoorCollision(short itemNum, ITEM_INFO* l, COLL_INFO* coll) 
{
	ITEM_INFO* item = &Items[itemNum];
	
	if (TrInput & IN_ACTION
	&&  l->currentAnimState == STATE_LARA_UNDERWATER_STOP
	&&  !(item->status && item->gravityStatus)
	&&  Lara.waterStatus == LW_UNDERWATER
	&&  !Lara.gunStatus
	||  Lara.isMoving && Lara.generalPtr == (void*)itemNum)
	{
		l->pos.yRot ^= ANGLE(180);
		
		if (TestLaraPosition(UnderwaterDoorBounds, item, l))
		{
			if (MoveLaraPosition(&UnderwaterDoorPos, item, l))
			{
				l->animNumber = ANIMATION_LARA_UNDERWATER_DOOR_OPEN;
				l->frameNumber = GF(ANIMATION_LARA_UNDERWATER_DOOR_OPEN, 0);
				l->currentAnimState = STATE_LARA_MISC_CONTROL;
				l->fallspeed = 0;
				item->status = ITEM_ACTIVE;
				AddActiveItem(itemNum);
				item->goalAnimState = STATE_LARA_RUN_FORWARD;
				AnimateItem(item);
				Lara.isMoving = false;
				Lara.gunStatus = LG_HANDS_BUSY;
			}
			else
			{
				Lara.generalPtr = (void*)itemNum;
			}
			l->pos.yRot ^= ANGLE(180);
		}
		else
		{
			if (Lara.isMoving && Lara.generalPtr == (void*)itemNum)
			{
				Lara.isMoving = false;
				Lara.gunStatus = LG_NO_ARMS;
			}
			l->pos.yRot ^= ANGLE(180);
		}
	}
	else if (item->status == ITEM_ACTIVE)
	{
		ObjectCollision(itemNum, l, coll);
	}
}

void DoubleDoorCollision(short itemNum, ITEM_INFO* l, COLL_INFO* coll) 
{
	ITEM_INFO* item = &Items[itemNum];

	if (TrInput & IN_ACTION
	&&  l->currentAnimState == STATE_LARA_STOP
	&&  l->animNumber == ANIMATION_LARA_STAY_IDLE
	&&  !(item->status && item->gravityStatus)
	&&  !(l->hitStatus)
	&&  !Lara.gunStatus
	||  Lara.isMoving && Lara.generalPtr == (void*)itemNum)
	{
		item->pos.yRot ^= ANGLE(180);
		if (TestLaraPosition(PushPullKickDoorBounds, item, l))
		{
			if (MoveLaraPosition(&DoubleDoorPos, item, l))
			{
				l->animNumber = ANIMATION_LARA_DOUBLEDOORS_PUSH;
				l->frameNumber = GF(ANIMATION_LARA_DOUBLEDOORS_PUSH, 0);
				l->currentAnimState = STATE_LARA_DOUBLEDOORS_PUSH;
				
				AddActiveItem(itemNum);

				item->status = ITEM_ACTIVE;
				Lara.isMoving = false;
				Lara.gunStatus = LG_HANDS_BUSY;
				Lara.headYrot = 0;
				Lara.headXrot = 0;
				Lara.torsoYrot = 0;
				Lara.torsoXrot = 0;
			}
			else
			{
				Lara.generalPtr = (void*)itemNum;
			}
			item->pos.yRot ^= ANGLE(180);
		}
		else
		{
			if (Lara.isMoving && Lara.generalPtr == (void*)itemNum)
			{
				Lara.isMoving = false;
				Lara.gunStatus = LG_NO_ARMS;
			}
			item->pos.yRot ^= ANGLE(180);
		}
	}
}

void PushPullKickDoorCollision(short itemNum, ITEM_INFO* l, COLL_INFO* coll)
{
	ITEM_INFO* item = &Items[itemNum];
	if (TrInput & IN_ACTION
	&&  l->currentAnimState == STATE_LARA_STOP
	&&  l->animNumber == ANIMATION_LARA_STAY_IDLE
	&&  item->status != ITEM_ACTIVE
	&&  !(l->hitStatus)
	&&  !Lara.gunStatus
	||  Lara.isMoving && Lara.generalPtr == (void*)itemNum)
	{
		bool applyRot = false;

		if (l->roomNumber == item->roomNumber)
		{
			item->pos.yRot ^= ANGLE(180);
			applyRot = true;
		}

		if (!TestLaraPosition(PushPullKickDoorBounds, item, l))
		{
			if (Lara.isMoving && Lara.generalPtr == (void*)itemNum)
			{
				Lara.isMoving = false;
				Lara.gunStatus = LG_NO_ARMS;
			}
			if (applyRot)
				item->pos.yRot ^= ANGLE(180);
			return;
		}

		if (applyRot)
		{
			if (!MoveLaraPosition(&PullDoorPos, item, l))
			{
				Lara.generalPtr = (void*)itemNum;
				item->pos.yRot ^= ANGLE(180);
				return;
			}

			l->animNumber = ANIMATION_LARA_DOOR_OPEN_BACK;
			l->frameNumber = GF(ANIMATION_LARA_DOOR_OPEN_BACK, 0);
			item->goalAnimState = 3;

			AddActiveItem(itemNum);

			item->status = ITEM_ACTIVE;
			l->currentAnimState = STATE_LARA_MISC_CONTROL;
			l->goalAnimState = STATE_LARA_STOP;
			Lara.isMoving = false;
			Lara.gunStatus = LG_HANDS_BUSY;

			if (applyRot)
				item->pos.yRot ^= ANGLE(180);
			return;
		}

		if (item->objectNumber >= ID_KICK_DOOR1)
		{
			if (MoveLaraPosition(&KickDoorPos, item, l))
			{
				l->animNumber = ANIMATION_LARA_DOOR_KICK;
				l->frameNumber = GF(ANIMATION_LARA_DOOR_KICK, 0);
				item->goalAnimState = 2;

				AddActiveItem(itemNum);

				item->status = ITEM_ACTIVE;
				l->currentAnimState = STATE_LARA_MISC_CONTROL;
				l->goalAnimState = STATE_LARA_STOP;
				Lara.isMoving = false;
				Lara.gunStatus = LG_HANDS_BUSY;

				if (applyRot)
					item->pos.yRot ^= ANGLE(180);
				return;
			}
		}
		else if (item->objectNumber == ID_PUSHPULL_DOOR1 || item->objectNumber == ID_PUSHPULL_DOOR2)
		{
			if (MoveLaraPosition(&PushDoorPos, item, l))
			{
				l->animNumber = ANIMATION_LARA_DOOR_OPEN_FORWARD;
				l->frameNumber = GF(ANIMATION_LARA_DOOR_OPEN_FORWARD, 0);
				item->goalAnimState = 2;

				AddActiveItem(itemNum);

				item->status = ITEM_ACTIVE;
				l->currentAnimState = STATE_LARA_MISC_CONTROL;
				l->goalAnimState = STATE_LARA_STOP;
				Lara.isMoving = false;
				Lara.gunStatus = LG_HANDS_BUSY;

				if (applyRot)
					item->pos.yRot ^= ANGLE(180);
			}
			return;
		}

		Lara.generalPtr = (void*)itemNum;
		return;
	}

	if (!item->currentAnimState)
		DoorCollision(itemNum, l, coll);
}

void PushPullKickDoorControl(short itemNumber)
{
	ITEM_INFO* item;
	DOOR_DATA* door;

	item = &Items[itemNumber];
	door = (DOOR_DATA*)item->data;

	if (!door->opened)
	{
		OpenThatDoor(&door->d1, door);
		OpenThatDoor(&door->d2, door);
		OpenThatDoor(&door->d1flip, door);
		OpenThatDoor(&door->d2flip, door);
		door->opened = TRUE;
	} 

	AnimateItem(item);
}

void DoorCollision(short itemNum, ITEM_INFO* l, COLL_INFO* coll) 
{
	ITEM_INFO* item = &Items[itemNum];

	if (item->triggerFlags == 2
	&&  item->status == ITEM_INACTIVE && !item->gravityStatus // CHECK
	&&  ((TrInput & IN_ACTION || g_Inventory->GetSelectedObject() == ID_CROWBAR_ITEM)
	&&  l->currentAnimState == STATE_LARA_STOP
	&&  l->animNumber == ANIMATION_LARA_STAY_IDLE
	&&  !l->hitStatus
	&&  Lara.gunStatus == LG_NO_ARMS
	||  Lara.isMoving && Lara.generalPtr == (void*)itemNum))
	{
		item->pos.yRot ^= ANGLE(180);
		if (TestLaraPosition(CrowbarDoorBounds, item, l))
		{
			if (!Lara.isMoving)
			{
				if (g_Inventory->GetSelectedObject() == NO_ITEM)
				{
					if (g_Inventory->IsObjectPresentInInventory(ID_CROWBAR_ITEM))
					{
						g_Inventory->SetEnterObject(ID_CROWBAR_ITEM);
						item->pos.yRot ^= ANGLE(180);
					}
					else
					{
						if (OldPickupPos.x != l->pos.xPos || OldPickupPos.y != l->pos.yPos || OldPickupPos.z != l->pos.zPos)
						{
							OldPickupPos.x = l->pos.xPos;
							OldPickupPos.y = l->pos.yPos;
							OldPickupPos.z = l->pos.zPos;
							SayNo();
						}
						item->pos.yRot ^= ANGLE(180);
					}
					return;
				}
				if (g_Inventory->GetSelectedObject() != ID_CROWBAR_ITEM)
				{
					item->pos.yRot ^= ANGLE(180);
					return;
				}
			}

			g_Inventory->SetSelectedObject(NO_ITEM);
			if (MoveLaraPosition(&CrowbarDoorPos, item, l))
			{
				l->animNumber = ANIMATION_LARA_DOOR_OPEN_CROWBAR;
				l->frameNumber = GF(ANIMATION_LARA_DOOR_OPEN_CROWBAR, 0);
				l->currentAnimState = STATE_LARA_MISC_CONTROL;
				item->pos.yRot ^= ANGLE(180);

				AddActiveItem(itemNum);

				item->flags |= IFLAG_ACTIVATION_MASK;
				item->status = ITEM_ACTIVE;
				item->goalAnimState = STATE_LARA_RUN_FORWARD;
				Lara.isMoving = 0;
				Lara.gunStatus = LG_HANDS_BUSY;

				return;
			}

			Lara.generalPtr = (void*)itemNum;
		}
		else if (Lara.isMoving && Lara.generalPtr == (void*)itemNum)
		{
			Lara.isMoving = 0;
			Lara.gunStatus = LG_NO_ARMS;
		}

		item->pos.yRot ^= ANGLE(180);
	}

	if (TestBoundsCollide(item, l, coll->radius))
	{
		if (TestCollision(item, l))
		{
			if (coll->enableBaddiePush)
			{
				if (!(item->objectNumber >= ID_LIFT_DOORS1 && item->objectNumber <= ID_LIFT_DOORS2) || item->itemFlags[0])
				{
					ItemPushLara(item, l, coll, FALSE, TRUE);
				}
			}
		}
	}
}

void DoorControl(short itemNumber) 
{
	ITEM_INFO* item = &Items[itemNumber];
	DOOR_DATA* door = (DOOR_DATA*)item->data;
	
	if (item->triggerFlags == 1)
	{
		if (item->itemFlags[0])
		{
			short* bounds = GetBoundsAccurate(item);
			--item->itemFlags[0];
			item->pos.yPos -= 12;
			int y = bounds[2] + item->itemFlags[2] - STEP_SIZE;
			if (item->pos.yPos < y)
			{
				item->pos.yPos = y;
				item->itemFlags[0] = 0;
			}
			if (!door->opened)
			{
				OpenThatDoor(&door->d1, door);
				OpenThatDoor(&door->d2, door);
				OpenThatDoor(&door->d1flip, door);
				OpenThatDoor(&door->d2flip, door);
				door->opened = TRUE;
			}
		}
		else
		{
			if (item->pos.yPos < item->itemFlags[2])
				item->pos.yPos += 2;
			if (item->pos.yPos >= item->itemFlags[2])
			{
				item->pos.yPos = item->itemFlags[2];
				if (door->opened)
				{
					ShutThatDoor(&door->d1, door);
					ShutThatDoor(&door->d2, door);
					ShutThatDoor(&door->d1flip, door);
					ShutThatDoor(&door->d2flip, door);
					door->opened = FALSE;
				}
			}
		}
		return;
	}

	if (item->objectNumber < ID_LIFT_DOORS1 || item->objectNumber > ID_LIFT_DOORS2)
	{
		if (TriggerActive(item))
		{
			if (!item->currentAnimState)
			{
				item->goalAnimState = 1;
				AnimateItem(item);
				return;
			}
			if (!door->opened)
			{
				OpenThatDoor(&door->d1, door);
				OpenThatDoor(&door->d2, door);
				OpenThatDoor(&door->d1flip, door);
				OpenThatDoor(&door->d2flip, door);
				door->opened = TRUE;
			}
			/*if (item->frameNumber == Anims[item->animNumber].frameEnd)
			{
				if (gfCurrentLevel == 11)
				{
					v9 = item->object_number;
					if (v9 != 302 && v9 != 304)
					{
						LOBYTE(v5) = AnimateItem((int)item);
						return v5;
					}
				LABEL_40:
					v10 = item->_bf15ea;
					LOBYTE(v10) = v10 | 6;
					item->_bf15ea = v10;
					LOBYTE(v5) = AnimateItem((int)item);
					return v5;
				}
				if (gfCurrentLevel >= 0xCu && gfCurrentLevel <= 0xEu && item->object_number == 300)
					goto LABEL_40;
			}*/
		}
		else
		{
			item->status = ITEM_ACTIVE;

			if (item->currentAnimState == 1)
			{
				item->goalAnimState = 0;
				AnimateItem(item);
				return;
			}

			if (door->opened)
			{
				ShutThatDoor(&door->d1, door);
				ShutThatDoor(&door->d2, door);
				ShutThatDoor(&door->d1flip, door);
				ShutThatDoor(&door->d2flip, door);
				door->opened = false;
			}
		}	
		AnimateItem(item);
		return;
	}

	if (!TriggerActive(item))
	{
		if (item->itemFlags[0] >= SECTOR(4))
		{
			if (door->opened)
			{
				ShutThatDoor(&door->d1, door);
				ShutThatDoor(&door->d2, door);
				ShutThatDoor(&door->d1flip, door);
				ShutThatDoor(&door->d2flip, door);
				door->opened = FALSE;
			}
		}
		else
		{
			if (!item->itemFlags[0])
				SoundEffect(SFX_LIFT_DOORS, &item->pos, 0);
			item->itemFlags[0] += STEP_SIZE;
		}
	}
	else
	{
		if (item->itemFlags[0] > 0)
		{
			if (item->itemFlags[0] == SECTOR(4))
				SoundEffect(SFX_LIFT_DOORS, &item->pos, 0);
			item->itemFlags[0] -= STEP_SIZE;
		}
		if (!door->opened)
		{
			DontUnlockBox = TRUE;
			OpenThatDoor(&door->d1, door);
			OpenThatDoor(&door->d2, door);
			OpenThatDoor(&door->d1flip, door);
			OpenThatDoor(&door->d2flip, door);
			DontUnlockBox = FALSE;
			door->opened = TRUE;
		}
	}
}

void OpenThatDoor(DOORPOS_DATA* doorPos, DOOR_DATA* dd) 
{
	FLOOR_INFO* floor = doorPos->floor;

	if (floor)
	{
		memcpy(doorPos->floor, &doorPos->data, sizeof(FLOOR_INFO));
		
		short boxIndex = doorPos->block;
		if (boxIndex != NO_BOX)
		{
			if (!DontUnlockBox)
				Boxes[boxIndex].overlapIndex &= ~BLOCKED;

			for (int i = 0; i < NUM_SLOTS; i++)
			{
				BaddieSlots[i].LOT.targetBox = NO_BOX;
			}
		}
	}

	if (dd->dptr1)
	{
		short n = dd->dn1 < 0 ? -1 : 1;
		if (dd->dn1 & 1)
		{
			dd->dptr1[0] = n;
		}
		else if (dd->dn1 & 2)
		{
			dd->dptr1[1] = n;
		}
		else
		{
			dd->dptr1[2] = n;
		}
		
		n = dd->dn3 < 0 ? -1 : 1;
		if (dd->dn3 & 1)
		{
			dd->dptr3[0] = n;
		}
		else if (dd->dn3 & 2)
		{
			dd->dptr3[1] = n;
		}
		else
		{
			dd->dptr3[2] = n;
		}
		
		if (dd->dptr2)
		{
			n = dd->dn2 < 0 ? -1 : 1;
			if (dd->dn2 & 1)
			{
				dd->dptr2[0] = n;
			}
			else if (dd->dn2 & 2)
			{
				dd->dptr2[1] = n;
			}
			else
			{
				dd->dptr2[2] = n;
			}
		}
		
		if (dd->dptr4)
		{
			n = dd->dn4 < 0 ? -1 : 1;
			if (dd->dn4 & 1)
			{
				dd->dptr4[0] = n;
			}
			else if (dd->dn4 & 2)
			{
				dd->dptr4[1] = n;
			}
			else
			{
				dd->dptr4[2] = n;
			}
		}
	}
}

void ShutThatDoor(DOORPOS_DATA* doorPos, DOOR_DATA* dd) 
{
	FLOOR_INFO* floor = doorPos->floor;

	if (floor)
	{
		floor->box = NO_BOX;
		floor->ceiling = -127;
		floor->floor = -127;
		floor->index = 0;
		floor->skyRoom = NO_ROOM;
		floor->pitRoom = NO_ROOM;

		short boxIndex = doorPos->block;
		if (boxIndex != NO_BOX)
		{
			Boxes[boxIndex].overlapIndex |= BLOCKED;
			for (int i = 0; i < NUM_SLOTS; i++)
			{
				BaddieSlots[i].LOT.targetBox = NO_BOX;
			}
		}
	}

	if (dd->dptr1)
	{
		dd->dptr1[0] = 0;
		dd->dptr1[1] = 0;
		dd->dptr1[2] = 0;

		dd->dptr3[0] = 0;
		dd->dptr3[1] = 0;
		dd->dptr3[2] = 0;

		if (dd->dptr2)
		{
			dd->dptr2[0] = 0;
			dd->dptr2[1] = 0;
			dd->dptr2[2] = 0;
		}
		
		if (dd->dptr4)
		{
			dd->dptr4[0] = 0;
			dd->dptr4[1] = 0;
			dd->dptr4[2] = 0;
		}
	}
}

void Inject_Door()
{
	INJECT(0x00429EC0, SequenceDoorControl);
	INJECT(0x00429CF0, UnderwaterDoorCollision);
	INJECT(0x00429B30, DoubleDoorCollision);
	INJECT(0x004298D0, PushPullKickDoorCollision);
	INJECT(0x00429840, PushPullKickDoorControl);
	INJECT(0x00429550, DoorCollision);
	INJECT(0x00429140, DoorControl);
	INJECT(0x00428FD0, OpenThatDoor);
	INJECT(0x00428EF0, ShutThatDoor);
}