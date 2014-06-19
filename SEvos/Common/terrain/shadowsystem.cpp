
#include "StdAfx.h"
#include "common/light.h"
#include "terrain.h"
#include "model/Model.h"
#include "common/shader.h"
#include "shadowsystem.h"



CShadowSystem::CShadowSystem() :
m_pEffect(NULL)
, m_pEdgeShader(NULL)
, m_hTechnique(NULL)
, m_pDecl(NULL)
, m_pShadowTex(NULL)
, m_pShadowSurf(NULL)
//, m_pTerrainSurf(NULL)
, m_pTerrainTex(NULL)
, m_pZ(NULL)
, m_pLight(NULL)
{

}


CShadowSystem::~CShadowSystem()
{
	Clear();

}


//------------------------------------------------------------------------
// 
// [2011/3/15 jjuiddong]
//------------------------------------------------------------------------
void CShadowSystem::Init()
{
	g_pDevice->GetViewport(&m_oldViewport);
	const int shadow_map_size_w = m_oldViewport.Width;
	const int shadow_map_size_h = m_oldViewport.Height;
	m_ShadowMapWidth = shadow_map_size_w;
	m_ShadowMapHeight = shadow_map_size_h;

	// 쉐이더 파일 읽기
	HRESULT hr;
	LPD3DXBUFFER pErr;
	if (FAILED(hr = D3DXCreateEffectFromFile(g_pDevice, "hlsl.fx", NULL, NULL, 
		D3DXSHADER_DEBUG , NULL, &m_pEffect, &pErr))) {
			MessageBox( NULL, (LPCTSTR)pErr->GetBufferPointer(), "ERROR", MB_OK);
			//DXTRACE_ERR( "CreateEffectFromFile", hr );
			return;
	}

	m_pEdgeShader = new CShader();
	m_pEdgeShader->Init( "hlsl_edge.fx", "TShader" );

	// 그림자맵 생성
	if (FAILED(g_pDevice->CreateTexture(shadow_map_size_w, shadow_map_size_h, 1, 
		D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8,
		D3DPOOL_DEFAULT, &m_pShadowTex, NULL)))
		return;

	if (FAILED(m_pShadowTex->GetSurfaceLevel(0, &m_pShadowSurf)))
		return;

	if (FAILED(g_pDevice->CreateDepthStencilSurface(
		shadow_map_size_w, shadow_map_size_h, 
		D3DFMT_D16, D3DMULTISAMPLE_NONE, 0,
		TRUE, &m_pZ, NULL)))
		return;

	// edge buffer
	if (FAILED(g_pDevice->CreateTexture(shadow_map_size_w, shadow_map_size_h, 1, 
		D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8,
		D3DPOOL_DEFAULT, &m_pEdgeTex, NULL)))
		return;

	if (FAILED(m_pEdgeTex->GetSurfaceLevel(0, &m_pEdgeSurf)))
		return;


// 	if (FAILED(g_pDevice->CreateTexture(shadow_map_size_w, shadow_map_size_h, 1, 
// 		D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8,
// 		D3DPOOL_DEFAULT, &m_pTerrainTex, NULL)))
// 		return;
// 
// 	if (FAILED(m_pTerrainTex->GetSurfaceLevel(0, &m_pTerrainSurf)))
// 		return;

	m_hTechnique = m_pEffect->GetTechniqueByName( "TShader" );

	D3DVERTEXELEMENT9 decl[] =
	{
		{0,  0, D3DDECLTYPE_FLOAT4,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
		{0, 12, D3DDECLTYPE_FLOAT4,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL,	0},
		{0, 24, D3DDECLTYPE_FLOAT2,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0},
		D3DDECL_END()
	};

	if (FAILED(hr = g_pDevice->CreateVertexDeclaration(decl, &m_pDecl)))
	{
		//DXTRACE_ERR ("CreateVertexDeclaration", hr);
		return;
	}

	// 투영공간에서 텍스처공간으로 변환
	const float fOffsetX = 0.5f + (0.5f / (float)shadow_map_size_w);
	const float fOffsetY = 0.5f + (0.5f / (float)shadow_map_size_h);
	Matrix44 mScaleBias(	
		0.5f,		0.0f,	0.0f,   0.0f,
		0.0f,		-0.5f,	0.0f,   0.0f,
		0.0f,		0.0f,	0.0f,	0.0f,
		fOffsetX, fOffsetY, 0.0f,   1.0f );

	m_mScaleBias = mScaleBias;

}


//------------------------------------------------------------------------
// 
// [2011/3/15 jjuiddong]
//------------------------------------------------------------------------
void CShadowSystem::BeginDepth(CLight *pLight)
{
	m_pLight = pLight;

	g_pDevice->GetRenderTarget(0, &m_pOldBackBuffer);
	g_pDevice->GetDepthStencilSurface(&m_pOldZBuffer);
	g_pDevice->GetViewport(&m_oldViewport);

	// 깊이버퍼로 타겟을 변경한다.
	g_pDevice->SetRenderTarget(0, m_pShadowSurf);
	g_pDevice->SetDepthStencilSurface(m_pZ);
	D3DVIEWPORT9 viewport = {0,0, m_oldViewport.Width, m_oldViewport.Height, 0.0f,1.0f};
	g_pDevice->SetViewport(&viewport);

	g_pDevice->Clear(0L, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0xffffff, 1.0f, 0L);

	m_pEffect->SetTechnique( m_hTechnique );
	g_pDevice->SetVertexDeclaration( m_pDecl );

	m_pEffect->Begin(NULL, 0);

}


//------------------------------------------------------------------------
// 
// [2011/3/15 jjuiddong]
//------------------------------------------------------------------------
void CShadowSystem::EndDepth()
{
	m_pEffect->EndPass();
	m_pEffect->End();
}


//------------------------------------------------------------------------
// 
// [2011/3/15 jjuiddong]
//------------------------------------------------------------------------
void CShadowSystem::BeginTexture()
{
// 	g_pDevice->SetRenderTarget(0, m_pTerrainSurf);
// 	g_pDevice->Clear(0L, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0xffffffff, 1.0f, 0L);

}


//------------------------------------------------------------------------
// 
// [2011/3/15 jjuiddong]
//------------------------------------------------------------------------
void CShadowSystem::EndTexture()
{
	// 아직 아무일 없음

}


//------------------------------------------------------------------------
// 텍스쳐와 깊이버퍼를 합성해서 그림자를 출력한다.
// [2011/3/15 jjuiddong]
//------------------------------------------------------------------------
void CShadowSystem::BeginRenderShadow()
{
	if (!m_pLight)
		return;

	// 렌더링 타겟 복구
 	g_pDevice->SetRenderTarget(0, m_pOldBackBuffer);
 	g_pDevice->SetDepthStencilSurface(m_pOldZBuffer);
 	g_pDevice->SetViewport(&m_oldViewport);
 	m_pOldBackBuffer->Release();
 	m_pOldZBuffer->Release();

	m_pEffect->Begin(NULL, 0);

	m_pEffect->SetTexture("ShadowMap", m_pShadowTex);
	m_pEffect->SetTexture("DecaleMap", m_pTerrainTex);
	m_pEffect->CommitChanges();

}


//------------------------------------------------------------------------
// 
// [2011/3/15 jjuiddong]
//------------------------------------------------------------------------
void CShadowSystem::EndRenderShadow()
{
	m_pEffect->EndPass();
	m_pEffect->End();
}


//------------------------------------------------------------------------
// 
// [2011/3/15 jjuiddong]
//------------------------------------------------------------------------
void CShadowSystem::RenderDepthBuffer( CDispObject *pDispObj )
{
	m_pEffect->BeginPass(0);

	Matrix44 mWLP;
	m_pLight->GetViewProjectMatrix(&mWLP);
	mWLP = *pDispObj->GetWorldTM() * mWLP;
	m_pEffect->SetMatrix( "mWLP", (D3DXMATRIX*)&mWLP );
	m_pEffect->CommitChanges();

	pDispObj->Render();

	m_pEffect->EndPass();
}


//------------------------------------------------------------------------
// 
// [2011/3/15 jjuiddong]
//------------------------------------------------------------------------
void CShadowSystem::RenderScreen( CDispObject *pDispObj )
{
	m_pEffect->BeginPass(1);

	Matrix44 mWLP;
	m_pLight->GetViewProjectMatrix(&mWLP);
	mWLP = *pDispObj->GetWorldTM() * mWLP;
	m_pEffect->SetMatrix( "mWLP", (D3DXMATRIX*)&mWLP );

	Matrix44 mWLPB = mWLP * m_mScaleBias;
	m_pEffect->SetMatrix( "mWLPB", (D3DXMATRIX*)&mWLPB );

	Matrix44 mWVP;
	g_Camera.GetViewProjMatrix(&mWVP);
	mWVP = *pDispObj->GetWorldTM() * mWVP;
	m_pEffect->SetMatrix( "mWVP", (D3DXMATRIX*)&mWVP);

	Matrix44 mWVPB = mWVP * m_mScaleBias;
	m_pEffect->SetMatrix( "mWVPB", (D3DXMATRIX*)&mWVPB );

	m_pEffect->SetTexture("ShadowMap", m_pShadowTex);
	m_pEffect->SetTexture("DecaleMap", m_pTerrainTex);
	m_pEffect->CommitChanges();

	pDispObj->RenderDepth();

	m_pEffect->EndPass();
}


//------------------------------------------------------------------------
// 깊이버퍼와 텍스쳐를 화면에 출력한다. 
// [2011/3/15 jjuiddong]
//------------------------------------------------------------------------
void CShadowSystem::RenderDebugTexture()
{
	g_pDevice->SetRenderState( D3DRS_LIGHTING, FALSE);

	g_pDevice->SetTextureStageState(0,D3DTSS_COLOROP,	D3DTOP_SELECTARG1);
	g_pDevice->SetTextureStageState(0,D3DTSS_COLORARG1,	D3DTA_TEXTURE);
	g_pDevice->SetTextureStageState(1,D3DTSS_COLOROP,   D3DTOP_DISABLE);
	for (int i=0; i < 3; ++i)
	{
		float y = 128;
		float scale = 128;
		SVtxRhwTex Vertex[4] = {
			// x  y  z rhw tu tv
			{Vector3(0,			y*(i+1),		0), 1, 0, 0,},
			{Vector3(scale,		y*(i+1),		0), 1, 1, 0,},
			{Vector3(scale,	y*(i+1)+scale,	0), 1, 1, 1,},
			{Vector3(0,		y*(i+1)+scale,	0), 1, 0, 1,},
		};

		LPDIRECT3DTEXTURE9 ptex;
		if (i==0) ptex = m_pShadowTex;
		if (i==1) ptex = m_pTerrainTex;
		if (i==2) ptex = m_pEdgeTex;
		g_pDevice->SetTexture( 0, ptex);
		g_pDevice->SetVertexShader(NULL);
		g_pDevice->SetFVF( D3DFVF_XYZRHW | D3DFVF_TEX1 );
		g_pDevice->SetPixelShader(0);
		g_pDevice->DrawPrimitiveUP( D3DPT_TRIANGLEFAN, 2, Vertex, sizeof(SVtxRhwTex) );
	}

	g_pDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
}


//------------------------------------------------------------------------
// 
// [2011/3/16 jjuiddong]
//------------------------------------------------------------------------
void CShadowSystem::BeginEdge()
{
	g_pDevice->GetRenderTarget(0, &m_pOldBackBuffer);
//	g_pDevice->GetDepthStencilSurface(&m_pOldZBuffer);
	g_pDevice->GetViewport(&m_oldViewport);

	// 깊이버퍼로 타겟을 변경한다.
	g_pDevice->SetRenderTarget(0, m_pEdgeSurf);
//	g_pDevice->SetDepthStencilSurface(m_pZ);
	D3DVIEWPORT9 viewport = {0,0, m_oldViewport.Width, m_oldViewport.Height, 0.0f,1.0f};
	g_pDevice->SetViewport(&viewport);

//	g_pDevice->Clear(0L, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0xffffff, 1.0f, 0L);
	g_pDevice->Clear(0L, NULL, D3DCLEAR_TARGET, 0xffffff, 1.0f, 0L);
	m_pEdgeShader->Begin();
	m_pEdgeShader->BeginPass(0);
	m_pEdgeShader->SetFloat("fNear", 1);
	m_pEdgeShader->SetFloat("fFar", 1000);

	m_DispList.clear();
}


//------------------------------------------------------------------------
// 
// [2011/3/16 jjuiddong]
//------------------------------------------------------------------------
void CShadowSystem::EndEdge()
{
	DispItor itor = m_DispList.begin();
	while (m_DispList.end() != itor)
	{
		EdgeDepthRender( *itor++ );
	}

	g_pDevice->SetRenderTarget(0, m_pOldBackBuffer);
//	g_pDevice->SetDepthStencilSurface(m_pOldZBuffer);
	g_pDevice->SetViewport(&m_oldViewport);
	m_pOldBackBuffer->Release();
//	m_pOldZBuffer->Release();

	m_pEdgeShader->End();

	//EdgeFinalRender();
}


//------------------------------------------------------------------------
// 
// [2011/3/16 jjuiddong]
//------------------------------------------------------------------------
void CShadowSystem::EdgeDepthRender(CDispObject *pDispObj)
{
	m_pEdgeShader->BeginPass(0);

	Matrix44 mWVP;
	g_Camera.GetViewProjMatrix(&mWVP);
	mWVP = *pDispObj->GetWorldTM() * mWVP;
	m_pEdgeShader->SetMatrix("mWLP", mWVP);
	m_pEdgeShader->CommitChanges();

	pDispObj->Render();

	m_pEdgeShader->EndPass();
}


//------------------------------------------------------------------------
// 
// [2011/3/16 jjuiddong]
//------------------------------------------------------------------------
void CShadowSystem::EdgeFinalRender()
{
	typedef struct {
		FLOAT       p[4];
		FLOAT       t[4][2];
	} T4VERTEX;

	m_pEdgeShader->Begin();
	m_pEdgeShader->BeginPass(2);

	g_pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	g_pDevice->SetRenderState(D3DRS_SRCBLEND , D3DBLEND_ZERO);
	g_pDevice->SetRenderState(D3DRS_DESTBLEND , D3DBLEND_SRCALPHA);

	FLOAT w = (FLOAT)m_oldViewport.Width;
	FLOAT h = (FLOAT)m_oldViewport.Height;
	FLOAT du = 1.5f/m_ShadowMapWidth;
	FLOAT dv = 1.5f/m_ShadowMapHeight;

	T4VERTEX Vertex[4] = {
		//   x    y   z    rhw     tu       tv
		{ 0.0f,   0, 0.1f, 1.0f,  0.0f-du, 0.0f-dv // 좌측상단
		, 0.0f+du, 0.0f+dv // 우측하단
		, 0.0f-du, 0.0f+dv // 좌측하단
		, 0.0f+du, 0.0f-dv},//우측상단
		{    w,   0, 0.1f, 1.0f,  1.0f-du, 0.0f-dv
		, 1.0f+du, 0.0f+dv
		, 1.0f-du, 0.0f+dv
		, 1.0f+du, 0.0f-dv, },
		{    w,   h, 0.1f, 1.0f,  1.0f-du, 1.0f-dv
		, 1.0f+du, 1.0f+dv
		, 1.0f-du, 1.0f+dv
		, 1.0f+du, 1.0f-dv, },
		{ 0.0f,   h, 0.1f, 1.0f,  0.0f-du, 1.0f-dv
		, 0.0f+du, 1.0f+dv
		, 0.0f-du, 1.0f+dv
		, 0.0f+du, 1.0f-dv, },
	};

	m_pEdgeShader->SetTexture( "SrcTex",   m_pEdgeTex );
	g_pDevice->SetFVF( D3DFVF_XYZRHW | D3DFVF_TEX4 );
	m_pEdgeShader->CommitChanges();
	g_pDevice->DrawPrimitiveUP( D3DPT_TRIANGLEFAN, 2, Vertex, sizeof(T4VERTEX) );

	m_pEdgeShader->EndPass();
	m_pEdgeShader->End();

	g_pDevice->SetRenderState( D3DRS_ZENABLE, TRUE );
	g_pDevice->SetRenderState( D3DRS_LIGHTING, TRUE );
	g_pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
}


//------------------------------------------------------------------------
// 
// [2011/3/16 jjuiddong]
//------------------------------------------------------------------------
void CShadowSystem::RenderModelEdge(CDispObject *pDispObj)
{
	m_DispList.push_back(pDispObj);
}


//------------------------------------------------------------------------
// 
// [2011/3/15 jjuiddong]
//------------------------------------------------------------------------
void CShadowSystem::Clear()
{
	SAFE_RELEASE(m_pEffect);

	SAFE_RELEASE(m_pDecl);
	SAFE_RELEASE(m_pShadowSurf);
	SAFE_RELEASE(m_pShadowTex);
// 	SAFE_RELEASE(m_pTerrainSurf);
// 	SAFE_RELEASE(m_pTerrainTex);

	SAFE_RELEASE(m_pEdgeSurf);
	SAFE_RELEASE(m_pEdgeTex);

	SAFE_RELEASE(m_pZ);

	SAFE_DELETE(m_pEdgeShader);

}

