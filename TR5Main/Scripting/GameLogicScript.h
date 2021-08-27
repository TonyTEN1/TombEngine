#pragma once
#include "items.h"
#include "room.h"
#include "LuaHandler.h"
#include "trmath.h"
#include <unordered_set>
#include "GameScriptColor.h"
#include "GameScriptPosition.h"
#include "GameScriptRotation.h"
#include "GameScriptItemInfo.h"
#include "GameScriptMeshInfo.h"
#include "GameScriptSinkInfo.h"
#include "GameScriptAIObject.h"
#include "GameScriptSoundSourceInfo.h"
#include "GameScriptCameraInfo.h"

struct LuaFunction {
	std::string Name;
	std::string Code;
	bool Executed;
};

struct GameScriptVector3 {
	float x;
	float y;
	float z;
};


class LuaVariables
{
public:
	std::map<std::string, sol::object>			variables;

	sol::object							GetVariable(sol::table tab, std::string key);
	void								SetVariable(sol::table tab, std::string key, sol::object value);
};

struct LuaVariable
{
	bool IsGlobal;
	std::string Name;
	int Type;
	float FloatValue;
	int IntValue;
	std::string StringValue;
	bool BoolValue;
};

using VarMapVal = std::variant< short,
	std::reference_wrapper<MESH_INFO>,
	std::reference_wrapper<LEVEL_CAMERA_INFO>,
	std::reference_wrapper<SINK_INFO>,
	std::reference_wrapper<SOUND_SOURCE_INFO>,
	std::reference_wrapper<AI_OBJECT>>;

class GameScript : public LuaHandler
{
private:

	LuaVariables										m_globals{};
	LuaVariables										m_locals{};
	std::unordered_map<std::string, VarMapVal>			m_nameMap{};
	std::unordered_map<std::string, short>				m_itemsMapName{};
	std::unordered_set<std::string>						m_levelFuncs{};
	sol::protected_function								m_onStart{};
	sol::protected_function								m_onLoad{};
	sol::protected_function								m_onControlPhase{};
	sol::protected_function								m_onSave{};
	sol::protected_function								m_onEnd{};

	void ResetLevelTables();

public:	
	GameScript(sol::state* lua);

	void								FreeLevelScripts();


	bool								SetLevelFunc(sol::table tab, std::string const& luaName, sol::object obj);
	sol::protected_function				GetLevelFunc(sol::table tab, std::string const& luaName);

	void								AssignItemsAndLara();


	void								ExecuteFunction(std::string const & name);
	void								MakeItemInvisible(short id);

	template <typename R, char const* S>
	std::unique_ptr<R> GetByName(std::string const& name)
	{
		ScriptAssertF(m_nameMap.find(name) != m_nameMap.end(), "{} name not found: {}", S, name);
		return std::make_unique<R>(std::get<R::IdentifierType>(m_nameMap.at(name)), false);
	}

	template <typename Value>
	bool AddName(std::string const& key, Value&& val)
	{
		auto p = std::pair<std::string const&, Value>{ key, val };
		return m_nameMap.insert(p).second;
	}

	bool RemoveName(std::string const& key)
	{
		return m_nameMap.erase(key);
	}

	// Variables
	template <typename T>
	void								GetVariables(std::map<std::string, T>& locals, std::map<std::string, T>& globals);
	template <typename T>
	void								SetVariables(std::map<std::string, T>& locals, std::map<std::string, T>& globals);
	void								ResetVariables();


	void								InitCallbacks();
	void								OnStart();
	void								OnLoad();
	void								OnControlPhase(float dt);
	void								OnSave();
	void								OnEnd();
};
