
#include "stdafx.h"
#include "../Common/evos/evosmap.h"
#include "movingobject.h"

#include "SimpleEvos.h"
#include "dbg.h"
#include "Graph/PathEdge.h"
#include "pathexplorer.h"


CPathExplorer::CPathExplorer(CUnit *powner) : m_pOwner(powner)
{
	m_CurrentEdge = m_PathList.begin();
}


CPathExplorer::~CPathExplorer() 
{
	Clear();

}

Vector2D CPathExplorer::GetNextNodePos()
{
	if (m_CurrentEdge == m_PathList.end()) return Vector2D(-1,-1);
	return m_CurrentEdge->Destination();
}


Vector2D CPathExplorer::GetPrevNodePos()
{
	if (m_CurrentEdge == m_PathList.end()) return Vector2D(-1,-1);
	return m_CurrentEdge->Source();
}


//------------------------------------------------------------------------
// 목적지에 도착했다면 TRUE를 리턴한다.
// [2011/1/17 jjuiddong]
//------------------------------------------------------------------------
BOOL CPathExplorer::IsArrive()
{
	return m_CurrentEdge == m_PathList.end();
}


//------------------------------------------------------------------------
// CurrentNode에 도착했다면 true를 리턴한다.
// [2011/1/18 jjuiddong]
//------------------------------------------------------------------------
BOOL CPathExplorer::IsNextNodeArrival(const Vector2D &curPos)
{
	if (IsArrive()) return TRUE;
	return (10.f > m_CurrentEdge->Destination().DistanceSq(curPos));
}


//------------------------------------------------------------------------
// 다음 노드로 목표점을 바꾼다.
// [2011/1/17 jjuiddong]
//------------------------------------------------------------------------
BOOL CPathExplorer::GetNext()
{
	if (m_CurrentEdge == m_PathList.end()) 
	{
		return FALSE;
	}

	++m_CurrentEdge;

	if (m_CurrentEdge == m_PathList.end()) 
	{
		return FALSE;
	}

	return TRUE;
}


//------------------------------------------------------------------------
// 다음노드에 도착하는데 걸리는 시간을 리턴한다.
// [2011/1/17 jjuiddong]
//------------------------------------------------------------------------
double CPathExplorer::GetNextNodeArrivalTime( CMovingObject *pobj )
{
	Vector2D nextPos = GetNextNodePos();

	Vector2D dist = nextPos - pobj->GetPosition();
	double t = dist.Length() / (double)pobj->GetVelocity();

	return t;
}


//------------------------------------------------------------------------
// 클래스 초기화
// [2011/1/17 jjuiddong]
//------------------------------------------------------------------------
void CPathExplorer::Clear()
{
	m_PathList.clear();
	m_CurrentEdge = m_PathList.begin();
}


//------------------------------------------------------------------------
// 길찾기 첫번째 노드로 currentPos를 이동한다.
// [2011/1/17 jjuiddong]
//------------------------------------------------------------------------
void CPathExplorer::Init(const Vector2D &begin, const Vector2D &end)
{
	CalculateEndPosition(end);

	SetStartPos(begin);

	const Vector2D front = GetBeginNodePos();
	const Vector2D back = GetEndNodePos();
	m_PathList.push_front(PathEdge(begin, front, 0));
	m_PathList.push_back(PathEdge(back, m_DestPos, 0));

	SmoothPath();
	m_CurrentEdge = m_PathList.begin();
}



//------------------------------------------------------------------------
// 디버깅용 함수
// [2011/1/19 jjuiddong]
//------------------------------------------------------------------------
void CPathExplorer::PrintPath()
{
/*
	g_Dbg.Print("----path list----\n");
	PathItor itor = m_PathList.begin();
	while (m_PathList.end() != itor)
	{
		int nodeIdx = *itor++;
		g_Dbg.Print("%d ", nodeIdx);
	}
	g_Dbg.Print("\n");
/**/
}


//------------------------------------------------------------------------
// 시작 노드의 위치를 리턴한다.
// [2011/1/19 jjuiddong]
//------------------------------------------------------------------------
Vector2D CPathExplorer::GetBeginNodePos()
{
	if (m_PathList.empty()) return Vector2D(-1,-1);

	const PathEdge &beginEdge = m_PathList.front();
	return beginEdge.Source();
}

//------------------------------------------------------------------------
// 마지막 노드의 위치를 리턴한다.
// [2011/1/19 jjuiddong]
//------------------------------------------------------------------------
Vector2D CPathExplorer::GetEndNodePos()
{
	if (m_PathList.empty()) return Vector2D(-1,-1);

	const PathEdge &endEdge = m_PathList.back();
	return endEdge.Destination();
}


//------------------------------------------------------------------------
// 경로를 화면에 출력한다.
// pencolor : CGdi::enum
// [2011/1/21 jjuiddong]
//------------------------------------------------------------------------
void CPathExplorer::RenderPath(int penColor)
{
	PathList &path = GetPathList();
	PathList::iterator itor = path.begin();

	gdi->SetPenColor(penColor);
	CEvosMap *pmap = GetGame()->GetMap();
	while (itor != path.end())
	{
		Vector2D dest = itor->Destination();
		Vector2D src = itor->Source();
		gdi->Line(src.x, src.y, dest.x, dest.y);
		++itor;
	}
}


//------------------------------------------------------------------------
// 경로의 마지막에서 destPos로 갈 수 있다면 목적지를 destPos로 결정한다. 
// destPos로 갈 수없다면 destPos에서 가장 가까운 노드가 destPos가 된다.
// [2011/2/5 jjuiddong]
//------------------------------------------------------------------------
void CPathExplorer::CalculateEndPosition(const Vector2D &destPos)
{
	// 마지막 노드에서 목적지까지 갈 수있는지 없는지 판단해서
	// 목적지를 재설정 한다.
	CEvosMap *pmap = GetGame()->GetMap();
	Vector2D endPos = GetEndNodePos();
	if (pmap->IsAvailableGoThrough(endPos, destPos))
	{
		m_DestPos = destPos;
	}
	else
	{
		m_DestPos = endPos;
	}
}


//------------------------------------------------------------------------
// 
// [2011/2/6 jjuiddong]
//------------------------------------------------------------------------
void CPathExplorer::SmoothPath()
{
	if (m_PathList.empty()) return;

	CEvosMap *pmap = GetGame()->GetMap();
	PathItor i = m_PathList.begin();
	while (i != m_PathList.end())
	{
		PathItor k = i;
		++k;
		if (k == m_PathList.end())
		{
			break;
		}

		Vector2D pos1 = i->Source();
		while (k != m_PathList.end())
		{
			Vector2D pos2 = k->Destination();
			if (pmap->IsAvailableGoThrough(pos1, pos2))
			{
				k->SetSource(i->Source());
				k = m_PathList.erase(i);
				i = k;
				++k;
			}
			else
			{
				i = k;
				break;
			}
		}
	}

}
