#include "framework.h"
#include "Game/Lara/lara_flare.h"

#include "Game/animation.h"
#include "Game/collision/collide_item.h"
#include "Game/effects/effects.h"
#include "Game/effects/chaffFX.h"
#include "Game/items.h"
#include "Game/Lara/lara.h"
#include "Game/Lara/lara_fire.h"
#include "Game/Lara/lara_helpers.h"
#include "Game/Lara/lara_tests.h"
#include "Sound/sound.h"
#include "Specific/level.h"
#include "Specific/setup.h"
#include "Specific/prng.h"

using namespace TEN::Math::Random;

constexpr auto FlareMainColor = Vector3(1, 0.52947, 0.3921);

void FlareControl(short itemNumber)
{
	auto* flareItem = &g_Level.Items[itemNumber];

	if (TestEnvironment(ENV_FLAG_SWAMP, flareItem))
	{
		KillItem(itemNumber);
		return;
	}

	if (flareItem->Animation.VerticalVelocity)
	{
		flareItem->Pose.Orientation.x += ANGLE(3.0f);
		flareItem->Pose.Orientation.z += ANGLE(5.0f);
	}
	else
	{
		flareItem->Pose.Orientation.x = 0;
		flareItem->Pose.Orientation.z = 0;
	}

	auto oldPos = flareItem->Pose.Position;

	int xVel = flareItem->Animation.Velocity * phd_sin(flareItem->Pose.Orientation.y);
	int zVel = flareItem->Animation.Velocity * phd_cos(flareItem->Pose.Orientation.y);

	flareItem->Pose.Position.x += xVel;
	flareItem->Pose.Position.z += zVel;

	if (TestEnvironment(ENV_FLAG_WATER, flareItem) ||
		TestEnvironment(ENV_FLAG_SWAMP, flareItem))
	{
		flareItem->Animation.VerticalVelocity += (5 - flareItem->Animation.VerticalVelocity) / 2;
		flareItem->Animation.Velocity += (5 - flareItem->Animation.Velocity) / 2;
	}
	else
		flareItem->Animation.VerticalVelocity += 6;

	flareItem->Pose.Position.y += flareItem->Animation.VerticalVelocity;

	DoProjectileDynamics(itemNumber, oldPos.x, oldPos.y, oldPos.z, xVel, flareItem->Animation.VerticalVelocity, zVel);

	int& life = flareItem->Data;
	life &= 0x7FFF;
	if (life >= FLARE_LIFE_MAX)
	{
		if (!flareItem->Animation.VerticalVelocity && !flareItem->Animation.Velocity)
		{
			KillItem(itemNumber);
			return;
		}
	}
	else
		life++;

	if (DoFlareLight((Vector3Int*)&flareItem->Pose, life))
	{
		TriggerChaffEffects(flareItem, life);
		/* Hardcoded code */

		life |= 0x8000;
	}
}

void ReadyFlare(ItemInfo* laraItem)
{
	auto* lara = GetLaraInfo(laraItem);

	lara->Control.HandStatus = HandStatus::Free;
	lara->LeftArm.Orientation = Vector3Shrt();
	lara->RightArm.Orientation = Vector3Shrt();
	lara->LeftArm.Locked = false;
	lara->RightArm.Locked = false;
	lara->TargetEntity = nullptr;
}

void UndrawFlareMeshes(ItemInfo* laraItem)
{
	auto* lara = GetLaraInfo(laraItem);

	lara->MeshPtrs[LM_LHAND] = Objects[ID_LARA_SKIN].meshIndex + LM_LHAND;
}

void DrawFlareMeshes(ItemInfo* laraItem)
{
	auto* lara = GetLaraInfo(laraItem);

	lara->MeshPtrs[LM_LHAND] = Objects[ID_LARA_FLARE_ANIM].meshIndex + LM_LHAND;
}

void UndrawFlare(ItemInfo* laraItem)
{
	auto* lara = GetLaraInfo(laraItem);

	int flareFrame = lara->Flare.Frame;
	int armFrame = lara->LeftArm.FrameNumber;

	lara->Flare.ControlLeft = true;

	if (laraItem->Animation.TargetState == LS_IDLE &&
		lara->Vehicle == NO_ITEM)
	{
		if (laraItem->Animation.AnimNumber == LA_STAND_IDLE)
		{
			laraItem->Animation.AnimNumber = LA_DISCARD_FLARE;
			flareFrame = armFrame + g_Level.Anims[laraItem->Animation.AnimNumber].frameBase;
			laraItem->Animation.FrameNumber = flareFrame;
			lara->Flare.Frame = flareFrame;
		}

		if (laraItem->Animation.AnimNumber == LA_DISCARD_FLARE)
		{
			lara->Flare.ControlLeft = false;

			if (flareFrame >= g_Level.Anims[laraItem->Animation.AnimNumber].frameBase + 31) // Last frame.
			{
				lara->Control.Weapon.RequestGunType = lara->Control.Weapon.LastGunType;
				lara->Control.Weapon.GunType = lara->Control.Weapon.LastGunType;
				lara->Control.HandStatus = HandStatus::Free;

				InitialiseNewWeapon(laraItem);

				lara->TargetEntity = nullptr;
				lara->RightArm.Locked = false;
				lara->LeftArm.Locked = false;
				SetAnimation(laraItem, LA_STAND_IDLE);
				lara->Flare.Frame = g_Level.Anims[laraItem->Animation.AnimNumber].frameBase;
				return;
			}

			lara->Flare.Frame++;
		}
	}
	else if (laraItem->Animation.AnimNumber == LA_DISCARD_FLARE)
		SetAnimation(laraItem, LA_STAND_IDLE);

	if (armFrame >= 33 && armFrame < 72)
	{
		armFrame = 2;
		DoFlareInHand(laraItem, lara->Flare.Life);
	}
	else if (!armFrame)
	{
		armFrame = 1;
		DoFlareInHand(laraItem, lara->Flare.Life);
	}
	else if (armFrame >= 72 && armFrame < 95)
	{
		armFrame++;

		if (armFrame == 94)
		{
			armFrame = 1;
			DoFlareInHand(laraItem, lara->Flare.Life);
		}
	}
	else if (armFrame >= 1 && armFrame < 33)
	{
		armFrame++;

		if (armFrame == 21)
		{
			CreateFlare(laraItem, ID_FLARE_ITEM, true);
			UndrawFlareMeshes(laraItem);
			lara->Flare.Life = 0;
		}
		else if (armFrame == 33)
		{
			armFrame = 0;

			lara->Control.Weapon.RequestGunType = lara->Control.Weapon.LastGunType;
			lara->Control.Weapon.GunType = lara->Control.Weapon.LastGunType;
			lara->Control.HandStatus = HandStatus::Free;

			InitialiseNewWeapon(laraItem);

			lara->TargetEntity = nullptr;
			lara->LeftArm.Locked = false;
			lara->RightArm.Locked = false;
			lara->Flare.ControlLeft = false;
			lara->Flare.Frame = 0;
		}
		else if (armFrame < 21)
			DoFlareInHand(laraItem, lara->Flare.Life);
	}
	else if (armFrame >= 95 && armFrame < 110)
	{
		armFrame++;

		if (armFrame == 110)
		{
			armFrame = 1;
			DoFlareInHand(laraItem, lara->Flare.Life);
		}
	}

	lara->LeftArm.FrameNumber = armFrame;
	SetFlareArm(laraItem, lara->LeftArm.FrameNumber);
}

void DrawFlare(ItemInfo* laraItem)
{
	auto* lara = GetLaraInfo(laraItem);

	if (laraItem->Animation.ActiveState == LS_PICKUP_FLARE ||
		laraItem->Animation.ActiveState == LS_PICKUP)
	{
		DoFlareInHand(laraItem, lara->Flare.Life);
		lara->Flare.ControlLeft = false;
		lara->LeftArm.FrameNumber = 93;
		SetFlareArm(laraItem, 93);
	}
	else
	{
		int armFrame = lara->LeftArm.FrameNumber + 1;
		lara->Flare.ControlLeft = true;

		if (armFrame < 33 || armFrame > 94)
			armFrame = 33;
		else if (armFrame == 46)
			DrawFlareMeshes(laraItem);
		else if (armFrame >= 72 && armFrame <= 93)
		{
			if (armFrame == 72)
			{
				SoundEffect(SFX_TR4_OBJ_GEM_SMASH, &laraItem->Pose, TestEnvironment(ENV_FLAG_WATER, laraItem));
				lara->Flare.Life = 1;
			}

			DoFlareInHand(laraItem, lara->Flare.Life);
		}
		else
		{
			if (armFrame == 94)
			{
				ReadyFlare(laraItem);
				armFrame = 0;
				DoFlareInHand(laraItem, lara->Flare.Life);
			}
		}

		lara->LeftArm.FrameNumber = armFrame;
		SetFlareArm(laraItem, armFrame);
	}
}

void SetFlareArm(ItemInfo* laraItem, int armFrame)
{
	auto* lara = GetLaraInfo(laraItem);
	int flareAnimNum = Objects[ID_LARA_FLARE_ANIM].animIndex;

	if (armFrame >= 95)
		flareAnimNum += 4;
	else if (armFrame >= 72)
		flareAnimNum += 3;
	else if (armFrame >= 33)
		flareAnimNum += 2;
	else if (armFrame >= 1)
		flareAnimNum += 1;

	lara->LeftArm.AnimNumber = flareAnimNum;
	lara->LeftArm.FrameBase = g_Level.Anims[flareAnimNum].framePtr;
}

void CreateFlare(ItemInfo* laraItem, GAME_OBJECT_ID objectNumber, bool thrown)
{
	auto* lara = GetLaraInfo(laraItem);
	auto itemNumber = CreateItem();

	if (itemNumber != NO_ITEM)
	{
		auto* flareItem = &g_Level.Items[itemNumber];
		bool flag = false;

		flareItem->ObjectNumber = objectNumber;
		flareItem->RoomNumber = laraItem->RoomNumber;

		auto pos = Vector3Int (-16, 32, 42);
		GetLaraJointPosition(&pos, LM_LHAND);

		flareItem->Pose.Position.x = pos.x;
		flareItem->Pose.Position.y = pos.y;
		flareItem->Pose.Position.z = pos.z;

		int floorHeight = GetCollision(pos.x, pos.y, pos.z, laraItem->RoomNumber).Position.Floor;
		auto collided = GetCollidedObjects(flareItem, 0, true, CollidedItems, CollidedMeshes, true);
		if (floorHeight < pos.y || collided)
		{
			flag = true;
			flareItem->Pose.Orientation.y = laraItem->Pose.Orientation.y + ANGLE(180.0f);
			flareItem->Pose.Position.x = laraItem->Pose.Position.x + 320 * phd_sin(flareItem->Pose.Orientation.y);
			flareItem->Pose.Position.z = laraItem->Pose.Position.z + 320 * phd_cos(flareItem->Pose.Orientation.y);
			flareItem->RoomNumber = laraItem->RoomNumber;
		}
		else
		{
			if (thrown)
				flareItem->Pose.Orientation.y = laraItem->Pose.Orientation.y;
			else
				flareItem->Pose.Orientation.y = laraItem->Pose.Orientation.y - ANGLE(45.0f);

			flareItem->RoomNumber = laraItem->RoomNumber;
		}

		InitialiseItem(itemNumber);

		flareItem->Pose.Orientation.x = 0;
		flareItem->Pose.Orientation.z = 0;
		flareItem->Shade = -1;

		if (thrown)
		{
			flareItem->Animation.Velocity = laraItem->Animation.Velocity + 50;
			flareItem->Animation.VerticalVelocity = laraItem->Animation.VerticalVelocity - 50;
		}
		else
		{
			flareItem->Animation.Velocity = laraItem->Animation.Velocity + 10;
			flareItem->Animation.VerticalVelocity = laraItem->Animation.VerticalVelocity + 50;
		}

		if (flag)
			flareItem->Animation.Velocity /= 2;

		if (objectNumber == ID_FLARE_ITEM)
		{
			flareItem->Data = (int)0;
			int& life = flareItem->Data;
			if (DoFlareLight((Vector3Int*)&flareItem->Pose, lara->Flare.Life))
				life = lara->Flare.Life | 0x8000;
			else
				life = lara->Flare.Life & 0x7FFF;
		}
		else
			flareItem->ItemFlags[3] = lara->LitTorch;

		AddActiveItem(itemNumber);
		flareItem->Status = ITEM_ACTIVE;
	}
}

void DrawFlareInAir(ItemInfo* flareItem)
{
	TENLog("DrawFlareInAir() not implemented!", LogLevel::Warning);
}

void DoFlareInHand(ItemInfo* laraItem, int flareLife)
{
	auto* lara = GetLaraInfo(laraItem);

	auto pos = Vector3Int(11, 32, 41);
	GetLaraJointPosition(&pos, LM_LHAND);

	if (DoFlareLight(&pos, flareLife))
		TriggerChaffEffects(flareLife);

	/* Hardcoded code */

	if (lara->Flare.Life >= FLARE_LIFE_MAX)
	{
		if (lara->Control.HandStatus == HandStatus::Free)
			lara->Control.HandStatus = HandStatus::WeaponUndraw;
	}
	else if (lara->Flare.Life != 0)
		lara->Flare.Life++;
}

int DoFlareLight(Vector3Int* pos, int flareLife)
{
	if (flareLife >= FLARE_LIFE_MAX || flareLife == 0)
		return 0;

	float random = GenerateFloat();

	int x = pos->x + (random * 120);
	int y = pos->y + (random * 120) - CLICK(1);
	int z = pos->z + (random * 120);

	if (flareLife < 4)
	{
		int falloff = 12 + ((1 - (flareLife / 4.0f)) * 16);

		int r = FlareMainColor.x * 255;
		int g = FlareMainColor.y * 255;
		int b = FlareMainColor.z * 255;

		TriggerDynamicLight(x, y, z, falloff, r, g, b);

		return (random < 0.9f);
	}
	else if (flareLife < (FLARE_LIFE_MAX - 90))
	{
		float multiplier = GenerateFloat(0.75f, 1.0f);
		int falloff = 12 * multiplier;

		int r = FlareMainColor.x * 255 * multiplier;
		int g = FlareMainColor.y * 255 * multiplier;
		int b = FlareMainColor.z * 255 * multiplier;
		TriggerDynamicLight(x, y, z, falloff, r, g, b);

		return (random < 0.4f);
	}
	else
	{
		float multiplier = GenerateFloat(0.05f, 0.8f);
		int falloff = 12 * (1.0f - ((flareLife - (FLARE_LIFE_MAX - 90)) / (FLARE_LIFE_MAX - (FLARE_LIFE_MAX - 90))));

		int r = FlareMainColor.x * 255 * multiplier;
		int g = FlareMainColor.y * 255 * multiplier;
		int b = FlareMainColor.z * 255 * multiplier;
		TriggerDynamicLight(x, y, z, falloff, r, g, b);

		return (random < 0.3f);
	}
}