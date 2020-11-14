#include "framework.h"
#include "explosion.h"
#include "trmath.h"
#include "spark.h"
#include "tomb4fx.h"
#include "setup.h"
#include "effect2.h"
#include "prng.h"
using namespace T5M::Math::Random;
namespace T5M {
	namespace Effects {
		namespace Explosion {
			using namespace DirectX::SimpleMath;
			using namespace T5M::Effects::Spark;
			std::array<ExplosionParticle, 64> explosionParticles;
			constexpr float PARTICLE_DISTANCE = 512;
			void TriggerExplosion(Vector3& pos, float size, bool triggerSparks, bool triggerSmoke, bool triggerShockwave, int room)
			{
				SpawnExplosionParticle(pos);
				for (int i = 0; i < 3; i++) {
					Vector3 particlePos = pos + Vector3(generateFloat(-size / 2, size / 2), generateFloat(-size / 2, size / 2), generateFloat(-size / 2, size / 2));
					SpawnExplosionParticle(particlePos);
				}
				if (triggerSparks) {
					PHD_VECTOR sparkPos;
					sparkPos.x = pos.x;
					sparkPos.y = pos.y;
					sparkPos.z = pos.z;
					//TriggerExplosionSparks(&sparkPos, room); @TODO
				}
				if (triggerShockwave) {
					PHD_3DPOS shockPos;
					shockPos.xPos = pos.x;
					shockPos.yPos = pos.y;
					shockPos.zPos = pos.z;
					TriggerShockwave(&shockPos, 0, size, 64, 32, 32, 32, 30, rand() & 0xFFFF, 0);
				}
			}
			void UpdateExplosionParticles()
			{
				for (int i = 0; i < explosionParticles.size(); i++) {
					ExplosionParticle& e = explosionParticles[i];
					if (!e.active) continue;
					e.age++;
					if (e.age > e.life) {
						e.active = false;
						continue;
					};
					e.vel *= 0.98f;
					e.pos += e.vel;
					e.angularVel *= 0.98f;
					e.rotation += e.angularVel;
					int numSprites = -Objects[ID_EXPLOSION_SPRITES].nmeshes - 1;
					float normalizedAge = e.age / e.life;
					e.sprite = lerp(0, numSprites, normalizedAge);
					e.tint = Vector4::Lerp(Vector4(2, 2, 2, 1), Vector4(0, 0, 0, 0), normalizedAge);

				}
			}
			ExplosionParticle& getFreeExplosionParticle()
			{
				for (int i = 0; i < explosionParticles.size(); i++)
				{
					if (!explosionParticles[i].active) {
						return explosionParticles[i];
					}
				}
				return explosionParticles[0];
			}
			void SpawnExplosionParticle(Vector3& pos)
			{
				ExplosionParticle& e = getFreeExplosionParticle();
				e = {};
				e.pos = pos;
				const float maxVel = 10;
				e.vel = Vector3(generateFloat(-maxVel / 2, maxVel / 2), generateFloat(-maxVel / 2, maxVel / 2), generateFloat(-maxVel / 2, maxVel / 2));
				e.active = true;
				e.tint = Vector4(1, 1, 1, 1);
				e.life = generateFloat(60, 90);
				e.size = generateFloat(512, 768);
				e.angularVel = generateFloat(-RADIAN, RADIAN);
				e.rotation = generateFloat(-0.05, 0.05);
			}
		}
	}
}