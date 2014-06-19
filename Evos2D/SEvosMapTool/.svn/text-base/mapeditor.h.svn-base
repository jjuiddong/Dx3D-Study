
#ifndef __MAPEDITOR_H__
#define __MAPEDITOR_H__


#include "../Common/global.h"
#include "../Common/evos/evosmap.h"

class CMapEditor : public CEvosMap
{
public:
	CMapEditor();
	virtual ~CMapEditor();

protected:
	typedef std::list<CBlock*> BlockList;
	typedef BlockList::iterator BlockItor;

	BlockList m_BlockList;
	Vector2D m_Offset;

public:
	BOOL AddBlock( CBlock *pWall );
	BOOL DelBlock( int index );
	CBlock* GetBlock( int index );
	CBlock* FindBlock( Vector2D *pPoint );
	void SetOffset(Vector2D offset) { m_Offset = offset; }
	Vector2D GetOffset() { return m_Offset; }
	BOOL GenerateNaviMap();
	void Clear();

	void Reset();
	BOOL Write( char *filename );
	BOOL Read( char *filename );

	virtual void Update(float timeDelta);
	virtual void Render();

protected:
	BlockItor GetBlockItor(int index);
	void GenerateNode(GraphType &graph, int *nodeTable, int from);
	BOOL IsInBlock(const Vector2D &pos);
	BOOL IsCrossPoint(int from, int to);
	int GetPositionToNodeIndex(const Vector2D &pos);

};

#endif // __MAPEDITOR_H__
