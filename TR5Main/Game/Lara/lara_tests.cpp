#include "framework.h"
#include "lara.h"
#include "lara_tests.h"
#include "input.h"
#include "draw.h"
#include "control.h"
#include "lara_climb.h"
#include "lara_collide.h"
#include "control.h"

using namespace TEN::Floordata;

static short LeftClimbTab[4] = // offset 0xA0638
{
	0x0200, 0x0400, 0x0800, 0x0100
};

static short RightClimbTab[4] = // offset 0xA0640
{
	0x0800, 0x0100, 0x0200, 0x0400
};

/*this file has all the generic test functions called in lara's state code*/

int TestLaraVault(ITEM_INFO* item, COLL_INFO* coll)
{
	if (!(TrInput & IN_ACTION) || Lara.gunStatus != LG_NO_ARMS)
		return 0;

	//##LUA debug etc.
//	Lara.NewAnims.CrawlVault1click = 1;
//	Lara.NewAnims.CrawlVault2click = 1;
//	Lara.NewAnims.CrawlVault3click = 1;
//	Lara.NewAnims.MonkeyVault = 1;

	if (coll->CollisionType == CT_FRONT)
	{
		short angle = item->pos.yRot;
		bool result;

		/*if (coll->Middle.SplitFloor)
		{
			if (coll->Front.SplitFloor != coll->Middle.SplitFloor)
				return 0;
			result = SnapToDiagonal(angle, 30);
		}
		else*/
		{
			result = SnapToQuadrant(angle, 30);
		}

		if (!result)
			return 0;

		int slope = abs(coll->FrontLeft.Floor - coll->FrontRight.Floor) >= 60;

		if (coll->Front.Floor < 0 && coll->Front.Floor >= -256)
		{
			if (!slope && (abs(coll->Front.Ceiling - coll->Front.Floor) < 256) && Lara.NewAnims.CrawlVault1click)
			{
				item->animNumber = LA_VAULT_TO_CROUCH_1CLICK;
				item->currentAnimState = LS_GRABBING;
				item->frameNumber = g_Level.Anims[item->animNumber].frameBase;
				item->goalAnimState = LS_CROUCH_IDLE;
				item->pos.yPos += coll->Front.Floor + 256;
				Lara.gunStatus = LG_HANDS_BUSY;
			}
		}
		else if (coll->Front.Floor >= -640 && coll->Front.Floor <= -384)
		{
			if (!slope &&
				coll->Front.Floor - coll->Front.Ceiling >= 0 &&
				coll->FrontLeft.Floor - coll->FrontLeft.Ceiling >= 0 &&
				coll->FrontRight.Floor - coll->FrontRight.Ceiling >= 0)
			{
#if 0
				if (g_Level.Rooms[item->roomNumber].flags & ENV_FLAG_SWAMP && Lara.waterSurfaceDist < -768)
					return 0;
#endif

				item->animNumber = LA_VAULT_TO_STAND_2CLICK_START;
				item->currentAnimState = LS_GRABBING;
				item->frameNumber = g_Level.Anims[item->animNumber].frameBase;
				item->goalAnimState = LS_STOP;
				item->pos.yPos += coll->Front.Floor + 512;
				Lara.gunStatus = LG_HANDS_BUSY;
			}
			else if ((!slope && (abs(coll->Front.Ceiling - coll->Front.Floor) < 256)) && Lara.NewAnims.CrawlVault2click)
			{
				item->animNumber = LA_VAULT_TO_CROUCH_2CLICK;
				item->frameNumber = g_Level.Anims[item->animNumber].frameBase;
				item->currentAnimState = LS_GRABBING;
				item->goalAnimState = LS_CROUCH_IDLE;
				item->pos.yPos += coll->Front.Floor + 512;
				Lara.gunStatus = LG_HANDS_BUSY;
			}
			else
			{
				return 0;
			}
		}
		else if (coll->Front.Floor >= -896 && coll->Front.Floor <= -640)
		{
			if (!slope &&
				coll->Front.Floor - coll->Front.Ceiling >= 0 &&
				coll->FrontLeft.Floor - coll->FrontLeft.Ceiling >= 0 &&
				coll->FrontRight.Floor - coll->FrontRight.Ceiling >= 0)
			{
#if 0
				if (g_Level.Rooms[item->roomNumber].flags & ENV_FLAG_SWAMP && Lara.waterSurfaceDist < -768)
					return 0;
#endif

				item->animNumber = LA_VAULT_TO_STAND_3CLICK;
				item->currentAnimState = LS_GRABBING;
				item->frameNumber = g_Level.Anims[item->animNumber].frameBase;
				item->goalAnimState = LS_STOP;
				item->pos.yPos += coll->Front.Floor + 768;
				Lara.gunStatus = LG_HANDS_BUSY;
			}
			else if ((!slope && (abs(coll->Front.Ceiling - coll->Front.Floor) < 256)) && Lara.NewAnims.CrawlVault3click)
			{
				item->animNumber = LA_VAULT_TO_CROUCH_3CLICK;
				item->frameNumber = g_Level.Anims[item->animNumber].frameBase;
				item->currentAnimState = LS_GRABBING;
				item->goalAnimState = LS_CROUCH_IDLE;
				item->pos.yPos += coll->Front.Floor + 768;
				Lara.gunStatus = LG_HANDS_BUSY;
			}
			else
			{
				return 0;
			}
		}
		else if (!slope && coll->Front.Floor >= -1920 && coll->Front.Floor <= -896)
		{
#if 0
			if (g_Level.Rooms[item->roomNumber].flags & ENV_FLAG_SWAMP)
				return 0;
#endif

			item->animNumber = LA_STAND_SOLID;
			item->frameNumber = g_Level.Anims[item->animNumber].frameBase;
			item->goalAnimState = LS_JUMP_UP;
			item->currentAnimState = LS_STOP;
			Lara.calcFallSpeed = -3 - sqrt(-9600 - 12 * coll->Front.Floor);
			AnimateLara(item);
		}
		else
		{
			if (!Lara.climbStatus)
				return 0;

			if (coll->Front.Floor > -1920 || Lara.waterStatus == LW_WADE || coll->FrontLeft.Floor > -1920 || coll->FrontRight.Floor > -2048 || coll->Middle.Ceiling > -1158)
			{
				if ((coll->Front.Floor < -1024 || coll->Front.Ceiling >= 506) && coll->Middle.Ceiling <= -518)
				{
					ShiftItem(item, coll);

					if (LaraTestClimbStance(item, coll))
					{
						item->animNumber = LA_STAND_SOLID;
						item->frameNumber = g_Level.Anims[item->animNumber].frameBase;
						item->goalAnimState = LS_LADDER_IDLE;
						item->currentAnimState = LS_STOP;
						AnimateLara(item);
						item->pos.yRot = angle;
						Lara.gunStatus = LG_HANDS_BUSY;
						return 1;
					}
				}
				return 0;
			}

			item->animNumber = LA_STAND_SOLID;
			item->frameNumber = g_Level.Anims[item->animNumber].frameBase;
			item->goalAnimState = LS_JUMP_UP;
			item->currentAnimState = LS_STOP;
			Lara.calcFallSpeed = -116;
			AnimateLara(item);
		}

		item->pos.yRot = angle;
		ShiftItem(item, coll);

		/*if (coll->Middle.SplitFloor) // diagonal alignment
		{
			Vector2 v = GetDiagonalIntersect(item->pos.xPos, item->pos.zPos, coll->Middle.SplitFloor, LARA_RAD, item->pos.yRot);
			item->pos.xPos = v.x;
			item->pos.zPos = v.y;
		}
		else*/ // regular aligment
		{
			Vector2 v = GetOrthogonalIntersect(item->pos.xPos, item->pos.zPos, LARA_RAD, item->pos.yRot);
			item->pos.xPos = v.x;
			item->pos.zPos = v.y;
		}
		return 1;
	}
	else if (Lara.NewAnims.MonkeyVault)//gross
	{
		if (Lara.canMonkeySwing)
		{
			short roomNum = item->roomNumber;
			int ceiling = (GetCeiling(GetFloor(item->pos.xPos, item->pos.yPos, item->pos.zPos, &roomNum),
				item->pos.xPos, item->pos.yPos, item->pos.zPos))-(item->pos.yPos);

			if (ceiling > 1792 || ceiling < -1792 || abs(ceiling) == 768)
				return 0;

			item->animNumber = LA_STAND_IDLE;
			item->frameNumber = g_Level.Anims[LA_STAND_IDLE].frameBase;
			item->goalAnimState = LS_JUMP_UP;
			item->currentAnimState = LS_TEST_1;
			return 1;
		}
	}
}

bool TestLaraStandUp(COLL_INFO* coll)
{
	return (coll->Middle.Ceiling >= -362 || coll->ObjectHeadroom < (LARA_HEIGHT - LARA_HEIGHT_CRAWL) + LARA_HEADROOM);
}

int TestWall(ITEM_INFO* item, int front, int right, int down)
{
	int x = item->pos.xPos;
	int y = item->pos.yPos + down;
	int z = item->pos.zPos;

	short angle = GetQuadrant(item->pos.yRot);
	short roomNum = item->roomNumber;

	FLOOR_INFO* floor;
	int h, c;

	switch (angle)
	{
	case NORTH:
		x -= right;
		break;
	case EAST:
		z -= right;
		break;
	case SOUTH:
		x += right;
		break;
	case WEST:
		z += right;
		break;
	default:
		break;
	}

	GetFloor(x, y, z, &roomNum);

	switch (angle)
	{
	case NORTH:
		z += front;
		break;
	case EAST:
		x += front;
		break;
	case SOUTH:
		z -= front;
		break;
	case WEST:
		x -= front;
		break;
	default:
		break;
	}

	floor = GetFloor(x, y, z, &roomNum);
	h = GetFloorHeight(floor, x, y, z);
	c = GetCeiling(floor, x, y, z);

	if (h == NO_HEIGHT)
		return 1;

	if (y >= h || y <= c)
		return 2;

	return 0;
}

int LaraHangTest(ITEM_INFO* item, COLL_INFO* coll)
{
	int delta, flag, flag2, front, dfront, x, z, result;
	short angle, hdif, cdif, dir;
	ANIM_FRAME* frame;

	delta = 0;
	flag = 0;
	angle = Lara.moveAngle;
	if (angle == (short) (item->pos.yRot - ANGLE(90)))
	{
		delta = -100;
	}
	else if (angle == (short) (item->pos.yRot + ANGLE(90)))
	{
		delta = 100;
	}
	hdif = LaraFloorFront(item, angle, 100);
	if (hdif < 200)
		flag = 1;
	cdif = LaraCeilingFront(item, angle, 100, 0);
	dir = GetQuadrant(item->pos.yRot);
	switch (dir)
	{
	case NORTH:
		item->pos.zPos += 4;
		break;
	case EAST:
		item->pos.xPos += 4;
		break;
	case SOUTH:
		item->pos.zPos -= 4;
		break;
	case WEST:
		item->pos.xPos -= 4;
		break;
	}
	coll->Setup.BadHeightUp = NO_BAD_POS;
	coll->Setup.BadHeightDown = -STEPUP_HEIGHT;
	coll->Setup.BadCeilingHeight = 0;
	Lara.moveAngle = item->pos.yRot;
	coll->Setup.ForwardAngle = Lara.moveAngle;
	GetCollisionInfo(coll, item->pos.xPos, item->pos.yPos, item->pos.zPos, item->roomNumber, LARA_HEIGHT);
	result = 0;
	if (Lara.climbStatus)
	{
		if (TrInput & IN_ACTION && item->hitPoints > 0)
		{
			Lara.moveAngle = angle;
			if (!LaraTestHangOnClimbWall(item, coll))
			{
				if (item->animNumber != LA_LADDER_TO_HANG_RIGHT && item->animNumber != LA_LADDER_TO_HANG_LEFT)
				{
					SnapLaraToEdgeOfBlock(item, coll, dir);
					item->pos.yPos = coll->Setup.OldPosition.y;
					item->currentAnimState = LS_HANG;
					item->goalAnimState = LS_HANG;
					item->animNumber = LA_REACH_TO_HANG;
					item->frameNumber = g_Level.Anims[item->animNumber].frameBase + 21;
				}
				result = 1;
			}
			else
			{
				if (item->animNumber == LA_REACH_TO_HANG && item->frameNumber == g_Level.Anims[LA_REACH_TO_HANG].frameBase + 21 && LaraTestClimbStance(item, coll))
					item->goalAnimState = LS_LADDER_IDLE;
			}
		}
		else
		{
			item->animNumber = LA_FALL_START;
			item->currentAnimState = LS_JUMP_FORWARD;
			item->goalAnimState = LS_JUMP_FORWARD;
			item->frameNumber = g_Level.Anims[item->animNumber].frameBase;
			item->pos.yPos += 256;
			item->gravityStatus = true;
			item->speed = 2;
			item->fallspeed = 1;
			Lara.gunStatus = LG_NO_ARMS;
		}
	}
	else
	{
		if (TrInput & IN_ACTION && item->hitPoints > 0 && coll->Front.Floor <= 0)
		{
			if (flag && hdif > 0 && delta > 0 == coll->MiddleLeft.Floor > coll->MiddleRight.Floor)
				flag = 0;
			frame = (ANIM_FRAME*)GetBoundsAccurate(item);
			front = coll->Front.Floor;
			dfront = coll->Front.Floor - frame->boundingBox.Y1;
			flag2 = 0;
			x = item->pos.xPos;
			z = item->pos.zPos;
			switch (dir)
			{
			case NORTH:
				x += delta;
				break;
			case EAST:
				z -= delta;
				break;
			case SOUTH:
				x -= delta;
				break;
			case WEST:
				z += delta;
				break;
			}
			Lara.moveAngle = angle;
			if (256 << dir & GetClimbFlags(x, item->pos.yPos, z, item->roomNumber))
			{
				if (!LaraTestHangOnClimbWall(item, coll))
					dfront = 0;
			}
			else if (abs(coll->FrontLeft.Floor - coll->FrontRight.Floor) >= 60)
			{
				if (delta < 0 && coll->FrontLeft.Floor != coll->Front.Floor || delta > 0 && coll->FrontRight.Floor != coll->Front.Floor)
					flag2 = 1;
			}
			coll->Front.Floor = front;
			if (!flag2 && coll->Middle.Ceiling < 0 && coll->CollisionType == CT_FRONT && !flag && !coll->HitStatic && cdif <= -950 && dfront >= -60 && dfront <= 60)
			{
				switch (dir)
				{
				case NORTH:
				case SOUTH:
					item->pos.zPos += coll->Shift.z;
					break;
				case EAST:
				case WEST:
					item->pos.xPos += coll->Shift.x;
					break;
				}
				item->pos.yPos += dfront;
			}
			else
			{
				item->pos.xPos = coll->Setup.OldPosition.x;
				item->pos.yPos = coll->Setup.OldPosition.y;
				item->pos.zPos = coll->Setup.OldPosition.z;
				if (item->currentAnimState == LS_SHIMMY_LEFT || item->currentAnimState == LS_SHIMMY_RIGHT)
				{
					item->currentAnimState = LS_HANG;
					item->goalAnimState = LS_HANG;
					item->animNumber = LA_REACH_TO_HANG;
					item->frameNumber = g_Level.Anims[item->animNumber].frameBase + 21;
				}
				else if (item->currentAnimState == LS_SHIMMY_FEET_LEFT || item->currentAnimState == LS_SHIMMY_FEET_RIGHT)
				{
					item->currentAnimState = LS_HANG_FEET;
					item->goalAnimState = LS_HANG_FEET;
					item->animNumber = LA_HANG_FEET_IDLE;
					item->frameNumber = g_Level.Anims[item->animNumber].frameBase;
				}
				result = 1;
			}
		}
		else
		{
			item->currentAnimState = LS_JUMP_UP;
			item->goalAnimState = LS_JUMP_UP;
			item->animNumber = LA_JUMP_UP;
			item->frameNumber = g_Level.Anims[item->animNumber].frameBase + 9;
			frame = (ANIM_FRAME*)GetBoundsAccurate(item);
			item->pos.xPos += coll->Shift.x;
			item->pos.yPos += frame->boundingBox.Y2;
			item->pos.zPos += coll->Shift.z;
			item->gravityStatus = true;
			item->speed = 2;
			item->fallspeed = 1;
			Lara.gunStatus = LG_NO_ARMS;
		}
	}
	return result;
}

int LaraHangLeftCornerTest(ITEM_INFO* item, COLL_INFO* coll)
{
	if (item->animNumber != LA_REACH_TO_HANG && item->animNumber != LA_HANG_FEET_IDLE)
		return 0;

	if (coll->HitStatic)
		return 0;

	int x;
	int z;

	int oldXpos = item->pos.xPos;
	int oldZpos = item->pos.zPos;
	short oldYrot = item->pos.yRot;
	int oldFrontFloor = coll->Front.Floor;

	short angle = GetQuadrant(item->pos.yRot);
	if (angle != NORTH && angle != SOUTH)
	{
		x = item->pos.xPos ^ (item->pos.xPos ^ item->pos.zPos) & 0x3FF;
		z = item->pos.zPos ^ (item->pos.xPos ^ item->pos.zPos) & 0x3FF;
	}
	else
	{
		x = (item->pos.xPos & 0xFFFFFC00) - (item->pos.zPos & 0x3FF) + SECTOR(1);
		z = (item->pos.zPos & 0xFFFFFC00) - (item->pos.xPos & 0x3FF) + SECTOR(1);
	}

	item->pos.xPos = x;
	Lara.cornerX = x;
	item->pos.zPos = z;
	Lara.cornerZ = z;
	item->pos.yRot -= ANGLE(90.0f);

	int result = -IsValidHangPos(item, coll);
	if (result)
	{
		if (Lara.climbStatus)
		{
			if (GetClimbFlags(x, item->pos.yPos, z, item->roomNumber) & RightClimbTab[angle])
			{
				item->pos.xPos = oldXpos;
				item->pos.zPos = oldZpos;
				item->pos.yRot = oldYrot;
				Lara.moveAngle = oldYrot;
				return result;
			}
		}
		else
		{
			if (abs(oldFrontFloor - coll->Front.Floor) <= 60)
			{
				item->pos.xPos = oldXpos;
				item->pos.zPos = oldZpos;
				item->pos.yRot = oldYrot;
				Lara.moveAngle = oldYrot;
				return result;
			}
		}
	}

	item->pos.xPos = oldXpos;
	item->pos.zPos = oldZpos;
	item->pos.yRot = oldYrot;
	Lara.moveAngle = oldYrot;

	if (LaraFloorFront(item, oldYrot - ANGLE(90.0f), 116) < 0)
		return 0;

	switch (angle)
	{
	case NORTH:
		x = ((item->pos.xPos ^ item->pos.zPos) & 0x3FF) ^ item->pos.xPos - SECTOR(1);
		z = ((item->pos.xPos ^ item->pos.zPos) & 0x3FF) ^ item->pos.zPos + SECTOR(1);
		break;

	case SOUTH:
		x = ((item->pos.xPos ^ item->pos.zPos) & 0x3FF) ^ (item->pos.xPos + SECTOR(1));
		z = ((item->pos.xPos ^ item->pos.zPos) & 0x3FF) ^ (item->pos.zPos - SECTOR(1));
		break;

	case WEST:
		x = (item->pos.xPos & 0xFFFFFC00) - (item->pos.zPos & 0x3FF);
		z = (item->pos.zPos & 0xFFFFFC00) - (item->pos.xPos & 0x3FF);
		break;

	default:
		x = ((item->pos.xPos + SECTOR(1)) & 0xFFFFFC00) - (item->pos.zPos & 0x3FF) + SECTOR(1);
		z = ((item->pos.zPos + SECTOR(1)) & 0xFFFFFC00) - (item->pos.xPos & 0x3FF) + SECTOR(1);
		break;

	}

	item->pos.xPos = x;
	Lara.cornerX = x;
	item->pos.zPos = z;
	Lara.cornerZ = z;
	item->pos.yRot += ANGLE(90.0f);

	result = IsValidHangPos(item, coll);
	if (!result)
	{
		item->pos.xPos = oldXpos;
		item->pos.zPos = oldZpos;
		item->pos.yRot = oldYrot;
		Lara.moveAngle = oldYrot;
		return result;
	}

	item->pos.xPos = oldXpos;
	item->pos.zPos = oldZpos;
	item->pos.yRot = oldYrot;
	Lara.moveAngle = oldYrot;

	if (!Lara.climbStatus)
	{
		if (abs(oldFrontFloor - coll->Front.Floor) <= 60)
		{
			switch (angle)
			{
			case NORTH:
				if ((oldXpos & 0x3FF) > 512)
					result = 0;
				break;
			case EAST:
				if ((oldZpos & 0x3FF) < 512)
					result = 0;
				break;
			case SOUTH:
				if ((oldXpos & 0x3FF) < 512)
					result = 0;
				break;
			case WEST:
				if ((oldZpos & 0x3FF) > 512)
					result = 0;
				break;
			}
			return result;
		}
		return 0;
	}

	if (GetClimbFlags(x, item->pos.yPos, z, item->roomNumber) & LeftClimbTab[angle])
		return result;

	short front = LaraFloorFront(item, item->pos.yRot, 116);
	if (abs(front - coll->Front.Floor) > 60)
		return 0;

	if (front < -768)
		return 0;

	return result;
}

int LaraHangRightCornerTest(ITEM_INFO* item, COLL_INFO* coll)
{
	if (item->animNumber != LA_REACH_TO_HANG && item->animNumber != LA_HANG_FEET_IDLE)
		return 0;

	if (coll->HitStatic)
		return 0;

	int x;
	int z;

	int oldXpos = item->pos.xPos;
	int oldZpos = item->pos.zPos;
	short oldYrot = item->pos.yRot;
	int oldFrontFloor = coll->Front.Floor;

	short angle = GetQuadrant(item->pos.yRot);
	if (angle != NORTH && angle != SOUTH)
	{
		x = (item->pos.xPos & 0xFFFFFC00) - (item->pos.zPos & 0x3FF) + SECTOR(1);
		z = (item->pos.zPos & 0xFFFFFC00) - (item->pos.xPos & 0x3FF) + SECTOR(1);
	}
	else
	{
		x = item->pos.xPos ^ (item->pos.xPos ^ item->pos.zPos) & 0x3FF;
		z = item->pos.zPos ^ (item->pos.xPos ^ item->pos.zPos) & 0x3FF;
	}

	item->pos.xPos = x;
	Lara.cornerX = x;
	item->pos.zPos = z;
	Lara.cornerZ = z;
	item->pos.yRot += ANGLE(90.0f);

	int result = -IsValidHangPos(item, coll);
	if (result)
	{
		if (Lara.climbStatus)
		{
			if (GetClimbFlags(x, item->pos.yPos, z, item->roomNumber) & LeftClimbTab[angle])
			{
				item->pos.xPos = oldXpos;
				item->pos.zPos = oldZpos;
				item->pos.yRot = oldYrot;
				Lara.moveAngle = oldYrot;
				return result;
			}
		}
		else
		{
			if (abs(oldFrontFloor - coll->Front.Floor) <= 60)
			{
				item->pos.xPos = oldXpos;
				item->pos.zPos = oldZpos;
				item->pos.yRot = oldYrot;
				Lara.moveAngle = oldYrot;
				return result;
			}
		}
	}

	item->pos.xPos = oldXpos;
	item->pos.zPos = oldZpos;
	item->pos.yRot = oldYrot;
	Lara.moveAngle = oldYrot;

	if (LaraFloorFront(item, oldYrot + ANGLE(90.0f), 116) < 0)
		return 0;

	switch (angle)
	{
	case NORTH:
		x = ((item->pos.xPos + SECTOR(1)) & 0xFFFFFC00) - (item->pos.zPos & 0x3FF) + SECTOR(1);
		z = ((item->pos.zPos + SECTOR(1)) & 0xFFFFFC00) - (item->pos.xPos & 0x3FF) + SECTOR(1);
		break;

	case SOUTH:
		x = ((item->pos.xPos - SECTOR(1)) & 0xFFFFFC00) - (item->pos.zPos & 0x3FF) + SECTOR(1);
		z = ((item->pos.zPos - SECTOR(1)) & 0xFFFFFC00) - (item->pos.xPos & 0x3FF) + SECTOR(1);
		break;

	case WEST:
		x = (item->pos.xPos ^ item->pos.zPos) & 0x3FF ^ (item->pos.xPos - SECTOR(1));
		z = (item->pos.xPos ^ item->pos.zPos) & 0x3FF ^ (item->pos.zPos + SECTOR(1));
		break;

	default:
		x = ((item->pos.xPos ^ item->pos.zPos) & 0x3FF) ^ (item->pos.xPos + SECTOR(1));
		z = ((item->pos.xPos ^ item->pos.zPos) & 0x3FF) ^ (item->pos.zPos - SECTOR(1));
		break;

	}

	item->pos.xPos = x;
	Lara.cornerX = x;
	item->pos.zPos = z;
	Lara.cornerZ = z;
	item->pos.yRot -= ANGLE(90.0f);

	result = IsValidHangPos(item, coll);
	if (!result)
	{
		item->pos.xPos = oldXpos;
		item->pos.zPos = oldZpos;
		item->pos.yRot = oldYrot;
		Lara.moveAngle = oldYrot;
		return result;
	}

	item->pos.xPos = oldXpos;
	item->pos.zPos = oldZpos;
	item->pos.yRot = oldYrot;
	Lara.moveAngle = oldYrot;

	if (!Lara.climbStatus)
	{
		if (abs(oldFrontFloor - coll->Front.Floor) <= 60)
		{
			switch (angle)
			{
			case NORTH:
				if ((oldXpos & 0x3FF) < 512)
					result = 0;
				break;
			case EAST:
				if ((oldZpos & 0x3FF) > 512)
					result = 0;
				break;
			case SOUTH:
				if ((oldXpos & 0x3FF) > 512)
					result = 0;
				break;
			case WEST:
				if ((oldZpos & 0x3FF) < 512)
					result = 0;
				break;
			}
			return result;
		}
		return 0;
	}

	if (GetClimbFlags(x, item->pos.yPos, z, item->roomNumber) & RightClimbTab[angle])
		return result;

	short front = LaraFloorFront(item, item->pos.yRot, 116);
	if (abs(front - coll->Front.Floor) > 60)
		return 0;

	if (front < -768)
		return 0;

	return result;
}

int IsValidHangPos(ITEM_INFO* item, COLL_INFO* coll)
{
	if (LaraFloorFront(item, Lara.moveAngle, 100) < 200)
		return 0;

	short angle = GetQuadrant(item->pos.yRot);
	switch (angle)
	{
	case NORTH:
		item->pos.zPos += 4;
		break;
	case EAST:
		item->pos.xPos += 4;
		break;
	case SOUTH:
		item->pos.zPos -= 4;
		break;
	case WEST:
		item->pos.xPos -= 4;
		break;
	default:
		break;
	}

	coll->Setup.BadHeightUp = NO_BAD_POS;
	coll->Setup.BadHeightDown = -512;
	coll->Setup.BadCeilingHeight = 0;

	Lara.moveAngle = item->pos.yRot;

	coll->Setup.ForwardAngle = Lara.moveAngle;
	GetCollisionInfo(coll, item->pos.xPos, item->pos.yPos, item->pos.zPos, item->roomNumber, LARA_HEIGHT);

	if (coll->Middle.Ceiling >= 0 || coll->CollisionType != CT_FRONT || coll->HitStatic)
		return 0;

	return abs(coll->Front.Floor - coll->FrontRight.Floor) < 60;
}

int LaraTestClimbStance(ITEM_INFO* item, COLL_INFO* coll)
{
	int shift_r, shift_l;

	if (LaraTestClimbPos(item, coll->Setup.Radius, coll->Setup.Radius + 120, -700, 512, &shift_r) != 1)
		return false;

	if (LaraTestClimbPos(item, coll->Setup.Radius, -(coll->Setup.Radius + 120), -700, 512, &shift_l) != 1)
		return false;

	if (shift_r)
	{
		if (shift_l)
		{
			if (shift_r < 0 != shift_l < 0)
				return false;

			if ((shift_r < 0 && shift_l < shift_r) ||
				(shift_r > 0 && shift_l > shift_r))
			{
				item->pos.yPos += shift_l;
				return true;
			}
		}

		item->pos.yPos += shift_r;
	}
	else if (shift_l)
	{
		item->pos.yPos += shift_l;
	}

	return true;
}

int LaraTestHangOnClimbWall(ITEM_INFO* item, COLL_INFO* coll)
{
	BOUNDING_BOX* bounds;
	int shift, result;

	if (Lara.climbStatus == 0)
		return 0;

	if (item->fallspeed < 0)
		return 0;

	switch (GetQuadrant(item->pos.yRot))
	{
	case NORTH:
	case SOUTH:
		item->pos.zPos += coll->Shift.z;
		break;

	case EAST:
	case WEST:
		item->pos.xPos += coll->Shift.x;
		break;

	default:
		break;
	}


	bounds = GetBoundsAccurate(item);

	if (Lara.moveAngle != item->pos.yRot)
	{
		short l = LaraCeilingFront(item, item->pos.yRot, 0, 0);
		short r = LaraCeilingFront(item, Lara.moveAngle, 128, 0);

		if (abs(l - r) > 60)
			return 0;
	}

	if (LaraTestClimbPos(item, coll->Setup.Radius, coll->Setup.Radius, bounds->Y1, bounds->Y2 - bounds->Y1, &shift) &&
		LaraTestClimbPos(item, coll->Setup.Radius, -coll->Setup.Radius, bounds->Y1, bounds->Y2 - bounds->Y1, &shift))
	{
		result = LaraTestClimbPos(item, coll->Setup.Radius, 0, bounds->Y1, bounds->Y2 - bounds->Y1, &shift);
		if (result)
		{
			if (result != 1)
				item->pos.yPos += shift;
			return 1;
		}
	}

	return 0;
}

int LaraTestEdgeCatch(ITEM_INFO* item, COLL_INFO* coll, int* edge)
{

	BOUNDING_BOX* bounds = GetBoundsAccurate(item);
	int hdif = coll->Front.Floor - bounds->Y1;

	if (hdif < 0 == hdif + item->fallspeed < 0)
	{
		hdif = item->pos.yPos + bounds->Y1;

		if ((hdif + item->fallspeed & 0xFFFFFF00) != (hdif & 0xFFFFFF00))
		{
			if (item->fallspeed > 0)
				*edge = (hdif + item->fallspeed) & 0xFFFFFF00;
			else
				*edge = hdif & 0xFFFFFF00;

			return -1;
		}

		return 0;
	}

	if (abs(coll->FrontLeft.Floor - coll->FrontRight.Floor) >= SLOPE_DIF)
		return 0;

	return 1;
}

int TestHangSwingIn(ITEM_INFO* item, short angle)
{
	int x = item->pos.xPos;
	int y = item->pos.yPos;
	int z = item->pos.zPos;
	short roomNum = item->roomNumber;
	FLOOR_INFO* floor;
	int h, c;

	//debug till scripting be ready
	Lara.NewAnims.OscillateHanging = 0;

	z += phd_cos(angle) * STEP_SIZE;
	x += phd_sin(angle) * STEP_SIZE;

	floor = GetFloor(x, y, z, &roomNum);
	h = GetFloorHeight(floor, x, y, z);
	c = GetCeiling(floor, x, y, z);

	if (h != NO_HEIGHT)
	{
		if (Lara.NewAnims.OscillateHanging)
		{
			if (h - y > 0 && c - y < -400)
				return 1;
		}
		else
		{
			if (h - y > 0 && c - y < -400 && (y - 819 - c > -72))
				return 1;
		}
	}

	return 0;
}

bool TestHangFeet(ITEM_INFO* item, short angle)
{
	//##LUA debug etc.
	Lara.NewAnims.FeetHanging = 0;

	if (Lara.climbStatus || !Lara.NewAnims.FeetHanging)
		return 0;

	int x = item->pos.xPos;
	int y = item->pos.yPos;
	int z = item->pos.zPos;
	short roomNum = item->roomNumber;
	FLOOR_INFO* floor;
	int h, c, g, m, j;

	z += phd_cos(angle) * STEP_SIZE;
	x += phd_sin(angle) * STEP_SIZE;

	floor = GetFloor(x, y, z, &roomNum);
	h = GetFloorHeight(floor, x, y, z);
	c = GetCeiling(floor, x, y, z);
	g = h - y;
	m = c - y;
	j = y - 128 - c;

	if (h != NO_HEIGHT)
	{
		if (g > 0 && m < -128 && j > -72)
			return 1;
	}
}

int CanLaraHangSideways(ITEM_INFO* item, COLL_INFO* coll, short angle)
{
	int oldx = item->pos.xPos;
	int oldz = item->pos.zPos;
	int x = item->pos.xPos;
	int z = item->pos.zPos;
	int res;

	Lara.moveAngle = item->pos.yRot + angle;

	switch (GetQuadrant(Lara.moveAngle))
	{
	case 0:
		z += 16;
		break;
	case 1:
		x += 16;
		break;
	case 2:
		z -= 16;
		break;
	case 3:
		x -= 16;
		break;
	}

	item->pos.xPos = x;
	item->pos.zPos = z;

	coll->Setup.OldPosition.y = item->pos.yPos;

	res = LaraHangTest(item, coll);

	item->pos.xPos = oldx;
	item->pos.zPos = oldz;

	Lara.moveAngle = item->pos.yRot + angle;

	return !res;
}

void SetCornerAnim(ITEM_INFO* item, COLL_INFO* coll, short rot, short flip)
{
	if (item->hitPoints <= 0)
	{
		item->goalAnimState = LS_JUMP_FORWARD;
		item->currentAnimState = LS_JUMP_FORWARD;
		item->animNumber = LA_FALL_START;
		item->frameNumber = g_Level.Anims[item->animNumber].frameBase;

		item->gravityStatus = true;
		item->speed = 2;
		item->pos.yPos += 256;
		item->fallspeed = 1;

		Lara.gunStatus = LG_NO_ARMS;

		item->pos.yRot += rot / 2;
	}
	else if (flip)
	{
		if (Lara.isClimbing)
		{
			item->animNumber = LA_LADDER_IDLE;
			item->frameNumber = g_Level.Anims[item->animNumber].frameBase;
			item->goalAnimState = LS_LADDER_IDLE;
			item->currentAnimState = LS_LADDER_IDLE;
		}
		else
		{
			item->animNumber = LA_REACH_TO_HANG;
			item->frameNumber = g_Level.Anims[item->animNumber].frameBase + 21;
			item->goalAnimState = LS_HANG;
			item->currentAnimState = LS_HANG;
		}

		coll->Setup.OldPosition.x = Lara.cornerX;
		item->pos.xPos = Lara.cornerX;

		coll->Setup.OldPosition.z = Lara.cornerZ;
		item->pos.zPos = Lara.cornerZ;

		item->pos.yRot += rot;
	}
}

void SetCornerAnimFeet(ITEM_INFO* item, COLL_INFO* coll, short rot, short flip)
{
	if (item->hitPoints <= 0)
	{
		item->goalAnimState = LS_JUMP_FORWARD;
		item->currentAnimState = LS_JUMP_FORWARD;
		item->animNumber = LA_FALL_START;
		item->frameNumber = g_Level.Anims[item->animNumber].frameBase;

		item->gravityStatus = true;
		item->speed = 2;
		item->pos.yPos += 256;
		item->fallspeed = 1;

		Lara.gunStatus = LG_NO_ARMS;

		item->pos.yRot += rot / 2;
	}
	else if (flip)
	{

		item->animNumber = LA_HANG_FEET_IDLE;
		item->frameNumber = g_Level.Anims[item->animNumber].frameBase;
		item->goalAnimState = LS_HANG_FEET;
		item->currentAnimState = LS_HANG_FEET;

		coll->Setup.OldPosition.x = Lara.cornerX;
		item->pos.xPos = Lara.cornerX;

		coll->Setup.OldPosition.z = Lara.cornerZ;
		item->pos.zPos = Lara.cornerZ;

		item->pos.yRot += rot;
	}
}

bool LaraFacingCorner(ITEM_INFO* item, short ang, int dist)
{
	auto angle1 = ang + ANGLE(15);
	auto angle2 = ang - ANGLE(15);

	auto vec1 = GAME_VECTOR(item->pos.xPos + dist * phd_sin(angle1),
							item->pos.yPos - (LARA_HEIGHT / 2),
							item->pos.zPos + dist * phd_cos(angle1), 
							item->roomNumber);

	auto vec2 = GAME_VECTOR(item->pos.xPos + dist * phd_sin(angle2),
							item->pos.yPos - (LARA_HEIGHT / 2),
							item->pos.zPos + dist * phd_cos(angle2),
							item->roomNumber);

	auto pos  = GAME_VECTOR(item->pos.xPos,
							item->pos.yPos,
							item->pos.zPos,
							item->roomNumber);

	auto result1 = LOS(&pos, &vec1);
	auto result2 = LOS(&pos, &vec2);

	return (result1 == 0 && result2 == 0);
}

int LaraFloorFront(ITEM_INFO* item, short ang, int dist)
{
	return LaraCollisionFront(item, ang, dist).Position.Floor;
}

COLL_RESULT LaraCollisionFront(ITEM_INFO* item, short ang, int dist)
{
	int x = item->pos.xPos + dist * phd_sin(ang);
	int y = item->pos.yPos - LARA_HEIGHT;
	int z = item->pos.zPos + dist * phd_cos(ang);

	auto collResult = GetCollisionResult(x, y, z, item->roomNumber);

	if (collResult.Position.Floor != NO_HEIGHT)
		collResult.Position.Floor -= item->pos.yPos;

	return collResult;
}

int LaraCeilingFront(ITEM_INFO* item, short ang, int dist, int h)
{
	return LaraCeilingCollisionFront(item, ang, dist, h).Position.Ceiling;
}

COLL_RESULT LaraCeilingCollisionFront(ITEM_INFO* item, short ang, int dist, int h)
{
	int x = item->pos.xPos + dist * phd_sin(ang);
	int y = item->pos.yPos - h;
	int z = item->pos.zPos + dist * phd_cos(ang);

	auto collResult = GetCollisionResult(x, y, z, item->roomNumber);

	if (collResult.Position.Ceiling != NO_HEIGHT)
		collResult.Position.Ceiling += h - item->pos.yPos;

	return collResult;
}

int LaraFallen(ITEM_INFO* item, COLL_INFO* coll)
{
	if (Lara.waterStatus == LW_WADE || coll->Middle.Floor <= STEPUP_HEIGHT)
	{
		return 0;
	}

	item->animNumber = LA_FALL_START;
	item->currentAnimState = LS_JUMP_FORWARD;
	item->goalAnimState = LS_JUMP_FORWARD;
	item->frameNumber = g_Level.Anims[item->animNumber].frameBase;
	item->fallspeed = 0;
	item->gravityStatus = true;
	return 1;
}

int LaraLandedBad(ITEM_INFO* item, COLL_INFO* coll)
{
	int landspeed = item->fallspeed - 140;

	if (landspeed > 0)
	{
		if (landspeed <= 14)
		{
			item->hitPoints -= 1000 * SQUARE(landspeed) / 196;
			return item->hitPoints <= 0;
		}
		else
		{
			item->hitPoints = -1;
			return 1;
		}
	}

	return 0;
}
#ifndef NEW_TIGHTROPE
void GetTighRopeFallOff(int regularity) {
	if(LaraItem->hitPoints <= 0 || LaraItem->hitStatus)
	{
		LaraItem->goalAnimState = LS_TIGHTROPE_UNBALANCE_LEFT;
		LaraItem->currentAnimState = LS_TIGHTROPE_UNBALANCE_LEFT;
		LaraItem->animNumber = LA_TIGHTROPE_FALL_LEFT;
		LaraItem->frameNumber = g_Level.Anims[LaraItem->animNumber].frameBase;
	}

	if(!Lara.tightRopeFall && !(GetRandomControl() & regularity))
		Lara.tightRopeFall = 2 - ((GetRandomControl() & 0xF) != 0);
}
#endif // !NEW_TIGHTROPE



bool TestLaraLean(ITEM_INFO* item, COLL_INFO* coll)
{
#if 0
	// TODO: make it more fine-tuned when new collision is done.
	switch (coll->CollisionType)
	{
	case CT_RIGHT:
		if (TrInput & IN_RIGHT)
			return false;
	case CT_LEFT:
		if (TrInput & IN_LEFT)
			return false;
	}
	return true;
#else
	if (coll->CollisionType == CT_RIGHT || coll->CollisionType == CT_LEFT)
		return 0;

	return 1;
#endif
}
