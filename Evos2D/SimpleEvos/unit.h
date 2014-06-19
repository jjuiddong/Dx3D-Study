
#ifndef __UNIT_H__
#define __UNIT_H__

#include "movingobject.h"

enum UnitState
{
	RUN,
	ARRIVE,
	STOP,
};
static const char *g_StateString[] = {"RUN","ARRIVE","STOP",};


struct SMessage;
struct lua_State;
class CStateMachine;
class CPathExplorer;
class CUnit : public CMovingObject
{
public:
	CUnit(const Vector2D &pos);
	virtual~CUnit();

protected:
	int m_Id;
	UnitState m_State;
	CStateMachine *m_pStateMachine;
	CPathExplorer *m_pPathData;
	Vector2D m_DestPos;

public:
	virtual void Update( const float timeDelta );
	virtual void Render();
	virtual BOOL MsgDipatch(const SMessage &msg);

	void Move(const Vector2D &pos);
	int GetId() { return m_Id; }
	void SetChangeState( UnitState state );
	void SetDestPos(const Vector2D &pos) { m_DestPos = pos; }
	Vector2D GetDestPos() { return m_DestPos; }
	CStateMachine* GetStateMachine() { return m_pStateMachine; }
	CPathExplorer* GetPathData() { return m_pPathData; }

	static void RegisteLuaBind(lua_State* pLua);

protected:
	void UnitDebug();
};

#endif
