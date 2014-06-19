
#include "stdafx.h"
#include "unit.h"
#include "statemachine.h"

#include "stateArrive.h"



CStateArrive::CStateArrive(CUnit *punit, const Vector2D &pos) : 
CState("State Arrive", punit)
{
	m_DestPos = pos;

}


CStateArrive::~CStateArrive()
{

}


//------------------------------------------------------------------------
// 
// [2011/1/20 jjuiddong]
//------------------------------------------------------------------------
void CStateArrive::StateBegin()
{
	Vector2D dir = m_DestPos - GetUnit()->GetPosition();
	dir.Normalize();
	GetUnit()->SetDirection(dir);
}


//------------------------------------------------------------------------
// 
// [2011/1/20 jjuiddong]
//------------------------------------------------------------------------
void CStateArrive::StateEnd()
{

}


//------------------------------------------------------------------------
// 
// [2011/1/20 jjuiddong]
//------------------------------------------------------------------------
void CStateArrive::Update(float timeDelta)
{
	const Vector2D &unitPos = GetUnit()->GetPosition();
	const double distSq = unitPos.DistanceSq(m_DestPos);
	if (distSq < 10.f)
	{
		GetStateMachine()->PopCurrentState(this);
		GetUnit()->SetVelocity(0.f);
	}
	else
	{

	}
}

