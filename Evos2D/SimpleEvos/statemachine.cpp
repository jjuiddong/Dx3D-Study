
#include "stdafx.h"
#include "state.h"
#include "dbg.h"
#include "unit.h"
#include "statemachine.h"


CStateMachine::CStateMachine()
{

}


CStateMachine::~CStateMachine()
{
	Clear();

}


//------------------------------------------------------------------------
// 스테이트 추가
// [2011/1/20 jjuiddong]
//------------------------------------------------------------------------
void CStateMachine::PushState( CState *pstate )
{
	g_Dbg.Print("%d %s\n", pstate->GetUnit()->GetId(), pstate->GetStateName());

	m_StateStack.push_back(pstate);
	pstate->StateBegin();

}


//------------------------------------------------------------------------
// Stack에 저장된 모든 State를 저장한다.
// [2011/1/20 jjuiddong]
//------------------------------------------------------------------------
void CStateMachine::PopAllState()
{
	PopState(0);
}


//------------------------------------------------------------------------
// Stack에 저장된 State의 인덱스 값이 idx보다 같거나 크면 모두 지운다.
// stack 의 bottom 의 인덱스값이 0 이고 top으로 올라가면서 커진다.
// [2011/1/20 jjuiddong]
//------------------------------------------------------------------------
void CStateMachine::PopState( int idx )
{
	const int size = (int)m_StateStack.size();
	for (int i = idx; i < size; ++i)
	{
		CState *p = m_StateStack.back();
		p->StateEnd();
		m_StateStack.pop_back();
		m_GarbageStack.push_back(p);
	}
}


//------------------------------------------------------------------------
// pstate의 sub State 즉 child State 를 모두 제거한다. pstate는 그대로 둔다.
// [2011/1/20 jjuiddong]
//------------------------------------------------------------------------
void CStateMachine::PopSubState( CState *pstate )
{
	int stackidx = GetStateStackIndex(pstate);
	if (stackidx < 0) return;

	PopState(stackidx + 1);
}


//------------------------------------------------------------------------
// pstate를 포함한 sub state(child state) 를 모두 제거한다.
// [2011/1/20 jjuiddong]
//------------------------------------------------------------------------
void CStateMachine::PopCurrentState( CState *pstate )
{
	int stackidx = GetStateStackIndex(pstate);
	if (stackidx < 0) return;

	PopState(stackidx);
}


//------------------------------------------------------------------------
// pstate 의 StackIndex를 리턴한다. 없다면 -1을 리턴한다.
// [2011/1/20 jjuiddong]
//------------------------------------------------------------------------
int CStateMachine::GetStateStackIndex( CState *pstate )
{
	int idx = 0;
	StateItor itor = m_StateStack.begin();
	while (m_StateStack.end() != itor)
	{
		CState *p = *itor++;
		if (pstate == p)
		{
			return idx;
		}
		++idx;
	}
	return -1;
}


//------------------------------------------------------------------------
// 클래스 초기화
// [2011/1/20 jjuiddong]
//------------------------------------------------------------------------
void CStateMachine::Clear()
{
	PopAllState();
}


//------------------------------------------------------------------------
// stack의 top 만 실행된다. 
// [2011/1/20 jjuiddong]
//------------------------------------------------------------------------
void CStateMachine::Update( float timeDelta )
{
	if (m_StateStack.empty()) return;

	CState *pstate = m_StateStack.back();
	pstate->Update(timeDelta);

	ClearGarbage();
}


//------------------------------------------------------------------------
// 
// [2011/1/20 jjuiddong]
//------------------------------------------------------------------------
BOOL CStateMachine::MsgDipatch(const SMessage &msg)
{
	StateItor itor = m_StateStack.begin();
	while (m_StateStack.end() != itor)
	{
		if (!(*itor++)->MsgDipatch(msg))
		{
			break;
		}
	}
	return TRUE;
}


//------------------------------------------------------------------------
// 
// [2011/1/21 jjuiddong]
//------------------------------------------------------------------------
void CStateMachine::Render()
{
	if (m_StateStack.empty()) return;

	CState *pstate = m_StateStack.back();
	pstate->Render();

}


//------------------------------------------------------------------------
// 
// [2011/1/21 jjuiddong]
//------------------------------------------------------------------------
void CStateMachine::ClearGarbage()
{
	StateItor itor = m_GarbageStack.begin();
	while (m_GarbageStack.end() != itor)
	{
		CState *p = *itor++;
		delete p;
	}
	m_GarbageStack.clear();
}
