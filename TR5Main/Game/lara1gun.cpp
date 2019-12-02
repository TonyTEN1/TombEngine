#include "lara1gun.h"
#include "items.h"
#include "Lara.h"
#include "larafire.h"
#include "draw.h"
#include "Box.h"
#include "control.h"
#include "effects.h"
#include "effect2.h"
#include "lot.h"
#include "collide.h"
#include "debris.h"
#include "lara2gun.h"
#include "switch.h"
#include "objects.h"
#include "sphere.h"
#include "traps.h"

#include "..\Global\global.h"
#include "..\Scripting\GameFlowScript.h"

extern LaraExtraInfo g_LaraExtra;
extern GameFlow* g_GameFlow;

int HKCounter = 0;

void __cdecl FireHarpoon()
{
	// If no ammo then exit
	//if (g_LaraExtra.numHarpoonAmmos <= 0)
	//	return;

	// Create a new item for harpoon
	short itemNumber = CreateItem();
	if (itemNumber != NO_ITEM)
	{
		GAME_VECTOR pos;
		Vector3 dxPos;

		ITEM_INFO* item = &Items[itemNumber];

		item->shade = 0x4210 | 0x8000;
		item->objectNumber = ID_HARPOON;
		item->roomNumber = LaraItem->roomNumber;
		pos.x = dxPos.x = -2;
		pos.y = dxPos.y = 0; // -273 - 100;
		pos.z = dxPos.z = 77;

		g_Renderer->GetLaraBonePosition(&dxPos, HAND_R);
		GetLaraJointPosition((PHD_VECTOR*)&pos, LJ_RHAND);

		/*item->pos.xPos = pos.x = dxPos.x;
		item->pos.yPos = pos.y = dxPos.y;
		item->pos.zPos = pos.z = dxPos.z;*/

		item->pos.xPos = pos.x;
		item->pos.yPos = pos.y;
		item->pos.zPos = pos.z;

		InitialiseItem(itemNumber);

		if (Lara.target)
		{
			find_target_point(Lara.target, &pos);

			item->pos.yRot = ATAN(pos.z - item->pos.zPos, pos.x - item->pos.xPos);
			int distance = SQRT_ASM(SQUARE(pos.z - item->pos.zPos) + SQUARE(pos.x - item->pos.xPos));
			item->pos.xRot = -ATAN(distance, pos.y - item->pos.yPos);
		}
		else
		{
			item->pos.xRot = LaraItem->pos.xRot + Lara.torsoXrot;
			item->pos.yRot = LaraItem->pos.yRot + Lara.torsoYrot;
		}

		item->pos.zRot = 0;

		item->fallspeed = (short)(-HARPOON_SPEED * SIN(item->pos.xRot) >> W2V_SHIFT);
		item->speed = (short)(HARPOON_SPEED * COS(item->pos.xRot) >> W2V_SHIFT);
		item->hitPoints = HARPOON_TIME;

		AddActiveItem(itemNumber);

		Savegame.Level.AmmoUsed++;
		Savegame.Game.AmmoUsed++;
	}
}

void __cdecl ControlHarpoonBolt(short itemNumber)
{
	ITEM_INFO* item = &Items[itemNumber];

	int oldX = item->pos.xPos;
	int oldY = item->pos.yPos;
	int oldZ = item->pos.zPos;
	short oldRoom = item->roomNumber;

	item->pos.xPos += item->speed * SIN(item->pos.yRot) >> W2V_SHIFT;
	item->pos.yPos += item->fallspeed;
	item->pos.zPos += item->speed * COS(item->pos.yRot) >> W2V_SHIFT;

	short roomNumber = item->roomNumber;
	FLOOR_INFO* floor = GetFloor(item->pos.xPos, item->pos.yPos, item->pos.zPos, &roomNumber);
	item->floor = GetFloorHeight(floor, item->pos.xPos, item->pos.yPos, item->pos.zPos);

	if (item->roomNumber != roomNumber)
		ItemNewRoom(itemNumber, roomNumber);

	//  First check if the harpoon has it an item
	short targetItemNumber = 0;
	ITEM_INFO* target;
	for (targetItemNumber = Rooms[item->roomNumber].itemNumber; targetItemNumber != NO_ITEM; targetItemNumber = target->nextItem)
	{
		target = &Items[targetItemNumber];
		if (target == LaraItem || !target->collidable)
			continue;

		if (/*target->objectNumber == SMASH_WINDOW ||
			target->objectNumber == SMASH_OBJECT1 ||
			target->objectNumber == SMASH_OBJECT2 ||
			target->objectNumber == SMASH_OBJECT3 ||
			target->objectNumber == CARCASS ||
			target->objectNumber == EXTRAFX6 ||*/
			(target->status != ITEM_INVISIBLE && Objects[target->objectNumber].collision))
		{
			// check against bounds of target for collision
			short* bounds = GetBestFrame(target);
			if (item->pos.yPos < target->pos.yPos + bounds[2] || item->pos.yPos > target->pos.yPos + bounds[3])
				continue;

			// get vector from target to bolt and check against x,z bounds
			short c = COS(target->pos.yRot);
			short s = SIN(target->pos.yRot);

			int x = item->pos.xPos - target->pos.xPos;
			int z = item->pos.zPos - target->pos.zPos;
			int rx = (c * x - s * z) >> W2V_SHIFT;

			int ox = oldX - target->pos.xPos;
			int oz = oldZ - target->pos.zPos;
			int sx = (c * ox - s * oz) >> W2V_SHIFT;

			if ((rx < bounds[0] && sx < bounds[0]) || (rx > bounds[1] && sx > bounds[1]))
				continue;

			int rz = (c * z + s * x) >> W2V_SHIFT;
			int sz = (c * oz + s * ox) >> W2V_SHIFT;

			if ((rz < bounds[4] && sz < bounds[4]) || (rz > bounds[5] && sz > bounds[5]))
				continue;

			// TODO:
			/*if (target->objectNumber == SMASH_OBJECT1 && CurrentLevel != LV_CRASH)
			{
				SmashWindow(targetItemNumber);
			}
			else if (target->objectNumber == SMASH_WINDOW ||
				target->objectNumber == SMASH_OBJECT2 ||
				target->objectNumber == SMASH_OBJECT3)
			{
				SmashWindow(targetItemNumber);
			}
			else if (target->objectNumber == CARCASS || target->objectNumber == EXTRAFX6)
			{
				if (item->status != ACTIVE)
				{
					item->status = ACTIVE;
					AddActiveItem(targetItemNumber);
				}
			}
			else if (target->objectNumber != SMASH_OBJECT1)
			{*/
			if (Objects[target->objectNumber].intelligent)
			{
				DoLotsOfBlood(item->pos.xPos, item->pos.yPos, item->pos.zPos, 0, 0, item->roomNumber, 3);
				HitTarget(target, NULL, Weapons[WEAPON_HARPOON_GUN].damage << item->itemFlags[0], 0);
				Savegame.Level.AmmoHits++;
				Savegame.Game.AmmoHits++;
			}

			KillItem(itemNumber);
			item->afterDeath = 0;
			return;
		}		
	}

	// Has harpoon hit a wall?
	if (item->pos.yPos >= item->floor || item->pos.yPos <= GetCeiling(floor, item->pos.xPos, item->pos.yPos, item->pos.zPos))
	{
		if (item->hitPoints == HARPOON_TIME)
		{
			item->currentAnimState = item->pos.xRot;
		}

		if (item->hitPoints >= 192)
		{
			item->pos.xRot = item->currentAnimState + ((((rcossin_tbl[((item->hitPoints - 192) << 9) & 4095] >> 1) - 1024)*(item->hitPoints - 192)) >> 6);
			item->hitPoints--;
		}

		item->hitPoints--;
		if (item->hitPoints <= 0)
		{
			KillItem(itemNumber);
			item->afterDeath = 0;
			return;
		}
		item->speed = item->fallspeed = 0;
	}
	else
	{
		item->pos.zRot += ANGLE(35);
		if (!(Rooms[item->roomNumber].flags & 1))
		{
			item->pos.xRot -= ANGLE(1);
			if (item->pos.xRot < -16384)
				item->pos.xRot = -16384;
			item->fallspeed = (short)(-HARPOON_SPEED * SIN(item->pos.xRot) >> W2V_SHIFT);
			item->speed = (short)(HARPOON_SPEED * COS(item->pos.xRot) >> W2V_SHIFT);
		}
		else
		{
			// Create bubbles
			if ((Wibble & 15) == 0)
				CreateBubble(&item->pos, item->roomNumber, 0);
			//TriggerRocketSmoke(item->pos.xPos, item->pos.yPos, item->pos.zPos, 64);
			item->fallspeed = (short)(-(HARPOON_SPEED >> 1) * SIN(item->pos.xRot) >> W2V_SHIFT);
			item->speed = (short)((HARPOON_SPEED >> 1) * COS(item->pos.xRot) >> W2V_SHIFT);
		}
	}


	roomNumber = item->roomNumber;
	floor = GetFloor(item->pos.xPos, item->pos.yPos, item->pos.zPos, &roomNumber);
	GetFloorHeight(floor, item->pos.xPos, item->pos.yPos, item->pos.zPos);
	if (item->roomNumber != roomNumber)
		ItemNewRoom(itemNumber, roomNumber);
}

long	tbx, tby, tbz;

void __cdecl FireGrenade()
{
	int x = 0;
	int y = 0;
	int z = 0;
	
	short* ammo = GetAmmo(WEAPON_GRENADE_LAUNCHER);
	if (*ammo)
	{
		Lara.hasFired = true;

		short itemNumber = CreateItem();
		if (itemNumber != NO_ITEM)
		{
			ITEM_INFO* item = &Items[itemNumber];
			
			item->shade = 0xC210;
			item->objectNumber = ID_GRENADE;
			item->roomNumber = LaraItem->roomNumber;

			PHD_VECTOR jointPos;
			jointPos.x = 0;
			jointPos.y = 276;
			jointPos.z = 80;

			GetLaraJointPosition(&jointPos, LJ_RHAND);

			item->pos.xPos = x = jointPos.x;
			item->pos.yPos = y = jointPos.y;
			item->pos.zPos = z = jointPos.z;

			FLOOR_INFO* floor = GetFloor(jointPos.x, jointPos.y, jointPos.z, &item->roomNumber);
			int height = GetFloorHeight(floor, jointPos.x, jointPos.y, jointPos.z);
			if (height < jointPos.y)
			{
				item->pos.xPos = LaraItem->pos.xPos;
				item->pos.yPos = jointPos.y;
				item->pos.zPos = LaraItem->pos.zPos;
				item->roomNumber = LaraItem->roomNumber;
			}

			jointPos.x = 0;
			jointPos.y = 1204;
			jointPos.z = 5;

			GetLaraJointPosition(&jointPos, LJ_RHAND);

			SmokeCountL = 32;
			SmokeWeapon = WEAPON_GRENADE_LAUNCHER;

			if (LaraItem->meshBits)
			{
				for (int i = 0; i < 5; i++)
					TriggerGunSmoke(x, y, z, jointPos.x - x, jointPos.y - y, jointPos.z - z, 1, WEAPON_GRENADE_LAUNCHER, 32);

			}

			InitialiseItem(itemNumber);

			item->pos.xRot = LaraItem->pos.xRot + Lara.leftArm.xRot;
			item->pos.yRot = LaraItem->pos.yRot + Lara.leftArm.yRot;
			item->pos.zRot = 0;

			if (!Lara.leftArm.lock)
			{
				item->pos.xRot += Lara.torsoXrot;
				item->pos.yRot += Lara.torsoYrot;
			}

			item->speed = GRENADE_SPEED;
			item->fallspeed = (-512 * SIN(item->pos.xRot)) >> W2V_SHIFT;
			item->currentAnimState = item->pos.xRot;
			item->goalAnimState = item->pos.yRot;
			item->requiredAnimState = 0;
			item->hitPoints = 120;	
			item->itemFlags[0] = WEAPON_AMMO2;

			AddActiveItem(itemNumber);

			if (*ammo != -1)
				(*ammo)--;

			item->itemFlags[0] = g_LaraExtra.Weapons[WEAPON_GRENADE_LAUNCHER].SelectedAmmo;

			Savegame.Level.AmmoUsed++;
			Savegame.Game.AmmoUsed++;
		}
	}
}

void __cdecl ControlGrenade(short itemNumber)
{
	ITEM_INFO* item = &Items[itemNumber];

	if (item->itemFlags[1])
	{
		item->itemFlags[1]--;

		if (item->itemFlags[1])
		{
			if (item->itemFlags[0] == WEAPON_AMMO3)
			{
				// Flash grenades

				if (item->itemFlags[1] == 1)
				{
					WeaponEnemyTimer = 120;
					FlashFadeR = 255;
					FlashFadeG = 255;
					FlashFadeB = 255;
				}
				else
				{
					FlashFadeR = (GetRandomControl() & 0x1F) + 224;
					FlashFadeG = FlashFadeB = FlashFadeR - GetRandomControl() & 0x1F;
				}

				FlashFader = 32;

				// if Volumetric then FlashFader = 0?
				/*if (sub_47F960())
				{
					FlashFader = 0;
				}*/

				GrenadeExplosionEffects(item->pos.xPos, item->pos.yPos, item->pos.zPos, item->roomNumber);
				GrenadeExplosionEffects(item->pos.xPos, item->pos.yPos, item->pos.zPos, item->roomNumber);
			}
			else
			{
				short newGrenadeItemNumber = CreateItem();

				if (newGrenadeItemNumber != NO_ITEM)
				{
					ITEM_INFO* newGrenade = &Items[newGrenadeItemNumber];

					newGrenade->shade = 0xC210;
					newGrenade->objectNumber = ID_GRENADE;
					newGrenade->roomNumber = item->roomNumber;
					newGrenade->pos.xPos = (GetRandomControl() & 0x1FF) + item->pos.xPos - 256;
					newGrenade->pos.yPos = item->pos.yPos - 256;
					newGrenade->pos.zPos = (GetRandomControl() & 0x1FF) + item->pos.zPos - 256;
					
					InitialiseItem(newGrenadeItemNumber);
					
					newGrenade->pos.xRot = (GetRandomControl() & 0x3FFF) + 0x2000;
					newGrenade->pos.yRot = GetRandomControl() * 2;
					newGrenade->pos.zRot = 0;
					newGrenade->speed = 64;
					newGrenade->fallspeed = -64 * SIN(newGrenade->pos.xRot) >> W2V_SHIFT;
					newGrenade->currentAnimState = newGrenade->pos.xRot;
					newGrenade->goalAnimState = newGrenade->pos.yRot;
					newGrenade->requiredAnimState = 0;
					
					AddActiveItem(newGrenadeItemNumber);
					
					newGrenade->status = ITEM_INVISIBLE;
					newGrenade->itemFlags[2] = item->itemFlags[2];
					newGrenade->hitPoints = 3000;
	
					if (Rooms[newGrenade->roomNumber].flags & ENV_FLAG_WATER)
						newGrenade->hitPoints = 1;
				}
			}

			return;
		}

		KillItem(itemNumber);
		return;
	}
	   
	int oldX = item->pos.xPos;
	int oldY = item->pos.yPos;
	int oldZ = item->pos.zPos;

	int xv;
	int yv;
	int zv;

	item->shade = 0xC210;

	bool aboveWater = false;
	if (Rooms[item->roomNumber].flags & 1)
	{
		aboveWater = false;
		item->fallspeed += (5 - item->fallspeed) >> 1;
		item->speed -= item->speed >> 2;
		if (item->speed)
		{
			item->pos.zRot += (((item->speed >> 4) + 3) * ANGLE(1));
			if (item->requiredAnimState)
				item->pos.yRot += (((item->speed >> 2) + 3) * ANGLE(1));
			else
				item->pos.xRot += (((item->speed >> 2) + 3) * ANGLE(1));
		}
	}
	else
	{
		aboveWater = true;
		item->fallspeed += 3;
		if (item->speed)
		{
			item->pos.zRot += (((item->speed >> 2) + 7) * ANGLE(1));
			if (item->requiredAnimState)
				item->pos.yRot += (((item->speed >> 1) + 7) * ANGLE(1));
			else
				item->pos.xRot += (((item->speed >> 1) + 7) * ANGLE(1));

		}
	}

	if (item->speed && aboveWater)
	{
		// For now keep this legacy math
		phd_PushUnitMatrix();

		*(MatrixPtr + M03) = 0;
		*(MatrixPtr + M13) = 0;
		*(MatrixPtr + M23) = 0;

		phd_RotYXZ(item->pos.yRot - ANGLE(180), item->pos.xRot, item->pos.zRot);
		phd_TranslateRel(0, 0, -64);

		int wx = (*(MatrixPtr + M03) >> W2V_SHIFT);
		int wy = (*(MatrixPtr + M13) >> W2V_SHIFT);
		int wz = (*(MatrixPtr + M23) >> W2V_SHIFT);

		MatrixPtr -= 12;
		DxMatrixPtr -= 48;

		TriggerRocketSmoke(wx + item->pos.xPos, wy + item->pos.yPos, wz + item->pos.zPos, -1);
	}

	/*XMMATRIX transform;
	XMMATRIX translation;
	XMMATRIX rotation;

	XMMATRIXRotationYawPitchRoll(&rotation, TR_ANGLE_TO_RAD(item->pos.yRot), TR_ANGLE_TO_RAD(item->pos.xRot),
		TR_ANGLE_TO_RAD(item->pos.zRot));
	XMMATRIXTranslation(&translation, 0, 0, -64);
	XMMATRIXMultiply(&transform, &rotation, &translation);

	int wx = transform._14;
	int wy = transform._24;
	int wz = transform._34;

	if (item->speed && aboveWater)
		TriggerRocketSmoke(wx + item->pos.xPos, wy + item->pos.yPos, wz + item->pos.zPos, -1);
*/

	xv = ((item->speed * SIN(item->goalAnimState)) >> W2V_SHIFT);
	yv = item->fallspeed;
	zv = ((item->speed * COS(item->goalAnimState)) >> W2V_SHIFT);

	item->pos.xPos += xv;
	item->pos.yPos += yv;
	item->pos.zPos += zv;

	FLOOR_INFO* floor;
	int height;
	int ceiling;
	short roomNumber;

	if (item->itemFlags[0] == WEAPON_AMMO4)
	{
		roomNumber = item->roomNumber;
		
		floor = GetFloor(item->pos.xPos, item->pos.yPos, item->pos.zPos, &roomNumber);
		height = GetFloorHeight(floor, item->pos.xPos, item->pos.yPos, item->pos.zPos);
		ceiling = GetCeiling(floor, item->pos.xPos, item->pos.yPos, item->pos.zPos);

		if (height < item->pos.yPos || ceiling > item->pos.yPos)
			item->hitPoints = 1;
	}
	else
	{
		short sYrot = item->pos.yRot;
		item->pos.yRot = item->goalAnimState;

		DoProperDetection(itemNumber, oldX, oldY, oldZ, xv, yv);

		item->goalAnimState = item->pos.yRot;
		item->pos.yRot = sYrot;
	}

	roomNumber = item->roomNumber;
	floor = GetFloor(item->pos.xPos, item->pos.yPos, item->pos.zPos, &roomNumber);

	if ((Rooms[roomNumber].flags & ENV_FLAG_WATER) && aboveWater)
	{
		// Setup splash
		/*dword_804E20 = item->pos.xPos;
    dword_804E24 = *(Rooms + 148 * v78 + 36);
    dword_804E28 = item->pos.zPos;
    word_804E2C = 32;
    word_804E2E = 8;
    word_804E30 = 320;
    v45 = item->fallSpeed;
    word_804E34 = 48;
    word_804E32 = -40 * v45;
    word_804E36 = 32;
    word_804E38 = 480;
    word_804E3A = -20 * item->fallSpeed;
    word_804E3C = 32;
    word_804E3E = 128;
    word_804E40 = 544;
    sub_435380(&dword_804E20);
    if ( item->itemFlags[0] != 4 )
    {
      goto LABEL_35;
    }
    item->hitPoints = 1;*/
	}

	if (item->itemFlags[0] == WEAPON_AMMO4)
		GrenadeLauncherSpecialEffect1(item->pos.xPos, item->pos.yPos, item->pos.zPos, -1, 1);

	// Time to explode?
	int radius = 0;
	bool explode = false; 

	if (item->hitPoints)
	{
		item->hitPoints--;

		if (item->hitPoints)
		{
			if (item->hitPoints > 118)
			{
				return;
			}
		}
		else
		{
			radius = 2048;
			explode = 1;
		}
	}

	if (!(item->itemFlags[0] == WEAPON_AMMO3 && explode))
	{
		int n = 0;
		bool foundCollidedObjects = false;

		do
		{
			// Found possible collided items and statics
			GetCollidedObjects(item, radius, 1, &CollidedItems[0], &CollidedMeshes[0], 1);

			// If no collided items and meshes are found, then exit the loop
			if (!CollidedItems[0] && !CollidedMeshes[0])
				break;

			foundCollidedObjects = true;

			if (item->itemFlags[0] != WEAPON_AMMO3 || explode)
			{
				if (CollidedItems[0])
				{
					if (explode)
					{
						ITEM_INFO* currentItem = CollidedItems[0];
						int k = 0;

						do
						{
							// Item is inside the radius and must explode
							if (item->objectNumber < ID_SMASH_OBJECT1 || item->objectNumber > ID_SMASH_OBJECT8 || (currentItem->flags & 0x40))
							{
								if (currentItem->objectNumber != ID_SHOOT_SWITCH1 &&
									currentItem->objectNumber != ID_SHOOT_SWITCH2 ||
									currentItem->flags & 0x40 &&
									(Objects[currentItem->objectNumber].explodableMeshbits || currentItem == LaraItem))
								{
									DoGrenadeDamageOnBaddie(currentItem, item);
								}
								else
								{
									if (currentItem->flags & 0x3E00 && (currentItem->flags & 0x3E00) != 0x3E00)
									{
										roomNumber = item->roomNumber;
										floor = GetFloor(currentItem->pos.xPos, currentItem->pos.yPos - 256, currentItem->pos.zPos, &roomNumber);
										GetFloorHeight(floor, currentItem->pos.xPos, currentItem->pos.yPos - 256, currentItem->pos.zPos);
										TestTriggers(TriggerIndex, 1, currentItem->flags & 0x3E00);
									}
									else
									{
										short itemNos[8];
										int sw = GetSwitchTrigger(currentItem, itemNos, 1);

										if (sw > 0)
										{
											for (int i = 0; i < sw; i++)
											{
												AddActiveItem(itemNos[i]);
												Items[itemNos[i]].status = ITEM_ACTIVE;
												Items[itemNos[i]].triggerFlags |= 0x3E00;
											}
										}
									}
								}

								if (currentItem->objectNumber == ID_SHOOT_SWITCH1)
								{
									ExplodeItemNode(currentItem, Objects[ID_SHOOT_SWITCH1].nmeshes - 1, 0, 64);
								}

								AddActiveItem((currentItem - Items) / sizeof(ITEM_INFO));
								currentItem->status = ITEM_ACTIVE;
								currentItem->triggerFlags |= 0x3E40;
							}
							else
							{
								TriggerExplosionSparks(currentItem->pos.xPos, currentItem->pos.yPos, currentItem->pos.zPos, 3, -2, 0, currentItem->roomNumber);
								currentItem->pos.yPos -= 128;
								TriggerShockwave(&currentItem->pos, 0x1300030, 96, 0x18806000, 0, 0);
								currentItem->pos.yPos += 128;
								ExplodeItemNode(currentItem, 0, 0, 128);
								short currentItemNumber = (currentItem - Items) / sizeof(ITEM_INFO);
								SmashObject(currentItemNumber);
								KillItem(currentItemNumber);
							}

							k++;
							currentItem = CollidedItems[k];

						} while (currentItem);
					}

					if (CollidedMeshes[0])
					{
						MESH_INFO* currentMesh = CollidedMeshes[0];
						int k = 0;

						do
						{
							if (currentMesh->staticNumber >= 50 && currentMesh->staticNumber < 58)
							{
								TriggerExplosionSparks(currentMesh->x, currentMesh->y, currentMesh->z, 3, -2, 0, item->roomNumber);
								currentMesh->y -= 128;
								TriggerShockwave((PHD_3DPOS*)&currentMesh, 0xB00028, 64, 0x10806000, 0, 0);
								currentMesh->y += 128;
								ShatterObject((SHATTER_ITEM*)item, NULL, -128, item->roomNumber, 0);
								SmashedMeshRoom[SmashedMeshCount] = item->roomNumber;
								SmashedMesh[SmashedMeshCount] = currentMesh;
								SmashedMeshCount++;
								currentMesh->Flags &= ~1;
							}

							k++;
							currentMesh = CollidedMeshes[k];

						} while (currentMesh);
					}
				}
			}

			n++;
			explode = true;
			radius = GRENADE_EXPLODE_RADIUS;
		} while (n < 2);
	}

	if (explode || (item->itemFlags[0] == WEAPON_AMMO3 && explode))
	{
		if (item->itemFlags[0] == WEAPON_AMMO3)
		{
			FlashFader = 32;
			FlashFadeR = 255;
			FlashFadeG = 255;
			FlashFadeB = 255;

			/*
			if ( sub_47F960() )
      {
        FlashFader = 0;
        sub_47F880(item->pos.xPos, item->pos.yPos, item->pos.zPos, 4096, 255, 255, 255, 255, item->roomNumber);
      }
			*/

			GrenadeExplosionEffects(item->pos.xPos, item->pos.yPos, item->pos.zPos, item->roomNumber);
			GrenadeExplosionEffects(item->pos.xPos, item->pos.yPos, item->pos.zPos, item->roomNumber);
		}
		else if (Rooms[item->roomNumber].flags & ENV_FLAG_WATER)
		{
			TriggerUnderwaterExplosion(item);
		}
		else
		{
			item->pos.yPos -= 128;
			TriggerShockwave(&item->pos, 0x1300030, 96, 0x18806000, 0, 0);

			TriggerExplosionSparks(oldX, oldY, oldZ, 3, -2, 0, item->roomNumber);
			for (int x = 0; x < 2; x++)
				TriggerExplosionSparks(oldX, oldY, oldZ, 3, -1, 0, item->roomNumber);
		}

		AlertNearbyGuards(item);
		SoundEffect(105, &item->pos, ENV_FLAG_PITCH_SHIFT | 0x1800000);
		SoundEffect(106, &item->pos, 0);

		if (item->itemFlags[0] != WEAPON_AMMO1 && item->itemFlags[0] != 4)
		{
			item->meshBits = 0;
			item->itemFlags[1] = (item->itemFlags[0] != WEAPON_AMMO2 ? 16 : 4);
			return;
		}

		KillItem(itemNumber);
		return;
	}
}

void __cdecl DrawShotgun(int weaponType)
{
	ITEM_INFO* item;

	if (Lara.weaponItem == NO_ITEM)
	{
		Lara.weaponItem = CreateItem();
		
		item = &Items[Lara.weaponItem];

		item->objectNumber = WeaponObject(weaponType);

		if (weaponType == WEAPON_ROCKET_LAUNCHER)
			item->animNumber = Objects[item->objectNumber].animIndex + 1;
		else if (weaponType == WEAPON_GRENADE_LAUNCHER)
			item->animNumber = Objects[item->objectNumber].animIndex + ROCKET_DRAW_ANIM;
		else
			item->animNumber = Objects[item->objectNumber].animIndex + HARPOON_DRAW_ANIM; // M16 too
		
		item->frameNumber = Anims[item->animNumber].frameBase;
		item->goalAnimState = 1;
		item->currentAnimState = 1;
		item->status = ITEM_ACTIVE;
		item->roomNumber = 255;
		
		Lara.rightArm.frameBase = Objects[item->objectNumber].frameBase;
		Lara.leftArm.frameBase = Lara.rightArm.frameBase;
	}
	else
	{
		item = &Items[Lara.weaponItem];
	}

	AnimateItem(item);

	if (item->currentAnimState != 0 && item->currentAnimState != 6)
	{
		if (item->frameNumber - Anims[item->animNumber].frameBase == Weapons[weaponType].drawFrame)
		{
			draw_shotgun_meshes(weaponType);
		}
		else if (Lara.waterStatus == 1)
		{
			item->goalAnimState = 6;
		}
	}
	else
	{
		ready_shotgun(weaponType);
	}

	Lara.leftArm.frameBase = Lara.rightArm.frameBase = Anims[item->animNumber].framePtr;
	Lara.leftArm.frameNumber = Lara.rightArm.frameNumber = item->frameNumber - Anims[item->animNumber].frameBase;
	Lara.leftArm.animNumber = Lara.rightArm.animNumber = item->animNumber;
}

void __cdecl AnimateShotgun(int weaponType)
{
	bool harpoonFired = false;

	if (HKTimer)
	{
		HKFlag = 0;
		--HKTimer;
	}

	if (SmokeCountL)
	{
		PHD_VECTOR pos;

		if (SmokeWeapon == WEAPON_HK)
		{
			pos.x = 0;
			pos.y = 228;
			pos.z = 96;
		}
		else if (SmokeWeapon == WEAPON_SHOTGUN)
		{
			pos.x = -16;
			pos.y = 228;
			pos.z = 32;
		}
		else if (SmokeWeapon == WEAPON_GRENADE_LAUNCHER)
		{
			pos.x = 0;
			pos.y = 180;
			pos.z = 80;
		}
		else if (SmokeWeapon == WEAPON_ROCKET_LAUNCHER)
		{
			pos.x = 0;
			pos.y = 84;
			pos.z = 72;
		}

		GetLaraJointPosition(&pos, LJ_LOUTARM);

		if (LaraItem->meshBits)
			TriggerGunSmoke(pos.x, pos.y, pos.z, 0, 0, 0, 0, SmokeWeapon, SmokeCountL);
	}

	ITEM_INFO* item = &Items[Lara.weaponItem];
	bool running = (weaponType == WEAPON_HK && LaraItem->speed != 0);

	switch (item->currentAnimState)
	{
	case 0:
		HKFlag = 0;
		HKTimer = 0;
		HKFlag2 = 0;

		if (Lara.waterStatus == 1 || running)
			item->goalAnimState = 6;
		else if ((!(TrInput & IN_ACTION) || Lara.target) && Lara.leftArm.lock == 0)
			item->goalAnimState = 4;
		else
			item->goalAnimState = 2;

		break;

	case 6:
		HKFlag = 0;
		HKTimer = 0;
		HKFlag2 = 0;

		if (Lara.waterStatus == 1 || running)
		{
			if ((!(TrInput & IN_ACTION) || Lara.target) && Lara.leftArm.lock == 0)
				item->goalAnimState = 7;
			else
				item->goalAnimState = 8;
		}
		else
			item->goalAnimState = 0;
		
		break;

	case 2:
		if (item->frameNumber == Anims[item->animNumber].frameBase)
		{
			item->goalAnimState = 4;
			
			if (Lara.waterStatus != 1 && !running && !harpoonFired)
			{
				if ((TrInput & IN_ACTION) && (!Lara.target || Lara.leftArm.lock))
				{
					if (weaponType == WEAPON_HARPOON_GUN)
					{
						FireHarpoon();
						if (!(g_LaraExtra.Weapons[WEAPON_HARPOON_GUN].Ammo[0] & 3)) 
							harpoonFired = true;
					}
					else if (weaponType == WEAPON_ROCKET_LAUNCHER)
					{
						//FireRocket();
					}
					else if (weaponType == WEAPON_GRENADE_LAUNCHER)
					{
						FireGrenade();
					}
					else if (weaponType == WEAPON_CROSSBOW)
					{
						FireCrossbow(NULL);
					}
					else if (weaponType == WEAPON_HK)
					{
						FireHK(0);
						HKFlag = 1;

						if (g_LaraExtra.Weapons[WEAPON_HK].HasSilencer)
						{
							SoundEffect(14, 0, 0);
						}
						else
						{
							SoundEffect(105, &LaraItem->pos, 83888140);
							SoundEffect(68, &LaraItem->pos, 0);
						}
					}
					else
						FireShotgun();

					item->goalAnimState = 2;
				}
				else if (Lara.leftArm.lock)
					item->goalAnimState = 0; 
			}

			if (item->goalAnimState != 2 && HKFlag && !(g_LaraExtra.Weapons[WEAPON_HK].HasSilencer))
			{
				StopSoundEffect(68);
				SoundEffect(69, &LaraItem->pos, 0);
				HKFlag = 0;
			}
		}
		else if (HKFlag)
		{
			if (g_LaraExtra.Weapons[WEAPON_HK].HasSilencer)
			{
				SoundEffect(14, 0, 0);
			}
			else
			{
				SoundEffect(105, &LaraItem->pos, 83888140);
				SoundEffect(68, &LaraItem->pos, 0);
			}
		}
		else if (weaponType == WEAPON_SHOTGUN && !(TrInput & IN_ACTION) && !Lara.leftArm.lock)
		{
			item->goalAnimState = 4;
		}

		if (item->frameNumber - Anims[item->animNumber].frameBase == 12 && weaponType == WEAPON_SHOTGUN)
			TriggerGunShell(1, 370, 4);
		break;

	case 8:
		if (item->frameNumber - Anims[item->animNumber].frameBase == 0)
		{
			item->goalAnimState = 7;

			if ((Lara.waterStatus == 1 || running) && !harpoonFired)
			{
				if ((TrInput & IN_ACTION) && (!Lara.target || Lara.leftArm.lock))
				{
					if (weaponType == WEAPON_HARPOON_GUN)
					{
						FireHarpoon();
						if (!(g_LaraExtra.Weapons[WEAPON_HARPOON_GUN].Ammo[0] & 3))
							harpoonFired = true;
					}
					else if (weaponType == WEAPON_HK && (/*!(Lara.HKtypeCarried & 0x18) || */!HKTimer))
					{
						FireHK(1);
						HKFlag = 1;
						item->goalAnimState = 8;
						if (g_LaraExtra.Weapons[WEAPON_HK].HasSilencer)
						{
							SoundEffect(14, 0, 0);
						}
						else
						{
							SoundEffect(105, &LaraItem->pos, 83888140);
							SoundEffect(68, &LaraItem->pos, 0);
						}
					}
					else
					{
						item->goalAnimState = 6;
					}

					item->goalAnimState = 8;
				}
				else if (Lara.leftArm.lock)
					item->goalAnimState = 6;
			}
			else if (item->goalAnimState != 8 && HKFlag && !(g_LaraExtra.Weapons[WEAPON_HK].HasSilencer))
			{
				StopSoundEffect(68);
				SoundEffect(69, &LaraItem->pos, 0);
				HKFlag = 0;
			}
			else if (HKFlag)
			{
				if (g_LaraExtra.Weapons[WEAPON_HK].HasSilencer)
				{
					SoundEffect(14, 0, 0);
				}
				else
				{
					SoundEffect(105, &LaraItem->pos, 83888140);
					SoundEffect(68, &LaraItem->pos, 0);
				}
			}
		}		

		break;

	default:
		break;
	}

	AnimateItem(item);

	Lara.leftArm.frameBase = Lara.rightArm.frameBase = Anims[item->animNumber].framePtr;
	Lara.leftArm.frameNumber = Lara.rightArm.frameNumber = item->frameNumber - Anims[item->animNumber].frameBase;
	Lara.leftArm.animNumber = Lara.rightArm.animNumber = item->animNumber;
}

void __cdecl ControlCrossbowBolt(short itemNumber)
{
	ITEM_INFO* item = &Items[itemNumber];

	int oldX = item->pos.xPos;
	int oldY = item->pos.yPos;
	int oldZ = item->pos.zPos;
	short roomNumber = item->roomNumber;

	bool land = false;
	bool explode = false;

	if (Rooms[roomNumber].flags & ENV_FLAG_WATER)
	{
		if (item->speed > 64)
			item->speed -= (item->speed >> 4);
		if (GlobalCounter & 1)
			CreateBubble(&item->pos, roomNumber, 4, 7);
	}
	else
	{
		land = true;
	}

	item->pos.xPos += ((item->speed * COS(item->pos.xRot) >> W2V_SHIFT) * SIN(item->pos.yRot)) >> W2V_SHIFT;
	item->pos.yPos += item->speed * SIN(-item->pos.xRot) >> W2V_SHIFT;
	item->pos.zPos += ((item->speed * COS(item->pos.xRot) >> W2V_SHIFT) * COS(item->pos.yRot)) >> W2V_SHIFT;

	roomNumber = item->roomNumber;
	FLOOR_INFO* floor = GetFloor(item->pos.xPos, item->pos.yPos, item->pos.zPos, &roomNumber);
	
	if (GetFloorHeight(floor, item->pos.xPos, item->pos.yPos, item->pos.zPos) < item->pos.yPos ||
		GetCeiling(floor, item->pos.xPos, item->pos.yPos, item->pos.zPos) > item->pos.yPos)
	{
		// I have hit the room, this is the end for the bolt
		item->pos.xPos = oldX;
		item->pos.yPos = oldY;
		item->pos.zPos = oldZ;

		// If ammos are normal, then just shatter the bolt and quit
		if (item->itemFlags[0] != WEAPON_AMMO3)
		{
			ExplodeItemNode(item, 0, 0, 256);
			KillItem(itemNumber);
			return;
		}

		// Otherwise, bolt must explode
		explode = true;
	}

	// Has bolt changed room?
	if (item->roomNumber != roomNumber)
		ItemNewRoom(itemNumber, roomNumber);

	// If now in water and before in land, add a ripple
	if ((Rooms[item->roomNumber].flags & ENV_FLAG_WATER) && land)
	{
		SetupRipple(item->pos.xPos, Rooms[item->roomNumber].minfloor, item->pos.zPos, (GetRandomControl() & 7) + 8, 0);
	}

	int radius = (explode ? CROSSBOW_EXPLODE_RADIUS : CROSSBOW_HIT_RADIUS);

	int n = 0;
	bool foundCollidedObjects = false;

	do
	{
		// Found possible collided items and statics
		GetCollidedObjects(item, radius, 1, &CollidedItems[0], &CollidedMeshes[0], 1);
		
		// If no collided items and meshes are found, then exit the loop
		if (!CollidedItems[0] && !CollidedMeshes[0])
			break;

		foundCollidedObjects = true;

		if (item->itemFlags[0] != WEAPON_AMMO3 || explode)
		{
			if (CollidedItems[0])
			{
				ITEM_INFO* currentItem = CollidedItems[0];
				int k = 0;

				do
				{
					if (explode)
					{
						// Item is inside the radius and must explode
						if (item->objectNumber < ID_SMASH_OBJECT1 || item->objectNumber > ID_SMASH_OBJECT8)
						{
							if (currentItem->objectNumber == ID_SHOOT_SWITCH1 || currentItem->objectNumber == ID_SHOOT_SWITCH2)
								CrossbowHitSwitchType78(item, currentItem, 0);
							else if (Objects[item->objectNumber].hitEffect)
								DoGrenadeDamageOnBaddie(currentItem, item);
						}
						else
						{
							TriggerExplosionSparks(currentItem->pos.xPos, currentItem->pos.yPos, currentItem->pos.zPos, 3, -2, 0, currentItem->roomNumber);
							currentItem->pos.yPos -= 128;
							TriggerShockwave(&currentItem->pos, 19922992, 96, 411066368, 0, 0);
							currentItem->pos.yPos += 128;
							ExplodeItemNode(currentItem, 0, 0, 128);
							short currentItemNumber = (currentItem - CollidedItems[0]) / sizeof(ITEM_INFO);
							SmashObject(currentItemNumber);
							KillItem(currentItemNumber);
						}
					}
					else if (currentItem->objectNumber == ID_SHOOT_SWITCH1 || currentItem->objectNumber == ID_SHOOT_SWITCH2)
					{
						// Special case for ID_SWITCH_TYPE7 and ID_SWITCH_TYPE8
						CrossbowHitSwitchType78(item, currentItem, 1);
					}
					else if (Objects[currentItem->objectNumber].hitEffect)
					{
						HitTarget(currentItem, (GAME_VECTOR*)&item->pos, Weapons[WEAPON_CROSSBOW].damage, 0);

						// Poisoned ammos
						if (item->itemFlags[0] == WEAPON_AMMO2 && !Objects[currentItem->objectNumber].explodableMeshbits)
							item->poisoned = true;
					}

					k++;
					currentItem = CollidedItems[k];

				} while (currentItem);
			}

			if (CollidedMeshes[0])
			{
				MESH_INFO* currentMesh = CollidedMeshes[0];
				int k = 0;

				do
				{
					if (currentMesh->staticNumber >= 50 && currentMesh->staticNumber < 58)
					{
						if (explode)
						{
							TriggerExplosionSparks(currentMesh->x, currentMesh->y, currentMesh->z, 3, -2, 0, item->roomNumber);
							currentMesh->y -= 128;
							TriggerShockwave((PHD_3DPOS*)&currentMesh, 0xB00028, 64, 0x10806000, 0, 0);
							currentMesh->y += 128;
						}
						ShatterObject((SHATTER_ITEM*)item, NULL, -128, item->roomNumber, 0);
						SmashedMeshRoom[SmashedMeshCount] = item->roomNumber;
						SmashedMesh[SmashedMeshCount] = currentMesh;
						SmashedMeshCount++;
						currentMesh->Flags &= ~1;
					}

					k++;
					currentMesh = CollidedMeshes[k];

				} while (currentMesh);
			}

			break;
		}

		n++;
		explode = true;
		radius = CROSSBOW_EXPLODE_RADIUS;
	} while (n < 2);
		
	if (!explode)
	{
		if (foundCollidedObjects)
		{
			ExplodeItemNode(item, 0, 0, 256);
			KillItem(itemNumber);
		}
		return;
	}

	// At this point, for sure bolt must explode

	if (Rooms[item->roomNumber].flags & ENV_FLAG_WATER)
		TriggerUnderwaterExplosion(item);
	else
	{
		TriggerShockwave(&item->pos, 19922992, 96, 411066368, 0, 0);
		item->pos.yPos += 128;
		TriggerExplosionSparks(oldX, oldY, oldZ, 3, -2, 0, item->roomNumber);
		for (int j = 0; j < 2; j++)
			TriggerExplosionSparks(oldX, oldY, oldZ, 3, -1, 0, item->roomNumber);
	}

	AlertNearbyGuards(item);
	
	SoundEffect(105, &item->pos, 0x1800004);
	SoundEffect(106, &item->pos, 0);

	if (foundCollidedObjects || explode)
	{
		ExplodeItemNode(item, 0, 0, 256);
		KillItem(itemNumber);
	}

	return;
}

void __cdecl RifleHandler(int weaponType)
{
	WEAPON_INFO* weapon = &Weapons[weaponType];

	LaraGetNewTarget(weapon);

	if (TrInput & IN_ACTION)
		LaraTargetInfo(weapon);

	AimWeapon(weapon, &Lara.leftArm);

	if (Lara.leftArm.lock)
	{
		Lara.torsoXrot = Lara.leftArm.xRot;
		Lara.torsoYrot = Lara.leftArm.yRot;
		if (Camera.oldType != CAMERA_TYPE::LOOK_CAMERA && !BinocularRange)
		{
			Lara.headYrot = 0;
			Lara.headXrot = 0;
		}
	}

	if (weaponType == WEAPON_REVOLVER)
		AnimatePistols(WEAPON_REVOLVER);
	else
		AnimateShotgun(weaponType);

	if (Lara.rightArm.flash_gun)
	{
		if (weaponType == WEAPON_SHOTGUN || weaponType == WEAPON_HK)
		{
			TriggerDynamicLight(
				LaraItem->pos.xPos + (SIN(LaraItem->pos.yRot) >> 4) + GetRandomControl() - 128,
				LaraItem->pos.yPos + (GetRandomControl() & 0x7F) - 575,
				LaraItem->pos.zPos + (COS(LaraItem->pos.yRot) >> 4) + GetRandomControl() - 128,
				12,
				(GetRandomControl() & 0x3F) + 192,
				(GetRandomControl() & 0x1F) + 128,
				GetRandomControl() & 0x3F
			);
		}
		else if (weaponType == WEAPON_REVOLVER)
		{
			PHD_VECTOR pos;

			pos.x = GetRandomControl() - 128;
			pos.y = (GetRandomControl() & 0x7F) - 63;
			pos.z = GetRandomControl() - 128;

			GetLaraJointPosition(&pos, LJ_RHAND);

			TriggerDynamicLight(pos.x, pos.y, pos.z, 12,
				(GetRandomControl() & 0x3F) + 192,
				(GetRandomControl() & 0x1F) + 128,
				(GetRandomControl() & 0x3F));
		}
	}
}

void __cdecl FireCrossbow(PHD_3DPOS* pos)
{
	short* ammos = GetAmmo(WEAPON_CROSSBOW);
	if (*ammos <= 0)
		return;

	Lara.hasFired = true;
	
	short itemNumber = CreateItem();
	if (itemNumber != NO_ITEM)
	{
		ITEM_INFO* item = &Items[itemNumber];
		item->objectNumber = ID_CROSSBOW_BOLT;
		item->shade = 0xC210;
		if (pos)
		{
			item->roomNumber = LaraItem->roomNumber;
			item->pos.xPos = pos->xPos;
			item->pos.yPos = pos->yPos;
			item->pos.zPos = pos->zPos;

			InitialiseItem(itemNumber);

			item->pos.xRot = pos->xRot;
			item->pos.yRot = pos->yRot;
			item->pos.zRot = pos->zRot;
		}
		else
		{
			if (*ammos != -1)
				(*ammos)--;

			PHD_VECTOR jointPos;
			jointPos.x = 0;
			jointPos.y = 228;
			jointPos.z = 32;

			GetLaraJointPosition(&jointPos, LJ_RHAND);

			item->roomNumber = LaraItem->roomNumber;
			
			FLOOR_INFO* floor = GetFloor(jointPos.x, jointPos.y, jointPos.z, &item->roomNumber);
			int height = GetFloorHeight(floor, jointPos.x, jointPos.y, jointPos.z);

			if (height >= jointPos.y)
			{
				item->pos.xPos = jointPos.x;
				item->pos.yPos = jointPos.y;
				item->pos.zPos = jointPos.z;
			}
			else
			{
				item->pos.xPos = LaraItem->pos.xPos;
				item->pos.yPos = jointPos.y;
				item->pos.zPos = LaraItem->pos.zPos;
				item->roomNumber = LaraItem->roomNumber;
			}

			InitialiseItem(itemNumber);

			item->pos.xRot = Lara.leftArm.xRot + LaraItem->pos.xRot;
			item->pos.zRot = 0;
			item->pos.yRot = Lara.leftArm.yRot + LaraItem->pos.yRot;

			if (!Lara.leftArm.lock)
			{
				item->pos.xRot += Lara.torsoXrot;
				item->pos.yRot += Lara.torsoYrot;
			}
		}

		item->speed = 512;

		AddActiveItem(itemNumber);
		
		item->itemFlags[0] = g_LaraExtra.Weapons[WEAPON_CROSSBOW].SelectedAmmo;

		SoundEffect(235, 0, 0);

		Savegame.Level.AmmoUsed++;
		Savegame.Game.AmmoUsed++;
	}
}

void __cdecl DoGrenadeDamageOnBaddie(ITEM_INFO* dest, ITEM_INFO* src)
{
	if (!(dest->flags & 0x8000))
	{
		if (dest != LaraItem || LaraItem->hitPoints <= 0)
		{
			if (!src->itemFlags[2])
			{
				dest->hitStatus = true;

				OBJECT_INFO* obj = &Objects[dest->objectNumber];
				if (!obj->undead)
				{
					HitTarget(dest, 0, 30, 1);
					if (dest != LaraItem)
					{
						++Savegame.Game.AmmoHits;
						if (src->hitPoints <= 0)
						{
							++Savegame.Level.Kills;
							CreatureDie((dest - Items) / sizeof(ITEM_INFO), 1);
						}
					}
				}
			}
		}
		else
		{
			LaraItem->hitPoints -= 50;
			if (!(Rooms[dest->roomNumber].flags & ENV_FLAG_WATER) && LaraItem->hitPoints <= 50)
				LaraBurn();
		}
	}
}

void __cdecl TriggerUnderwaterExplosion(ITEM_INFO* item)
{
	TriggerExplosionBubbles(item->pos.xPos, item->pos.yPos, item->pos.zPos, item->roomNumber);
	TriggerExplosionSparks(item->pos.xPos, item->pos.yPos, item->pos.zPos, 2, -2, 1, item->roomNumber);
	
	for (int i = 0; i < 3; i++)
	{
		TriggerExplosionSparks(item->pos.xPos, item->pos.yPos, item->pos.zPos, 2, -1, 1, item->roomNumber);
	}
	
	int wh = GetWaterHeight(item->pos.xPos, item->pos.yPos, item->pos.zPos, item->roomNumber);
	if (wh != NO_HEIGHT)
	{
		int dy = item->pos.yPos - wh;
		if (dy < 2048)
		{
			SplashSetup.y = wh;
			SplashSetup.x = item->pos.xPos;
			SplashSetup.z = item->pos.zPos;
			SplashSetup.innerRadVel = 160;
			SplashSetup.middleRadVel = 224;
			SplashSetup.outerRadVel = 272;
			SplashSetup.innerRad = (2048 - dy) >> 6 + 16;
			SplashSetup.innerSize = (2048 - dy) >> 6 + 12;
			SplashSetup.innerYVel = 8 * (-512 - (2048 - dy));
			SplashSetup.middleRad = (2048 - dy) >> 6 + 24;
			SplashSetup.middleSize = (2048 - dy) >> 6 + 24;
			SplashSetup.middleYVel = 4 * (-768 - (2048 - dy));
			SplashSetup.outerRad = (2048 - dy) >> 6 + 32;
			SplashSetup.outerSize = (2048 - dy) >> 6 + 32;
			
			SetupSplash(&SplashSetup);
		}
	}
}

void __cdecl undraw_shotgun(int weapon)
{
	ITEM_INFO* item = &Items[Lara.weaponItem];
	item->goalAnimState = 3;
	
	AnimateItem(item);

	if (item->status == ITEM_DEACTIVATED)
	{
		Lara.gunStatus = LG_NO_ARMS;
		Lara.target = NULL;
		Lara.rightArm.lock = 0;
		Lara.leftArm.lock = 0;
		
		KillItem(Lara.weaponItem);

		Lara.weaponItem = NO_ITEM;
		Lara.rightArm.frameNumber = 0;
		Lara.leftArm.frameNumber = 0;
	}
	else if (item->currentAnimState == 3 && item->frameNumber - Anims[item->animNumber].frameBase == 21)
	{
		undraw_shotgun_meshes(weapon);
	}
	
	Lara.rightArm.frameBase = Anims[item->animNumber].framePtr;
	Lara.leftArm.frameBase = Anims[item->animNumber].framePtr;
	Lara.rightArm.frameNumber = item->frameNumber - Anims[item->animNumber].frameBase;
	Lara.leftArm.frameNumber = item->frameNumber - Anims[item->animNumber].frameBase;
	Lara.rightArm.animNumber = item->animNumber;
	Lara.leftArm.animNumber = Lara.rightArm.animNumber;
}

void __cdecl undraw_shotgun_meshes(int weapon)
{
	short objectNumber = WeaponObject(weapon);
	Lara.backGun = objectNumber;
	Lara.meshPtrs[10] = Meshes[Objects[0].meshIndex + 20];
}

void __cdecl draw_shotgun(int weapon)
{
	ITEM_INFO* item;

	if (Lara.weaponItem == NO_ITEM)
	{
		Lara.weaponItem = CreateItem();
		item = &Items[Lara.weaponItem];
		item->objectNumber = WeaponObject(weapon);
		item->animNumber = Objects[item->objectNumber].animIndex;
		item->frameNumber = Anims[item->animNumber].frameBase;
		item->goalAnimState = 1;
		item->currentAnimState = 1;
		item->status = ITEM_ACTIVE;
		item->roomNumber = 255;
		Lara.rightArm.frameBase = Objects[item->objectNumber].frameBase;
		Lara.leftArm.frameBase = Lara.rightArm.frameBase;
	}
	else
	{
		item = &Items[Lara.weaponItem];
	}

	AnimateItem(item);
	
	if (item->currentAnimState && item->currentAnimState != 6)
	{
		if (item->frameNumber - Anims[item->animNumber].frameBase == Weapons[weapon].drawFrame)
		{
			draw_shotgun_meshes(weapon);
		}
		else if (Lara.waterStatus == LW_UNDERWATER)
		{
			item->goalAnimState = 6;
		}
	}
	else
	{
		ready_shotgun(weapon);
	}

	Lara.rightArm.frameBase = Anims[item->animNumber].framePtr;
	Lara.leftArm.frameBase = Anims[item->animNumber].framePtr;
	Lara.rightArm.frameNumber = item->frameNumber - Anims[item->animNumber].frameBase;
	Lara.leftArm.frameNumber = item->frameNumber - Anims[item->animNumber].frameBase;
	Lara.rightArm.animNumber = item->animNumber;
	Lara.leftArm.animNumber = Lara.rightArm.animNumber;
}

void __cdecl draw_shotgun_meshes(int weaponType)
{
	Lara.backGun = 0;
	Lara.meshPtrs[HAND_R] = Meshes[Objects[WeaponObjectMesh(weaponType)].meshIndex + HAND_R];
}

void __cdecl CrossbowHitSwitchType78(ITEM_INFO* item1, ITEM_INFO* item2, signed int search)
{
	/*v4 = item2;
	if (!(item2->flags & 0x40))
	{
		if (search)
		{
			int numSpheres = GetSpheres(item2, SphereList, 1);
			int best = -1;
			int bestDistance = 0x7FFFFFFF;

			for (int i = 0; i < numSpheres; i++)
			{
				SPHERE* sphere = &SphereList[i];

				int dx = sphere->x - item1->pos.xPos;
				int dy = sphere->y - item1->pos.yPos;
				int dz = sphere->z - item1->pos.zPos;

				if (SQUARE(dx) + SQUARE(dy) + SQUARE(dz) - SQUARE(sphere->r) < bestDistance)
				{
					bestDistance = SQUARE(dx) + SQUARE(dy) + SQUARE(dz) - SQUARE(sphere->r);
					best = i;
				}
			}

			if (numSpheres > 0)
			{
				v7 = &Slist[0].z;
				v8 = item1->pos.yPos;
				v9 = item1->pos.zPos;
				item1a = item1->pos.xPos;
				v20 = v8;
				v21 = v9;
				do
				{
					r = v7[1];
					a1 = *(v7 - 2) - item1a;
					v11 = *(v7 - 1) - v20;
					if (dx2 + dy2 + dz2 - r2 < search)
					{
						search = (dx2 + dy2 + dz2 - r2;
						best = item2;
					}
					v5 = v22;
					v7 += 4;
					item2 = (item2 + 1);
				} while (item2 < v22);
				best = best;
			}
		}
		else
		{
			v5 = Objects[item2->objectNumber].nmeshes;
			v6 = v5 - 1;
		}
		if (v6 == v5 - 1)
		{
			if (v4->flags & 0x3E00 && (item2->flags & 0x3E00) != 15872)
			{
				short roomNumber = item2->roomNumber;
				FLOOR_INFO* floor = GetFloor(item2->pos.xPos, item2->pos.yPos - 256, item2->pos.zPos, &roomNumber);
				GetFloorHeight(floor, item2->pos.xPos, item2->pos.yPos - 256, item2->pos.zPos);
				TestTriggers(TriggerIndex, 1, item2->flags & 0x3E00);
			}
			else
			{
				v14 = GetSwitchTrigger(v4, &ItemNos, 1);
				if (v14 > 0)
				{
					v15 = v14;
					v16 = (&v22 + 2 * v14 + 2);
					do
					{
						AddActiveItem(*v16);
						v17 = *v16;
						--v16;
						Items[v17]._bf15ea = Items[v17]._bf15ea & 0xFFFFFFFB | 2;
						HIBYTE(Items[v16[1]].flags) |= 0x3Eu;
						--v15;
					} while (v15);
				}
			}
			
			if (item2->objectNumber == ID_SHOOT_SWITCH1)
				ExplodeItemNode(item2, Objects[ID_SHOOT_SWITCH1].nmeshes - 1, 0, 64);
			
			AddActiveItem((item2 - Items) / sizeof(ITEM_INFO));
			item2->flags |= 0x3E40;
			item2->status = ITEM_ACTIVE;
		}
	}*/
}

void __cdecl FireHK(int mode)
{
	if (g_LaraExtra.Weapons[WEAPON_HK].SelectedAmmo == WEAPON_AMMO1)
	{
		HKTimer = 12;
	}
	else if (g_LaraExtra.Weapons[WEAPON_HK].SelectedAmmo == WEAPON_AMMO2)
	{
		HKCounter++;
		if (HKCounter == 5)
		{
			HKCounter = 0;
			HKTimer = 12;
		}
	}

	short angles[2];

	angles[1] = Lara.leftArm.xRot;
	angles[0] = Lara.leftArm.yRot + LaraItem->pos.yRot;

	if (!Lara.leftArm.lock)
	{
		angles[0] = Lara.torsoYrot + Lara.leftArm.yRot + LaraItem->pos.yRot;
		angles[1] = Lara.torsoXrot + Lara.leftArm.xRot;
	}

	if (mode)
	{
		Weapons[WEAPON_HK].shotAccuracy = 2184;
		Weapons[WEAPON_HK].damage = 1;
	}
	else
	{
		Weapons[WEAPON_HK].shotAccuracy = 728;
		Weapons[WEAPON_HK].damage = 3;
	}

	if (FireWeapon(WEAPON_HK, Lara.target, LaraItem, angles))
	{
		SmokeCountL = 12;
		SmokeWeapon = WEAPON_HK;
		TriggerGunShell(1, ID_GUNSHELL, 5);
		Lara.rightArm.flash_gun = Weapons[WEAPON_HK].flashTime;
	}
}

void __cdecl FireShotgun()
{
	short angles[2];
	
	angles[1] = Lara.leftArm.xRot;
	angles[0] = Lara.leftArm.yRot + LaraItem->pos.yRot;

	if (!Lara.leftArm.lock)
	{
		angles[0] = Lara.torsoYrot + Lara.leftArm.yRot + LaraItem->pos.yRot;
		angles[1] = Lara.torsoXrot + Lara.leftArm.xRot;
	}

	short loopAngles[2];
	bool fired = false;
	int value = (g_LaraExtra.Weapons[WEAPON_SHOTGUN].SelectedAmmo == WEAPON_AMMO1 ? 1820 : 5460);

	for (int i = 0; i < 6; i++)
	{
		loopAngles[0] = angles[0] + value * (GetRandomControl() - 0x4000) / 0x10000;
		loopAngles[1] = angles[1] + value * (GetRandomControl() - 0x4000) / 0x10000;

		if (FireWeapon(WEAPON_SHOTGUN, Lara.target, LaraItem, loopAngles))
			fired = true;
	}

	if (fired)
	{
		PHD_VECTOR pos;

		pos.x = 0;
		pos.y = 228;
		pos.z = 32;

		GetLaraJointPosition(&pos, LJ_RHAND);

		PHD_VECTOR pos2;

		pos2.x = pos.x;
		pos2.y = pos.y;
		pos2.z = pos.z;

		pos.x = 0;
		pos.y = 1508;
		pos.z = 32;

		GetLaraJointPosition(&pos, LJ_RHAND);

		SmokeCountL = 32;
		SmokeWeapon = WEAPON_SHOTGUN;

		if (LaraItem->meshBits)
		{
			for (int i = 0; i < 7; i++)
			{
				TriggerGunSmoke(pos2.x, pos2.y, pos2.z, pos.x - pos2.x, pos.y - pos2.y, pos.z - pos2.z, 1, SmokeWeapon, 32);
			}
		}

		Lara.rightArm.flash_gun = Weapons[WEAPON_SHOTGUN].flashTime;
		
		SoundEffect(SFX_EXPLOSION1, &LaraItem->pos, 20971524);
		SoundEffect(Weapons[WEAPON_SHOTGUN].sampleNum, &LaraItem->pos, 0);
		
		Savegame.Game.AmmoUsed++;
	}
}

void __cdecl ready_shotgun(int weaponType)
{
	Lara.gunStatus = LG_READY;
	Lara.leftArm.zRot = 0;
	Lara.leftArm.yRot = 0;
	Lara.leftArm.xRot = 0;
	Lara.rightArm.zRot = 0;
	Lara.rightArm.yRot = 0;
	Lara.rightArm.xRot = 0;
	Lara.rightArm.frameNumber = 0;
	Lara.leftArm.frameNumber = 0;
	Lara.rightArm.lock = 0;
	Lara.leftArm.lock = 0;
	Lara.target = NULL;
	Lara.rightArm.frameBase = Objects[WeaponObject(weaponType)].frameBase;
	Lara.leftArm.frameBase = Objects[WeaponObject(weaponType)].frameBase;
}

void __cdecl Inject_Lara1Gun()
{
	
}