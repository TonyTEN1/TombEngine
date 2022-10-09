#pragma once

class Vector3i;

//namespace TEN::Math
//{
	class GameVector
	{
	public:
		// Components
		int	  x			 = 0;
		int	  y			 = 0;
		int	  z			 = 0;
		short RoomNumber = 0;
		int	  BoxNumber	 = 0; // Unused.

		// Constants
		static const GameVector Zero;

		// Constructors
		GameVector();
		GameVector(const Vector3i& pos);
		GameVector(const Vector3i& pos, short roomNumber);
		GameVector(const Vector3i& pos, short roomNumber, short boxNumber);
		GameVector(int xPos, int yPos, int zPos);
		GameVector(int xPos, int yPos, int zPos, short roomNumber);
		GameVector(int xPos, int yPos, int zPos, short roomNumber, short boxNumber);

		// Converters
		Vector3	 ToVector3() const;
		Vector3i ToVector3i() const;
	};
//}
