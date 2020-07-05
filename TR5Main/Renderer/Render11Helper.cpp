#include "framework.h"
#include "Renderer11.h"
#include "configuration.h"
#include "camera.h"
#include "draw.h"
#include "setup.h"
#include "level.h"
#include "control.h"
#include "lara.h"
#include "sphere.h"
#include "GameFlowScript.h"
#include <Renderer\RenderView\RenderView.h>
extern GameConfiguration g_Configuration;
extern GameFlow *g_GameFlow;
namespace T5M::Renderer
{
	using std::pair;
	using std::vector;

	bool Renderer11::isRoomUnderwater(short roomNumber)
	{
		return (m_rooms[roomNumber].Room->flags & ENV_FLAG_WATER);
	}

	bool Renderer11::isInRoom(int x, int y, int z, short roomNumber)
	{
		RendererRoom &const room = m_rooms[roomNumber];
		ROOM_INFO *r = room.Room;

		return (x >= r->x && x <= r->x + r->xSize * 1024.0f &&
				y >= r->maxceiling && y <= r->minfloor &&
				z >= r->z && z <= r->z + r->ySize * 1024.0f);
	}

	vector<RendererVideoAdapter> *Renderer11::GetAdapters()
	{
		return &m_adapters;
	}

	void Renderer11::createBillboardMatrix(Matrix *out, Vector3 *particlePos, Vector3 *cameraPos, float rotation)
	{
		/*
		Vector3 look = *particlePos;
		look = look - *cameraPos;
		look.Normalize();

		*out = Matrix::CreateBillboard(*particlePos, *cameraPos, cameraUp);

		Vector3 right;
		right = cameraUp.Cross(look);
		right.Normalize();

		// Rotate right vector
		Matrix rightTransform = Matrix::CreateFromAxisAngle(look, rotation);
		right = Vector3::Transform(right, rightTransform);

		Vector3 up;
		up = look.Cross(right);
		up.Normalize();

		*out = Matrix::Identity;

		out->_11 = right.x;
		out->_12 = right.y;
		out->_13 = right.z;

		out->_21 = up.x;
		out->_22 = up.y;
		out->_23 = up.z;

		out->_31 = look.x;
		out->_32 = look.y;
		out->_33 = look.z;

		out->_41 = particlePos->x;
		out->_42 = particlePos->y;
		out->_43 = particlePos->z;
		*/
	}

	void Renderer11::updateAnimatedTextures()
	{
		// Update room's animated textures
		for (int i = 0; i < Rooms.size(); i++)
		{
			if (m_rooms.size() <= i)
				continue;
			RendererRoom &const room = m_rooms[i];

			for (int bucketIndex = 0; bucketIndex < NUM_BUCKETS; bucketIndex++)
			{
				RendererBucket *bucket = &room.AnimatedBuckets[bucketIndex];

				if (bucket->Vertices.size() == 0)
					continue;

				for (int p = 0; p < bucket->Polygons.size(); p++)
				{
					RendererPolygon *polygon = &bucket->Polygons[p];
					RendererAnimatedTextureSet &const set = m_animatedTextureSets[polygon->AnimatedSet];
					int textureIndex = -1;
					for (int j = 0; j < set.NumTextures; j++)
					{
						if (set.Textures[j].Id == polygon->TextureId)
						{
							textureIndex = j;
							break;
						}
					}
					if (textureIndex == -1)
						continue;

					if (textureIndex == set.NumTextures - 1)
						textureIndex = 0;
					else
						textureIndex++;

					polygon->TextureId = set.Textures[textureIndex].Id;

					for (int v = 0; v < (polygon->Shape == SHAPE_RECTANGLE ? 4 : 3); v++)
					{
						bucket->Vertices[polygon->Indices[v]].UV.x = set.Textures[textureIndex].UV[v].x;
						bucket->Vertices[polygon->Indices[v]].UV.y = set.Textures[textureIndex].UV[v].y;
					}
				}
			}
		}

		// Update waterfalls textures
		/*for (int i = ID_WATERFALL1; i <= ID_WATERFALLSS2; i++) {
			ObjectInfo* obj = &Objects[i];

			if (obj->loaded) {
				RendererObject* waterfall = m_moveableObjects[i];

				for (int m = 0; m < waterfall->ObjectMeshes.size(); m++) {
					RendererMesh* mesh = waterfall->ObjectMeshes[m];
					RendererBucket* bucket = &mesh->Buckets[RENDERER_BUCKET_TRANSPARENT_DS];

					for (int v = 0; v < bucket->Vertices.size(); v++) {
						RendererVertex* vertex = &bucket->Vertices[v];
						int y = vertex->UV.y * TEXTURE_ATLAS_SIZE + 64;
						y %= 128;
						vertex->UV.y = (float)y / TEXTURE_ATLAS_SIZE;
					}
				}
			}
		}*/
	}

	void Renderer11::updateEffects()
	{
		for (int i = 0; i < m_effectsToDraw.size(); i++)
		{
			RendererEffect *fx = m_effectsToDraw[i];

			Matrix translation = Matrix::CreateTranslation(fx->Effect->pos.xPos, fx->Effect->pos.yPos, fx->Effect->pos.zPos);
			Matrix rotation = Matrix::CreateFromYawPitchRoll(TO_RAD(fx->Effect->pos.yRot), TO_RAD(fx->Effect->pos.xRot), TO_RAD(fx->Effect->pos.zRot));
			m_effectsToDraw[i]->World = rotation * translation;
		}
	}

	void Renderer11::updateAnimation(RendererItem *item, RendererObject& obj, short **frmptr, short frac, short rate, int mask, bool useObjectWorldRotation)
	{
		RendererBone *Bones[32];
		int nextBone = 0;

		Matrix rotation;

		Matrix *transforms = (item == NULL ? obj.AnimationTransforms.data() : &item->AnimationTransforms[0]);

		// Push
		Bones[nextBone++] = obj.Skeleton;

		while (nextBone != 0)
		{
			// Pop the last bone in the stack
			RendererBone *bone = Bones[--nextBone];

			bool calculateMatrix = (mask >> bone->Index) & 1;

			if (calculateMatrix)
			{
				Vector3 p = Vector3((int)*(frmptr[0] + 6), (int)*(frmptr[0] + 7), (int)*(frmptr[0] + 8));

				fromTrAngle(&rotation, frmptr[0], bone->Index);

				if (frac)
				{
					Vector3 p2 = Vector3((int)*(frmptr[1] + 6), (int)*(frmptr[1] + 7), (int)*(frmptr[1] + 8));
					p = Vector3::Lerp(p, p2, frac / ((float)rate));

					Matrix rotation2;
					fromTrAngle(&rotation2, frmptr[1], bone->Index);

					Quaternion q1, q2, q3;

					q1 = Quaternion::CreateFromRotationMatrix(rotation);
					q2 = Quaternion::CreateFromRotationMatrix(rotation2);
					q3 = Quaternion::Slerp(q1, q2, frac / ((float)rate));

					rotation = Matrix::CreateFromQuaternion(q3);
				}

				Matrix translation;
				if (bone == obj.Skeleton)
					translation = Matrix::CreateTranslation(p.x, p.y, p.z);

				Matrix extraRotation;
				extraRotation = Matrix::CreateFromYawPitchRoll(bone->ExtraRotation.y, bone->ExtraRotation.x, bone->ExtraRotation.z);
				if (useObjectWorldRotation)
				{
					Quaternion invertedQuat;
					transforms[bone->Parent->Index].Invert().Decompose(Vector3(), invertedQuat, Vector3());
					rotation = extraRotation * rotation * Matrix::CreateFromQuaternion(invertedQuat);
				}
				else
				{
					rotation = extraRotation * rotation;
				}

				if (bone != obj.Skeleton)
					transforms[bone->Index] = rotation * bone->Transform;
				else
					transforms[bone->Index] = rotation * translation;

				if (bone != obj.Skeleton)

					transforms[bone->Index] = transforms[bone->Index] * transforms[bone->Parent->Index];
			}

			for (int i = 0; i < bone->Children.size(); i++)
			{
				// Push
				Bones[nextBone++] = bone->Children[i];
			}
		}
	}
	int Renderer11::getFrame(short animation, short frame, short **framePtr, int *rate)
	{
		ITEM_INFO item;
		item.animNumber = animation;
		item.frameNumber = frame;

		return GetFrame_D2(&item, framePtr, rate);
	}

	void Renderer11::UpdateItemAnimations(int itemNumber, bool force)
	{
		RendererItem *itemToDraw = &m_items[itemNumber];
		itemToDraw->Id = itemNumber;
		itemToDraw->Item = &Items[itemNumber];

		ITEM_INFO *item = itemToDraw->Item;
		CREATURE_INFO *creature = (CREATURE_INFO *)item->data;

		// Lara has her own routine
		if (item->objectNumber == ID_LARA)
			return;

		// Has been already done?
		if (!force && itemToDraw->DoneAnimations)
			return;

		ObjectInfo *obj = &Objects[item->objectNumber];
		RendererObject &moveableObj = *m_moveableObjects[item->objectNumber];

		// Update animation matrices
		if (obj->animIndex != -1 /*&& item->objectNumber != ID_HARPOON*/)
		{
			// Apply extra rotations
			int lastJoint = 0;
			for (int j = 0; j < moveableObj.LinearizedBones.size(); j++)
			{
				RendererBone *currentBone = moveableObj.LinearizedBones[j];
				currentBone->ExtraRotation = Vector3(0.0f, 0.0f, 0.0f);

				if (creature)
				{
					if (currentBone->ExtraRotationFlags & ROT_Y)
					{
						currentBone->ExtraRotation.y = TO_RAD(creature->jointRotation[lastJoint]);
						lastJoint++;
					}

					if (currentBone->ExtraRotationFlags & ROT_X)
					{
						currentBone->ExtraRotation.x = TO_RAD(creature->jointRotation[lastJoint]);
						lastJoint++;
					}

					if (currentBone->ExtraRotationFlags & ROT_Z)
					{
						currentBone->ExtraRotation.z = TO_RAD(creature->jointRotation[lastJoint]);
						lastJoint++;
					}
				}
			}

			short *framePtr[2];
			int rate;
			int frac = GetFrame_D2(item, framePtr, &rate);

			updateAnimation(itemToDraw, moveableObj, framePtr, frac, rate, 0xFFFFFFFF);

			for (int m = 0; m < itemToDraw->NumMeshes; m++)
				itemToDraw->AnimationTransforms[m] = itemToDraw->AnimationTransforms[m];
		}

		itemToDraw->DoneAnimations = true;
	}

	void Renderer11::updateItemsAnimations()
	{
		Matrix translation;
		Matrix rotation;

		int numItems = m_itemsToDraw.size();

		for (int i = 0; i < numItems; i++)
		{
			RendererItem *itemToDraw = m_itemsToDraw[i];
			ITEM_INFO *item = itemToDraw->Item;
			CREATURE_INFO *creature = (CREATURE_INFO *)item->data;

			// Lara has her own routine
			if (item->objectNumber == ID_LARA)
				continue;

			UpdateItemAnimations(itemToDraw->Id, false);
		}
	}

	void Renderer11::fromTrAngle(Matrix *matrix, short *frameptr, int index)
	{
		short *ptr = &frameptr[0];

		ptr += 9;
		for (int i = 0; i < index; i++)
		{
			ptr += ((*ptr & 0xc000) == 0 ? 2 : 1);
		}

		int rot0 = *ptr++;
		int frameMode = (rot0 & 0xc000);

		int rot1;
		int rotX;
		int rotY;
		int rotZ;

		switch (frameMode)
		{
		case 0:
			rot1 = *ptr++;
			rotX = ((rot0 & 0x3ff0) >> 4);
			rotY = (((rot1 & 0xfc00) >> 10) | ((rot0 & 0xf) << 6) & 0x3ff);
			rotZ = ((rot1)&0x3ff);

			*matrix = Matrix::CreateFromYawPitchRoll(rotY * (360.0f / 1024.0f) * RADIAN,
													 rotX * (360.0f / 1024.0f) * RADIAN,
													 rotZ * (360.0f / 1024.0f) * RADIAN);
			break;

		case 0x4000:
			*matrix = Matrix::CreateRotationX((rot0 & 0xfff) * (360.0f / 4096.0f) * RADIAN);
			break;

		case 0x8000:
			*matrix = Matrix::CreateRotationY((rot0 & 0xfff) * (360.0f / 4096.0f) * RADIAN);
			break;

		case 0xc000:
			*matrix = Matrix::CreateRotationZ((rot0 & 0xfff) * (360.0f / 4096.0f) * RADIAN);
			break;
		}
	}

	void Renderer11::buildHierarchyRecursive(RendererObject *obj, RendererBone *node, RendererBone *parentNode)
	{
		node->GlobalTransform = node->Transform * parentNode->GlobalTransform;
		obj->BindPoseTransforms[node->Index] = node->GlobalTransform;
		obj->Skeleton->GlobalTranslation = Vector3(0.0f, 0.0f, 0.0f);
		node->GlobalTranslation = node->Translation + parentNode->GlobalTranslation;

		for (int j = 0; j < node->Children.size(); j++)
		{
			buildHierarchyRecursive(obj, node->Children[j], node);
		}
	}

	void Renderer11::buildHierarchy(RendererObject *obj)
	{
		obj->Skeleton->GlobalTransform = obj->Skeleton->Transform;
		obj->BindPoseTransforms[obj->Skeleton->Index] = obj->Skeleton->GlobalTransform;
		obj->Skeleton->GlobalTranslation = Vector3(0.0f, 0.0f, 0.0f);

		for (int j = 0; j < obj->Skeleton->Children.size(); j++)
		{
			buildHierarchyRecursive(obj, obj->Skeleton->Children[j], obj->Skeleton);
		}
	}

	RendererMesh *Renderer11::getRendererMeshFromTrMesh(RendererObject *obj, MESH *meshPtr, short boneIndex, int isJoints, int isHairs)
	{
		RendererMesh *mesh = new RendererMesh();

		mesh->Sphere = meshPtr->sphere;

		if (meshPtr->positions.size() == 0)
			return mesh;

		mesh->Positions.reserve(meshPtr->positions.size());
		for (int i = 0; i < meshPtr->positions.size(); i++)
			mesh->Positions.push_back(meshPtr->positions[i]);

		for (int n = 0; n < meshPtr->buckets.size(); n++)
		{
			BUCKET *levelBucket = &meshPtr->buckets[n];
			RendererBucket *bucket;
			int bucketIndex;

			if (levelBucket->blendMode != 0)
				bucketIndex = RENDERER_BUCKET_TRANSPARENT;
			else
				bucketIndex = RENDERER_BUCKET_SOLID;

			bucket = &mesh->Buckets[bucketIndex];
			
			bucket->Vertices.reserve(levelBucket->numQuads * 4 + levelBucket->numTriangles * 3);
			bucket->Indices.reserve(levelBucket->numQuads * 6 + levelBucket->numTriangles * 3);

			for (int p = 0; p < levelBucket->polygons.size(); p++)
			{
				POLYGON* poly = &levelBucket->polygons[p];

				int baseVertices = bucket->Vertices.size();

				for (int k = 0; k < poly->indices.size(); k++)
				{
					RendererVertex vertex;
					int v = poly->indices[k];

					vertex.Position.x = meshPtr->positions[v].x;
					vertex.Position.y = meshPtr->positions[v].y;
					vertex.Position.z = meshPtr->positions[v].z;

					vertex.Normal.x = meshPtr->normals[v].x;
					vertex.Normal.y = meshPtr->normals[v].y;
					vertex.Normal.z = meshPtr->normals[v].z;

					vertex.UV.x = poly->textureCoordinates[k].x;
					vertex.UV.y = poly->textureCoordinates[k].y;

					vertex.Color.x = meshPtr->colors[v].x;
					vertex.Color.y = meshPtr->colors[v].y;
					vertex.Color.z = meshPtr->colors[v].z;
					vertex.Color.w = 1.0f;

					vertex.Bone = meshPtr->bones[v];
					//vertex.Bone = boneIndex;
					if (isHairs)
						vertex.Bone = v;

					bucket->Vertices.push_back(vertex);
				}

				if (poly->shape == 0)
				{
					bucket->Indices.push_back(baseVertices);
					bucket->Indices.push_back(baseVertices + 1);
					bucket->Indices.push_back(baseVertices + 3);
					bucket->Indices.push_back(baseVertices + 2);
					bucket->Indices.push_back(baseVertices + 3);
					bucket->Indices.push_back(baseVertices + 1);
				}
				else
				{
					bucket->Indices.push_back(baseVertices);
					bucket->Indices.push_back(baseVertices + 1);
					bucket->Indices.push_back(baseVertices + 2);
				}
			}
		}

		m_meshes.push_back(mesh);

		return mesh;
	}

	int Renderer11::getAnimatedTextureInfo(short textureId)
	{
		for (int i = 0; i < m_numAnimatedTextureSets; i++)
		{
			RendererAnimatedTextureSet &const set = m_animatedTextureSets[i];
			for (int j = 0; j < set.NumTextures; j++)
			{
				if (set.Textures[j].Id == textureId)
					return i;
			}
		}

		return -1;
	}
	bool Renderer11::IsFullsScreen()
	{
		return (!Windowed);
	}
	bool Renderer11::IsFading()
	{
		return false;
		return (m_fadeStatus != FADEMODE_NONE);
	}

	void Renderer11::UpdateCameraMatrices(CAMERA_INFO *cam, float roll, float fov)
	{
		gameCamera = RenderView(cam, roll, fov, 32, 102400, g_Configuration.Width, g_Configuration.Height);
	}

	bool Renderer11::EnumerateVideoModes()
	{
		HRESULT res;

		IDXGIFactory *dxgiFactory = NULL;
		res = CreateDXGIFactory(__uuidof(IDXGIFactory), (void **)&dxgiFactory);
		if (FAILED(res))
			return false;

		IDXGIAdapter *dxgiAdapter = NULL;

		for (int i = 0; dxgiFactory->EnumAdapters(i, &dxgiAdapter) != DXGI_ERROR_NOT_FOUND; i++)
		{
			DXGI_ADAPTER_DESC adapterDesc;
			UINT stringLength;
			char videoCardDescription[128];

			dxgiAdapter->GetDesc(&adapterDesc);
			int error = wcstombs_s(&stringLength, videoCardDescription, 128, adapterDesc.Description, 128);

			RendererVideoAdapter adapter;

			adapter.Index = i;
			adapter.Name = videoCardDescription;

			printf("Adapter %d\n", i);
			printf("\t Device Name: %s\n", videoCardDescription);

			IDXGIOutput *output = NULL;
			res = dxgiAdapter->EnumOutputs(0, &output);
			if (FAILED(res))
				return false;

			UINT numModes = 0;
			DXGI_MODE_DESC *displayModes = NULL;
			DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM;

			// Get the number of elements
			res = output->GetDisplayModeList(format, 0, &numModes, NULL);
			if (FAILED(res))
				return false;

			// Get the list
			displayModes = new DXGI_MODE_DESC[numModes];
			res = output->GetDisplayModeList(format, 0, &numModes, displayModes);
			if (FAILED(res))
			{
				delete displayModes;
				return false;
			}

			for (int j = 0; j < numModes; j++)
			{
				DXGI_MODE_DESC *mode = &displayModes[j];

				RendererDisplayMode newMode;

				// discard lower resolutions
				if (mode->Width < 1024 || mode->Height < 768)
					continue;

				newMode.Width = mode->Width;
				newMode.Height = mode->Height;
				newMode.RefreshRate = mode->RefreshRate.Numerator / mode->RefreshRate.Denominator;

				bool found = false;
				for (int k = 0; k < adapter.DisplayModes.size(); k++)
				{
					RendererDisplayMode *currentMode = &adapter.DisplayModes[k];
					if (currentMode->Width == newMode.Width && currentMode->Height == newMode.Height &&
						currentMode->RefreshRate == newMode.RefreshRate)
					{
						found = true;
						break;
					}
				}
				if (found)
					continue;

				adapter.DisplayModes.push_back(newMode);
				printf("\t\t %d x %d %d Hz\n", newMode.Width, newMode.Height, newMode.RefreshRate);
			}

			m_adapters.push_back(adapter);

			delete displayModes;
		}

		dxgiFactory->Release();

		return true;
	}

	int SortLightsFunction(RendererLight *a, RendererLight *b)
	{
		if (a->Dynamic > b->Dynamic)
			return -1;
		return 0;
	}

	bool SortRoomsFunction(RendererRoom *a, RendererRoom *b)
	{
		return (a->Distance < b->Distance);
	}

	int SortRoomsFunctionNonStd(RendererRoom *a, RendererRoom *b)
	{
		return (a->Distance - b->Distance);
	}

	void Renderer11::getVisibleObjects(int from, int to, Vector4 *viewPort, bool water, int count, RenderView &renderView)
	{
		// Avoid allocations, 1024 should be fine
		RendererRoomNode nodes[256];
		int nextNode = 0;

		// Avoid reallocations, 1024 should be fine
		RendererRoomNode *stack[256];
		int stackDepth = 0;

		RendererRoomNode *node = &nodes[nextNode++];
		node->To = to;
		node->From = -1;

		// Push
		stack[stackDepth++] = node;

		while (stackDepth > 0)
		{
			// Pop
			node = stack[--stackDepth];

			if (m_rooms[node->To].Visited)
				continue;

			ROOM_INFO *room = &Rooms[node->To];

			Vector3 roomCentre = Vector3(room->x + room->xSize * WALL_SIZE / 2.0f,
										 (room->minfloor + room->maxceiling) / 2.0f,
										 room->z + room->ySize * WALL_SIZE / 2.0f);
			Vector3 laraPosition = Vector3(Camera.pos.x, Camera.pos.y, Camera.pos.z);

			m_rooms[node->To].Distance = (roomCentre - laraPosition).Length();
			m_rooms[node->To].Visited = true;
			renderView.roomsToDraw.push_back(&m_rooms[node->To]);
			Rooms[node->To].boundActive = true;

			collectLightsForRoom(node->To, renderView);
			collectItems(node->To, renderView);
			collectStatics(node->To, renderView);
			collectEffects(node->To, renderView);

			Vector4 clipPort;

			for (int i = 0; i < room->doors.size(); i++)
			{
				short adjoiningRoom = room->doors[i].room;

				if (node->From != adjoiningRoom && checkPortal(node->To, &room->doors[i], viewPort, &node->ClipPort, renderView.camera.ViewProjection))
				{
					RendererRoomNode *childNode = &nodes[nextNode++];
					childNode->From = node->To;
					childNode->To = adjoiningRoom;

					// Push
					stack[stackDepth++] = childNode;
				}
			}
		}
	}

	bool Renderer11::checkPortal(short roomIndex, ROOM_DOOR *portal, Vector4 *viewPort, Vector4 *clipPort, const Matrix &viewProjection)
	{
		ROOM_INFO *room = &Rooms[roomIndex];

		Vector3 n = portal->normal;
		Vector3 v = Vector3(
			Camera.pos.x - (room->x + portal->vertices[0].x),
			Camera.pos.y - (room->y + portal->vertices[0].y),
			Camera.pos.z - (room->z + portal->vertices[0].z));

		// Test camera and normal positions and decide if process door or not
		if (n.Dot(v) <= 0.0f)
			return false;

		int zClip = 0;
		Vector4 p[4];

		clipPort->x = FLT_MAX;
		clipPort->y = FLT_MAX;
		clipPort->z = FLT_MIN;
		clipPort->w = FLT_MIN;

		// Project all portal's corners in screen space
		for (int i = 0; i < 4; i++)
		{
			Vector4 tmp = Vector4(portal->vertices[i].x + room->x, portal->vertices[i].y + room->y, portal->vertices[i].z + room->z, 1.0f);

			// Project corner on screen
			Vector4::Transform(tmp, viewProjection, p[i]);

			if (p[i].w > 0.0f)
			{
				// The corner is in front of camera
				p[i].x *= (1.0f / p[i].w);
				p[i].y *= (1.0f / p[i].w);
				p[i].z *= (1.0f / p[i].w);

				clipPort->x = min(clipPort->x, p[i].x);
				clipPort->y = min(clipPort->y, p[i].y);
				clipPort->z = max(clipPort->z, p[i].x);
				clipPort->w = max(clipPort->w, p[i].y);
			}
			else
				// The corner is behind camera
				zClip++;
		}

		// If all points are behind camera then door is not visible
		if (zClip == 4)
			return false;

		// If door crosses camera plane...
		if (zClip > 0)
		{
			for (int i = 0; i < 4; i++)
			{
				Vector4 a = p[i];
				Vector4 b = p[(i + 1) % 4];

				if ((a.w > 0.0f) ^ (b.w > 0.0f))
				{

					if (a.x < 0.0f && b.x < 0.0f)
						clipPort->x = -1.0f;
					else if (a.x > 0.0f && b.x > 0.0f)
						clipPort->z = 1.0f;
					else
					{
						clipPort->x = -1.0f;
						clipPort->z = 1.0f;
					}

					if (a.y < 0.0f && b.y < 0.0f)
						clipPort->y = -1.0f;
					else if (a.y > 0.0f && b.y > 0.0f)
						clipPort->w = 1.0f;
					else
					{
						clipPort->y = -1.0f;
						clipPort->w = 1.0f;
					}
				}
			}
		}

		clipPort->x = max(clipPort->x, viewPort->x);
		clipPort->y = max(clipPort->y, viewPort->y);
		clipPort->z = min(clipPort->z, viewPort->z);
		clipPort->w = min(clipPort->w, viewPort->w);

		if (clipPort->x > viewPort->z || clipPort->y > viewPort->w || clipPort->z < viewPort->x || clipPort->w < viewPort->y)
			return false;

		return true;
	}

	bool Renderer11::sphereBoxIntersection(Vector3 boxMin, Vector3 boxMax, Vector3 sphereCentre, float sphereRadius)
	{
		Vector3 centre = (boxMin + boxMax) / 2.0f;
		Vector3 extens = boxMax - centre;
		BoundingBox box = BoundingBox(centre, extens);
		BoundingSphere sphere = BoundingSphere(sphereCentre, sphereRadius);
		return box.Intersects(sphere);
	}

	void Renderer11::GetLaraBonePosition(Vector3 *pos, int bone) {}

	void Renderer11::FlipRooms(short roomNumber1, short roomNumber2)
	{
		RendererRoom temporary;

		temporary = m_rooms[roomNumber1];
		m_rooms[roomNumber1] = m_rooms[roomNumber2];
		m_rooms[roomNumber2] = temporary;
		m_rooms[roomNumber1].Room = &Rooms[roomNumber1];
		m_rooms[roomNumber2].Room = &Rooms[roomNumber2];
	}

	RendererMesh *Renderer11::getMesh(int meshIndex)
	{
		return m_meshes[meshIndex];
	}

	void Renderer11::GetLaraAbsBonePosition(Vector3 *pos, int joint)
	{
		Matrix world = m_moveableObjects[ID_LARA]->AnimationTransforms[joint];
		world = world * m_LaraWorldMatrix;
		*pos = Vector3::Transform(*pos, world);
	}

	void Renderer11::GetItemAbsBonePosition(int itemNumber, Vector3 *pos, int joint)
	{
		RendererItem *rendererItem = &m_items[itemNumber];
		rendererItem->Id = itemNumber;
		rendererItem->Item = &Items[itemNumber];
		ITEM_INFO *item = rendererItem->Item;

		if (!item)
			return;

		if (!rendererItem->DoneAnimations)
		{
			if (itemNumber == Lara.itemNumber)
				UpdateLaraAnimations(false);
			else
				UpdateItemAnimations(itemNumber, false);
		}

		Matrix world = rendererItem->AnimationTransforms[joint] * rendererItem->World;
		*pos = Vector3::Transform(*pos, world);
	}

	int Renderer11::GetSpheres(short itemNumber, BoundingSphere *spheres, char worldSpace, Matrix local)
	{
		RendererItem *rendererItem = &m_items[itemNumber];
		rendererItem->Id = itemNumber;
		rendererItem->Item = &Items[itemNumber];
		ITEM_INFO *item = rendererItem->Item;

		if (!item)
			return 0;

		if (!rendererItem->DoneAnimations)
		{
			if (itemNumber == Lara.itemNumber)
				UpdateLaraAnimations(false);
			else
				UpdateItemAnimations(itemNumber, false);
		}

		int x, y, z;
		Matrix world;

		if (worldSpace & SPHERES_SPACE_WORLD)
		{
			x = item->pos.xPos;
			y = item->pos.yPos;
			z = item->pos.zPos;
			world = Matrix::Identity;
		}
		else
		{
			x = 0;
			y = 0;
			z = 0;
			world = Matrix::CreateTranslation(item->pos.xPos, item->pos.yPos, item->pos.zPos) * local;
		}

		world = Matrix::CreateFromYawPitchRoll(TO_RAD(item->pos.yRot), TO_RAD(item->pos.xRot), TO_RAD(item->pos.zRot)) * world;

		RendererObject &moveable = *m_moveableObjects[item->objectNumber];

		for (int i = 0; i < moveable.ObjectMeshes.size(); i++)
		{
			RendererMesh *mesh = moveable.ObjectMeshes[i];

			Vector3 pos;
			if (worldSpace & SPHERES_SPACE_BONE_ORIGIN)
				pos = Vector3::Zero;
			else
				pos = mesh->Sphere.Center;

			spheres[i].Center = Vector3(x, y, z) + Vector3::Transform(pos, (rendererItem->AnimationTransforms[i] * world));
			spheres[i].Radius = mesh->Sphere.Radius;
		}

		return moveable.ObjectMeshes.size();
	}

	void Renderer11::GetBoneMatrix(short itemNumber, int joint, Matrix *outMatrix)
	{
		RendererObject &obj = *m_moveableObjects[ID_LARA];
		*outMatrix = obj.AnimationTransforms[joint] * m_LaraWorldMatrix;
	}
} // namespace T5M::Renderer
