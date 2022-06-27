#pragma once
#include "Specific/phd_global.h"
#include "Specific/trmath.h"

using std::vector;

struct CollisionInfo;
struct CollisionResult;
struct ItemInfo;

namespace TEN::Entities::Vehicles
{
	constexpr int VEHICLE_VELOCITY_SCALE = 256; // TODO: Deal with this nonsense *immediately* post-beta. @Sezz 2022.06.25

	constexpr int	VEHICLE_SINK_VELOCITY		  = 15;
	constexpr int	VEHICLE_WATER_HEIGHT_MAX	  = CLICK(2.5f);
	constexpr float VEHICLE_WATER_VELOCITY_COEFF  = 16.0f;
	constexpr float VEHICLE_WATER_TURN_RATE_COEFF = 10.0f;
	constexpr float VEHICLE_SWAMP_VELOCITY_COEFF  = 8.0f;
	constexpr float VEHICLE_SWAMP_TURN_RATE_COEFF = 6.0f;

	enum class VehicleMountType
	{
		None,
		LevelStart,
		Front,
		Back,
		Left,
		Right,
		Jump
	};

	VehicleMountType GetVehicleMountType(ItemInfo* vehicleItem, ItemInfo* laraItem, CollisionInfo* coll, vector<VehicleMountType> allowedMountTypes, float minDistance);
	int GetVehicleHeight(ItemInfo* vehicleItem, int forward, int right, bool clamp, Vector3Int* pos);
	int GetVehicleWaterHeight(ItemInfo* vehicleItem, int forward, int right, bool clamp, Vector3Int* pos);

	void DoVehicleCollision(ItemInfo* vehicleItem, int radius);
	int  DoVehicleWaterMovement(ItemInfo* vehicleItem, ItemInfo* laraItem, int currentVelocity, int radius, short* turnRate);

	void ModulateVehicleTurnRateX(short* turnRate, short accelRate, short minTurnRate, short maxTurnRate);
	void ModulateVehicleTurnRateY(short* turnRate, short accelRate, short minTurnRate, short maxTurnRate);
}
