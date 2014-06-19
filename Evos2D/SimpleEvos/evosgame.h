
#ifndef __EVOSGAME_H__
#define __EVOSGAME_H__

#include <list>

class CUnit;
class CEvosMap;
class CPathFindManager;
class CUnitControl;
class CLuaScript;
struct lua_State;
class CEvosGame
{
public:
	CEvosGame();
	virtual ~CEvosGame();

protected:
	CEvosMap *m_pMap;
	CPathFindManager *m_pPathFinderManager;
	CUnitControl *m_pUnitControl;
	CLuaScript *m_pLuaScript;

public:
	void Init();
	void Update( const float timeDelta );
	void Render( HDC hdc );
	void Clear();

	void KeyDownEvent( int key );
	void MouseEvent( int windowEvent, POINT pos);

	CPathFindManager* GetPathFinder() { return m_pPathFinderManager; }
	CEvosMap* GetMap() { return m_pMap; }
	CUnitControl* GetUnitControl() { return m_pUnitControl; }
	CLuaScript* GetLuaScript() { return m_pLuaScript; }

};

#endif
