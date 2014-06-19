
#ifndef __STATETHINK_H__
#define __STATETHINK_H__

#include "state.h"


class CStateThink : public CState
{
public:
	CStateThink(CUnit *punit);
	virtual ~CStateThink();

protected:

public:
	virtual void StateBegin();
	virtual void StateEnd();
	virtual void Update(float timeDelta);

};

#endif
