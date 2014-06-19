
#include "stdafx.h"
#include "global.h"
#include "graph/TimeSlicedGraphAlgorithms.h"
#include "graph/GraphEdgeTypes.h"
#include "evos/evosmap.h"
#include "pathplanner.h"



CPathPalnner::CPathPalnner(CUnit *pOwner, const Vector2D &dest)
: m_pOwner(pOwner), m_DestPos(dest)
{
	m_pSearch = NULL;
}


CPathPalnner::~CPathPalnner()
{
	SAFE_DELETE(m_pSearch);

}


//------------------------------------------------------------------------
// 
// [2011/2/4 jjuiddong]
//------------------------------------------------------------------------
int CPathPalnner::CycleOnce()
{
	if (!m_pSearch) return 0;

	int result = m_pSearch->CycleOnce();

	if (result == target_not_found)
	{
		SendMsg( SMessage(MSG_NOPATHAVAILABLE, m_pOwner, NULL, 0) );
	}
	else if (result == target_found)
	{
		CPathExplorer *pexplorer = m_pOwner->GetPathData();
		pexplorer->Clear();
		m_pSearch->GetPathAsPathEdges(&pexplorer->GetPathList());
		pexplorer->Init(m_pOwner->GetPosition(), m_DestPos);
		SendMsg( SMessage(MSG_PATHREADY, m_pOwner, NULL, 0) );
	}

	return result;
}


//------------------------------------------------------------------------
// 
// [2011/2/5 jjuiddong]
//------------------------------------------------------------------------
BOOL CPathPalnner::InitPathToTarget(SearchType type, const Vector2D &targetPos)
{
	CEvosMap *pmap = GetGame()->GetMap();
	NavGraphNode<> *pdestNode = pmap->GetClosetNode(targetPos);
	if (!pdestNode) return FALSE;

	NavGraphNode<> *psrcNode = pmap->GetClosetNode(m_pOwner->GetPosition());
	if (!psrcNode) return FALSE;

	SAFE_DELETE(m_pSearch);

	m_DestPos = targetPos;
	if (type == AStar)
	{
		m_pSearch = new Graph_SearchAStar_TS<CEvosMap::GraphType, Heuristic_Euclid>(
			pmap->GetMap(), psrcNode->Index(), pdestNode->Index());
	}
	else 
	{
		// Dijkstra
		m_pSearch = new Graph_SearchDijkstras_TS<CEvosMap::GraphType, FindNodeIndex>(
			pmap->GetMap(), psrcNode->Index(), pdestNode->Index());
	}

	return TRUE;
}


//------------------------------------------------------------------------
// 
// [2011/2/5 jjuiddong]
//------------------------------------------------------------------------
void CPathPalnner::Clear()
{
	SAFE_DELETE(m_pSearch);

}

