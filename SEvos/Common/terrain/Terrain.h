//-----------------------------------------------------------------------------//
// 2009-08-05  programer: jaejung ┏(⊙д⊙)┛
//
// 2011-02-09 주석 추가
//
//-----------------------------------------------------------------------------//

#if !defined (__MAP_H__)
#define __MAP_H__

#include "chunk.h"
#include "region.h"


/*
<Chunk가 저장된 순서>

0 1	2 3	4 5 6 7	| 8 9 10 ~~~ CHUNK_COL-1
CHUNK_COL...	|
				|
				|
				|Pos(0,0)
------------------------------------->x
				|
				|
				|
				|
				| ~~~~~~~~~~~~~~~~(CHUNK_COL * CHUNK_ROW)-1

/**/

class CWater;
class CSkyBox;
class CLight;
class CQuadTree;
class CModel;
class CChunkManager;
class CShadowSystem;
class CTerrain : public CRegion
{
public:
	CTerrain();
	virtual ~CTerrain();

protected:
	typedef struct _tagSTerrainStructure
	{
		_tagSTerrainStructure() {}
		_tagSTerrainStructure( int i, CModel *p ) : nId(i), pModel(p) {}
		int nId;
		int nModelIdx;
		char szObjName[ 64]; // 임시
		Vector2 vPos;
		CModel *pModel;
		BOOL operator==( const _tagSTerrainStructure &rhs ) { return nId==rhs.nId; }
	} STerrainStructure;

public:
	enum
	{
		// map vtx:			32 * 32 = 1024
		// map cell size:	10
		CHUNK_COL = 4,
		CHUNK_ROW = 4,
	};

protected:
	BOOL m_bLoaded;
	float m_fHeightScale;
	int m_nRenderType;
	BOOL m_bFog;
	BOOL m_IsDisplayQuadTree;
	BOOL m_IsDisplayNormal;
	BOOL m_IsDisplayShadow;

	CSkyBox *m_pSkyBox;
	CLight *m_pLight;

	// quad tree
	CQuadTree *m_pQuadTree;

	// chunk
	CChunkManager *m_pChunkMng;

	// shadow
	CShadowSystem *m_pShadowSystem;

	std::list< STerrainStructure > m_DynList;
	std::list< STerrainStructure > m_RigidList;
	static int m_nTModelId;
	typedef std::list< STerrainStructure >::iterator ModelItor;

	IDirect3DIndexBuffer9 *m_pDynamicIdxBuff; // QuadTree에서 동적으로 생성하는 index를 저장한다.
	IDirect3DTexture9 *m_pTexture;

public:
	virtual void Render();
	virtual void RenderPostEffect();
	virtual void Update( int nElapsTick );
	virtual void UpdateEdge();
	virtual void Clear();

	BOOL Create( int nVtxRow, int nVtxCol, float cellSize, float fHeightScale, char *textureFileName );
	BOOL Load( char *szHeightFileName, int nVtxRow, int nVtxCol, float cellSize, float fHeightScale );
	BOOL Load( char *szFileName );
	BOOL LoadTexture( char *szFileName );
	BOOL AddTerrainStructure( Vector3 vPos, OBJ_TYPE eOType, char *szObjName, CModel *pAdd );
	BOOL DelTerrainStructure( OBJ_TYPE eOType, int nId );
	BOOL IsLoaded() { return m_bLoaded; }
	void SetFog( DWORD dwColor, float fMin, float fMax );
	void FogEnable( BOOL bEnable ) { m_bFog = bEnable; }
	void SetRenderType( int nRenderType ) { m_nRenderType = nRenderType; }
	void SetDisplayQuadTree(BOOL displayQuadTree) { m_IsDisplayQuadTree = displayQuadTree; }
	void SetDisplayNormal(BOOL displayNormal) { m_IsDisplayNormal = displayNormal; }
	void SetDisplayShadow(BOOL displayShadow) { m_IsDisplayShadow = displayShadow; }

	CLight* GetLight() { return m_pLight; }

protected:
	virtual void CreateIndexBuffer();
	void RenderQuadTree();
	void RenderShadow();
	void RenderNotShadow();
	float CalcShade( int nCellRow, int nCellCol, Vector3 *pvLightDir );
	void SetRenderState();
	BOOL ReadRawFile( char *szFileName );
	void RenderStructure();
	void RenderStructureToDepthBuffer();
	void RenderStructureEdge();

};

#endif // __MAP_H__
