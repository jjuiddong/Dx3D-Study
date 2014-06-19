
#ifndef __STATEARRIVE_H__
#define __STATEARRIVE_H__


#include "state.h"
#include "../Common/global.h"

class CStateArrive : public CState
{
public:
	CStateArrive(CUnit *punit, const Vector2D &pos);
	virtual ~CStateArrive();

protected:
	Vector2D m_DestPos;

public:
	virtual void StateBegin();
	virtual void StateEnd();
	virtual void Update(float timeDelta);

};

#endif
