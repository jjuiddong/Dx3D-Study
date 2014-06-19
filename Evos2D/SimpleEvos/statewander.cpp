
#include "stdafx.h"

#include "SimpleEvos.h"
#include "evosgame.h"
#include "pathfindmanager.h"
#include "pathexplorer.h"
#include "../Common/evos/evosmap.h"
#include "unit.h"
#include "statemachine.h"
#include "stateArrive.h"
#include "statemove.h"

#include "dbg.h"

#include "statewander.h"


CStateWander::CStateWander(CUnit *punit) : 
CState("State Wander", punit)
{

}


CStateWander::~CStateWander()
{

}


//------------------------------------------------------------------------
// 
// [2011/1/20 jjuiddong]
//------------------------------------------------------------------------
void CStateWander::StateBegin()
{
	// 유닛주위를 돌아다닌다.
	const Vector2D &pos = GetUnit()->GetPosition();
	double x = RandFloat() * 250.f;
	double y = RandFloat() * 250.f;
	x -= 125.f;
	y -= 125.f;

	GetStateMachine()->PopCurrentState(this);
	Vector2D newPos = Vector2D(pos.x + x, pos.y + y);

//	GetStateMachine()->PushState(new CStateArrive(GetUnit(), GetUnit()->GetPosition()));
	GetGame()->GetPathFinder()->Register(GetUnit(), newPos);

}


//------------------------------------------------------------------------
// 
// [2011/1/20 jjuiddong]
//------------------------------------------------------------------------
void CStateWander::StateEnd()
{

}


//------------------------------------------------------------------------
// 
// [2011/1/20 jjuiddong]
//------------------------------------------------------------------------
void CStateWander::Update(float timeDelta)
{

}


//------------------------------------------------------------------------
// 디버그용 함수. 경로를 출력한다.
// [2011/1/21 jjuiddong]
//------------------------------------------------------------------------
void CStateWander::Render()
{

}
