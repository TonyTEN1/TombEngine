#include "framework.h"
#include "tr3_flamethrower.h"
#include "control/box.h"
#include "effects/effects.h"
#include "people.h"
#include "items.h"
#include "animation.h"
#include "setup.h"
#include "control/lot.h"
#include "level.h"
#include "lara.h"
#include "Sound/sound.h"
#include "itemdata/creature_info.h"
#include "traps.h"

BITE_INFO flamerBite = { 0, 340, 64, 7 };

static void TriggerPilotFlame(int itemnum)
{
	int dx = LaraItem->pos.xPos - g_Level.Items[itemnum].pos.xPos;
	int dz = LaraItem->pos.zPos - g_Level.Items[itemnum].pos.zPos;

	if (dx < -16384 || dx > 16384 || dz < -16384 || dz > 16384)
		return;

	SPARKS* spark = &Sparks[GetFreeSpark()];

	spark->on = 1;
	spark->sR = 48 + (GetRandomControl() & 31);
	spark->sG = spark->sR;
	spark->sB = 192 + (GetRandomControl() & 63);

	spark->dR = 192 + (GetRandomControl() & 63);
	spark->dG = 128 + (GetRandomControl() & 63);
	spark->dB = 32;

	spark->colFadeSpeed = 12 + (GetRandomControl() & 3);
	spark->fadeToBlack = 4;
	spark->sLife = spark->life = (GetRandomControl() & 3) + 20;
	spark->transType = COLADD;
	spark->extras = 0;
	spark->dynamic = -1;
	spark->x = (GetRandomControl() & 31) - 16;
	spark->y = (GetRandomControl() & 31) - 16;
	spark->z = (GetRandomControl() & 31) - 16;

	spark->xVel = (GetRandomControl() & 31) - 16;
	spark->yVel = -(GetRandomControl() & 3);
	spark->zVel = (GetRandomControl() & 31) - 16;

	spark->flags = SP_SCALE | SP_DEF | SP_EXPDEF | SP_ITEM | SP_NODEATTACH;
	spark->fxObj = itemnum;
	spark->nodeNumber = 9;
	spark->friction = 4;
	spark->gravity = -(GetRandomControl() & 3) - 2;
	spark->maxYvel = -(GetRandomControl() & 3) - 4;
	//spark->def = Objects[EXPLOSION1].mesh_index;
	SparkSpriteSequence(spark, ID_FIRE_SPRITES);
	spark->scalar = 0;
	int size = (GetRandomControl() & 7) + 32;
	spark->size = size / 2;
	spark->dSize = size;
}

static void TriggerFlamethrowerFlame(int x, int y, int z, int xv, int yv, int zv, int fxnum)
{
	SPARKS* spark = &Sparks[GetFreeSpark()];

	spark->on = true;
	spark->sR = 48 + (GetRandomControl() & 31);
	spark->sG = spark->sR;
	spark->sB = 192 + (GetRandomControl() & 63);

	spark->dR = 192 + (GetRandomControl() & 63);
	spark->dG = 128 + (GetRandomControl() & 63);
	spark->dB = 32;

	if (true/*xv || yv || zv*/)
	{
		spark->colFadeSpeed = 6;
		spark->fadeToBlack = 2;
		spark->sLife = spark->life = (GetRandomControl() & 1) + 12;
	}
	/*
	else
	{
		spark->colFadeSpeed = 8;
		spark->fadeToBlack = 16;
		spark->sLife = spark->life = (GetRandomControl() & 3) + 20;
	}
	*/

	spark->transType = COLADD;

	spark->extras = 0;
	spark->dynamic = -1;

	spark->x = x + ((GetRandomControl() & 31) - 16);
	spark->y = y;
	spark->z = z + ((GetRandomControl() & 31) - 16);

	spark->xVel = ((GetRandomControl() & 15) - 16) + xv;
	spark->yVel = yv;
	spark->zVel = ((GetRandomControl() & 15) - 16) + zv;
	spark->friction = 0;

	if (GetRandomControl() & 1)
	{
		if (fxnum >= 0)
			spark->flags = SP_SCALE | SP_DEF | SP_ROTATE | SP_EXPDEF | SP_FX;
		else
			spark->flags = SP_SCALE | SP_DEF | SP_ROTATE | SP_EXPDEF;
		spark->rotAng = GetRandomControl() & 4095;
		if (GetRandomControl() & 1)
			spark->rotAdd = -(GetRandomControl() & 15) - 16;
		else
			spark->rotAdd = (GetRandomControl() & 15) + 16;
	}
	else
	{
		if (fxnum >= 0)
			spark->flags = SP_SCALE | SP_DEF | SP_EXPDEF | SP_FX;
		else
			spark->flags = SP_SCALE | SP_DEF | SP_EXPDEF;
	}

	spark->fxObj = fxnum;
	spark->gravity = 0;
	spark->maxYvel = 0;
	int size = (GetRandomControl() & 31) + 64;

	if (true/*xv || yv || zv*/)
	{
		spark->size = size / 32;
		if (fxnum == -2)
			spark->scalar = 2;
		else
			spark->scalar = 3;
	}
	/*
	else
	{
		spark->size = size / 16;
		spark->scalar = 4;
	}
	*/
	spark->dSize = size;
	SparkSpriteSequence(spark, ID_FIRE_SPRITES);
}

static short TriggerFlameThrower(ITEM_INFO* item, BITE_INFO* bite, short speed)
{
	PHD_VECTOR pos1;
	PHD_VECTOR pos2;
	short angles[2];
	int velocity;
	int xv;
	int yv;
	int zv;

	short effectNumber = CreateNewEffect(item->roomNumber,ID_FLAME,item->pos);
	if (effectNumber != NO_ITEM)
	{
		ITEM_INFO* fx = &g_Level.Items[effectNumber];
		FX_INFO* fxInfo = fx->data;
		pos1.x = bite->x;
		pos1.y = bite->y;
		pos1.z = bite->z;
		GetJointAbsPosition(item, &pos1, bite->meshNum);

		pos2.x = bite->x;
		pos2.y = bite->y / 2;
		pos2.z = bite->z;
		GetJointAbsPosition(item, &pos2, bite->meshNum);

		phd_GetVectorAngles(pos1.x - pos2.x, pos1.y - pos2.y, pos1.z - pos2.z, angles);

		fx->pos.xPos = pos1.x;
		fx->pos.yPos = pos1.y;
		fx->pos.zPos = pos1.z;

		fx->roomNumber = item->roomNumber;

		fx->pos.xRot = angles[1];
		fx->pos.zRot = 0;
		fx->pos.yRot = angles[0];
		fx->speed = speed * 4;
		fxInfo->counter = 20;
		fxInfo->flag1 = 0;
		//using new ID_FLAME projectile instead
		//TriggerFlamethrowerFlame(0, 0, 0, 0, 0, 0, effectNumber);

		for (int i = 0; i < 2; i++)
		{
			speed = (GetRandomControl() % (speed * 4)) + 32;
			velocity = speed * phd_cos(fx->pos.xRot);

			xv = velocity * phd_sin(fx->pos.yRot);
			yv = -speed * phd_sin(fx->pos.xRot);
			zv = velocity * phd_cos(fx->pos.yRot);

			//TriggerFlamethrowerFlame(fx->pos.xPos, fx->pos.yPos, fx->pos.zPos, xv * 32, yv * 32, zv * 32, -1);
		}

		velocity = (speed * 2) * phd_cos(fx->pos.xRot);
		
		xv = velocity * phd_sin(fx->pos.yRot);
		yv = -(speed * 2) * phd_sin(fx->pos.xRot);
		zv = velocity * phd_cos(fx->pos.yRot);
		fx->currentAnimState = FLAME_STATES::FLAME_PROJECTILE_STATE;
		//TriggerFlamethrowerFlame(fx->pos.xPos, fx->pos.yPos, fx->pos.zPos, xv * 32, yv * 32, zv * 32, -2);
	}

	return effectNumber;
}

void FlameThrowerControl(short itemNumber)
{
	if (!CreatureActive(itemNumber))
		return;

	ITEM_INFO* item = &g_Level.Items[itemNumber];
	CREATURE_INFO* creature = (CREATURE_INFO*)item->data;

	short angle = 0;
	short tilt = 0;
	short torsoX = 0;
	short torsoY = 0;
	short head = 0;

	PHD_VECTOR pos;
	pos.x = flamerBite.x;
	pos.y = flamerBite.y;
	pos.z = flamerBite.z;
	GetJointAbsPosition(item, &pos, flamerBite.meshNum);

	int random = GetRandomControl();
	if (item->currentAnimState != 6 && item->currentAnimState != 11)
	{
 		TriggerDynamicLight(pos.x, pos.y, pos.z, (random & 3) + 6, 24 - ((random / 16) & 3), 16 - ((random / 64) & 3), random & 3); 
		TriggerPilotFlame(itemNumber);
	}
	else
		TriggerDynamicLight(pos.x, pos.y, pos.z, (random & 3) + 10, 31 - ((random / 16) & 3), 24 - ((random / 64) & 3), random & 7);  

	if (item->hitPoints <= 0)
	{
		if (item->currentAnimState != 7)
		{
			item->animNumber = Objects[item->objectNumber].animIndex + 19;
			item->frameNumber = g_Level.Anims[item->animNumber].frameBase;
			item->currentAnimState = 7;
		}
	}
	else
	{
		if (item->aiBits)
			GetAITarget(creature);
		else if (creature->hurtByLara)
			creature->enemy = LaraItem;
		else
		{
			creature->enemy = NULL;

			int minDistance = 0x7FFFFFFF;
			ITEM_INFO* target = NULL;

			for (int i = 0; i < ActiveCreatures.size(); i++)
			{
				CREATURE_INFO* currentCreature = ActiveCreatures[i];
				if (currentCreature->itemNum == NO_ITEM || currentCreature->itemNum == itemNumber)
					continue;

				target = &g_Level.Items[currentCreature->itemNum];
				if (target->objectNumber == ID_LARA || target->hitPoints <= 0)
					continue;

				int x = target->pos.xPos - item->pos.xPos;
				int z = target->pos.zPos - item->pos.zPos;

				int distance = SQUARE(x) + SQUARE(z);

				if (distance < minDistance)
				{
					creature->enemy = target;
					minDistance = distance;
				}
			}
		}

		AI_INFO info;
		AI_INFO laraInfo;

		CreatureAIInfo(item, &info);

		if (creature->enemy == LaraItem)
		{
			laraInfo.angle = info.angle;
			laraInfo.distance = info.distance;
			if (!creature->hurtByLara)
				creature->enemy = NULL;
		}
		else
		{
			int dx = LaraItem->pos.xPos - item->pos.xPos;
			int dz = LaraItem->pos.zPos - item->pos.zPos;
			
			laraInfo.angle = phd_atan(dz, dz) - item->pos.yRot; 
			laraInfo.distance = SQUARE(dx) + SQUARE(dz);
			
			info.xAngle -= 0x800;
		}

		GetCreatureMood(item, &info, VIOLENT);
		CreatureMood(item, &info, VIOLENT);

		angle = CreatureTurn(item, creature->maximumTurn);

		ITEM_INFO* realEnemy = creature->enemy; 

		if (item->hitStatus || laraInfo.distance < SQUARE(1024) || TargetVisible(item, &laraInfo)) 
		{
			if (!creature->alerted)
				SoundEffect(300, &item->pos, 0);
			AlertAllGuards(itemNumber);
		}
		
		switch (item->currentAnimState)
		{
		case 1:
			head = laraInfo.angle;

			creature->flags = 0;
			creature->maximumTurn = 0;

			if (item->aiBits & GUARD)
			{
				head = AIGuard(creature);
				if (!(GetRandomControl() & 0xFF))
					item->goalAnimState = 4;
				break;
			}
			else if (item->aiBits & PATROL1)
				item->goalAnimState = 2;
			else if (creature->mood == ESCAPE_MOOD)
				item->goalAnimState = 2;
			else if (Targetable(item, &info) && (realEnemy != LaraItem || creature->hurtByLara))
			{
				if (info.distance < SQUARE(4096))
					item->goalAnimState = 10;
				else
					item->goalAnimState = 2;
			}
			else if (creature->mood == BORED_MOOD && info.ahead && !(GetRandomControl() & 0xFF))
				item->goalAnimState = 4;
			else if (creature->mood == ATTACK_MOOD || !(GetRandomControl() & 0xFF))
				item->goalAnimState = 2;
			
			break;

		case 4:
			head = laraInfo.angle;

			if (item->aiBits & GUARD)
			{
				head = AIGuard(creature);

				if (!(GetRandomControl() & 0xFF))
					item->goalAnimState = 1;
				break;
			}
			else if ((Targetable(item, &info) && 
				info.distance < SQUARE(4096) && 
				(realEnemy != LaraItem || creature->hurtByLara) || 
				creature->mood != BORED_MOOD || 
				!(GetRandomControl() & 0xFF)))
				item->goalAnimState = 1;

			break;

		case 2:
			head = laraInfo.angle;

			creature->flags = 0;
			creature->maximumTurn = ANGLE(5);

			if (item->aiBits & GUARD)
			{
				item->animNumber = Objects[item->objectNumber].animIndex + 12;
				item->frameNumber = g_Level.Anims[item->animNumber].frameBase;
				item->currentAnimState = 1;
				item->goalAnimState = 1;
			}
			else if (item->aiBits & PATROL1)
				item->goalAnimState = 2;
			else if (creature->mood == ESCAPE_MOOD)
				item->goalAnimState = 2;
			else if (Targetable(item, &info) && 
				(realEnemy != LaraItem || creature->hurtByLara))
			{
				if (info.distance < SQUARE(4096))
					item->goalAnimState = 1;
				else
					item->goalAnimState = 9;
			}
			else if (creature->mood == BORED_MOOD && info.ahead)
				item->goalAnimState = 1;
			else
				item->goalAnimState = 2;

			break;

		case 10:
			creature->flags = 0;

			if (info.ahead)
			{
				torsoY = info.angle;
				torsoX = info.xAngle;

				if (Targetable(item, &info) && 
					info.distance < SQUARE(4096) && 
					(realEnemy != LaraItem || creature->hurtByLara))
					item->goalAnimState = 11;
				else
					item->goalAnimState = 1;
			}
			break;

		case 9:
			creature->flags = 0;

			if (info.ahead)
			{
				torsoY = info.angle;
				torsoX = info.xAngle;

				if (Targetable(item, &info) && 
					info.distance < SQUARE(4096) && 
					(realEnemy != LaraItem || creature->hurtByLara))
					item->goalAnimState = 6;
				else
					item->goalAnimState = 2;
			}

			break;

		case 11:
			if (creature->flags < 40)
				creature->flags += (creature->flags / 4) + 1;

			if (info.ahead)
			{
				torsoY = info.angle;
				torsoX = info.xAngle;
				if (Targetable(item, &info) && 
					info.distance < SQUARE(4096) && 
					(realEnemy != LaraItem || creature->hurtByLara))
					item->goalAnimState = 11;
				else
					item->goalAnimState = 1;
			}
			else
				item->goalAnimState = 1;

			if (creature->flags < 40)
				TriggerFlameThrower(item, &flamerBite, creature->flags);
			else
			{
				TriggerFlameThrower(item, &flamerBite, (GetRandomControl() & 31) + 12);
				if (realEnemy)
				{
					/*code*/
				}
			}

			SoundEffect(204, &item->pos, 0);

			break;
			
		case 6:
			if (creature->flags < 40)
				creature->flags += (creature->flags / 4) + 1;

			if (info.ahead)
			{
				torsoY = info.angle;
				torsoX = info.xAngle;
				if (Targetable(item, &info) && 
					info.distance < SQUARE(4096) && 
					(realEnemy != LaraItem || creature->hurtByLara))
					item->goalAnimState = 6;
				else
					item->goalAnimState = 2;
			}
			else
				item->goalAnimState = 2;

			if (creature->flags < 40)
				TriggerFlameThrower(item, &flamerBite, creature->flags);
			else
			{
				TriggerFlameThrower(item, &flamerBite, (GetRandomControl() & 31) + 12);
				if (realEnemy)
				{
					/*code*/
				}
			}

			SoundEffect(204, &item->pos, 0);

			break;
		}
	}

	CreatureTilt(item, tilt);
	CreatureJoint(item, 0, torsoY);
	CreatureJoint(item, 1, torsoX);
	CreatureJoint(item, 2, head);

	CreatureAnimation(itemNumber, angle, 0);
}