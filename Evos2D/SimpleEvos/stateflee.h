
#ifndef __STATEFLEE_H__
#define __STATEFLEE_H__


#include "state.h"

class CStateFlee : public CState
{
public:
	CStateFlee(CUnit *punit);
	virtual ~CStateFlee();

protected:

public:
	virtual void StateBegin();
	virtual void StateEnd();
	virtual void Update(float timeDelta);

};

#endif
