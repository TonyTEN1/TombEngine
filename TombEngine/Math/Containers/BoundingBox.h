#pragma once

class EulerAngles;
struct ItemInfo;
class Pose;

//namespace TEN::Math
//{
	class BOUNDING_BOX
	{
	public:
		// Components
		short X1, X2 = 0;
		short Y1, Y2 = 0;
		short Z1, Z2 = 0;

		// Constants
		static const BOUNDING_BOX Zero;

		// Constructors
		BOUNDING_BOX();
		BOUNDING_BOX(int x1, int x2, int y1, int y2, int z1, int z2);
		BOUNDING_BOX(ItemInfo* item, bool accurate = true);

		// Getters
		int GetWidth() const;
		int GetHeight() const;
		int GetDepth() const;

		// Utilities
		void RotNoPersp(const EulerAngles& orient, const BOUNDING_BOX& bounds);

		// Converters
		BoundingOrientedBox ToBoundingOrientedBox(const Pose& pose) const;
		BoundingOrientedBox ToBoundingOrientedBox(const Vector3& pos, const Quaternion& orient) const;

		// Operators
		BOUNDING_BOX operator +(const BOUNDING_BOX& bounds) const;
		BOUNDING_BOX operator +(const Pose& pose) const;
		BOUNDING_BOX operator -(const BOUNDING_BOX& bounds) const;
		BOUNDING_BOX operator -(const Pose& pose) const;
		BOUNDING_BOX operator *(float scale) const;
	};
//}
