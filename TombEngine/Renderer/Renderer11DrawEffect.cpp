#include "framework.h"
#include "Renderer/Renderer11.h"
#include "Game/effects/footprint.h"
#include "Game/effects/effects.h"
#include "Game/effects/tomb4fx.h"
#include "Game/Lara/lara.h"
#include "Game/animation.h"
#include "Game/camera.h"
#include "Game/collision/collide_room.h"
#include "Game/control/control.h"
#include "Game/effects/debris.h"
#include "Specific/setup.h"
#include "Game/effects/bubble.h"
#include "Specific/level.h"
#include "Game/control/box.h"
#include "Game/effects/smoke.h"
#include "Game/effects/spark.h"
#include "Game/effects/drip.h"
#include "Game/effects/explosion.h"
#include "Game/effects/weather.h"
#include "Quad/RenderQuad.h"
#include "Game/particle/SimpleParticle.h"
#include "Renderer/RendererSprites.h"
#include "Game/effects/lightning.h"
#include "Game/items.h"
#include "Game/misc.h"

using namespace TEN::Effects::Lightning;
using namespace TEN::Effects::Environment;

extern BLOOD_STRUCT Blood[MAX_SPARKS_BLOOD];
extern FIRE_SPARKS FireSparks[MAX_SPARKS_FIRE];
extern SMOKE_SPARKS SmokeSparks[MAX_SPARKS_SMOKE];
extern DRIP_STRUCT Drips[MAX_DRIPS];
extern SHOCKWAVE_STRUCT ShockWaves[MAX_SHOCKWAVE];
extern FIRE_LIST Fires[MAX_FIRE_LIST];
extern GUNFLASH_STRUCT Gunflashes[MAX_GUNFLASH]; // offset 0xA31D8
extern SPARKS Sparks[MAX_SPARKS];
extern SPLASH_STRUCT Splashes[MAX_SPLASHES];
extern RIPPLE_STRUCT Ripples[MAX_RIPPLES];

BITE_INFO EnemyBites[9] =
{
	{ 20, -95, 240, 13 },
	{ 48, 0, 180, -11 },
	{ -48, 0, 180, 14 },
	{ -48, 5, 225, 14 },
	{ 15, -60, 195, 13 },
	{ -30, -65, 250, 18 },
	{ 0, -110, 480, 13 },
	{ -20, -80, 190, -10 },
	{ 10, -60, 200, 13 }
};

namespace TEN::Renderer 
{
	using namespace TEN::Effects::Footprints;
	using std::vector;

	void Renderer11::DrawLightning(RenderView& view) 
	{
		for (int i = 0; i < Lightning.size(); i++)
		{
			LIGHTNING_INFO* arc = &Lightning[i];

			if (arc->life)
			{
				LightningPos[0].x = arc->pos1.x;
				LightningPos[0].y = arc->pos1.y;
				LightningPos[0].z = arc->pos1.z;

				memcpy(&LightningPos[1], arc, 48);

				LightningPos[5].x = arc->pos4.x;
				LightningPos[5].y = arc->pos4.y;
				LightningPos[5].z = arc->pos4.z;

				for (int j = 0; j < 6; j++)
				{
					LightningPos[j].x -= LaraItem->Pose.Position.x;
					LightningPos[j].y -= LaraItem->Pose.Position.y;
					LightningPos[j].z -= LaraItem->Pose.Position.z;
				}

				CalcLightningSpline(&LightningPos[0], LightningBuffer, arc);

				if (abs(LightningPos[0].x) <= 24576 && abs(LightningPos[0].y) <= 24576 && abs(LightningPos[0].z) <= 24576)
				{
					short* interpolatedPos = &LightningBuffer[0];

					for (int s = 0; s < 3 * arc->segments - 1; s++)
					{
						int ix = LaraItem->Pose.Position.x + interpolatedPos[0];
						int iy = LaraItem->Pose.Position.y + interpolatedPos[1];
						int iz = LaraItem->Pose.Position.z + interpolatedPos[2];

						interpolatedPos += 4;

						int ix2 = LaraItem->Pose.Position.x + interpolatedPos[0];
						int iy2 = LaraItem->Pose.Position.y + interpolatedPos[1];
						int iz2 = LaraItem->Pose.Position.z + interpolatedPos[2];

						byte r, g, b;

						if (arc->life >= 16)
						{
							r = arc->r;
							g = arc->g;
							b = arc->b;
						}
						else
						{
							r = arc->life * arc->r / 16;
							g = arc->life * arc->g / 16;
							b = arc->life * arc->b / 16;
						}

						Vector3 pos1 = Vector3(ix, iy, iz);
						Vector3 pos2 = Vector3(ix2, iy2, iz2);

						Vector3 d = pos2 - pos1;
						d.Normalize();

						Vector3 c = (pos1 + pos2) / 2.0f;

						AddSpriteBillboardConstrained(&m_sprites[Objects[ID_DEFAULT_SPRITES].meshIndex + SPR_LIGHTHING],
							c,
							Vector4(r / 255.0f, g / 255.0f, b / 255.0f, 1.0f),
							(PI / 2),
							1.0f,
							{ arc->width * 8.0f,
							Vector3::Distance(pos1, pos2) },
							BLENDMODE_ADDITIVE,
							d, view);
					}
				}
			}
		}
	}

	void Renderer11::DrawSmokes(RenderView& view) 
	{
		for (int i = 0; i < 32; i++) 
		{
			SMOKE_SPARKS* spark = &SmokeSparks[i];

			if (spark->on) 
			{
				AddSpriteBillboard(&m_sprites[spark->def],
								   Vector3(spark->x, spark->y, spark->z),
								   Vector4(spark->shade / 255.0f, spark->shade / 255.0f, spark->shade / 255.0f, 1.0f),
								   TO_RAD(spark->rotAng), spark->scalar, { spark->size * 4.0f, spark->size * 4.0f },
								   BLENDMODE_ADDITIVE, view);
			}
		}
	}


	void Renderer11::DrawFires(RenderView& view) 
	{
		for (int k = 0; k < MAX_FIRE_LIST; k++) 
		{
			FIRE_LIST* fire = &Fires[k];
			if (fire->on) {
				for (int i = 0; i < MAX_SPARKS_FIRE; i++) 
				{
					FIRE_SPARKS* spark = &FireSparks[i];
					if (spark->on)
						AddSpriteBillboard(&m_sprites[spark->def], Vector3(fire->x + spark->x, fire->y + spark->y, fire->z + spark->z), 
																   Vector4(spark->r / 255.0f, spark->g / 255.0f, spark->b / 255.0f, 1.0f), 
																   TO_RAD(spark->rotAng), 
																   spark->scalar,
																   { spark->size * (float)fire->size, spark->size * (float)fire->size }, BLENDMODE_ADDITIVE, view);
				}
			}
		}
	}

	void Renderer11::DrawSparks(RenderView& view) 
	{
		Vector3Int nodePos;

		for (int i = 0; i < MAX_NODE; i++)
			NodeOffsets[i].gotIt = false;

		for (int i = 0; i < MAX_SPARKS; i++) 
		{
			SPARKS* spark = &Sparks[i];
			if (spark->on) 
			{
				if (spark->flags & SP_DEF) 
				{
					Vector3 pos = Vector3(spark->x, spark->y, spark->z);

					if (spark->flags & SP_FX) 
					{
						FX_INFO* fx = &EffectList[spark->fxObj];

						pos.x += fx->pos.Position.x;
						pos.y += fx->pos.Position.y;
						pos.z += fx->pos.Position.z;

						if ((spark->sLife - spark->life) > (rand() & 7) + 4) 
						{
							spark->flags &= ~SP_FX;
							spark->x = pos.x;
							spark->y = pos.y;
							spark->z = pos.z;
						}
					} 
					else if (!(spark->flags & SP_ITEM)) 
					{
						pos.x = spark->x;
						pos.y = spark->y;
						pos.z = spark->z;
					}
					else 
					{
						ItemInfo* item = &g_Level.Items[spark->fxObj];

						if (spark->flags & SP_NODEATTACH) {
							if (NodeOffsets[spark->nodeNumber].gotIt) {
								nodePos.x = NodeVectors[spark->nodeNumber].x;
								nodePos.y = NodeVectors[spark->nodeNumber].y;
								nodePos.z = NodeVectors[spark->nodeNumber].z;
							} else {
								nodePos.x = NodeOffsets[spark->nodeNumber].x;
								nodePos.y = NodeOffsets[spark->nodeNumber].y;
								nodePos.z = NodeOffsets[spark->nodeNumber].z;

								int meshNum = NodeOffsets[spark->nodeNumber].meshNum;
								if (meshNum >= 0)
									GetJointAbsPosition(item, &nodePos, meshNum);
								else
									GetLaraJointPosition(&nodePos, -meshNum);

								NodeOffsets[spark->nodeNumber].gotIt = true;

								NodeVectors[spark->nodeNumber].x = nodePos.x;
								NodeVectors[spark->nodeNumber].y = nodePos.y;
								NodeVectors[spark->nodeNumber].z = nodePos.z;
							}

							pos.x += nodePos.x;
							pos.y += nodePos.y;
							pos.z += nodePos.z;

							if (spark->sLife - spark->life > (rand() & 3) + 8) {
								spark->flags &= ~SP_ITEM;
								spark->x = pos.x;
								spark->y = pos.y;
								spark->z = pos.z;
							}
						} 
						else 
						{
							pos.x += item->Pose.Position.x;
							pos.y += item->Pose.Position.y;
							pos.z += item->Pose.Position.z;
						}
					}

					AddSpriteBillboard(&m_sprites[spark->def],
									   pos,
									   Vector4(spark->r / 255.0f, spark->g / 255.0f, spark->b / 255.0f, 1.0f),
									   TO_RAD(spark->rotAng), spark->scalar, 
									   {spark->size, spark->size},
						BLENDMODE_ADDITIVE, view);
				} 
				else 
				{
					Vector3 pos = Vector3(spark->x, spark->y, spark->z);
					Vector3 v = Vector3(spark->xVel, spark->yVel, spark->zVel);
					v.Normalize();
					AddSpriteBillboardConstrained(&m_sprites[Objects[ID_SPARK_SPRITE].meshIndex], 
						pos, 
						Vector4(spark->r / 255.0f, spark->g / 255.0f, spark->b / 255.0f, 1.0f), 
						TO_RAD(spark->rotAng), 
						spark->scalar, 
						Vector2(4, spark->size), BLENDMODE_ADDITIVE, v, view);
				}
			}
		}
	}

	void Renderer11::DrawSplashes(RenderView& view) 
	{
		constexpr size_t NUM_POINTS = 9;

		for (int i = 0; i < MAX_SPLASHES; i++) 
		{
			SPLASH_STRUCT& splash = Splashes[i];

			if (splash.isActive) 
			{
				constexpr float alpha = 360 / NUM_POINTS;
				byte color = (splash.life >= 32 ? 128 : (byte)((splash.life / 32.0f) * 128));

				if (!splash.isRipple) 
				{
					if (splash.heightSpeed < 0 && splash.height < 1024) 
					{
						float multiplier = splash.height / 1024.0f;
						color = (float)color * multiplier;
					}
				}

				float innerRadius = splash.innerRad;
				float outerRadius = splash.outerRad;
				float xInner;
				float zInner;
				float xOuter;
				float zOuter;
				float x2Inner;
				float z2Inner;
				float x2Outer;
				float z2Outer;
				float yInner = splash.y;
				float yOuter = splash.y - splash.height;

				for (int i = 0; i < NUM_POINTS; i++) 
				{
					xInner = innerRadius * sin(alpha * i * PI / 180);
					zInner = innerRadius * cos(alpha * i * PI / 180);
					xOuter = outerRadius * sin(alpha * i * PI / 180);
					zOuter = outerRadius * cos(alpha * i * PI / 180);
					xInner += splash.x;
					zInner += splash.z;
					xOuter += splash.x;
					zOuter += splash.z;
					int j = (i + 1) % NUM_POINTS;
					x2Inner = innerRadius * sin(alpha * j * PI / 180);
					x2Inner += splash.x;
					z2Inner = innerRadius * cos(alpha * j * PI / 180);
					z2Inner += splash.z;
					x2Outer = outerRadius * sin(alpha * j * PI / 180);
					x2Outer += splash.x;
					z2Outer = outerRadius * cos(alpha * j * PI / 180);
					z2Outer += splash.z;
					addSprite3D(&m_sprites[Objects[ID_DEFAULT_SPRITES].meshIndex + splash.spriteSequenceStart + (int)splash.animationPhase], 
								Vector3(xOuter, yOuter, zOuter), 
								Vector3(x2Outer, yOuter, z2Outer), 
								Vector3(x2Inner, yInner, z2Inner), 
								Vector3(xInner, yInner, zInner), Vector4(color / 255.0f, color / 255.0f, color / 255.0f, 1.0f), 
								0, 1, { 0, 0 }, BLENDMODE_ADDITIVE, view);
				}
			}
		}
	}

	void Renderer11::DrawBubbles(RenderView& view) 
	{
		for (int i = 0; i < MAX_BUBBLES; i++) 
		{
			BUBBLE_STRUCT* bubble = &Bubbles[i];
			if (bubble->active)
			{
				AddSpriteBillboard(&m_sprites[Objects[ID_DEFAULT_SPRITES].meshIndex + bubble->spriteNum],
					Vector3(bubble->worldPosition.x, bubble->worldPosition.y, bubble->worldPosition.z),
					bubble->color,
					bubble->rotation,
					1.0f, { bubble->size * 0.5f, bubble->size * 0.5f }, BLENDMODE_ADDITIVE, view);
			}
		}
	}

	void Renderer11::DrawDrips(RenderView& view) 
	{
		for (int i = 0; i < MAX_DRIPS; i++) 
		{
			DRIP_STRUCT* drip = &Drips[i];

			if (drip->on) 
			{
				AddSpriteBillboardConstrained(&m_sprites[Objects[ID_DRIP_SPRITE].meshIndex],
					Vector3(drip->x, drip->y, drip->z),
					Vector4(drip->r / 255.0f, drip->g / 255.0f, drip->b / 255.0f, 1.0f),
					0.0f, 1.0f, Vector2(TEN::Effects::Drip::DRIP_WIDTH, 24.0f), BLENDMODE_ADDITIVE, -Vector3::UnitY, view);
			}
		}
	}

	void Renderer11::DrawRipples(RenderView& view) 
	{
		for (int i = 0; i < MAX_RIPPLES; i++) 
		{
			RIPPLE_STRUCT* ripple = &Ripples[i];

			if (ripple->active) 
			{
				float y = ripple->worldPos.y;
				if (ripple->isBillboard) 
				{
					AddSpriteBillboard(&m_sprites[ripple->SpriteID], ripple->worldPos, ripple->currentColor, ripple->rotation, 1, { ripple->size, ripple->size }, BLENDMODE_ADDITIVE, view);
				} 
				else 
				{
					AddSpriteBillboardConstrainedLookAt(&m_sprites[ripple->SpriteID], 
						ripple->worldPos, 
						ripple->currentColor, 
						ripple->rotation, 
						1, { ripple->size * 2, ripple->size * 2 }, BLENDMODE_ADDITIVE, Vector3(0, -1, 0), view);

					//AddSprite3D(&m_sprites[Objects[ID_DEFAULT_SPRITES].meshIndex + ripple->SpriteID], Vector3(x1, y, z2), Vector3(x2, y, z2), Vector3(x2, y, z1), Vector3(x1, y, z1), ripple->currentColor, 0.0f, 1.0f, ripple->size, ripple->size, BLENDMODE_ALPHABLEND);
				}
			}
		}
	}

	void Renderer11::DrawShockwaves(RenderView& view) 
	{
		for (int i = 0; i < MAX_SHOCKWAVE; i++) 
		{
			SHOCKWAVE_STRUCT* shockwave = &ShockWaves[i];

			if (shockwave->life) 
			{
				byte color = shockwave->life * 8;

				int dl = shockwave->outerRad - shockwave->innerRad;
				Matrix rotationMatrix = Matrix::CreateRotationX(TO_RAD(shockwave->xRot));
				Vector3 pos = Vector3(shockwave->x, shockwave->y, shockwave->z);

				// Inner circle
				float angle = PI / 32.0f;
				float c = cos(angle);
				float s = sin(angle);
				float x1 = (shockwave->innerRad * c);
				float z1 = (shockwave->innerRad * s);
				float x4 = (shockwave->outerRad * c);
				float z4 = (shockwave->outerRad * s);
				angle -= PI / 8.0f;

				Vector3 p1 = Vector3(x1, 0, z1);
				Vector3 p4 = Vector3(x4, 0, z4);

				p1 = Vector3::Transform(p1, rotationMatrix);
				p4 = Vector3::Transform(p4, rotationMatrix);

				for (int j = 0; j < 16; j++) 
				{
					c = cos(angle);
					s = sin(angle);
					float x2 = (shockwave->innerRad * c);
					float z2 = (shockwave->innerRad * s);
					float x3 = (shockwave->outerRad * c);
					float z3 = (shockwave->outerRad * s);
					angle -= PI / 8.0f;

					Vector3 p2 = Vector3(x2, 0, z2);
					Vector3 p3 = Vector3(x3, 0, z3);

					p2 = Vector3::Transform(p2, rotationMatrix);
					p3 = Vector3::Transform(p3, rotationMatrix);

					addSprite3D(&m_sprites[Objects[ID_DEFAULT_SPRITES].meshIndex + SPR_SPLASH],
								pos + p1,
								pos + p2,
								pos + p3,
								pos + p4,
								Vector4(
								shockwave->r * shockwave->life / 255.0f / 16.0f,
								shockwave->g * shockwave->life / 255.0f / 16.0f,
								shockwave->b * shockwave->life / 255.0f / 16.0f,
								1.0f),
								0, 1, {0,0}, BLENDMODE_ADDITIVE, view);

					p1 = p2;
					p4 = p3;
				}
			}
		}
	}

	void Renderer11::DrawBlood(RenderView& view) 
	{
		for (int i = 0; i < 32; i++) 
		{
			BLOOD_STRUCT* blood = &Blood[i];

			if (blood->on) 
			{
				AddSpriteBillboard(&m_sprites[Objects[ID_DEFAULT_SPRITES].meshIndex + SPR_BLOOD],
								   Vector3(blood->x, blood->y, blood->z),
								   Vector4(blood->shade / 255.0f, blood->shade * 0, blood->shade * 0, 1.0f),
								   TO_RAD(blood->rotAng), 1.0f, { blood->size * 8.0f, blood->size * 8.0f },
								   BLENDMODE_ADDITIVE, view);
			}
		}
	}

	void Renderer11::DrawWeatherParticles(RenderView& view) 
	{		
		for (auto& p : Weather.GetParticles())
		{
			if (!p.Enabled)
				continue;

			switch (p.Type)
			{
			case WeatherType::Snow:
				AddSpriteBillboard(&m_sprites[Objects[ID_DEFAULT_SPRITES].meshIndex + SPR_UNDERWATERDUST],
					p.Position,
					Vector4(1.0f, 1.0f, 1.0f, p.Transparency()),
					0.0f, 1.0f, Vector2(p.Size),
					BLENDMODE_ADDITIVE, view);
				break;

			case WeatherType::Rain:
				Vector3 v;
				p.Velocity.Normalize(v);
				AddSpriteBillboardConstrained(&m_sprites[Objects[ID_DRIP_SPRITE].meshIndex], 
					p.Position,
					Vector4(0.8f, 1.0f, 1.0f, p.Transparency()),
					0.0f, 1.0f, Vector2(TEN::Effects::Drip::DRIP_WIDTH, p.Size), BLENDMODE_ADDITIVE, -v, view);
				break;
			}
		}
	}

	bool Renderer11::DrawGunFlashes(RenderView& view) 
	{
		if (!Lara.RightArm.FlashGun && !Lara.LeftArm.FlashGun)
			return true;

		if (BinocularRange > 0)
			return true;

		Matrix world;
		Matrix translation;
		Matrix rotation;

		RendererObject& laraObj = *m_moveableObjects[ID_LARA];
		RendererObject& laraSkin = *m_moveableObjects[ID_LARA_SKIN];

		ObjectInfo* obj = &Objects[0];
		RendererRoom const & room = m_rooms[LaraItem->RoomNumber];
		RendererItem* item = &m_items[Lara.ItemNumber];

		m_stItem.AmbientLight = room.AmbientLight;
		memcpy(m_stItem.BonesMatrices, &Matrix::Identity, sizeof(Matrix));

		m_stLights.NumLights = item->LightsToDraw.size();
		for (int j = 0; j < item->LightsToDraw.size(); j++)
			memcpy(&m_stLights.Lights[j], item->LightsToDraw[j], sizeof(ShaderLight));
		m_cbLights.updateData(m_stLights, m_context.Get());
		m_context->PSSetConstantBuffers(2, 1, m_cbLights.get());

		short length = 0;
		short zOffset = 0;
		short rotationX = 0;

		SetBlendMode(BLENDMODE_ADDITIVE);

		SetAlphaTest(ALPHA_TEST_GREATER_THAN, ALPHA_TEST_THRESHOLD);

		if (Lara.Control.Weapon.GunType != LaraWeaponType::Flare &&
			Lara.Control.Weapon.GunType != LaraWeaponType::Shotgun &&
			Lara.Control.Weapon.GunType != LaraWeaponType::Crossbow)
		{
			switch (Lara.Control.Weapon.GunType)
			{
			case LaraWeaponType::Revolver:
				length = 192;
				zOffset = 68;
				rotationX = -14560;
				break;

			case LaraWeaponType::Uzi:
				length = 190;
				zOffset = 50;
				rotationX = -14560;
				break;

			case LaraWeaponType::HK:
				length = 300;
				zOffset = 92;
				rotationX = -14560;
				break;

			default:
			case LaraWeaponType::Pistol:
				length = 180;
				zOffset = 40;
				rotationX = -16830;
				break;
			}

			ObjectInfo* flashObj = &Objects[ID_GUN_FLASH];
			RendererObject& flashMoveable = *m_moveableObjects[ID_GUN_FLASH];
			RendererMesh* flashMesh = flashMoveable.ObjectMeshes[0];

			for (auto& flashBucket : flashMesh->buckets) 
			{
				if (flashBucket.BlendMode == BLENDMODE_OPAQUE)
					continue;
				if (flashBucket.Vertices.size() != 0) 
				{
					Matrix offset = Matrix::CreateTranslation(0, length, zOffset);
					Matrix rotation2 = Matrix::CreateRotationX(TO_RAD(rotationX));

					if (Lara.LeftArm.FlashGun)
					{
						world = laraObj.AnimationTransforms[LM_LHAND] * m_LaraWorldMatrix;
						world = offset * world;
						world = rotation2 * world;

						m_stItem.World = world;
						m_cbItem.updateData(m_stItem, m_context.Get());
						m_context->VSSetConstantBuffers(1, 1, m_cbItem.get());

						DrawIndexedTriangles(flashBucket.Indices.size(), flashBucket.StartIndex, 0);
					}

					if (Lara.RightArm.FlashGun)
					{
						world = laraObj.AnimationTransforms[LM_RHAND] * m_LaraWorldMatrix;
						world = offset * world;
						world = rotation2 * world;

						m_stItem.World = world;
						m_cbItem.updateData(m_stItem, m_context.Get());
						m_context->VSSetConstantBuffers(1, 1, m_cbItem.get());

						DrawIndexedTriangles(flashBucket.Indices.size(), flashBucket.StartIndex, 0);
					}
				}
			}
		}

		SetBlendMode(BLENDMODE_OPAQUE);

		return true;
	}

	void Renderer11::DrawBaddyGunflashes(RenderView& view)
	{
		for (auto room : view.roomsToDraw)
		{
			for (auto item : room->ItemsToDraw)
			{
				// Does the item need gunflash?
				ItemInfo* nativeItem = &g_Level.Items[item->ItemNumber];
				ObjectInfo* obj = &Objects[nativeItem->ObjectNumber];

				if (obj->biteOffset == -1)
					continue;

				if (nativeItem->Data.is<CreatureInfo>())
				{
					auto* creature = GetCreatureInfo(nativeItem);
					if (!creature->FiredWeapon)
						continue;
				}

				RendererRoom const& room = m_rooms[nativeItem->RoomNumber];
				RendererObject& flashMoveable = *m_moveableObjects[ID_GUN_FLASH];

				m_stItem.AmbientLight = room.AmbientLight;
				memcpy(m_stItem.BonesMatrices, &Matrix::Identity, sizeof(Matrix));

				m_stLights.NumLights = item->LightsToDraw.size();
				for (int j = 0; j < item->LightsToDraw.size(); j++)
					memcpy(&m_stLights.Lights[j], item->LightsToDraw[j], sizeof(ShaderLight));
				m_cbLights.updateData(m_stLights, m_context.Get());
				m_context->PSSetConstantBuffers(2, 1, m_cbLights.get());

				SetBlendMode(BLENDMODE_ADDITIVE);
				
				SetAlphaTest(ALPHA_TEST_GREATER_THAN, ALPHA_TEST_THRESHOLD);

				BITE_INFO* bites[2] = {
					&EnemyBites[obj->biteOffset],
					&EnemyBites[obj->biteOffset + 1]
				};

				int numBites = (bites[0]->meshNum < 0) + 1;

				for (int k = 0; k < numBites; k++)
				{
					int joint = abs(bites[k]->meshNum);

					RendererMesh* flashMesh = flashMoveable.ObjectMeshes[0];

					for (auto& flashBucket : flashMesh->buckets)
					{
						if (flashBucket.BlendMode == BLENDMODE_OPAQUE)
							continue;
						if (flashBucket.Vertices.size() != 0)
						{
							Matrix offset = Matrix::CreateTranslation(bites[k]->x, bites[k]->y, bites[k]->z);
							Matrix rotationX = Matrix::CreateRotationX(TO_RAD(49152));
							Matrix rotationZ = Matrix::CreateRotationZ(TO_RAD(2 * GetRandomControl()));

							Matrix world = item->AnimationTransforms[joint] * item->World;
							world = rotationX * world;
							world = offset * world;
							world = rotationZ * world;

							m_stItem.World = world;
							m_cbItem.updateData(m_stItem, m_context.Get());
							m_context->VSSetConstantBuffers(1, 1, m_cbItem.get());

							DrawIndexedTriangles(flashBucket.Indices.size(), flashBucket.StartIndex, 0);
						}
					}
				}
			}
		}

		SetBlendMode(BLENDMODE_OPAQUE);

	}

	Texture2D Renderer11::CreateDefaultNormalTexture() 
	{
		vector<byte> data = {128,128,255,1};
		return Texture2D(m_device.Get(),1,1,data.data());
	}

	void Renderer11::DrawFootprints(RenderView& view) 
	{
		for (auto i = footprints.begin(); i != footprints.end(); i++) 
		{
			FOOTPRINT_STRUCT& footprint = *i;
			auto spriteIndex = Objects[ID_MISC_SPRITES].meshIndex + 1 + (int)footprint.RightFoot;

			if (footprint.Active && g_Level.Sprites.size() > spriteIndex)
				addSprite3D(&m_sprites[spriteIndex], 
					footprint.Position[0], footprint.Position[1], footprint.Position[2], footprint.Position[3], 
					Vector4(footprint.Opacity), 0, 1, { 1,1 }, BLENDMODE_SUBTRACTIVE, view);
		}
	}

	void Renderer11::DrawUnderwaterDust(RenderView& view) 
	{
		if (m_firstUnderwaterDustParticles) {
			for (int i = 0; i < NUM_UNDERWATER_DUST_PARTICLES; i++)
				m_underwaterDustParticles[i].Reset = true;
		}

		for (int i = 0; i < NUM_UNDERWATER_DUST_PARTICLES; i++) 
		{
			RendererUnderwaterDustParticle* dust = &m_underwaterDustParticles[i];

			if (dust->Reset) {
				dust->X = LaraItem->Pose.Position.x + rand() % UNDERWATER_DUST_PARTICLES_RADIUS - UNDERWATER_DUST_PARTICLES_RADIUS / 2.0f;
				dust->Y = LaraItem->Pose.Position.y + rand() % UNDERWATER_DUST_PARTICLES_RADIUS - UNDERWATER_DUST_PARTICLES_RADIUS / 2.0f;
				dust->Z = LaraItem->Pose.Position.z + rand() % UNDERWATER_DUST_PARTICLES_RADIUS - UNDERWATER_DUST_PARTICLES_RADIUS / 2.0f;

				// Check if water room
				short roomNumber = Camera.pos.roomNumber;
				FloorInfo* floor = GetFloor(dust->X, dust->Y, dust->Z, &roomNumber);
				if (!IsRoomUnderwater(roomNumber))
					continue;

				if (!IsInRoom(dust->X, dust->Y, dust->Z, roomNumber)) 
				{
					dust->Reset = true;
					continue;
				}

				dust->Life = 0;
				dust->Reset = false;
			}

			dust->Life++;
			byte color = (dust->Life > 16 ? 32 - dust->Life : dust->Life) * 4;

			AddSpriteBillboard(
				&m_sprites[Objects[ID_DEFAULT_SPRITES].meshIndex + SPR_UNDERWATERDUST], 
				Vector3(dust->X, dust->Y, dust->Z),
				Vector4(color / 255.0f, color / 255.0f, color / 255.0f, 1.0f), 
				0.0f,
				1.0f, 
				{ 24, 24 },
				BLENDMODE_ADDITIVE,
				view);

			if (dust->Life >= 32)
				dust->Reset = true;
		}

		m_firstUnderwaterDustParticles = false;

		return;
	}

	void Renderer11::DrawSprites(RenderView& view)
	{
		const int numSpritesToDraw = view.spritesToDraw.size();
		
		BindRenderTargetAsTexture(TEXTURE_DEPTH_MAP, &m_depthMap, SAMPLER_LINEAR_CLAMP);

		std::sort(
			view.spritesToDraw.begin(),
			view.spritesToDraw.end(),
			[](RendererSpriteToDraw& a, RendererSpriteToDraw& b)
			{
				return (a.Type > b.Type);
			}
		);

		for (auto& spr : view.spritesToDraw) 
		{
			// Calculate matrices for sprites
			Matrix spriteMatrix;
			Matrix scale = Matrix::CreateScale((spr.Width) * spr.Scale, (spr.Height) * spr.Scale, spr.Scale);

			if (spr.Type == RENDERER_SPRITE_TYPE::SPRITE_TYPE_BILLBOARD)
			{
				Vector3 cameraUp = Vector3(View._12, View._22, View._32);
				spriteMatrix = scale * Matrix::CreateBillboard(spr.pos, Vector3(Camera.pos.x, Camera.pos.y, Camera.pos.z), cameraUp);
			}
			else if (spr.Type == RENDERER_SPRITE_TYPE::SPRITE_TYPE_BILLBOARD_CUSTOM)
			{
				Matrix rotation = Matrix::CreateRotationY(spr.Rotation);
				Vector3 quadForward = Vector3(0, 0, 1);
				spriteMatrix = scale * Matrix::CreateConstrainedBillboard(
					spr.pos,
					Vector3(Camera.pos.x, Camera.pos.y, Camera.pos.z),
					spr.ConstrainAxis,
					nullptr,
					&quadForward);
			}
			else if (spr.Type == RENDERER_SPRITE_TYPE::SPRITE_TYPE_BILLBOARD_LOOKAT)
			{
				Matrix translation = Matrix::CreateTranslation(spr.pos);
				Matrix rotation = Matrix::CreateRotationZ(spr.Rotation) * Matrix::CreateLookAt(Vector3::Zero, spr.LookAtAxis, Vector3::UnitZ);
				spriteMatrix = scale * rotation * translation;
			}
			else if (spr.Type == RENDERER_SPRITE_TYPE::SPRITE_TYPE_3D)
			{
				spriteMatrix = Matrix::Identity;
			}

			if (DoesBlendModeRequireSorting(spr.BlendMode))
			{
				// Collect sprites
				int distance = (spr.pos - Vector3(Camera.pos.x, Camera.pos.y, Camera.pos.z)).Length();
				RendererTransparentFace face;
				face.type = RendererTransparentFaceType::TRANSPARENT_FACE_SPRITE;
				face.info.sprite = &spr;
				face.distance = distance;
				face.info.world = spriteMatrix;
				face.info.blendMode = spr.BlendMode;

				RendererRoom& room = m_rooms[GetRoomNumberForSpriteTest(spr.pos)];
				room.TransparentFacesToDraw.push_back(face);
			}
			else
			{
				// Draw sprites immediately
				SetBlendMode(spr.BlendMode);

				BindTexture(TEXTURE_COLOR_MAP, spr.Sprite->Texture, SAMPLER_LINEAR_CLAMP);

				Matrix scale = Matrix::CreateScale((spr.Width) * spr.Scale, (spr.Height) * spr.Scale, spr.Scale);
				
				if (spr.Type == RENDERER_SPRITE_TYPE::SPRITE_TYPE_BILLBOARD) 
				{
					UINT stride = sizeof(RendererVertex);
					UINT offset = 0;

					SetDepthState(DEPTH_STATE_READ_ONLY_ZBUFFER);
					SetCullMode(CULL_MODE_NONE);
					SetAlphaTest(ALPHA_TEST_GREATER_THAN, ALPHA_TEST_THRESHOLD);

					m_context->VSSetShader(m_vsSprites.Get(), NULL, 0);
					m_context->PSSetShader(m_psSprites.Get(), NULL, 0);

					m_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
					m_context->IASetInputLayout(m_inputLayout.Get());
					m_context->IASetVertexBuffers(0, 1, quadVertexBuffer.GetAddressOf(), &stride, &offset);
					
					m_stSprite.billboardMatrix = spriteMatrix;
					m_stSprite.color = spr.color;
					m_stSprite.isBillboard = true;
					m_cbSprite.updateData(m_stSprite, m_context.Get());
					BindConstantBufferVS(CB_SPRITE, m_cbSprite.get());

					m_context->Draw(4, 0);
					m_numDrawCalls++;
					m_numPolygons += 2;
					m_numSpritesDrawCalls++;

				}
				else if (spr.Type == RENDERER_SPRITE_TYPE::SPRITE_TYPE_BILLBOARD_CUSTOM) 
				{
					UINT stride = sizeof(RendererVertex);
					UINT offset = 0;

					SetDepthState(DEPTH_STATE_READ_ONLY_ZBUFFER);
					SetCullMode(CULL_MODE_NONE);

					m_context->VSSetShader(m_vsSprites.Get(), NULL, 0);
					m_context->PSSetShader(m_psSprites.Get(), NULL, 0);

					SetAlphaTest(ALPHA_TEST_GREATER_THAN, ALPHA_TEST_THRESHOLD);

					m_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
					m_context->IASetInputLayout(m_inputLayout.Get());
					m_context->IASetVertexBuffers(0, 1, quadVertexBuffer.GetAddressOf(), &stride, &offset);
					
					m_stSprite.billboardMatrix = spriteMatrix;
					m_stSprite.color = spr.color;
					m_stSprite.isBillboard = true;
					m_cbSprite.updateData(m_stSprite, m_context.Get());
					BindConstantBufferVS(CB_SPRITE, m_cbSprite.get());

					m_context->Draw(4, 0);
					m_numDrawCalls++;
					m_numPolygons += 2;
					m_numSpritesDrawCalls++;

				}
				else if (spr.Type == RENDERER_SPRITE_TYPE::SPRITE_TYPE_BILLBOARD_LOOKAT)
				{
					UINT stride = sizeof(RendererVertex);
					UINT offset = 0;
					SetDepthState(DEPTH_STATE_READ_ONLY_ZBUFFER);
					SetCullMode(CULL_MODE_NONE);

					m_context->VSSetShader(m_vsSprites.Get(), NULL, 0);
					m_context->PSSetShader(m_psSprites.Get(), NULL, 0);

					SetAlphaTest(ALPHA_TEST_GREATER_THAN, ALPHA_TEST_THRESHOLD);

					m_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
					m_context->IASetInputLayout(m_inputLayout.Get());
					m_context->IASetVertexBuffers(0, 1, quadVertexBuffer.GetAddressOf(), &stride, &offset);
					
					m_stSprite.billboardMatrix = spriteMatrix;
					m_stSprite.color = spr.color;
					m_stSprite.isBillboard = true;
					m_cbSprite.updateData(m_stSprite, m_context.Get());
					BindConstantBufferVS(CB_SPRITE, m_cbSprite.get());

					m_context->Draw(4, 0);
					m_numDrawCalls++;
					m_numPolygons += 2;
					m_numSpritesDrawCalls++;

				}
				else if (spr.Type == RENDERER_SPRITE_TYPE::SPRITE_TYPE_3D) 
				{
					UINT stride = sizeof(RendererVertex);
					UINT offset = 0;
					SetDepthState(DEPTH_STATE_READ_ONLY_ZBUFFER);
					SetCullMode(CULL_MODE_NONE);

					m_context->VSSetShader(m_vsSprites.Get(), NULL, 0);
					m_context->PSSetShader(m_psSprites.Get(), NULL, 0);

					SetAlphaTest(ALPHA_TEST_GREATER_THAN, ALPHA_TEST_THRESHOLD);

					m_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
					m_context->IASetInputLayout(m_inputLayout.Get());
					m_context->IASetVertexBuffers(0, 1, quadVertexBuffer.GetAddressOf(), &stride, &offset);
					Vector3 p0t = spr.vtx1;
					Vector3 p1t = spr.vtx2;
					Vector3 p2t = spr.vtx3;
					Vector3 p3t = spr.vtx4;

					RendererVertex v0;
					v0.Position.x = p0t.x;
					v0.Position.y = p0t.y;
					v0.Position.z = p0t.z;
					v0.UV.x = spr.Sprite->UV[0].x;
					v0.UV.y = spr.Sprite->UV[0].y;
					v0.Color = spr.color;

					RendererVertex v1;
					v1.Position.x = p1t.x;
					v1.Position.y = p1t.y;
					v1.Position.z = p1t.z;
					v1.UV.x = spr.Sprite->UV[1].x;
					v1.UV.y = spr.Sprite->UV[1].y;
					v1.Color = spr.color;

					RendererVertex v2;
					v2.Position.x = p2t.x;
					v2.Position.y = p2t.y;
					v2.Position.z = p2t.z;
					v2.UV.x = spr.Sprite->UV[2].x;
					v2.UV.y = spr.Sprite->UV[2].y;
					v2.Color = spr.color;

					RendererVertex v3;
					v3.Position.x = p3t.x;
					v3.Position.y = p3t.y;
					v3.Position.z = p3t.z;
					v3.UV.x = spr.Sprite->UV[3].x;
					v3.UV.y = spr.Sprite->UV[3].y;
					v3.Color = spr.color;
					
					m_stSprite.color = spr.color;
					m_stSprite.isBillboard = false;
					m_cbSprite.updateData(m_stSprite, m_context.Get());
					BindConstantBufferVS(CB_SPRITE, m_cbSprite.get());

					m_primitiveBatch->Begin();
					m_primitiveBatch->DrawTriangle(v0, v1, v3);
					m_primitiveBatch->DrawTriangle(v1, v2, v3);
					m_primitiveBatch->End();

					m_numSpritesDrawCalls++;
					m_numSpritesDrawCalls++;
					m_numDrawCalls++;
				}
			}
		}

	}

	void Renderer11::DrawSpritesTransparent(RendererTransparentFaceInfo* info, RenderView& view)
	{	
		UINT stride = sizeof(RendererVertex);
		UINT offset = 0;

		m_context->VSSetShader(m_vsSprites.Get(), NULL, 0);
		m_context->PSSetShader(m_psSprites.Get(), NULL, 0);

		m_transparentFacesVertexBuffer.Update(m_context.Get(), m_transparentFacesVertices, 0, m_transparentFacesVertices.size());
		  
		m_context->IASetVertexBuffers(0, 1, m_transparentFacesVertexBuffer.Buffer.GetAddressOf(), &stride, &offset);
		m_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		m_context->IASetInputLayout(m_inputLayout.Get());

		m_stSprite.billboardMatrix = Matrix::Identity;
		m_stSprite.color = Vector4::One;
		m_stSprite.isBillboard = false;
		m_cbSprite.updateData(m_stSprite, m_context.Get());
		BindConstantBufferVS(CB_SPRITE, m_cbSprite.get());

		SetBlendMode(info->blendMode);
		SetDepthState(DEPTH_STATE_READ_ONLY_ZBUFFER);
		SetCullMode(CULL_MODE_NONE);
		SetAlphaTest(ALPHA_TEST_NONE, 0);

		BindTexture(TEXTURE_COLOR_MAP, info->sprite->Sprite->Texture, SAMPLER_LINEAR_CLAMP);

		DrawTriangles(m_transparentFacesVertices.size(), 0);

		m_numTransparentDrawCalls++;
		m_numSpritesTransparentDrawCalls++;
	}

	void Renderer11::DrawEffect(RenderView& view,RendererEffect* effect, bool transparent) 
	{
		UINT stride = sizeof(RendererVertex);
		UINT offset = 0;

		int firstBucket = (transparent ? 2 : 0);
		int lastBucket = (transparent ? 4 : 2);

		RendererRoom const & room = m_rooms[effect->Effect->roomNumber];
		//RendererObject * moveableObj = m_moveableObjects[effect->Effect->objectNumber];

		m_stItem.World = effect->World;
		m_stItem.Position = Vector4(effect->Effect->pos.Position.x, effect->Effect->pos.Position.y, effect->Effect->pos.Position.z, 1.0f);
		m_stItem.AmbientLight = room.AmbientLight;
		Matrix matrices[1] = { Matrix::Identity };
		memcpy(m_stItem.BonesMatrices, matrices, sizeof(Matrix));
		m_cbItem.updateData(m_stItem, m_context.Get());
		m_context->VSSetConstantBuffers(1, 1, m_cbItem.get());

		m_stLights.NumLights = effect->Lights.size();
		for (int j = 0; j < effect->Lights.size(); j++)
			memcpy(&m_stLights.Lights[j], effect->Lights[j], sizeof(ShaderLight));
		m_cbLights.updateData(m_stLights, m_context.Get());
		m_context->PSSetConstantBuffers(2, 1, m_cbLights.get());

		if (transparent)
		{
			SetAlphaTest(ALPHA_TEST_NONE, 1.0f);
		}
		else
		{
			SetAlphaTest(ALPHA_TEST_GREATER_THAN, ALPHA_TEST_THRESHOLD);
		}

		RendererMesh* mesh = effect->Mesh;

		for (auto& bucket : mesh->buckets) 
		{
			if (bucket.NumVertices == 0)
				continue;
			if (transparent && bucket.BlendMode == BLENDMODE_OPAQUE)
				continue;

			// Draw vertices
			DrawIndexedTriangles(bucket.NumIndices, bucket.StartIndex, 0);
		}

	}

	void Renderer11::DrawEffects(RenderView& view, bool transparent) 
	{
		UINT stride = sizeof(RendererVertex);
		UINT offset = 0;

		int firstBucket = (transparent ? 2 : 0);
		int lastBucket = (transparent ? 4 : 2);

		m_context->IASetVertexBuffers(0, 1, m_moveablesVertexBuffer.Buffer.GetAddressOf(), &stride, &offset);
		m_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		m_context->IASetInputLayout(m_inputLayout.Get());
		m_context->IASetIndexBuffer(m_moveablesIndexBuffer.Buffer.Get(), DXGI_FORMAT_R32_UINT, 0);

		for (auto room : view.roomsToDraw)
		{
			for (auto effect : room->EffectsToDraw)
			{
				RendererRoom const& room = m_rooms[effect->Effect->roomNumber];
				ObjectInfo* obj = &Objects[effect->Effect->objectNumber];

				if (obj->drawRoutine && obj->loaded)
					DrawEffect(view, effect, transparent);
			}
		}
	}

	void Renderer11::DrawDebris(RenderView& view,bool transparent)
	{		
		extern vector<DebrisFragment> DebrisFragments;
		vector<RendererVertex> vertices;

		for (auto deb = DebrisFragments.begin(); deb != DebrisFragments.end(); deb++)
		{
			if (deb->active) 
			{
				Matrix translation = Matrix::CreateTranslation(deb->worldPosition.x, deb->worldPosition.y, deb->worldPosition.z);
				Matrix rotation = Matrix::CreateFromQuaternion(deb->rotation);
				Matrix world = rotation * translation;

				m_primitiveBatch->Begin();
				m_context->VSSetShader(m_vsStatics.Get(), NULL, 0);
				m_context->PSSetShader(m_psStatics.Get(), NULL, 0);

				if (!deb->isStatic) 
				{
					BindTexture(TEXTURE_COLOR_MAP, &std::get<0>(m_staticsTextures[deb->mesh.tex]), SAMPLER_LINEAR_CLAMP);
				} 
				else 
				{
					BindTexture(TEXTURE_COLOR_MAP, &std::get<0>(m_moveablesTextures[deb->mesh.tex]), SAMPLER_LINEAR_CLAMP);
				}

				if (transparent)
				{
					SetAlphaTest(ALPHA_TEST_NONE, 1.0f);
				}
				else
				{
					SetAlphaTest(ALPHA_TEST_GREATER_THAN, ALPHA_TEST_THRESHOLD);
				}

				m_stStatic.World = world;
				m_stStatic.Color = Vector4::One;
				m_cbStatic.updateData(m_stStatic, m_context.Get());
				m_context->VSSetConstantBuffers(1, 1, m_cbStatic.get());

				RendererVertex vtx0;
				vtx0.Position = deb->mesh.Positions[0];
				vtx0.UV = deb->mesh.TextureCoordinates[0];
				vtx0.Normal = deb->mesh.Normals[0];
				vtx0.Color = m_rooms[deb->roomNumber].AmbientLight;

				RendererVertex vtx1;
				vtx1.Position = deb->mesh.Positions[1];
				vtx1.UV = deb->mesh.TextureCoordinates[1];
				vtx1.Normal = deb->mesh.Normals[1];
				vtx1.Color = m_rooms[deb->roomNumber].AmbientLight;

				RendererVertex vtx2;
				vtx2.Position = deb->mesh.Positions[2];
				vtx2.UV = deb->mesh.TextureCoordinates[2];
				vtx2.Normal = deb->mesh.Normals[2];
				vtx2.Color = m_rooms[deb->roomNumber].AmbientLight;

				SetCullMode(CULL_MODE_NONE);
				m_primitiveBatch->DrawTriangle(vtx0, vtx1, vtx2);
				m_numDrawCalls++;
				m_primitiveBatch->End();
			}
		}
	}

	void Renderer11::DrawSmokeParticles(RenderView& view)
	{
		using TEN::Effects::Smoke::SmokeParticles;
		using TEN::Effects::Smoke::SmokeParticle;

		for (int i = 0; i < SmokeParticles.size(); i++) 
		{
			SmokeParticle& s = SmokeParticles[i];
			if (!s.active) continue;
			AddSpriteBillboard(&m_sprites[Objects[ID_SMOKE_SPRITES].meshIndex + s.sprite], s.position, s.color, s.rotation, 1.0f, { s.size, s.size }, BLENDMODE_ALPHABLEND, view);
		}
	}

	void Renderer11::DrawSparkParticles(RenderView& view)
	{
		using TEN::Effects::Spark::SparkParticle;
		using TEN::Effects::Spark::SparkParticles;

		extern std::array<SparkParticle, 64> SparkParticles;

		for (int i = 0; i < SparkParticles.size(); i++) 
		{
			SparkParticle& s = SparkParticles[i];
			if (!s.active) continue;
			Vector3 v;
			s.velocity.Normalize(v);
			AddSpriteBillboardConstrained(&m_sprites[Objects[ID_SPARK_SPRITE].meshIndex], s.pos, s.color, 0, 1, { s.width, s.height }, BLENDMODE_ADDITIVE, -v, view);
		}
	}

	void Renderer11::DrawDripParticles(RenderView& view)
	{
		using TEN::Effects::Drip::DripParticle;
		using TEN::Effects::Drip::dripParticles;
		using TEN::Effects::Drip::DRIP_WIDTH;

		for (int i = 0; i < dripParticles.size(); i++) 
		{
			DripParticle& d = dripParticles[i];
			if (!d.active) continue;
			Vector3 v;
			d.velocity.Normalize(v);
			AddSpriteBillboardConstrained(&m_sprites[Objects[ID_DRIP_SPRITE].meshIndex], d.pos, d.color, 0, 1, { DRIP_WIDTH, d.height }, BLENDMODE_ADDITIVE, -v, view);
		}
	}

	void Renderer11::DrawExplosionParticles(RenderView& view)
	{
		using TEN::Effects::Explosion::explosionParticles;
		using TEN::Effects::Explosion::ExplosionParticle;

		for (int i = 0; i < explosionParticles.size(); i++) 
		{
			ExplosionParticle& e = explosionParticles[i];
			if (!e.active) continue;
			AddSpriteBillboard(&m_sprites[Objects[ID_EXPLOSION_SPRITES].meshIndex + e.sprite], e.pos, e.tint, e.rotation, 1.0f, { e.size, e.size }, BLENDMODE_ADDITIVE,view);
		}
	}

	void Renderer11::DrawSimpleParticles(RenderView& view)
	{
		using namespace TEN::Effects;

		for(SimpleParticle& s : simpleParticles)
		{
			if(!s.active) continue;
			AddSpriteBillboard(&m_sprites[Objects[s.sequence].meshIndex + s.sprite], s.worldPosition, Vector4(1, 1, 1, 1), 0, 1.0f, { s.size, s.size / 2 }, BLENDMODE_ALPHABLEND,view);
		}
	}

	DebrisFragment* getNewDebrisFragment()
	{
		return nullptr;

		/*DebrisFragment* fragment = GetFreeDebrisFragment();
		if (!fragment)
		{
			return nullptr;
		}

		if (!fragment->active)
		{
			Matrix rotationMatrix = Matrix::CreateFromYawPitchRoll(TO_RAD(yRot), 0, 0);
			RendererVertex vtx0 = meshVertices.at(renderBucket.Indices[i]);
			RendererVertex vtx1 = meshVertices.at(renderBucket.Indices[i + 1]);
			RendererVertex vtx2 = meshVertices.at(renderBucket.Indices[i + 2]);

			//Take the average of all 3 local positions
			Vector3 localPos = (vtx0.Position + vtx1.Position + vtx2.Position) / 3;
			vtx0.Position -= localPos;
			vtx1.Position -= localPos;
			vtx2.Position -= localPos;

			Vector3 worldPos = Vector3::Transform(localPos, rotationMatrix);
			fragment->worldPosition = worldPos + pos;
			fragment->mesh.vertices[0] = vtx0;
			fragment->mesh.vertices[1] = vtx1;
			fragment->mesh.vertices[2] = vtx2;
			fragment->mesh.blendMode = renderBucket.BlendMode;
			fragment->mesh.tex = renderBucket.Texture;
			fragment->isStatic = isStatic;
			fragment->active = true;
			fragment->terminalVelocity = 1024;
			fragment->gravity = Vector3(0, 7, 0);
			fragment->restitution = 0.6f;
			fragment->friction = 0.6f;
			fragment->linearDrag = .99f;
			fragment->angularVelocity = Vector3(GenerateFloat(-1, 1) * 0.39, GenerateFloat(-1, 1) * 0.39, GenerateFloat(-1, 1) * 0.39);
			fragment->angularDrag = GenerateFloat(0.9f, 0.999f);
			fragment->velocity = CalculateFragmentImpactVelocity(fragment->worldPosition, ShatterImpactData.impactDirection, ShatterImpactData.impactLocation);
			fragment->roomNumber = roomNumber;
			fragment->numBounces = 0;

			return fragment;
		}
		else
		{
			return nullptr;
		}*/
	}
}