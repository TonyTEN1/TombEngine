#include "frameworkandsol.h"
#include "LogicHandler.h"

#if TEN_OPTIONAL_LUA
#include "ScriptAssert.h"
#include "Game/savegame.h"
#include "Sound/sound.h"
#include "Specific/setup.h"
#include "ReservedScriptNames.h"
#include "Game/effects/lightning.h"
#include "ScriptUtil.h"
#include "Objects/Moveable/Moveable.h"

using namespace TEN::Effects::Lightning;

/***
Functions and callbacks for level-specific logic scripts.
@tentable Logic 
@pragma nostrip
*/

#endif

void SetVariable(sol::table tab, sol::object key, sol::object value)
{
#if TEN_OPTIONAL_LUA
	switch (value.get_type())
	{
	case sol::type::lua_nil:
	case sol::type::boolean:
	case sol::type::number:
	case sol::type::string:
	case sol::type::table:
		switch (key.get_type())
		{
		case sol::type::number:
		case sol::type::string:
			tab.raw_set(key, value);
			break;
		default:
			ScriptAssert(false, "Unsupported key type used for special table. Valid types are string and number.", ErrorMode::Terminate);
			break;
		}
		break;
	default:
		key.push();
		size_t strLen;
		const char* str = luaL_tolstring(tab.lua_state(), -1, &strLen);
		if (str)
		{
			ScriptAssert(false, "Variable " + std::string{ str } + " has an unsupported type.", ErrorMode::Terminate);
			lua_pop(tab.lua_state(), 1);
		}
		else
		{
			ScriptAssert(false, "Variable has an unsupported type.", ErrorMode::Terminate);
		}
		key.pop();
		break;
	}
#endif
}

sol::object GetVariable(sol::table tab, sol::object key)
{
#if TEN_OPTIONAL_LUA
	return tab.raw_get<sol::object>(key);
#endif
}

LogicHandler::LogicHandler(sol::state* lua, sol::table & parent) : m_handler{ lua }
{
#if TEN_OPTIONAL_LUA
	ResetLevelTables();

	MakeSpecialTable(m_handler.GetState(), ScriptReserved_GameVars, &GetVariable, &SetVariable);
	m_handler.GetState()->new_enum<GAME_OBJECT_ID>("Object", {
		{"LARA", ID_LARA}
		});
#endif
}

void LogicHandler::ResetGameTables() 
{
#if TEN_OPTIONAL_LUA
	MakeSpecialTable(m_handler.GetState(), ScriptReserved_GameVars, &GetVariable, &SetVariable);
#endif
}


void LogicHandler::ResetLevelTables()
{
#if TEN_OPTIONAL_LUA
	MakeSpecialTable(m_handler.GetState(), ScriptReserved_LevelFuncs, &LogicHandler::GetLevelFunc, &LogicHandler::SetLevelFunc, this);
	MakeSpecialTable(m_handler.GetState(), ScriptReserved_LevelVars, &GetVariable, &SetVariable);
#endif
}

sol::protected_function LogicHandler::GetLevelFunc(sol::table tab, std::string const& luaName)
{
#if TEN_OPTIONAL_LUA
	if (m_levelFuncs.find(luaName) == m_levelFuncs.end())
		return sol::lua_nil;

	return m_levelFuncs.at(luaName);
#else
	return sol::nil;
#endif
}

bool LogicHandler::SetLevelFunc(sol::table tab, std::string const& luaName, sol::object value)
{
#if TEN_OPTIONAL_LUA
	switch (value.get_type())
	{
	case sol::type::lua_nil:
		m_levelFuncs.erase(luaName);
		break;
	case sol::type::function:
		m_levelFuncs.insert_or_assign(luaName, value.as<sol::protected_function>());
		break;
	default:
		std::string error{ "Could not assign LevelFuncs." };
		error += luaName + "; it must be a function (or nil).";
		return ScriptAssert(false, error);
	}
	return true;
#else
	return true;
#endif
}

void LogicHandler::FreeLevelScripts()
{
#if TEN_OPTIONAL_LUA
	m_levelFuncs.clear();
	ResetLevelTables();
	m_onStart = sol::nil;
	m_onLoad = sol::nil;
	m_onControlPhase = sol::nil;
	m_onSave = sol::nil;
	m_onEnd = sol::nil;
	m_handler.GetState()->collect_garbage();
#endif
}

void JumpToLevel(int levelNum)
{
#if TEN_OPTIONAL_LUA
	if (levelNum >= g_GameFlow->GetNumLevels())
		return;

	LevelComplete = levelNum;
#endif
}

int GetSecretsCount()
{
#if TEN_OPTIONAL_LUA
	return Statistics.Level.Secrets;
#else
	return 0;
#endif
}

void SetSecretsCount(int secretsNum)
{
#if TEN_OPTIONAL_LUA
	if (secretsNum > 255)
		return;
	Statistics.Level.Secrets = secretsNum;
#endif
}

void AddOneSecret()
{
#if TEN_OPTIONAL_LUA
	if (Statistics.Level.Secrets >= 255)
		return;
	Statistics.Level.Secrets++;
	PlaySecretTrack();
#endif
}

void LogicHandler::SetVariables(std::vector<SavedVar> const & vars)
{
#if TEN_OPTIONAL_LUA
	ResetGameTables();
	ResetLevelTables();

	std::unordered_map<uint32_t, sol::table> solTables;

	for(std::size_t i = 0; i < vars.size(); ++i)
	{
		if (std::holds_alternative<IndexTable>(vars[i]))
		{
			solTables.try_emplace(i, *m_handler.GetState(), sol::create);
			auto indexTab = std::get<IndexTable>(vars[i]);
			for (auto& [first, second] : indexTab)
			{
				// if we're wanting to reference a table, make sure that table exists
				// create it if need be
				if (std::holds_alternative<IndexTable>(vars[second]))
				{
					solTables.try_emplace(second, *m_handler.GetState(), sol::create);
					solTables[i][vars[first]] = solTables[second];
				}
				else
				{
					solTables[i][vars[first]] = vars[second];
				}
			}
		}
	}
	
	auto rootTable = solTables[0];

	sol::table levelVars = rootTable[ScriptReserved_LevelVars];
	for (auto& [first, second] : levelVars)
	{
		(*m_handler.GetState())[ScriptReserved_LevelVars][first] = second;
	}

	sol::table gameVars = rootTable[ScriptReserved_GameVars];
	for (auto& [first, second] : gameVars)
	{
		(*m_handler.GetState())[ScriptReserved_GameVars][first] = second;
	}
#endif
}

void LogicHandler::GetVariables(std::vector<SavedVar> & vars)
{
#if TEN_OPTIONAL_LUA
	sol::table tab{ *m_handler.GetState(), sol::create };
	tab[ScriptReserved_LevelVars] = (*m_handler.GetState())[ScriptReserved_LevelVars];
	tab[ScriptReserved_GameVars] = (*m_handler.GetState())[ScriptReserved_GameVars];

	std::unordered_map<void const*, uint32_t> varsMap;
	std::unordered_map<double, uint32_t> numMap;
	std::unordered_map<bool, uint32_t> boolMap;
	size_t nVars = 0;
	auto handleNum = [&](double num)
	{
		auto [first, second] = numMap.insert(std::pair<double, uint32_t>(num, nVars));

		// true if the var was inserted
		if (second)
		{
			vars.push_back(num);
			++nVars;
		}

		return first->second;
	};

	auto handleBool = [&](bool num)
	{
		auto [first, second] = boolMap.insert(std::pair<bool, uint32_t>(num, nVars));

		// true if the var was inserted
		if (second)
		{
			vars.push_back(num);
			++nVars;
		}

		return first->second;
	};

	auto handleStr = [&](sol::object const& obj)
	{
		auto str = obj.as<sol::string_view>();
		auto [first, second] = varsMap.insert(std::pair<void const*, uint32_t>(str.data(), nVars));

		// true if the string was inserted
		if (second)
		{
			vars.push_back(std::string{ str.data() });
			++nVars;
		}

		return first->second;
	};

	std::function<uint32_t(sol::table const &)> populate = [&](sol::table const & obj) 
	{
		auto [first, second] = varsMap.insert(std::pair<void const*, uint32_t>(obj.pointer(), nVars));

		// true if the table was inserted
		if(second)
		{
			++nVars;
			auto id = first->second;

			vars.push_back(IndexTable{});
			for (auto& [first, second] : obj)
			{
				uint32_t keyIndex = 0;
				
				// Strings and numbers can be keys AND values
				switch (first.get_type())
				{
				case sol::type::string:
					keyIndex = handleStr(first);
					break;
				case sol::type::number:
					keyIndex = handleNum(first.as<double>());
					break;
				default:
					ScriptAssert(false, "Tried saving an unsupported type as a key");
				}

				uint32_t valIndex = 0;
				switch (second.get_type())
				{
				case sol::type::table:
					valIndex = populate(second.as<sol::table>());
					std::get<IndexTable>(vars[id]).push_back(std::make_pair(keyIndex, valIndex));
					break;
				case sol::type::string:
					valIndex = handleStr(second);
					std::get<IndexTable>(vars[id]).push_back(std::make_pair(keyIndex, valIndex));
					break;
				case sol::type::number:
					valIndex = handleNum(second.as<double>());
					std::get<IndexTable>(vars[id]).push_back(std::make_pair(keyIndex, valIndex));
					break;
				case sol::type::boolean:
					valIndex = handleBool(second.as<bool>());
					std::get<IndexTable>(vars[id]).push_back(std::make_pair(keyIndex, valIndex));
					break;
				default:
					ScriptAssert(false, "Tried saving an unsupported type as a value");
				}
			}
		}
		return first->second;
	};
	populate(tab);
#endif
}

template <typename R, char const * S, typename mapType>
std::unique_ptr<R> GetByName(std::string const & type, std::string const & name, mapType const & map)
{
#if TEN_OPTIONAL_LUA
	ScriptAssert(map.find(name) != map.end(), std::string{ type + " name not found: " + name }, ErrorMode::Terminate);
	return std::make_unique<R>(map.at(name), false);
#endif
}

/*** Special objects
@section specialobjects
*/

/*** A @{Objects.Moveable} representing Lara herself.
@table Lara
*/
void LogicHandler::ResetVariables()
{
#if TEN_OPTIONAL_LUA
	(*m_handler.GetState())["Lara"] = NULL;
#endif
}

void LogicHandler::ExecuteScriptFile(const std::string & luaFilename)
{
#if TEN_OPTIONAL_LUA
	m_handler.ExecuteScript(luaFilename);
#endif
}

void LogicHandler::ExecuteFunction(std::string const& name, TEN::Control::Volumes::VolumeTriggerer triggerer)
{
#if TEN_OPTIONAL_LUA
	sol::protected_function_result r;
	sol::protected_function func = (*m_handler.GetState())["LevelFuncs"][name.c_str()];
	if (std::holds_alternative<short>(triggerer))
	{
		r = func(std::make_unique<Moveable>( std::get<short>(triggerer), true));
	}
	else
	{
		r = func();
	}

	if (!r.valid())
	{
		sol::error err = r;
		ScriptAssertF(false, "Could not execute function {}: {}", name, err.what());
	}
#endif
}

#if TEN_OPTIONAL_LUA
static void doCallback(sol::protected_function const & func, std::optional<float> dt = std::nullopt)  {
	auto r = dt.has_value() ? func(dt) : func();

	if (!r.valid())
	{
		sol::error err = r;
		ScriptAssert(false, err.what(), ErrorMode::Terminate);
	}
}
#endif

void LogicHandler::OnStart()
{
#if TEN_OPTIONAL_LUA
	if (m_onStart.valid())
		doCallback(m_onStart);

#endif
}

void LogicHandler::OnLoad()
{
#if TEN_OPTIONAL_LUA
	if(m_onLoad.valid())
		doCallback(m_onLoad);
#endif
}

void LogicHandler::OnControlPhase(float dt)
{
#if TEN_OPTIONAL_LUA
	if(m_onControlPhase.valid())
		doCallback(m_onControlPhase, dt);
#endif
}

void LogicHandler::OnSave()
{
#if TEN_OPTIONAL_LUA
	if(m_onSave.valid())
		doCallback(m_onSave);
#endif
}

void LogicHandler::OnEnd()
{
#if TEN_OPTIONAL_LUA
	if(m_onEnd.valid())
		doCallback(m_onEnd);
#endif
}

/*** Special tables

TombEngine uses the following tables for specific things.

@section levelandgametables
*/

/*** A table with level-specific data which will be saved and loaded.
This is for level-specific information that you want to store in saved games.

For example, you may have a level with a custom puzzle where Lara has
to kill exactly seven enemies to open a door to a secret. You could use
the following line each time an enemy is killed:

	LevelVars.enemiesKilled = LevelVars.enemiesKilled + 1

If the player saves the level after killing three, saves, and then reloads the save
some time later, the values `3` will be put back into `LevelVars.enemiesKilled.`

__This table is emptied when a level is finished.__ If the player needs to be able
to return to the level (like in the Karnak and Alexandria levels in *The Last Revelation*),
you will need to use the @{GameVars} table, below.
@table LevelVars
*/

/*** A table with game data which will be saved and loaded.
This is for information not specific to any level, but which concerns your whole
levelset or game, that you want to store in saved games.

For example, you may wish to have a final boss say a specific voice line based on
a choice the player made in a previous level. In the level with the choice, you could
write:

	GameVars.playerSnoopedInDraws = true

And in the script file for the level with the boss, you could write:

	if GameVars.playerSnoopedInDrawers then
		PlayAudioTrack("how_dare_you.wav")
	end

Unlike @{LevelVars}, this table will remain intact for the entirety of the game.
@table GameVars
*/

/*** A table with level-specific functions.

This serves two purposes: it holds the level callbacks (listed below) as well as
any trigger functions you might have specified. For example, if you give a trigger
a Lua name of "my_trigger" in Tomb Editor, you will have to implement it as a member
of this table:

	LevelFuncs.my_trigger = function() 
		-- implementation goes here
	end

The following are the level callbacks. They are optional; if your level has no special
behaviour for a particular scenario, you do not need to implement the function. For
example, if your level does not need any special initialisation when it is loaded,
you can just leave out `LevelFuncs.OnStart`.

@tfield function OnStart Will be called when a level is loaded
@tfield function OnLoad Will be called when a saved game is loaded
@tfield function(float) OnControlPhase Will be called during the game's update loop,
and provides the delta time (a float representing game time since last call) via its argument.
@tfield function OnSave Will be called when the player saves the game
@tfield function OnEnd Will be called when leaving a level. This includes finishing it, exiting to the menu, or loading a save in a different level. 
@table LevelFuncs
*/

void LogicHandler::InitCallbacks()
{
#if TEN_OPTIONAL_LUA
	auto assignCB = [this](sol::protected_function& func, std::string const & luaFunc) {
		std::string fullName = "LevelFuncs." + luaFunc;
		func = (*m_handler.GetState())["LevelFuncs"][luaFunc];
		std::string err{ "Level's script does not define callback " + fullName};
		if (!ScriptAssert(func.valid(), err)) {
			ScriptWarn("Defaulting to no " + fullName + " behaviour.");
		}
	};

	assignCB(m_onStart, "OnStart");
	assignCB(m_onLoad, "OnLoad");
	assignCB(m_onControlPhase, "OnControlPhase");
	assignCB(m_onSave, "OnSave");
	assignCB(m_onEnd, "OnEnd");
#endif
}