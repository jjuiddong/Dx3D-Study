
#include "stdafx.h"

#include "global.h"
#include "dbg.h"
#include "luascript.h"


CLuaScript::CLuaScript()
{
/*
	m_pLuaState = lua_open();
	luaL_openlibs(m_pLuaState);
	luaopen_base(m_pLuaState);
	luaopen_string(m_pLuaState);
	luaopen_table(m_pLuaState);
	luaopen_math(m_pLuaState);
	luabind::open(m_pLuaState);

	Init();
/**/
}


CLuaScript::~CLuaScript()
{
	Clear();

}


//------------------------------------------------------------------------
// 
// [2011/2/2 jjuiddong]
//------------------------------------------------------------------------
int DebugPrint(lua_State *pl)
{
/*
	int n = lua_gettop(pl);
	if (1 != n)
	{
		return 0;
	}

	if (!lua_isnumber(pl, 1))
	{
		return 0;
	}

	int num1 = (int)lua_tonumber(pl, 1);
	g_Dbg.Print("%d\n", num1);
/**/

	return 1;
}


//------------------------------------------------------------------------
// 
// [2011/2/2 jjuiddong]
//------------------------------------------------------------------------
void CLuaScript::RegisterDebugPrint(lua_State* pLua)
{
//	lua_register(pLua,"DebugPrint", DebugPrint);
}


void CLuaScript::Init()
{
/*
	CMovingObject::RegisteLuaBind(m_pLuaState);
	CUnit::RegisteLuaBind(m_pLuaState);
	RegisterDebugPrint(m_pLuaState);
/**/
}


//------------------------------------------------------------------------
// 
// [2011/2/2 jjuiddong]
//------------------------------------------------------------------------
void CLuaScript::UnitDebug(CUnit *punit)
{
//	luabind::object &table = luabind::globals(m_pLuaState);
//	table["UnitDebug"](100);
}


//------------------------------------------------------------------------
// 
// [2011/2/2 jjuiddong]
//------------------------------------------------------------------------
void CLuaScript::Clear()
{
/*
	if (m_pLuaState)
	{
		lua_close(m_pLuaState);
		m_pLuaState = NULL;
	}
/**/
}


//------------------------------------------------------------------------
// 
// [2011/2/2 jjuiddong]
//------------------------------------------------------------------------
BOOL CLuaScript::RunLuaScript(const char* script_name)
{
/*
	if (int error = luaL_dofile(m_pLuaState, script_name) != 0)
	{
		throw std::runtime_error("ERROR Problem with lua script file ");
		return FALSE;
	}
/**/
	return TRUE;
}
