#include "frameworkandsol.h"
#include "StringsHandler.h"

#if TEN_OPTIONAL_LUA
#include "ScriptAssert.h"
#include "Flow/FlowHandler.h"
#include <Renderer/RenderEnums.h>
#include "ReservedScriptNames.h"

/***
Scripts that will be run on game startup.
@tentable Strings 
@pragma nostrip
*/
#endif

StringsHandler::StringsHandler(sol::state* lua, sol::table & parent) : LuaHandler{ lua }
{
#if TEN_OPTIONAL_LUA
	sol::table table_strings{ m_lua->lua_state(), sol::create };
	parent.set(ScriptReserved_Strings, table_strings);

/***
Show some text on-screen.
@tparam DisplayString str the string object to draw
@function ShowString
@tparam float time the time in seconds for which to show the string.
If not given, the string will have an "infinite" life, and will show
until @{HideString} is called or until the level is finished.
Default: nil (i.e. infinite)
*/
	table_strings.set_function(ScriptReserved_ShowString, &StringsHandler::ShowString, this);

/***
Hide some on-screen text.
@function HideString
@tparam DisplayString str the string object to hide. Must previously have been shown
with a call to @{ShowString}, or this function will have no effect.
*/
	table_strings.set_function(ScriptReserved_HideString, [this](DisplayString const& s) {ShowString(s, 0.0f); });

	DisplayString::Register(table_strings);
	DisplayString::SetCallbacks(
		[this](auto && ... param) {return SetDisplayString(std::forward<decltype(param)>(param)...); },
		[this](auto && ... param) {return ScheduleRemoveDisplayString(std::forward<decltype(param)>(param)...); },
		[this](auto && ... param) {return GetDisplayString(std::forward<decltype(param)>(param)...); }
		);
	
	MakeReadOnlyTable(table_strings, ScriptReserved_DisplayStringOption, kDisplayStringOptionNames);
#endif
}

std::optional<std::reference_wrapper<UserDisplayString>> StringsHandler::GetDisplayString(DisplayStringIDType id)
{
#if TEN_OPTIONAL_LUA
	auto it = m_userDisplayStrings.find(id);
	if (std::cend(m_userDisplayStrings) == it)
		return std::nullopt;

	return std::ref(m_userDisplayStrings.at(id));
#else
	return std::nullopt;
#endif
}

bool StringsHandler::ScheduleRemoveDisplayString(DisplayStringIDType id)
{
#if TEN_OPTIONAL_LUA
	auto it = m_userDisplayStrings.find(id);
	if (std::cend(m_userDisplayStrings) == it)
		return false;

	it->second.m_deleteWhenZero = true;
	return true;
#else
	return true;
#endif
}

void StringsHandler::SetCallbackDrawString(CallbackDrawString cb)
{
#if TEN_OPTIONAL_LUA
	m_callbackDrawSring = cb;
#endif
}

bool StringsHandler::SetDisplayString(DisplayStringIDType id, UserDisplayString const & ds)
{
#if TEN_OPTIONAL_LUA
	return m_userDisplayStrings.insert_or_assign(id, ds).second;
#else
	return true;
#endif
}

void StringsHandler::ShowString(DisplayString const & str, sol::optional<float> nSeconds)
{
#if TEN_OPTIONAL_LUA
	auto it = m_userDisplayStrings.find(str.GetID());
	it->second.m_timeRemaining = nSeconds.value_or(0.0f);
	it->second.m_isInfinite = !nSeconds.has_value();
#endif
}

void StringsHandler::ProcessDisplayStrings(float dt)
{
#if TEN_OPTIONAL_LUA
	auto it = std::begin(m_userDisplayStrings);
	while (it != std::end(m_userDisplayStrings))
	{
		auto& str = it->second;
		bool endOfLife = (0.0f >= str.m_timeRemaining);
		if (str.m_deleteWhenZero && endOfLife)
		{
			ScriptAssertF(!str.m_isInfinite, "The infinite string {} (key \"{}\") went out of scope without being hidden.", it->first, str.m_key);
			it = m_userDisplayStrings.erase(it);
		}
		else
		{
			if (!endOfLife || str.m_isInfinite)
			{
				char const* cstr = str.m_isTranslated ? g_GameFlow->GetString(str.m_key.c_str()) : str.m_key.c_str();
				int flags = 0;

				if (str.m_flags[static_cast<size_t>(DisplayStringOptions::CENTER)])
					flags |= PRINTSTRING_CENTER;

				if (str.m_flags[static_cast<size_t>(DisplayStringOptions::OUTLINE)])
					flags |= PRINTSTRING_OUTLINE;

				m_callbackDrawSring(cstr, str.m_color, str.m_x, str.m_y, flags);

				str.m_timeRemaining -= dt;
			}
			++it;
		}
	}
#endif
}
