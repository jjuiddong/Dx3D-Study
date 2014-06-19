//-----------------------------------------------------------------------------//
// 2009-08-07  programer: jaejung ¦®(¢Á¬Õ¢Á)¦°
// 
//-----------------------------------------------------------------------------//

#if !defined(__MAP_EDITOR_H__)
#define __MAP_EDITOR_H__


#include "terrain.h"

class CTerrainCursor;
class CTerrainEditor : public CTerrain
{
public:
	CTerrainEditor();
	virtual ~CTerrainEditor();

protected:
	typedef struct _tagSelectObj
	{
		_tagSelectObj() {}
		_tagSelectObj(int id, OBJ_TYPE e, char *c, CModel* p) : nId(id), eOType(e), pModel(p) 
		{ 
			if (c) strcpy_s( szObjName, sizeof(szObjName), c );
		}
		int nId;
		OBJ_TYPE eOType;
		char szObjName[ 64];
		CModel* pModel;
	} SSelectObj;

	typedef std::list<SSelectObj>::iterator SelectItor;
	std::list<SSelectObj> m_SelectList;
	std::list<SSelectObj> m_CaptureList;
	BOOL m_bDrawChunkEdge;

public:
	virtual void Render();
	virtual void Update( int nElapsTick );
	virtual void Clear();
	BOOL Load( char *szFileName );
	BOOL Save( char *szFileName );

	// select object
	void SetSelectModel( OBJ_TYPE eOType, char *szObjName );
	void SelectModelCancel();
	void MoveSelectModel( CTerrainCursor *pCursor );
	BOOL LocateModel( CTerrainCursor *pCursor );
	BOOL IsSelected() { return !m_SelectList.empty(); }
	int Capture( Vector2 screenPos );
	BOOL CaptureToHover( LPPOINT lpPos );
	void UpdateObjPos();

	// Terrain
	void SetHeight( CTerrainCursor *pcursor, int nElapsTick );
	CChunk* SetChunkFocusing( CTerrainCursor *pcursor );

	// Brush
	void setVisibleBrush(BOOL visible);
	BOOL DrawBrush( CTerrainCursor *pcursor );

	// Chunk
	CChunk* GetChunkFromAxis( int x, int y );
	void OptimizeChunk();
	void ApplyHeightToChunk( CMesh *pheightrange );
	void SetDrawChunkEdgeOption( BOOL chunkdraw );

	// Layer
	BOOL MergeLayer( CChunk *pchunk, int layeridx_from, int layeridx_to );
	BOOL MoveLayer( CChunk *pchunk, int layeridx_from, int layeridx_to );

protected:
	void RenderTerrainCursor();
	void RenderSelectModel();
	void UpdateSelectModel(int nElapsTick);

};

#endif // __MAP_EDITOR_H__
