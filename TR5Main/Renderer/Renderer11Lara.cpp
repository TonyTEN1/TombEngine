#include "Renderer11.h"
#include "../Game/draw.h"
#include "../Game/hair.h"
#include "../Game/lara.h"
#include "../Game/control.h"
#include "../Game/spotcam.h"
#include "../game/camera.h"
#include "../Global/global.h"
#include "..\Specific\level.h"

extern GameFlow* g_GameFlow;

void Renderer11::UpdateLaraAnimations()
{
	Matrix translation;
	Matrix rotation;
	Matrix lastMatrix;
	Matrix hairMatrix;
	Matrix identity;
	Matrix world;

	RendererObject* laraObj = m_moveableObjects[ID_LARA];

	// Clear extra rotations
	for (int i = 0; i < laraObj->LinearizedBones.size(); i++)
		laraObj->LinearizedBones[i]->ExtraRotation = Vector3(0.0f, 0.0f, 0.0f);

	// Lara world matrix
	translation = Matrix::CreateTranslation(LaraItem->pos.xPos, LaraItem->pos.yPos, LaraItem->pos.zPos);
	rotation = Matrix::CreateFromYawPitchRoll(TO_RAD(LaraItem->pos.yRot), TO_RAD(LaraItem->pos.xRot), TO_RAD(LaraItem->pos.zRot));

	m_LaraWorldMatrix = rotation * translation;

	// Update first Lara's animations
	laraObj->LinearizedBones[LM_TORSO]->ExtraRotation = Vector3(TO_RAD(Lara.torsoXrot), TO_RAD(Lara.torsoYrot), TO_RAD(Lara.torsoZrot));
	laraObj->LinearizedBones[LM_HEAD]->ExtraRotation = Vector3(TO_RAD(Lara.headXrot), TO_RAD(Lara.headYrot), TO_RAD(Lara.headZrot));

	// First calculate matrices for legs, hips, head and torso
	int mask = MESH_BITS(LM_HIPS) | MESH_BITS(LM_LTHIGH) | MESH_BITS(LM_LSHIN) | MESH_BITS(LM_LFOOT) | MESH_BITS(LM_RTHIGH) | MESH_BITS(LM_RSHIN) | MESH_BITS(LM_RFOOT) | MESH_BITS(LM_TORSO) | MESH_BITS(LM_HEAD);
	short* framePtr[2];
	int rate, frac;

	frac = GetFrame_D2(LaraItem, framePtr, &rate);
	updateAnimation(NULL, laraObj, framePtr, frac, rate, mask);

	// Then the arms, based on current weapon status
	if (Lara.gunType != WEAPON_FLARE && (Lara.gunStatus == LG_NO_ARMS || Lara.gunStatus == LG_HANDS_BUSY) || Lara.gunType == WEAPON_FLARE && !Lara.flareControlLeft)
	{
		// Both arms
		mask = MESH_BITS(LM_LINARM) | MESH_BITS(LM_LOUTARM) | MESH_BITS(LM_LHAND) | MESH_BITS(LM_RINARM) | MESH_BITS(LM_ROUTARM) | MESH_BITS(LM_RHAND);
		frac = GetFrame_D2(LaraItem, framePtr, &rate);
		updateAnimation(NULL, laraObj, framePtr, frac, rate, mask);
	}
	else
	{
		// While handling weapon some extra rotation could be applied to arms
		laraObj->LinearizedBones[LM_LINARM]->ExtraRotation += Vector3(TO_RAD(Lara.leftArm.xRot), TO_RAD(0), TO_RAD(-Lara.leftArm.yRot));
		laraObj->LinearizedBones[LM_RINARM]->ExtraRotation += Vector3(TO_RAD(Lara.rightArm.xRot), TO_RAD(0), TO_RAD(-Lara.rightArm.yRot));

		LARA_ARM * leftArm = &Lara.leftArm;
		LARA_ARM * rightArm = &Lara.rightArm;

		// HACK: backguns handles differently // TokyoSU: not really a hack since it's the original way to do that.
		switch (Lara.gunType)
		{
		case WEAPON_SHOTGUN:
		case WEAPON_HK:
		case WEAPON_CROSSBOW:
		case WEAPON_GRENADE_LAUNCHER:
		case WEAPON_HARPOON_GUN:
			short* shotgunFramePtr;

			// Left arm
			mask = MESH_BITS(LM_LINARM) | MESH_BITS(LM_LOUTARM) | MESH_BITS(LM_LHAND);
			shotgunFramePtr = Lara.leftArm.frameBase + (Lara.leftArm.frameNumber) * (Anims[Lara.leftArm.animNumber].interpolation >> 8);
			updateAnimation(NULL, laraObj, &shotgunFramePtr, 0, 1, mask);

			// Right arm
			mask = MESH_BITS(LM_RINARM) | MESH_BITS(LM_ROUTARM) | MESH_BITS(LM_RHAND);
			shotgunFramePtr = Lara.rightArm.frameBase + (Lara.rightArm.frameNumber) * (Anims[Lara.rightArm.animNumber].interpolation >> 8);
			updateAnimation(NULL, laraObj, &shotgunFramePtr, 0, 1, mask);
			break;

		case WEAPON_PISTOLS:
		case WEAPON_UZI:
		case WEAPON_REVOLVER:
		default:
		{
			short* pistolFramePtr;

			// Left arm
			int upperArmMask = MESH_BITS(LM_LINARM);
			mask = MESH_BITS(LM_LOUTARM) | MESH_BITS(LM_LHAND);
			pistolFramePtr = Lara.leftArm.frameBase + (Lara.leftArm.frameNumber - Anims[Lara.leftArm.animNumber].frameBase) * (Anims[Lara.leftArm.animNumber].interpolation >> 8);
			updateAnimation(NULL, laraObj, &pistolFramePtr, 0, 1, upperArmMask, true);
			updateAnimation(NULL, laraObj, &pistolFramePtr, 0, 1, mask);

			// Right arm
			upperArmMask = MESH_BITS(LM_RINARM);
			mask = MESH_BITS(LM_ROUTARM) | MESH_BITS(LM_RHAND);
			pistolFramePtr = Lara.rightArm.frameBase + (Lara.rightArm.frameNumber - Anims[Lara.rightArm.animNumber].frameBase) * (Anims[Lara.rightArm.animNumber].interpolation >> 8);
			updateAnimation(NULL, laraObj, &pistolFramePtr, 0, 1, upperArmMask, true);
			updateAnimation(NULL, laraObj, &pistolFramePtr, 0, 1, mask);
		}

		break;

		case WEAPON_FLARE:
		case WEAPON_TORCH:
			// Left arm
			mask = MESH_BITS(LM_LINARM) | MESH_BITS(LM_LOUTARM) | MESH_BITS(LM_LHAND);
			frac = getFrame(Lara.leftArm.animNumber, Lara.leftArm.frameNumber, framePtr, &rate);
			updateAnimation(NULL, laraObj, framePtr, frac, rate, mask);

			// Right arm
			mask = MESH_BITS(LM_RINARM) | MESH_BITS(LM_ROUTARM) | MESH_BITS(LM_RHAND);
			frac = GetFrame_D2(LaraItem, framePtr, &rate);
			updateAnimation(NULL, laraObj, framePtr, frac, rate, mask);
			break;
		}
	}

	// At this point, Lara's matrices are ready. Now let's do ponytails...
	if (m_moveableObjects[ID_LARA_HAIR] != NULL)
	{
		RendererObject* hairsObj = m_moveableObjects[ID_LARA_HAIR];

		lastMatrix = Matrix::Identity;
		identity = Matrix::Identity;

		Vector3 parentVertices[6][4];
		Matrix headMatrix;

		RendererObject* objSkin = m_moveableObjects[ID_LARA_SKIN];
		RendererObject* objLara = m_moveableObjects[ID_LARA];
		RendererMesh* parentMesh = objSkin->ObjectMeshes[LM_HEAD];
		RendererBone* parentBone = objSkin->LinearizedBones[LM_HEAD];

		world = objLara->AnimationTransforms[LM_HEAD] * m_LaraWorldMatrix;

		int lastVertex = 0;
		int lastIndex = 0;

		GameScriptLevel* level = g_GameFlow->GetLevel(CurrentLevel);

		for (int p = 0; p < ((level->LaraType == LARA_YOUNG) ? 2 : 1); p++)
		{
			// We can't use hardware skinning here, however hairs have just a few vertices so 
			// it's not so bad doing skinning in software
			if (level->LaraType == LARA_YOUNG)
			{
				if (p == 1)
				{
					parentVertices[0][0] = Vector3::Transform(parentMesh->Positions[68], world);
					parentVertices[0][1] = Vector3::Transform(parentMesh->Positions[69], world);
					parentVertices[0][2] = Vector3::Transform(parentMesh->Positions[70], world);
					parentVertices[0][3] = Vector3::Transform(parentMesh->Positions[71], world);
				}
				else
				{
					parentVertices[0][0] = Vector3::Transform(parentMesh->Positions[78], world);
					parentVertices[0][1] = Vector3::Transform(parentMesh->Positions[78], world);
					parentVertices[0][2] = Vector3::Transform(parentMesh->Positions[77], world);
					parentVertices[0][3] = Vector3::Transform(parentMesh->Positions[76], world);
				}
			}
			else
			{
				parentVertices[0][0] = Vector3::Transform(parentMesh->Positions[37], world);
				parentVertices[0][1] = Vector3::Transform(parentMesh->Positions[39], world);
				parentVertices[0][2] = Vector3::Transform(parentMesh->Positions[40], world);
				parentVertices[0][3] = Vector3::Transform(parentMesh->Positions[38], world);
			}

			for (int i = 0; i < 6; i++)
			{
				RendererMesh* mesh = hairsObj->ObjectMeshes[i];
				RendererBucket* bucket = &mesh->Buckets[RENDERER_BUCKET_SOLID];

				translation = Matrix::CreateTranslation(Hairs[p][i + 1].pos.xPos, Hairs[p][i + 1].pos.yPos, Hairs[p][i + 1].pos.zPos);
				rotation = Matrix::CreateFromYawPitchRoll(TO_RAD(Hairs[p][i + 1].pos.yRot), TO_RAD(Hairs[p][i + 1].pos.xRot), 0);
				m_hairsMatrices[6 * p + i] = rotation * translation;

				int baseVertex = lastVertex;

				for (int j = 0; j < bucket->Vertices.size(); j++)
				{
					int oldVertexIndex = (int)bucket->Vertices[j].Bone;
					if (oldVertexIndex < 4)
					{
						m_hairVertices[lastVertex].Position.x = parentVertices[i][oldVertexIndex].x;
						m_hairVertices[lastVertex].Position.y = parentVertices[i][oldVertexIndex].y;
						m_hairVertices[lastVertex].Position.z = parentVertices[i][oldVertexIndex].z;
						m_hairVertices[lastVertex].UV.x = bucket->Vertices[j].UV.x;
						m_hairVertices[lastVertex].UV.y = bucket->Vertices[j].UV.y;

						Vector3 n = Vector3(bucket->Vertices[j].Normal.x, bucket->Vertices[j].Normal.y, bucket->Vertices[j].Normal.z);
						n.Normalize();
						n = Vector3::TransformNormal(n, m_hairsMatrices[6 * p + i]);
						n.Normalize();

						m_hairVertices[lastVertex].Normal.x = n.x;
						m_hairVertices[lastVertex].Normal.y = n.y;
						m_hairVertices[lastVertex].Normal.z = n.z;

						m_hairVertices[lastVertex].Color = Vector4::One * 0.5f;

						lastVertex++;
					}
					else
					{
						Vector3 in = Vector3(bucket->Vertices[j].Position.x, bucket->Vertices[j].Position.y, bucket->Vertices[j].Position.z);
						Vector3 out = Vector3::Transform(in, m_hairsMatrices[6 * p + i]);

						if (i < 5)
						{
							parentVertices[i + 1][oldVertexIndex - 4].x = out.x;
							parentVertices[i + 1][oldVertexIndex - 4].y = out.y;
							parentVertices[i + 1][oldVertexIndex - 4].z = out.z;
						}

						m_hairVertices[lastVertex].Position.x = out.x;
						m_hairVertices[lastVertex].Position.y = out.y;
						m_hairVertices[lastVertex].Position.z = out.z;
						m_hairVertices[lastVertex].UV.x = bucket->Vertices[j].UV.x;
						m_hairVertices[lastVertex].UV.y = bucket->Vertices[j].UV.y;

						Vector3 n = Vector3(bucket->Vertices[j].Normal.x, bucket->Vertices[j].Normal.y, bucket->Vertices[j].Normal.z);
						n.Normalize();
						n = Vector3::TransformNormal(n, m_hairsMatrices[6 * p + i]);
						n.Normalize();

						m_hairVertices[lastVertex].Normal.x = n.x;
						m_hairVertices[lastVertex].Normal.y = n.y;
						m_hairVertices[lastVertex].Normal.z = n.z;

						m_hairVertices[lastVertex].Color = Vector4::One * 0.5f;

						lastVertex++;
					}
				}

				for (int j = 0; j < bucket->Indices.size(); j++)
				{
					m_hairIndices[lastIndex] = baseVertex + bucket->Indices[j];
					lastIndex++;
				}
			}
		}
	}

	// Transpose matrices for shaders
	for (int m = 0; m < 15; m++)
		laraObj->AnimationTransforms[m] = laraObj->AnimationTransforms[m];

	m_items[Lara.itemNumber].DoneAnimations = true;
}

bool Renderer11::drawLara(bool transparent, bool shadowMap)
{
	// Don't draw Lara if binoculars or sniper
	if (BinocularRange || SpotcamOverlay || SpotcamDontDrawLara || CurrentLevel == 0)
		return true;

	UINT stride = sizeof(RendererVertex);
	UINT offset = 0;

	int firstBucket = (transparent ? 2 : 0);
	int lastBucket = (transparent ? 4 : 2);

	m_context->IASetVertexBuffers(0, 1, &m_moveablesVertexBuffer->Buffer, &stride, &offset);
	m_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	m_context->IASetInputLayout(m_inputLayout);
	m_context->IASetIndexBuffer(m_moveablesIndexBuffer->Buffer, DXGI_FORMAT_R32_UINT, 0);

	RendererItem * item = &m_items[Lara.itemNumber];

	// Set shaders
	if (shadowMap)
	{
		m_context->VSSetShader(m_vsShadowMap, NULL, 0);
		m_context->PSSetShader(m_psShadowMap, NULL, 0);
	}
	else
	{
		m_context->VSSetShader(m_vsItems, NULL, 0);
		m_context->PSSetShader(m_psItems, NULL, 0);
	}

	// Set texture
	m_context->PSSetShaderResources(0, 1, &m_textureAtlas->ShaderResourceView);
	ID3D11SamplerState* sampler = m_states->AnisotropicClamp();
	m_context->PSSetSamplers(0, 1, &sampler);

	// Set camera matrices
	updateConstantBuffer(m_cbCameraMatrices, &m_stCameraMatrices, sizeof(CCameraMatrixBuffer));
	m_context->VSSetConstantBuffers(0, 1, &m_cbCameraMatrices);

	m_stMisc.AlphaTest = !transparent;
	updateConstantBuffer(m_cbMisc, &m_stMisc, sizeof(CMiscBuffer));
	m_context->PSSetConstantBuffers(3, 1, &m_cbMisc);

	RendererObject* laraObj = m_moveableObjects[ID_LARA];
	RendererObject* laraSkin = m_moveableObjects[ID_LARA_SKIN];
	RendererRoom& const room = m_rooms[LaraItem->roomNumber];

	m_stItem.World = m_LaraWorldMatrix;
	m_stItem.Position = Vector4(LaraItem->pos.xPos, LaraItem->pos.yPos, LaraItem->pos.zPos, 1.0f);
	m_stItem.AmbientLight = room.AmbientLight;
	memcpy(m_stItem.BonesMatrices, laraObj->AnimationTransforms.data(), sizeof(Matrix) * 32);
	updateConstantBuffer(m_cbItem, &m_stItem, sizeof(CItemBuffer));
	m_context->VSSetConstantBuffers(1, 1, &m_cbItem);
	m_context->PSSetConstantBuffers(1, 1, &m_cbItem);

	if (!shadowMap)
	{
		m_stLights.NumLights = item->Lights.size();
		for (int j = 0; j < item->Lights.size(); j++)
			memcpy(&m_stLights.Lights[j], item->Lights[j], sizeof(ShaderLight));
		updateConstantBuffer(m_cbLights, &m_stLights, sizeof(CLightBuffer));
		m_context->PSSetConstantBuffers(2, 1, &m_cbLights);
	}

	for (int k = 0; k < laraSkin->ObjectMeshes.size(); k++)
	{
		RendererMesh* mesh = m_meshPointersToMesh[reinterpret_cast<unsigned int>(Lara.meshPtrs[k])];

		for (int j = firstBucket; j < lastBucket; j++)
		{
			RendererBucket* bucket = &mesh->Buckets[j];

			if (bucket->Vertices.size() == 0)
				continue;

			if (j == RENDERER_BUCKET_SOLID_DS || j == RENDERER_BUCKET_TRANSPARENT_DS)
				m_context->RSSetState(m_states->CullNone());
			else
				m_context->RSSetState(m_states->CullCounterClockwise());

			// Draw vertices
			m_context->DrawIndexed(bucket->NumIndices, bucket->StartIndex, 0);
			m_numDrawCalls++;
		}
	}

	if (m_moveableObjects[ID_LARA_SKIN_JOINTS] != NULL)
	{
		RendererObject* laraSkinJoints = m_moveableObjects[ID_LARA_SKIN_JOINTS];

		for (int k = 0; k < laraSkinJoints->ObjectMeshes.size(); k++)
		{
			RendererMesh* mesh = laraSkinJoints->ObjectMeshes[k];

			for (int j = firstBucket; j < lastBucket; j++)
			{
				RendererBucket* bucket = &mesh->Buckets[j];

				if (bucket->Vertices.size() == 0)
					continue;

				// Draw vertices
				m_context->DrawIndexed(bucket->NumIndices, bucket->StartIndex, 0);
				m_numDrawCalls++;
			}
		}
	}

	if (!transparent)
	{
		for (int k = 0; k < laraSkin->ObjectMeshes.size(); k++)
		{
			RendererMesh* mesh = laraSkin->ObjectMeshes[k];

			for (int j = 0; j < NUM_BUCKETS; j++)
			{
				RendererBucket* bucket = &mesh->Buckets[j];

				if (bucket->Vertices.size() == 0)
					continue;

				// Draw vertices
				m_context->DrawIndexed(bucket->NumIndices, bucket->StartIndex, 0);
				m_numDrawCalls++;
			}
		}

		// Hairs are pre-transformed
		Matrix matrices[8] = { Matrix::Identity, Matrix::Identity, Matrix::Identity, Matrix::Identity,
							   Matrix::Identity, Matrix::Identity, Matrix::Identity, Matrix::Identity };
		memcpy(m_stItem.BonesMatrices, matrices, sizeof(Matrix) * 8);
		m_stItem.World = Matrix::Identity;
		updateConstantBuffer(m_cbItem, &m_stItem, sizeof(CItemBuffer));

		if (m_moveableObjects[ID_LARA_HAIR] != NULL)
		{
			m_primitiveBatch->Begin();
			m_primitiveBatch->DrawIndexed(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST, (const uint16_t*)m_hairIndices.data(), m_numHairIndices, m_hairVertices.data(), m_numHairVertices);
			m_primitiveBatch->End();
		}
	}

	return true;
}

void Renderer11::GetLaraAbsBonePosition(Vector3* pos, int joint)
{
	Matrix world = m_moveableObjects[ID_LARA]->AnimationTransforms[joint];
	world = world * m_LaraWorldMatrix;
	*pos = Vector3::Transform(*pos, world);
}

void Renderer11::GetItemAbsBonePosition(int itemNumber, Vector3* pos, int joint)
{
	ITEM_INFO* item = &Items[itemNumber];
	RendererObject* obj = m_moveableObjects[item->objectNumber];

	int rate;
	short* frmptr[2];
	int frac = GetFrame_D2(item, frmptr, &rate);

	RendererBone* bones[32];
	int nextBone = 0;

	Matrix rotation;
	Matrix transforms[32];
	for (int i = 0; i < 32; i++)
		transforms[i] = Matrix::Identity;

	// Push
	bones[nextBone++] = obj->Skeleton;

	while (nextBone != 0)
	{
		// Pop the last bone in the stack
		RendererBone* bone = bones[--nextBone];

		Vector3 p = Vector3((int) * (frmptr[0] + 6), (int) * (frmptr[0] + 7), (int) * (frmptr[0] + 8));

		fromTrAngle(&rotation, frmptr[0], bone->Index);

		if (frac)
		{
			Vector3 p2 = Vector3((int) * (frmptr[1] + 6), (int) * (frmptr[1] + 7), (int) * (frmptr[1] + 8));
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
		if (bone == obj->Skeleton)
			translation = Matrix::CreateTranslation(p.x, p.y, p.z);

		Matrix extraRotation;
		extraRotation = Matrix::CreateFromYawPitchRoll(bone->ExtraRotation.y, bone->ExtraRotation.x, bone->ExtraRotation.z);
		rotation = extraRotation * rotation;

		if (bone != obj->Skeleton)
			transforms[bone->Index] = rotation * bone->Transform;
		else
			transforms[bone->Index] = rotation * translation;

		if (bone != obj->Skeleton)
			transforms[bone->Index] = transforms[bone->Index] * transforms[bone->Parent->Index];

		if (bone->Index == joint)
		{
			*pos = Vector3::Transform(*pos, transforms[bone->Index]);
			return;
		}

		for (int i = 0; i < bone->Children.size(); i++)
		{
			// Push
			bones[nextBone++] = bone->Children[i];
		}
	}
}

int Renderer11::GetSpheres(ITEM_INFO* item, SPHERE* ptr, char worldSpace)
{
	short itemNumber = (item - Items) / sizeof(ITEM_INFO);

	int x, y, z;

	Matrix transforms[32];
	for (int i = 0; i < 32; i++)
		transforms[i] = Matrix::Identity;

	if (!item)
		return 0;

	transforms[0] = Matrix::CreateFromYawPitchRoll(item->pos.yRot, item->pos.xRot, item->pos.zRot);

	RendererBone* bones[32];
	int nextBone = 0;
	RendererObject* obj = m_moveableObjects[item->objectNumber];
	short* frmptr = GetBestFrame(item);

	// Push
	bones[nextBone++] = obj->Skeleton;

	while (nextBone != 0)
	{
		// Pop the last bone in the stack
		RendererBone* bone = bones[--nextBone];

		Vector3 p = Vector3((int) * (frmptr + 6), (int) * (frmptr + 7), (int) * (frmptr + 8));

		Matrix rotation;
		fromTrAngle(&rotation, frmptr, bone->Index);

		Matrix translation;
		if (bone == obj->Skeleton)
			translation = Matrix::CreateTranslation(p.x, p.y, p.z);

		Matrix extraRotation;
		extraRotation = Matrix::CreateFromYawPitchRoll(bone->ExtraRotation.y, bone->ExtraRotation.x, bone->ExtraRotation.z);
		rotation = extraRotation * rotation;

		if (bone != obj->Skeleton)
			transforms[bone->Index] = rotation * bone->Transform;
		else
			transforms[bone->Index] = rotation * translation;

		if (bone != obj->Skeleton)
			transforms[bone->Index] = transforms[bone->Index] * transforms[bone->Parent->Index];

		for (int i = 0; i < bone->Children.size(); i++)
		{
			// Push
			bones[nextBone++] = bone->Children[i];
		}
	}

	for (int i = 0; i < obj->ObjectMeshes.size(); i++)
	{
		RendererMesh* mesh = obj->ObjectMeshes[i];
		
		Vector3 c = Vector3::Transform(mesh->Sphere.Center, transforms[i]);
		
		ptr->x = c.x;
		ptr->y = c.y;
		ptr->z = c.z;
		ptr->r = mesh->Sphere.Radius;

		ptr++;
	}

	return obj->ObjectMeshes.size();
}

Matrix Renderer11::GetBoneMatrix(ITEM_INFO* item, int joint)
{
	int itemNumber = (item - Items) / sizeof(ITEM_INFO);
	RendererObject* obj = m_moveableObjects[ID_LARA];
	return obj->AnimationTransforms[joint] * m_LaraWorldMatrix;
}