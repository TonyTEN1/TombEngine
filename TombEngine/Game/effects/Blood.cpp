#include "framework.h"
#include "Game/effects/Blood.h"

#include "Game/collision/collide_room.h"
#include "Game/effects/Ripple.h"
#include "Game/effects/weather.h"
#include "Math/Math.h"
#include "Renderer/Renderer11.h"
#include "Specific/clock.h"
#include "Specific/setup.h"

#include "lara.h"

#include "Game/effects/effects.h"
#include "Game/effects/tomb4fx.h"

using namespace TEN::Effects::Environment;
using namespace TEN::Effects::Ripple;
using namespace TEN::Math;
using namespace TEN::Renderer;

namespace TEN::Effects::Blood
{
	constexpr auto BLOOD_MIST_LIFE_MIN	   = 0.5f;
	constexpr auto BLOOD_MIST_LIFE_MAX	   = 1.0f;
	constexpr auto BLOOD_MIST_VELOCITY_MAX = 16.0f;
	constexpr auto BLOOD_MIST_SCALE_MIN	   = 64.0f;
	constexpr auto BLOOD_MIST_SCALE_MAX	   = 128.0f;
	constexpr auto BLOOD_MIST_OPACITY_MAX  = 0.7f;
	constexpr auto BLOOD_MIST_GRAVITY_MIN  = 1.0f;
	constexpr auto BLOOD_MIST_GRAVITY_MAX  = 2.0f;
	constexpr auto BLOOD_MIST_FRICTION	   = 4.0f;
	const	  auto BLOOD_MIST_ROTATION_MAX = ANGLE(10.0f);

	constexpr auto BLOOD_DRIP_LIFE_MAX			= 5.0f;
	constexpr auto BLOOD_DRIP_LIFE_START_FADING = 0.5f;
	constexpr auto BLOOD_DRIP_GRAVITY_MIN		= 5.0f;
	constexpr auto BLOOD_DRIP_GRAVITY_MAX		= 15.0f;
	constexpr auto BLOOD_DRIP_SPRAY_SEMIANGLE	= 50.0f;

	constexpr auto BLOOD_STAIN_LIFE_MAX			  = 5.0f * 60.0f;
	constexpr auto BLOOD_STAIN_LIFE_START_FADING  = 30.0f;
	constexpr auto BLOOD_STAIN_OPACITY_MAX		  = 0.8f;
	constexpr auto BLOOD_STAIN_POOLING_SCALE_RATE = 0.4f;
	constexpr auto BLOOD_STAIN_POOLING_TIME_DELAY = 5.0f;
	constexpr auto BLOOD_STAIN_HEIGHT_OFFSET	  = 4;
	constexpr auto BLOOD_STAIN_SPRITE_INDEX_MAX	  = 8; // TODO: Dehardcode this index range.

	constexpr auto BLOOD_COLOR_RED	 = Vector4(0.8f, 0.0f, 0.0f, 1.0f);
	constexpr auto BLOOD_COLOR_BROWN = Vector4(0.3f, 0.1f, 0.0f, 1.0f);

	std::array<BloodMist, BLOOD_MIST_NUM_MAX> BloodMists  = {};
	std::array<BloodDrip, BLOOD_DRIP_NUM_MAX> BloodDrips  = {};
	std::deque<BloodStain>					  BloodStains = {};

	BloodMist& GetFreeBloodMist()
	{
		float shortestLife = INFINITY;
		auto* oldestMistPtr = &BloodMists[0];

		for (auto& mist : BloodMists)
		{
			if (!mist.IsActive)
				return mist;

			if (mist.Life < shortestLife)
			{
				shortestLife = mist.Life;
				oldestMistPtr = &mist;
			}
		}

		return *oldestMistPtr;
	}

	BloodDrip& GetFreeBloodDrip()
	{
		float shortestLife = INFINITY;
		auto* oldestDripPtr = &BloodDrips[0];

		for (auto& drip : BloodDrips)
		{
			if (!drip.IsActive)
				return drip;

			if (drip.Life < shortestLife)
			{
				shortestLife = drip.Life;
				oldestDripPtr = &drip;
			}
		}

		return *oldestDripPtr;
	}

	std::array<Vector3, 4> GetBloodStainVertexPoints(const Vector3& pos, short orient2D, const Vector3& normal, float scale)
	{
		static constexpr auto point0 = Vector3( SQRT_2, 0.0f,  SQRT_2);
		static constexpr auto point1 = Vector3(-SQRT_2, 0.0f,  SQRT_2);
		static constexpr auto point2 = Vector3(-SQRT_2, 0.0f, -SQRT_2);
		static constexpr auto point3 = Vector3( SQRT_2, 0.0f, -SQRT_2);

		// No scale; return early.
		if (scale == 0.0f)
			return std::array<Vector3, 4>{ pos, pos, pos, pos };

		// Determine rotation matrix.
		auto rotMatrix = Geometry::GetRelOrientToNormal(orient2D, normal).ToRotationMatrix();

		return std::array<Vector3, 4>
		{
			pos + Vector3::Transform(point0 * (scale / 2), rotMatrix),
			pos + Vector3::Transform(point1 * (scale / 2), rotMatrix),
			pos + Vector3::Transform(point2 * (scale / 2), rotMatrix),
			pos + Vector3::Transform(point3 * (scale / 2), rotMatrix)
		};
	}

	bool TestBloodStainFloor(const Vector3& pos, int roomNumber, const std::array<Vector3, 4>& vertexPoints)
	{
		static constexpr auto heightRange = CLICK(0.5f);

		// Get point collision at every vertex point.
		auto pointColl0 = GetCollision(vertexPoints[0].x, pos.y - CLICK(1), vertexPoints[0].z, roomNumber);
		auto pointColl1 = GetCollision(vertexPoints[1].x, pos.y - CLICK(1), vertexPoints[1].z, roomNumber);
		auto pointColl2 = GetCollision(vertexPoints[2].x, pos.y - CLICK(1), vertexPoints[2].z, roomNumber);
		auto pointColl3 = GetCollision(vertexPoints[3].x, pos.y - CLICK(1), vertexPoints[3].z, roomNumber);

		// Stop scaling blood stain if floor heights at vertex points are outside relative range.
		if ((abs(pointColl0.Position.Floor - pointColl1.Position.Floor) > heightRange) ||
			(abs(pointColl1.Position.Floor - pointColl2.Position.Floor) > heightRange) ||
			(abs(pointColl2.Position.Floor - pointColl3.Position.Floor) > heightRange) ||
			(abs(pointColl3.Position.Floor - pointColl0.Position.Floor) > heightRange))
		{
			return false;
		}

		return true;
	}

	void SpawnBloodMist(const Vector3& pos, int roomNumber, const Vector3& direction)
	{
		auto& mist = GetFreeBloodMist();

		auto sphere = BoundingSphere(pos, BLOCK(1 / 16.0f));

		mist = BloodMist();
		mist.SpriteIndex = Objects[ID_BLOOD_MIST_SPRITES].meshIndex;
		mist.IsActive = true;
		mist.Position = Random::GeneratePointInSphere(sphere);
		mist.RoomNumber = roomNumber;
		mist.Orientation2D = Random::GenerateAngle();
		mist.Velocity = Random::GenerateDirectionInCone(direction, 20.0f) * Random::GenerateFloat(0.0f, BLOOD_MIST_VELOCITY_MAX);
		mist.Color = BLOOD_COLOR_RED;
		mist.Life = std::round(Random::GenerateFloat(BLOOD_MIST_LIFE_MIN, BLOOD_MIST_LIFE_MAX) * FPS);
		mist.LifeMax = mist.Life;
		mist.Scale = Random::GenerateFloat(BLOOD_MIST_SCALE_MIN, BLOOD_MIST_SCALE_MAX);
		mist.ScaleMax = mist.Scale * 4;
		mist.ScaleMin = mist.Scale;
		mist.Opacity = BLOOD_MIST_OPACITY_MAX;
		mist.OpacityMax = mist.Opacity;
		mist.Gravity = Random::GenerateFloat(BLOOD_MIST_GRAVITY_MIN, BLOOD_MIST_GRAVITY_MAX);
		mist.Friction = BLOOD_MIST_FRICTION;
		mist.Rotation = Random::GenerateAngle(-BLOOD_MIST_ROTATION_MAX, BLOOD_MIST_ROTATION_MAX);
	}

	void SpawnBloodMistCloud(const Vector3& pos, int roomNumber, const Vector3& direction, unsigned int count)
	{
		if (TestEnvironment(ENV_FLAG_WATER, roomNumber))
			return;

		auto sphere = BoundingSphere(pos, BLOCK(1 / 8.0f));

		for (int i = 0; i < count; i++)
		{
			auto pos = Random::GeneratePointInSphere(sphere);
			SpawnBloodMist(pos, roomNumber, direction);
		}
	}

	void SpawnBloodCloudUnderwater(const Vector3& pos, int roomNumber, float scale)
	{
		if (!TestEnvironment(ENV_FLAG_WATER, roomNumber))
			return;
		
		SpawnUnderwaterBlood(pos, scale);
	}

	void SpawnBloodDrip(const Vector3& pos, int roomNumber, const Vector3& velocity, float lifeInSec, float scale, bool canSpawnStain)
	{
		auto& drip = GetFreeBloodDrip();

		drip = BloodDrip();
		drip.SpriteIndex = Objects[ID_DRIP_SPRITE].meshIndex;
		drip.IsActive = true;
		drip.CanSpawnStain = canSpawnStain;
		drip.Position = pos;
		drip.RoomNumber = roomNumber;
		drip.Velocity = velocity;
		drip.Color = BLOOD_COLOR_RED;
		drip.Life = std::round(lifeInSec * FPS);
		drip.LifeStartFading = std::round(BLOOD_DRIP_LIFE_START_FADING * FPS);
		drip.Opacity = 1.0f;
		drip.Scale = scale;
		drip.Gravity = Random::GenerateFloat(BLOOD_DRIP_GRAVITY_MIN, BLOOD_DRIP_GRAVITY_MAX);
	}

	void SpawnBloodDripSpray(const Vector3& pos, int roomNumber, const Vector3& direction, const Vector3& baseVelocity, unsigned int count)
	{
		static constexpr auto minLength = 15.0f;
		static constexpr auto maxLength = 45.0f;

		// BIG TODO: Art direction needs special attention.
		// Combine mists, long drips, and round drips of various sizes.

		SpawnBloodMistCloud(pos, roomNumber, direction, count * 4);

		// Spawn decorative drips.
		for (int i = 0; i < count * 12; i++)
		{
			float length = Random::GenerateFloat(minLength, maxLength);
			auto velocity = Random::GenerateDirectionInCone(-direction, BLOOD_DRIP_SPRAY_SEMIANGLE) * length;
			float scale = length / 4;

			SpawnBloodDrip(pos, roomNumber, velocity, BLOOD_DRIP_LIFE_START_FADING, scale, false);
		}

		// Spawn special drips capable of creating stains.
		for (int i = 0; i < count; i++)
		{
			float length = Random::GenerateFloat(minLength, maxLength);
			auto velocity = baseVelocity + Random::GenerateDirectionInCone(direction, BLOOD_DRIP_SPRAY_SEMIANGLE) * length;
			float scale = length / 2;

			SpawnBloodDrip(pos, roomNumber, velocity, BLOOD_DRIP_LIFE_MAX, scale, true);
		}
	}

	void SpawnBloodStain(const Vector3& pos, int roomNumber, const Vector3& normal, float scaleMax, float scaleRate, float delayTimeInSec)
	{
		auto stain = BloodStain();

		stain.SpriteIndex = Objects[ID_BLOOD_STAIN_SPRITES].meshIndex + Random::GenerateInt(0, BLOOD_STAIN_SPRITE_INDEX_MAX);
		stain.Position = pos;
		stain.RoomNumber = roomNumber;
		stain.Orientation2D = Random::GenerateAngle();
		stain.Normal = normal;
		stain.Color = BLOOD_COLOR_RED;
		stain.ColorStart = stain.Color;
		stain.ColorEnd = BLOOD_COLOR_BROWN;
		stain.VertexPoints = GetBloodStainVertexPoints(stain.Position, stain.Orientation2D, stain.Normal, 0.0f);
		stain.Life = std::round(BLOOD_STAIN_LIFE_MAX * FPS);
		stain.LifeStartFading = std::round(BLOOD_STAIN_LIFE_START_FADING * FPS);
		stain.Scale = 0.0f;
		stain.ScaleMax = scaleMax;
		stain.ScaleRate = scaleRate;
		stain.Opacity = BLOOD_STAIN_OPACITY_MAX;
		stain.OpacityMax = stain.Opacity;
		stain.DelayTime = std::round(delayTimeInSec * FPS);

		if (BloodStains.size() >= BLOOD_STAIN_NUM_MAX)
			BloodStains.pop_back();

		BloodStains.push_front(stain);
	}

	void SpawnBloodStainFromDrip(const BloodDrip& drip, const CollisionResult& pointColl)
	{
		if (!drip.CanSpawnStain)
			return;

		auto pos = Vector3(drip.Position.x, pointColl.Position.Floor - BLOOD_STAIN_HEIGHT_OFFSET, drip.Position.z);
		auto normal = Geometry::GetFloorNormal(pointColl.FloorTilt);
		float scale = drip.Scale * 5;
		float scaleRate = std::min(drip.Velocity.Length() / 2, scale / 2);

		SpawnBloodStain(pos, drip.RoomNumber, normal, scale, scaleRate);
	}

	void SpawnBloodStainPool(ItemInfo& item)
	{
		auto pos = Vector3(item.Pose.Position.x, item.Pose.Position.y - BLOOD_STAIN_HEIGHT_OFFSET, item.Pose.Position.z);
		auto normal = Geometry::GetFloorNormal(GetCollision(&item).FloorTilt);

		auto bounds = GameBoundingBox(&item);
		float scaleMax = (bounds.GetWidth() + bounds.GetDepth()) / 2;

		SpawnBloodStain(pos, item.RoomNumber, normal, scaleMax, BLOOD_STAIN_POOLING_SCALE_RATE, BLOOD_STAIN_POOLING_TIME_DELAY);
	}

	void UpdateBloodMists()
	{
		for (auto& mist : BloodMists)
		{
			if (!mist.IsActive)
				continue;

			// Despawn.
			mist.Life -= 1.0f; // NOTE: Life tracked in frame time.
			if (mist.Life <= 0.0f)
			{
				mist.IsActive = false;
				continue;
			}

			// Update velocity.
			mist.Velocity.y += mist.Gravity;
			mist.Velocity -= mist.Velocity / mist.Friction;

			// Update position.
			mist.Position += mist.Velocity;
			mist.Orientation2D += mist.Rotation;

			// Update scale.
			mist.Scale = Lerp(mist.ScaleMin, mist.ScaleMax, 1.0f - (mist.Life / mist.LifeMax));

			// Update opacity.
			mist.Opacity = Lerp(mist.OpacityMax, 0.0f, 1.0f - (mist.Life / mist.LifeMax));
			mist.Color.w = mist.Opacity;
		}
	}

	void UpdateBloodDrips()
	{
		for (auto& drip : BloodDrips)
		{
			if (!drip.IsActive)
				continue;

			// Despawn.
			drip.Life -= 1.0f; // NOTE: Life tracked in frame time.
			if (drip.Life <= 0.0f)
			{
				drip.IsActive = false;
				continue;
			}

			// Update opacity.
			if (drip.Life <= drip.LifeStartFading)
				drip.Opacity = Lerp(1.0f, 0.0f, 1.0f - (drip.Life / std::round(BLOOD_DRIP_LIFE_START_FADING * FPS)));

			// Update color.
			drip.Color.w = drip.Opacity;

			// Update velocity.
			drip.Velocity.y += drip.Gravity;
			if (TestEnvironment(ENV_FLAG_WIND, drip.RoomNumber))
				drip.Velocity += Weather.Wind();

			// Update position.
			drip.Position += drip.Velocity;

			auto pointColl = GetCollision(drip.Position.x, drip.Position.y, drip.Position.z, drip.RoomNumber);

			drip.RoomNumber = pointColl.RoomNumber;

			// Hit water; spawn blood cloud.
			if (TestEnvironment(ENV_FLAG_WATER, drip.RoomNumber))
			{
				drip.IsActive = false;
				SpawnBloodCloudUnderwater(drip.Position, drip.RoomNumber, drip.Scale * 5);
			}
			// Hit wall or ceiling; deactivate.
			if (pointColl.Position.Floor == NO_HEIGHT || drip.Position.y <= pointColl.Position.Ceiling)
			{
				drip.IsActive = false;
			}
			// Hit floor; spawn stain.
			else if (drip.Position.y >= pointColl.Position.Floor)
			{
				drip.IsActive = false;
				SpawnBloodStainFromDrip(drip, pointColl);
			}
		}
	}

	void UpdateBloodStains()
	{
		if (BloodStains.empty())
			return;

		for (auto& stain : BloodStains)
		{
			// Update delay time.
			if (stain.DelayTime > 0.0f)
			{
				stain.DelayTime -= 1.0f;
				if (stain.DelayTime < 0.0f)
					stain.DelayTime = 0.0f;

				continue;
			}

			// Despawn.
			stain.Life -= 1.0f; // Life tracked in frame time.
			if (stain.Life <= 0.0f)
			{
				BloodStains.pop_back();
				continue;
			}

			// Update scale.
			if (stain.ScaleRate > 0.0f)
			{
				stain.Scale += stain.ScaleRate;

				// Update scale rate.
				if (stain.Scale >= (stain.ScaleMax * 0.8f))
				{
					stain.ScaleRate *= 0.2f;
					if (abs(stain.ScaleRate) <= FLT_EPSILON)
						stain.ScaleRate = 0.0f;
				}

				// Update vertex points.
				auto vertexPoints = GetBloodStainVertexPoints(stain.Position, stain.Orientation2D, stain.Normal, stain.Scale);
				if (TestBloodStainFloor(stain.Position, stain.RoomNumber, vertexPoints))
					stain.VertexPoints = vertexPoints;
				else
					stain.ScaleRate = 0.0f;
			}

			// Update opacity.
			if (stain.Life <= stain.LifeStartFading)
				stain.Opacity = Lerp(stain.OpacityMax, 0.0f, 1.0f - (stain.Life / std::round(BLOOD_STAIN_LIFE_START_FADING * FPS)));

			// Update color.
			stain.Color = Vector4::Lerp(stain.ColorStart, stain.ColorEnd, 1.0f - (stain.Life / std::round(BLOOD_STAIN_LIFE_MAX * FPS)));
			stain.Color.w = stain.Opacity;
		}
	}

	void ClearBloodMists()
	{
		BloodMists.fill(BloodMist());
	}

	void ClearBloodDrips()
	{
		BloodDrips.fill(BloodDrip());
	}

	void ClearBloodStains()
	{
		BloodStains.clear();
	}

	void DrawBloodDebug()
	{
		int numActiveMists = 0;
		for (const auto& mist : BloodMists)
		{
			if (!mist.IsActive)
				continue;

			numActiveMists++;
		}
		
		int numActiveDrips = 0;
		for (const auto& drip : BloodDrips)
		{
			if (!drip.IsActive)
				continue;

			numActiveDrips++;
		}

		g_Renderer.PrintDebugMessage("Num. blood mists: %d ", numActiveMists);
		g_Renderer.PrintDebugMessage("Num. blood drips: %d ", numActiveDrips);
		g_Renderer.PrintDebugMessage("Num. blood stains: %d ", BloodStains.size());
	}
}
