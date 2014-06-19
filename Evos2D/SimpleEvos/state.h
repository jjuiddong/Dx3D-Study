
#ifndef __STATE_H__
#define __STATE_H__

#include "messagedispatch.h"


class CStateMachine;
class CUnit;
class CState : public CMessageDispatch
{
public:
	CState(char *stateName, CUnit *punit) : m_pUnit(punit) 
	{
		if (stateName)
			strcpy_s(m_StateName, sizeof(m_StateName), stateName);
	}
	virtual ~CState() {}

private:
	CUnit *m_pUnit;
	char m_StateName[64];

public:
	virtual void StateBegin()=0;
	virtual void StateEnd()=0;
	virtual void Update(float timeDelta)=0;
	virtual void Render() {}
	CStateMachine* GetStateMachine();
	CUnit* GetUnit() { return m_pUnit; }
	char* GetStateName() { return m_StateName; }

};

#endif
