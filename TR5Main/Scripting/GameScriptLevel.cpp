#include "framework.h"
#include "GameScriptLevel.h"
#include <unordered_map>

/***
A container for level metadata - things which aren't present in the compiled
level file itself.

@classmod Level
@pragma nostrip
*/

void GameScriptLevel::Register(sol::state* state)
{
	state->new_usertype<GameScriptLevel>("Level",
		sol::constructors<GameScriptLevel()>(),

/// (string) todo 
//@mem name
		"name", &GameScriptLevel::NameStringKey,

/// (string) todo 
//@mem script
		"script", &GameScriptLevel::ScriptFileName,

/// (string) todo 
//@mem fileName
		"fileName", &GameScriptLevel::FileName,

/// (string) todo 
//@mem loadScreen
		"loadScreen", &GameScriptLevel::LoadScreenFileName,
		
/// (string) todo 
//@mem ambientTrack
		"ambientTrack", &GameScriptLevel::AmbientTrack,

/// (@{SkyLayer}) Primary sky layer  
//@mem layer1
		"layer1", &GameScriptLevel::Layer1,

/// (@{SkyLayer}) Secondary sky layer __(not yet implemented)__
//@mem layer2
		"layer2", &GameScriptLevel::Layer2,

/// (@{Color}) distance fog RGB color (as seen in TR4's Desert Railroad).
// if not provided, distance fog will be black.
// __(not yet implemented)__
//@mem fog
		"fog", &GameScriptLevel::Fog,

/// (bool) if set to true, the horizon and sky layer will be drawn; if set to false; they won't.
//@mem horizon
		"horizon", &GameScriptLevel::Horizon,

/// (bool) if true, the horizon graphic will transition smoothly to the sky layer.
// If set to false, there will be a black band between the two.
// __(not yet implemented)__
//@mem colAddHorizon
		"colAddHorizon", &GameScriptLevel::ColAddHorizon,

/// (bool) equivalent to classic TRLE's LIGHTNING setting.
// If true, there will be a flickering lightning in the skylayer, as in the TRC Ireland levels.
// __(thunder sounds not yet implemented)__
//@mem storm
		"storm", &GameScriptLevel::Storm,

/// (WeatherType) Must be one of the values WeatherType.NORMAL, WeatherType.RAIN, or WeatherType.SNOW.
// __(not yet implemented)__
//@mem weather
		"weather", &GameScriptLevel::Weather,

/*** (LaraType) Must be one of the LaraType values.
These are:

	NORMAL  
	YOUNG
	BUNHEAD
	CATSUIT
	DIVESUIT
	INVISIBLE

e.g. `myLevel.laraType = LaraType.DIVESUIT`

 __(not yet fully implemented)__
 @mem laraType*/
		"laraType", &GameScriptLevel::LaraType,

/// (bool) todo 
//@mem rumble
		"rumble", &GameScriptLevel::Rumble,

/// (bool) todo 
//@mem resetHub
		"resetHub", &GameScriptLevel::ResetHub,

/// (Mirror) todo 
//@mem mirror
		"mirror", &GameScriptLevel::Mirror,

/// (table of InventoryObjects) todo 
//@mem mirror
		"objects", &GameScriptLevel::InventoryObjects
		);
}
