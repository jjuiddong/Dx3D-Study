
#ifndef __CHUNKMANAGER_H__
#define __CHUNKMANAGER_H__

//------------------------------------------------------------------------
// Chunk를 관리하는 클래스다.
//
// [2011/2/28 jjuiddong]
//------------------------------------------------------------------------

#include "model/dispObject.h"
#include "terrain/chunk.h"


class CWater;
class CShader;
class CTerrain;
class CTerrainCursor;
class CChunkManager : public CDispObject
{
public:
	CChunkManager();
	virtual ~CChunkManager();
	enum { SHADOW_MAP_SIZE = 2048 };

protected:
	int m_ObjId;
	CChunk *m_pChunk; // [col*row]
	CTerrain *m_pTerrain; // reference
	int m_ColChunkCount;
	int m_RowChunkCount;
	int m_ChunkSize;
	int m_ColumnVtxCountPerChunk;
	int m_RowVtxCountPerChunk;
	int m_ColumnCellCountPerChunk;
	int m_RowCellCountPerChunk;
	Matrix44 m_matWorld;

	// water
	CWater *m_pWater;

	// loader
	SChunkGroupLoader *m_pLoader;

	// shader
	CShader *m_pShader;

	// Terrain Texture
	int m_TerrainMapWidth;
	int m_TerrainMapHeight;
	LPDIRECT3DTEXTURE9 m_pTerrainTex;
	LPDIRECT3DSURFACE9 m_pTerrainSurf;
	LPDIRECT3DSURFACE9 m_pZ;	// Stencil buffer

	LPDIRECT3DTEXTURE9 m_pTerrainDepth;
	LPDIRECT3DSURFACE9 m_pTerrainDepthSurf;

	Matrix44 m_mScaleBias;

public:
	void Init(CTerrain *pTerrain, int colChunkCount, int rowChunkCount);
	void Load(CTerrain *pTerrain, SChunkGroupLoader *pLoader);
	void RenderDepth();
	void RenderEdge();
	virtual int GetId() { return m_ObjId; }
	virtual void Render();
	void RenderDirect();
	void RenderDebugTexture();
	virtual BOOL Update( int elapseTime );
	void UpdateEdge();
	void SetHeight( CTerrainCursor *pcursor, int nElapsTick );
	BOOL DrawBrush( CTerrainCursor *pcursor );

	CChunk* GetChunkFromPos( Vector3 pickPos, float *pu=NULL, float *pv=NULL );
	CChunk* GetChunkFromScreenPos( Vector2 screenPos, float *pu=NULL, float *pv=NULL );
	CChunk* SetFocus( CTerrainCursor *pcursor );
	CChunk* GetChunkFrom( int x, int y );
	IDirect3DTexture9* GetTerrainTexture() { return m_pTerrainTex; }

	// file
	SChunkGroupLoader* GetChunkGroupLoader( char *szAlphaTextureWriteLocatePath );

	virtual Matrix44* GetWorldTM() { return &m_matWorld; }
	void UpdateChunkToTerrain();
	void UpdateTerrainToChunk();
	void Clear();

protected:
	void RenderChunks(BOOL isRenderTerrainBuffer, BOOL isRenderDeptBuffer);
	void SetRenderState();
	SChunkGroupLoader* NewChunkGroupLoader();
	void DeleteChunkGroupLoader( SChunkGroupLoader *pLoader);

};

#endif
