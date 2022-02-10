#include "framework.h"
#include "Game/Lara/lara_climb.h"

#include "Game/animation.h"
#include "Game/camera.h"
#include "Game/collision/sphere.h"
#include "Game/control/control.h"
#include "Game/items.h"
#include "Game/Lara/lara.h"
#include "Game/Lara/lara_helpers.h"
#include "Game/Lara/lara_overhang.h"
#include "Specific/level.h"
#include "Specific/input.h"

short GetClimbFlags(int x, int y, int z, short roomNumber)
{
	return GetClimbFlags(GetFloor(x, y, z, &roomNumber));
}

short GetClimbFlags(FLOOR_INFO* floor)
{
	short result = 0;

	if (floor->Flags.ClimbEast)
		result |= (short)CLIMB_DIRECTION::East;
	if (floor->Flags.ClimbWest)
		result |= (short)CLIMB_DIRECTION::West;
	if (floor->Flags.ClimbNorth)
		result |= (short)CLIMB_DIRECTION::North;
	if (floor->Flags.ClimbSouth)
		result |= (short)CLIMB_DIRECTION::South;

	return result;
}

CLIMB_DIRECTION GetClimbDirection(short angle)
{
	switch (GetQuadrant(angle))
	{
	default:
	case NORTH:
		return CLIMB_DIRECTION::North;
	case EAST:
		return CLIMB_DIRECTION::East;
	case SOUTH:
		return CLIMB_DIRECTION::South;
	case WEST:
		return CLIMB_DIRECTION::West;
	}
}

void lara_col_climbend(ITEM_INFO* item, COLL_INFO* coll)
{
	return;
}

void lara_as_climbend(ITEM_INFO* item, COLL_INFO* coll)
{
	coll->Setup.EnableObjectPush = false;
	coll->Setup.EnableSpasm = false;

	Camera.flags = CF_FOLLOW_CENTER;
	Camera.targetAngle = -ANGLE(45);
}

void lara_col_climbdown(ITEM_INFO* item, COLL_INFO* coll)
{
	if (LaraCheckForLetGo(item, coll) 
		|| item->AnimNumber != LA_LADDER_DOWN)
		return;

	int frame = item->FrameNumber - g_Level.Anims[LA_LADDER_DOWN].frameBase;
	int xShift;
	int yShift;

	switch (frame)
	{
	case 0:
		yShift = 0;
		break;

	case 28:
	case 29:
		yShift = CLICK(1);
		break;

	case 57:
		yShift = CLICK(2);
		break;

	default:
		return;
	}

	item->Position.yPos += yShift + CLICK(1);

	int shiftLeft = 0;
	int shiftRight = 0;
	int resultRight = LaraTestClimbPos(item, coll->Setup.Radius, coll->Setup.Radius + CLICK(0.5f), -CLICK(2), CLICK(2), &shiftRight);
	int resultLeft = LaraTestClimbPos(item, coll->Setup.Radius, -(coll->Setup.Radius + CLICK(0.5f)), -CLICK(2), CLICK(2), &shiftLeft);

	item->Position.yPos -= CLICK(1);

	if (resultRight != 0 && resultLeft != 0 &&
		resultRight != -2 && resultLeft != -2 &&
		TrInput & IN_BACK)
	{
		if (shiftRight && shiftLeft)
		{
			if (shiftRight < 0 != shiftLeft < 0)
			{
				item->TargetState = LS_LADDER_IDLE;
				AnimateLara(item);
				return;
			}

			if (shiftRight < 0 && shiftRight < shiftLeft ||
				shiftRight > 0 && shiftRight > shiftLeft)
			{
				shiftLeft = shiftRight;
			}
		}

		if (resultRight == -1 || resultLeft == -1)
		{
			SetAnimation(item, LA_LADDER_IDLE);
			item->TargetState = LS_HANG;

			AnimateLara(item);
		}
		else
		{
			item->TargetState = LS_LADDER_DOWN;
			item->Position.yPos -= yShift;
		}
		return;
	}

	item->TargetState = LS_LADDER_IDLE;

	if (yShift != 0)
		AnimateLara(item);
}

void lara_as_climbdown(ITEM_INFO* item, COLL_INFO* coll)
{
	coll->Setup.EnableObjectPush = false;
	coll->Setup.EnableSpasm = false;

	Camera.targetElevation = -ANGLE(45);

	SlopeClimbDownExtra(item, coll);
}

void lara_col_climbing(ITEM_INFO* item, COLL_INFO* coll)
{
	if (!LaraCheckForLetGo(item, coll) && item->AnimNumber == LA_LADDER_UP)
	{
		int frame = item->FrameNumber - g_Level.Anims[LA_LADDER_UP].frameBase;
		int yShift;
		int resultRight, resultLeft;
		int shiftRight, shiftLeft;
		int ledgeRight, ledgeLeft;

		if (frame == 0)
		{
			yShift = 0;
		}
		else if (frame == 28 || frame == 29)
		{
			yShift = -CLICK(1);
		}
		else if (frame == 57)
		{
			yShift = -CLICK(2);
		}
		else
		{
			return;
		}

		item->Position.yPos += yShift - CLICK(1);

		resultRight = LaraTestClimbUpPos(item, coll->Setup.Radius, coll->Setup.Radius + CLICK(0.5f), &shiftRight, &ledgeRight);
		resultLeft = LaraTestClimbUpPos(item, coll->Setup.Radius, -(coll->Setup.Radius + CLICK(0.5f)), &shiftLeft, &ledgeLeft);

		item->Position.yPos += CLICK(1);
		 
		if (resultRight && resultLeft && TrInput & IN_FORWARD)
		{
			if (resultRight < 0 || resultLeft < 0)
			{
				item->TargetState = LS_LADDER_IDLE;

				AnimateLara(item);

				if (abs(ledgeRight - ledgeLeft) <= CLICK(0.5f))
				{
					if (resultRight != -1 || resultLeft != -1)
					{
						item->TargetState = LS_LADDER_TO_CROUCH;
						item->RequiredState = LS_CROUCH_IDLE;
					}
					else
					{
						item->TargetState = LS_GRABBING;
						item->Position.yPos += (ledgeRight + ledgeLeft) / 2 - CLICK(1);
					}
				}
			}
			else
			{
				item->TargetState = LS_LADDER_UP;
				item->Position.yPos -= yShift;
			}
		}
		else
		{
			item->TargetState = LS_LADDER_IDLE;

			if (yShift != 0)
				AnimateLara(item);
		}
	}
}

void lara_as_climbing(ITEM_INFO* item, COLL_INFO* coll)
{
	coll->Setup.EnableObjectPush = false;
	coll->Setup.EnableSpasm = false;

	Camera.targetElevation = ANGLE(30);
}

void lara_col_climbright(ITEM_INFO* item, COLL_INFO* coll)
{
	if (!LaraCheckForLetGo(item, coll))
	{
		int shift = 0;
		Lara.Control.MoveAngle = item->Position.yRot + ANGLE(90);
		LaraDoClimbLeftRight(item, coll, LaraTestClimbPos(item, coll->Setup.Radius, coll->Setup.Radius + CLICK(0.5f), -CLICK(2), CLICK(2), &shift), shift);
	}
}

void lara_as_climbright(ITEM_INFO* item, COLL_INFO* coll)
{
	coll->Setup.EnableObjectPush = false;
	coll->Setup.EnableSpasm = false;

	Camera.targetAngle = ANGLE(30);
	Camera.targetElevation = -ANGLE(15);

	if (!(TrInput & (IN_RIGHT | IN_RSTEP)))
		item->TargetState = LS_LADDER_IDLE;
}

void lara_col_climbleft(ITEM_INFO* item, COLL_INFO* coll)
{
	if (!LaraCheckForLetGo(item, coll))
	{
		int shift = 0;
		Lara.Control.MoveAngle = item->Position.yRot - ANGLE(90);
		LaraDoClimbLeftRight(item, coll, LaraTestClimbPos(item, coll->Setup.Radius, -(coll->Setup.Radius + CLICK(0.5f)), -CLICK(2), CLICK(2), &shift), shift);
	}
}

void lara_as_climbleft(ITEM_INFO* item, COLL_INFO* coll)
{
	coll->Setup.EnableObjectPush = false;
	coll->Setup.EnableSpasm = false;

	Camera.targetAngle = -ANGLE(30);
	Camera.targetElevation = -ANGLE(15);

	if (!(TrInput & (IN_LEFT | IN_LSTEP)))
		item->TargetState = LS_LADDER_IDLE;
}

void lara_col_climbstnc(ITEM_INFO* item, COLL_INFO* coll)
{
	int yShift;
	int resultRight, resultLeft;
	int shiftRight, shiftLeft;
	int ledgeRight, ledgeLeft;

	if (LaraCheckForLetGo(item, coll) || item->AnimNumber != LA_LADDER_IDLE)
		return;

	if (!(TrInput & IN_FORWARD))
	{
		if (!(TrInput & IN_BACK))
			return;

		if (item->TargetState == LS_HANG)
			return;

		item->TargetState = LS_LADDER_IDLE;
		item->Position.yPos += CLICK(1);
		
		resultRight = LaraTestClimbPos(item, coll->Setup.Radius, coll->Setup.Radius + CLICK(0.5f), -CLICK(2), CLICK(2), &ledgeRight);
		resultLeft = LaraTestClimbPos(item, coll->Setup.Radius, -CLICK(0.5f) - coll->Setup.Radius, -CLICK(2), CLICK(2), &ledgeLeft);
		
		item->Position.yPos -= CLICK(1);
		
		if (!resultRight || !resultLeft || resultLeft == -2 || resultRight == -2)
			return;

		yShift = ledgeLeft;

		if (ledgeRight && ledgeLeft)
		{
			if (ledgeLeft < 0 != ledgeRight < 0)
				return;
			if (ledgeRight < 0 == ledgeRight < ledgeLeft)
				yShift = ledgeRight;
		}

		if (resultRight == 1 && resultLeft == 1)
		{
			item->TargetState = LS_LADDER_DOWN;
			item->Position.yPos += yShift;
		}
		else
		{
			item->TargetState = LS_HANG;
		}
	}
	else if (item->TargetState != LS_GRABBING)
	{
		item->TargetState = LS_LADDER_IDLE;
		resultRight = LaraTestClimbUpPos(item, coll->Setup.Radius, coll->Setup.Radius + CLICK(0.5f), &shiftRight, &ledgeRight);
		resultLeft = LaraTestClimbUpPos(item, coll->Setup.Radius, -CLICK(0.5f) - coll->Setup.Radius, &shiftLeft, &ledgeLeft);

		if (!resultRight || !resultLeft)
		{
			LadderMonkeyExtra(item, coll);
			return;
		}

		if (resultRight >= 0 && resultLeft >= 0)
		{
			yShift = shiftLeft;

			if (shiftRight)
			{
				if (shiftLeft)
				{
					if (shiftLeft < 0 != shiftRight < 0)
						return;
					if (shiftRight < 0 == shiftRight < shiftLeft)
						yShift = shiftRight;
				}
				else
				{
					yShift = shiftRight;
				}
			}

			item->TargetState = LS_LADDER_UP;
			item->Position.yPos += yShift;
		}
		else if (abs(ledgeLeft - ledgeRight) <= CLICK(0.5f))
		{
			if (resultRight == -1 && resultLeft == -1)
			{
				item->TargetState = LS_GRABBING;
				item->Position.yPos += (ledgeRight + ledgeLeft) / 2 - CLICK(1);
			}
			else
			{
				item->TargetState = LS_LADDER_TO_CROUCH;
				item->RequiredState = LS_CROUCH_IDLE;
			}
		}
	}
}

void lara_as_climbstnc(ITEM_INFO* item, COLL_INFO* coll)
{
	Lara.Control.IsClimbingLadder = true;

	coll->Setup.EnableSpasm = false;
	coll->Setup.EnableObjectPush = false;

	Camera.targetElevation = -ANGLE(20);

	if (item->AnimNumber == LA_LADDER_DISMOUNT_LEFT_START)
		Camera.targetAngle = -ANGLE(60.0f);
	if (item->AnimNumber == LA_LADDER_DISMOUNT_RIGHT_START)
		Camera.targetAngle = ANGLE(60.0f);

	if (TrInput & IN_LOOK)
	{
		LookUpDown();
	}

	if (TrInput & IN_LEFT || TrInput & IN_LSTEP)
	{
		item->TargetState = LS_LADDER_LEFT;
		Lara.Control.MoveAngle = item->Position.yRot - ANGLE(90);
	}
	else if (TrInput & IN_RIGHT || TrInput & IN_RSTEP)
	{
		item->TargetState = LS_LADDER_RIGHT;
		Lara.Control.MoveAngle = item->Position.yRot + ANGLE(90);
	}
	else if (TrInput & IN_JUMP)
	{
		if (item->AnimNumber == LA_LADDER_IDLE)
		{
			item->TargetState = LS_JUMP_BACK;
			Lara.Control.HandStatus = HandStatus::Free;
			Lara.Control.MoveAngle = item->Position.yRot + ANGLE(180);
		}
	}

	SlopeClimbExtra(item, coll);
}

void lara_as_stepoff_left(ITEM_INFO* item, COLL_INFO* coll)
{
	coll->Setup.EnableObjectPush = false;
	coll->Setup.EnableSpasm = false;

	Camera.targetAngle = -ANGLE(60.0f);
	Camera.targetElevation = -ANGLE(15.0f);

	item->Position.yRot -= ANGLE(90.0f);
}

void lara_as_stepoff_right(ITEM_INFO* item, COLL_INFO* coll)
{
	coll->Setup.EnableObjectPush = false;
	coll->Setup.EnableSpasm = false;

	Camera.targetAngle = ANGLE(60.0f);
	Camera.targetElevation = -ANGLE(15.0f);

	item->Position.yRot += ANGLE(90.0f);
}

int LaraTestClimbPos(ITEM_INFO* item, int front, int right, int origin, int height, int* shift)
{
	int x;
	int z;
	int xFront = 0;
	int zFront = 0;

	switch (GetQuadrant(item->Position.yRot))
	{
	case NORTH:
		x = item->Position.xPos + right;
		z = item->Position.zPos + front;
		zFront = CLICK(1);
		break;

	case EAST:
		x = item->Position.xPos + front;
		z = item->Position.zPos - right;
		xFront = CLICK(1);
		break;

	case SOUTH:
		x = item->Position.xPos - right;
		z = item->Position.zPos - front;
		zFront = -CLICK(1);
		break;

	case WEST:
	default:
		x = item->Position.xPos - front;
		z = item->Position.zPos + right;
		xFront = -CLICK(1);
		break;
	}

	return LaraTestClimb(x, item->Position.yPos + origin, z, xFront, zFront, height, item->RoomNumber, shift);
}

void LaraDoClimbLeftRight(ITEM_INFO* item, COLL_INFO* coll, int result, int shift)
{
	if (result == 1)
	{
		if (TrInput & IN_LEFT)
		{
			item->TargetState = LS_LADDER_LEFT;
		}
		else if (TrInput & IN_RIGHT)
		{
			item->TargetState = LS_LADDER_RIGHT;
		}
		else
		{
			item->TargetState = LS_LADDER_IDLE;
		}

		item->Position.yPos += shift;

		return;
	}

	if (result != 0)
	{
		item->TargetState = LS_HANG;

		do
		{
			AnimateItem(item);
		} while (item->ActiveState != LS_HANG);

		item->Position.xPos = coll->Setup.OldPosition.x;
		item->Position.zPos = coll->Setup.OldPosition.z;

		return;
	}

	item->Position.xPos = coll->Setup.OldPosition.x;
	item->Position.zPos = coll->Setup.OldPosition.z;

	item->TargetState = LS_LADDER_IDLE;
	item->ActiveState = LS_LADDER_IDLE;

	if (coll->Setup.OldState != LS_LADDER_IDLE)
	{	
		SetAnimation(item, LA_LADDER_IDLE);
		return;
	}

	if (TrInput & IN_LEFT)
	{
		short troomnumber = item->RoomNumber;
		int dx = int(sin(TO_RAD(item->Position.yRot - ANGLE(90.0f))) * 10);
		int dz = int(cos(TO_RAD(item->Position.yRot - ANGLE(90.0f))) * 10);
		int height = GetFloorHeight(GetFloor(item->Position.xPos + dx, item->Position.yPos, item->Position.zPos + dz, &troomnumber),
			item->Position.xPos, item->Position.yPos, item->Position.zPos) - item->Position.yPos;
		if (height < 3 * STEP_SIZE / 2) // LADDER dismounts (left/right)
		{
			item->TargetState = LS_LADDER_DISMOUNT_LEFT;
			item->ActiveState = LS_MISC_CONTROL;
		}
	}
	else if (TrInput & IN_RIGHT)
	{
		short troomnumber = item->RoomNumber;
		int dx = int(sin(TO_RAD(item->Position.yRot + ANGLE(90.0f))) * 10);
		int dz = int(cos(TO_RAD(item->Position.yRot + ANGLE(90.0f))) * 10);
		int height = GetFloorHeight(GetFloor(item->Position.xPos + dx, item->Position.yPos, item->Position.zPos + dz, &troomnumber),
			item->Position.xPos, item->Position.yPos, item->Position.zPos) - item->Position.yPos;
		if (height < 3 * STEP_SIZE / 2) // LADDER dismounts (left/right)
		{
			item->TargetState = LS_LADDER_DISMOUNT_RIGHT;
			item->ActiveState = LS_MISC_CONTROL;
		}
	}

	if (TrInput & IN_LEFT)
	{
		int flag = LaraClimbLeftCornerTest(item, coll);

		if (flag)
		{
			if (flag <= 0)
				SetAnimation(item, LA_LADDER_LEFT_CORNER_INNER_START);
			else
				SetAnimation(item, LA_LADDER_LEFT_CORNER_OUTER_START);

			return;
		}
	}
	else if (TrInput & IN_RIGHT)
	{
		int flag = LaraClimbRightCornerTest(item, coll);

		if (flag)
		{
			if (flag <= 0)
				SetAnimation(item, LA_LADDER_RIGHT_CORNER_INNER_START);
			else
				SetAnimation(item, LA_LADDER_RIGHT_CORNER_OUTER_START);

			return;
		}
	}

	item->AnimNumber = coll->Setup.OldAnimNumber;
	item->FrameNumber = coll->Setup.OldFrameNumber;

	AnimateLara(item);
}

int LaraClimbRightCornerTest(ITEM_INFO* item, COLL_INFO* coll)
{
	int result = 0;

	if (item->AnimNumber != LA_LADDER_RIGHT)
		return 0;

	auto oldPos = item->Position;
	auto oldRot = Lara.Control.MoveAngle;

	short angle = GetQuadrant(item->Position.yRot);
	int x, z;

	if (angle && angle != SOUTH)
	{
		x = (item->Position.xPos & -WALL_SIZE) - (item->Position.zPos % WALL_SIZE) + WALL_SIZE;
		z = (item->Position.zPos & -WALL_SIZE) - (item->Position.xPos % WALL_SIZE) + WALL_SIZE;
	}
	else
	{
		x = item->Position.xPos ^ (item->Position.xPos ^ item->Position.zPos) & (WALL_SIZE - 1);
		z = item->Position.zPos ^ (item->Position.xPos ^ item->Position.zPos) & (WALL_SIZE - 1);
	}

	int shift = 0;

	if (GetClimbFlags(x, item->Position.yPos, z, item->RoomNumber) & (short)LeftExtRightIntTab[angle])
	{
		Lara.nextCornerPos.xPos = item->Position.xPos = x;
		Lara.nextCornerPos.yPos = item->Position.yPos;
		Lara.nextCornerPos.zPos = item->Position.zPos = z;
		Lara.nextCornerPos.yRot = item->Position.yRot = Lara.Control.MoveAngle = item->Position.yRot + ANGLE(90);

		result = LaraTestClimbPos(item, coll->Setup.Radius, coll->Setup.Radius + CLICK(0.5f), -CLICK(2), CLICK(2), &shift);
		item->ItemFlags[3] = result;
	}

	if (!result)
	{
		item->Position = oldPos;
		Lara.Control.MoveAngle = oldRot;

		switch (angle)
		{
		case NORTH:
			x = ((item->Position.xPos + WALL_SIZE) & -WALL_SIZE) - (item->Position.zPos % WALL_SIZE) + WALL_SIZE;
			z = ((item->Position.zPos + WALL_SIZE) & -WALL_SIZE) - (item->Position.xPos % WALL_SIZE) + WALL_SIZE;
			break;

		case SOUTH:
			x = ((item->Position.xPos - WALL_SIZE) & -WALL_SIZE) - (item->Position.zPos % WALL_SIZE) + WALL_SIZE;
			z = ((item->Position.zPos - WALL_SIZE) & -WALL_SIZE) - (item->Position.xPos % WALL_SIZE) + WALL_SIZE;
			break;

		case EAST:
			x = ((item->Position.zPos ^ item->Position.xPos) % WALL_SIZE) ^ (item->Position.xPos + WALL_SIZE);
			z = (item->Position.zPos ^ ((item->Position.zPos ^ item->Position.xPos) % WALL_SIZE)) - WALL_SIZE;
			break;

		case WEST:
		default:
			x = (item->Position.xPos ^ (item->Position.zPos ^ item->Position.xPos) % WALL_SIZE) - WALL_SIZE;
			z = ((item->Position.zPos ^ item->Position.xPos) % WALL_SIZE) ^ (item->Position.zPos + WALL_SIZE);
			break;

		}

		if (GetClimbFlags(x, item->Position.yPos, z, item->RoomNumber) & (short)LeftIntRightExtTab[angle])
		{
			Lara.nextCornerPos.xPos = item->Position.xPos = x;
			Lara.nextCornerPos.yPos = item->Position.yPos;
			Lara.nextCornerPos.zPos = item->Position.zPos = z;
			Lara.nextCornerPos.yRot = item->Position.yRot = Lara.Control.MoveAngle = item->Position.yRot - ANGLE(90);

			result = LaraTestClimbPos(item, coll->Setup.Radius, coll->Setup.Radius + CLICK(0.5f), -CLICK(2), CLICK(2), &shift);
			item->ItemFlags[3] = result;
		}
	}
	else
	{
		result = -1;
	}

	item->Position = oldPos;
	Lara.Control.MoveAngle = oldRot;

	return result;
}

int LaraClimbLeftCornerTest(ITEM_INFO* item, COLL_INFO* coll)
{
	int result = 0;

	if (item->AnimNumber != LA_LADDER_LEFT)
		return 0;

	auto oldPos = item->Position;
	auto oldRot = Lara.Control.MoveAngle;

	short angle = GetQuadrant(item->Position.yRot);
	int x, z;

	if (angle && angle != SOUTH)
	{
		x = item->Position.xPos ^ (item->Position.xPos ^ item->Position.zPos) & (WALL_SIZE - 1);
		z = item->Position.zPos ^ (item->Position.xPos ^ item->Position.zPos) & (WALL_SIZE - 1);
	}
	else
	{
		x = (item->Position.xPos & -WALL_SIZE) - (item->Position.zPos & (WALL_SIZE - 1)) + WALL_SIZE;
		z = (item->Position.zPos & -WALL_SIZE) - (item->Position.xPos & (WALL_SIZE - 1)) + WALL_SIZE;
	}

	int shift = 0;

	if (GetClimbFlags(x, item->Position.yPos, z, item->RoomNumber) & (short)LeftIntRightExtTab[angle])
	{
		Lara.nextCornerPos.xPos = item->Position.xPos = x;
		Lara.nextCornerPos.yPos = item->Position.yPos;
		Lara.nextCornerPos.zPos = item->Position.zPos = z;
		Lara.nextCornerPos.yRot = item->Position.yRot = Lara.Control.MoveAngle = item->Position.yRot - ANGLE(90);

		result = LaraTestClimbPos(item, coll->Setup.Radius, -coll->Setup.Radius - CLICK(0.5f), -CLICK(2), CLICK(2), &shift);
		item->ItemFlags[3] = result;
	}

	if (!result)
	{
		item->Position = oldPos;
		Lara.Control.MoveAngle = oldRot;

		switch (angle)
		{
		case NORTH:
			x = (item->Position.xPos ^ ((item->Position.zPos ^ item->Position.xPos) & (WALL_SIZE - 1))) - WALL_SIZE;
			z = ((item->Position.zPos ^ item->Position.xPos) & (WALL_SIZE - 1)) ^ (item->Position.zPos + WALL_SIZE);
			break;

		case SOUTH:
			x = ((item->Position.zPos ^ item->Position.xPos) & (WALL_SIZE - 1)) ^ (item->Position.xPos + WALL_SIZE);
			z = ((item->Position.zPos ^ item->Position.xPos) & (WALL_SIZE - 1)) ^ (item->Position.zPos - WALL_SIZE);
			break;

		case EAST:
			x = ((item->Position.xPos + WALL_SIZE) & -WALL_SIZE) - (item->Position.zPos & (WALL_SIZE - 1)) + WALL_SIZE;
			z = ((item->Position.zPos + WALL_SIZE) & -WALL_SIZE) - (item->Position.xPos & (WALL_SIZE - 1)) + WALL_SIZE;
			break;

		case WEST:
		default:
			x = (item->Position.xPos & -WALL_SIZE) - (item->Position.zPos & (WALL_SIZE - 1));
			z = (item->Position.zPos & -WALL_SIZE) - (item->Position.xPos & (WALL_SIZE - 1));
			break;

		}

		if (GetClimbFlags(x, item->Position.yPos, z, item->RoomNumber) & (short)LeftExtRightIntTab[angle])
		{
			Lara.nextCornerPos.xPos = item->Position.xPos = x;
			Lara.nextCornerPos.yPos = item->Position.yPos;
			Lara.nextCornerPos.zPos = item->Position.zPos = z;
			Lara.nextCornerPos.yRot = item->Position.yRot = Lara.Control.MoveAngle = item->Position.yRot + ANGLE(90);

			item->ItemFlags[3] = LaraTestClimbPos(item, coll->Setup.Radius, -coll->Setup.Radius - CLICK(0.5f), -CLICK(2), CLICK(2), &shift);
			result = item->ItemFlags[3] != 0;
		}
	}
	else
	{
		result = -1;
	}

	item->Position = oldPos;
	Lara.Control.MoveAngle = oldRot;

	return result;
}

int LaraTestClimb(int x, int y, int z, int xFront, int zFront, int itemHeight, int itemRoom, int* shift)
{
	*shift = 0;
	int hang = 1;
	if (!Lara.Control.CanClimbLadder)
		return 0;

	short roomNumber = itemRoom;
	FLOOR_INFO* floor = GetFloor(x, y - CLICK(0.5f), z, &roomNumber);
	int height = GetFloorHeight(floor, x, y, z);
	if (height == NO_HEIGHT)
		return 0;

	height -= (CLICK(0.5f) + y + itemHeight);
	if (height < -70)
		return 0;
	if (height < 0)
		*shift = height;

	int ceiling = GetCeiling(floor, x, y, z) - y;
	if (ceiling > 70)
		return 0;
	if (ceiling > 0)
	{
		if (*shift)
			return 0;
		*shift = ceiling;
	}

	if (itemHeight + height < 900)
		hang = 0;

	int dz = zFront + z;
	int dx = xFront + x;
	
	floor = GetFloor(dx, y, dz, &roomNumber);
	height = GetFloorHeight(floor, dx, y, dz);
	
	if (height != NO_HEIGHT)
	{
		height -= y;
	}

	if (height <= 70)
	{
		if (height > 0)
		{
			if (*shift < 0)
				return 0;
			if (height > *shift)
				*shift = height;
		}
		
		roomNumber = itemRoom;
		GetFloor(x, y + itemHeight, z, &roomNumber);
		FLOOR_INFO* floor2 = GetFloor(dx, y + itemHeight, dz, &roomNumber);
		ceiling = GetCeiling(floor2, dx, y + itemHeight, dz);
		
		if (ceiling == NO_HEIGHT)
			return 1;
		if (ceiling - y <= height)
			return 1;
		if (ceiling - y >= CLICK(2))
			return 1;
		if (ceiling - y <= 442)
			return -(hang != 0);
		if (*shift > 0)
			return -(hang != 0);
		*shift = ceiling - y - CLICK(2);

		return 1;
	}
	
	ceiling = GetCeiling(floor, dx, y, dz) - y;
	if (ceiling >= CLICK(2))
		return 1;
	if (ceiling > 442)
	{
		if (*shift > 0)
			return -(hang != 0);
		*shift = ceiling - CLICK(2);

		return 1;
	}

	if (ceiling > 0)
		return -(hang != 0);
	if (ceiling <= -70 || !hang || *shift > 0)
		return 0;
	if (*shift > ceiling)
		*shift = ceiling;

	return -1;
}

int LaraTestClimbUpPos(ITEM_INFO* item, int front, int right, int* shift, int* ledge)
{
	int y = item->Position.yPos - 768;

	int x, z;
	int xFront = 0;
	int zFront = 0;

	switch (GetQuadrant(item->Position.yRot))
	{
	case NORTH:
		x = item->Position.xPos + right;
		z = item->Position.zPos + front;
		zFront = 4;
		break;
	case EAST:
		x = item->Position.xPos + front;
		z = item->Position.zPos - right;
		xFront = 4;
		break;
	case SOUTH:
		x = item->Position.xPos - right;
		z = item->Position.zPos - front;
		zFront = -4;
		break;
	default:
		x = item->Position.xPos - front;
		z = item->Position.zPos + right;
		xFront = -4;
		break;
	}

	*shift = 0;

	short roomNumber = item->RoomNumber;
	FLOOR_INFO* floor = GetFloor(x, y, z, &roomNumber);
	int ceiling = CLICK(1) - y + GetCeiling(floor, x, y, z);
	
	if (ceiling > 70)
		return 0;

	if (ceiling > 0)
		*shift = ceiling;

	floor = GetFloor(x + xFront, y, z + zFront, &roomNumber);
	int height = GetFloorHeight(floor, x + xFront, y, z + zFront);
	
	if (height == NO_HEIGHT)
	{
		*ledge = NO_HEIGHT;
		return 1;
	}
	else
	{
		height -= y;
		*ledge = height;
		
		if (height <= CLICK(0.5f))
		{
			if (height > 0 && height > *shift)
				*shift = height;

			roomNumber = item->RoomNumber;
			GetFloor(x, y + CLICK(2), z, &roomNumber);
			floor = GetFloor(x + xFront, y + CLICK(2), z + zFront, &roomNumber);
			ceiling = GetCeiling(floor, x + xFront, y + CLICK(2), z + zFront) - y;
			if (ceiling <= height)
				return 1;						
			if (ceiling >= CLICK(2))
				return 1;					
			else
				return 0;
		}
		else
		{
			ceiling = GetCeiling(floor, x + xFront, y, z + zFront) - y;
			if (ceiling < CLICK(2))
			{
				if (height - ceiling <= LARA_HEIGHT)
				{
					if (height - ceiling < CLICK(2))
						return 0;
					*shift = height;
					return -2;
				}
				else
				{
					*shift = height;
					return -1;
				}
			}
			else
			{
				return 1;
			}
		}
	}

	return -2;
}

bool LaraCheckForLetGo(ITEM_INFO* item, COLL_INFO* coll)
{
	short roomNumber = item->RoomNumber;

	item->Airborne = false;
	item->VerticalVelocity = 0;

	if (TrInput & IN_ACTION && item->HitPoints > 0 || item->AnimNumber == LA_ONWATER_TO_LADDER) // Can't let go on this anim
		return false;

	ResetLaraFlex(item);

	SetAnimation(item, LA_FALL_START);

	item->Velocity = 2;
	item->Airborne = true;
	item->VerticalVelocity = 1;

	Lara.Control.HandStatus = HandStatus::Free;

	return true;
}
