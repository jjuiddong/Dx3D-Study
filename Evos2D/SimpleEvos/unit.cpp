
#include "stdafx.h"
#include "../Common/global.h"
#include "../Common/evos/evosmap.h"
#include "global.h"
#include "../Common/lua-5.1.3/include/luahelperfunctions.h"
#include "luascript.h"
#include "unit.h"



CUnit::CUnit(const Vector2D &pos)
{
	static int s_UnitId = 0;
	m_Id = s_UnitId++;

	m_pStateMachine = new CStateMachine();
	m_pPathData = new CPathExplorer(this);

	SetChangeState(STOP);
	SetPosition(pos);
	m_DestPos = Vector2D(0,0);

	m_pStateMachine->PushState(new CStateThink(this));
}


CUnit::~CUnit()
{
	SAFE_DELETE(m_pStateMachine);
	SAFE_DELETE(m_pPathData);

}


//------------------------------------------------------------------------
// 
// [2011/1/13 jjuiddong]
//------------------------------------------------------------------------
void CUnit::Update( const float timeDelta )
{
	CMovingObject::Update(timeDelta);
	m_pStateMachine->Update(timeDelta);

//	UnitDebug();
}


//------------------------------------------------------------------------
// 
// [2011/2/2 jjuiddong]
//------------------------------------------------------------------------
void CUnit::UnitDebug()
{
	CLuaScript *pscript = GetGame()->GetLuaScript();
	if (pscript->RunLuaScript("lua_unitdebug.lua"))
	{
		pscript->UnitDebug(this);
	}
}


//------------------------------------------------------------------------
// 
// [2011/1/13 jjuiddong]
//------------------------------------------------------------------------
void CUnit::Render()
{
	gdi->YellowBrush();
	gdi->Circle(m_Position.x, m_Position.y, m_Scale);
	m_pStateMachine->Render();

}


//------------------------------------------------------------------------
// 
// [2011/1/13 jjuiddong]
//------------------------------------------------------------------------
BOOL CUnit::MsgDipatch(const SMessage &msg)
{
	switch (msg.msg)
	{
	case MSG_PATHREADY:
		{
			GetStateMachine()->PushState(new CStateMove(this, GetPathData()));
		}
		break;

	case MSG_NOPATHAVAILABLE:
		break;
	}

	return TRUE;
}


//------------------------------------------------------------------------
// 
// [2011/1/17 jjuiddong]
//------------------------------------------------------------------------
void CUnit::Move(const Vector2D &pos)
{
//	GetStateMachine()->PopAllState();
//	GetStateMachine()->PushState(new CStateMove(this, pos));

	GetStateMachine()->PopAllState();
	GetGame()->GetPathFinder()->Register(this, pos);

}


//------------------------------------------------------------------------
// 
// [2011/1/19 jjuiddong]
//------------------------------------------------------------------------
void CUnit::SetChangeState( UnitState state )
{
	m_State = state;
	g_Dbg.Print( "%s\n", g_StateString[state] );

	switch (state)
	{
	case STOP:
		SetVelocity(0.f);
		break;
	}
}


//------------------------------------------------------------------------
// 
// [2011/2/2 jjuiddong]
//------------------------------------------------------------------------
void CUnit::RegisteLuaBind(lua_State* pLua)
{
/*
	using namespace luabind;
	module(pLua)
		[
			class_<CUnit, bases<CMovingObject> >("Unit")
			.def(constructor<const Vector2D&>())
			.def("GetDestPos", &CUnit::GetDestPos)
			.def("GetId", &CUnit::GetId)
		];
/**/
}
