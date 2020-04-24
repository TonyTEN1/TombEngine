#include "../newobjects.h"
#include "../../Game/Box.h"
#include "../../Game/people.h"
#include "../../Game/items.h"
#include "../../Game/missile.h"
#include "..\..\Specific\level.h"
#include <effects.h>
#include "../../Game/sound.h"
#include "../../Game/effects.h"

/* HIT POINTS for Natla when she enters her second stage */
#define NATLA_NEAR_DEATH 200

enum natla_anims {
	NATLA_EMPTY, NATLA_STOP, NATLA_FLY, NATLA_RUN, NATLA_AIM, NATLA_SEMIDEATH, NATLA_SHOOT, NATLA_FALL, NATLA_STAND, NATLA_DEATH
};

BITE_INFO natla_gun = { 5, 220, 7, 4 };

#define NATLA_FLYMODE 0x8000
#define NATLA_TIMER   0x7fff
#define NATLA_FIRE_ARC ANGLE(30)
#define NATLA_FLY_TURN ANGLE(5)
#define NATLA_RUN_TURN ANGLE(6)
#define NATLA_LAND_CHANCE 0x100
#define NATLA_DIE_TIME 30*16
#define NATLA_SHOT_DAMAGE 100

void NatlaControl(short itemNum)
{
	if (!CreatureActive(itemNum))
		return;

	ITEM_INFO* item;
	FX_INFO* fx;
	CREATURE_INFO* natla;
	AI_INFO info;
	int shoot;
	short angle, head, tilt, gun;
	short facing = 0;
	short fx_number, timer;

	item = &Items[itemNum];
	natla = (CREATURE_INFO*)item->data;
	head = angle = tilt = 0;
	gun = natla->jointRotation[0] * 7 / 8;
	timer = natla->flags & NATLA_TIMER;

	if (item->hitPoints <= 0 && item->hitPoints > -16384)
	{
		item->goalAnimState = NATLA_DEATH;
	}
	else if (item->hitPoints <= NATLA_NEAR_DEATH)
	{
		natla->LOT.step = STEP_SIZE;
		natla->LOT.drop = -STEP_SIZE;
		natla->LOT.fly = NO_FLYING;
		CreatureAIInfo(item, &info);

		if (info.ahead)
			head = info.angle;

		GetCreatureMood(item, &info, VIOLENT);
		CreatureMood(item, &info, VIOLENT);

		angle = CreatureTurn(item, NATLA_RUN_TURN);
		shoot = (info.angle > -NATLA_FIRE_ARC && info.angle < NATLA_FIRE_ARC && Targetable(item, &info));

		/* Reset facing */
		if (facing)
		{
			item->pos.yRot += facing;
			facing = 0;
		}

		switch (item->currentAnimState)
		{
		case NATLA_FALL:
			if (item->pos.yPos < item->floor)
			{
				item->gravityStatus = true;
				item->speed = 0;
			}
			else
			{
				item->gravityStatus = 0;
				item->goalAnimState = NATLA_SEMIDEATH;
				item->pos.yPos = item->floor;
				timer = 0;
			}
			break;

		case NATLA_STAND:
			if (!shoot)
				item->goalAnimState = NATLA_RUN;

			if (timer >= 20)
			{
				fx_number = CreatureEffect(item, &natla_gun, ShardGun);
				if (fx_number != NO_ITEM)
				{
					fx = &Effects[fx_number];
					gun = fx->pos.xRot;
					SoundEffect(123, &fx->pos, NULL);
				}
				timer = 0;
			}
			break;

		case NATLA_RUN:
			tilt = angle;

			if (timer >= 20)
			{
				fx_number = CreatureEffect(item, &natla_gun, ShardGun);
				if (fx_number != NO_ITEM)
				{
					fx = &Effects[fx_number];
					gun = fx->pos.xRot;
					SoundEffect(123, &fx->pos, NULL);
				}
				timer = 0;
			}

			if (shoot)
				item->goalAnimState = NATLA_STAND;
			break;

		case NATLA_SEMIDEATH:
			if (timer == NATLA_DIE_TIME)
			{
				item->goalAnimState = NATLA_STAND;
				natla->flags = 0;
				timer = 0;
				item->hitPoints = NATLA_NEAR_DEATH;
				S_CDPlay(54, 0);
			}
			else
				item->hitPoints = -16384;
			break;

		case NATLA_FLY:
			item->goalAnimState = NATLA_FALL;
			timer = 0;
			break;

		case NATLA_STOP:
		case NATLA_SHOOT:
		case NATLA_AIM:
			item->goalAnimState = NATLA_SEMIDEATH;
			item->flags = 0;
			timer = 0;
			break;
		}
	}
	else
	{
		natla->LOT.step = STEP_SIZE;
		natla->LOT.drop = -STEP_SIZE;
		natla->LOT.fly = NO_FLYING;
		CreatureAIInfo(item, &info);

		shoot = (info.angle > -NATLA_FIRE_ARC && info.angle < NATLA_FIRE_ARC && Targetable(item, &info));

		if (item->currentAnimState == NATLA_FLY && (natla->flags & NATLA_FLYMODE))
		{
			if ((natla->flags & NATLA_FLYMODE) && shoot && GetRandomControl() < NATLA_LAND_CHANCE)
				natla->flags -= NATLA_FLYMODE;

			if (!(natla->flags & NATLA_FLYMODE))
				CreatureMood(item, &info, VIOLENT);

			natla->LOT.step = WALL_SIZE * 20;
			natla->LOT.drop = -WALL_SIZE * 20;
			natla->LOT.fly = STEP_SIZE / 8;
			CreatureAIInfo(item, &info);
		}
		else if (!shoot)
			natla->flags |= NATLA_FLYMODE;

		if (info.ahead)
			head = info.angle;

		if (item->currentAnimState != NATLA_FLY || (natla->flags & NATLA_FLYMODE))
			CreatureMood(item, &info, TIMID);

		item->pos.yRot -= facing;
		angle = CreatureTurn(item, NATLA_FLY_TURN);

		/* Adjust facing if flying, else reset */
		if (item->currentAnimState == NATLA_FLY)
		{
			if (info.angle > NATLA_FLY_TURN)
				facing += NATLA_FLY_TURN;
			else if (info.angle < -NATLA_FLY_TURN)
				facing -= NATLA_FLY_TURN;
			else
				facing += info.angle;
			item->pos.yRot += facing;
		}
		else
		{
			item->pos.yRot += facing - angle;
			facing = 0;
		}

		switch (item->currentAnimState)
		{
		case NATLA_STOP:
			timer = 0;
			if (natla->flags & NATLA_FLYMODE)
				item->goalAnimState = NATLA_FLY;
			else
				item->goalAnimState = NATLA_AIM;
			break;

		case NATLA_FLY:
			if (!(natla->flags & NATLA_FLYMODE) && item->pos.yPos == item->floor)
				item->goalAnimState = NATLA_STOP;

			if (timer >= 30)
			{
				fx_number = CreatureEffect(item, &natla_gun, BombGun);
				if (fx_number != NO_ITEM)
				{
					fx = &Effects[fx_number];
					gun = fx->pos.xRot;
					SoundEffect(123, &fx->pos, NULL);
				}
				timer = 0;
			}
			break;

		case NATLA_AIM:
			if (item->requiredAnimState)
				item->goalAnimState = item->requiredAnimState;
			else if (shoot)
				item->goalAnimState = NATLA_SHOOT;
			else
				item->goalAnimState = NATLA_STOP;
			break;

		case NATLA_SHOOT:
			if (!item->requiredAnimState)
			{
				/* Eat this ... */
				fx_number = CreatureEffect(item, &natla_gun, BombGun);
				if (fx_number != NO_ITEM)
					gun = Effects[fx_number].pos.xRot;
				fx_number = CreatureEffect(item, &natla_gun, BombGun);
				if (fx_number != NO_ITEM)
					Effects[fx_number].pos.yRot += (short)((GetRandomControl() - 0x4000) / 4);
				fx_number = CreatureEffect(item, &natla_gun, BombGun);
				if (fx_number != NO_ITEM)
					Effects[fx_number].pos.yRot += (short)((GetRandomControl() - 0x4000) / 4);

				item->requiredAnimState = NATLA_STOP;
			}
			break;
		}
	}

	CreatureTilt(item, tilt);

	/* Aim Natla's gun arm */
	CreatureJoint(item, 0, -head);
	if (gun)
		CreatureJoint(item, 0, gun);

	timer++;
	natla->flags = (natla->flags & NATLA_FLYMODE) + timer;

	/* Actually do animation allowing for collisions */
	item->pos.yRot -= facing;
	CreatureAnimation(itemNum, angle, tilt);
	item->pos.yRot += facing;
}