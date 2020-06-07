#include "framework.h"
#include "tr4_crocodile.h"
#include "box.h"
#include "effect.h"
#include "people.h"
#include "items.h"
#include "setup.h"
#include "level.h"
#include "lara.h"
#include "draw.h"
#include "misc.h"

typedef struct CROCODILE_BONE
{
	short torsoY;
	short torsoX;
	short hipsY;
	short hipsX;

	CROCODILE_BONE()
	{
		this->torsoY = 0;
		this->torsoX = 0;
		this->hipsY = 0;
		this->hipsX = 0;
	}

	CROCODILE_BONE(short angle)
	{
		this->torsoY = angle;
		this->torsoX = angle;
		this->hipsY = -angle;
		this->hipsX = -angle;
	}

	CROCODILE_BONE(short torsoY, short torsoX)
	{
		this->torsoY = torsoY;
		this->torsoX = torsoX;
		this->hipsY = 0;
		this->hipsX = 0;
	}
};

enum CROCODILE_STATE
{
	CROC_EMPTY,
	CROC_IDLE,
	CROC_RUN,
	CROC_WALK,
	CROC_HIT,
	CROC_ATK,
	CROC_EMPTY1,
	CROC_DIE,
	WCROC_SWIM,
	WCROC_ATK,
	WCROC_DIE,
};

#define CROC_WALK_ANGLE ANGLE(3.0f)
#define CROC_SWIM_ANGLE ANGLE(3.0f)
#define CROC_RUN_ANGLE ANGLE(5.0f)

constexpr auto CROC_ANIM_IDLE = 0;
constexpr auto CROC_ANIM_DIE = 11;
constexpr auto CROC_ANIM_SWIM = 12;
constexpr auto CROC_ANIM_WDIE = 16;
constexpr auto CROC_ALERT_RANGE = SQUARE(SECTOR(1) + CLICK(2));
constexpr auto CROC_VISIBILITY_RANGE = SQUARE(SECTOR(5));
constexpr auto CROC_RUN_RANGE = SQUARE(SECTOR(1));
constexpr auto CROC_MAXRUN_RANGE = SQUARE(SECTOR(1) + CLICK(2));
constexpr auto CROC_ATTACK_RANGE = SQUARE(CLICK(2)); // NOTE: TR4 is CLICK(3), but the crocodile not go near lara to do damage in certain case !
constexpr auto CROC_SWIM_SPEED = 16;
constexpr auto CROC_TOUCHBITS = 768;
constexpr auto CROC_DAMAGE = 120;
static BITE_INFO crocBite = { 0, -100, 500, 9 };

// crocodile mode (land or swim) transition anim
constexpr auto CROC_ANIM_SWIM_MODE = 17;
constexpr auto CROC_ANIM_LAND_MODE = 18;

static bool IsNextRoomAWaterRoom(ITEM_INFO* item)
{
    ROOM_INFO* room;
    GAME_VECTOR position;
    position.x = item->pos.xPos;
    position.y = item->pos.yPos;
    position.z = item->pos.zPos;
    position.roomNumber = item->roomNumber;
    GetFloor(position.x, position.y, position.z, &position.roomNumber);
    room = &Rooms[position.roomNumber];
    return CHK_EXI(room->flags, ENV_FLAG_WATER);
}

void InitialiseCrocodile(short itemNumber)
{
	ITEM_INFO* item = &Items[itemNumber];
	InitialiseCreature(itemNumber);

	// if the room is a "water room"
	if (Rooms[item->roomNumber].flags & ENV_FLAG_WATER)
	{
		item->animNumber = Objects[item->objectNumber].animIndex + CROC_ANIM_SWIM;
		item->frameNumber = Anims[item->animNumber].frameBase;
		item->currentAnimState = WCROC_SWIM;
		item->goalAnimState = WCROC_SWIM;
	}
	// then it's a "ground room"
	else
	{
		item->animNumber = Objects[item->objectNumber].animIndex + CROC_ANIM_IDLE;
		item->frameNumber = Anims[item->animNumber].frameBase;
		item->currentAnimState = CROC_IDLE;
		item->goalAnimState = CROC_IDLE;
	}
}

void CrocodileControl(short itemNumber)
{
    ITEM_INFO* item;
    ObjectInfo* obj;
    GAME_VECTOR transitionLand, transitionWater;
    CREATURE_INFO* crocodile;
    AI_INFO info;
    CROCODILE_BONE boneRot;
    short angle;
    short bone_angle;
    bool nextRoomIsWater = false;

    item = &Items[itemNumber];
    obj = &Objects[item->objectNumber];
    crocodile = GetCreatureInfo(item);
    angle = 0;
    bone_angle = 0;

    if (item->hitPoints <= 0)
    {
        angle = 0;
        bone_angle = 0;

        if (item->currentAnimState != CROC_DIE && item->currentAnimState != WCROC_DIE)
        {
            // water
            if (Rooms[item->roomNumber].flags & ENV_FLAG_WATER)
            {
                item->animNumber = obj->animIndex + CROC_ANIM_WDIE;
                item->frameNumber = Anims[item->animNumber].frameBase;
                item->currentAnimState = WCROC_DIE;
                item->goalAnimState = WCROC_DIE;
            }
            // land
            else
            {
                item->animNumber = obj->animIndex + CROC_ANIM_DIE;
                item->frameNumber = Anims[item->animNumber].frameBase;
                item->currentAnimState = CROC_DIE;
                item->goalAnimState = CROC_DIE;
            }
        }

        // creature in water are floating after death.
        if (Rooms[item->roomNumber].flags & ENV_FLAG_WATER)
            CreatureFloat(itemNumber);
    }
    else
    {
        if (item->aiBits & ALL_AIOBJ)
            GetAITarget(crocodile);
        else if (crocodile->hurtByLara)
            crocodile->enemy = LaraItem;

        CreatureAIInfo(item, &info);

        if (item->currentAnimState < WCROC_SWIM)
        {
            crocodile->LOT.step = CLICK(1);
            crocodile->LOT.drop = -CLICK(1);
            crocodile->LOT.fly = NO_FLYING;
            CreatureUnderwater(item, CLICK(0));
        }
        else
        {
            crocodile->LOT.step = SECTOR(20);
            crocodile->LOT.drop = -SECTOR(20);
            crocodile->LOT.fly = CROC_SWIM_SPEED;
            CreatureUnderwater(item, CLICK(1));
        }

        GetCreatureMood(item, &info, VIOLENT);
        CreatureMood(item, &info, VIOLENT);
        angle = CreatureTurn(item, crocodile->maximumTurn);

        if ((item->hitStatus || info.distance < CROC_ALERT_RANGE) || (TargetVisible(item, &info) && info.distance < CROC_VISIBILITY_RANGE))
        {
            if (!crocodile->alerted)
                crocodile->alerted = TRUE;
            AlertAllGuards(itemNumber);
        }

        bone_angle = angle << 2;
        switch (item->currentAnimState)
        {
        case CROC_IDLE:
            crocodile->maximumTurn = 0;

            if (item->aiBits & GUARD)
            {
                bone_angle = item->itemFlags[0];
                item->goalAnimState = CROC_IDLE;
                item->itemFlags[0] = item->itemFlags[1] + bone_angle;

                if (!(GetRandomControl() & 0x1F))
                {
                    if (GetRandomControl() & 1)
                        item->itemFlags[1] = 0;
                    else
                        item->itemFlags[1] = (GetRandomControl() & 1) != 0 ? 12 : -12;
                }

                CLAMP(item->itemFlags[0], -1024, 1024);
            }
            else if (info.bite && info.distance < CROC_ATTACK_RANGE)
            {
                item->goalAnimState = CROC_ATK;
            }
            else
            {
                if (info.ahead && info.distance < CROC_RUN_RANGE)
                    item->goalAnimState = CROC_WALK;
                else
                    item->goalAnimState = CROC_RUN;
            }
            break;
        case CROC_WALK:
            crocodile->maximumTurn = CROC_WALK_ANGLE;

            if (IsNextRoomAWaterRoom(item))
            {
                // water to land transition:
                item->requiredAnimState = WCROC_SWIM;
                item->goalAnimState = WCROC_SWIM;
            }

            if (item->requiredAnimState)
            {
                item->goalAnimState = item->requiredAnimState;
            }
            else if (info.bite && info.distance < CROC_ATTACK_RANGE)
            {
                item->goalAnimState = CROC_IDLE;
            }
            else if (!info.ahead || info.distance > CROC_MAXRUN_RANGE)
            {
                item->goalAnimState = CROC_RUN;
            }
            break;
        case CROC_RUN:
            crocodile->maximumTurn = CROC_RUN_ANGLE;

            if (IsNextRoomAWaterRoom(item))
            {
                // water to land transition:
                item->requiredAnimState = CROC_WALK;
                item->goalAnimState = CROC_WALK;
            }

            if (item->requiredAnimState)
            {
                item->goalAnimState = item->requiredAnimState;
            }
            else if (info.bite && info.distance < CROC_ATTACK_RANGE)
            {
                item->goalAnimState = CROC_IDLE;
            }
            else if (info.ahead && info.distance < CROC_RUN_RANGE)
            {
                item->goalAnimState = CROC_WALK;
            }
            break;
        case CROC_ATK:
            if (item->frameNumber == Anims[item->animNumber].frameBase)
                item->requiredAnimState = 0;

            if (info.bite && (item->touchBits & CROC_TOUCHBITS))
            {
                if (!item->requiredAnimState)
                {
                    CreatureEffect2(item, &crocBite, 10, -1, DoBloodSplat);
                    LaraItem->hitPoints -= CROC_DAMAGE;
                    LaraItem->hitStatus = TRUE;
                    item->requiredAnimState = CROC_IDLE;
                }
            }
            else
            {
                item->goalAnimState = CROC_IDLE;
            }
            break;
        case WCROC_SWIM:
            crocodile->maximumTurn = CROC_SWIM_ANGLE;

            if (!IsNextRoomAWaterRoom(item))
            {
                // water to land transition:
                item->requiredAnimState = CROC_WALK;
                item->goalAnimState = CROC_WALK;
            }

            if (item->requiredAnimState)
            {
                item->goalAnimState = item->requiredAnimState;
            }
            else if (info.bite)
            {
                if (item->touchBits & CROC_TOUCHBITS)
                    item->goalAnimState = WCROC_ATK;
            }
            break;
        case WCROC_ATK:
            if (item->frameNumber == Anims[item->animNumber].frameBase)
                item->requiredAnimState = CROC_EMPTY;

            if (info.bite && (item->touchBits & CROC_TOUCHBITS))
            {
                if (!item->requiredAnimState)
                {
                    CreatureEffect2(item, &crocBite, 10, -1, DoBloodSplat);
                    LaraItem->hitPoints -= CROC_DAMAGE;
                    LaraItem->hitStatus = TRUE;
                    item->requiredAnimState = WCROC_SWIM;
                }
            }
            else
            {
                item->goalAnimState = WCROC_SWIM;
            }
            break;
        }
    }

    if (item->currentAnimState == CROC_IDLE || item->currentAnimState == CROC_ATK || item->currentAnimState == WCROC_ATK)
        boneRot = CROCODILE_BONE(info.angle, info.xAngle);
    else
        boneRot = CROCODILE_BONE(bone_angle);
    CreatureTilt(item, 0);
    CreatureJoint(item, 0, boneRot.torsoY);
    CreatureJoint(item, 1, boneRot.torsoX);
    CreatureJoint(item, 2, boneRot.hipsY);
    CreatureJoint(item, 3, boneRot.hipsX);
    if (item->currentAnimState < WCROC_SWIM)
        CalcItemToFloorRotation(item, 2);
    CreatureAnimation(itemNumber, angle, 0);
}