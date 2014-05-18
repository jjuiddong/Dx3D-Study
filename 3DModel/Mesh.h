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

protected:
	typedef struct _tagSVtxInfo
	{
		DWORD dwFVF;
		int nStride;
		int nSize;
		BYTE *pV;
	} SVtxInfo;

	int m_nId;
	int m_test;
	SMeshLoader *m_pLoader;
	SMaterialLoader *m_pMtrl;
	IDirect3DTexture9 *m_pTex;
	IDirect3DVertexBuffer9 *m_pVtxBuff;
	IDirect3DIndexBuffer9 *m_pIdxBuff;
	Matrix44 m_matAniTM;
	SVtxInfo m_VtxInfo;
	Box *m_pBox;							// 충돌박스

public:
	int GetId() { return m_nId; }
	virtual void Animate( int Delta ) {}
	virtual void Render();
	virtual BOOL LoadMesh( SMeshLoader *pLoader );
	virtual BOOL LoadAni( SKeyLoader *pKey ) { return TRUE; } // 파생 클래스에서 구현한다.
	void UpdateCollisionBox( Matrix44 *pMat );
	Box* GetCollisionBox() { return m_pBox; }

};

#endif // __JMESH_H__
