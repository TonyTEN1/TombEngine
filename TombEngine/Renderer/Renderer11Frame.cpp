#include "framework.h"
#include "Renderer/Renderer11.h"
#include "Game/animation.h"
#include "Game/Lara/lara.h"
#include "Game/effects/effects.h"
#include "Game/camera.h"
#include "Specific/level.h"
#include "Specific/setup.h"
#include "RenderView\RenderView.h"
#include "Game/items.h"

namespace TEN::Renderer
{
	using namespace TEN::Renderer;
	using TEN::Memory::LinearArrayBuffer;
	using std::vector;

	void Renderer11::CollectRooms(RenderView &renderView, bool onlyRooms)
	{
		short baseRoomIndex = renderView.camera.RoomNumber;

		for (int i = 0; i < g_Level.Rooms.size(); i++)
		{
			m_rooms[i].ItemsToDraw.clear();
			m_rooms[i].EffectsToDraw.clear();
			m_rooms[i].TransparentFacesToDraw.clear();
			m_rooms[i].StaticsToDraw.clear();
			m_rooms[i].Visited = false;
			m_rooms[i].Clip = RendererRectangle(ScreenWidth, ScreenHeight, 0, 0);
			m_rooms[i].ClipTest = RendererRectangle(ScreenWidth, ScreenHeight, 0, 0);
			m_rooms[i].BoundActive = 0;
		}

		GetVisibleObjects(renderView, onlyRooms);
	}

	void Renderer11::SetRoomBounds(ROOM_DOOR* door, short parentRoomNumber, RenderView& renderView)
	{
		RendererRoom* room = &m_rooms[door->room];
		ROOM_INFO* nativeRoom = &g_Level.Rooms[door->room];

		RendererRoom* parentRoom = &m_rooms[parentRoomNumber];
		ROOM_INFO* parentNativeRoom = &g_Level.Rooms[parentRoomNumber];

		// If parent's bounds are bigger than current bounds test, then don't do anything else
		if (room->Clip.left <= parentRoom->ClipTest.left
			&& room->Clip.right >= parentRoom->ClipTest.right
			&& room->Clip.top <= parentRoom->ClipTest.top
			&& room->Clip.bottom >= parentRoom->ClipTest.bottom)
		{
			return;
		}

		int left = parentRoom->ClipTest.right;
		int right = parentRoom->ClipTest.left;
		int top = parentRoom->ClipTest.bottom;
		int bottom = parentRoom->ClipTest.top;

		int zBehind = 0;
		int zTooFar = 0;

		Vector4 p[4];

		int xs = 0;
		int ys = 0;

		for (int i = 0; i < 4; i++)
		{
			// Project vertices of the door in clip space
			Vector4 tmp = Vector4(
				door->vertices[i].x + parentNativeRoom->x,
				door->vertices[i].y + parentNativeRoom->y,
				door->vertices[i].z + parentNativeRoom->z,
				1.0f);

			Vector4::Transform(tmp, renderView.camera.ViewProjection, p[i]);

			// Convert coordinates to screen space
			if (p[i].w > 0.0f)
			{
				p[i].x *= (1.0f / p[i].w);
				p[i].y *= (1.0f / p[i].w);
				p[i].z *= (1.0f / p[i].w);

				if (p[i].w > 0)
				{
					xs = 0.5f * (p[i].x + 1.0f) * g_Renderer.ScreenWidth;
					ys = 0.5f * (p[i].y + 1.0f) * g_Renderer.ScreenHeight;
				}
				else
				{
					xs = (p[i].x >= 0) ? 0 : g_Renderer.ScreenWidth;
					ys = (p[i].y >= 0) ? g_Renderer.ScreenHeight : 0;
				}

				// Has bound changed?
				if (xs - 1 < left)
					left = xs - 1;
				if (xs + 1 > right)
					right = xs + 1;

				if (ys - 1 < top)
					top = ys - 1;
				if (ys + 1 > bottom)
					bottom = ys + 1;
			}
			else
			{
				zBehind++;
			}
		}

		// If all vertices of the door ar behind the camera, exit now
		if (zBehind == 4)
			return;

		// If some vertices are behind the camera, we need to properly clip
		if (zBehind > 0)
		{
			for (int i = 0; i < 4; i++)
			{
				Vector4 a = p[i];
				Vector4 b = p[(i + 1) % 4];

				if ((a.z <= 0) ^ (b.z <= 0))
				{
					// X clip
					if (a.x < 0 && b.x < 0)
						left = 0;
					else if (a.x > 0 && b.x > 0)
						right = g_Renderer.ScreenWidth;
					else
					{
						left = 0;
						right = g_Renderer.ScreenWidth;
					}

					// Y clip
					if (a.y < 0 && b.y < 0)
						top = 0;
					else if (a.y > 0 && b.y > 0)
						bottom = g_Renderer.ScreenHeight;
					else
					{
						top = 0;
						bottom = g_Renderer.ScreenHeight;
					}
				}
			}
		}

		// Clip bounds to parent bounds
		if (left < parentRoom->ClipTest.left)
			left = parentRoom->ClipTest.left;
		if (right > parentRoom->ClipTest.right)
			right = parentRoom->ClipTest.right;
		if (top < parentRoom->ClipTest.top)
			top = parentRoom->ClipTest.top;
		if (bottom > parentRoom->ClipTest.bottom)
			bottom = parentRoom->ClipTest.bottom;

		if (left >= right || top >= bottom)
			return;

		// Store the new calculated bounds
		if (room->BoundActive & 2)
		{
			// The room is already in the bounds list

			if (left < room->ClipTest.left)
			{
				room->Clip.left = left;
				room->ClipTest.left = left;
			}

			if (top < room->ClipTest.top)
			{
				room->Clip.top = top;
				room->ClipTest.top = top;
			}

			if (right > room->ClipTest.right)
			{
				room->Clip.right = right;
				room->ClipTest.right = right;
			}

			if (bottom > room->ClipTest.bottom)
			{
				room->Clip.bottom = bottom;
				room->ClipTest.bottom = bottom;
			}
		}
		else
		{
			// The room must be added to the bounds list

			m_boundList[(m_boundEnd++) % MAX_ROOM_BOUNDS] = door->room;
			room->BoundActive |= 2;

			room->ClipTest.left = left;
			room->ClipTest.right = right;
			room->ClipTest.top = top;
			room->ClipTest.bottom = bottom;

			room->Clip.left = left;
			room->Clip.right = right;
			room->Clip.top = top;
			room->Clip.bottom = bottom;
		}
	}

	void Renderer11::GetRoomBounds(RenderView& renderView, bool onlyRooms)
	{
		while (m_boundStart != m_boundEnd)
		{
			short roomNumber = m_boundList[(m_boundStart++) % MAX_ROOM_BOUNDS];
			RendererRoom* room = &m_rooms[roomNumber];
			ROOM_INFO* nativeRoom = &g_Level.Rooms[roomNumber];

			room->BoundActive -= 2;

			if (room->ClipTest.left < room->Clip.left)
				room->Clip.left = room->ClipTest.left;
			if (room->ClipTest.top < room->Clip.top)
				room->Clip.top = room->ClipTest.top;
			if (room->ClipTest.right > room->Clip.right)
				room->Clip.right = room->ClipTest.right;
			if (room->ClipTest.bottom > room->Clip.bottom)
				room->Clip.bottom = room->ClipTest.bottom;

			// Add room to the list of rooms to draw
			if (!(room->BoundActive & 1))
			{
				renderView.roomsToDraw.push_back(room);

				room->BoundActive |= 1;

				if (nativeRoom->flags & ENV_FLAG_OUTSIDE)
					m_outside = ENV_FLAG_OUTSIDE;

				Vector3 roomCentre = Vector3(nativeRoom->x + nativeRoom->xSize * WALL_SIZE / 2.0f,
					(nativeRoom->minfloor + nativeRoom->maxceiling) / 2.0f,
					nativeRoom->z + nativeRoom->zSize * WALL_SIZE / 2.0f);
				Vector3 laraPosition = Vector3(Camera.pos.x, Camera.pos.y, Camera.pos.z);

				m_rooms[roomNumber].Distance = (roomCentre - laraPosition).Length();
				m_rooms[roomNumber].Visited = true;

				if (!onlyRooms)
				{
					CollectLightsForRoom(roomNumber, renderView);
					CollectItems(roomNumber, renderView);
					CollectStatics(roomNumber, renderView);
					CollectEffects(roomNumber, renderView);
				}
			}

			if (nativeRoom->flags & ENV_FLAG_OUTSIDE)
			{
				if (room->Clip.left < m_outsideClip.left)
					m_outsideClip.left = room->Clip.left;
				if (room->Clip.right > m_outsideClip.right)
					m_outsideClip.right = room->Clip.right;
				if (room->Clip.top < m_outsideClip.top)
					m_outsideClip.top = room->Clip.top;
				if (room->Clip.bottom > m_outsideClip.bottom)
					m_outsideClip.bottom = room->Clip.bottom;
			}

			for (int i = 0; i < nativeRoom->doors.size(); i++)
			{
				ROOM_DOOR* portal = &nativeRoom->doors[i];

				Vector3 n = portal->normal;
				Vector3 v = Vector3(
					Camera.pos.x - (nativeRoom->x + portal->vertices[0].x),
					Camera.pos.y - (nativeRoom->y + portal->vertices[0].y),
					Camera.pos.z - (nativeRoom->z + portal->vertices[0].z));

				// Test camera and normal positions and decide if process door or not
				if (n.Dot(v) <= 0.0f)
					continue;

				SetRoomBounds(portal, roomNumber, renderView);
			}
		}
	}

	void Renderer11::GetVisibleObjects(RenderView& renderView, bool onlyRooms)
	{
		RendererRoom* room = &m_rooms[Camera.pos.roomNumber];
		ROOM_INFO* nativeRoom = &g_Level.Rooms[Camera.pos.roomNumber];

		room->ClipTest = RendererRectangle(0, 0, ScreenWidth, ScreenHeight);
		m_outside = nativeRoom->flags & ENV_FLAG_OUTSIDE;
		m_cameraUnderwater = (nativeRoom->flags & ENV_FLAG_WATER);

		room->BoundActive = 2;

		// Initialize bounds list
		m_boundList[0] = Camera.pos.roomNumber;
		m_boundStart = 0;
		m_boundEnd = 1;

		// Horizon clipping
		if (m_outside)
		{
			m_outsideClip = RendererRectangle(0, 0, ScreenWidth, ScreenHeight);
		}
		else
		{
			m_outsideClip = RendererRectangle(ScreenWidth, ScreenHeight, 0, 0);
		}

		// Get all rooms and objects to draw
		GetRoomBounds(renderView, onlyRooms);
	}

	void Renderer11::CollectItems(short roomNumber, RenderView &renderView)
	{
		if (m_rooms.size() < roomNumber)
			return;

		RendererRoom& room = m_rooms[roomNumber];
		ROOM_INFO* r = &g_Level.Rooms[room.RoomNumber];

		short itemNum = NO_ITEM;
		for (itemNum = r->itemNumber; itemNum != NO_ITEM; itemNum = g_Level.Items[itemNum].NextItem)
		{
			ItemInfo* item = &g_Level.Items[itemNum];

			if (item->ObjectNumber == ID_LARA && itemNum == g_Level.Items[itemNum].NextItem)
				break;

			if (item->ObjectNumber == ID_LARA)
				continue;

			if (item->Status == ITEM_INVISIBLE)
				continue;

			if (!m_moveableObjects[item->ObjectNumber].has_value())
				continue;

			auto bounds = TO_DX_BBOX(item->Pose, GetBoundsAccurate(item));
			Vector3 min = bounds.Center - bounds.Extents;
			Vector3 max = bounds.Center + bounds.Extents;

			if (!renderView.camera.frustum.AABBInFrustum(min, max))
				continue;

			auto newItem = &m_items[itemNum];

			newItem->ItemNumber = itemNum;
			newItem->Translation = Matrix::CreateTranslation(item->Pose.Position.x, item->Pose.Position.y, item->Pose.Position.z);
			newItem->Rotation = Matrix::CreateFromYawPitchRoll(TO_RAD(item->Pose.Orientation.y),
															   TO_RAD(item->Pose.Orientation.x),
															   TO_RAD(item->Pose.Orientation.z));
			newItem->Scale = Matrix::CreateScale(1.0f);
			newItem->World = newItem->Rotation * newItem->Translation;

			CollectLightsForItem(item->RoomNumber, newItem, renderView);

			room.ItemsToDraw.push_back(newItem);
		}
	}

	void Renderer11::CollectStatics(short roomNumber, RenderView &renderView)
	{
		if (m_rooms.size() < roomNumber)
			return;

		RendererRoom& room = m_rooms[roomNumber];
		ROOM_INFO* r = &g_Level.Rooms[room.RoomNumber];

		if (r->mesh.size() <= 0)
			return;

		int numStatics = r->mesh.size();
		for (int i = 0; i < numStatics; i++)
		{
			auto mesh = &r->mesh[i];
			if (mesh->flags & StaticMeshFlags::SM_VISIBLE)
			{
				auto sinfo = &StaticObjects[mesh->staticNumber];

				auto bounds = TO_DX_BBOX(mesh->pos, &sinfo->visibilityBox);
				Vector3 min = bounds.Center - bounds.Extents;
				Vector3 max = bounds.Center + bounds.Extents;

				if (!renderView.camera.frustum.AABBInFrustum(min, max))
					continue;

				room.StaticsToDraw.push_back(mesh);
			}
		}
	}

	void Renderer11::CollectLightsForEffect(short roomNumber, RendererEffect *effect, RenderView &renderView)
	{
		effect->Lights.clear();
		if (m_rooms.size() < roomNumber)
			return;

		RendererRoom& room = m_rooms[roomNumber];
		ROOM_INFO* r = &g_Level.Rooms[room.RoomNumber];

		if (r->lights.size() <= 0)
			return;
		LinearArrayBuffer<RendererLight*, 8> tempLights;

		Vector3 itemPosition = Vector3(effect->Effect->pos.Position.x, effect->Effect->pos.Position.y, effect->Effect->pos.Position.z);

		// Dynamic lights have the priority
		for (int i = 0; i < dynamicLights.size(); i++)
		{
			RendererLight* light = &dynamicLights[i];

			Vector3 lightPosition = Vector3(light->Position.x, light->Position.y, light->Position.z);

			float distance = (itemPosition - lightPosition).Length();
			if (distance > light->Out)
				continue;

			tempLights.push_back(light);
		}

		int numLights = room.Lights.size();

		shadowLight = NULL;
		RendererLight* brightestLight = NULL;
		float brightest = 0.0f;

		for (int j = 0; j < numLights; j++)
		{
			RendererLight *light = &room.Lights[j];

			// Check only lights different from sun
			if (light->Type == LIGHT_TYPE_SUN)
			{
				// Sun is added without checks
			}
			else if (light->Type == LIGHT_TYPE_POINT || light->Type == LIGHT_TYPE_SHADOW)
			{
				Vector3 lightPosition = Vector3(light->Position.x, light->Position.y, light->Position.z);

				float distance = (itemPosition - lightPosition).Length();

				// Collect only lights nearer than 20 sectors
				if (distance >= 20 * WALL_SIZE)
					continue;

				// Check the out radius
				if (distance > light->Out)
					continue;

				// If Lara, try to collect shadow casting light
				if (effect->Effect->objectNumber == ID_LARA)
				{
					float attenuation = 1.0f - distance / light->Out;
					float intensity = std::max(0.0f, attenuation * (light->Color.x + light->Color.y + light->Color.z) / 3.0f);

					if (intensity >= brightest)
					{
						brightest = intensity;
						brightestLight = light;
					}
				}
			}
			else if (light->Type == LIGHT_TYPE_SPOT)
			{
				Vector3 lightPosition = Vector3(light->Position.x, light->Position.y, light->Position.z);

				float distance = (itemPosition - lightPosition).Length();

				// Collect only lights nearer than 20 sectors
				if (distance >= 20 * WALL_SIZE)
					continue;

				// Check the range
				if (distance > light->Range)
					continue;
			}
			else
			{
				// Invalid light type
				continue;
			}

			tempLights.push_back(light);
		}

		for (int i = 0; i < std::min(static_cast<size_t>(MAX_LIGHTS_PER_ITEM), tempLights.size()); i++)
		{
			if (renderView.lightsToDraw.size() < NUM_LIGHTS_PER_BUFFER - 1)
				renderView.lightsToDraw.push_back(tempLights[i]);
		}
	}

	void Renderer11::CollectLightsForItem(short roomNumber, RendererItem *item, RenderView &renderView)
	{
		item->LightsToDraw.clear();

		if (m_rooms.size() < roomNumber)
			return;

		RendererRoom& room = m_rooms[roomNumber];
		ROOM_INFO* nativeRoom = &g_Level.Rooms[room.RoomNumber];
		ItemInfo* nativeItem = &g_Level.Items[item->ItemNumber];

		std::vector<int> roomsToCheck;
		roomsToCheck.push_back(roomNumber);
		for (auto& door : nativeRoom->doors)
		{
			roomsToCheck.push_back(door.room);
		};

		// Interpolate ambient light between rooms
		if (item->PreviousRoomNumber == NO_ITEM)
		{
			item->PreviousRoomNumber = nativeItem->RoomNumber;
			item->CurrentRoomNumber = nativeItem->RoomNumber;
			item->AmbientLightSteps = AMBIENT_LIGHT_INTERPOLATION_STEPS;
		}
		else if (nativeItem->RoomNumber != item->CurrentRoomNumber)
		{
			item->PreviousRoomNumber = item->CurrentRoomNumber;
			item->CurrentRoomNumber = nativeItem->RoomNumber;
			item->AmbientLightSteps = 0;
		}
		else if (item->AmbientLightSteps < AMBIENT_LIGHT_INTERPOLATION_STEPS)
			item->AmbientLightSteps++;

		if (item->PreviousRoomNumber == NO_ITEM)
			item->AmbientLight = m_rooms[nativeItem->RoomNumber].AmbientLight;
		else
		{
			item->AmbientLight = (((AMBIENT_LIGHT_INTERPOLATION_STEPS - item->AmbientLightSteps) / (float)AMBIENT_LIGHT_INTERPOLATION_STEPS) * m_rooms[item->PreviousRoomNumber].AmbientLight +
				(item->AmbientLightSteps / (float)AMBIENT_LIGHT_INTERPOLATION_STEPS) * m_rooms[item->CurrentRoomNumber].AmbientLight);
			item->AmbientLight.w = 1.0f;
		}

		// Now collect lights from dynamic list and from rooms
		std::vector<RendererLight*> tempLights;
		tempLights.reserve(MAX_LIGHTS_DRAW);

		Vector3 itemPosition = Vector3(nativeItem->Pose.Position.x, nativeItem->Pose.Position.y, nativeItem->Pose.Position.z);

		if (nativeItem->ObjectNumber == ID_LARA)
			shadowLight = nullptr;

		RendererLight* brightestLight = NULL;
		float brightest = 0.0f;

		// Dynamic lights have the priority
		for (int i = 0; i < dynamicLights.size(); i++)
		{
			RendererLight* light = &dynamicLights[i];

			Vector3 lightPosition = Vector3(light->Position.x, light->Position.y, light->Position.z);

			float distance = (itemPosition - lightPosition).Length();
			if (distance > light->Out)
				continue;

			float attenuation = 1.0f - distance / light->Out;
			float intensity = std::max(0.0f, attenuation * (light->Color.x + light->Color.y + light->Color.z) / 3.0f);

			light->LocalIntensity = intensity;
			light->Distance = distance;

			tempLights.push_back(light);
		}

		// Check current room and also neighbour rooms
		for (int roomToCheck : roomsToCheck)
		{
			RendererRoom& currentRoom = m_rooms[roomToCheck];
			int numLights = currentRoom.Lights.size();

			for (int j = 0; j < numLights; j++)
			{
				RendererLight* light = &currentRoom.Lights[j];

				light->AffectNeighbourRooms = light->Type != LIGHT_TYPES::LIGHT_TYPE_SUN;

				if (!light->AffectNeighbourRooms && roomToCheck != roomNumber)
					continue;

				// Check only lights different from sun
				if (light->Type == LIGHT_TYPE_SUN)
				{
					// Sun is added without checks
					light->Distance = D3D11_FLOAT32_MAX;
					light->LocalIntensity = 0;
				}
				else if (light->Type == LIGHT_TYPE_POINT || light->Type == LIGHT_TYPE_SHADOW)
				{
					Vector3 lightPosition = Vector3(light->Position.x, light->Position.y, light->Position.z);

					float distance = (itemPosition - lightPosition).Length();

					// Collect only lights nearer than 20 sectors
					if (distance >= 20 * WALL_SIZE)
						continue;

					// Check the out radius
					if (distance > light->Out)
						continue;

					float attenuation = 1.0f - distance / light->Out;
					float intensity = std::max(0.0f, attenuation * (light->Color.x + light->Color.y + light->Color.z) / 3.0f);

					light->LocalIntensity = intensity;

					// If Lara, try to collect shadow casting light
					if (nativeItem->ObjectNumber == ID_LARA && light->Type == LIGHT_TYPE_POINT)
					{
						if (intensity >= brightest)
						{
							brightest = intensity;
							brightestLight = light;
						}
					}

					light->Distance = distance;
				}
				else if (light->Type == LIGHT_TYPE_SPOT)
				{
					Vector3 lightPosition = Vector3(light->Position.x, light->Position.y, light->Position.z);

					float distance = (itemPosition - lightPosition).Length();

					// Collect only lights nearer than 20 sectors
					if (distance >= 20 * WALL_SIZE)
						continue;

					// Check the range
					if (distance > light->Range)
						continue;

					float attenuation = 1.0f - distance / light->Range;
					float intensity = std::max(0.0f, attenuation * (light->Color.x + light->Color.y + light->Color.z) / 3.0f);

					light->LocalIntensity = intensity;

					// If Lara, try to collect shadow casting light
					if (nativeItem->ObjectNumber == ID_LARA)
					{
						if (intensity >= brightest)
						{
							brightest = intensity;
							brightestLight = light;
						}
					}

					light->Distance = distance;
				}
				else
				{
					// Invalid light type
					continue;
				}

				tempLights.push_back(light);
			}
		}

		if (nativeItem->ObjectNumber == ID_LARA)
			shadowLight = brightestLight;

		// Sort lights by distance
		std::sort(
			tempLights.begin(),
			tempLights.end(),
			[](RendererLight* a, RendererLight* b)
			{
				return a->LocalIntensity > b->LocalIntensity;
			}
		);

		// Add max 8 lights per item, including the shadow light for Lara eventually
		if (shadowLight != nullptr)
			item->LightsToDraw.push_back(shadowLight);
		for (int i = 0; i < tempLights.size(); i++)
		{
			if (shadowLight != nullptr && shadowLight == tempLights[i])
				continue;

			item->LightsToDraw.push_back(tempLights[i]);

			if (item->LightsToDraw.size() == MAX_LIGHTS_PER_ITEM)
				break;
		}
	}

	void Renderer11::CollectLightsForRoom(short roomNumber, RenderView &renderView)
	{
		if (m_rooms.size() < roomNumber)
			return;
		
		RendererRoom& room = m_rooms[roomNumber];
		ROOM_INFO* r = &g_Level.Rooms[roomNumber];

		int numLights = room.Lights.size();

		// Collect dynamic lights for rooms
		for (int i = 0; i < dynamicLights.size(); i++)
		{
			RendererLight* light = &dynamicLights[i];

			Vector3 boxMin = Vector3(r->x - 2 * WALL_SIZE, -(r->minfloor + STEP_SIZE), r->z - 2 * WALL_SIZE);
			Vector3 boxMax = Vector3(r->x + (r->xSize + 1) * WALL_SIZE, -(r->maxceiling - STEP_SIZE), r->z + (r->zSize + 1) * WALL_SIZE);
			Vector3 center = Vector3(light->Position.x, -light->Position.y, light->Position.z);

			if (renderView.lightsToDraw.size() < NUM_LIGHTS_PER_BUFFER - 1 &&
				SphereBoxIntersection(boxMin, boxMax, center, light->Out))
				renderView.lightsToDraw.push_back(light);
		}
	}

	void Renderer11::CollectEffects(short roomNumber, RenderView &renderView)
	{
		if (m_rooms.size() < roomNumber)
			return;

		RendererRoom& room = m_rooms[roomNumber];
		ROOM_INFO* r = &g_Level.Rooms[room.RoomNumber];

		short fxNum = NO_ITEM;
		for (fxNum = r->fxNumber; fxNum != NO_ITEM; fxNum = EffectList[fxNum].nextFx)
		{
			FX_INFO *fx = &EffectList[fxNum];

			if (fx->objectNumber < 0)
				continue;

			ObjectInfo *obj = &Objects[fx->objectNumber];

			RendererEffect *newEffect = &m_effects[fxNum];

			newEffect->Effect = fx;
			newEffect->Id = fxNum;
			newEffect->World = Matrix::CreateFromYawPitchRoll(fx->pos.Orientation.y, fx->pos.Position.x, fx->pos.Position.z) * Matrix::CreateTranslation(fx->pos.Position.x, fx->pos.Position.y, fx->pos.Position.z);
			newEffect->Mesh = GetMesh(obj->nmeshes ? obj->meshIndex : fx->frameNumber);

			CollectLightsForEffect(fx->roomNumber, newEffect, renderView);

			room.EffectsToDraw.push_back(newEffect);
		}
	}

	void Renderer11::ResetAnimations()
	{
		for (int i = 0; i < NUM_ITEMS; i++)
			m_items[i].DoneAnimations = false;
	}
} // namespace TEN::Renderer
