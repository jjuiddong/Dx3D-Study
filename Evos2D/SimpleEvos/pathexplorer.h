
#ifndef __PATHEXPLORER_H__
#define __PATHEXPLORER_H__

#include "../Common/global.h"
#include "Graph/PathEdge.h"

class CEvosMap;
class NavGraphEdge;
class CMovingObject;
class CPathExplorer
{
public:
	CPathExplorer(CUnit *powner);
	virtual ~CPathExplorer();

	typedef std::list<PathEdge> PathList;
	typedef PathList::iterator PathItor;

protected:
	CUnit *m_pOwner;
	PathList m_PathList;
	PathItor m_CurrentEdge;
	Vector2D m_StartPos;
	Vector2D m_DestPos;

public:
	void Init(const Vector2D &begin, const Vector2D &end);
	void Clear();

	PathList& GetPathList() { return m_PathList; }
	Vector2D GetNextNodePos();
	Vector2D GetPrevNodePos();
	Vector2D GetBeginNodePos();
	Vector2D GetEndNodePos();
	BOOL GetNext();
	BOOL IsArrive();
	BOOL IsNextNodeArrival( const Vector2D &curPos );
	double GetNextNodeArrivalTime( CMovingObject *pobj );
	void SetStartPos(const Vector2D &startPos) { m_StartPos = startPos; }
	const Vector2D& GetStartPos() const { return m_StartPos; }
	const Vector2D& GetDestPos() const { return m_DestPos; }
	CUnit* GetOwner() { return m_pOwner; }

	void PrintPath(); // debug
	void RenderPath(int penColor); // debug

protected:
	void SmoothPath();
	void CalculateEndPosition(const Vector2D &endPos);

};

#endif
