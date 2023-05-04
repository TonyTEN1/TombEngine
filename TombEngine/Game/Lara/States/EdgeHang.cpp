#include "framework.h"
#include "Game/Lara/States/EdgeHang.h"

#include "Game/camera.h"
#include "Game/collision/collide_item.h"
#include "Game/collision/collide_room.h"
#include "Game/items.h"
#include "Game/Lara/Context.h"
#include "Game/Lara/lara.h"
#include "Game/Lara/lara_helpers.h"
#include "Game/Lara/lara_overhang.h"
#include "Game/Lara/lara_tests.h"
#include "Specific/Input/Input.h"
#include "Specific/level.h"

using namespace TEN::Input;

//------Debug
#include "Renderer/Renderer11.h"
using namespace TEN::Renderer;
//------

namespace TEN::Player
{
	// notes:
	// 1. get shimmy context data (ShimmyData with attractor collisions).
	// 2. pass to edge movement handler?

	struct EdgeHangAttractorCollisionData
	{
		std::optional<AttractorCollisionData> Center = std::nullopt;
		std::optional<AttractorCollisionData> Left	 = std::nullopt;
		std::optional<AttractorCollisionData> Right	 = std::nullopt;
	};

	static std::optional<AttractorCollisionData> GetConnectingAttractorCollision(const ItemInfo& item, const CollisionInfo& coll, const Attractor& currentAttrac,
																				 const Vector3& refPoint)
	{
		constexpr auto CONNECT_DIST_THRESHOLD = 10.0f;

		// Get attractor collisions.
		auto attracColls = GetAttractorCollisions(item, refPoint, CONNECT_DIST_THRESHOLD);

		const AttractorCollisionData* attracCollPtr = nullptr;
		float closestDist = INFINITY;

		// Assess attractor collision.
		for (const auto& attracColl : attracColls)
		{
			// 1. Ensure attractor is new.
			if (attracColl.AttractorPtr == &currentAttrac)
				continue;

			// 2) Check if attractor is edge type.
			if (!attracColl.AttractorPtr->IsEdge())
				continue;

			// 3) Check if edge is within range.
			if (!attracColl.IsIntersected)
				continue;

			// Track closest attractor.
			if (attracColl.Proximity.Distance < closestDist)
			{
				attracCollPtr = &attracColl;
				closestDist = attracColl.Proximity.Distance;
			}
		}

		if (attracCollPtr == nullptr)
			return std::nullopt;

		return *attracCollPtr;
	}

	static EdgeHangAttractorCollisionData GetEdgeHangAttractorCollisions(const ItemInfo& item, const CollisionInfo& coll, float sideOffset = 0.0f)
	{
		const auto& player = GetLaraInfo(item);
		const auto& handsAttrac = player.Context.HandsAttractor;

		float length = handsAttrac.Ptr->GetLength();
		auto points = handsAttrac.Ptr->GetPoints();

		// Get clamped projected distances along attractor.
		float lineDist = handsAttrac.LineDistance + sideOffset;

		// Calculate line distances.
		float distLeft = lineDist - coll.Setup.Radius;
		float distRight = lineDist + coll.Setup.Radius;

		// TODO: Horrible, organise later.
		// Get connecting attractors just in case.
		auto centerConnectingAttracColl = GetConnectingAttractorCollision(item, coll, *player.Context.HandsAttractor.Ptr, (lineDist <= 0.0f) ? points.front() : points.back());
		auto leftConnectingAttracColl = GetConnectingAttractorCollision(item, coll, *player.Context.HandsAttractor.Ptr, points.front());
		auto rightConnectingAttracColl = GetConnectingAttractorCollision(item, coll, *player.Context.HandsAttractor.Ptr, points.back());

		// Get points.
		auto pointCenter = handsAttrac.Ptr->GetPointAtLineDistance(lineDist);
		if (!handsAttrac.Ptr->IsLooped() && centerConnectingAttracColl.has_value())
		{
			if (lineDist <= 0.0f)
			{
				pointCenter = centerConnectingAttracColl->AttractorPtr->GetPointAtLineDistance(centerConnectingAttracColl->Proximity.Distance + lineDist);
			}
			else if (lineDist >= length)
			{
				pointCenter = centerConnectingAttracColl->AttractorPtr->GetPointAtLineDistance(centerConnectingAttracColl->Proximity.Distance + (lineDist - length));
			}
		}

		auto pointLeft = ((distLeft <= 0.0f) && !handsAttrac.Ptr->IsLooped() && leftConnectingAttracColl.has_value()) ?
			leftConnectingAttracColl->AttractorPtr->GetPointAtLineDistance(leftConnectingAttracColl->Proximity.LineDistance + distLeft) :
			handsAttrac.Ptr->GetPointAtLineDistance(distLeft);
		auto pointRight = ((distRight >= length) && !handsAttrac.Ptr->IsLooped()) ?
			rightConnectingAttracColl->AttractorPtr->GetPointAtLineDistance(rightConnectingAttracColl->Proximity.LineDistance + (distRight - length)) :
			handsAttrac.Ptr->GetPointAtLineDistance(distRight);

		auto basePos = item.Pose.Position.ToVector3();
		auto orient = item.Pose.Orientation;

		// Get attractor collisions.
		auto attracCollCenter = handsAttrac.Ptr->GetCollision(basePos, orient, pointCenter, coll.Setup.Radius);
		if ((lineDist <= 0.0f || lineDist >= length) && leftConnectingAttracColl.has_value())
			attracCollCenter = centerConnectingAttracColl->AttractorPtr->GetCollision(basePos, orient, pointCenter, coll.Setup.Radius);

		auto attracCollLeft = ((distLeft <= 0.0f) && !handsAttrac.Ptr->IsLooped() && leftConnectingAttracColl.has_value()) ?
			leftConnectingAttracColl->AttractorPtr->GetCollision(basePos, orient, pointLeft, coll.Setup.Radius) :
			handsAttrac.Ptr->GetCollision(basePos, orient, pointLeft, coll.Setup.Radius);
		auto attracCollRight = ((distRight >= length) && !handsAttrac.Ptr->IsLooped()) ?
			rightConnectingAttracColl->AttractorPtr->GetCollision(basePos, orient, pointLeft, coll.Setup.Radius) :
			handsAttrac.Ptr->GetCollision(basePos, orient, pointRight, coll.Setup.Radius);

		// ----------Debug
		constexpr auto COLOR_MAGENTA = Vector4(1, 0, 1, 1);
		g_Renderer.AddLine3D(attracCollCenter.Proximity.Point, attracCollCenter.Proximity.Point + Vector3(0.0f, -150.0f, 0.0f), COLOR_MAGENTA);
		g_Renderer.AddLine3D(attracCollLeft.Proximity.Point, attracCollLeft.Proximity.Point + Vector3(0.0f, -100.0f, 0.0f), COLOR_MAGENTA);
		g_Renderer.AddLine3D(attracCollRight.Proximity.Point, attracCollRight.Proximity.Point + Vector3(0.0f, -100.0f, 0.0f), COLOR_MAGENTA);

		short angleDelta = Geometry::GetShortestAngle(attracCollCenter.HeadingAngle, (sideOffset >= 0.0f) ? attracCollRight.HeadingAngle : attracCollLeft.HeadingAngle);
		g_Renderer.PrintDebugMessage("Angle delta: %.3f", TO_DEGREES(angleDelta));
		//------------
		
		// Return attractor collisions at three points.
		return EdgeHangAttractorCollisionData
		{
			attracCollCenter,
			attracCollLeft,
			attracCollRight
		};
	}

	void HandlePlayerEdgeMovement(ItemInfo& item, const CollisionInfo& coll, bool isGoingRight)
	{
		constexpr auto ORIENT_LERP_ALPHA = 0.5f;

		auto& player = GetLaraInfo(item);

		// Check for hands attractor.
		if (player.Context.HandsAttractor.Ptr == nullptr)
		{
			player.Control.IsHanging = false;
			return;
		}

		// Get edge hang attractor collisions.
		float offset = item.Animation.Velocity.z * (isGoingRight ? 1 : -1);
		auto edgeAttracColls = GetEdgeHangAttractorCollisions(item, coll, offset);

		// Check for center attractor collision.
		if (!edgeAttracColls.Center.has_value())
		{
			player.Control.IsHanging = false;
			return;
		}

		// Test segment slope angle.
		if (edgeAttracColls.Center->SlopeAngle >= SLIPPERY_SLOPE_ANGLE)
		{
			player.Control.IsHanging = false;
			return;
		}

		// Calculate heading angle.
		auto orient = Geometry::GetOrientToPoint(edgeAttracColls.Left->Proximity.Point, edgeAttracColls.Right->Proximity.Point);
		auto headingAngle = orient.y - ANGLE(90.0f);

		// Align orientation.
		player.Context.TargetOrientation = EulerAngles(0, headingAngle, 0);
		item.Pose.Orientation.Lerp(player.Context.TargetOrientation, ORIENT_LERP_ALPHA);

		// Determine target point (correctly handles positioning at inner bends).
		auto targetPoint = edgeAttracColls.Center->Proximity.Point;
		if (!Geometry::IsPointInFront(targetPoint, edgeAttracColls.Left->Proximity.Point, player.Context.TargetOrientation) &&
			!Geometry::IsPointInFront(targetPoint, edgeAttracColls.Right->Proximity.Point, player.Context.TargetOrientation))
		{
			targetPoint = (edgeAttracColls.Left->Proximity.Point + edgeAttracColls.Right->Proximity.Point) / 2;
		}

		// Align position.
		auto rotMatrix = Matrix::CreateRotationY(TO_RAD(headingAngle));
		auto relOffset = Vector3(0.0f, coll.Setup.Height, -coll.Setup.Radius);
		item.Pose.Position = targetPoint + Vector3::Transform(relOffset, rotMatrix);

		// Set edge hang parameters.
		player.Control.IsHanging = true;
		player.Context.HandsAttractor.Set(*edgeAttracColls.Center->AttractorPtr, edgeAttracColls.Center->Proximity.LineDistance);
	}

	// State:	  LS_HANG_IDLE (10)
	// Collision: lara_col_hang_idle()
	void lara_as_hang_idle(ItemInfo* item, CollisionInfo* coll)
	{
		auto& player = GetLaraInfo(*item);

		// Setup
		item->Animation.IsAirborne = false;
		player.Control.MoveAngle = item->Pose.Orientation.y;
		player.Control.IsClimbingLadder = false;
		coll->Setup.Mode = CollisionProbeMode::FreeFlat;
		coll->Setup.Height = LARA_HEIGHT_STRETCH;
		coll->Setup.EnableObjectPush = false;
		coll->Setup.EnableSpasm = false;
		Camera.targetAngle = 0;
		Camera.targetElevation = ANGLE(-45.0f);

		if (item->HitPoints <= 0)
		{
			SetPlayerEdgeHangRelease(*item);
			return;
		}

		HandlePlayerEdgeMovement(*item, *coll, true);

		if (IsHeld(In::Look))
			LookUpDown(item);

		if (IsHeld(In::Action) && player.Control.IsHanging)
		{
			if (TestLaraClimbIdle(item, coll))
			{
				item->Animation.TargetState = LS_LADDER_IDLE;
				return;
			}

			if (IsHeld(In::Jump) && Context::CanPerformLedgeJump(*item, *coll))
			{
				if (IsHeld(In::Back))
				{
					item->Animation.TargetState = LS_JUMP_FORWARD;
				}
				else
				{
					item->Animation.TargetState = LS_JUMP_UP;
				}

				return;
			}

			if (IsHeld(In::Forward))
			{
				if (Context::CanClimbLedgeToCrouch(*item, *coll))
				{
					item->Animation.TargetState = LS_HANG_TO_CROUCH;
					return;
				}
				else if (Context::CanClimbLedgeToStand(*item, *coll))
				{
					if (IsHeld(In::Crouch))
					{
						item->Animation.TargetState = LS_HANG_TO_CROUCH;
					}
					else if (IsHeld(In::Walk) && Context::CanPerformLedgeHandstand(*item, *coll))
					{
						item->Animation.TargetState = LS_HANDSTAND;
					}
					else
					{
						item->Animation.TargetState = LS_GRABBING;
					}

					return;
				}

				if (Context::CanShimmyUp(*item, *coll))
				{
					// TODO: State dispatch.
					SetAnimation(item, LA_LADDER_SHIMMY_UP);
					return;
				}
			}
			else if (IsHeld(In::Back))
			{
				if (Context::CanShimmyDown(*item, *coll))
				{
					// TODO: State dispatch.
					SetAnimation(item, LA_LADDER_SHIMMY_UP);
					return;
				}
			}

			if (IsHeld(In::Left) || IsHeld(In::LeftStep))
			{
				if (Context::CanShimmyLeft(*item, *coll) && HasStateDispatch(item, LS_SHIMMY_LEFT))
				{
					item->Animation.TargetState = LS_SHIMMY_LEFT;
					return;
				}

				auto cornerShimmyState = GetPlayerCornerShimmyState(*item, *coll);
				if (cornerShimmyState.has_value())
				{
					item->Animation.TargetState = cornerShimmyState.value();
					return;
				}
			}
			else if (IsHeld(In::Right) || IsHeld(In::RightStep))
			{
				if (Context::CanShimmyRight(*item, *coll) && HasStateDispatch(item, LS_SHIMMY_RIGHT))
				{
					item->Animation.TargetState = LS_SHIMMY_RIGHT;
					return;
				}

				auto cornerShimmyState = GetPlayerCornerShimmyState(*item, *coll);
				if (cornerShimmyState.has_value())
				{
					item->Animation.TargetState = cornerShimmyState.value();
					return;
				}
			}

			item->Animation.TargetState = LS_HANG_IDLE;
			return;
		}

		SetPlayerEdgeHangRelease(*item);
	}

	// State:	LS_HANG_IDLE (10)
	// Control: lara_as_hang_idle()
	void lara_col_hang_idle(ItemInfo* item, CollisionInfo* coll)
	{
		//HandlePlayerEdgeHang(*item, *coll);
	}

	// State:	  LS_SHIMMY_LEFT (30)
	// Collision: lara_col_shimmy_left()
	void lara_as_shimmy_left(ItemInfo* item, CollisionInfo* coll)
	{
		auto& player = GetLaraInfo(*item);

		// Setup
		player.Control.MoveAngle = item->Pose.Orientation.y - ANGLE(90.0f);
		coll->Setup.Mode = CollisionProbeMode::FreeFlat;
		coll->Setup.Radius = LARA_RADIUS;
		coll->Setup.EnableObjectPush = false;
		coll->Setup.EnableSpasm = false;
		coll->Setup.Height = LARA_HEIGHT_STRETCH;
		Camera.targetAngle = 0;
		Camera.targetElevation = ANGLE(-45.0f);

		if (item->HitPoints <= 0)
		{
			SetPlayerEdgeHangRelease(*item);
			return;
		}

		HandlePlayerEdgeMovement(*item, *coll, false);

		if (IsHeld(In::Action) && player.Control.IsHanging)
		{
			if (IsHeld(In::Left) || IsHeld(In::LeftStep))
			{
				item->Animation.TargetState = LS_SHIMMY_LEFT;
				return;
			}

			item->Animation.TargetState = LS_HANG_IDLE;
			return;
		}

		SetPlayerEdgeHangRelease(*item);
	}

	// State:	LS_SHIMMY_LEFT (30)
	// Control: lara_as_shimmy_left()
	void lara_col_shimmy_left(ItemInfo* item, CollisionInfo* coll)
	{
		//auto& player = GetLaraInfo(*item);

		//HandlePlayerEdgeHang(*item, *coll);
		//player.Control.MoveAngle = item->Pose.Orientation.y - ANGLE(90.0f);
	}

	// State:	  LS_SHIMMY_RIGHT (31)
	// Collision: lara_col_shimmy_right()
	void lara_as_shimmy_right(ItemInfo* item, CollisionInfo* coll)
	{
		auto& player = GetLaraInfo(*item);

		// Setup
		player.Control.MoveAngle = item->Pose.Orientation.y + ANGLE(90.0f);
		coll->Setup.Mode = CollisionProbeMode::FreeFlat;
		coll->Setup.Radius = LARA_RADIUS;
		coll->Setup.EnableObjectPush = false;
		coll->Setup.EnableSpasm = false;
		coll->Setup.Height = LARA_HEIGHT_STRETCH;
		Camera.targetAngle = 0;
		Camera.targetElevation = ANGLE(-45.0f);

		if (item->HitPoints <= 0)
		{
			SetPlayerEdgeHangRelease(*item);
			return;
		}

		HandlePlayerEdgeMovement(*item, *coll, true);

		if (IsHeld(In::Action) && player.Control.IsHanging)
		{
			if (IsHeld(In::Right) || IsHeld(In::RightStep))
			{
				item->Animation.TargetState = LS_SHIMMY_RIGHT;
				return;
			}

			item->Animation.TargetState = LS_HANG_IDLE;
			return;
		}

		SetPlayerEdgeHangRelease(*item);
	}

	// State:	LS_SHIMMY_RIGHT (31)
	// Control: lara_as_shimmy_right()
	void lara_col_shimmy_right(ItemInfo* item, CollisionInfo* coll)
	{
		//auto& player = GetLaraInfo(*item);

		//HandlePlayerEdgeHang(*item, *coll);
		//player.Control.MoveAngle = item->Pose.Orientation.y + ANGLE(90.0f);
	}

	// State:	  LS_SHIMMY_OUTER_LEFT (107), LS_SHIMMY_OUTER_RIGHT (108), LS_SHIMMY_INNER_LEFT (109), LS_SHIMMY_INNER_RIGHT (110)
	// Collision: lara_default_col()
	void lara_as_shimmy_corner(ItemInfo* item, CollisionInfo* coll)
	{
		// Setup
		coll->Setup.Height = LARA_HEIGHT_STRETCH;
		Camera.targetAngle = 0;
		Camera.targetElevation = ANGLE(-33.0f);
		Camera.laraNode = LM_TORSO;

		SetPlayerCornerShimmyEnd(*item, *coll, TestLastFrame(item));
	}

	// State:	  LS_HANDSTAND (54)
	// Collision: lara_default_col()
	void lara_as_handstand(ItemInfo* item, CollisionInfo* coll)
	{
		// Setup
		coll->Setup.EnableObjectPush = false;
		coll->Setup.EnableSpasm = false;
	}
}
