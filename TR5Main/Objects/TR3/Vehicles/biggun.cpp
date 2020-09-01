#include "framework.h"
#include "biggun.h"
#include "items.h"
#include "level.h"
#include "collide.h"
#include "input.h"
#include "lara.h"
#include "lara_flare.h"
#include "sound.h"
#include "sphere.h"
#include "effect2.h"
#include "lara_struct.h"
#include "tomb4fx.h"
#include "draw.h"

static long GunRotYAdd = 0;
int fireCount = 0;

void FireBigGun(ITEM_INFO *obj)
{
		Lara.hasFired = true;

		short itemNumber = CreateItem();
		if (itemNumber != NO_ITEM)
		{
			ITEM_INFO* item = &g_Level.Items[itemNumber];
			item->objectNumber = ID_ROCKET;
			item->roomNumber = LaraItem->roomNumber;


			PHD_VECTOR jointPos;
			jointPos.x = Lara.torsoXrot;
			jointPos.y = Lara.torsoYrot + 1300;
			jointPos.z = Lara.torsoZrot;

			GetLaraJointPosition(&jointPos, LM_RHAND);

			int x, y, z;
			item->pos.xPos = x = jointPos.x;
			item->pos.yPos = y = jointPos.y;
			item->pos.zPos = z = jointPos.z;

			jointPos.x = 0;
			jointPos.y = 180 + 1024;
			jointPos.z = 0;

			SmokeCountL = 32;
			SmokeWeapon = WEAPON_ROCKET_LAUNCHER;

			for (int i = 0; i < 5; i++)
				TriggerGunSmoke(x, y, z, jointPos.x - x, jointPos.y - y, jointPos.z - z, 1, WEAPON_ROCKET_LAUNCHER, 32);

			InitialiseItem(itemNumber);

			item->pos.xRot = LaraItem->pos.xRot + Lara.leftArm.xRot;
			item->pos.yRot = LaraItem->pos.yRot + Lara.leftArm.yRot;
			item->pos.zRot = 0;

			if (!Lara.leftArm.lock)
			{
				item->pos.xRot += Lara.torsoXrot;
				item->pos.yRot += Lara.torsoYrot;
			}

			item->speed = 512 >> 5;
			item->itemFlags[0] = 0;

			AddActiveItem(itemNumber);

			SoundEffect(SFX_EXPLOSION1, 0, 0);
		}

}

static int CanUseGun(ITEM_INFO *obj, ITEM_INFO *lara)
{
	int dist;
	long x, z;

	if ((!(TrInput & IN_ACTION))
		|| (Lara.gunStatus != LG_NO_ARMS)
		|| (lara->gravityStatus))
		return 0;

	x = lara->pos.xPos - obj->pos.xPos;
	z = lara->pos.zPos - obj->pos.zPos;

	dist = SQUARE(x) + SQUARE(z);

	if (dist > 30000)
		return 0;
	else
		return 1;
}

void BigGunInitialise(short itemNum)
{
	ITEM_INFO *obj;
	BIGGUNINFO *gun;

	obj = &g_Level.Items[itemNum];

	gun = (BIGGUNINFO*)malloc(sizeof(BIGGUNINFO));
	obj->data = malloc(sizeof(BIGGUNINFO));

	gun->flags = 0;
	fireCount = 0;
	gun->yRot = 30;
	gun->yRot = 0;
	gun->startYRot = obj->pos.yRot;
}

void BigGunCollision(short itemNum, ITEM_INFO* lara, COLL_INFO* coll)
{
	ITEM_INFO* obj;
	BIGGUNINFO *gun;

	if (lara->hitPoints <= 0 || Lara.Vehicle != NO_ITEM)
		return;

	obj = &g_Level.Items[itemNum];

	if (CanUseGun(obj, lara))
	{
		Lara.Vehicle = itemNum;

		if (Lara.gunType == WEAPON_FLARE)
		{
			CreateFlare(ID_FLARE_ITEM, 0);
			undraw_flare_meshes();
			Lara.flareControlLeft = false;
			Lara.requestGunType = WEAPON_NONE;
			Lara.gunType = WEAPON_NONE;
		}

		Lara.gunStatus = LG_HANDS_BUSY;

		lara->pos.xPos = obj->pos.xPos;
		lara->pos.xRot = obj->pos.xRot;
		lara->pos.yPos = obj->pos.yPos;
		lara->pos.yRot = obj->pos.yRot;
		lara->pos.zPos = obj->pos.zPos;
		lara->pos.zRot = obj->pos.zRot;

		lara->animNumber = Objects[ID_BIGGUN_ANIMS].animIndex + 0;
		lara->frameNumber = g_Level.Anims[Objects[ID_BIGGUN_ANIMS].animIndex].frameBase;
		lara->currentAnimState = 0;
		lara->goalAnimState = 0;

		gun = (BIGGUNINFO*)obj->data;
		gun->flags = 0;
		gun->xRot = 30;

	}
	else
		ObjectCollision(itemNum, lara, coll);
}

int BigGunControl(COLL_INFO *coll)
{
	BIGGUNINFO *gun;
	ITEM_INFO *obj;
	ITEM_INFO *lara;

	lara = LaraItem;
	obj = &g_Level.Items[Lara.Vehicle];
	gun = (BIGGUNINFO *)obj->data;

	if (gun->flags & 1)
	{
		if ((lara->hitPoints <= 0) || (TrInput & IN_ROLL))
		{
			gun->flags = 2;
		}
		else if ((TrInput & IN_ACTION) && fireCount == 0)
		{
			FireBigGun(obj);
			fireCount = 26;
		}
		else
		{
			if (TrInput & IN_LEFT)
			{
				if (GunRotYAdd > 0)
					GunRotYAdd--;// >>= 1;

				GunRotYAdd -= 16;

				if (GunRotYAdd < -64)
					GunRotYAdd = -64;

				if (((Wibble & 7) == 0) && (abs(gun->yRot) < 544))
					SoundEffect(SFX_TR3_LARA_UZI_STOP, &obj->pos, NULL);
			}
			else if (TrInput & IN_RIGHT)
			{
				if (GunRotYAdd < 0)
					GunRotYAdd++;// >>= 1;

				GunRotYAdd += 16;

				if (GunRotYAdd < 64)
					GunRotYAdd = 64;

				if (((Wibble & 7) == 0) && (abs(gun->yRot) < (136 << 2)))
					SoundEffect(SFX_TR3_LARA_UZI_STOP, &obj->pos, NULL);
			}
			else
			{
//				GunRotYAdd -= GunRotYAdd >> 2;
				if (abs(GunRotYAdd) < 16)
					GunRotYAdd = 0;
			}

			gun->yRot = GunRotYAdd >> 2;
			
			if (gun->yRot < -(136 << 2))
			{
				gun->yRot = -(136 << 2);
				GunRotYAdd = 0;
			}
			else if (gun->yRot > (136 << 2))
			{
				gun->yRot = (136 << 2);
				GunRotYAdd = 0;
			}
			
			if ((TrInput & IN_FORWARD) && (gun->xRot < 59))
				gun->xRot++;
			else if ((TrInput & IN_BACK) && (gun->xRot))
				gun->xRot--;
		}
	}
	if (gun->flags & 2)
	{
		if (gun->xRot < 30)
			gun->xRot++;
		else if (gun->xRot > 30)
			gun->xRot--;
		else
		{
			lara->animNumber = Objects[ID_BIGGUN_ANIMS].animIndex + 1;
			lara->frameNumber = g_Level.Anims[Objects[ID_BIGGUN_ANIMS].animIndex].frameBase;
			lara->currentAnimState = 1;
			lara->goalAnimState = 1;
			gun->flags = 4;
		}
	}

	switch (lara->currentAnimState)
	{
	case 0:
	case 1:
		AnimateItem(lara);
		obj->animNumber = Objects[ID_BIGGUN].animIndex + (lara->animNumber - Objects[ID_BIGGUN_ANIMS].animIndex);
		obj->frameNumber = g_Level.Anims[obj->animNumber].frameBase + (lara->frameNumber - g_Level.Anims[lara->animNumber].frameBase);

		if ((gun->flags & 4) && lara->frameNumber == g_Level.Anims[lara->animNumber].frameEnd)
		{
			lara->animNumber = LA_STAND_SOLID;
			lara->frameNumber = g_Level.Anims[lara->animNumber].frameBase;
			lara->currentAnimState = LS_STOP;
			lara->goalAnimState = LS_STOP;
			Lara.Vehicle = NO_ITEM;
			Lara.gunStatus = LG_NO_ARMS;
		}
		break;
	case 2:
		lara->animNumber = Objects[ID_BIGGUN_ANIMS].animIndex + 2;
		lara->frameNumber = g_Level.Anims[Objects[ID_BIGGUN].animIndex + 2].frameBase + gun->xRot;

		obj->animNumber = Objects[ID_BIGGUN].animIndex + (lara->animNumber - Objects[ID_BIGGUN_ANIMS].animIndex);
		obj->frameNumber = g_Level.Anims[obj->animNumber].frameBase + (lara->frameNumber - g_Level.Anims[lara->animNumber].frameBase);

		if (fireCount)
			fireCount--;

		gun->flags = 1;
		break;
	}
	
	lara->pos.yRot = gun->startYRot + (gun->yRot * (ANGLE(1) >> 2));
	obj->pos.yRot = gun->startYRot + (gun->yRot * (ANGLE(1) >> 2));

	coll->enableSpaz = false;
	coll->enableBaddiePush = false;
	LaraBaddieCollision(lara, coll);
	
	Camera.targetElevation = -ANGLE(15);
	return 1;
}
