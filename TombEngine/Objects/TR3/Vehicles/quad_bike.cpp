#include "framework.h"
#include "Objects/TR3/Vehicles/quad_bike.h"

#include "Game/animation.h"
#include "Game/camera.h"
#include "Game/collision/collide_room.h"
#include "Game/collision/collide_item.h"
#include "Game/effects/effects.h"
#include "Game/effects/tomb4fx.h"
#include "Game/items.h"
#include "Game/Lara/lara.h"
#include "Game/Lara/lara_flare.h"
#include "Game/Lara/lara_helpers.h"
#include "Game/Lara/lara_one_gun.h"
#include "Game/misc.h"
#include "Objects/TR3/Vehicles/quad_bike_info.h"
#include "Sound/sound.h"
#include "Specific/level.h"
#include "Specific/input.h"
#include "Specific/setup.h"
#include "Specific/prng.h"
#include "Game/effects/simple_particle.h"

using namespace TEN::Math::Random;

namespace TEN::Entities::Vehicles
{
	BITE_INFO quadEffectsPositions[6] =
	{
		{ -56, -32, -380, 0	},
		{ 56, -32, -380, 0 },
		{ -8, 180, -48, 3 },
		{ 8, 180, -48, 4 },
		{ 90, 180, -32, 6 },
		{ -90, 180, -32, 7 }
	};

	#define MAX_VELOCITY				0xA000
	#define MIN_DRIFT_VELOCITY			0x3000
	#define BRAKE						0x0280
	#define REVERSE_ACCELERATION		-0x0300
	#define MAX_BACK					-0x3000
	#define MAX_REVS					0xa000
	#define TERMINAL_VERTICAL_VELOCITY	240
	#define QBIKE_SLIP					100
	#define QBIKE_SLIP_SIDE				50

	#define QBIKE_FRONT	550
	#define QBIKE_BACK  -550
	#define QBIKE_SIDE	260
	#define QBIKE_RADIUS	500
	#define QBIKE_HEIGHT	512

	// TODO
	#define QBIKE_HIT_LEFT  11
	#define QBIKE_HIT_RIGHT 12
	#define QBIKE_HIT_FRONT 13
	#define QBIKE_HIT_BACK  14

	#define DAMAGE_START  140
	#define DAMAGE_LENGTH 14

	#define DISMOUNT_DISTANCE 385	// Precise root bone offset derived from final frame of animation.

	#define QBIKE_UNDO_TURN			ANGLE(2.0f)
	#define QBIKE_TURN_RATE			(ANGLE(0.5f) + QBIKE_UNDO_TURN)
	#define QBIKE_TURN_MAX			ANGLE(5.0f)
	#define QBIKE_DRIFT_TURN_RATE	(ANGLE(0.75f) + QBIKE_UNDO_TURN)
	#define QBIKE_DRIFT_TURN_MAX	ANGLE(8.0f)

	#define MIN_MOMENTUM_TURN ANGLE(3.0f)
	#define MAX_MOMENTUM_TURN ANGLE(1.5f)
	#define QBIKE_MAX_MOM_TURN ANGLE(150.0f)

	#define QBIKE_MAX_HEIGHT CLICK(1)
	#define QBIKE_MIN_BOUNCE ((MAX_VELOCITY / 2) / CLICK(1))

	// TODO: Common controls for all vehicles + unique settings page to set them. @Sezz 2021.11.14
	#define QBIKE_IN_ACCELERATE	IN_ACTION
	#define QBIKE_IN_REVERSE	IN_JUMP
	#define QBIKE_IN_DRIFT		(IN_CROUCH | IN_SPRINT)
	#define QBIKE_IN_DISMOUNT	(IN_JUMP | IN_ROLL)
	#define QBIKE_IN_LEFT		IN_LEFT
	#define QBIKE_IN_RIGHT		IN_RIGHT

	enum QuadBikeState
	{
		QBIKE_STATE_DRIVE = 1,
		QBIKE_STATE_TURN_LEFT = 2,
		QBIKE_STATE_SLOW = 5,
		QBIKE_STATE_BRAKE = 6,
		QBIKE_STATE_BIKE_DEATH = 7,
		QBIKE_STATE_FALL = 8,
		QBIKE_STATE_MOUNT_RIGHT = 9,
		QBIKE_STATE_DISMOUNT_RIGHT = 10,
		QBIKE_STATE_HIT_BACK = 11,
		QBIKE_STATE_HIT_FRONT = 12,
		QBIKE_STATE_HIT_LEFT = 13,
		QBIKE_STATE_HIT_RIGHT = 14,
		QBIKE_STATE_IDLE = 15,
		QBIKE_STATE_LAND = 17,
		QBIKE_STATE_STOP_SLOWLY = 18,
		QBIKE_STATE_FALL_DEATH = 19,
		QBIKE_STATE_FALL_OFF = 20,
		QBIKE_STATE_WHEELIE = 21,	// Unused.
		QBIKE_STATE_TURN_RIGHT = 22,
		QBIKE_STATE_MOUNT_LEFT = 23,
		QBIKE_STATE_DISMOUNT_LEFT = 24,
	};

	enum QuadBikeAnim
	{
		QBIKE_ANIM_IDLE_DEATH = 0,
		QBIKE_ANIM_UNK_1 = 1,
		QBIKE_ANIM_DRIVE_BACK = 2,
		QBIKE_ANIM_TURN_LEFT_START = 3,
		QBIKE_ANIM_TURN_LEFT_CONTINUE = 4,
		QBIKE_ANIM_TURN_LEFT_END = 5,
		QBIKE_ANIM_LEAP_START = 6,
		QBIKE_ANIM_LEAP_CONTINUE = 7,
		QBIKE_ANIM_LEAP_END = 8,
		QBIKE_ANIM_MOUNT_RIGHT = 9,
		QBIKE_ANIM_DISMOUNT_RIGHT = 10,
		QBIKE_ANIM_HIT_FRONT = 11,
		QBIKE_ANIM_HIT_BACK = 12,
		QBIKE_ANIM_HIT_RIGHT = 13,
		QBIKE_ANIM_HIT_LEFT = 14,
		QBIKE_ANIM_UNK_2 = 15,
		QBIKE_ANIM_UNK_3 = 16,
		QBIKE_ANIM_UNK_4 = 17,
		QBIKE_ANIM_IDLE = 18,
		QBIKE_ANIM_FALL_OFF_DEATH = 19,
		QBIKE_ANIM_TURN_RIGHT_START = 20,
		QBIKE_ANIM_TURN_RIGHT_CONTINUE = 21,
		QBIKE_ANIM_TURN_RIGHT_END = 22,
		QBIKE_ANIM_MOUNT_LEFT = 23,
		QBIKE_ANIM_DISMOUNT_LEFT = 24,
		QBIKE_ANIM_LEAP_START2 = 25,
		QBIKE_ANIM_LEAP_CONTINUE2 = 26,
		QBIKE_ANIM_LEAP_END2 = 27,
		QBIKE_ANIM_LEAP_TO_FREEFALL = 28
	};

	enum QuadBikeFlags
	{
		QBIKE_FLAG_FALLING = (1 << 6),
		QBIKE_FLAG_DEAD = (1 << 7)
	};

	enum QuadBikeEffectPosition
	{
		EXHAUST_LEFT = 0,
		EXHAUST_RIGHT = 1,
		FRONT_LEFT_TYRE = 2,
		FRONT_RIGHT_TYRE = 3,
		BACK_LEFT_TYRE = 4,
		BACK_RIGHT_TYRE = 5
	};

	static QuadBikeInfo* GetQuadBikeInfo(ItemInfo* qBikeItem)
	{
		return (QuadBikeInfo*)qBikeItem->Data;
	}

	void InitialiseQuadBike(short itemNumber)
	{
		auto* qBikeItem = &g_Level.Items[itemNumber];
		qBikeItem->Data = QuadBikeInfo();
		auto* qBike = GetQuadBikeInfo(qBikeItem);

		qBike->TurnRate = 0;
		qBike->MomentumAngle = qBikeItem->Pose.Orientation.y;
		qBike->ExtraRotation = 0;
		qBike->Velocity = 0;
		qBike->LeftVerticalVelocity = 0;
		qBike->RightVerticalVelocity = 0;
		qBike->Pitch = 0;
		qBike->Flags = 0;
	}

	static int CanQuadbikeGetOff(int direction)
	{
		auto* item = &g_Level.Items[Lara.Vehicle];
		short angle;

		if (direction < 0)
			angle = item->Pose.Orientation.y - ANGLE(90.0f);
		else
			angle = item->Pose.Orientation.y + ANGLE(90.0f);

		int x = item->Pose.Position.x + CLICK(2) * phd_sin(angle);
		int y = item->Pose.Position.y;
		int z = item->Pose.Position.z + CLICK(2) * phd_cos(angle);

		auto collResult = GetCollision(x, y, z, item->RoomNumber);

		if (collResult.Position.FloorSlope ||
			collResult.Position.Floor == NO_HEIGHT)
		{
			return false;
		}

		if (abs(collResult.Position.Floor - item->Pose.Position.y) > CLICK(2))
			return false;

		if ((collResult.Position.Ceiling - item->Pose.Position.y) > -LARA_HEIGHT ||
			(collResult.Position.Floor - collResult.Position.Ceiling) < LARA_HEIGHT)
		{
			return false;
		}

		return true;
	}

	static bool QuadBikeCheckGetOff(ItemInfo* laraItem, ItemInfo* qBikeItem)
	{
		auto* lara = GetLaraInfo(laraItem);
		auto* qBike = GetQuadBikeInfo(qBikeItem);

		if (lara->Vehicle == NO_ITEM)
			return true;

		if ((laraItem->Animation.ActiveState == QBIKE_STATE_DISMOUNT_RIGHT || laraItem->Animation.ActiveState == QBIKE_STATE_DISMOUNT_LEFT) &&
			TestLastFrame(laraItem))
		{
			if (laraItem->Animation.ActiveState == QBIKE_STATE_DISMOUNT_LEFT)
				laraItem->Pose.Orientation.y += ANGLE(90.0f);
			else
				laraItem->Pose.Orientation.y -= ANGLE(90.0f);

			SetAnimation(laraItem, LA_STAND_IDLE);
			TranslateItem(laraItem, laraItem->Pose.Orientation.y, -DISMOUNT_DISTANCE);
			laraItem->Pose.Orientation.x = 0;
			laraItem->Pose.Orientation.z = 0;
			lara->Vehicle = NO_ITEM;
			lara->Control.HandStatus = HandStatus::Free;

			if (laraItem->Animation.ActiveState == QBIKE_STATE_FALL_OFF)
			{
				auto pos = Vector3Int();

				SetAnimation(laraItem, LA_FREEFALL);
				GetJointAbsPosition(laraItem, &pos, LM_HIPS);

				laraItem->Pose.Position = pos;
				laraItem->Animation.Airborne = true;
				laraItem->Animation.VerticalVelocity = qBikeItem->Animation.VerticalVelocity;
				laraItem->Pose.Orientation.x = 0;
				laraItem->Pose.Orientation.z = 0;
				laraItem->HitPoints = 0;
				lara->Control.HandStatus = HandStatus::Free;
				qBikeItem->Flags |= ONESHOT;

				return false;
			}
			else if (laraItem->Animation.ActiveState == QBIKE_STATE_FALL_DEATH)
			{
				laraItem->Animation.TargetState = LS_DEATH;
				laraItem->Animation.Velocity = 0;
				laraItem->Animation.VerticalVelocity = DAMAGE_START + DAMAGE_LENGTH;
				qBike->Flags |= QBIKE_FLAG_DEAD;

				return false;
			}

			return true;
		}
		else
			return true;
	}

	static int GetOnQuadBike(ItemInfo* laraItem, ItemInfo* qBikeItem, CollisionInfo* coll)
	{
		auto* lara = GetLaraInfo(laraItem);

		if (!(TrInput & IN_ACTION) ||
			laraItem->Animation.Airborne ||
			lara->Control.HandStatus != HandStatus::Free ||
			qBikeItem->Flags & ONESHOT ||
			abs(qBikeItem->Pose.Position.y - laraItem->Pose.Position.y) > CLICK(1))
		{
			return false;
		}

		auto dist = pow(laraItem->Pose.Position.x - qBikeItem->Pose.Position.x, 2) + pow(laraItem->Pose.Position.z - qBikeItem->Pose.Position.z, 2);
		if (dist > 170000)
			return false;

		auto probe = GetCollision(qBikeItem);
		if (probe.Position.Floor < -32000)
			return false;
		else
		{
			short angle = phd_atan(qBikeItem->Pose.Position.z - laraItem->Pose.Position.z, qBikeItem->Pose.Position.x - laraItem->Pose.Position.x);
			angle -= qBikeItem->Pose.Orientation.y;

			if ((angle > -ANGLE(45.0f)) && (angle < ANGLE(135.0f)))
			{
				short tempAngle = laraItem->Pose.Orientation.y - qBikeItem->Pose.Orientation.y;
				if (tempAngle > ANGLE(45.0f) && tempAngle < ANGLE(135.0f))
					return true;
				else
					return false;
			}
			else
			{
				short tempAngle = laraItem->Pose.Orientation.y - qBikeItem->Pose.Orientation.y;
				if (tempAngle > ANGLE(225.0f) && tempAngle < ANGLE(315.0f))
					return true;
				else
					return false;
			}
		}

		return true;
	}

	static int GetQuadCollisionAnim(ItemInfo* qBikeItem, Vector3Int* p)
	{
		p->x = qBikeItem->Pose.Position.x - p->x;
		p->z = qBikeItem->Pose.Position.z - p->z;

		if (p->x || p->z)
		{
			float c = phd_cos(qBikeItem->Pose.Orientation.y);
			float s = phd_sin(qBikeItem->Pose.Orientation.y);
			int front = p->z * c + p->x * s;
			int side = -p->z * s + p->x * c;

			if (abs(front) > abs(side))
			{
				if (front > 0)
					return QBIKE_HIT_BACK;
				else
					return QBIKE_HIT_FRONT;
			}
			else
			{
				if (side > 0)
					return QBIKE_HIT_LEFT;
				else
					return QBIKE_HIT_RIGHT;
			}
		}

		return 0;
	}

	static int TestQuadHeight(ItemInfo* qBikeItem, int dz, int dx, Vector3Int* pos)
	{
		pos->y = qBikeItem->Pose.Position.y - dz * phd_sin(qBikeItem->Pose.Orientation.x) + dx * phd_sin(qBikeItem->Pose.Orientation.z);

		float c = phd_cos(qBikeItem->Pose.Orientation.y);
		float s = phd_sin(qBikeItem->Pose.Orientation.y);

		pos->z = qBikeItem->Pose.Position.z + dz * c - dx * s;
		pos->x = qBikeItem->Pose.Position.x + dz * s + dx * c;

		auto probe = GetCollision(pos->x, pos->y, pos->z, qBikeItem->RoomNumber);
		if (probe.Position.Ceiling > pos->y ||
			probe.Position.Ceiling == NO_HEIGHT)
		{
			return NO_HEIGHT;
		}

		return probe.Position.Floor;
	}

	static int DoQuadShift(ItemInfo* qBikeItem, Vector3Int* pos, Vector3Int* old)
	{
		CollisionResult probe;
		int x = pos->x / SECTOR(1);
		int z = pos->z / SECTOR(1);
		int oldX = old->x / SECTOR(1);
		int oldZ = old->z / SECTOR(1);
		int shiftX = pos->x & (SECTOR(1) - 1);
		int shiftZ = pos->z & (SECTOR(1) - 1);

		if (x == oldX)
		{
			if (z == oldZ)
			{
				qBikeItem->Pose.Position.z += (old->z - pos->z);
				qBikeItem->Pose.Position.x += (old->x - pos->x);
			}
			else if (z > oldZ)
			{
				qBikeItem->Pose.Position.z -= shiftZ + 1;
				return (pos->x - qBikeItem->Pose.Position.x);
			}
			else
			{
				qBikeItem->Pose.Position.z += SECTOR(1) - shiftZ;
				return (qBikeItem->Pose.Position.x - pos->x);
			}
		}
		else if (z == oldZ)
		{
			if (x > oldX)
			{
				qBikeItem->Pose.Position.x -= shiftX + 1;
				return (qBikeItem->Pose.Position.z - pos->z);
			}
			else
			{
				qBikeItem->Pose.Position.x += SECTOR(1) - shiftX;
				return (pos->z - qBikeItem->Pose.Position.z);
			}
		}
		else
		{
			x = 0;
			z = 0;

			probe = GetCollision(old->x, pos->y, pos->z, qBikeItem->RoomNumber);
			if (probe.Position.Floor < (old->y - CLICK(1)))
			{
				if (pos->z > old->z)
					z = -shiftZ - 1;
				else
					z = SECTOR(1) - shiftZ;
			}

			probe = GetCollision(pos->x, pos->y, old->z, qBikeItem->RoomNumber);
			if (probe.Position.Floor < (old->y - CLICK(1)))
			{
				if (pos->x > old->x)
					x = -shiftX - 1;
				else
					x = SECTOR(1) - shiftX;
			}

			if (x && z)
			{
				qBikeItem->Pose.Position.z += z;
				qBikeItem->Pose.Position.x += x;
			}
			else if (z)
			{
				qBikeItem->Pose.Position.z += z;

				if (z > 0)
					return (qBikeItem->Pose.Position.x - pos->x);
				else
					return (pos->x - qBikeItem->Pose.Position.x);
			}
			else if (x)
			{
				qBikeItem->Pose.Position.x += x;

				if (x > 0)
					return (pos->z - qBikeItem->Pose.Position.z);
				else
					return (qBikeItem->Pose.Position.z - pos->z);
			}
			else
			{
				qBikeItem->Pose.Position.z += (old->z - pos->z);
				qBikeItem->Pose.Position.x += (old->x - pos->x);
			}
		}

		return 0;
	}

	static int DoQuadDynamics(int height, int verticalVelocity, int* y)
	{
		if (height > *y)
		{
			*y += verticalVelocity;
			if (*y > height - QBIKE_MIN_BOUNCE)
			{
				*y = height;
				verticalVelocity = 0;
			}
			else
				verticalVelocity += 6;
		}
		else
		{
			int kick = (height - *y) * 4;
			if (kick < -80)
				kick = -80;

			verticalVelocity += ((kick - verticalVelocity) / 8);

			if (*y > height)
				*y = height;
		}

		return verticalVelocity;
	}

	static int QuadDynamics(ItemInfo* laraItem, ItemInfo* qBikeItem)
	{
		auto* lara = GetLaraInfo(laraItem);
		auto* qBike = GetQuadBikeInfo(qBikeItem);

		qBike->NoDismount = false;

		Vector3Int oldFrontLeft, oldFrontRight, oldBottomLeft, oldBottomRight;
		int holdFrontLeft = TestQuadHeight(qBikeItem, QBIKE_FRONT, -QBIKE_SIDE, &oldFrontLeft);
		int holdFrontRight = TestQuadHeight(qBikeItem, QBIKE_FRONT, QBIKE_SIDE, &oldFrontRight);
		int holdBottomLeft = TestQuadHeight(qBikeItem, -QBIKE_FRONT, -QBIKE_SIDE, &oldBottomLeft);
		int holdBottomRight = TestQuadHeight(qBikeItem, -QBIKE_FRONT, QBIKE_SIDE, &oldBottomRight);

		Vector3Int mtlOld, mtrOld, mmlOld, mmrOld;
		int hmml_old = TestQuadHeight(qBikeItem, 0, -QBIKE_SIDE, &mmlOld);
		int hmmr_old = TestQuadHeight(qBikeItem, 0, QBIKE_SIDE, &mmrOld);
		int hmtl_old = TestQuadHeight(qBikeItem, QBIKE_FRONT / 2, -QBIKE_SIDE, &mtlOld);
		int hmtr_old = TestQuadHeight(qBikeItem, QBIKE_FRONT / 2, QBIKE_SIDE, &mtrOld);

		Vector3Int moldBottomLeft, moldBottomRight;
		int hmoldBottomLeft = TestQuadHeight(qBikeItem, -QBIKE_FRONT / 2, -QBIKE_SIDE, &moldBottomLeft);
		int hmoldBottomRight = TestQuadHeight(qBikeItem, -QBIKE_FRONT / 2, QBIKE_SIDE, &moldBottomRight);

		Vector3Int old;
		old.x = qBikeItem->Pose.Position.x;
		old.y = qBikeItem->Pose.Position.y;
		old.z = qBikeItem->Pose.Position.z;

		if (oldBottomLeft.y > holdBottomLeft)
			oldBottomLeft.y = holdBottomLeft;

		if (oldBottomRight.y > holdBottomRight)
			oldBottomRight.y = holdBottomRight;

		if (oldFrontLeft.y > holdFrontLeft)
			oldFrontLeft.y = holdFrontLeft;

		if (oldFrontRight.y > holdFrontRight)
			oldFrontRight.y = holdFrontRight;

		if (moldBottomLeft.y > hmoldBottomLeft)
			moldBottomLeft.y = hmoldBottomLeft;

		if (moldBottomRight.y > hmoldBottomRight)
			moldBottomRight.y = hmoldBottomRight;

		if (mtlOld.y > hmtl_old)
			mtlOld.y = hmtl_old;

		if (mtrOld.y > hmtr_old)
			mtrOld.y = hmtr_old;

		if (mmlOld.y > hmml_old)
			mmlOld.y = hmml_old;

		if (mmrOld.y > hmmr_old)
			mmrOld.y = hmmr_old;

		if (qBikeItem->Pose.Position.y > (qBikeItem->Floor - CLICK(1)))
		{
			if (qBike->TurnRate < -QBIKE_UNDO_TURN)
				qBike->TurnRate += QBIKE_UNDO_TURN;
			else if (qBike->TurnRate > QBIKE_UNDO_TURN)
				qBike->TurnRate -= QBIKE_UNDO_TURN;
			else
				qBike->TurnRate = 0;

			qBikeItem->Pose.Orientation.y += qBike->TurnRate + qBike->ExtraRotation;

			short momentum = MIN_MOMENTUM_TURN - (((((MIN_MOMENTUM_TURN - MAX_MOMENTUM_TURN) * 256) / MAX_VELOCITY) * qBike->Velocity) / 256);
			if (!(TrInput & QBIKE_IN_ACCELERATE) && qBike->Velocity > 0)
				momentum += momentum / 4;

			short rot = qBikeItem->Pose.Orientation.y - qBike->MomentumAngle;
			if (rot < -MAX_MOMENTUM_TURN)
			{
				if (rot < -QBIKE_MAX_MOM_TURN)
				{
					rot = -QBIKE_MAX_MOM_TURN;
					qBike->MomentumAngle = qBikeItem->Pose.Orientation.y - rot;
				}
				else
					qBike->MomentumAngle -= momentum;
			}
			else if (rot > MAX_MOMENTUM_TURN)
			{
				if (rot > QBIKE_MAX_MOM_TURN)
				{
					rot = QBIKE_MAX_MOM_TURN;
					qBike->MomentumAngle = qBikeItem->Pose.Orientation.y - rot;
				}
				else
					qBike->MomentumAngle += momentum;
			}
			else
				qBike->MomentumAngle = qBikeItem->Pose.Orientation.y;
		}
		else
			qBikeItem->Pose.Orientation.y += qBike->TurnRate + qBike->ExtraRotation;

		auto probe = GetCollision(qBikeItem);
		int speed = 0;
		if (qBikeItem->Pose.Position.y >= probe.Position.Floor)
			speed = qBikeItem->Animation.Velocity * phd_cos(qBikeItem->Pose.Orientation.x);
		else
			speed = qBikeItem->Animation.Velocity;

		TranslateItem(qBikeItem, qBike->MomentumAngle, speed);

		int slip = QBIKE_SLIP * phd_sin(qBikeItem->Pose.Orientation.x);
		if (abs(slip) > QBIKE_SLIP / 2)
		{
			if (slip > 0)
				slip -= 10;
			else
				slip += 10;
			qBikeItem->Pose.Position.z -= slip * phd_cos(qBikeItem->Pose.Orientation.y);
			qBikeItem->Pose.Position.x -= slip * phd_sin(qBikeItem->Pose.Orientation.y);
		}

		slip = QBIKE_SLIP_SIDE * phd_sin(qBikeItem->Pose.Orientation.z);
		if (abs(slip) > QBIKE_SLIP_SIDE / 2)
		{
			qBikeItem->Pose.Position.z -= slip * phd_sin(qBikeItem->Pose.Orientation.y);
			qBikeItem->Pose.Position.x += slip * phd_cos(qBikeItem->Pose.Orientation.y);
		}

		Vector3Int moved;
		moved.x = qBikeItem->Pose.Position.x;
		moved.z = qBikeItem->Pose.Position.z;

		if (!(qBikeItem->Flags & ONESHOT))
			DoVehicleCollision(qBikeItem, QBIKE_RADIUS);

		short rot = 0;
		short rotAdd = 0;

		Vector3Int fl;
		int heightFrontLeft = TestQuadHeight(qBikeItem, QBIKE_FRONT, -QBIKE_SIDE, &fl);
		if (heightFrontLeft < (oldFrontLeft.y - CLICK(1)))
			rot = DoQuadShift(qBikeItem, &fl, &oldFrontLeft);

		Vector3Int mtl;
		int hmtl = TestQuadHeight(qBikeItem, QBIKE_FRONT / 2, -QBIKE_SIDE, &mtl);
		if (hmtl < (mtlOld.y - CLICK(1)))
			DoQuadShift(qBikeItem, &mtl, &mtlOld);

		Vector3Int mml;
		int hmml = TestQuadHeight(qBikeItem, 0, -QBIKE_SIDE, &mml);
		if (hmml < (mmlOld.y - CLICK(1)))
			DoQuadShift(qBikeItem, &mml, &mmlOld);

		Vector3Int mbl;
		int hmbl = TestQuadHeight(qBikeItem, -QBIKE_FRONT / 2, -QBIKE_SIDE, &mbl);
		if (hmbl < (moldBottomLeft.y - CLICK(1)))
			DoQuadShift(qBikeItem, &mbl, &moldBottomLeft);

		Vector3Int bl;
		int heightBackLeft = TestQuadHeight(qBikeItem, -QBIKE_FRONT, -QBIKE_SIDE, &bl);
		if (heightBackLeft < (oldBottomLeft.y - CLICK(1)))
		{
			rotAdd = DoQuadShift(qBikeItem, &bl, &oldBottomLeft);
			if ((rotAdd > 0 && rot >= 0) || (rotAdd < 0 && rot <= 0))
				rot += rotAdd;
		}

		Vector3Int fr;
		int heightFrontRight = TestQuadHeight(qBikeItem, QBIKE_FRONT, QBIKE_SIDE, &fr);
		if (heightFrontRight < (oldFrontRight.y - CLICK(1)))
		{
			rotAdd = DoQuadShift(qBikeItem, &fr, &oldFrontRight);
			if ((rotAdd > 0 && rot >= 0) || (rotAdd < 0 && rot <= 0))
				rot += rotAdd;
		}

		Vector3Int mtr;
		int hmtr = TestQuadHeight(qBikeItem, QBIKE_FRONT / 2, QBIKE_SIDE, &mtr);
		if (hmtr < (mtrOld.y - CLICK(1)))
			DoQuadShift(qBikeItem, &mtr, &mtrOld);

		Vector3Int mmr;
		int hmmr = TestQuadHeight(qBikeItem, 0, QBIKE_SIDE, &mmr);
		if (hmmr < (mmrOld.y - CLICK(1)))
			DoQuadShift(qBikeItem, &mmr, &mmrOld);

		Vector3Int mbr;
		int hmbr = TestQuadHeight(qBikeItem, -QBIKE_FRONT / 2, QBIKE_SIDE, &mbr);
		if (hmbr < (moldBottomRight.y - CLICK(1)))
			DoQuadShift(qBikeItem, &mbr, &moldBottomRight);

		Vector3Int br;
		int heightBackRight = TestQuadHeight(qBikeItem, -QBIKE_FRONT, QBIKE_SIDE, &br);
		if (heightBackRight < (oldBottomRight.y - CLICK(1)))
		{
			rotAdd = DoQuadShift(qBikeItem, &br, &oldBottomRight);
			if ((rotAdd > 0 && rot >= 0) || (rotAdd < 0 && rot <= 0))
				rot += rotAdd;
		}

		probe = GetCollision(qBikeItem);
		if (probe.Position.Floor < qBikeItem->Pose.Position.y - CLICK(1))
			DoQuadShift(qBikeItem, (Vector3Int*)&qBikeItem->Pose, &old);

		qBike->ExtraRotation = rot;

		int collide = GetQuadCollisionAnim(qBikeItem, &moved);

		int newVelocity = 0;
		if (collide)
		{
			newVelocity = (qBikeItem->Pose.Position.z - old.z) * phd_cos(qBike->MomentumAngle) + (qBikeItem->Pose.Position.x - old.x) * phd_sin(qBike->MomentumAngle);
			newVelocity *= 256;

			if (&g_Level.Items[lara->Vehicle] == qBikeItem &&
				qBike->Velocity == MAX_VELOCITY &&
				newVelocity < (qBike->Velocity - 10))
			{
				laraItem->HitPoints -= (qBike->Velocity - newVelocity) / 128;
				laraItem->HitStatus = 1;
			}

			if (qBike->Velocity > 0 && newVelocity < qBike->Velocity)
				qBike->Velocity = (newVelocity < 0) ? 0 : newVelocity;

			else if (qBike->Velocity < 0 && newVelocity > qBike->Velocity)
				qBike->Velocity = (newVelocity > 0) ? 0 : newVelocity;

			if (qBike->Velocity < MAX_BACK)
				qBike->Velocity = MAX_BACK;
		}

		return collide;
	}

	static void AnimateQuadBike(ItemInfo* laraItem, ItemInfo* qBikeItem, int collide, bool dead)
	{
		auto* qBike = GetQuadBikeInfo(qBikeItem);

		if (qBikeItem->Pose.Position.y != qBikeItem->Floor &&
			laraItem->Animation.ActiveState != QBIKE_STATE_FALL &&
			laraItem->Animation.ActiveState != QBIKE_STATE_LAND &&
			laraItem->Animation.ActiveState != QBIKE_STATE_FALL_OFF &&
			!dead)
		{
			if (qBike->Velocity < 0)
				laraItem->Animation.AnimNumber = Objects[ID_QUAD_LARA_ANIMS].animIndex + QBIKE_ANIM_LEAP_START;
			else
				laraItem->Animation.AnimNumber = Objects[ID_QUAD_LARA_ANIMS].animIndex + QBIKE_ANIM_LEAP_START2;

			laraItem->Animation.FrameNumber = GetFrameNumber(laraItem, laraItem->Animation.AnimNumber);
			laraItem->Animation.ActiveState = QBIKE_STATE_FALL;
			laraItem->Animation.TargetState = QBIKE_STATE_FALL;
		}
		else if (collide &&
			laraItem->Animation.ActiveState != QBIKE_STATE_HIT_FRONT &&
			laraItem->Animation.ActiveState != QBIKE_STATE_HIT_BACK &&
			laraItem->Animation.ActiveState != QBIKE_STATE_HIT_LEFT &&
			laraItem->Animation.ActiveState != QBIKE_STATE_HIT_RIGHT &&
			laraItem->Animation.ActiveState != QBIKE_STATE_FALL_OFF &&
			qBike->Velocity > (MAX_VELOCITY / 3) &&
			!dead)
		{
			if (collide == QBIKE_HIT_FRONT)
			{
				laraItem->Animation.AnimNumber = Objects[ID_QUAD_LARA_ANIMS].animIndex + QBIKE_ANIM_HIT_BACK;
				laraItem->Animation.ActiveState = QBIKE_STATE_HIT_FRONT;
				laraItem->Animation.TargetState = QBIKE_STATE_HIT_FRONT;
			}
			else if (collide == QBIKE_HIT_BACK)
			{
				laraItem->Animation.AnimNumber = Objects[ID_QUAD_LARA_ANIMS].animIndex + QBIKE_ANIM_HIT_FRONT;
				laraItem->Animation.ActiveState = QBIKE_STATE_HIT_BACK;
				laraItem->Animation.TargetState = QBIKE_STATE_HIT_BACK;
			}
			else if (collide == QBIKE_HIT_LEFT)
			{
				laraItem->Animation.AnimNumber = Objects[ID_QUAD_LARA_ANIMS].animIndex + QBIKE_ANIM_HIT_RIGHT;
				laraItem->Animation.ActiveState = QBIKE_STATE_HIT_LEFT;
				laraItem->Animation.TargetState = QBIKE_STATE_HIT_LEFT;
			}
			else
			{
				laraItem->Animation.AnimNumber = Objects[ID_QUAD_LARA_ANIMS].animIndex + QBIKE_ANIM_HIT_LEFT;
				laraItem->Animation.ActiveState = QBIKE_STATE_HIT_RIGHT;
				laraItem->Animation.TargetState = QBIKE_STATE_HIT_RIGHT;
			}

			laraItem->Animation.FrameNumber = GetFrameNumber(laraItem, laraItem->Animation.AnimNumber);
			SoundEffect(SFX_TR3_VEHICLE_QUADBIKE_FRONT_IMPACT, &qBikeItem->Pose);
		}
		else
		{
			switch (laraItem->Animation.ActiveState)
			{
			case QBIKE_STATE_IDLE:
				if (dead)
					laraItem->Animation.TargetState = QBIKE_STATE_BIKE_DEATH;
				else if (TrInput & QBIKE_IN_DISMOUNT &&
					qBike->Velocity == 0 &&
					!qBike->NoDismount)
				{
					if (TrInput & QBIKE_IN_LEFT && CanQuadbikeGetOff(-1))
						laraItem->Animation.TargetState = QBIKE_STATE_DISMOUNT_LEFT;
					else if (TrInput & QBIKE_IN_RIGHT && CanQuadbikeGetOff(1))
						laraItem->Animation.TargetState = QBIKE_STATE_DISMOUNT_RIGHT;
				}
				else if (TrInput & (QBIKE_IN_ACCELERATE | QBIKE_IN_REVERSE))
					laraItem->Animation.TargetState = QBIKE_STATE_DRIVE;

				break;

			case QBIKE_STATE_DRIVE:
				if (dead)
				{
					if (qBike->Velocity > (MAX_VELOCITY / 2))
						laraItem->Animation.TargetState = QBIKE_STATE_FALL_DEATH;
					else
						laraItem->Animation.TargetState = QBIKE_STATE_BIKE_DEATH;
				}
				else if (!(TrInput & (QBIKE_IN_ACCELERATE | QBIKE_IN_REVERSE)) &&
					(qBike->Velocity / 256) == 0)
				{
					laraItem->Animation.TargetState = QBIKE_STATE_IDLE;
				}
				else if (TrInput & QBIKE_IN_LEFT &&
					!qBike->DriftStarting)
				{
					laraItem->Animation.TargetState = QBIKE_STATE_TURN_LEFT;
				}
				else if (TrInput & QBIKE_IN_RIGHT &&
					!qBike->DriftStarting)
				{
					laraItem->Animation.TargetState = QBIKE_STATE_TURN_RIGHT;
				}
				else if (TrInput & QBIKE_IN_REVERSE)
				{
					if (qBike->Velocity > (MAX_VELOCITY / 3 * 2))
						laraItem->Animation.TargetState = QBIKE_STATE_BRAKE;
					else
						laraItem->Animation.TargetState = QBIKE_STATE_SLOW;
				}

				break;

			case QBIKE_STATE_BRAKE:
			case QBIKE_STATE_SLOW:
			case QBIKE_STATE_STOP_SLOWLY:
				if ((qBike->Velocity / 256) == 0)
					laraItem->Animation.TargetState = QBIKE_STATE_IDLE;
				else if (TrInput & QBIKE_IN_LEFT)
					laraItem->Animation.TargetState = QBIKE_STATE_TURN_LEFT;
				else if (TrInput & QBIKE_IN_RIGHT)
					laraItem->Animation.TargetState = QBIKE_STATE_TURN_RIGHT;

				break;

			case QBIKE_STATE_TURN_LEFT:
				if ((qBike->Velocity / 256) == 0)
					laraItem->Animation.TargetState = QBIKE_STATE_IDLE;
				else if (TrInput & QBIKE_IN_RIGHT)
				{
					laraItem->Animation.AnimNumber = Objects[ID_QUAD_LARA_ANIMS].animIndex + QBIKE_ANIM_TURN_RIGHT_START;
					laraItem->Animation.FrameNumber = GetFrameNumber(laraItem, laraItem->Animation.AnimNumber);
					laraItem->Animation.ActiveState = QBIKE_STATE_TURN_RIGHT;
					laraItem->Animation.TargetState = QBIKE_STATE_TURN_RIGHT;
				}
				else if (!(TrInput & QBIKE_IN_LEFT))
					laraItem->Animation.TargetState = QBIKE_STATE_DRIVE;

				break;

			case QBIKE_STATE_TURN_RIGHT:
				if ((qBike->Velocity / 256) == 0)
					laraItem->Animation.TargetState = QBIKE_STATE_IDLE;
				else if (TrInput & QBIKE_IN_LEFT)
				{
					laraItem->Animation.AnimNumber = Objects[ID_QUAD_LARA_ANIMS].animIndex + QBIKE_ANIM_TURN_LEFT_START;
					laraItem->Animation.FrameNumber = GetFrameNumber(laraItem, laraItem->Animation.AnimNumber);
					laraItem->Animation.ActiveState = QBIKE_STATE_TURN_LEFT;
					laraItem->Animation.TargetState = QBIKE_STATE_TURN_LEFT;
				}
				else if (!(TrInput & QBIKE_IN_RIGHT))
					laraItem->Animation.TargetState = QBIKE_STATE_DRIVE;

				break;

			case QBIKE_STATE_FALL:
				if (qBikeItem->Pose.Position.y == qBikeItem->Floor)
					laraItem->Animation.TargetState = QBIKE_STATE_LAND;
				else if (qBikeItem->Animation.VerticalVelocity > TERMINAL_VERTICAL_VELOCITY)
					qBike->Flags |= QBIKE_FLAG_FALLING;

				break;

			case QBIKE_STATE_FALL_OFF:
				break;

			case QBIKE_STATE_HIT_FRONT:
			case QBIKE_STATE_HIT_BACK:
			case QBIKE_STATE_HIT_LEFT:
			case QBIKE_STATE_HIT_RIGHT:
				if (TrInput & (QBIKE_IN_ACCELERATE | QBIKE_IN_REVERSE))
					laraItem->Animation.TargetState = QBIKE_STATE_DRIVE;

				break;
			}
		}
	}

	static int QuadUserControl(ItemInfo* qBikeItem, int height, int* pitch)
	{
		auto* qBike = GetQuadBikeInfo(qBikeItem);

		bool drive = false; // Never changes?

		if (!(TrInput & QBIKE_IN_DRIFT) &&
			!qBike->Velocity && !qBike->CanStartDrift)
		{
			qBike->CanStartDrift = true;
		}
		else if (qBike->Velocity)
			qBike->CanStartDrift = false;

		if (!(TrInput & QBIKE_IN_DRIFT))
			qBike->DriftStarting = false;

		if (!qBike->DriftStarting)
		{
			if (qBike->Revs > 0x10)
			{
				qBike->Velocity += (qBike->Revs / 16);
				qBike->Revs -= (qBike->Revs / 8);
			}
			else
				qBike->Revs = 0;
		}

		if (qBikeItem->Pose.Position.y >= (height - CLICK(1)))
		{
			if (TrInput & IN_LOOK && !qBike->Velocity)
				LookUpDown(LaraItem);

			// Driving forward.
			if (qBike->Velocity > 0)
			{
				if (TrInput & QBIKE_IN_DRIFT &&
					!qBike->DriftStarting &&
					qBike->Velocity > MIN_DRIFT_VELOCITY)
				{
					if (TrInput & QBIKE_IN_LEFT)
					{
						qBike->TurnRate -= QBIKE_DRIFT_TURN_RATE;
						if (qBike->TurnRate < -QBIKE_DRIFT_TURN_MAX)
							qBike->TurnRate = -QBIKE_DRIFT_TURN_MAX;
					}
					else if (TrInput & QBIKE_IN_RIGHT)
					{
						qBike->TurnRate += QBIKE_DRIFT_TURN_RATE;
						if (qBike->TurnRate > QBIKE_DRIFT_TURN_MAX)
							qBike->TurnRate = QBIKE_DRIFT_TURN_MAX;
					}
				}
				else
				{
					if (TrInput & QBIKE_IN_LEFT)
					{
						qBike->TurnRate -= QBIKE_TURN_RATE;
						if (qBike->TurnRate < -QBIKE_TURN_MAX)
							qBike->TurnRate = -QBIKE_TURN_MAX;
					}
					else if (TrInput & QBIKE_IN_RIGHT)
					{
						qBike->TurnRate += QBIKE_TURN_RATE;
						if (qBike->TurnRate > QBIKE_TURN_MAX)
							qBike->TurnRate = QBIKE_TURN_MAX;
					}
				}
			}
			// Driving back.
			else if (qBike->Velocity < 0)
			{
				if (TrInput & QBIKE_IN_DRIFT &&
					!qBike->DriftStarting &&
					qBike->Velocity < (-MIN_DRIFT_VELOCITY + 0x800))
				{
					if (TrInput & QBIKE_IN_LEFT)
					{
						qBike->TurnRate -= QBIKE_DRIFT_TURN_RATE;
						if (qBike->TurnRate < -QBIKE_DRIFT_TURN_MAX)
							qBike->TurnRate = -QBIKE_DRIFT_TURN_MAX;
					}
					else if (TrInput & QBIKE_IN_RIGHT)
					{
						qBike->TurnRate += QBIKE_DRIFT_TURN_RATE;
						if (qBike->TurnRate > QBIKE_DRIFT_TURN_MAX)
							qBike->TurnRate = QBIKE_DRIFT_TURN_MAX;
					}
				}
				else
				{
					if (TrInput & QBIKE_IN_RIGHT)
					{
						qBike->TurnRate -= QBIKE_TURN_RATE;
						if (qBike->TurnRate < -QBIKE_TURN_MAX)
							qBike->TurnRate = -QBIKE_TURN_MAX;
					}
					else if (TrInput & QBIKE_IN_LEFT)
					{
						qBike->TurnRate += QBIKE_TURN_RATE;
						if (qBike->TurnRate > QBIKE_TURN_MAX)
							qBike->TurnRate = QBIKE_TURN_MAX;
					}
				}
			}

			// Driving back / braking.
			if (TrInput & QBIKE_IN_REVERSE)
			{
				if (TrInput & QBIKE_IN_DRIFT &&
					(qBike->CanStartDrift || qBike->DriftStarting))
				{
					qBike->DriftStarting = true;
					qBike->Revs -= 0x200;
					if (qBike->Revs < MAX_BACK)
						qBike->Revs = MAX_BACK;
				}
				else if (qBike->Velocity > 0)
					qBike->Velocity -= BRAKE;
				else
				{
					if (qBike->Velocity > MAX_BACK)
						qBike->Velocity += REVERSE_ACCELERATION;
				}
			}
			else if (TrInput & QBIKE_IN_ACCELERATE)
			{
				if (TrInput & QBIKE_IN_DRIFT &&
					(qBike->CanStartDrift || qBike->DriftStarting))
				{
					qBike->DriftStarting = true;
					qBike->Revs += 0x200;
					if (qBike->Revs >= MAX_VELOCITY)
						qBike->Revs = MAX_VELOCITY;
				}
				else if (qBike->Velocity < MAX_VELOCITY)
				{
					if (qBike->Velocity < 0x4000)
						qBike->Velocity += (8 + (0x4000 + 0x800 - qBike->Velocity) / 8);
					else if (qBike->Velocity < 0x7000)
						qBike->Velocity += (4 + (0x7000 + 0x800 - qBike->Velocity) / 16);
					else if (qBike->Velocity < MAX_VELOCITY)
						qBike->Velocity += (2 + (MAX_VELOCITY - qBike->Velocity) / 8);
				}
				else
					qBike->Velocity = MAX_VELOCITY;

				qBike->Velocity -= abs(qBikeItem->Pose.Orientation.y - qBike->MomentumAngle) / 64;
			}

			else if (qBike->Velocity > 0x0100)
				qBike->Velocity -= 0x0100;
			else if (qBike->Velocity < -0x0100)
				qBike->Velocity += 0x0100;
			else
				qBike->Velocity = 0;

			if (!(TrInput & (QBIKE_IN_ACCELERATE | QBIKE_IN_REVERSE)) &&
				qBike->DriftStarting &&
				qBike->Revs)
			{
				if (qBike->Revs > 0x8)
					qBike->Revs -= qBike->Revs / 8;
				else
					qBike->Revs = 0;
			}

			qBikeItem->Animation.Velocity = qBike->Velocity / 256;

			if (qBike->EngineRevs > 0x7000)
				qBike->EngineRevs = -0x2000;

			int revs = 0;
			if (qBike->Velocity < 0)
				revs = abs(qBike->Velocity / 2);
			else if (qBike->Velocity < 0x7000)
				revs = -0x2000 + (qBike->Velocity * (0x6800 - -0x2000)) / 0x7000;
			else if (qBike->Velocity <= MAX_VELOCITY)
				revs = -0x2800 + ((qBike->Velocity - 0x7000) * (0x7000 - -0x2800)) / (MAX_VELOCITY - 0x7000);

			revs += abs(qBike->Revs);
			qBike->EngineRevs += (revs - qBike->EngineRevs) / 8;
		}
		else
		{
			if (qBike->EngineRevs < 0xA000)
				qBike->EngineRevs += (0xA000 - qBike->EngineRevs) / 8;
		}

		*pitch = qBike->EngineRevs;

		return drive;
	}

	void QuadBikeCollision(short itemNumber, ItemInfo* laraItem, CollisionInfo* coll)
	{
		auto* lara = GetLaraInfo(laraItem);
		auto* qBikeItem = &g_Level.Items[itemNumber];
		auto* qBike = GetQuadBikeInfo(qBikeItem);

		if (laraItem->HitPoints < 0 || lara->Vehicle != NO_ITEM)
			return;

		if (GetOnQuadBike(laraItem, &g_Level.Items[itemNumber], coll))
		{
			lara->Vehicle = itemNumber;

			if (lara->Control.Weapon.GunType == LaraWeaponType::Flare)
			{
				CreateFlare(laraItem, ID_FLARE_ITEM, 0);
				UndrawFlareMeshes(laraItem);
				lara->Flare.ControlLeft = false;
				lara->Control.Weapon.RequestGunType = lara->Control.Weapon.GunType = LaraWeaponType::None;
			}

			lara->Control.HandStatus = HandStatus::Busy;

			short angle = phd_atan(qBikeItem->Pose.Position.z - laraItem->Pose.Position.z, qBikeItem->Pose.Position.x - laraItem->Pose.Position.x);
			angle -= qBikeItem->Pose.Orientation.y;

			if (angle > -ANGLE(45.0f) && angle < ANGLE(135.0f))
			{
				laraItem->Animation.AnimNumber = Objects[ID_QUAD_LARA_ANIMS].animIndex + QBIKE_ANIM_MOUNT_LEFT;
				laraItem->Animation.ActiveState = laraItem->Animation.TargetState = QBIKE_STATE_MOUNT_LEFT;
			}
			else
			{
				laraItem->Animation.AnimNumber = Objects[ID_QUAD_LARA_ANIMS].animIndex + QBIKE_ANIM_MOUNT_RIGHT;
				laraItem->Animation.ActiveState = laraItem->Animation.TargetState = QBIKE_STATE_MOUNT_RIGHT;
			}

			laraItem->Animation.FrameNumber = g_Level.Anims[laraItem->Animation.AnimNumber].frameBase;
			laraItem->Pose.Position.x = qBikeItem->Pose.Position.x;
			laraItem->Pose.Position.y = qBikeItem->Pose.Position.y;
			laraItem->Pose.Position.z = qBikeItem->Pose.Position.z;
			laraItem->Pose.Orientation.y = qBikeItem->Pose.Orientation.y;
			ResetLaraFlex(laraItem);
			lara->HitDirection = -1;
			qBikeItem->HitPoints = 1;

			AnimateItem(laraItem);

			qBike->Revs = 0;
		}
		else
			ObjectCollision(itemNumber, laraItem, coll);
	}

	static void TriggerQuadExhaustSmoke(int x, int y, int z, short angle, int speed, int moving)
	{
		auto* spark = GetFreeParticle();

		spark->on = true;
		spark->sR = 0;
		spark->sG = 0;
		spark->sB = 0;

		spark->dR = 96;
		spark->dG = 96;
		spark->dB = 128;

		if (moving)
		{
			spark->dR = (spark->dR * speed) / 32;
			spark->dG = (spark->dG * speed) / 32;
			spark->dB = (spark->dB * speed) / 32;
		}

		spark->sLife = spark->life = (GetRandomControl() & 3) + 20 - (speed / 4096);
		if (spark->sLife < 9)
			spark->sLife = spark->life = 9;

		spark->blendMode = BLEND_MODES::BLENDMODE_SCREEN;
		spark->colFadeSpeed = 4;
		spark->fadeToBlack = 4;
		spark->extras = 0;
		spark->dynamic = -1;
		spark->x = x + ((GetRandomControl() & 15) - 8);
		spark->y = y + ((GetRandomControl() & 15) - 8);
		spark->z = z + ((GetRandomControl() & 15) - 8);
		int zv = speed * phd_cos(angle) / 4;
		int xv = speed * phd_sin(angle) / 4;
		spark->xVel = xv + ((GetRandomControl() & 255) - 128);
		spark->yVel = -(GetRandomControl() & 7) - 8;
		spark->zVel = zv + ((GetRandomControl() & 255) - 128);
		spark->friction = 4;

		if (GetRandomControl() & 1)
		{
			spark->flags = SP_SCALE | SP_DEF | SP_ROTATE | SP_EXPDEF;
			spark->rotAng = GetRandomControl() & 4095;
			if (GetRandomControl() & 1)
				spark->rotAdd = -(GetRandomControl() & 7) - 24;
			else
				spark->rotAdd = (GetRandomControl() & 7) + 24;
		}
		else
			spark->flags = SP_SCALE | SP_DEF | SP_EXPDEF;

		spark->spriteIndex = Objects[ID_DEFAULT_SPRITES].meshIndex;
		spark->scalar = 2;
		spark->gravity = -(GetRandomControl() & 3) - 4;
		spark->maxYvel = -(GetRandomControl() & 7) - 8;
		int size = (GetRandomControl() & 7) + 64 + (speed / 128);
		spark->dSize = size;
		spark->size = spark->sSize = size / 2;
	}

	bool QuadBikeControl(ItemInfo* laraItem, CollisionInfo* coll)
	{
		auto* lara = GetLaraInfo(laraItem);
		auto* qBikeItem = &g_Level.Items[lara->Vehicle];
		auto* qBike = GetQuadBikeInfo(qBikeItem);

		GameVector	oldPos;
		oldPos.x = qBikeItem->Pose.Position.x;
		oldPos.y = qBikeItem->Pose.Position.y;
		oldPos.z = qBikeItem->Pose.Position.z;
		oldPos.roomNumber = qBikeItem->RoomNumber;

		bool collide = QuadDynamics(laraItem, qBikeItem);

		auto probe = GetCollision(qBikeItem);

		Vector3Int frontLeft, frontRight;
		auto floorHeightLeft = TestQuadHeight(qBikeItem, QBIKE_FRONT, -QBIKE_SIDE, &frontLeft);
		auto floorHeightRight = TestQuadHeight(qBikeItem, QBIKE_FRONT, QBIKE_SIDE, &frontRight);

		TestTriggers(qBikeItem, false);

		bool dead = false;
		if (laraItem->HitPoints <= 0)
		{
			TrInput &= ~(IN_LEFT | IN_RIGHT | IN_BACK | IN_FORWARD);
			dead = true;
		}

		int drive = -1;
		int pitch = 0;
		if (qBike->Flags)
			collide = false;
		else
		{
			switch (laraItem->Animation.ActiveState)
			{
			case QBIKE_STATE_MOUNT_LEFT:
			case QBIKE_STATE_MOUNT_RIGHT:
			case QBIKE_STATE_DISMOUNT_LEFT:
			case QBIKE_STATE_DISMOUNT_RIGHT:
				drive = -1;
				collide = false;
				break;

			default:
				drive = QuadUserControl(qBikeItem, probe.Position.Floor, &pitch);
				break;
			}
		}

		if (qBike->Velocity || qBike->Revs)
		{
			qBike->Pitch = pitch;
			if (qBike->Pitch < -0x8000)
				qBike->Pitch = -0x8000;
			else if (qBike->Pitch > 0xA000)
				qBike->Pitch = 0xA000;

			SoundEffect(SFX_TR3_VEHICLE_QUADBIKE_MOVE, &qBikeItem->Pose, SoundEnvironment::Land, 0.5f + (float)abs(qBike->Pitch) / (float)MAX_VELOCITY);
		}
		else
		{
			if (drive != -1)
				SoundEffect(SFX_TR3_VEHICLE_QUADBIKE_IDLE, &qBikeItem->Pose);

			qBike->Pitch = 0;
		}

		qBikeItem->Floor = probe.Position.Floor;

		short rotAdd = qBike->Velocity / 4;
		qBike->RearRot -= rotAdd;
		qBike->RearRot -= (qBike->Revs / 8);
		qBike->FrontRot -= rotAdd;

		qBike->LeftVerticalVelocity = DoQuadDynamics(floorHeightLeft, qBike->LeftVerticalVelocity, (int*)&frontLeft.y);
		qBike->RightVerticalVelocity = DoQuadDynamics(floorHeightRight, qBike->RightVerticalVelocity, (int*)&frontRight.y);
		qBikeItem->Animation.VerticalVelocity = DoQuadDynamics(probe.Position.Floor, qBikeItem->Animation.VerticalVelocity, (int*)&qBikeItem->Pose.Position.y);
		qBike->Velocity = DoVehicleWaterMovement(qBikeItem, laraItem, qBike->Velocity, QBIKE_RADIUS, &qBike->TurnRate);

		probe.Position.Floor = (frontLeft.y + frontRight.y) / 2;
		short xRot = phd_atan(QBIKE_FRONT, qBikeItem->Pose.Position.y - probe.Position.Floor);
		short zRot = phd_atan(QBIKE_SIDE, probe.Position.Floor - frontLeft.y);

		qBikeItem->Pose.Orientation.x += ((xRot - qBikeItem->Pose.Orientation.x) / 2);
		qBikeItem->Pose.Orientation.z += ((zRot - qBikeItem->Pose.Orientation.z) / 2);

		if (!(qBike->Flags & QBIKE_FLAG_DEAD))
		{
			if (probe.RoomNumber != qBikeItem->RoomNumber)
			{
				ItemNewRoom(lara->Vehicle, probe.RoomNumber);
				ItemNewRoom(lara->ItemNumber, probe.RoomNumber);
			}

			laraItem->Pose = qBikeItem->Pose;

			AnimateQuadBike(laraItem, qBikeItem, collide, dead);
			AnimateItem(laraItem);

			qBikeItem->Animation.AnimNumber = Objects[ID_QUAD].animIndex + (laraItem->Animation.AnimNumber - Objects[ID_QUAD_LARA_ANIMS].animIndex);
			qBikeItem->Animation.FrameNumber = g_Level.Anims[qBikeItem->Animation.AnimNumber].frameBase + (laraItem->Animation.FrameNumber - g_Level.Anims[laraItem->Animation.AnimNumber].frameBase);

			Camera.targetElevation = -ANGLE(30.0f);

			if (qBike->Flags & QBIKE_FLAG_FALLING)
			{
				if (qBikeItem->Pose.Position.y == qBikeItem->Floor)
				{
					ExplodeVehicle(laraItem, qBikeItem);
					return false;
				}
			}
		}

		if (laraItem->Animation.ActiveState != QBIKE_STATE_MOUNT_RIGHT &&
			laraItem->Animation.ActiveState != QBIKE_STATE_MOUNT_LEFT &&
			laraItem->Animation.ActiveState != QBIKE_STATE_DISMOUNT_RIGHT &&
			laraItem->Animation.ActiveState != QBIKE_STATE_DISMOUNT_LEFT)
		{
			Vector3Int pos;
			int speed = 0;
			short angle = 0;

			for (int i = 0; i < 2; i++)
			{
				pos.x = quadEffectsPositions[i].x;
				pos.y = quadEffectsPositions[i].y;
				pos.z = quadEffectsPositions[i].z;
				GetJointAbsPosition(qBikeItem, &pos, quadEffectsPositions[i].meshNum);
				angle = qBikeItem->Pose.Orientation.y + ((i == 0) ? 0x9000 : 0x7000);
				if (qBikeItem->Animation.Velocity > 32)
				{
					if (qBikeItem->Animation.Velocity < 64)
					{
						speed = 64 - qBikeItem->Animation.Velocity;
						TriggerQuadExhaustSmoke(pos.x, pos.y, pos.z, angle, speed, 1);
					}
				}
				else
				{
					if (qBike->SmokeStart < 16)
					{
						speed = ((qBike->SmokeStart * 2) + (GetRandomControl() & 7) + (GetRandomControl() & 16)) * 128;
						qBike->SmokeStart++;
					}
					else if (qBike->DriftStarting)
						speed = (abs(qBike->Revs) * 2) + ((GetRandomControl() & 7) * 128);
					else if ((GetRandomControl() & 3) == 0)
						speed = ((GetRandomControl() & 15) + (GetRandomControl() & 16)) * 128;
					else
						speed = 0;

					TriggerQuadExhaustSmoke(pos.x, pos.y, pos.z, angle, speed, 0);
				}
			}
		}
		else
			qBike->SmokeStart = 0;

		return QuadBikeCheckGetOff(laraItem, qBikeItem);
	}
}
