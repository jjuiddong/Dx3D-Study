
#ifndef __SHADOWSYSTEM_H__
#define __SHADOWSYSTEM_H__


class CLight;
class CTerrain;
class CModel;
class CShadowSystem
{
public:
	CShadowSystem();
	virtual ~CShadowSystem();
	enum { SHADOW_MAP_SIZE = 1024 };

protected:
	typedef std::list<CDispObject*> DispList;
	typedef DispList::iterator DispItor;

	// shadow
	LPD3DXEFFECT m_pEffect;
	D3DXHANDLE m_hTechnique;
	LPDIRECT3DVERTEXDECLARATION9 m_pDecl;

	LPDIRECT3DTEXTURE9	m_pShadowTex;
	LPDIRECT3DSURFACE9	m_pShadowSurf;

 	LPDIRECT3DTEXTURE9	m_pTerrainTex;
 	//LPDIRECT3DSURFACE9	m_pTerrainSurf;

	// edge
	CShader *m_pEdgeShader;

	LPDIRECT3DTEXTURE9 m_pEdgeTex;
	LPDIRECT3DSURFACE9 m_pEdgeSurf;

	LPDIRECT3DSURFACE9	m_pZ;	// Stencil buffer
	int m_ShadowMapWidth;
	int m_ShadowMapHeight;
	
	LPDIRECT3DSURFACE9 m_pOldBackBuffer;
	LPDIRECT3DSURFACE9 m_pOldZBuffer;
	D3DVIEWPORT9 m_oldViewport;

	Matrix44 m_mScaleBias;
	CLight *m_pLight; // Reference

	DispList m_DispList;

public:
	void Init();
	void Clear();

	// 깊이버퍼에 출력
	void BeginDepth(CLight *pLight);
	void RenderDepthBuffer( CDispObject *pDispObj );
	void EndDepth();

	void BeginTexture();
	void EndTexture();

	void BeginRenderShadow();
	void RenderScreen( CDispObject *pDispObj );
	void EndRenderShadow();

	// Edge
	void BeginEdge();
	void RenderModelEdge(CDispObject *pDispObj);
	void EndEdge();
	void EdgeFinalRender();

	void SetTerrainTexture(IDirect3DTexture9 *pTerrainTex) { m_pTerrainTex = pTerrainTex; }

	// Debug
	void RenderDebugTexture();

protected:
	void EdgeDepthRender(CDispObject *pDispObj);


};

#endif
