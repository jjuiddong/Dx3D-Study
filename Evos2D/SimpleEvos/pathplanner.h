
#ifndef __PATHPLANNER_H__
#define __PATHPLANNER_H__

#include "messagedispatch.h"

class CPathExplorer;
class CPathPalnner : public CMessageDispatch
{
public:
	CPathPalnner(CUnit *pOwner, const Vector2D &dest);
	virtual ~CPathPalnner();

protected:
	CUnit *m_pOwner;
	Vector2D m_DestPos;
	Graph_SearchTimeSliced<NavGraphEdge> *m_pSearch;

public:
	BOOL InitPathToTarget(SearchType type, const Vector2D &targetPos);
	int CycleOnce();
	CUnit* GetOwner() { return m_pOwner; }
	const Vector2D& GetDestPos();
	void Clear();
		
};

#endif
