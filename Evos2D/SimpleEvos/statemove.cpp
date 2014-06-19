
#include "stdafx.h"

#include "../Common/global.h"
#include "SimpleEvos.h"
#include "evosgame.h"
#include "pathfindmanager.h"
#include "pathexplorer.h"
#include "../Common/evos/evosmap.h"
#include "unit.h"
#include "statemachine.h"
#include "stateArrive.h"

#include "dbg.h"

#include "statemove.h"


CStateMove::CStateMove(CUnit *punit, CPathExplorer *ppathData) :
CState("State Move", punit), m_pPathData(ppathData)
{

}


CStateMove::~CStateMove()
{
//	SAFE_DELETE(m_pPathData2);
}


//------------------------------------------------------------------------
// 
// [2011/1/23 jjuiddong]
//------------------------------------------------------------------------
void CStateMove::StateBegin()
{
/*	GetGame()->GetPathFinder()->SearchMap(GetGame()->GetMap(), GetUnit(), m_DestPos, m_pPathData);

	if (m_pPathData->IsArrive())
	{
		GetStateMachine()->PopCurrentState(this);
		return;
	}
/**/

	CUnit *punit = GetUnit();	
	m_pPathData = punit->GetPathData();

//	m_pPathData->PrintPath();

	Vector2D curPos = punit->GetPosition();
	Vector2D nextPos = m_pPathData->GetNextNodePos();

	Vector2D dir = nextPos - curPos;
	dir.Normalize();
	punit->SetDirection(dir);

	punit->SetVelocity(50.f);
	m_RunningTime = 0.f;
	m_NextNodeArrivalTime = m_pPathData->GetNextNodeArrivalTime(punit);

	punit->SetDestPos(m_pPathData->GetDestPos());
}


//------------------------------------------------------------------------
// 
// [2011/1/23 jjuiddong]
//------------------------------------------------------------------------
void CStateMove::StateEnd()
{

}


//------------------------------------------------------------------------
// 
// [2011/1/23 jjuiddong]
//------------------------------------------------------------------------
void CStateMove::Update(float timeDelta)
{
	m_RunningTime += timeDelta;

	const Vector2D &unitPos = GetUnit()->GetPosition();
	if (m_RunningTime + 0.1f > m_NextNodeArrivalTime || m_pPathData->IsNextNodeArrival(unitPos))
	{
		if (m_pPathData->GetNext())
		{
			Vector2D nextPos = m_pPathData->GetNextNodePos();
			Vector2D dir = nextPos - unitPos;
			dir.Normalize();
			GetUnit()->SetDirection(dir);

			m_RunningTime = 0;
			m_NextNodeArrivalTime = m_pPathData->GetNextNodeArrivalTime(GetUnit());

//			g_Dbg.Print("time=%f node=%d {%f, %f} \n", m_NextNodeArrivalTime, m_pPathData->GetCurrentNode()->Index(), unitPos.x, unitPos.y);
//			g_Dbg.Print("dir = {%f, %f}\n", dir.x, dir.y);
		}
		else
		{
			GetStateMachine()->PopCurrentState(this);
			GetStateMachine()->PushState(new CStateArrive(GetUnit(), m_pPathData->GetDestPos()));
		}
	}
/**/
}


//------------------------------------------------------------------------
// 
// [2011/1/23 jjuiddong]
//------------------------------------------------------------------------
void CStateMove::Render()
{
	m_pPathData->RenderPath(Cgdi::thickblue);
//	m_pPathData2->RenderPath(Cgdi::thickblue);

	char buf[128];
	const Vector2D &dest = m_pPathData->GetDestPos();
	sprintf_s(buf,64, "dest: %f, %f", dest.x, dest.y);
	gdi->TextAtPos(Vector2D(0,0), buf);
}

