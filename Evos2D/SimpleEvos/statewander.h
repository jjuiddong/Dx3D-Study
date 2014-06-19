
#ifndef __STATEWANDER_H__
#define __STATEWANDER_H__

#include "state.h"
#include "../Common/global.h"


class CPathExplorer;
class CStateWander : public CState
{
public:
	CStateWander(CUnit *punit);
	virtual ~CStateWander();

protected:

public:
	virtual void StateBegin();
	virtual void StateEnd();
	virtual void Update(float timeDelta);
	virtual void Render();

};

#endif
