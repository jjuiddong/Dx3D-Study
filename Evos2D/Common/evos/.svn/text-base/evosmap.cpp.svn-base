
#include "stdafx.h"
#include "evosmap.h"

#include "../graph/HandyGraphFunctions.h"

using namespace std;

CEvosMap::CEvosMap() : 
m_Graph(false),
m_IsRenderID(false)
{

}


CEvosMap::~CEvosMap()
{
	Clear();

}


// NaviGraph 를 그리드 형태로 미리 생성해 놓는다.
//
void CEvosMap::Create(int width, int height, int cellsize)
{
	m_MapWidth = width;
	m_MapHeight = height;
	m_NumCellX = m_MapWidth / cellsize;
	m_NumCellY = m_MapHeight / cellsize;
	m_CellSize = cellsize;

}


//
//
void CEvosMap::Update(float timeDelta)
{

}


//
//
void CEvosMap::Render()
{
	GraphHelper_DrawUsingGDI(m_Graph, 0, m_IsRenderID);
	RenderWall();
}


void CEvosMap::Clear()
{
	m_Graph.Clear();

	WallItor itor = m_WallList.begin();
	while (m_WallList.end() != itor)
	{
		Wall2D *pwall = *itor++;
		delete pwall;
	}
	m_WallList.clear();

}


//------------------------------------------------------------------------
// 맵파일을 읽는다.
// Block 정보는 제외되고 Wall 오브젝트만 생성한다.
// [2011/1/13 jjuiddong]
//------------------------------------------------------------------------
BOOL CEvosMap::Read( char *filename )
{
	ifstream is(filename);
	if (!is.is_open()) 
	{
		return FALSE;
	}

	is >> m_MapWidth;
	is >> m_MapHeight;
	is >> m_CellSize;
	m_NumCellX = m_MapWidth / m_CellSize;
	m_NumCellY = m_MapHeight / m_CellSize;

	int blockSize;
	is >> blockSize;

	for (int i=0; i < blockSize; ++i)
	{
		int wallSize;
		is >> wallSize;
		for (int k=0; k < wallSize; ++k)
		{
			Wall2D *pwall = new Wall2D();
			pwall->Read(is);
			m_WallList.push_back(pwall);
		}
	}

	m_Graph.Load(is);

	is.close();

	// 노드맵을 설정한다.
	InitNodeMap();

	return TRUE;
}


//------------------------------------------------------------------------
// 
// [2011/1/13 jjuiddong]
//------------------------------------------------------------------------
void CEvosMap::RenderWall()
{
	WallItor itor = m_WallList.begin();
	while (itor != m_WallList.end())
	{
		(*itor)->Render(true);
		++itor;
	}
}


//------------------------------------------------------------------------
// 노드의 위치별로 map에 저장한다.
// map key = 노드의 위치 (x/cellSize + (map width/cellsize) * (y/cellSize))
// map data = node
// [2011/1/17 jjuiddong]
//------------------------------------------------------------------------
void CEvosMap::InitNodeMap()
{
	m_NodeMap.clear();

	const int nodeCount = m_Graph.NumNodes();
	for (int i=0; i < nodeCount; ++i)
	{
		NavGraphNode<> node = m_Graph.GetNode(i);
		const int key = GetNodeMapKey(node.Pos());
		m_NodeMap.insert(NodeMap::value_type(key, node.Index()));
	}
}


//------------------------------------------------------------------------
// NodeMap 의 키값을 리턴한다.
// [2011/1/17 jjuiddong]
//------------------------------------------------------------------------
int CEvosMap::GetNodeMapKey(Vector2D nodepos)
{
	double nodeX = max(0, nodepos.x);
	double nodeY = max(0, nodepos.y);

	const int cellSize = m_MapWidth / m_NumCellX;
	double x = nodeX / (double)cellSize;
	double y = nodeY / (double)cellSize;
	if (x >= m_NumCellX)
		x = m_NumCellX - 1;
	if (y >= m_NumCellY)
		y = m_NumCellY - 1;

	const int key = (int)x + (int)y * m_NumCellY;
	return key;
}


//------------------------------------------------------------------------
// pos 위치와 가장 가까운 노드를 리턴한다.
// [2011/1/17 jjuiddong]
//------------------------------------------------------------------------
NavGraphNode<>* CEvosMap::GetClosetNode(const Vector2D &pos)
{
	int key = GetNodeMapKey(pos);
	NodeMapItor itor = m_NodeMap.find(key);
	if (itor == m_NodeMap.end()) 
	{
		return _GetClosetNode(pos);
	}

	return &m_Graph.GetNode(itor->second);
}


//------------------------------------------------------------------------
// NodeMap에서 찾을 수 없을 때, graph 를 순회하면서 가장 가까운 노드를 리턴한다.
// [2011/1/19 jjuiddong]
//------------------------------------------------------------------------
NavGraphNode<>* CEvosMap::_GetClosetNode(const Vector2D &pos)
{
	int closetNodeIdx = -1;
	double closetLength = 100000.f;

	const int nodeCount = m_Graph.NumNodes();
	for (int i=0; i < nodeCount; ++i)
	{
		NavGraphNode<> &node = m_Graph.GetNode(i);
		const double len = node.Pos().DistanceSq(pos);
		if (closetLength > len)
		{
			closetLength = len;
			closetNodeIdx = i;
		}
	}

	if (closetNodeIdx == -1)
	{
		return NULL;
	}

	return &m_Graph.GetNode(closetNodeIdx);
}


//------------------------------------------------------------------------
// from 에서 to 로 곧장 지나갈 수 있다면 true를 리턴한다.
// [2011/1/19 jjuiddong]
//------------------------------------------------------------------------
BOOL CEvosMap::IsAvailableGoThrough(const Vector2D &from, const Vector2D &to)
{
	WallItor itor = m_WallList.begin();
	while (m_WallList.end() != itor)
	{
		Wall2D *pwall = *itor++;
		if (pwall->IsCross(from, to))
		{
			return FALSE;
		}
	}

	// 맵의 범위를 넘었다면 지나갈 수 없다.
	if (to.x < 0 || to.y < 0)
	{
		return FALSE;
	}
	if (to.x > m_MapWidth || to.y > m_MapHeight)
	{
		return FALSE;
	}

	return TRUE;
}

//------------------------------------------------------------------------
// 
// [2011/2/6 jjuiddong]
//------------------------------------------------------------------------
BOOL CEvosMap::IsAvailableGoThrough(CUnit *punit, int from, int to)
{
	const NavGraphNode<> &fromNode = m_Graph.GetNode(from);
	const NavGraphNode<> &toNode = m_Graph.GetNode(to);
	Vector2D pos1 = fromNode.Pos();
	Vector2D pos2 = toNode.Pos();

	return IsAvailableGoThrough(pos1, pos2);
}
