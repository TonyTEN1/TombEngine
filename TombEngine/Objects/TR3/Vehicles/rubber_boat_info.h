#pragma once

namespace TEN::Entities::Vehicles
{
	struct RubberBoatInfo
	{
		int LeftVerticalVelocity  = 0;
		int RightVerticalVelocity = 0;

		short TurnRate		= 0;
		short LeanAngle		= 0;
		short ExtraRotation = 0;

		int	  Water				= 0;
		int	  Pitch				= 0;
		short PropellerRotation = 0;
	};
}
