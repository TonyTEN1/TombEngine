#include "framework.h"
#include "Objects/TR3/Vehicles/upv.h"

#include "Game/animation.h"
#include "Game/camera.h"
#include "Game/collision/sphere.h"
#include "Game/collision/collide_item.h"
#include "Game/control/box.h"
#include "Game/control/los.h"
#include "Game/effects/bubble.h"
#include "Game/effects/effects.h"
#include "Game/items.h"
#include "Game/Lara/lara.h"
#include "Game/Lara/lara_flare.h"
#include "Game/Lara/lara_helpers.h"
#include "Game/Lara/lara_one_gun.h"
#include "Game/savegame.h"
#include "Objects/TR3/Vehicles/upv_info.h"
#include "Sound/sound.h"
#include "Specific/level.h"
#include "Specific/input.h"
#include "Specific/setup.h"

#define	UPV_CONTROL 1
#define	UPV_SURFACE 2
#define	UPV_DIVE 4
#define	UPV_DEAD 8

#define ACCELERATION		0x40000
#define FRICTION			0x18000
#define MAX_SPEED			0x400000
#define ROT_ACCELERATION	0x400000
#define ROT_SLOWACCEL		0x200000
#define ROT_FRICTION 		0x100000
#define MAX_ROTATION		0x1c00000
#define UPDOWN_ACCEL		(ANGLE(2.0f) * 65536)
#define UPDOWN_SLOWACCEL	(ANGLE(1.0f) * 65536)
#define UPDOWN_FRICTION		(ANGLE(1.0f) * 65536)
#define MAX_UPDOWN			(ANGLE(2.0f) * 65536)
#define UPDOWN_LIMIT		ANGLE(80.0f)
#define UPDOWN_SPEED		10
#define SURFACE_DIST		210
#define SURFACE_ANGLE		ANGLE(30.0f)
#define DIVE_ANGLE			ANGLE(15.0f)
#define DIVE_SPEED			ANGLE(5.0f)
#define SUB_DRAW_SHIFT		128
#define SUB_RADIUS			300
#define SUB_HEIGHT			400
#define SUB_LENGTH			WALL_SIZE
#define FRONT_TOLERANCE		(ANGLE(45.0f) * 65536)
#define TOP_TOLERANCE		(ANGLE(45.0f) * 65536)
#define WALLDEFLECT			(ANGLE(2.0f) * 65536)
#define GETOFF_DIST 		WALL_SIZE
#define HARPOON_SPEED		256
#define HARPOON_RELOAD		15

#define UPV_TURBINE_BONE 3

#define DEATH_FRAME_1					16
#define DEATH_FRAME_2					17
#define DISMOUNT_SURFACE_FRAME			51
#define MOUNT_SURFACE_SOUND_FRAME		30
#define MOUNT_SURFACE_CONTROL_FRAME		50
#define DISMOUNT_UNDERWATER_FRAME		42
#define MOUNT_UNDERWATER_SOUND_FRAME	30
#define MOUNT_UNDERWATER_CONTROL_FRAME	42

#define UPV_IN_PROPEL		IN_JUMP
#define UPV_IN_UP			IN_FORWARD
#define UPV_IN_DOWN			IN_BACK
#define UPV_IN_LEFT			IN_LEFT
#define UPV_IN_RIGHT		IN_RIGHT
#define UPV_IN_FIRE			IN_ACTION
#define UPV_IN_DISMOUNT		IN_ROLL

BITE_INFO sub_bites[6] =
{
	{ 0, 0, 0, 3 },
	{ 0, 96, 256, 0 },
	{ -128, 0, -64, 1 },
	{ 0, 0, -64, 1 },
	{ 128, 0, -64, 2 },
	{ 0, 0, -64, 2 }
};

enum SUB_BITE_FLAG
{
	SUB_FAN = 0,
	SUB_FRONT_LIGHT,
	SUB_LEFT_FIN_LEFT,
	SUB_LEFT_FIN_RIGHT,
	SUB_RIGHT_FIN_RIGHT,
	SUB_RIGHT_FIN_LEFT
};

enum UPVState
{
	UPV_STATE_DEATH,
	UPV_STATE_HIT,
	UPV_STATE_DISMOUNT_SURFACE,
	UPV_STATE_UNK1,
	UPV_STATE_MOVE,
	UPV_STATE_IDLE,
	UPV_STATE_UNK2,
	UPV_STATE_UNK3,
	UPV_STATE_MOUNT,
	UPV_STATE_DISMOUNT_UNDERWATER
};

// TODO
enum UPVAnim
{
	UPV_ANIM_DEATH = 0,

	UPV_ANIM_IDLE = 5,

	UPV_ANIM_DISMOUNT_SURFACE = 9,
	UPV_ANIM_MOUNT_SURFACE_START = 10,
	UPV_ANIM_MOUNT_SURFACE_END = 11,
	UPV_ANIM_DISMOUNT_UNDERWATER = 12,
	UPV_ANIM_MOUNT_UNDERWATER = 13,
};

void SubInitialise(short itemNum)
{
	ITEM_INFO* UPVItem = &g_Level.Items[itemNum];
	UPVItem->Data = SUB_INFO();
	SUB_INFO* UPVInfo = UPVItem->Data;

	UPVInfo->Vel = UPVInfo->Rot = 0;
	UPVInfo->Flags = UPV_SURFACE;
	UPVInfo->WeaponTimer = 0;
}

static void FireSubHarpoon(ITEM_INFO* laraItem, ITEM_INFO* UPVItem)
{
	short itemNum = CreateItem();

	if (itemNum != NO_ITEM)
	{
		static char lr = 0;
		PHD_VECTOR pos { (lr ? 22 : -22), 24, 230 };
		ITEM_INFO* harpoonItem = &g_Level.Items[itemNum];

		harpoonItem->ObjectNumber = ID_HARPOON;
		harpoonItem->Shade = 0xC210;
		harpoonItem->RoomNumber = UPVItem->RoomNumber;

		GetJointAbsPosition(UPVItem, &pos, UPV_TURBINE_BONE);

		harpoonItem->Position.xPos = pos.x;
		harpoonItem->Position.yPos = pos.y;
		harpoonItem->Position.zPos = pos.z;
		InitialiseItem(itemNum);

		harpoonItem->Position.xRot = UPVItem->Position.xRot;
		harpoonItem->Position.yRot = UPVItem->Position.yRot;
		harpoonItem->Position.zRot = 0;

		harpoonItem->VerticalVelocity = -HARPOON_SPEED * phd_sin(harpoonItem->Position.xRot);
		harpoonItem->VerticalVelocity = HARPOON_SPEED * phd_cos(harpoonItem->Position.xRot);
		harpoonItem->HitPoints = HARPOON_TIME;
		harpoonItem->ItemFlags[0] = 1;

		AddActiveItem(itemNum);

		SoundEffect(SFX_TR3_LARA_HARPOON_FIRE_WATER, &LaraItem->Position, 2);

		if (Lara.Weapons[WEAPON_HARPOON_GUN].Ammo[WEAPON_AMMO1])
			Lara.Weapons[WEAPON_HARPOON_GUN].Ammo[WEAPON_AMMO1]--;
		Statistics.Game.AmmoUsed++;

		lr ^= 1;
	}
}

static void TriggerSubMist(long x, long y, long z, long speed, short angle)
{
	long size, xv, zv;
	SPARKS* sptr;

	sptr = &Sparks[GetFreeSpark()];

	sptr->on = 1;
	sptr->sR = 0;
	sptr->sG = 0;
	sptr->sB = 0;

	sptr->dR = 64;
	sptr->dG = 64;
	sptr->dB = 64;

	sptr->colFadeSpeed = 4 + (GetRandomControl() & 3);
	sptr->fadeToBlack = 12;
	sptr->sLife = sptr->life = (GetRandomControl() & 3) + 20;
	sptr->transType = TransTypeEnum::COLADD;
	sptr->extras = 0;
	sptr->dynamic = -1;

	sptr->x = x + ((GetRandomControl() & 15) - 8);
	sptr->y = y + ((GetRandomControl() & 15) - 8);
	sptr->z = z + ((GetRandomControl() & 15) - 8);
	zv = speed * phd_cos(angle) / 4;
	xv = speed * phd_sin(angle) / 4;
	sptr->xVel = xv + ((GetRandomControl() & 127) - 64);
	sptr->yVel = 0;
	sptr->zVel = zv + ((GetRandomControl() & 127) - 64);
	sptr->friction = 3;

	if (GetRandomControl() & 1)
	{
		sptr->flags = SP_SCALE | SP_DEF | SP_ROTATE | SP_EXPDEF;
		sptr->rotAng = GetRandomControl() & 4095;

		if (GetRandomControl() & 1)
			sptr->rotAdd = -(GetRandomControl() & 15) - 16;
		else
			sptr->rotAdd = (GetRandomControl() & 15) + 16;
	}
	else
		sptr->flags = SP_SCALE | SP_DEF | SP_EXPDEF;

	sptr->scalar = 3;
	sptr->gravity = sptr->maxYvel = 0;
	size = (GetRandomControl() & 7) + (speed / 2) + 16;
	sptr->size = sptr->sSize = size / 4;
	sptr->dSize = size;
}

void SubEffects(short itemNum)
{
	if (itemNum == NO_ITEM)
		return;

	ITEM_INFO* laraItem = LaraItem;
	LaraInfo*& laraInfo = laraItem->Data;
	ITEM_INFO* UPVItem = &g_Level.Items[itemNum];
	SUB_INFO* UPVInfo = UPVItem->Data;
	PHD_VECTOR pos;
	long lp;

	if (laraInfo->Vehicle == itemNum)
	{
		if (!UPVInfo->Vel)
			UPVInfo->FanRot += ANGLE(2.0f);
		else
			UPVInfo->FanRot += (UPVInfo->Vel / 4069);

		if (UPVInfo->Vel)
		{
			pos = { sub_bites[SUB_FAN].x, sub_bites[SUB_FAN].y, sub_bites[SUB_FAN].z };
			GetJointAbsPosition(UPVItem, &pos, sub_bites[SUB_FAN].meshNum);
			TriggerSubMist(pos.x, pos.y + SUB_DRAW_SHIFT, pos.z, abs(UPVInfo->Vel) / 65536, UPVItem->Position.yRot + ANGLE(180.0f));

			if ((GetRandomControl() & 1) == 0)
			{
				PHD_3DPOS pos3d;
				short roomNum;

				pos3d.xPos = pos.x + (GetRandomControl() & 63) - 32;
				pos3d.yPos = pos.y + SUB_DRAW_SHIFT;
				pos3d.zPos = pos.z + (GetRandomControl() & 63) - 32;
				roomNum = UPVItem->RoomNumber;
				GetFloor(pos3d.xPos, pos3d.yPos, pos3d.zPos, &roomNum);
				CreateBubble((PHD_VECTOR*)&pos3d, roomNum, 4, 8, BUBBLE_FLAG_CLUMP, 0, 0, 0);
			}
		}
	}
	
	for (lp = 0; lp < 2; lp++)
	{
		GAME_VECTOR	source, target;
		long r;

		r = 31 - (GetRandomControl() & 3);
		pos = { sub_bites[SUB_FRONT_LIGHT].x, sub_bites[SUB_FRONT_LIGHT].y, sub_bites[SUB_FRONT_LIGHT].z << (lp * 6) };
		GetJointAbsPosition(UPVItem, &pos, sub_bites[SUB_FRONT_LIGHT].meshNum);

		if (lp == 1)
		{
			target.x = pos.x;
			target.y = pos.y;
			target.z = pos.z;
			target.roomNumber = UPVItem->RoomNumber;
			LOS(&source, &target);
			pos = { target.x, target.y, target.z };
		}
		else
		{
			source.x = pos.x;
			source.y = pos.y;
			source.z = pos.z;
			source.roomNumber = UPVItem->RoomNumber;
		}

		TriggerDynamicLight(pos.x, pos.y, pos.z, 16 + (lp << 3), r, r, r);
	}

	if (UPVInfo->WeaponTimer)
		UPVInfo->WeaponTimer--;
}

static bool TestUPVDismount(ITEM_INFO* laraItem, ITEM_INFO* UPVItem)
{
	LaraInfo*& laraInfo = laraItem->Data;

	if (laraInfo->Control.ExtraVelocity.x || laraInfo->Control.ExtraVelocity.z)
		return false;

	short moveAngle = UPVItem->Position.yRot + ANGLE(180.0f);
	int speed = GETOFF_DIST * phd_cos(UPVItem->Position.xRot);
	int x = UPVItem->Position.xPos + speed * phd_sin(moveAngle);
	int z = UPVItem->Position.zPos + speed * phd_cos(moveAngle);
	int y = UPVItem->Position.yPos - GETOFF_DIST * phd_sin(-UPVItem->Position.xRot);
	auto probe = GetCollisionResult(x, y, z, UPVItem->RoomNumber);

	if (probe.Position.Floor < y ||
		(probe.Position.Floor - probe.Position.Ceiling) < STEP_SIZE ||
		probe.Position.Ceiling > y ||
		probe.Position.Floor == NO_HEIGHT ||
		probe.Position.Ceiling == NO_HEIGHT)
	{
		return false;
	}

	return true;
}

static bool TestUPVMount(ITEM_INFO* laraItem, ITEM_INFO* UPVItem)
{
	LaraInfo*& laraInfo = laraItem->Data;

	if (!(TrInput & IN_ACTION) ||
		laraInfo->gunStatus != LG_HANDS_FREE ||
		laraItem->Airborne)
	{
		return false;
	}

	int y = abs(laraItem->Position.yPos - (UPVItem->Position.yPos - STEP_SIZE / 2));
	int dist = pow(laraItem->Position.xPos - UPVItem->Position.xPos, 2) + pow(laraItem->Position.zPos - UPVItem->Position.zPos, 2);
	short rotDelta = abs(laraItem->Position.yRot - UPVItem->Position.yRot);

	if (y > STEP_SIZE ||
		dist > pow(STEP_SIZE * 2, 2) ||
		rotDelta > ANGLE(35.0f) || rotDelta < -ANGLE(35.0f) ||
		GetCollisionResult(UPVItem).Position.Floor < -32000)
	{
		return false;
	}

	return true;
}

static void DoCurrent(ITEM_INFO* item)
{
	PHD_VECTOR target;

	if (!Lara.currentActive)
	{
		long shifter, absvel;

		absvel = abs(Lara.Control.ExtraVelocity.x);

		if (absvel > 16)
			shifter = 4;
		else if (absvel > 8)
			shifter = 3;
		else
			shifter = 2;

		Lara.Control.ExtraVelocity.x -= Lara.Control.ExtraVelocity.x >> shifter;

		if (abs(Lara.Control.ExtraVelocity.x) < 4)
			Lara.Control.ExtraVelocity.x = 0;

		absvel = abs(Lara.Control.ExtraVelocity.z);
		if (absvel > 16)
			shifter = 4;
		else if (absvel > 8)
			shifter = 3;
		else
			shifter = 2;

		Lara.Control.ExtraVelocity.z -= Lara.Control.ExtraVelocity.z >> shifter;
		if (abs(Lara.Control.ExtraVelocity.z) < 4)
			Lara.Control.ExtraVelocity.z = 0;

		if (Lara.Control.ExtraVelocity.x == 0 && Lara.Control.ExtraVelocity.z == 0)
			return;
	}
	else
	{
		long angle, dx, dz, speed, sinkval;

		sinkval = Lara.currentActive - 1;
		target.x = g_Level.Sinks[sinkval].x;
		target.y = g_Level.Sinks[sinkval].y;
		target.z = g_Level.Sinks[sinkval].z;
		angle = ((mGetAngle(target.x, target.z, LaraItem->Position.xPos, LaraItem->Position.zPos) - ANGLE(90)) / 16) & 4095;

		dx = target.x - LaraItem->Position.xPos;
		dz = target.z - LaraItem->Position.zPos;

		speed = g_Level.Sinks[sinkval].strength;
		dx = phd_sin(angle * 16) * speed * 1024;
		dz = phd_cos(angle * 16) * speed * 1024;

		Lara.Control.ExtraVelocity.x += ((dx - Lara.Control.ExtraVelocity.x) / 16);
		Lara.Control.ExtraVelocity.z += ((dz - Lara.Control.ExtraVelocity.z) / 16);
	}

	item->Position.xPos += (Lara.Control.ExtraVelocity.x / 256);
	item->Position.zPos += (Lara.Control.ExtraVelocity.z / 256);
	Lara.currentActive = 0;
}

static void BackgroundCollision(ITEM_INFO* laraItem, ITEM_INFO* UPVItem)
{
	LaraInfo*& laraInfo = laraItem->Data;
	SUB_INFO* UPVInfo = UPVItem->Data;
	COLL_INFO cinfo, * coll = &cinfo; // ??

	coll->Setup.LowerFloorBound = NO_LOWER_BOUND;
	coll->Setup.UpperFloorBound = -SUB_HEIGHT;
	coll->Setup.LowerCeilingBound = SUB_HEIGHT;
	coll->Setup.OldPosition.x = UPVItem->Position.xPos;
	coll->Setup.OldPosition.y = UPVItem->Position.yPos;
	coll->Setup.OldPosition.z = UPVItem->Position.zPos;
	coll->Setup.Radius = SUB_RADIUS;
	coll->Setup.FloorSlopeIsWall = false;
	coll->Setup.FloorSlopeIsPit = false;
	coll->Setup.DeathFlagIsPit = false;
	coll->Setup.EnableSpasm = false;
	coll->Setup.EnableObjectPush = true;
	coll->Setup.Mode = COLL_PROBE_MODE::QUADRANTS;

	if ((UPVItem->Position.xRot >= -16384) && (UPVItem->Position.xRot <= 16384))
		coll->Setup.ForwardAngle = laraInfo->Control.MoveAngle = UPVItem->Position.yRot;
	else
		coll->Setup.ForwardAngle = laraInfo->Control.MoveAngle = UPVItem->Position.yRot - ANGLE(180.0f);

	int height = phd_sin(UPVItem->Position.xRot) * SUB_LENGTH;
	if (height < 0)
		height = -height;
	if (height < 200)
		height = 200;

	coll->Setup.UpperFloorBound = -height;
	coll->Setup.Height = height;

	GetCollisionInfo(coll, UPVItem, PHD_VECTOR(0, height / 2, 0));
	ShiftItem(UPVItem, coll);

	if (coll->CollisionType == CT_FRONT)
	{
		if (UPVInfo->RotX > FRONT_TOLERANCE)
			UPVInfo->RotX += WALLDEFLECT;
		else if (UPVInfo->RotX < -FRONT_TOLERANCE)
			UPVInfo->RotX -= WALLDEFLECT;
		else
		{
			if (abs(UPVInfo->Vel) >= MAX_SPEED)
			{
				laraItem->TargetState = UPV_STATE_HIT;
				UPVInfo->Vel = -UPVInfo->Vel / 2;
			}
			else
				UPVInfo->Vel = 0;
		}
	}
	else if (coll->CollisionType == CT_TOP)
	{
		if (UPVInfo->RotX >= -TOP_TOLERANCE)
			UPVInfo->RotX -= WALLDEFLECT;
	}
	else if (coll->CollisionType == CT_TOP_FRONT)
		UPVInfo->Vel = 0;
	else if (coll->CollisionType == CT_LEFT)
		UPVItem->Position.yRot += ANGLE(5.0f);
	else if (coll->CollisionType == CT_RIGHT)
		UPVItem->Position.yRot -= ANGLE(5.0f);
	else if (coll->CollisionType == CT_CLAMP)
	{
		UPVItem->Position.xPos = coll->Setup.OldPosition.x;
		UPVItem->Position.yPos = coll->Setup.OldPosition.y;
		UPVItem->Position.zPos = coll->Setup.OldPosition.z;
		UPVInfo->Vel = 0;
		return;
	}

	if (coll->Middle.Floor < 0)
	{
		UPVItem->Position.yPos += coll->Middle.Floor;
		UPVInfo->RotX += WALLDEFLECT;
	}
}

static void UserInput(ITEM_INFO* laraItem, ITEM_INFO* UPVItem)
{
	LaraInfo*& laraInfo = laraItem->Data;
	SUB_INFO* UPVInfo = UPVItem->Data;

	TestUPVDismount(laraItem, UPVItem);

	int anim = laraItem->AnimNumber - Objects[ID_UPV_LARA_ANIMS].animIndex;
	int frame = laraItem->FrameNumber - g_Level.Anims[laraItem->AnimNumber].frameBase;

	switch (laraItem->ActiveState)
	{
	case UPV_STATE_MOVE:
		if (laraItem->HitPoints <= 0)
		{
			laraItem->TargetState = UPV_STATE_DEATH;
			break;
		}

		if (TrInput & UPV_IN_LEFT)
			UPVInfo->Rot -= ROT_ACCELERATION;

		else if (TrInput & UPV_IN_RIGHT)
			UPVInfo->Rot += ROT_ACCELERATION;

		if (UPVInfo->Flags & UPV_SURFACE)
		{
			int xa = UPVItem->Position.xRot - SURFACE_ANGLE;
			int ax = SURFACE_ANGLE - UPVItem->Position.xRot;

			if (xa > 0)
			{
				if (xa > ANGLE(1.0f))
					UPVItem->Position.xRot -= ANGLE(1.0f);
				else
					UPVItem->Position.xRot -= ANGLE(0.1f);
			}
			else if (ax)
			{
				if (ax > ANGLE(1.0f))
					UPVItem->Position.xRot += ANGLE(1.0f);
				else
					UPVItem->Position.xRot += ANGLE(0.1f);
			}
			else
				UPVItem->Position.xRot = SURFACE_ANGLE;
		}
		else
		{
			if (TrInput & UPV_IN_UP)
				UPVInfo->RotX -= UPDOWN_ACCEL;
			else if (TrInput & UPV_IN_DOWN)
				UPVInfo->RotX += UPDOWN_ACCEL;
		}

		if (TrInput & UPV_IN_PROPEL)
		{
			if (TrInput & UPV_IN_UP &&
				UPVInfo->Flags & UPV_SURFACE &&
				UPVItem->Position.xRot > -DIVE_ANGLE)
			{
				UPVInfo->Flags |= UPV_DIVE;
			}

			UPVInfo->Vel += ACCELERATION;
		}

		else
			laraItem->TargetState = UPV_STATE_IDLE;

		break;

	case UPV_STATE_IDLE:
		if (laraItem->HitPoints <= 0)
		{
			laraItem->TargetState = UPV_STATE_DEATH;
			break;
		}

		if (TrInput & UPV_IN_LEFT)
			UPVInfo->Rot -= ROT_SLOWACCEL;
		else if (TrInput & UPV_IN_RIGHT)
			UPVInfo->Rot += ROT_SLOWACCEL;

		if (UPVInfo->Flags & UPV_SURFACE)
		{
			int xa = UPVItem->Position.xRot - SURFACE_ANGLE;
			int ax = SURFACE_ANGLE - UPVItem->Position.xRot;
			if (xa > 0)
			{
				if (xa > ANGLE(1.0f))
					UPVItem->Position.xRot -= ANGLE(1.0f);
				else
					UPVItem->Position.xRot -= ANGLE(0.1f);
			}
			else if (ax)
			{
				if (ax > ANGLE(1.0f))
					UPVItem->Position.xRot += ANGLE(1.0f);
				else
					UPVItem->Position.xRot += ANGLE(0.1f);
			}
			else
				UPVItem->Position.xRot = SURFACE_ANGLE;
		}
		else
		{
			if (TrInput & UPV_IN_UP)
				UPVInfo->RotX -= UPDOWN_ACCEL;
			else if (TrInput & UPV_IN_DOWN)
				UPVInfo->RotX += UPDOWN_ACCEL;
		}

		if (TrInput & UPV_IN_DISMOUNT && TestUPVDismount(laraItem, UPVItem))
		{
			if (UPVInfo->Vel > 0)
				UPVInfo->Vel -= ACCELERATION;
			else
			{
				if (UPVInfo->Flags & UPV_SURFACE)
					laraItem->TargetState = UPV_STATE_DISMOUNT_SURFACE;
				else
					laraItem->TargetState = UPV_STATE_DISMOUNT_UNDERWATER;

				//sub->Flags &= ~UPV_CONTROL; having this here causes the UPV glitch, moving it directly to the states' code is better

				StopSoundEffect(SFX_TR3_LITTLE_SUB_LOOP);
				SoundEffect(SFX_TR3_LITTLE_SUB_STOP, (PHD_3DPOS*)&UPVItem->Position.xPos, 2);
			}
		}

		else if (TrInput & UPV_IN_PROPEL)
		{
			if (TrInput & UPV_IN_UP &&
				UPVInfo->Flags & UPV_SURFACE &&
				UPVItem->Position.xRot > -DIVE_ANGLE)
			{
				UPVInfo->Flags |= UPV_DIVE;
			}

			laraItem->TargetState = UPV_STATE_MOVE;
		}

		break;

	case UPV_STATE_MOUNT:
		if (anim == UPV_ANIM_MOUNT_SURFACE_END)
		{
			UPVItem->Position.yPos += 4;
			UPVItem->Position.xRot += ANGLE(1.0f);

			if (frame == MOUNT_SURFACE_SOUND_FRAME)
				SoundEffect(SFX_TR3_LITTLE_SUB_LOOP, (PHD_3DPOS*)&UPVItem->Position.xPos, 2);

			if (frame == MOUNT_SURFACE_CONTROL_FRAME)
				UPVInfo->Flags |= UPV_CONTROL;
		}

		else if (anim == UPV_ANIM_MOUNT_UNDERWATER)
		{
			if (frame == MOUNT_UNDERWATER_SOUND_FRAME)
				SoundEffect(SFX_TR3_LITTLE_SUB_LOOP, (PHD_3DPOS*)&UPVItem->Position.xPos, 2);

			if (frame == MOUNT_UNDERWATER_CONTROL_FRAME)
				UPVInfo->Flags |= UPV_CONTROL;
		}

		break;

	case UPV_STATE_DISMOUNT_UNDERWATER:
		if (anim == UPV_ANIM_DISMOUNT_UNDERWATER && frame == DISMOUNT_UNDERWATER_FRAME)
		{
			UPVInfo->Flags &= ~UPV_CONTROL;
			PHD_VECTOR vec = { 0, 0, 0 };
			GAME_VECTOR VPos, LPos;

			GetLaraJointPosition(&vec, LM_HIPS);

			LPos.x = vec.x;
			LPos.y = vec.y;
			LPos.z = vec.z;
			LPos.roomNumber = UPVItem->RoomNumber;
			VPos.x = UPVItem->Position.xPos;
			VPos.y = UPVItem->Position.yPos;
			VPos.z = UPVItem->Position.zPos;
			VPos.roomNumber = UPVItem->RoomNumber;
			LOSAndReturnTarget(&VPos, &LPos, 0);

			laraItem->Position.xPos = LPos.x;
			laraItem->Position.yPos = LPos.y;
			laraItem->Position.zPos = LPos.z;

			SetAnimation(laraItem, LA_UNDERWATER_IDLE);
			laraItem->VerticalVelocity = 0;
			laraItem->Airborne = false;
			laraItem->Position.xRot = laraItem->Position.zRot = 0;

			UpdateItemRoom(laraItem, 0);

			laraInfo->waterStatus = LW_UNDERWATER;
			laraInfo->gunStatus = LG_HANDS_FREE;
			laraInfo->Vehicle = NO_ITEM;

			UPVItem->HitPoints = 0;
		}

		break;

	case UPV_STATE_DISMOUNT_SURFACE:
		if (anim == UPV_ANIM_DISMOUNT_SURFACE && frame == DISMOUNT_SURFACE_FRAME)
		{
			UPVInfo->Flags &= ~UPV_CONTROL;
			int waterDepth, waterHeight, heightFromWater;
			PHD_VECTOR vec = { 0, 0, 0 };

			waterDepth = GetWaterSurface(laraItem);
			waterHeight = GetWaterHeight(laraItem);

			if (waterHeight != NO_HEIGHT)
				heightFromWater = laraItem->Position.yPos - waterHeight;
			else
				heightFromWater = NO_HEIGHT;

			GetLaraJointPosition(&vec, LM_HIPS);

			laraItem->Position.xPos = vec.x;
			//laraItem->pos.yPos += -heightFromWater + 1; // Doesn't work as intended.
			laraItem->Position.yPos = vec.y;
			laraItem->Position.zPos = vec.z;

			SetAnimation(laraItem, LA_ONWATER_IDLE);
			laraItem->VerticalVelocity = 0;
			laraItem->Airborne = false;
			laraItem->Position.xRot = laraItem->Position.zRot = 0;

			UpdateItemRoom(laraItem, -LARA_HEIGHT / 2);

			laraInfo->waterStatus = LW_SURFACE;
			laraInfo->waterSurfaceDist = -heightFromWater;
			laraInfo->Control.DiveCount = 11;
			ResetLaraFlex(laraItem);
			laraInfo->gunStatus = LG_HANDS_FREE;
			laraInfo->Vehicle = NO_ITEM;

			UPVItem->HitPoints = 0;
		}
		else
		{
			UPVInfo->RotX -= UPDOWN_ACCEL;
			if (UPVItem->Position.xRot < 0)
				UPVItem->Position.xRot = 0;
		}

		break;

	case UPV_STATE_DEATH:
		if (anim == UPV_ANIM_DEATH && (frame == DEATH_FRAME_1 || frame == DEATH_FRAME_2))
		{
			PHD_VECTOR vec = { 0, 0, 0 };

			GetLaraJointPosition(&vec, LM_HIPS);

			laraItem->Position.xPos = vec.x;
			laraItem->Position.yPos = vec.y;
			laraItem->Position.zPos = vec.z;
			laraItem->Position.xRot = 0;
			laraItem->Position.zRot = 0;

			SetAnimation(UPVItem, LA_UNDERWATER_DEATH, 17);
			laraItem->VerticalVelocity = 0;
			laraItem->Airborne = 0;
			
			UPVInfo->Flags |= UPV_DEAD;
		}

		UPVItem->VerticalVelocity = 0;
		break;
	}

	if (UPVInfo->Flags & UPV_DIVE)
	{
		if (UPVItem->Position.xRot > -DIVE_ANGLE)
			UPVItem->Position.xRot -= DIVE_SPEED;
		else
			UPVInfo->Flags &= ~UPV_DIVE;
	}

	if (UPVInfo->Vel > 0)
	{
		UPVInfo->Vel -= FRICTION;
		if (UPVInfo->Vel < 0)
			UPVInfo->Vel = 0;
	}
	else if (UPVInfo->Vel < 0)
	{
		UPVInfo->Vel += FRICTION;
		if (UPVInfo->Vel > 0)
			UPVInfo->Vel = 0;
	}

	if (UPVInfo->Vel > MAX_SPEED)
		UPVInfo->Vel = MAX_SPEED;
	else if (UPVInfo->Vel < -MAX_SPEED)
		UPVInfo->Vel = -MAX_SPEED;

	if (UPVInfo->Rot > 0)
	{
		UPVInfo->Rot -= ROT_FRICTION;
		if (UPVInfo->Rot < 0)
			UPVInfo->Rot = 0;
	}
	else if (UPVInfo->Rot < 0)
	{
		UPVInfo->Rot += ROT_FRICTION;
		if (UPVInfo->Rot > 0)
			UPVInfo->Rot = 0;
	}

	if (UPVInfo->RotX > 0)
	{
		UPVInfo->RotX -= UPDOWN_FRICTION;
		if (UPVInfo->RotX < 0)
			UPVInfo->RotX = 0;
	}
	else if (UPVInfo->RotX < 0)
	{
		UPVInfo->RotX += UPDOWN_FRICTION;
		if (UPVInfo->RotX > 0)
			UPVInfo->RotX = 0;
	}

	if (UPVInfo->Rot > MAX_ROTATION)
		UPVInfo->Rot = MAX_ROTATION;
	else if (UPVInfo->Rot < -MAX_ROTATION)
		UPVInfo->Rot = -MAX_ROTATION;

	if (UPVInfo->RotX > MAX_UPDOWN)
		UPVInfo->RotX = MAX_UPDOWN;
	else if (UPVInfo->RotX < -MAX_UPDOWN)
		UPVInfo->RotX = -MAX_UPDOWN;
}

void NoGetOnCollision(short itemNum, ITEM_INFO* laraitem, COLL_INFO* coll)
{
	ITEM_INFO* item = &g_Level.Items[itemNum];

	if (!TestBoundsCollide(item, laraitem, coll->Setup.Radius))
		return;
	if (!TestCollision(item, laraitem))
		return;

	ItemPushItem(item, laraitem, coll, 0, 0);
}

void SubCollision(short itemNum, ITEM_INFO* laraItem, COLL_INFO* coll)
{
	LaraInfo*& laraInfo = laraItem->Data;

	if (laraItem->HitPoints <= 0 || laraInfo->Vehicle != NO_ITEM)
		return;

	ITEM_INFO* UPVItem = &g_Level.Items[itemNum];

	if (TestUPVMount(laraItem, UPVItem))
	{
		laraInfo->Vehicle = itemNum;
		laraInfo->waterStatus = LW_ABOVE_WATER;

		if (laraInfo->gunType == WEAPON_FLARE)
		{
			CreateFlare(LaraItem, ID_FLARE_ITEM, 0);
			UndrawFlareMeshes(laraItem);

			laraInfo->Flare.ControlLeft = false;
			laraInfo->requestGunType = laraInfo->gunType = WEAPON_NONE;
		}

		laraInfo->gunStatus = LG_HANDS_BUSY;
		laraItem->Position.xPos = UPVItem->Position.xPos;
		laraItem->Position.yPos = UPVItem->Position.yPos;
		laraItem->Position.zPos = UPVItem->Position.zPos;
		laraItem->Position.xRot = UPVItem->Position.xRot;
		laraItem->Position.yRot = UPVItem->Position.yRot;
		laraItem->Position.zRot = UPVItem->Position.zRot;
		UPVItem->HitPoints = 1;

		if (laraItem->ActiveState == LS_ONWATER_STOP || laraItem->ActiveState == LS_ONWATER_FORWARD)
		{
			laraItem->AnimNumber = Objects[ID_UPV_LARA_ANIMS].animIndex + UPV_ANIM_MOUNT_SURFACE_START;
			laraItem->ActiveState = laraItem->TargetState = UPV_STATE_MOUNT;
		}
		else
		{
			laraItem->AnimNumber = Objects[ID_UPV_LARA_ANIMS].animIndex + UPV_ANIM_MOUNT_UNDERWATER;
			laraItem->ActiveState = laraItem->TargetState = UPV_STATE_MOUNT;
		}
		laraItem->FrameNumber = g_Level.Anims[laraItem->AnimNumber].frameBase;

		AnimateItem(laraItem);
	}
	else
	{
		UPVItem->Position.yPos += SUB_DRAW_SHIFT;
		NoGetOnCollision(itemNum, laraItem, coll);
		UPVItem->Position.yPos -= SUB_DRAW_SHIFT;
	}
}

bool SubControl(ITEM_INFO* laraItem, COLL_INFO* coll)
{
	LaraInfo*& laraInfo = laraItem->Data;
	ITEM_INFO* UPVItem = &g_Level.Items[laraInfo->Vehicle];
	SUB_INFO* UPVInfo = UPVItem->Data;
	
	auto oldPos = UPVItem->Position;
	auto probe = GetCollisionResult(UPVItem);

	if (!(UPVInfo->Flags & UPV_DEAD))
	{
		UserInput(laraItem, UPVItem);

		UPVItem->VerticalVelocity = UPVInfo->Vel / 65536;

		UPVItem->Position.xRot += UPVInfo->RotX / 65536;
		UPVItem->Position.yRot += UPVInfo->Rot / 65536;
		UPVItem->Position.zRot = UPVInfo->Rot / 65536;

		if (UPVItem->Position.xRot > UPDOWN_LIMIT)
			UPVItem->Position.xRot = UPDOWN_LIMIT;
		else if (UPVItem->Position.xRot < -UPDOWN_LIMIT)
			UPVItem->Position.xRot = -UPDOWN_LIMIT;

		UPVItem->Position.xPos += phd_sin(UPVItem->Position.yRot) * UPVItem->VerticalVelocity * phd_cos(UPVItem->Position.xRot);
		UPVItem->Position.yPos -= phd_sin(UPVItem->Position.xRot) * UPVItem->VerticalVelocity;
		UPVItem->Position.zPos += phd_cos(UPVItem->Position.yRot) * UPVItem->VerticalVelocity * phd_cos(UPVItem->Position.xRot);
	}

	int newHeight = GetCollisionResult(UPVItem).Position.Floor;
	int waterHeight = GetWaterHeight(UPVItem);

	if ((newHeight - waterHeight) < SUB_HEIGHT || (newHeight < UPVItem->Position.yPos - SUB_HEIGHT / 2))
	{
		UPVItem->Position.xPos = oldPos.xPos;
		UPVItem->Position.yPos = oldPos.yPos;
		UPVItem->Position.zPos = oldPos.zPos;
		UPVItem->VerticalVelocity = 0;
	}

	UPVItem->Floor = probe.Position.Floor;

	if (UPVInfo->Flags & UPV_CONTROL && !(UPVInfo->Flags & UPV_DEAD))
	{
		if (!(g_Level.Rooms[UPVItem->RoomNumber].flags & ENV_FLAG_WATER) &&
			waterHeight != NO_HEIGHT)
		{
			if ((waterHeight - UPVItem->Position.yPos) >= -SURFACE_DIST)
				UPVItem->Position.yPos = waterHeight + SURFACE_DIST;

			if (!(UPVInfo->Flags & UPV_SURFACE))
			{
				SoundEffect(SFX_TR4_LARA_BREATH, &LaraItem->Position, 2);
				UPVInfo->Flags &= ~UPV_DIVE;
			}

			UPVInfo->Flags |= UPV_SURFACE;
		}

		else if ((waterHeight - UPVItem->Position.yPos) >= -SURFACE_DIST && waterHeight != NO_HEIGHT)
		{
			UPVItem->Position.yPos = waterHeight + SURFACE_DIST;

			if (!(UPVInfo->Flags & UPV_SURFACE))
			{
				SoundEffect(SFX_TR4_LARA_BREATH, &LaraItem->Position, 2);
				UPVInfo->Flags &= ~UPV_DIVE;
			}

			UPVInfo->Flags |= UPV_SURFACE;
		}

		else
			UPVInfo->Flags &= ~UPV_SURFACE;

		if (!(UPVInfo->Flags & UPV_SURFACE))
		{
			if (laraItem->HitPoints > 0)
			{
				laraInfo->air--;

				if (laraInfo->air < 0)
				{
					laraInfo->air = -1;
					laraItem->HitPoints -= 5;
				}
			}
		}
		else
		{
			if (laraItem->HitPoints >= 0)
			{
				laraInfo->air += 10;

				if (laraInfo->air > 1800)
					laraInfo->air = 1800;
			}
		}
	}

	TestTriggers(UPVItem, false);
	SubEffects(laraInfo->Vehicle);

	if (!(UPVInfo->Flags & UPV_DEAD) &&
		laraInfo->Vehicle != NO_ITEM)
	{
		DoCurrent(UPVItem);

		if (TrInput & UPV_IN_FIRE &&
			UPVInfo->Flags & UPV_CONTROL &&
			!UPVInfo->WeaponTimer)
		{
			if (laraItem->ActiveState != UPV_STATE_DISMOUNT_UNDERWATER &&
				laraItem->ActiveState != UPV_STATE_DISMOUNT_SURFACE &&
				laraItem->ActiveState != UPV_STATE_MOUNT)
			{
				FireSubHarpoon(laraItem, UPVItem);
				UPVInfo->WeaponTimer = HARPOON_RELOAD;
			}
		}

		if (probe.RoomNumber != UPVItem->RoomNumber)
		{
			ItemNewRoom(laraInfo->Vehicle, probe.RoomNumber);
			ItemNewRoom(laraInfo->itemNumber, probe.RoomNumber);
		}

		laraItem->Position.xPos = UPVItem->Position.xPos;
		laraItem->Position.yPos = UPVItem->Position.yPos;
		laraItem->Position.zPos = UPVItem->Position.zPos;
		laraItem->Position.xRot = UPVItem->Position.xRot;
		laraItem->Position.yRot = UPVItem->Position.yRot;
		laraItem->Position.zRot = UPVItem->Position.zRot;

		AnimateItem(laraItem);
		BackgroundCollision(laraItem, UPVItem);

		if (UPVInfo->Flags & UPV_CONTROL)
			SoundEffect(SFX_TR3_LITTLE_SUB_LOOP, (PHD_3DPOS*)&UPVItem->Position.xPos, 2 | 4 | 0x1000000 | (UPVItem->VerticalVelocity * 65536));

		UPVItem->AnimNumber = Objects[ID_UPV].animIndex + (laraItem->AnimNumber - Objects[ID_UPV_LARA_ANIMS].animIndex);
		UPVItem->FrameNumber = g_Level.Anims[UPVItem->AnimNumber].frameBase + (laraItem->FrameNumber - g_Level.Anims[laraItem->AnimNumber].frameBase);

		if (UPVInfo->Flags & UPV_SURFACE)
			Camera.targetElevation = -ANGLE(60.0f);
		else
			Camera.targetElevation = 0;

		return true;
	}
	else if (UPVInfo->Flags & UPV_DEAD)
	{
		AnimateItem(laraItem);

		if (probe.RoomNumber != UPVItem->RoomNumber)
			ItemNewRoom(laraInfo->Vehicle, probe.RoomNumber);

		BackgroundCollision(laraItem, UPVItem);

		UPVInfo->RotX = 0;

		SetAnimation(UPVItem, UPV_ANIM_IDLE);
		UPVItem->VerticalVelocity = 0;
		UPVItem->VerticalVelocity = 0;
		UPVItem->Airborne = true;
		AnimateItem(UPVItem);

		return true;
	}
	else
		return false;
}
