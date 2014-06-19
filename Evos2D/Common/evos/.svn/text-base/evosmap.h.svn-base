
#ifndef  __EVOSMAP_H__
#define  __EVOSMAP_H__

#include "../global.h"

#include <map>

class CUnit;
class CEvosMap
{
public:
	CEvosMap();
	virtual ~CEvosMap();

	typedef SparseGraph<NavGraphNode<>, NavGraphEdge> GraphType;
	typedef std::map<int, int> NodeMap;	// 노드의 위치가 키값이 된다. 노드 인덱스가 데이타가 된다.
	typedef NodeMap::iterator NodeMapItor;

protected:
	typedef std::list<Wall2D*> WallList;
	typedef WallList::iterator WallItor;

	WallList m_WallList;
	GraphType m_Graph;
	NodeMap m_NodeMap;

	int m_MapWidth;
	int m_MapHeight;
	int m_CellSize;
	int m_NumCellX;
	int m_NumCellY;
	bool m_IsRenderID;

public:
	virtual void Create(int width, int height, int cellsize);
	virtual void Update(float timeDelta);
	virtual void Render();
	void Clear();

	void SetIsShowID(bool isShowId) { m_IsRenderID = isShowId; }
	const GraphType& GetMap() { return m_Graph; }
	NavGraphNode<>* GetClosetNode(const Vector2D &pos);
	BOOL IsAvailableGoThrough(const Vector2D &from, const Vector2D &to);
	BOOL IsAvailableGoThrough(CUnit *punit, int from, int to);
	BOOL Read( char *filename );

protected:
	void InitNodeMap();
	void RenderWall();
	int GetNodeMapKey(Vector2D nodepos);
	NavGraphNode<>* _GetClosetNode(const Vector2D &pos);

};

#endif
