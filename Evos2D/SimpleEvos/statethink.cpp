
#include "stdafx.h"
#include "global.h"
#include "statethink.h"



CStateThink::CStateThink(CUnit *punit) : CState("State Think", punit)
{

}


CStateThink::~CStateThink()
{

}


//------------------------------------------------------------------------
// 
// [2011/1/20 jjuiddong]
//------------------------------------------------------------------------
void CStateThink::StateBegin()
{

}


//------------------------------------------------------------------------
// 
// [2011/1/20 jjuiddong]
//------------------------------------------------------------------------
void CStateThink::StateEnd()
{

}


//------------------------------------------------------------------------
// 
// [2011/1/20 jjuiddong]
//------------------------------------------------------------------------
void CStateThink::Update(float timeDelta)
{
	BOOL isFleeState = FALSE;
	CUnit *pclosetUnit = GetGame()->GetUnitControl()->GetClosetUnit(GetUnit());
	if (pclosetUnit)
	{
		const double len = GetUnit()->GetPosition().Distance(pclosetUnit->GetPosition());
		if (len < 200.f)
		{
			GetStateMachine()->PushState(new CStateFlee(GetUnit()));
			isFleeState = TRUE;
		}
	}

	if (!isFleeState)
	{
		GetStateMachine()->PushState(new CStateWander(GetUnit()));
	}

}

