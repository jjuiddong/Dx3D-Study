
#ifndef  __STATEMACHINE_H__
#define __STATEMACHINE_H__

#include <list>
#include "messagedispatch.h"


class CState;
class CStateMachine : public CMessageDispatch
{
public:
	CStateMachine();
	virtual ~CStateMachine();

	typedef std::list<CState*> StateStack; // 가장 최근 데이타를 back에 저장한다.
	typedef StateStack::iterator StateItor;
protected:
	StateStack m_StateStack;
	StateStack m_GarbageStack;

public:
	void PushState( CState *pstate );
	void Update( float timeDelta );
	void Render();
	BOOL MsgDipatch(const SMessage &msg);
	void Clear();

	void PopAllState();
	void PopState( int idx );
	void PopSubState( CState *pstate );
	void PopCurrentState( CState *pstate );
	int GetStateStackIndex( CState *pstate );

protected:
	void ClearGarbage();

};

#endif
