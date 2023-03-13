#include "framework.h"
#include "Game/effects/footprint.h"

#include "Game/animation.h"
#include "Game/collision/collide_room.h"
#include "Game/control/control.h"
#include "Game/items.h"
#include "Game/Lara/lara.h"
#include "Math/Math.h"
#include "Sound/sound.h"
#include "Specific/clock.h"
#include "Specific/level.h"

using namespace TEN::Math;

namespace TEN::Effects::Footprints
{
	std::deque<FOOTPRINT_STRUCT> footprints = std::deque<FOOTPRINT_STRUCT>();

	bool CheckFootOnFloor(const ItemInfo& item, int mesh, Vector3& outFootprintPosition)
	{
		int x = item.Pose.Position.x;
		int y = item.Pose.Position.y;
		int z = item.Pose.Position.z;
		short roomNumber = item.RoomNumber;

		auto floor = GetFloor(x, y, z, &roomNumber);
		auto pos = GetJointPosition(LaraItem, mesh, Vector3i(0, FOOT_HEIGHT_OFFSET, 0));
		int height = GetFloorHeight(floor, pos.x, pos.y - CLICK(1), pos.z);

		outFootprintPosition.x = pos.x;
		outFootprintPosition.y = height - 8;
		outFootprintPosition.z = pos.z;

		return abs(pos.y - height) < 64;
	}

	void AddFootprint(ItemInfo* item, bool rightFoot)
	{
		if (item != LaraItem)
			return;

		auto foot = rightFoot ? LM_RFOOT : LM_LFOOT;

		// Don't process actual footprint placement if foot isn't on floor.
		auto footPos = Vector3();
		if (!CheckFootOnFloor(*item, foot, footPos))
			return;

		// Slightly randomize foot position to avoid patterns.
		footPos.x += (GetRandomControl() & 10) - 5;
		footPos.z += (GetRandomControl() & 10) - 5;

		auto result = GetCollision(footPos.x, footPos.y - STEP_SIZE, footPos.z, item->RoomNumber);
		auto floor = result.BottomBlock;

		// Don't process material if foot has hit bridge object.
		if (result.Position.Bridge >= 0)
			return;

		auto fx = SOUND_EFFECTS::SFX_TR4_LARA_FOOTSTEPS;
		// Choose material for footstep sound
		switch (floor->Material)
		{
		case MaterialType::Concrete:
			fx = SOUND_EFFECTS::SFX_TR4_LARA_FOOTSTEPS;
			break;

		case MaterialType::Grass:
			fx = SOUND_EFFECTS::SFX_TR4_LARA_FOOTSTEPS_GRASS;
			break;

		case MaterialType::Gravel:
			fx = SOUND_EFFECTS::SFX_TR4_LARA_FOOTSTEPS_GRAVEL;
			break;

		case MaterialType::Ice:
			fx = SOUND_EFFECTS::SFX_TR4_LARA_FOOTSTEPS_ICE;
			break;

		case MaterialType::Marble:
			fx = SOUND_EFFECTS::SFX_TR4_LARA_FOOTSTEPS_MARBLE;
			break;

		case MaterialType::Metal:
			fx = SOUND_EFFECTS::SFX_TR4_LARA_FOOTSTEPS_METAL;
			break;

		case MaterialType::Mud:
			fx = SOUND_EFFECTS::SFX_TR4_LARA_FOOTSTEPS_MUD;
			break;

		case MaterialType::OldMetal:
			fx = SOUND_EFFECTS::SFX_TR4_LARA_FOOTSTEPS_METAL;
			break;

		case MaterialType::OldWood:
			fx = SOUND_EFFECTS::SFX_TR4_LARA_FOOTSTEPS_WOOD;
			break;

		case MaterialType::Sand:
			fx = SOUND_EFFECTS::SFX_TR4_LARA_FOOTSTEPS_SAND;
			break;

		case MaterialType::Snow:
			fx = SOUND_EFFECTS::SFX_TR4_LARA_FOOTSTEPS_SNOW;
			break;

		case MaterialType::Stone:
			fx = SOUND_EFFECTS::SFX_TR4_LARA_FOOTSTEPS;
			break;

		case MaterialType::Water:
			fx = SOUND_EFFECTS::SFX_TR4_LARA_WET_FEET;
			break;

		case MaterialType::Wood:
			fx = SOUND_EFFECTS::SFX_TR4_LARA_FOOTSTEPS_WOOD;
			break;

		case MaterialType::Custom1:
			fx = SOUND_EFFECTS::SFX_CUSTOM_FOOTSTEP_1;
		break;

		case MaterialType::Custom2:
			fx = SOUND_EFFECTS::SFX_CUSTOM_FOOTSTEP_2;
		break; 
		
		case MaterialType::Custom3:
			fx = SOUND_EFFECTS::SFX_CUSTOM_FOOTSTEP_3;
		break;
		
		case MaterialType::Custom4:
			fx = SOUND_EFFECTS::SFX_CUSTOM_FOOTSTEP_4;
		break; 
		
		case MaterialType::Custom5:
			fx = SOUND_EFFECTS::SFX_CUSTOM_FOOTSTEP_5;
			break;

		case MaterialType::Custom6:
			fx = SOUND_EFFECTS::SFX_CUSTOM_FOOTSTEP_6;
			break;

		case MaterialType::Custom7:
			fx = SOUND_EFFECTS::SFX_CUSTOM_FOOTSTEP_7;
			break;

		case MaterialType::Custom8:
			fx = SOUND_EFFECTS::SFX_CUSTOM_FOOTSTEP_8;
			break;
		}

		// HACK: Must be here until reference WAD2 is revised.
		if (fx != SOUND_EFFECTS::SFX_TR4_LARA_FOOTSTEPS)
			SoundEffect(fx, &item->Pose);

		if (floor->Material != MaterialType::Sand &&
			floor->Material != MaterialType::Snow &&
			floor->Material != MaterialType::Gravel &&
			floor->Material != MaterialType::Mud &&
			floor->Material != MaterialType::Custom2 &&
			floor->Material != MaterialType::Custom1 &&
			floor->Material != MaterialType::Custom3 &&
			floor->Material != MaterialType::Custom4)
		{
			return;
		}

		// Calculate footprint tilts.
		auto plane = floor->FloorCollision.Planes[floor->GetSurfacePlaneIndex(footPos.x, footPos.z, true)];
		auto c = phd_cos(item->Pose.Orientation.y + ANGLE(180.0f));
		auto s = phd_sin(item->Pose.Orientation.y + ANGLE(180.0f));
		auto yRot = TO_RAD(item->Pose.Orientation.y);
		auto xRot = plane.x * s + plane.y * c;
		auto zRot = plane.y * s - plane.x * c;

		// Calculate footprint positions.
		auto p0 = Vector3( FOOTPRINT_SIZE, 0,  FOOTPRINT_SIZE);
		auto p1 = Vector3(-FOOTPRINT_SIZE, 0,  FOOTPRINT_SIZE);
		auto p2 = Vector3(-FOOTPRINT_SIZE, 0, -FOOTPRINT_SIZE);
		auto p3 = Vector3( FOOTPRINT_SIZE, 0, -FOOTPRINT_SIZE);
		auto rot = Matrix::CreateFromYawPitchRoll(yRot, xRot, zRot);
		p0 = XMVector3Transform(p0, rot);
		p1 = XMVector3Transform(p1, rot);
		p2 = XMVector3Transform(p2, rot);
		p3 = XMVector3Transform(p3, rot);
		p0 += Vector3(footPos.x, footPos.y, footPos.z);
		p1 += Vector3(footPos.x, footPos.y, footPos.z);
		p2 += Vector3(footPos.x, footPos.y, footPos.z);
		p3 += Vector3(footPos.x, footPos.y, footPos.z);

		// Get blocks for every footprint corner
		auto c0 = GetCollision(p0.x, footPos.y - STEP_SIZE, p0.z, item->RoomNumber);
		auto c1 = GetCollision(p1.x, footPos.y - STEP_SIZE, p1.z, item->RoomNumber);
		auto c2 = GetCollision(p2.x, footPos.y - STEP_SIZE, p2.z, item->RoomNumber);
		auto c3 = GetCollision(p3.x, footPos.y - STEP_SIZE, p3.z, item->RoomNumber);

		// Don't process footprint placement if all foot corners aren't on the same tilt level
		if ((c0.FloorTilt.x != c1.FloorTilt.x) || (c1.FloorTilt.x != c2.FloorTilt.x) || (c2.FloorTilt.x != c3.FloorTilt.x))
			return;
		if ((c0.FloorTilt.y != c1.FloorTilt.y) || (c1.FloorTilt.y != c2.FloorTilt.y) || (c2.FloorTilt.y != c3.FloorTilt.y))
			return;

		// Don't process footprint placement if all foot corners aren't on the same height
		if ((abs(c0.Position.Floor - c1.Position.Floor) > STEP_SIZE / 2) ||
			(abs(c1.Position.Floor - c2.Position.Floor) > STEP_SIZE / 2) ||
			(abs(c2.Position.Floor - c3.Position.Floor) > STEP_SIZE / 2) ||
			(abs(c3.Position.Floor - c0.Position.Floor) > STEP_SIZE / 2))
			return;

		// Construct footprint
		FOOTPRINT_STRUCT footprint = {};
		footprint.Position[0] = p0;
		footprint.Position[1] = p1;
		footprint.Position[2] = p2;
		footprint.Position[3] = p3;
		footprint.StartOpacity = 0.5f;
		footprint.LifeStartFading = FPS * 10;
		footprint.Life = FPS * 20;
		footprint.Active = true;
		footprint.RightFoot = rightFoot;

		// Add footprint
		if (footprints.size() >= MAX_FOOTPRINTS)
			footprints.pop_back();
		footprints.push_front(footprint);
	}

	void UpdateFootprints()
	{
		if (footprints.size() == 0)
			return;

		int numInvalidFootprints = 0;

		for (auto i = footprints.begin(); i != footprints.end(); i++) 
		{
			FOOTPRINT_STRUCT& footprint = *i;
			footprint.Life--;

			if (footprint.Life <= 0) 
			{
				numInvalidFootprints++;
				continue;
			}

			if (footprint.Life > footprint.LifeStartFading) 
			{
				footprint.Opacity = footprint.StartOpacity;
			}
			else 
			{
				float opacity = Lerp(0.0f, footprint.StartOpacity, fmax(0, fmin(1, footprint.Life / (float)footprint.LifeStartFading)));
				footprint.Opacity = opacity;
			}
		}

		for (int i = 0; i < numInvalidFootprints; i++) 
		{
			footprints.pop_back();
		}
	}
}
