
#include "stdafx.h"

#include "global.h"
#include "stateflee.h"


CStateFlee::CStateFlee(CUnit *punit) :
CState("State Flee", punit)
{

}


CStateFlee::~CStateFlee()
{

}


//------------------------------------------------------------------------
// 
// [2011/1/23 jjuiddong]
//------------------------------------------------------------------------
void CStateFlee::StateBegin()
{
	CUnit *pclosetUnit = GetGame()->GetUnitControl()->GetClosetUnit(GetUnit());
	if (!pclosetUnit) return;

	// 가장 가까운 유닛의 반대 방향으로 움직인다.
	const Vector2D &closetUnitPos = pclosetUnit->GetPosition();
	const Vector2D &unitPos = GetUnit()->GetPosition();

	Vector2D dir = closetUnitPos - unitPos;
	dir.Normalize();
	dir.Negation();

	GetStateMachine()->PopCurrentState(this);
	Vector2D destPos = unitPos + (dir * 100.f);

//	GetStateMachine()->PushState(new CStateArrive(GetUnit(), GetUnit()->GetPosition()));
	GetGame()->GetPathFinder()->Register(GetUnit(), destPos);
}


//------------------------------------------------------------------------
// 
// [2011/1/23 jjuiddong]
//------------------------------------------------------------------------
void CStateFlee::StateEnd()
{

}


//------------------------------------------------------------------------
// 
// [2011/1/23 jjuiddong]
//------------------------------------------------------------------------
void CStateFlee::Update(float timeDelta)
{

}

