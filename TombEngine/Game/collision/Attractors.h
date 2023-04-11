#pragma once

class EulerAngles;
struct CollisionInfo;
struct CollisionResult;
struct ItemInfo;

namespace TEN::Collision
{
	enum class AttractorType
	{
		Edge,
		/*VerticalPole,
		HorizontalPole,
		SwingPole,
		ZipLine,
		Tightrope,
		Pinnacle*/
	};

	enum class AttractorPoint
	{
		Point0,
		Point1
	};

	class Attractor
	{
	private:
		// Members
		AttractorType Type		 = AttractorType::Edge;
		Vector3		  Point0	 = Vector3::Zero;
		Vector3		  Point1	 = Vector3::Zero;
		int			  RoomNumber = 0;

	public:
		// Constructors
		Attractor() {};
		Attractor(AttractorType type, const Vector3& point0, const Vector3& point1, int roomNumber);

		// Getters
		AttractorType GetType() const;
		Vector3		  GetPoint0() const;
		Vector3		  GetPoint1() const;
		int			  GetRoomNumber() const;

		// Inquirers
		bool IsEdge();

		// Helpers
		void DrawDebug(ItemInfo& item);
	};

	struct AttractorCollision
	{
		const Attractor* AttractorPtr = nullptr;

		Vector3 ClosestPoint = Vector3::Zero;

		bool  IsIntersected	  = false;
		bool  IsInFront		  = false;
		float Distance		  = 0.0f;
		float DistanceFromEnd = 0.0f;
		short FacingAngle	  = 0;
		short SlopeAngle	  = 0;

		AttractorPoint PointOnLeft = AttractorPoint::Point0;
	};

	std::vector<Attractor>			GetSectorAttractors(const CollisionResult& pointColl);
	std::vector<const Attractor*>	GetNearbyAttractorPtrs(const ItemInfo& item);
	std::vector<AttractorCollision> GetAttractorCollisions(const ItemInfo& item, const CollisionInfo& coll,
														   const std::vector<const Attractor*>& attracPtrs,
														   const Vector3& refPoint, float range);
}
