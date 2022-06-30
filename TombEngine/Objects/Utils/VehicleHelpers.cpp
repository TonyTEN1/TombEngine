#include "framework.h"
#include "Objects/Utils/VehicleHelpers.h"

#include "Game/effects/simple_particle.h"
#include "Game/effects/tomb4fx.h"
#include "Game/collision/collide_item.h"
#include "Game/collision/sphere.h"
#include "Game/Lara/lara_flare.h"
#include "Game/Lara/lara_helpers.h"
#include "Game/Lara/lara_struct.h"
#include "Game/room.h"
#include "Sound/sound.h"
#include "Specific/input.h"
#include "Specific/prng.h"
#include "Specific/setup.h"

using namespace TEN::Input;

namespace TEN::Entities::Vehicles
{
	VehicleMountType GetVehicleMountType(ItemInfo* vehicleItem, ItemInfo* laraItem, CollisionInfo* coll, vector<VehicleMountType> allowedMountTypes, float maxDistance2D, float maxVerticalDistance)
	{
		auto* lara = GetLaraInfo(laraItem);

		// Assess vehicle usability.
		if (vehicleItem->Flags & IFLAG_INVISIBLE)
			return VehicleMountType::None;

		// Assess hand status.
		if (lara->Control.HandStatus != HandStatus::Free)
			return VehicleMountType::None;

		// Assess vertical distance to vehicle.
		if (abs(laraItem->Pose.Position.y - vehicleItem->Pose.Position.y) > maxVerticalDistance)
			return VehicleMountType::None;

		// Assess 2D distance to vehicle.
		float distance2D = Vector2::Distance(
			Vector2(laraItem->Pose.Position.x, laraItem->Pose.Position.z),
			Vector2(vehicleItem->Pose.Position.x, vehicleItem->Pose.Position.z));
		if (distance2D > maxDistance2D)
			return VehicleMountType::None;

		// Assess object collision.
		if (!TestBoundsCollide(vehicleItem, laraItem, coll->Setup.Radius) || !TestCollision(vehicleItem, laraItem))
			return VehicleMountType::None;

		bool hasInputAction = TrInput & IN_ACTION;

		short deltaHeadingAngle = vehicleItem->Pose.Orientation.y - laraItem->Pose.Orientation.y;
		short angleBetweenPositions = vehicleItem->Pose.Orientation.y - GetOrientBetweenPoints(laraItem->Pose.Position, vehicleItem->Pose.Position).y;
		bool onCorrectSide = abs(deltaHeadingAngle - angleBetweenPositions) < ANGLE(45.0f);

		// Assess mount types allowed for vehicle.
		for (auto mountType : allowedMountTypes)
		{
			switch (mountType)
			{
			case VehicleMountType::LevelStart:
				if (!laraItem->Animation.Velocity &&
					!laraItem->Animation.VerticalVelocity &&
					laraItem->Pose.Position == vehicleItem->Pose.Position)
				{
					break;
				}

				continue;

			case VehicleMountType::Front:
				if (hasInputAction &&
					deltaHeadingAngle > ANGLE(135.0f) && deltaHeadingAngle < -ANGLE(135.0f) &&
					onCorrectSide &&
					!laraItem->Animation.IsAirborne)
				{
					break;
				}

				continue;

			case VehicleMountType::Back:
				if (hasInputAction &&
					abs(deltaHeadingAngle) < ANGLE(45.0f) &&
					onCorrectSide &&
					!laraItem->Animation.IsAirborne)
				{
					break;
				}

				continue;

			case VehicleMountType::Left:
				if (hasInputAction &&
					deltaHeadingAngle > -ANGLE(135.0f) && deltaHeadingAngle < -ANGLE(45.0f) &&
					onCorrectSide &&
					!laraItem->Animation.IsAirborne)
				{
					break;
				}

				continue;

			case VehicleMountType::Right:
				if (hasInputAction &&
					deltaHeadingAngle > ANGLE(45.0f) && deltaHeadingAngle < ANGLE(135.0f) &&
					onCorrectSide &&
					!laraItem->Animation.IsAirborne)
				{
					break;
				}

				continue;

			case VehicleMountType::Jump:
				if (laraItem->Animation.IsAirborne &&
					laraItem->Animation.VerticalVelocity > 0 &&
					laraItem->Pose.Position.y < vehicleItem->Pose.Position.y)
				{
					break;
				}

				continue;

			default:
				return VehicleMountType::None;
			}

			return mountType;
		}

		return VehicleMountType::None;
	}

	VehicleImpactDirection GetVehicleImpactDirection(ItemInfo* vehicleItem, Vector3Int deltaPos)
	{
		deltaPos.x = vehicleItem->Pose.Position.x - deltaPos.x;
		deltaPos.z = vehicleItem->Pose.Position.z - deltaPos.z;

		if (deltaPos.x || deltaPos.z)
		{
			float sinY = phd_sin(vehicleItem->Pose.Orientation.y);
			float cosY = phd_cos(vehicleItem->Pose.Orientation.y);

			int front = (deltaPos.x * sinY) + (deltaPos.z * cosY);
			int side = (deltaPos.x * cosY) - (deltaPos.z * sinY);

			if (abs(front) > abs(side))
			{
				if (front > 0)
					return VehicleImpactDirection::Back;
				else
					return VehicleImpactDirection::Front;
			}
			else
			{
				if (side > 0)
					return VehicleImpactDirection::Left;
				else
					return VehicleImpactDirection::Right;
			}
		}

		return VehicleImpactDirection::None;
	}

	int GetVehicleHeight(ItemInfo* vehicleItem, int forward, int right, bool clamp, Vector3Int* pos)
	{
		float sinX = phd_sin(vehicleItem->Pose.Orientation.x);
		float sinY = phd_sin(vehicleItem->Pose.Orientation.y);
		float cosY = phd_cos(vehicleItem->Pose.Orientation.y);
		float sinZ = phd_sin(vehicleItem->Pose.Orientation.z);

		pos->x = vehicleItem->Pose.Position.x + (forward * sinY) + (right * cosY);
		pos->y = vehicleItem->Pose.Position.y - (forward * sinX) + (right * sinZ);
		pos->z = vehicleItem->Pose.Position.z + (forward * cosY) - (right * sinY);

		// Get collision a bit higher to be able to detect bridges.
		auto probe = GetCollision(pos->x, pos->y - CLICK(2), pos->z, vehicleItem->RoomNumber);

		if (pos->y < probe.Position.Ceiling || probe.Position.Ceiling == NO_HEIGHT)
			return NO_HEIGHT;

		if (pos->y > probe.Position.Floor && clamp)
			pos->y = probe.Position.Floor;

		return probe.Position.Floor;
	}

	int GetVehicleWaterHeight(ItemInfo* vehicleItem, int forward, int right, bool clamp, Vector3Int* pos)
	{
		Matrix world =
			Matrix::CreateFromYawPitchRoll(TO_RAD(vehicleItem->Pose.Orientation.y), TO_RAD(vehicleItem->Pose.Orientation.x), TO_RAD(vehicleItem->Pose.Orientation.z)) *
			Matrix::CreateTranslation(vehicleItem->Pose.Position.x, vehicleItem->Pose.Position.y, vehicleItem->Pose.Position.z);

		Vector3 vec = Vector3(right, 0, forward);
		vec = Vector3::Transform(vec, world);

		pos->x = vec.x;
		pos->y = vec.y;
		pos->z = vec.z;

		auto probe = GetCollision(pos->x, pos->y, pos->z, vehicleItem->RoomNumber);
		int probedRoomNumber = probe.RoomNumber;

		int height = GetWaterHeight(pos->x, pos->y, pos->z, probedRoomNumber);

		if (height == NO_HEIGHT)
		{
			height = probe.Position.Floor;
			if (height == NO_HEIGHT)
				return height;
		}

		height -= 5;

		if (pos->y > height && clamp)
			pos->y = height;

		return height;
	}

	void DoVehicleCollision(ItemInfo* vehicleItem, int radius)
	{
		CollisionInfo coll = {};
		coll.Setup.Radius = radius * 0.8f; // HACK: Most vehicles use radius larger than needed.
		coll.Setup.UpperCeilingBound = MAX_HEIGHT; // HACK: this needs to be set to prevent GCI result interference.
		coll.Setup.OldPosition = vehicleItem->Pose.Position;
		coll.Setup.EnableObjectPush = true;

		DoObjectCollision(vehicleItem, &coll);
	}

	// TODO: Extend to take a weight argument.
	// Motorbike and jeep had the values in their dynamics functions tweaked to make them feel heavier.
	// Using this unified function, they leap off hills as easily as the older vehicles for now. @Sezz 2022.06.30
	int DoVehicleDynamics(int height, int verticalVelocity, int minBounce, int maxKick, int* yPos)
	{
		// Grounded.
		if (height > *yPos)
		{
			*yPos += verticalVelocity;
			if (*yPos > (height - minBounce))
			{
				*yPos = height;
				verticalVelocity = 0;
			}
			else
				verticalVelocity += GRAVITY;
		}
		// Airborne.
		else
		{
			int kick = (height - *yPos) * 4;
			if (kick < maxKick)
				kick = maxKick;

			verticalVelocity += (kick - verticalVelocity) / 8;

			if (*yPos > height)
				*yPos = height;
		}

		return verticalVelocity;
	}

	// Temp. for reference. Flag parameter was unused. @Sezz 2022.06.30
	int DoMotorBikeDynamics(int height, int verticalVelocity, int* y, int flags)
	{
		int kick;

		if (height <= *y)
		{
			if (flags)
				return verticalVelocity;
			else
			{
				kick = (height - *y);
				if (kick < -80)
					kick = -80;

				verticalVelocity += (kick - verticalVelocity) / 16;

				if (*y > height)
					*y = height;
			}
		}
		else
		{
			*y += verticalVelocity;
			if (*y > (height - 32))
			{
				*y = height;
				verticalVelocity = 0;
			}
			else
			{
				if (flags)
					verticalVelocity += flags;
				else
					verticalVelocity += GRAVITY;
			}
		}

		return verticalVelocity;
	}

	// Temp. for reference. Flag parameter was unused. @Sezz 2022.06.30
	// TODO: Adapt damage application.
	int DoJeepDynamics(ItemInfo* laraItem, int height, int verticalVelocity, int* yPos, int flags)
	{
		if (height <= *yPos)
		{
			if (flags)
				return verticalVelocity;
			else
			{
				int kick = height - *yPos;
				if (kick < -80)
					kick = -80;

				verticalVelocity += ((kick - verticalVelocity) / 16);

				if (*yPos > height)
					*yPos = height;
			}
		}
		else
		{
			*yPos += verticalVelocity;

			if (*yPos <= (height - 32))
			{
				if (flags)
					verticalVelocity += flags + (flags / 2);
				else
					verticalVelocity += (int)((float)GRAVITY * 1.5f);
			}
			else
			{
				*yPos = height;

				if (verticalVelocity > 150)
					laraItem->HitPoints += 150 - verticalVelocity;

				verticalVelocity = 0;
			}
		}

		return verticalVelocity;
	}

	int DoVehicleShift(ItemInfo* vehicleItem, Vector3Int* pos, Vector3Int* oldPos)
	{
		int	x = pos->x / SECTOR(1);
		int z = pos->z / SECTOR(1);
		int xOld = oldPos->x / SECTOR(1);
		int zOld = oldPos->z / SECTOR(1);
		int shiftX = pos->x & (SECTOR(1) - 1);
		int shiftZ = pos->z & (SECTOR(1) - 1);

		if (x == xOld)
		{
			if (z == zOld)
			{
				vehicleItem->Pose.Position.z += (oldPos->z - pos->z);
				vehicleItem->Pose.Position.x += (oldPos->x - pos->x);
			}
			else if (z > zOld)
			{
				vehicleItem->Pose.Position.z -= shiftZ + 1;
				return (pos->x - vehicleItem->Pose.Position.x);
			}
			else
			{
				vehicleItem->Pose.Position.z += SECTOR(1) - shiftZ;
				return (vehicleItem->Pose.Position.x - pos->x);
			}
		}
		else if (z == zOld)
		{
			if (x > xOld)
			{
				vehicleItem->Pose.Position.x -= shiftX + 1;
				return (vehicleItem->Pose.Position.z - pos->z);
			}
			else
			{
				vehicleItem->Pose.Position.x += SECTOR(1) - shiftX;
				return (pos->z - vehicleItem->Pose.Position.z);
			}
		}
		else
		{
			x = 0;
			z = 0;

			auto probe = GetCollision(oldPos->x, pos->y, pos->z, vehicleItem->RoomNumber);
			if (probe.Position.Floor < (oldPos->y - CLICK(1)))
			{
				if (pos->z > oldPos->z)
					z = -shiftZ - 1;
				else
					z = SECTOR(1) - shiftZ;
			}

			probe = GetCollision(pos->x, pos->y, oldPos->z, vehicleItem->RoomNumber);
			if (probe.Position.Floor < (oldPos->y - CLICK(1)))
			{
				if (pos->x > oldPos->x)
					x = -shiftX - 1;
				else
					x = SECTOR(1) - shiftX;
			}

			// NOTE: Commented lines are skidoo-specific. Likely unnecessary but keeping for reference. @Sezz 2022.06.30
			if (x && z)
			{
				vehicleItem->Pose.Position.z += z;
				vehicleItem->Pose.Position.x += x;
				//vehicleItem->Animation.Velocity -= 50;
			}
			else if (z)
			{
				vehicleItem->Pose.Position.z += z;
				//vehicleItem->Animation.Velocity -= 50;

				if (z > 0)
					return (vehicleItem->Pose.Position.x - pos->x);
				else
					return (pos->x - vehicleItem->Pose.Position.x);
			}
			else if (x)
			{
				vehicleItem->Pose.Position.x += x;
				//vehicleItem->Animation.Velocity -= 50;

				if (x > 0)
					return (pos->z - vehicleItem->Pose.Position.z);
				else
					return (vehicleItem->Pose.Position.z - pos->z);
			}
			else
			{
				vehicleItem->Pose.Position.z += oldPos->z - pos->z;
				vehicleItem->Pose.Position.x += oldPos->x - pos->x;
				//vehicleItem->Animation.Velocity -= 50;
			}
		}

		return 0;
	}

	int DoVehicleWaterMovement(ItemInfo* vehicleItem, ItemInfo* laraItem, int currentVelocity, int radius, short* turnRate)
	{
		if (TestEnvironment(ENV_FLAG_WATER, vehicleItem) ||
			TestEnvironment(ENV_FLAG_SWAMP, vehicleItem))
		{
			auto waterDepth = (float)GetWaterDepth(vehicleItem);
			auto waterHeight = vehicleItem->Pose.Position.y - GetWaterHeight(vehicleItem);

			// HACK: Sometimes quadbike test position may end up under non-portal ceiling block.
			// GetWaterDepth returns DEEP_WATER constant in that case, which is too large for our needs.
			if (waterDepth == DEEP_WATER)
				waterDepth = VEHICLE_WATER_HEIGHT_MAX;

			if (waterDepth <= VEHICLE_WATER_HEIGHT_MAX)
			{
				bool isWater = TestEnvironment(ENV_FLAG_WATER, vehicleItem);

				if (currentVelocity != 0)
				{
					auto coeff = isWater ? VEHICLE_WATER_VELOCITY_COEFF : VEHICLE_SWAMP_VELOCITY_COEFF;
					currentVelocity -= std::copysign(currentVelocity * ((waterDepth / VEHICLE_WATER_HEIGHT_MAX) / coeff), currentVelocity);

					if (TEN::Math::Random::GenerateInt(0, 32) > 28)
						SoundEffect(SFX_TR4_LARA_WADE, &PHD_3DPOS(vehicleItem->Pose.Position), SoundEnvironment::Land, isWater ? 0.8f : 0.7f);

					if (isWater)
						TEN::Effects::TriggerSpeedboatFoam(vehicleItem, Vector3(0, -waterDepth / 2.0f, -radius));
				}

				if (*turnRate)
				{
					auto coeff = isWater ? VEHICLE_WATER_TURN_RATE_COEFF : VEHICLE_SWAMP_TURN_RATE_COEFF;
					*turnRate -= *turnRate * ((waterDepth / VEHICLE_WATER_HEIGHT_MAX) / coeff);
				}
			}
			else
			{
				if (waterDepth > VEHICLE_WATER_HEIGHT_MAX && waterHeight > VEHICLE_WATER_HEIGHT_MAX)
					ExplodeVehicle(laraItem, vehicleItem);
				else if (TEN::Math::Random::GenerateInt(0, 32) > 25)
					Splash(vehicleItem);
			}
		}

		return currentVelocity;
	}

	// TODO: Allow flares on every vehicle and see how that works. Boats already allowed them originally.
	void DoVehicleFlareDiscard(ItemInfo* laraItem)
	{
		auto* lara = GetLaraInfo(laraItem);

		if (lara->Control.Weapon.GunType == LaraWeaponType::Flare)
		{
			CreateFlare(laraItem, ID_FLARE_ITEM, 0);
			UndrawFlareMeshes(laraItem);
			lara->Control.Weapon.GunType = LaraWeaponType::None;
			lara->Control.Weapon.RequestGunType = LaraWeaponType::None;
			lara->Flare.ControlLeft = false;
		}
	}

	// TODO: This is a copy-pase of the player version, but I may need a different method for vehicles.
	short ModulateVehicleTurnRate(short turnRate, short accelRate, short minTurnRate, short maxTurnRate, float axisCoeff, bool invert)
	{
		axisCoeff *= invert ? -1 : 1;

		int sign = std::copysign(1, axisCoeff);
		short minTurnRateNormalized = minTurnRate * abs(axisCoeff);
		short maxTurnRateNormalized = maxTurnRate * abs(axisCoeff);

		short newTurnRate = (turnRate + (accelRate * sign)) * sign;
		newTurnRate = std::clamp(newTurnRate, minTurnRateNormalized, maxTurnRateNormalized);
		return newTurnRate * sign;
	}

	void ModulateVehicleTurnRateX(short* turnRate, short accelRate, short minTurnRate, short maxTurnRate, bool invert)
	{
		*turnRate = ModulateVehicleTurnRate(*turnRate, accelRate, minTurnRate, maxTurnRate, AxisMap[InputAxis::MoveVertical], invert);
	}

	void ModulateVehicleTurnRateY(short* turnRate, short accelRate, short minTurnRate, short maxTurnRate, bool invert)
	{
		*turnRate = ModulateVehicleTurnRate(*turnRate, accelRate, minTurnRate, maxTurnRate, AxisMap[InputAxis::MoveHorizontal], invert);
	}

	void ApplyTurnRateFriction()
	{

	}
	
	void ModulateVehicleLean(ItemInfo* vehicleItem, short baseRate, short maxAngle)
	{
		float axisCoeff = AxisMap[InputAxis::MoveHorizontal];
		int sign = copysign(1, axisCoeff);
		short maxAngleNormalized = maxAngle * axisCoeff;
		vehicleItem->Pose.Orientation.z += std::min<short>(baseRate, abs(maxAngleNormalized - vehicleItem->Pose.Orientation.z) / 3) * sign;
	}

	void ResetVehicleLean(ItemInfo* vehicleItem, float rate)
	{
		if (abs(vehicleItem->Pose.Orientation.z) > ANGLE(0.1f))
			vehicleItem->Pose.Orientation.z += vehicleItem->Pose.Orientation.z / -rate;
		else
			vehicleItem->Pose.Orientation.z = 0;
	}
}
