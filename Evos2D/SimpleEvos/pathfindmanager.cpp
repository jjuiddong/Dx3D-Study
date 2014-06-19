
#include "stdafx.h"
#include "../Common/global.h"
#include "../Common/graph/GraphAlgorithms.h"
#include "../Common/graph/AStarHeuristicPolicies.h"

#include "../Common/evos/evosmap.h"
#include "pathfindmanager.h"
#include "unit.h"
#include "graph/TimeSlicedGraphAlgorithms.h"
#include "graph/GraphEdgeTypes.h"
#include "pathplanner.h"
#include <algorithm>

#include "pathexplorer.h"


class is_pathplanner: public std::unary_function<CUnit*, bool>
{
public:
	is_pathplanner(CUnit *p):m_p(p) {}
	CUnit *m_p;
	bool operator ()(CPathPalnner *p) const
	{
		if (p->GetOwner() == m_p)
			return true;
		return false;
	}
};


//------------------------------------------------------------------------
// 
// [2011/1/17 jjuiddong]
//------------------------------------------------------------------------
BOOL CPathFindManager::SearchMap( CEvosMap *pmap, CUnit *pobject, const Vector2D &dest, CPathExplorer *ppathdata )
{
	NavGraphNode<> *pdestNode = pmap->GetClosetNode(dest);
	if (!pdestNode) return FALSE;

	NavGraphNode<> *psrcNode = pmap->GetClosetNode(pobject->GetPosition());
	if (!psrcNode) return FALSE;

	Graph_SearchDijkstra<CEvosMap::GraphType> dijkstra(pmap->GetMap(), psrcNode->Index(), pdestNode->Index());
	ppathdata->Clear();
//	dijkstra.GetPathToTarget(&ppathdata->GetPathList());
//	ppathdata->Init();
	return TRUE;
}


//------------------------------------------------------------------------
// 
// [2011/2/4 jjuiddong]
//------------------------------------------------------------------------
BOOL CPathFindManager::SearchMapAstar( CEvosMap *pmap, CUnit *pobject, const Vector2D &dest, CPathExplorer *ppathdata )
{
	NavGraphNode<> *pdestNode = pmap->GetClosetNode(dest);
	if (!pdestNode) return FALSE;

	NavGraphNode<> *psrcNode = pmap->GetClosetNode(pobject->GetPosition());
	if (!psrcNode) return FALSE;

	Graph_SearchAStar<CEvosMap::GraphType, Heuristic_Euclid> astar(
		pmap->GetMap(), psrcNode->Index(), pdestNode->Index());
	ppathdata->Clear();
//	astar.GetPathToTarget(&ppathdata->GetPathList());
//	ppathdata->Init();

	return TRUE;
}


//------------------------------------------------------------------------
// 
// [2011/2/4 jjuiddong]
//------------------------------------------------------------------------
void CPathFindManager::Register(CUnit *punit, const Vector2D &dest)
{
	if (IsExist(punit))
	{
		CPathPalnner *pathPlanner = GetPathPlanner(punit);
		pathPlanner->InitPathToTarget(AStar, dest);
	}
	else
	{
		CPathPalnner *pathPlanner = new CPathPalnner(punit, dest);
		pathPlanner->InitPathToTarget(AStar, dest);
		m_PlannerList.push_back(pathPlanner);
	}
}


//------------------------------------------------------------------------
// 
// [2011/2/5 jjuiddong]
//------------------------------------------------------------------------
BOOL CPathFindManager::IsExist(CUnit *punit)
{
	PlannerItor itor = find_if(m_PlannerList.begin(), m_PlannerList.end(), is_pathplanner(punit));
	if (m_PlannerList.end() != itor)
		return TRUE;
	return FALSE;
}


//------------------------------------------------------------------------
// 
// [2011/2/5 jjuiddong]
//------------------------------------------------------------------------
CPathPalnner* CPathFindManager::GetPathPlanner(CUnit *punit)
{
	PlannerItor itor = find_if(m_PlannerList.begin(), m_PlannerList.end(), is_pathplanner(punit));
	if (m_PlannerList.end() != itor)
		return *itor;
	return NULL;
}


//------------------------------------------------------------------------
// 
// [2011/2/5 jjuiddong]
//------------------------------------------------------------------------
void CPathFindManager::DeletePathPlanner(CUnit *punit)
{
	PlannerItor newend = remove_if(m_PlannerList.begin(), m_PlannerList.end(), is_pathplanner(punit));
	m_PlannerList.erase(newend, m_PlannerList.end());
}


//------------------------------------------------------------------------
// 
// [2011/2/5 jjuiddong]
//------------------------------------------------------------------------
void CPathFindManager::Update(float timeDelta)
{
	int NumCyclesRemaining = m_iNumSearchCyclesPerUpdate;

	PlannerItor curPath = m_PlannerList.begin();
	while(NumCyclesRemaining > 0 && !m_PlannerList.empty())
	{
		--NumCyclesRemaining;

		CPathPalnner *p = *curPath;
		int result = p->CycleOnce();
		if ((result == target_found) || (result == target_not_found))
		{
			curPath = m_PlannerList.erase(curPath);
		}
		else
		{
			++curPath;
		}

		if (m_PlannerList.end() == curPath)
		{
			curPath = m_PlannerList.begin();
		}
	}

}

