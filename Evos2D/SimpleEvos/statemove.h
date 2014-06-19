
#ifndef __STATEMOVE_H__
#define __STATEMOVE_H__


#include "state.h"

class CUnit;
class CPathExplorer;
class CStateMove : public CState
{
public:
	CStateMove(CUnit *punit, CPathExplorer *ppathData);
	virtual ~CStateMove();

protected:
	CPathExplorer *m_pPathData;	// dijkstra
	CPathExplorer *m_pPathData2; // astar
	double m_RunningTime;
	double m_NextNodeArrivalTime;

public:
	virtual void StateBegin();
	virtual void StateEnd();
	virtual void Update(float timeDelta);
	virtual void Render();

};

#endif
