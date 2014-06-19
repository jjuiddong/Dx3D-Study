
#ifndef __LUASCRIPT_H__
#define __LUASCRIPT_H__

//#include "luabind/luabind.hpp"

//struct lua_State;
//class luabind::object;
class CLuaScript
{
public:
	CLuaScript();
	virtual ~CLuaScript();

protected:
//	lua_State *m_pLuaState;

public:
//	lua_State* GetLuaState() const { return m_pLuaState; }
	BOOL RunLuaScript(const char* script_name);
	void Clear();

	void UnitDebug(CUnit *punit);

protected:
	void Init();
	void RegisterDebugPrint(lua_State* pLua);

};

#endif // __LUASCRIPT_H__
