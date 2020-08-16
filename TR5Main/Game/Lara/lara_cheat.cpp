#include "framework.h"
#include "lara.h"
#include "lara_cheat.h"
#include "input.h"
#include "GameFlowScript.h"
#include "inventory.h"
#include "effect2.h"


extern GameFlow* g_GameFlow;
short cheatHitPoints;


void lara_as_swimcheat(ITEM_INFO* item, COLL_INFO* coll)//4C3A8, 4C80C (F)
{
	if (TrInput & IN_FORWARD)
	{
		item->pos.xRot -= ANGLE(3);
	}
	else if (TrInput & IN_BACK)
	{
		item->pos.xRot += ANGLE(3);
	}

	if (TrInput & IN_LEFT)
	{
		Lara.turnRate -= 613;

		if (Lara.turnRate < -ANGLE(6))
			Lara.turnRate = -ANGLE(6);
	}
	else if (TrInput & IN_RIGHT)
	{
		Lara.turnRate += 613;

		if (Lara.turnRate > ANGLE(6))
			Lara.turnRate = ANGLE(6);
	}

	if (TrInput & IN_ACTION)
	{
		TriggerDynamicLight(item->pos.xPos, item->pos.yPos, item->pos.zPos, 31, 255, 255, 255);
	}

	if (TrInput & IN_OPTION)
	{
		Lara.turnRate = -ANGLE(12);
	}

	if (TrInput & IN_JUMP)
	{
		item->fallspeed += 16;

		if (item->fallspeed > 400)
			item->fallspeed = 400;
	}
	else
	{
		if (item->fallspeed >= 8)
			item->fallspeed -= item->fallspeed >> 3;
		else
			item->fallspeed = 0;
	}
}

void LaraCheatyBits() // (F) (D)
{
	if (g_GameFlow->FlyCheat)
	{
#ifdef _DEBUG
		if (TrInput & IN_PAUSE)
#else
		if (TrInput & IN_D)
#endif
		{
			LaraCheatGetStuff();
			LaraItem->hitPoints = 1000;
		}

#ifdef _DEBUG
		if (TrInput & IN_PAUSE)
#else
		if (TrInput & IN_CHEAT)
#endif
		{
			DelsGiveLaraItemsCheat();
			LaraItem->pos.yPos -= 128;
			if (Lara.waterStatus != LW_FLYCHEAT)
			{
				Lara.waterStatus = LW_FLYCHEAT;
				LaraItem->animNumber = LA_DOZY;
				LaraItem->frameNumber = g_Level.Anims[LaraItem->animNumber].frameBase;
				LaraItem->currentAnimState = LA_ONWATER_IDLE_TO_SWIM;
				LaraItem->goalAnimState = LA_ONWATER_IDLE_TO_SWIM;
				LaraItem->gravityStatus = false;
				LaraItem->pos.xRot = ANGLE(30);
				LaraItem->fallspeed = 30;
				Lara.air = 1800;
				Lara.deathCount = 0;
				Lara.torsoYrot = 0;
				Lara.torsoXrot = 0;
				Lara.headYrot = 0;
				Lara.headXrot = 0;
				cheatHitPoints = LaraItem->hitPoints;
			}
		}
	}
}

void LaraCheatGetStuff() // (F) (D)
{
	Lara.NumFlares = -1;
	Lara.NumSmallMedipacks = -1;
	Lara.NumLargeMedipacks = -1;

	if (Objects[ID_CROWBAR_ITEM].loaded)
		Lara.Crowbar = true;

	if (Objects[ID_LASERSIGHT_ITEM].loaded)
		Lara.Lasersight = true;

	if (Objects[ID_REVOLVER_ITEM].loaded)
	{
		Lara.Weapons[WEAPON_REVOLVER].Present = true;
		Lara.Weapons[WEAPON_REVOLVER].SelectedAmmo = WEAPON_AMMO1;
		Lara.Weapons[WEAPON_REVOLVER].HasLasersight = false;
		Lara.Weapons[WEAPON_REVOLVER].HasSilencer = false;
		Lara.Weapons[WEAPON_REVOLVER].Ammo[WEAPON_AMMO1] = -1;
	}

	if (Objects[ID_UZI_ITEM].loaded)
	{
		Lara.Weapons[WEAPON_UZI].Present = true;
		Lara.Weapons[WEAPON_UZI].SelectedAmmo = WEAPON_AMMO1;
		Lara.Weapons[WEAPON_UZI].HasLasersight = false;
		Lara.Weapons[WEAPON_UZI].HasSilencer = false;
		Lara.Weapons[WEAPON_UZI].Ammo[WEAPON_AMMO1] = -1;
	}

	if (Objects[ID_SHOTGUN_ITEM].loaded)
	{
		Lara.Weapons[WEAPON_SHOTGUN].Present = true;
		Lara.Weapons[WEAPON_SHOTGUN].SelectedAmmo = WEAPON_AMMO1;
		Lara.Weapons[WEAPON_SHOTGUN].HasLasersight = false;
		Lara.Weapons[WEAPON_SHOTGUN].HasSilencer = false;
		Lara.Weapons[WEAPON_SHOTGUN].Ammo[WEAPON_AMMO1] = -1;
	}

	/*	if (Objects[ID_HARPOON_ITEM].loaded)
		{
			Lara.Weapons[WEAPON_HARPOON_GUN].Present = true;
			Lara.Weapons[WEAPON_HARPOON_GUN].SelectedAmmo = WEAPON_AMMO1;
			Lara.Weapons[WEAPON_HARPOON_GUN].HasLasersight = false;
			Lara.Weapons[WEAPON_HARPOON_GUN].HasSilencer = false;
			Lara.Weapons[WEAPON_HARPOON_GUN].Ammo[WEAPON_AMMO1] = -1;
		}

		if (Objects[ID_GRENADE_GUN_ITEM].loaded)
		{
			Lara.Weapons[WEAPON_GRENADE_LAUNCHER].Present = true;
			Lara.Weapons[WEAPON_GRENADE_LAUNCHER].SelectedAmmo = WEAPON_AMMO1;
			Lara.Weapons[WEAPON_GRENADE_LAUNCHER].HasSilencer = false;
			Lara.Weapons[WEAPON_GRENADE_LAUNCHER].Ammo[WEAPON_AMMO1] = -1;
			Lara.Weapons[WEAPON_GRENADE_LAUNCHER].Ammo[WEAPON_AMMO2] = -1;
			Lara.Weapons[WEAPON_GRENADE_LAUNCHER].Ammo[WEAPON_AMMO3] = -1;
		}

		if (Objects[ID_ROCKET_LAUNCHER_ITEM].loaded)
		{
			Lara.Weapons[WEAPON_ROCKET_LAUNCHER].Present = true;
			Lara.Weapons[WEAPON_ROCKET_LAUNCHER].SelectedAmmo = WEAPON_AMMO1;
			Lara.Weapons[WEAPON_ROCKET_LAUNCHER].HasLasersight = false;
			Lara.Weapons[WEAPON_ROCKET_LAUNCHER].HasSilencer = false;
			Lara.Weapons[WEAPON_ROCKET_LAUNCHER].Ammo[WEAPON_AMMO1] = -1;
		}*/

	if (Objects[ID_HK_ITEM].loaded)
	{
		Lara.Weapons[WEAPON_HK].Present = true;
		Lara.Weapons[WEAPON_HK].SelectedAmmo = WEAPON_AMMO1;
		Lara.Weapons[WEAPON_HK].HasLasersight = false;
		Lara.Weapons[WEAPON_HK].HasSilencer = false;
		Lara.Weapons[WEAPON_HK].Ammo[WEAPON_AMMO1] = -1;
	}

	/*	if (Objects[ID_CROSSBOW_ITEM].loaded)
		{
			Lara.Weapons[ID_CROSSBOW_ITEM].Present = true;
			Lara.Weapons[ID_CROSSBOW_ITEM].SelectedAmmo = WEAPON_AMMO1;
			Lara.Weapons[ID_CROSSBOW_ITEM].HasLasersight = false;
			Lara.Weapons[ID_CROSSBOW_ITEM].HasSilencer = false;
			Lara.Weapons[ID_CROSSBOW_ITEM].Ammo[WEAPON_AMMO1] = -1;
			Lara.Weapons[ID_CROSSBOW_ITEM].Ammo[WEAPON_AMMO2] = -1;
			Lara.Weapons[ID_CROSSBOW_ITEM].Ammo[WEAPON_AMMO3] = -1;
		}*/

		/*Commented out the blocks for weapons that don't work ingame, after they're fixed, it'll be okay to uncomment*/

	g_Inventory.LoadObjects(false);
}

void DelsGiveLaraItemsCheat() // (AF) (D)
{
	int i;

	for (i = 0; i < 8; ++i)
	{
		if (Objects[ID_PUZZLE_ITEM1 + i].loaded)
			Lara.Puzzles[i] = 1;
		Lara.PuzzlesCombo[2 * i] = false;
		Lara.PuzzlesCombo[2 * i + 1] = false;
	}
	for (i = 0; i < 8; ++i)
	{
		if (Objects[ID_KEY_ITEM1 + i].loaded)
			Lara.Keys[i] = 1;
		Lara.KeysCombo[2 * i] = false;
		Lara.KeysCombo[2 * i + 1] = false;
	}
	for (i = 0; i < 3; ++i)
	{
		if (Objects[ID_PICKUP_ITEM1 + i].loaded)
			Lara.Pickups[i] = 1;
		Lara.PickupsCombo[2 * i] = false;
		Lara.PickupsCombo[2 * i + 1] = false;
	}

	g_Inventory.LoadObjects(false);

	/* Hardcoded code */
}
