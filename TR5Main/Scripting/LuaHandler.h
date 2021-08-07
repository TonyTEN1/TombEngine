#pragma once
#include "framework.h"
class LuaHandler {
protected:
	sol::state*	m_lua;
	~LuaHandler() = default;

public:
	LuaHandler(sol::state* lua);
	LuaHandler(LuaHandler const &) = delete;
	LuaHandler& operator=(LuaHandler const &) = delete;

	void ExecuteScript(const std::string & luaFilename);
	void ExecuteString(const std::string & command);

	template <typename T>void MakeReadOnlyTable(std::string const& tableName, T const& container)
	{
		auto mt = tableName + "Meta";
		// Put all the data in the metatable	
		m_lua->set(mt, sol::as_table(container));

		// Make the metatable's __index refer to itself so that requests
		// to the main table will go through to the metatable (and thus the
		// container's members)
		m_lua->safe_script(mt + ".__index = " + mt);

		// Don't allow the table to have new elements put into it
		m_lua->safe_script(mt + ".__newindex = function() error('" + tableName + " is read-only') end");

		// Protect the metatable
		m_lua->safe_script(mt + ".__metatable = 'metatable is protected'");

		auto tab = m_lua->create_named_table(tableName);
		m_lua->safe_script("setmetatable(" + tableName + ", " + mt + ")");

		// point the initial metatable variable away from its contents. this is just for cleanliness
		m_lua->safe_script(mt + "= nil");
	}
};
