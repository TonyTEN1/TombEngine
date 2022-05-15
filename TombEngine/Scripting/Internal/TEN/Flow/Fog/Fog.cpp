#include "framework.h"
#include "Fog.h"
#include "Specific/RGBAColor8Byte.h"
/***
Fog

@tenclass Flow.Fog
@pragma nostrip
*/
 
void Fog::Register(sol::table & parent)
{
	using ctors = sol::constructors<Fog(RGBAColor8Byte const&, short, short)>;
	parent.new_usertype<Fog>("Fog",
		ctors(),
		sol::call_constructor, ctors(), 

		/// (@{Color}) RGB fog color
		//@mem color
		"color", sol::property(&Fog::SetColor),

		/*** (int) min distance.

		This is the distance at which the fog starts 

		@mem minDistance*/
		"minDistance", &Fog::MinDistance,

		/*** (int) max distance.

		This is the distance at which the fog reaches the maximum strength

		@mem maxDistance*/
		"maxDistance", &Fog::MaxDistance
		);
}

/***
@tparam Color color RGB color
@tparam int Min distance todo fix this up
@tparam int Max distance todo fix this up
@return A fog object.
@function Fog.new
*/
Fog::Fog(RGBAColor8Byte const& col, short minDistance, short maxDistance)
{
	SetColor(col);
	MinDistance = minDistance;
	MaxDistance = maxDistance;
	Enabled = true;
}

void Fog::SetColor(RGBAColor8Byte const& col)
{
	R = col.GetR();
	G = col.GetG();
	B = col.GetB();
}


RGBAColor8Byte Fog::GetColor() const
{
	return RGBAColor8Byte{ R, G, B };
}