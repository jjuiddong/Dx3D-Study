//-----------------------------------------------------------------------------//
// 2005-10-04  programer: jaejung ┏(⊙д⊙)┛
// 
//-----------------------------------------------------------------------------//
#if !defined(__MESH_H__)
#define __MESH_H__


//-----------------------------------------------------------------------------//
// CMesh
//-----------------------------------------------------------------------------//
class CMesh
{
public:
	CMesh();
	CMesh( SMeshLoader *pLoader, BOOL bCollision );
	virtual ~CMesh();

public:
	typedef struct _tagSVtxInfo
	{
		DWORD dwFVF;
		int nStride;
		int nSize;
		int nTriangleCount;
		BYTE *pV;
	} SVtxInfo;

	int m_nId;
	BOOL m_isDynamicLoad;		// 동적으로 할당될 경우 true가 된다.
	SMeshLoader *m_pLoader;
	SMaterialLoader *m_pMtrl;
	IDirect3DTexture9 *m_pTex;
	IDirect3DVertexBuffer9 *m_pVtxBuff;
	IDirect3DIndexBuffer9 *m_pIdxBuff;
	Matrix44 m_matAniTM;
	SVtxInfo m_VtxInfo;
	Box *m_pBox;	// 충돌박스
	BOOL m_IsRenderCollisionBox;

public:
	int GetId() { return m_nId; }
	virtual BOOL Create( int id, int vtxBufSize, int stride, DWORD fvf, int idxBufSize);
	virtual void Animate( int Delta ) {}
	virtual void Render();
	virtual void RenderLineStrip();
	virtual BOOL LoadMesh( SMeshLoader *pLoader );
	virtual BOOL LoadAni( SKeyLoader *pKey ) { return TRUE; } // 파생 클래스에서 구현한다.
	void UpdateCollisionBox( Matrix44 *pMat );
	Box* GetCollisionBox() { return m_pBox; }

	BOOL Pick(const Matrix44 &parentTM, const Vector2 &pickPos);

	IDirect3DVertexBuffer9* getVtxBuffer() { return m_pVtxBuff; }
	IDirect3DIndexBuffer9* getIdxBuffer() { return m_pIdxBuff; }
	void setVtxBuffCount(int vtxCount);
	void setIdxBuffCount(int idxCount);
	int getVtxBuffCount();
	int getIdxBuffCount();
	void SetRenderCollisionBox(BOOL isRender) { m_IsRenderCollisionBox = isRender; }
	BOOL IsRenderCollisionBox() { return m_IsRenderCollisionBox; }

};

#endif // __JMESH_H__
