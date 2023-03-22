#include "framework.h"
#include "Objects/Utils/VehicleHelpers.h"

#include "Game/collision/collide_item.h"
#include "Game/collision/sphere.h"
#include "Game/effects/simple_particle.h"
#include "Game/effects/Streamer.h"
#include "Game/effects/tomb4fx.h"
#include "Game/Lara/lara.h"
#include "Game/Lara/lara_flare.h"
#include "Game/Lara/lara_helpers.h"
#include "Game/Lara/lara_struct.h"
#include "Game/room.h"
#include "Math/Math.h"
#include "Sound/sound.h"
#include "Specific/Input/Input.h"
#include "Specific/setup.h"

using namespace TEN::Effects::Streamer;
using namespace TEN::Input;
using namespace TEN::Math;

namespace TEN::Entities::Vehicles
{
	enum class VehicleWakeEffectTag
	{
		FrontLeft,
		FrontRight,
		BackLeft,
		BackRight
	};

	// Deprecated.
	int GetVehicleHeight(const ItemInfo& vehicleItem, int forward, int right, bool clamp, Vector3i& pos)
	{
		float sinX = phd_sin(vehicleItem.Pose.Orientation.x);
		float sinY = phd_sin(vehicleItem.Pose.Orientation.y);
		float cosY = phd_cos(vehicleItem.Pose.Orientation.y);
		float sinZ = phd_sin(vehicleItem.Pose.Orientation.z);

		pos.x = vehicleItem.Pose.Position.x + (forward * sinY) + (right * cosY);
		pos.y = vehicleItem.Pose.Position.y - (forward * sinX) + (right * sinZ);
		pos.z = vehicleItem.Pose.Position.z + (forward * cosY) - (right * sinY);

		// Get collision a bit higher to be able to detect bridges.
		auto pointColl = GetCollision(pos.x, pos.y - CLICK(2), pos.z, vehicleItem.RoomNumber);

		if (pos.y < pointColl.Position.Ceiling || pointColl.Position.Ceiling == NO_HEIGHT)
			return NO_HEIGHT;

		if (pos.y > pointColl.Position.Floor && clamp)
			pos.y = pointColl.Position.Floor;

		return pointColl.Position.Floor;
	}

	VehicleMountType GetVehicleMountType(ItemInfo& vehicleItem, ItemInfo& laraItem, const CollisionInfo& coll, const std::vector<VehicleMountType>& allowedMountTypes, float maxDistance2D, float maxVerticalDistance)
	{
		const auto& player = *GetLaraInfo(&laraItem);

		// Check vehicle usability.
		if (vehicleItem.Flags & IFLAG_INVISIBLE)
			return VehicleMountType::None;

		// Check hand status.
		if (player.Control.HandStatus != HandStatus::Free)
			return VehicleMountType::None;

		// Assess vertical distance to vehicle.
		if (abs(laraItem.Pose.Position.y - vehicleItem.Pose.Position.y) > maxVerticalDistance)
			return VehicleMountType::None;

		// Assess 2D distance to vehicle.
		float distance2D = Vector2::Distance(
			Vector2(laraItem.Pose.Position.x, laraItem.Pose.Position.z),
			Vector2(vehicleItem.Pose.Position.x, vehicleItem.Pose.Position.z));
		if (distance2D > maxDistance2D)
			return VehicleMountType::None;

		// Test object collision.
		if (!TestBoundsCollide(&vehicleItem, &laraItem, coll.Setup.Radius) || !TestCollision(&vehicleItem, &laraItem))
			return VehicleMountType::None;

		bool hasInputAction = IsHeld(In::Action);

		short deltaHeadingAngle = vehicleItem.Pose.Orientation.y - laraItem.Pose.Orientation.y;
		short angleBetweenPositions = vehicleItem.Pose.Orientation.y - Geometry::GetOrientToPoint(laraItem.Pose.Position.ToVector3(), vehicleItem.Pose.Position.ToVector3()).y;
		bool isOnCorrectSide = abs(deltaHeadingAngle - angleBetweenPositions) < ANGLE(45.0f);

		// Assess mount types allowed for vehicle.
		for (const auto& mountType : allowedMountTypes)
		{
			switch (mountType)
			{
			// TODO: Holding Forward the moment a level loads, this mount will fail. Need a better method.
			case VehicleMountType::LevelStart:
				if (laraItem.Animation.Velocity == Vector3::Zero &&
					laraItem.Pose.Position == vehicleItem.Pose.Position)
				{
					break;
				}
				continue;

			case VehicleMountType::Front:
				if (hasInputAction && isOnCorrectSide &&
					deltaHeadingAngle > ANGLE(135.0f) && deltaHeadingAngle < ANGLE(-135.0f) &&
					!laraItem.Animation.IsAirborne)
				{
					break;
				}
				continue;

			case VehicleMountType::Back:
				if (hasInputAction && isOnCorrectSide &&
					abs(deltaHeadingAngle) < ANGLE(45.0f) &&
					!laraItem.Animation.IsAirborne)
				{
					break;
				}
				continue;

			case VehicleMountType::Left:
				if (hasInputAction && isOnCorrectSide &&
					deltaHeadingAngle > ANGLE(-135.0f) && deltaHeadingAngle < ANGLE(-45.0f) &&
					!laraItem.Animation.IsAirborne)
				{
					break;
				}
				continue;

			case VehicleMountType::Right:
				if (hasInputAction && isOnCorrectSide &&
					deltaHeadingAngle > ANGLE(45.0f) && deltaHeadingAngle < ANGLE(135.0f) &&
					!laraItem.Animation.IsAirborne)
				{
					break;
				}
				continue;

			case VehicleMountType::Jump:
				if (laraItem.Animation.IsAirborne &&
					laraItem.Animation.Velocity.y > 0.0f &&
					laraItem.Pose.Position.y < vehicleItem.Pose.Position.y)
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

	VehicleDismountType GetVehicleDismountType(ItemInfo& vehicleItem, const std::vector<VehicleDismountType>& allowedDismountTypes, float distance, bool isOnLand)
	{
		if (!(TrInput & VEHICLE_IN_BRAKE))
			return VehicleDismountType::None;
		
		// Assess dismount types allowed for vehicle.
		for (const auto& dismountType : allowedDismountTypes)
		{
			short headingAngle = 0;
			switch (dismountType)
			{
			case VehicleDismountType::Front:
				if (IsHeld(In::Forward))
				{
					headingAngle = ANGLE(0.0f);
					break;
				}
				continue;

			case VehicleDismountType::Back:
				if (IsHeld(In::Back))
				{
					headingAngle = ANGLE(180.0f);
					break;
				}
				continue;

			case VehicleDismountType::Left:
				if (IsHeld(In::Left))
				{
					headingAngle = ANGLE(-90.0f);
					break;
				}
				continue;

			case VehicleDismountType::Right:
				if (IsHeld(In::Right))
				{
					headingAngle = ANGLE(90.0f);
					break;
				}
				continue;
			}

			if (TestVehicleDismount(vehicleItem, dismountType, headingAngle, distance, isOnLand))
				return dismountType;
		}

		return VehicleDismountType::None;
	}

	bool TestVehicleDismount(ItemInfo& vehicleItem, VehicleDismountType dismountType, short headingAngle, float distance, bool isOnLand)
	{
		if (dismountType == VehicleDismountType::None)
			return false;

		auto pointColl = GetCollision(&vehicleItem, headingAngle, distance, -CLICK(2));

		// Check for wall.
		if (pointColl.Position.Floor == NO_HEIGHT)
			return false;

		// Check for slopes (if applicable).
		if (pointColl.Position.FloorSlope && isOnLand)
			return false;

		// Assess feasibility of dismount.
		if (abs(pointColl.Position.Floor - vehicleItem.Pose.Position.y) <= STEPUP_HEIGHT && // Floor is within highest step threshold.
			(pointColl.Position.Ceiling - vehicleItem.Pose.Position.y) > -LARA_HEIGHT &&	// Gap is visually permissive.
			abs(pointColl.Position.Ceiling - pointColl.Position.Floor) > LARA_HEIGHT)		// Space is not too narrom.
		{
			return true;
		}

		return false;
	}

	VehicleImpactType GetVehicleImpactType(ItemInfo& vehicleItem, const Vector3i& prevPos)
	{
		auto posDelta = vehicleItem.Pose.Position - prevPos;

		if (posDelta.x || posDelta.z)
		{
			float sinY = phd_sin(vehicleItem.Pose.Orientation.y);
			float cosY = phd_cos(vehicleItem.Pose.Orientation.y);

			int front = (posDelta.x * sinY) + (posDelta.z * cosY);
			int side = (posDelta.x * cosY) - (posDelta.z * sinY);

			if (abs(front) > abs(side))
			{
				if (front > 0)
					return VehicleImpactType::Back;
				else
					return VehicleImpactType::Front;
			}
			else
			{
				if (side > 0)
					return VehicleImpactType::Left;
				else
					return VehicleImpactType::Right;
			}
		}

		return VehicleImpactType::None;
	}

	VehiclePointCollision GetVehicleCollision(ItemInfo& vehicleItem, int forward, int right, bool clamp)
	{
		float sinX = phd_sin(vehicleItem.Pose.Orientation.x);
		float sinY = phd_sin(vehicleItem.Pose.Orientation.y);
		float cosY = phd_cos(vehicleItem.Pose.Orientation.y);
		float sinZ = phd_sin(vehicleItem.Pose.Orientation.z);

		auto point = Vector3i(
			vehicleItem.Pose.Position.x + (forward * sinY) + (right * cosY),
			vehicleItem.Pose.Position.y - (forward * sinX) + (right * sinZ),
			vehicleItem.Pose.Position.z + (forward * cosY) - (right * sinY));

		// Get collision a bit higher to be able to detect bridges.
		auto pointColl = GetCollision(point.x, point.y - CLICK(2), point.z, vehicleItem.RoomNumber);

		if (point.y < pointColl.Position.Ceiling || pointColl.Position.Ceiling == NO_HEIGHT)
			return VehiclePointCollision{ point, NO_HEIGHT, NO_HEIGHT};

		if (point.y > pointColl.Position.Floor && clamp)
			point.y = pointColl.Position.Floor;

		return VehiclePointCollision{ point, pointColl.Position.Floor, pointColl.Position.Ceiling };
	}
	
	int GetVehicleWaterHeight(ItemInfo& vehicleItem, int forward, int right, bool clamp, Vector3i& pos)
	{
		auto rotMatrix = vehicleItem.Pose.Orientation.ToRotationMatrix();
		auto tMatrix = Matrix::CreateTranslation(vehicleItem.Pose.Position.ToVector3());
		auto worldMatrix = rotMatrix * tMatrix;

		auto point = Vector3(right, 0, forward);
		point = Vector3::Transform(point, worldMatrix);
		pos = Vector3i(point);

		auto pointColl = GetCollision(pos.x, pos.y, pos.z, vehicleItem.RoomNumber);
		int height = GetWaterHeight(pos.x, pos.y, pos.z, pointColl.RoomNumber);

		if (height == NO_HEIGHT)
		{
			height = pointColl.Position.Floor;
			if (height == NO_HEIGHT)
				return height;
		}

		height -= 5;

		if (pos.y > height && clamp)
			pos.y = height;

		return height;
	}

	void DoVehicleCollision(ItemInfo& vehicleItem, int radius)
	{
		CollisionInfo coll = {};
		coll.Setup.Radius = radius * 0.8f; // HACK: Most vehicles use radius larger than needed.
		coll.Setup.UpperCeilingBound = MAX_HEIGHT; // HACK: this needs to be set to prevent GCI result interference.
		coll.Setup.OldPosition = vehicleItem.Pose.Position;
		coll.Setup.EnableObjectPush = true;

		DoObjectCollision(&vehicleItem, &coll);
	}

	// Motorbike and jeep had the values in their dynamics functions tweaked to make them feel heavier.
	// Using this unified function, they leap off hills as easily as the older vehicles for now. -- Sezz 2022.06.30
	float DoVehicleDynamics(int height, float verticalVelocity, int minBounce, int maxKick, int& outYPos, float weightMult)
	{
		// Grounded.
		if (height > outYPos)
		{
			outYPos += verticalVelocity;
			if (outYPos > (height - minBounce))
			{
				outYPos = height;
				verticalVelocity = 0.0f;
			}
			else
			{
				verticalVelocity += GRAVITY * weightMult;
			}
		}
		// Airborne.
		else
		{
			int kick = (height - outYPos) * 4;
			if (kick < maxKick)
				kick = maxKick;

			verticalVelocity += (kick - verticalVelocity) / 8;

			if (outYPos > height)
				outYPos = height;
		}

		return verticalVelocity;
	}

	// Temp. scaffolding function. Shifts need a rework.
	void CalculateVehicleShift(ItemInfo& vehicleItem, short& outExtraRot, const VehiclePointCollision& prevPoint, int height, int front, int side, int step, bool clamp)
	{
		auto vPointColl = GetVehicleCollision(vehicleItem, front, side, clamp);

		if (vPointColl.FloorHeight < (prevPoint.Position.y - step) ||		  // Floor is beyond upper bound.
			//point.CeilingHeight > (prevPoint.Position.y - height) ||		  // Ceiling is too low. Will get stuck on jumps that hit the ceiling.
			abs(vPointColl.CeilingHeight - vPointColl.FloorHeight) <= height) // Floor and ceiling form a clamp.
		{
			outExtraRot += DoVehicleShift(vehicleItem, vPointColl.Position, prevPoint.Position);
		}
	}

	short DoVehicleShift(ItemInfo& vehicleItem, const Vector3i& pos, const Vector3i& prevPos)
	{
		auto alignedPos = pos / BLOCK(1);
		auto alignedPrevPos = prevPos / BLOCK(1);
		auto alignedShift = Vector3i(
			pos.x & WALL_MASK,
			0,
			pos.z & WALL_MASK);

		if (alignedPos.x == alignedPrevPos.x)
		{
			if (alignedPos.z == alignedPrevPos.z)
			{
				vehicleItem.Pose.Position.x += (prevPos.x - pos.x);
				vehicleItem.Pose.Position.z += (prevPos.z - pos.z);
			}
			else if (alignedPos.z > alignedPrevPos.z)
			{
				vehicleItem.Pose.Position.z -= alignedShift.z + 1;
				return (pos.x - vehicleItem.Pose.Position.x);
			}
			else
			{
				vehicleItem.Pose.Position.z += BLOCK(1) - alignedShift.z;
				return (vehicleItem.Pose.Position.x - pos.x);
			}
		}
		else if (alignedPos.z == alignedPrevPos.z)
		{
			if (alignedPos.x > alignedPrevPos.x)
			{
				vehicleItem.Pose.Position.x -= alignedShift.x + 1;
				return (vehicleItem.Pose.Position.z - pos.z);
			}
			else
			{
				vehicleItem.Pose.Position.x += BLOCK(1) - alignedShift.x;
				return (pos.z - vehicleItem.Pose.Position.z);
			}
		}
		else
		{
			alignedPos = Vector3i::Zero;

			auto pointColl = GetCollision(prevPos.x, pos.y, pos.z, vehicleItem.RoomNumber);
			if (pointColl.Position.Floor < (prevPos.y - CLICK(1)))
			{
				if (pos.z > prevPos.z)
					alignedPos.z = -alignedShift.z - 1;
				else
					alignedPos.z = BLOCK(1) - alignedShift.z;
			}

			pointColl = GetCollision(pos.x, pos.y, prevPos.z, vehicleItem.RoomNumber);
			if (pointColl.Position.Floor < (prevPos.y - CLICK(1)))
			{
				if (pos.x > prevPos.x)
					alignedPos.x = -alignedShift.x - 1;
				else
					alignedPos.x = BLOCK(1) - alignedShift.x;
			}

			// NOTE: Commented lines are skidoo-specific. Likely unnecessary but keeping for reference. -- Sezz 2022.06.30
			if (alignedPos.x && alignedPos.z)
			{
				vehicleItem.Pose.Position.z += alignedPos.z;
				vehicleItem.Pose.Position.x += alignedPos.x;
				//vehicleItem.Animation.Velocity -= 50.0f;
			}
			else if (alignedPos.z)
			{
				vehicleItem.Pose.Position.z += alignedPos.z;
				//vehicleItem.Animation.Velocity -= 50.0f;

				if (alignedPos.z > 0)
					return (vehicleItem.Pose.Position.x - pos.x);
				else
					return (pos.x - vehicleItem.Pose.Position.x);
			}
			else if (alignedPos.x)
			{
				vehicleItem.Pose.Position.x += alignedPos.x;
				//vehicleItem.Animation.Velocity -= 50.0f;

				if (alignedPos.x > 0)
					return (pos.z - vehicleItem.Pose.Position.z);
				else
					return (vehicleItem.Pose.Position.z - pos.z);
			}
			else
			{
				vehicleItem.Pose.Position.z += prevPos.z - pos.z;
				vehicleItem.Pose.Position.x += prevPos.x - pos.x;
				//vehicleItem.Animation.Velocity -= 50.0f;
			}
		}

		return 0;
	}

	float DoVehicleWaterMovement(ItemInfo& vehicleItem, ItemInfo& laraItem, float currentVelocity, int radius, short& turnRate, const Vector3& wakeOffset)
	{
		if (TestEnvironment(ENV_FLAG_WATER, &vehicleItem) ||
			TestEnvironment(ENV_FLAG_SWAMP, &vehicleItem))
		{
			float waterDepth = (float)GetWaterDepth(&vehicleItem);
			float waterHeight = vehicleItem.Pose.Position.y - GetWaterHeight(&vehicleItem);

			// HACK: Sometimes quadbike test position may end up under non-portal ceiling block.
			// GetWaterDepth returns DEEP_WATER constant in that case, which is too large for our needs.
			if (waterDepth == DEEP_WATER)
				waterDepth = VEHICLE_WATER_HEIGHT_MAX;

			if (waterDepth <= VEHICLE_WATER_HEIGHT_MAX)
			{
				bool isWater = TestEnvironment(ENV_FLAG_WATER, &vehicleItem);

				if (currentVelocity != 0.0f)
				{
					float coeff = isWater ? VEHICLE_WATER_VELOCITY_COEFF : VEHICLE_SWAMP_VELOCITY_COEFF;
					currentVelocity -= std::copysign(currentVelocity * ((waterDepth / VEHICLE_WATER_HEIGHT_MAX) / coeff), currentVelocity);

					if (Random::TestProbability(1 / 7.0f))
						SoundEffect(SFX_TR4_LARA_WADE, &Pose(vehicleItem.Pose.Position), SoundEnvironment::Land, isWater ? 0.8f : 0.7f);

					if (isWater)
					{
						int waterHeight = GetWaterHeight(vehicleItem);
						SpawnVehicleWake(*vehicleItem, wakeOffset, waterHeight);
					}
				}

				if (turnRate)
				{
					float coeff = isWater ? VEHICLE_WATER_TURN_RATE_COEFF : VEHICLE_SWAMP_TURN_RATE_COEFF;
					turnRate -= turnRate * ((waterDepth / VEHICLE_WATER_HEIGHT_MAX) / coeff);
				}
			}
			else
			{
				if (waterDepth > VEHICLE_WATER_HEIGHT_MAX && waterHeight > VEHICLE_WATER_HEIGHT_MAX)
				{
					ExplodeVehicle(laraItem, vehicleItem);
				}
				else if (Random::GenerateInt(0, 32) > 25)
				{
					Splash(&vehicleItem);
				}
				}
			}
		}

		return currentVelocity;
	}

	// TODO: Allow flares on every vehicle. Boats have this already.
	void DoVehicleFlareDiscard(ItemInfo& laraItem)
	{
		auto& player = *GetLaraInfo(&laraItem);

		if (player.Control.Weapon.GunType != LaraWeaponType::Flare)
			return;

		CreateFlare(&laraItem, ID_FLARE_ITEM, 0);
		UndrawFlareMeshes(&laraItem);
		player.Control.Weapon.GunType = LaraWeaponType::None;
		player.Control.Weapon.RequestGunType = LaraWeaponType::None;
		player.Flare.ControlLeft = false;
	}

	// TODO: Vehicle turn rates must be affected by speed for more tactile modulation. Slower speed = slower turn rate.
	short ModulateVehicleTurnRate(short turnRate, short accelRate, short minTurnRate, short maxTurnRate, float axisCoeff, bool invert)
	{
		axisCoeff *= invert ? -1 : 1;
		int sign = std::copysign(1, axisCoeff);

		short minTurnRateNorm = minTurnRate * abs(axisCoeff);
		short maxTurnRateNorm = maxTurnRate * abs(axisCoeff);

		short newTurnRate = (turnRate + (accelRate * sign)) * sign;
		newTurnRate = std::clamp(newTurnRate, minTurnRateNorm, maxTurnRateNorm);
		return (newTurnRate * sign);
	}

	void ModulateVehicleTurnRateX(short& turnRate, short accelRate, short minTurnRate, short maxTurnRate, bool invert)
	{
		turnRate = ModulateVehicleTurnRate(turnRate, accelRate, minTurnRate, maxTurnRate, AxisMap[InputAxis::MoveVertical], invert);
	}

	void ModulateVehicleTurnRateY(short& turnRate, short accelRate, short minTurnRate, short maxTurnRate, bool invert)
	{
		turnRate = ModulateVehicleTurnRate(turnRate, accelRate, minTurnRate, maxTurnRate, AxisMap[InputAxis::MoveHorizontal], invert);
	}

	short ResetVehicleTurnRate(short turnRate, short decelRate)
	{
		if (abs(turnRate) > decelRate)
		{
			int sign = std::copysign(1, turnRate);
			return (turnRate - (decelRate * sign));
		}
		else
		{
			return 0;
		}
	}

	void ResetVehicleTurnRateX(short& turnRate, short decelRate)
	{
		turnRate = ResetVehicleTurnRate(turnRate, decelRate);
	}

	void ResetVehicleTurnRateY(short& turnRate, short decelRate)
	{
		turnRate = ResetVehicleTurnRate(turnRate, decelRate);
	}
	
	void ModulateVehicleLean(ItemInfo& vehicleItem, short baseRate, short maxAngle)
	{
		float axisCoeff = AxisMap[InputAxis::MoveHorizontal];
		int sign = copysign(1, axisCoeff);
		short maxAngleNorm = maxAngle * axisCoeff;
		vehicleItem.Pose.Orientation.z += std::min<short>(baseRate, abs(maxAngleNorm - vehicleItem.Pose.Orientation.z) / 3) * sign;
	}

	void ResetVehicleLean(ItemInfo& vehicleItem, float alpha)
	{
		constexpr auto ANGLE_EPSILON = ANGLE(0.1f);

		if (abs(vehicleItem.Pose.Orientation.z) > ANGLE_EPSILON)
			vehicleItem.Pose.Orientation.z *= alpha;
		else
			vehicleItem.Pose.Orientation.z = 0;
	}

	static std::pair<Vector3, Vector3> GetVehicleWakePositions(const ItemInfo& vehicleItem, const Vector3& relOffset, int waterHeight,
															   bool isUnderwater, bool isMovingForward)
	{
		constexpr auto HEIGHT_OFFSET_ON_WATER = (int)BLOCK(1 / 32.0f);
		
		waterHeight -= HEIGHT_OFFSET_ON_WATER;

		int vPos = isUnderwater ? vehicleItem.Pose.Position.y : waterHeight;
		auto posBase = Vector3(vehicleItem.Pose.Position.x, vPos, vehicleItem.Pose.Position.z);
		auto orient = isUnderwater ? vehicleItem.Pose.Orientation : EulerAngles(0, vehicleItem.Pose.Orientation.y, 0);
		auto rotMatrix = orient.ToRotationMatrix();

		// Calculate relative offsets.
		// NOTE: X and Z offsets are flipped accordingly.
		auto relOffsetLeft = Vector3(-relOffset.x, relOffset.y, isMovingForward ? relOffset.z : -relOffset.z);
		auto relOffsetRight = Vector3(relOffset.x, relOffset.y, isMovingForward ? relOffset.z : -relOffset.z);

		// Calculate positions.
		auto posLeft = posBase + Vector3::Transform(relOffsetLeft, rotMatrix);
		auto posRight = posBase + Vector3::Transform(relOffsetRight, rotMatrix);

		// Clamp vertical positions to water surface.
		posLeft.y = (posLeft.y < waterHeight) ? waterHeight : posLeft.y;
		posRight.y = (posRight.y < waterHeight) ? waterHeight : posRight.y;

		// Return left and right positions in pair.
		return std::pair(posLeft, posRight);
	}

	void SpawnVehicleWake(const ItemInfo& vehicleItem, const Vector3& relOffset, int waterHeight, bool isUnderwater)
	{
		constexpr auto COLOR				 = Vector4(0.75f);
		constexpr auto LIFE_MAX				 = 2.5f;
		constexpr auto VEL_ABS				 = 4.0f;
		constexpr auto SCALE_RATE_ON_WATER	 = 6.0f;
		constexpr auto SCALE_RATE_UNDERWATER = 1.5f;

		// Vehicle is out of water; return early.
		if (waterHeight == NO_HEIGHT)
			return;

		bool isMovingForward = (vehicleItem.Animation.Velocity.z >= 0.0f);

		// Determine tags.
		auto tagLeft = isMovingForward ? VehicleWakeEffectTag::FrontLeft : VehicleWakeEffectTag::BackLeft;
		auto tagRight = isMovingForward ? VehicleWakeEffectTag::FrontRight : VehicleWakeEffectTag::BackRight;

		// Determine key parameters.
		auto positions = GetVehicleWakePositions(vehicleItem, relOffset, waterHeight, isUnderwater, isMovingForward);
		auto direction = -vehicleItem.Pose.Orientation.ToDirection();
		short orient2D = isUnderwater ? vehicleItem.Pose.Orientation.z : 0;
		float life = isUnderwater ? (LIFE_MAX / 2) : LIFE_MAX;
		float vel = isMovingForward ? VEL_ABS : -VEL_ABS;
		float scaleRate = isUnderwater ? SCALE_RATE_UNDERWATER : SCALE_RATE_ON_WATER;

		// Spawn left wake.
		StreamerEffect.Spawn(
			vehicleItem.Index, (int)tagLeft,
			positions.first, direction, orient2D, COLOR,
			0.0f, life, vel, scaleRate, 0, (int)StreamerFlags::FadeLeft);

		// Spawn right wake.
		StreamerEffect.Spawn(
			vehicleItem.Index, (int)tagRight,
			positions.second, direction, orient2D, COLOR,
			0.0f, life, vel, scaleRate, 0, (int)StreamerFlags::FadeRight);
	}
}
