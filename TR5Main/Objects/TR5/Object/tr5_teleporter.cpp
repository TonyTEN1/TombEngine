#include "framework.h"
#include "tr5_teleporter.h"
#include "Game/items.h"
#include "Specific/level.h"
#include "Game/control/control.h"
#include "Sound/sound.h"
#include "Game/effects/weather.h"
#include "Game/Lara/lara.h"
#include "Game/camera.h"

using namespace TEN::Effects::Environment;

void InitialiseTeleporter(short itemNumber)
{
	/*ITEM_INFO* item = &g_Level.Items[itemNumber];

	if (item->triggerFlags == 512)
	{
		ITEM_INFO* puzzleHoleItem = FindItem(ID_PUZZLE_HOLE2);
		v4 = (signed int)((unsigned __int64)(391146079i64 * ((char*)v3 - (char*)items)) >> 32) >> 9;
		result = (unsigned int)((unsigned __int64)(391146079i64 * ((char*)v3 - (char*)items)) >> 32) >> 31;
		item->itemFlags[1] = result + v4;
	}*/
}

void ControlTeleporter(short itemNumber)
{
	ITEM_INFO* item = &g_Level.Items[itemNumber];

	if (!TriggerActive(item))
		return;

	/*if (item->triggerFlags == 512)
	{
		if (item->itemFlags[2])
		{
			Lara.Puzzles[1] = 1;
			RemoveActiveItem(itemNumber);
			item->flags &= 0xC1FF;
		}
		else
		{
			item->itemFlags[0] += 2;

			if (item->itemFlags[0] <= 255)
			{
				int flags = item->itemFlags[0] >> 3;
				if (flags >= 4)
				{
					if (flags > 31)
						flags = 31;
				}
				else
				{
					flags = 4;
				}

				ITEM_INFO* targetItem = &g_Level.Items[item->itemFlags[1]];
				SoundEffect(SFX_RICH_TELEPORT, &targetItem->pos, (flags << 8) | 8);

				if (GlobalCounter & 1)
				{
					PHD_VECTOR src;
					pos.x = targetItem->pos.xPos;
					pos.y = targetItem->pos.yPos - 496;
					pos.z = targetItem->pos.zPos + 472;

					int dl = 4 * item->itemFlags[0] + 256;

					PHD_VECTOR dest;
					dest.x = src.x + GetRandomControl() % dl - (dl >> 1);
					dest.y = src.y + GetRandomControl() % dl - (dl >> 1);
					dest.z = src.z + GetRandomControl() % dl - (dl >> 1);

					int color = (item->itemFlags[0] >> 2) | (((item->itemFlags[0] - (GetRandomControl() % (item->itemFlags[0] >> 1))) | (item->itemFlags[0] << 8)) << 8);
					color |= 0x18; // BYTE1

					//TriggerEnergyArc(&src, &dest, (GetRandomControl() & 0x1F) + 16, color, 15, 40, 5);

					v20 = v16;
					v21 = v12 & 0xFFFFFFFE;
					LOBYTE(v20) = v16 & 0xFE;
					BYTE1(v21) |= 0x80u;
					TriggerLightningGlow(src.x, src.y, src.z, (item->itemFlags[0] >> 3) | ((v20 | (v21 << 8)) << 7));
					v22 = GetRandomControl();
					TriggerDynamicLight(src.x, src.y, src.z, (v22 & 3) + (item->itemFlags[0] >> 5) + 8, v12, v16, v13);
				}
				LOBYTE(v3) = GetRandomControl();
				if (v3 & 1)
				{
					v23 = item->itemFlags[0];
					v24 = item->itemFlags[0];
					v25 = GetRandomControl();

					auto R = v23;
					auto G = v24 - v25 % (v24 >> 1);
					auto B = v24 >> 2;
					Weather.Flash(R, G, B, 0.03f);

					LOBYTE(v3) = SoundEffect(399, 0, 0);
				}
				if (!(GlobalCounter & 3))
				{
					v26 = GetRandomControl();
					v27 = 0;
					v28 = v26 & 3;
					v29 = 0;
					if (v28)
					{
						if (v28 == 1)
							v29 = 512;
						else
							v27 = v28 != 2 ? 512 : -512;
					}
					else
					{
						v29 = -512;
					}
					v30 = item->itemFlags[0];
					v31 = &g_Level.Items[item->itemFlags[1]];
					src.xPos = v29 + v31->pos.xPos;
					src.yPos = v31->pos.yPos - 2328;
					src.zPos = v27 + v31->pos.zPos;
					*(_DWORD*)& src.xRot = v31->pos.xPos;
					v32 = item->itemFlags[0];
					*(_DWORD*)& src.zRot = v31->pos.yPos - 496;
					v45 = v31->pos.zPos + 472;
					v33 = (v30 >> 2) | (((v30 - GetRandomControl() % (v30 >> 1)) | ((v32 | 0x2400) << 8)) << 8);
					v34 = GetRandomControl();
					TriggerEnergyArc((PHD_VECTOR*)& src, (PHD_VECTOR*)& src.xRot, (v34 & 0xF) + 16, v33, 13, 56, 5);
					v35 = &spark[GetFreeSpark()];
					v35->On = 1;
					v36 = item->itemFlags[0];
					v35->dR = v36;
					v35->sR = v36;
					v37 = item->itemFlags[0] >> 1;
					v35->dG = v37;
					v35->sG = v37;
					v38 = item->itemFlags[0];
					v35->ColFadeSpeed = 20;
					v38 >>= 2;
					v35->dB = v38;
					v35->sB = v38;
					v35->FadeToBlack = 4;
					v35->Life = 24;
					v35->sLife = 24;
					v35->TransType = TransTypeEnum::COLADD;
					v35->x = src.xPos;
					v35->y = src.yPos;
					v35->z = src.zPos;
					v35->Zvel = 0;
					v35->Yvel = 0;
					v35->Xvel = 0;
					v35->Flags = 10;
					v39 = objects[458].mesh_index;
					v35->Scalar = 3;
					v35->MaxYvel = 0;
					v35->Def = v39 + 11;
					v35->Gravity = 0;
					v3 = (GetRandomControl() & 3) + 24;
					v35->dSize = v3;
					v35->sSize = v3;
					v35->Size = v3;
				}
				return v3;
			}
			FlashFadeR = 255;
			FlashFadeG = 255;
			FlashFadeB = 64;
			FlashFader = 32;
			item->itemFlags[2] = 1;
			SoundEffect(400, 0, (int)& unk_800004);
		}
	}*/

	Lara.Control.Locked = false;

	if (item->TriggerFlags == 666)
	{
		if (item->ItemFlags[0] == 70)
		{
			SoundEffect(SFX_TR5_LIFTHITFLOOR1, 0, 0);
			SoundEffect(SFX_TR5_LIFTHITFLOOR2, 0, 0);
		}

		LaraItem->AnimNumber = LA_ELEVATOR_RECOVER;
		LaraItem->FrameNumber = g_Level.Anims[LaraItem->AnimNumber].frameBase;
		LaraItem->TargetState = LS_MISC_CONTROL;
		LaraItem->ActiveState = LS_MISC_CONTROL;

		item->ItemFlags[0]++;
		if (item->ItemFlags[0] >= 150)
			KillItem(itemNumber);
	}
	else
	{
		Camera.fixedCamera = true;
		LaraItem->Position.xPos = item->Position.xPos;
		LaraItem->Position.zPos = item->Position.zPos;
		LaraItem->Position.yRot = item->Position.yRot - ANGLE(180.0f);

		short roomNumber = item->RoomNumber;
		FLOOR_INFO* floor = GetFloor(item->Position.xPos, item->Position.yPos, item->Position.zPos, &roomNumber);
		LaraItem->Position.yPos = GetFloorHeight(floor, item->Position.xPos, item->Position.yPos, item->Position.zPos);

		if (LaraItem->RoomNumber != roomNumber)
			ItemNewRoom(Lara.ItemNumber, roomNumber);

		if (item->Flags & ONESHOT)
		{
			KillItem(itemNumber);
		}
		else if (item->TriggerFlags != 512)
		{
			RemoveActiveItem(itemNumber);
			item->Flags &= 0xC1FFu;
		}
	}
}
