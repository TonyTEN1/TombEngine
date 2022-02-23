#include "framework.h"
#include "Objects/TR3/Vehicles/minecart.h"

#include "Game/animation.h"
#include "Game/camera.h"
#include "Game/collision/sphere.h"
#include "Game/collision/collide_item.h"
#include "Game/effects/effects.h"
#include "Game/items.h"
#include "Game/Lara/lara.h"
#include "Game/Lara/lara_flare.h"
#include "Objects/TR3/Vehicles/minecart_info.h"
#include "Sound/sound.h"
#include "Specific/input.h"
#include "Specific/level.h"
#include "Specific/setup.h"

using std::vector;

enum MinecartState
{
	CART_STATE_MOUNT,
	CART_STATE_DISMOUNT,
	CART_STATE_DISMOUNT_LEFT,
	CART_STATE_DISMOUNT_RIGHT,
	CART_STATE_IDLE,
	CART_STATE_DUCK,
	CART_STATE_MOVE,
	CART_STATE_RIGHT,
	CART_STATE_HARD_LEFT,	// Unused.
	CART_STATE_LEFT,
	CART_STATE_HARD_RIGHT,	// Unused.
	CART_STATE_BRAKE,
	CART_STATE_FORWARD,
	CART_STATE_BACK,
	CART_TURN_DEATH,
	CART_FALL_DEATH,
	CART_WALL_DEATH,
	CART_STATE_HIT,
	CART_STATE_USE,
	CART_STATE_BRAKING
};

enum MinecartFlags
{
	CART_FLAG_MESH = 1,
	CART_FLAG_TURNING_LEFT = 2,
	CART_FLAG_TURNING_RIGHT = 4,
	CART_FLAG_RDIR = 8, // TODO
	CART_FLAG_CONTROL = 16,
	CART_FLAG_STOPPED = 32,
	CART_FLAG_NO_ANIM = 64,
	CART_FLAG_DEAD = 128
};

#define DISMOUNT_DISTANCE 330
#define CART_DEC -1536
#define CART_MIN_SPEED 2560
#define CART_MIN_VEL 32
#define TURN_DEATH_VEL 128
#define CART_FORWARD_GRADIENT -128
#define CART_BACK_GRADIENT 128
#define CART_JUMP_VELOCITY 64512
#define CART_GRAVITY (SECTOR(1) + 1)
#define MAX_CART_YVEL 16128
#define TERMINAL_ANGLE SECTOR(4)
#define CART_RADIUS 100
#define CART_HEIGHT CLICK(3)
#define CART_NHITS 25
#define CART_ENTITY_RADIUS CLICK(1)

#define CART_IN_SWITCH		IN_ACTION
#define CART_IN_DISMOUNT	IN_ROLL
#define CART_IN_DUCK		IN_CROUCH
#define CART_IN_BRAKE		(IN_BACK | IN_JUMP)
#define CART_IN_LEFT		IN_LEFT
#define CART_IN_RIGHT		IN_RIGHT

void InitialiseMineCart(short itemNumber)
{
	auto* cartItem = &g_Level.Items[itemNumber];
	cartItem->Data = MinecartInfo();
	auto* cartInfo = (MinecartInfo*)cartItem->Data;

	cartInfo->Velocity = 0;
	cartInfo->YVel = 0;
	cartInfo->Gradient = 0;
	cartInfo->Flags = NULL;
}

static int TestMinecartHeight(ITEM_INFO* cartItem, int xOffset, int zOffset)
{
	float c = phd_cos(cartItem->Position.yRot);
	float s = phd_sin(cartItem->Position.yRot);

	PHD_VECTOR pos;
	pos.x = cartItem->Position.xPos + zOffset * s + xOffset * c;
	pos.y = cartItem->Position.yPos - zOffset * phd_sin(cartItem->Position.xRot) + xOffset * phd_sin(cartItem->Position.zRot);
	pos.z = cartItem->Position.zPos + zOffset * c - xOffset * s;

	return GetCollisionResult(pos.x, pos.y, pos.z, cartItem->RoomNumber).Position.Floor;
}

static short GetMinecartCollision(ITEM_INFO* cartItem, short angle, int distance, short* ceiling)
{
	auto probe = GetCollisionResult(cartItem, angle, distance, -LARA_HEIGHT);

	*ceiling = (short)probe.Position.Ceiling;

	if (probe.Position.Floor != NO_HEIGHT)
		probe.Position.Floor -= cartItem->Position.yPos;

	return (short)probe.Position.Floor;
}

static bool GetInMineCart(ITEM_INFO* cartItem, ITEM_INFO* laraItem, COLL_INFO* coll)
{
	if (!(TrInput & CART_IN_SWITCH) || Lara.Control.HandStatus != HandStatus::Free ||
		laraItem->Airborne)
	{
		return false;
	}

	if (!TestBoundsCollide(cartItem, laraItem, coll->Setup.Radius))
		return false;

	if (!TestCollision(cartItem, laraItem))
		return false;

	int x = laraItem->Position.xPos - cartItem->Position.xPos;
	int z = laraItem->Position.zPos - cartItem->Position.zPos;

	int distance = pow(x, 2) + pow(z, 2);
	if (distance > pow(CLICK(2), 2))
		return false;

	if (GetCollisionResult(cartItem).Position.Floor < -32000)
		return false;

	return true;
}

static bool CanGetOut(int direction)
{
	auto v = &g_Level.Items[Lara.Vehicle];

	short angle;
	if (direction < 0)
		angle = v->Position.yRot + 0x4000;
	else
		angle = v->Position.yRot - 0x4000;

	int x = v->Position.xPos - DISMOUNT_DISTANCE * phd_sin(angle);
	int y = v->Position.yPos;
	int z = v->Position.zPos - DISMOUNT_DISTANCE * phd_cos(angle);

	auto collResult = GetCollisionResult(x, y, z, v->RoomNumber);

	if (collResult.Position.FloorSlope || collResult.Position.Floor == NO_HEIGHT)
		return false;

	if (abs(collResult.Position.Floor - v->Position.yPos) > SECTOR(0.5f))
		return false;

	if ((collResult.Position.Ceiling - v->Position.yPos > -LARA_HEIGHT) || (collResult.Position.Floor - collResult.Position.Ceiling < LARA_HEIGHT))
		return false;

	return true;
}

static void CartToEntityCollision(ITEM_INFO* cartItem)
{
	vector<short> roomsList;
	roomsList.push_back(cartItem->RoomNumber);

	auto* room = &g_Level.Rooms[cartItem->RoomNumber];
	for (int i = 0; i < room->doors.size(); i++)
		roomsList.push_back(room->doors[i].room);

	for (int i = 0; i < roomsList.size(); i++)
	{
		short itemNumber = g_Level.Rooms[roomsList[i]].itemNumber;

		while (itemNumber != NO_ITEM)
		{
			auto* item = &g_Level.Items[itemNumber];
			if (item->Collidable && item->Status != ITEM_INVISIBLE && item != LaraItem && item != cartItem)
			{
				auto* object = &Objects[item->ObjectNumber];
				if (object->collision && (object->intelligent || item->ObjectNumber == ID_ROLLINGBALL || item->ObjectNumber == ID_ANIMATING2))
				{
					int x = cartItem->Position.xPos - item->Position.xPos;
					int y = cartItem->Position.yPos - item->Position.yPos;
					int z = cartItem->Position.zPos - item->Position.zPos;
					if (x > -2048 && x < 2048 &&
						y > -2048 && y < 2048 &&
						z > -2048 && z < 2048)
					{
						if (TestBoundsCollide(item, LaraItem, CART_ENTITY_RADIUS))
						{
							if (item->ObjectNumber == ID_ANIMATING2)
							{
								if (item->FrameNumber == g_Level.Anims[item->AnimNumber].frameBase &&
									(LaraItem->ActiveState == CART_STATE_USE &&
										LaraItem->AnimNumber == Objects[ID_MINECART_LARA_ANIMS].animIndex + 6))
								{
									int frame = LaraItem->FrameNumber - g_Level.Anims[LaraItem->AnimNumber].frameBase;
									if (frame >= 12 && frame <= 22)
									{
										SoundEffect(220, &item->Position, 2);
										TestTriggers(item, true);
										item->FrameNumber++;
									}
								}
							}
							else if (item->ObjectNumber == ID_ROLLINGBALL)
							{
								/*code, kill lara and stop both the boulder and the minecart*/
							}
							else
							{
								DoLotsOfBlood(item->Position.xPos, cartItem->Position.yPos - CLICK(1), item->Position.zPos, GetRandomControl() & 3, cartItem->Position.yRot, item->RoomNumber, 3);
								item->HitPoints = 0;
							}
						}
					}
				}
			}

			itemNumber = item->NextItem;
		}
	}
}

static void MoveCart(ITEM_INFO* cartItem, ITEM_INFO* laraItem, MinecartInfo* cartInfo)
{
	if (cartInfo->StopDelay)
		cartInfo->StopDelay--;

	if ((Lara.mineL && Lara.mineR && !cartInfo->StopDelay) &&
		(cartItem->Position.xPos & 0x380 == 512 ||
			cartItem->Position.zRot & 0x380 == 512))
	{
		if (cartInfo->Velocity < 0xf000)
		{
			cartInfo->Velocity = 0;
			cartInfo->Flags |= CART_FLAG_STOPPED | CART_FLAG_CONTROL;
			return;
		}
		else
			cartInfo->StopDelay = 16;
	}

	if ((Lara.mineL || Lara.mineR) && !(Lara.mineL && Lara.mineR) &&
		!cartInfo->StopDelay &&
		!(cartInfo->Flags & (CART_FLAG_TURNING_LEFT | CART_FLAG_TURNING_RIGHT)))
	{
		short angle;
		unsigned short rotation = (((unsigned short)cartItem->Position.yRot) / 16384) | (Lara.mineL * 4);

		switch (rotation)
		{
		case 0:
			cartInfo->TurnX = (cartItem->Position.xPos + 4096) & ~1023;
			cartInfo->TurnZ = cartItem->Position.zPos & ~1023;
			break;
		case 1:
			cartInfo->TurnX = cartItem->Position.xPos & ~1023;
			cartInfo->TurnZ = (cartItem->Position.zPos - 4096) | 1023;
			break;
		case 2:
			cartInfo->TurnX = (cartItem->Position.xPos - 4096) | 1023;
			cartInfo->TurnZ = cartItem->Position.zPos | 1023;
			break;
		case 3:
			cartInfo->TurnX = cartItem->Position.xPos | 1023;
			cartInfo->TurnZ = (cartItem->Position.zPos + 4096) & ~1023;
			break;

		case 4:
			cartInfo->TurnX = (cartItem->Position.xPos - 4096) | 1023;
			cartInfo->TurnZ = cartItem->Position.zPos & ~1023;
			break;
		case 5:
			cartInfo->TurnX = cartItem->Position.xPos & ~1023;
			cartInfo->TurnZ = (cartItem->Position.zPos + 4096) & ~1023;
			break;
		case 6:
			cartInfo->TurnX = (cartItem->Position.xPos + 4096) & ~1023;
			cartInfo->TurnZ = cartItem->Position.zPos | 1023;
			break;
		case 7:
			cartInfo->TurnX = cartItem->Position.xPos | 1023;
			cartInfo->TurnZ = (cartItem->Position.zPos - 4096) | 1023;
			break;
		}

		angle = mGetAngle(cartItem->Position.xPos, cartItem->Position.zPos, cartInfo->TurnX, cartInfo->TurnZ) & 0x3fff;

		if (rotation < 4)
		{
			cartInfo->TurnRot = cartItem->Position.yRot;
			cartInfo->TurnLen = angle;
		}
		else
		{
			cartInfo->TurnRot = cartItem->Position.yRot;

			if (angle)
				angle = 16384 - angle;

			cartInfo->TurnLen = angle;
		}

		cartInfo->Flags |= (Lara.mineL) ? CART_FLAG_TURNING_LEFT : CART_FLAG_TURNING_RIGHT;
	}

	if (cartInfo->Velocity < CART_MIN_SPEED)
		cartInfo->Velocity = CART_MIN_SPEED;

	cartInfo->Velocity += -cartInfo->Gradient * 4;

	cartInfo->Velocity /= 256;
	if (cartItem->Velocity < CART_MIN_VEL)
	{
		cartItem->Velocity = CART_MIN_VEL;
		StopSoundEffect(209);

		if (cartInfo->YVel)
			StopSoundEffect(210);
		else
			SoundEffect(210, &cartItem->Position, 2);
	}
	else
	{
		StopSoundEffect(210);

		if (cartInfo->YVel)
			StopSoundEffect(209);
		else
			SoundEffect(209, &cartItem->Position, (2 | 4) + 0x1000000 + (cartItem->Velocity * 32768));
	}

	if (cartInfo->Flags & (CART_FLAG_TURNING_LEFT | CART_FLAG_TURNING_RIGHT))
	{
		if ((cartInfo->TurnLen += (cartItem->Velocity * 3)) > ANGLE(90.0f))
		{
			if (cartInfo->Flags & CART_FLAG_TURNING_LEFT)
				cartItem->Position.yRot = cartInfo->TurnRot - 16384;
			else
				cartItem->Position.yRot = cartInfo->TurnRot + 16384;

			cartInfo->Flags &= ~(CART_FLAG_TURNING_LEFT | CART_FLAG_TURNING_RIGHT);
		}
		else
		{
			if (cartInfo->Flags & CART_FLAG_TURNING_LEFT)
				cartItem->Position.yRot = cartInfo->TurnRot - cartInfo->TurnLen;
			else
				cartItem->Position.yRot = cartInfo->TurnRot + cartInfo->TurnLen;
		}

		if (cartInfo->Flags & (CART_FLAG_TURNING_LEFT | CART_FLAG_TURNING_RIGHT))
		{
			unsigned short quadrant = ((unsigned short)cartItem->Position.yRot) / 16384;
			unsigned short degree = cartItem->Position.yRot & 16383;

			float x, z;
			switch (quadrant)
			{
			case 0:
				x = -phd_cos(degree);
				z = phd_sin(degree);
				break;
			case 1:
				x = phd_sin(degree);
				z = phd_cos(degree);
				break;
			case 2:
				x = phd_cos(degree);
				z = -phd_sin(degree);
				break;
			default:
				x = -phd_sin(degree);
				z = -phd_cos(degree);
				break;
			}

			if (cartInfo->Flags & CART_FLAG_TURNING_LEFT)
			{
				x = -x;
				z = -z;
			}

			cartItem->Position.xPos = cartInfo->TurnX + x * 3584;
			cartItem->Position.zPos = cartInfo->TurnZ + z * 3584;
		}
	}
	else
	{
		cartItem->Position.xPos += cartItem->Velocity * phd_sin(cartItem->Position.yRot);
		cartItem->Position.zPos += cartItem->Velocity * phd_cos(cartItem->Position.yRot);
	}

	cartInfo->MidPos = TestMinecartHeight(cartItem, 0, 0);

	if (!cartInfo->YVel)
	{
		cartInfo->FrontPos = TestMinecartHeight(cartItem, 0, CLICK(1));
		cartInfo->Gradient = cartInfo->MidPos - cartInfo->FrontPos;
		cartItem->Position.yPos = cartInfo->MidPos;
	}
	else
	{
		if (cartItem->Position.yPos > cartInfo->MidPos)
		{
			if (cartInfo->YVel > 0)
				SoundEffect(202, &cartItem->Position, 2);

			cartItem->Position.yPos = cartInfo->MidPos;
			cartInfo->YVel = 0;
		}
		else
		{
			cartInfo->YVel += CART_GRAVITY;
			if (cartInfo->YVel > MAX_CART_YVEL)
				cartInfo->YVel = MAX_CART_YVEL;

			cartItem->Position.yPos += cartInfo->YVel / 256;
		}
	}

	cartItem->Position.xRot = cartInfo->Gradient * 32;

	if (cartInfo->Flags & (CART_FLAG_TURNING_LEFT | CART_FLAG_TURNING_RIGHT))
	{
		short val = cartItem->Position.yRot & 16383;
		if (cartInfo->Flags & CART_FLAG_TURNING_RIGHT)
			cartItem->Position.zRot = -(val * cartItem->Velocity) / 512;
		else
			cartItem->Position.zRot = ((0x4000 - val) * cartItem->Velocity) / 512;
	}
	else
		cartItem->Position.zRot -= cartItem->Position.zRot / 8;
}

static void DoUserInput(ITEM_INFO* cartItem, ITEM_INFO* laraItem, MinecartInfo* cartInfo)
{
	short floorHeight, ceilingHeight;

	switch (laraItem->ActiveState)
	{
	case CART_STATE_MOVE:
		if (TrInput & CART_IN_SWITCH)
			laraItem->TargetState = CART_STATE_USE;
		else if (TrInput & CART_IN_DUCK)
			laraItem->TargetState = CART_STATE_DUCK;
		else if (TrInput & CART_IN_BRAKE)
			laraItem->TargetState = CART_STATE_BRAKE;
		else if (cartInfo->Velocity == CART_MIN_VEL || cartInfo->Flags & CART_FLAG_STOPPED)
			laraItem->TargetState = CART_STATE_IDLE;
		else if (cartInfo->Gradient < CART_FORWARD_GRADIENT)
			laraItem->TargetState = CART_STATE_FORWARD;
		else if (cartInfo->Gradient > CART_BACK_GRADIENT)
			laraItem->TargetState = CART_STATE_BACK;
		else if (TrInput & CART_IN_LEFT)
			laraItem->TargetState = CART_STATE_LEFT;
		else if (TrInput & CART_IN_RIGHT)
			laraItem->TargetState = CART_STATE_RIGHT;

		break;

	case CART_STATE_FORWARD:
		if (TrInput & CART_IN_SWITCH)
			laraItem->TargetState = CART_STATE_USE;
		else if (TrInput & CART_IN_DUCK)
			laraItem->TargetState = CART_STATE_DUCK;
		else if (TrInput & CART_IN_BRAKE)
			laraItem->TargetState = CART_STATE_BRAKE;
		else if (cartInfo->Gradient > CART_FORWARD_GRADIENT)
			laraItem->TargetState = CART_STATE_MOVE;

		break;

	case CART_STATE_BACK:
		if (TrInput & CART_IN_SWITCH)
			laraItem->TargetState = CART_STATE_USE;
		else if (TrInput & CART_IN_DUCK)
			laraItem->TargetState = CART_STATE_DUCK;
		else if (TrInput & CART_IN_BRAKE)
			laraItem->TargetState = CART_STATE_BRAKE;
		else if (cartInfo->Gradient < CART_BACK_GRADIENT)
			laraItem->TargetState = CART_STATE_MOVE;

		break;

	case CART_STATE_LEFT:
		if (TrInput & CART_IN_SWITCH)
			laraItem->TargetState = CART_STATE_USE;
		else if (TrInput & CART_IN_DUCK)
			laraItem->TargetState = CART_STATE_DUCK;
		else if (TrInput & CART_IN_BRAKE)
			laraItem->TargetState = CART_STATE_BRAKE;

		if (!(TrInput & CART_IN_LEFT))
			laraItem->TargetState = CART_STATE_MOVE;

		break;

	case CART_STATE_RIGHT:
		if (TrInput & CART_IN_SWITCH)
			laraItem->TargetState = CART_STATE_USE;
		else if (TrInput & CART_IN_DUCK)
			laraItem->TargetState = CART_STATE_DUCK;
		else if (TrInput & CART_IN_BRAKE)
			laraItem->TargetState = CART_STATE_BRAKE;

		if (!(TrInput & CART_IN_RIGHT))
			laraItem->TargetState = CART_STATE_MOVE;

		break;

	case CART_STATE_IDLE:
		if (!(cartInfo->Flags & CART_FLAG_CONTROL))
		{
			SoundEffect(211, &cartItem->Position, 2);
			cartInfo->Flags |= CART_FLAG_CONTROL;
			cartInfo->StopDelay = 64;
		}

		if (TrInput & CART_IN_DISMOUNT && cartInfo->Flags & CART_FLAG_STOPPED)
		{
			if (TrInput & CART_IN_LEFT && CanGetOut(-1))
			{
				laraItem->TargetState = CART_STATE_DISMOUNT;
				cartInfo->Flags &= ~CART_FLAG_RDIR;
			}
			else if (TrInput & CART_IN_RIGHT && CanGetOut(1))
			{
				laraItem->TargetState = CART_STATE_DISMOUNT;
				cartInfo->Flags |= CART_FLAG_RDIR;
			}
		}

		if (TrInput & CART_IN_DUCK)
			laraItem->TargetState = CART_STATE_DUCK;
		else if (cartInfo->Velocity > CART_MIN_VEL)
			laraItem->TargetState = CART_STATE_MOVE;

		break;

	case CART_STATE_DUCK:
		if (TrInput & CART_IN_SWITCH)
			laraItem->TargetState = CART_STATE_USE;
		else if (TrInput & CART_IN_BRAKE)
			laraItem->TargetState = CART_STATE_BRAKE;
		else if (!(TrInput & CART_IN_DUCK))
			laraItem->TargetState = CART_STATE_IDLE;

		break;

	case CART_STATE_USE:
		laraItem->TargetState = CART_STATE_MOVE;
		break;

	case CART_STATE_BRAKING:
		if (TrInput & CART_IN_DUCK)
		{
			laraItem->TargetState = CART_STATE_DUCK;
			StopSoundEffect(219);
		}
		else if (!(TrInput & CART_IN_BRAKE) || cartInfo->Flags & CART_FLAG_STOPPED)
		{
			laraItem->TargetState = CART_STATE_MOVE;
			StopSoundEffect(219);
		}
		else
		{
			cartInfo->Velocity += CART_DEC;
			SoundEffect(219, &laraItem->Position, 2);
		}

		break;

	case CART_STATE_BRAKE:
		laraItem->TargetState = CART_STATE_BRAKING;
		break;

	case CART_STATE_DISMOUNT:
		if (laraItem->AnimNumber == Objects[ID_MINECART_LARA_ANIMS].animIndex + 7)
		{
			if ((laraItem->FrameNumber == GetFrameNumber(cartItem, 20)) && (cartInfo->Flags & CART_FLAG_MESH))
			{
				Lara.meshPtrs[LM_RHAND] = Objects[ID_MINECART_LARA_ANIMS].meshIndex + LM_RHAND;
				cartInfo->Flags &= ~CART_FLAG_MESH;
			}

			if (cartInfo->Flags & CART_FLAG_RDIR)
				laraItem->TargetState = CART_STATE_DISMOUNT_RIGHT;
			else
				laraItem->TargetState = CART_STATE_DISMOUNT_LEFT;
		}

		break;

	case CART_STATE_DISMOUNT_LEFT:
		if (laraItem->AnimNumber == Objects[ID_MINECART_LARA_ANIMS].animIndex + 1 &&
			laraItem->FrameNumber == g_Level.Anims[laraItem->AnimNumber].frameEnd)
		{
			PHD_VECTOR vec = { 0, 640, 0 };
			GetLaraJointPosition(&vec, LM_HIPS);

			laraItem->Position.xPos = vec.x;
			laraItem->Position.yPos = vec.y;
			laraItem->Position.zPos = vec.z;
			laraItem->Position.xRot = 0;
			laraItem->Position.yRot = cartItem->Position.yRot + 0x4000;
			laraItem->Position.zRot = 0;

			SetAnimation(laraItem, LA_STAND_SOLID);
			Lara.Vehicle = NO_ITEM;
			Lara.Control.HandStatus = HandStatus::Free;
		}

		break;

	case CART_STATE_DISMOUNT_RIGHT:
		if (laraItem->AnimNumber == Objects[ID_MINECART_LARA_ANIMS].animIndex + 47 &&
			laraItem->FrameNumber == g_Level.Anims[laraItem->AnimNumber].frameEnd)
		{
			PHD_VECTOR vec = { 0, 640, 0 };
			GetLaraJointPosition(&vec, LM_HIPS);

			laraItem->Position.xPos = vec.x;
			laraItem->Position.yPos = vec.y;
			laraItem->Position.zPos = vec.z;
			laraItem->Position.xRot = 0;
			laraItem->Position.yRot = cartItem->Position.yRot - 0x4000;
			laraItem->Position.zRot = 0;

			SetAnimation(laraItem, LA_STAND_SOLID);
			Lara.Vehicle = NO_ITEM;
			Lara.Control.HandStatus = HandStatus::Free;
		}

		break;

	case CART_STATE_MOUNT:
		if (laraItem->AnimNumber == Objects[ID_MINECART_LARA_ANIMS].animIndex + 5 &&
			laraItem->FrameNumber == GetFrameNumber(cartItem, 20) &&
			!cartInfo->Flags & CART_FLAG_MESH)
		{
			MESH tmp = g_Level.Meshes[Lara.meshPtrs[LM_RHAND]];

			Lara.meshPtrs[LM_RHAND] = Objects[ID_MINECART_LARA_ANIMS].meshIndex + LM_RHAND;
			g_Level.Meshes[Objects[ID_MINECART_LARA_ANIMS].meshIndex + LM_RHAND] = tmp;

			cartInfo->Flags |= CART_FLAG_MESH;
		}

		break;

	case CART_WALL_DEATH:
		Camera.targetElevation = -ANGLE(25.0f);
		Camera.targetDistance = SECTOR(4);

		break;

	case CART_TURN_DEATH:
		Camera.targetElevation = -ANGLE(45.0f);
		Camera.targetDistance = SECTOR(2);

		ceilingHeight;
		floorHeight = GetMinecartCollision(cartItem, cartItem->Position.yRot, CLICK(2), &ceilingHeight);
		if (floorHeight > -CLICK(1) &&
			floorHeight < CLICK(1))
		{
			if (Wibble & 7 == 0)
				SoundEffect(SFX_TR3_QUAD_FRONT_IMPACT, &cartItem->Position, 2);

			cartItem->Position.xPos += TURN_DEATH_VEL * phd_sin(cartItem->Position.yRot);
			cartItem->Position.zPos += TURN_DEATH_VEL * phd_cos(cartItem->Position.yRot);
		}
		else
		{
			if (laraItem->AnimNumber == Objects[ID_MINECART_LARA_ANIMS].animIndex + 30)
			{
				cartInfo->Flags |= CART_FLAG_NO_ANIM;
				laraItem->HitPoints = -1;
			}
		}

		break;

	case CART_STATE_HIT:
		if (laraItem->HitPoints <= 0 &&
			laraItem->FrameNumber == GetFrameNumber(cartItem, 34) + 28)
		{
			laraItem->FrameNumber = GetFrameNumber(cartItem, 34) + 28;
			cartInfo->Flags = (cartInfo->Flags & ~CART_FLAG_CONTROL) | (CART_FLAG_NO_ANIM);
			cartInfo->Velocity = cartItem->Velocity = 0;
		}

		break;
	}

	if (Lara.Vehicle != NO_ITEM &&
		!(cartInfo->Flags & CART_FLAG_NO_ANIM))
	{
		AnimateItem(laraItem);

		cartItem->AnimNumber = Objects[ID_MINECART].animIndex + (laraItem->AnimNumber - Objects[ID_MINECART_LARA_ANIMS].animIndex);
		cartItem->FrameNumber = g_Level.Anims[cartItem->AnimNumber].frameBase + (laraItem->FrameNumber - g_Level.Anims[laraItem->AnimNumber].frameBase);
	}
	if (laraItem->ActiveState != CART_TURN_DEATH &&
		laraItem->ActiveState != CART_WALL_DEATH &&
		laraItem->HitPoints > 0)
	{
		if (cartItem->Position.zRot > TERMINAL_ANGLE ||
			cartItem->Position.zRot < -TERMINAL_ANGLE)
		{
			laraItem->AnimNumber = Objects[ID_MINECART_LARA_ANIMS].animIndex + 31;
			laraItem->FrameNumber = g_Level.Anims[laraItem->AnimNumber].frameBase;
			laraItem->ActiveState = laraItem->TargetState = CART_TURN_DEATH;
			cartInfo->Flags = (cartInfo->Flags & ~CART_FLAG_CONTROL) | CART_FLAG_STOPPED | CART_FLAG_DEAD;
			cartInfo->Velocity = cartItem->Velocity = 0;
			return;
		}

		ceilingHeight;
		floorHeight = GetMinecartCollision(cartItem, cartItem->Position.yRot, CLICK(2), &ceilingHeight);
		if (floorHeight < -CLICK(2))
		{
			laraItem->AnimNumber = Objects[ID_MINECART_LARA_ANIMS].animIndex + 23;
			laraItem->FrameNumber = g_Level.Anims[laraItem->AnimNumber].frameBase;
			laraItem->ActiveState = laraItem->TargetState = CART_WALL_DEATH;
			cartInfo->Flags = (cartInfo->Flags & ~CART_FLAG_CONTROL) | (CART_FLAG_STOPPED | CART_FLAG_DEAD);
			cartInfo->Velocity = cartItem->Velocity = 0;
			laraItem->HitPoints = -1;
			return;
		}

		if (laraItem->ActiveState != CART_STATE_DUCK &&
			laraItem->ActiveState != CART_STATE_HIT)
		{
			COLL_INFO coll;
			coll.Setup.Radius = CART_RADIUS;
			DoObjectCollision(cartItem, &coll);

			if (coll.HitStatic)
			{
				int hits;

				laraItem->AnimNumber = Objects[ID_MINECART_LARA_ANIMS].animIndex + 34;
				laraItem->FrameNumber = g_Level.Anims[laraItem->AnimNumber].frameBase;
				laraItem->ActiveState = laraItem->TargetState = CART_STATE_HIT;
				DoLotsOfBlood(laraItem->Position.xPos, laraItem->Position.yPos - 768, laraItem->Position.zPos, cartItem->Velocity, cartItem->Position.yRot, laraItem->RoomNumber, 3);

				hits = CART_NHITS * short((cartInfo->Velocity) / 2048);
				if (hits < 20)
					hits = 20;

				laraItem->HitPoints -= hits;
				return;
			}
		}

		if (floorHeight > CLICK(2.25f) && !cartInfo->YVel)
			cartInfo->YVel = CART_JUMP_VELOCITY;

		CartToEntityCollision(cartItem);
	}
}

void MineCartCollision(short itemNumber, ITEM_INFO* laraItem, COLL_INFO* coll)
{
	if (laraItem->HitPoints < 0 || Lara.Vehicle != NO_ITEM)
		return;

	auto* cartItem = &g_Level.Items[itemNumber];

	int mount;
	if (mount = GetInMineCart(cartItem, laraItem, coll))
	{
		Lara.Vehicle = itemNumber;

		if (Lara.Control.WeaponControl.GunType == WEAPON_FLARE)
		{
			CreateFlare(LaraItem, ID_FLARE_ITEM, FALSE);
			UndrawFlareMeshes(laraItem);
			Lara.Flare.ControlLeft = false;
			Lara.Control.WeaponControl.RequestGunType = WEAPON_NONE;
			Lara.Control.WeaponControl.GunType = WEAPON_NONE;
		}

		Lara.Control.HandStatus = HandStatus::Busy;

		short angle = short(mGetAngle(cartItem->Position.xPos, cartItem->Position.zPos, laraItem->Position.xPos, laraItem->Position.zPos) - cartItem->Position.yRot);
		if (angle > -ANGLE(45.0f) && angle < ANGLE(135.0f))
			laraItem->AnimNumber = Objects[ID_MINECART_LARA_ANIMS].animIndex + 46;
		else
			laraItem->AnimNumber = Objects[ID_MINECART_LARA_ANIMS].animIndex + 0;

		laraItem->FrameNumber = g_Level.Anims[laraItem->AnimNumber].frameBase;
		laraItem->ActiveState = laraItem->TargetState = CART_STATE_MOUNT;
		laraItem->Position.xPos = cartItem->Position.xPos;
		laraItem->Position.yPos = cartItem->Position.yPos;
		laraItem->Position.zPos = cartItem->Position.zPos;
		laraItem->Position.xRot = cartItem->Position.xRot;
		laraItem->Position.yRot = cartItem->Position.yRot;
		laraItem->Position.zRot = cartItem->Position.zRot;
	}
	else
		ObjectCollision(itemNumber, laraItem, coll);
}

int MineCartControl(void)
{
	FLOOR_INFO* floor;
	short roomNumber;

	auto* cartItem = &g_Level.Items[Lara.Vehicle];

	if (!cartItem->Data) 
	{
		TENLog("Minecart data is nullptr!", LogLevel::Error);
		return 0; 
	}
	auto* cartInfo = (MinecartInfo*)cartItem->Data;

	DoUserInput(cartItem, LaraItem, cartInfo);

	if (cartInfo->Flags & CART_FLAG_CONTROL)
		MoveCart(cartItem, LaraItem, cartInfo);

	if (Lara.Vehicle != NO_ITEM)
	{
		LaraItem->Position.xPos = cartItem->Position.xPos;
		LaraItem->Position.yPos = cartItem->Position.yPos;
		LaraItem->Position.zPos = cartItem->Position.zPos;
		LaraItem->Position.xRot = cartItem->Position.xRot;
		LaraItem->Position.yRot = cartItem->Position.yRot;
		LaraItem->Position.zRot = cartItem->Position.zRot;
	}

	short probedRoomNumber = GetCollisionResult(cartItem).RoomNumber;
	if (probedRoomNumber != cartItem->RoomNumber)
	{
		ItemNewRoom(Lara.Vehicle, probedRoomNumber);
		ItemNewRoom(Lara.ItemNumber, probedRoomNumber);
	}

	TestTriggers(cartItem, false);

	if (!(cartInfo->Flags & CART_FLAG_DEAD))
	{
		Camera.targetElevation = -ANGLE(45.0f);
		Camera.targetDistance = SECTOR(2);
	}

	return (Lara.Vehicle == NO_ITEM) ? 0 : 1;
}
