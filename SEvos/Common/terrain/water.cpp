
#include "StdAfx.h"
#include "model/dispObject.h"
#include "Terrain.h"
#include "common/shader.h"
#include "water.h"



CWater::CWater() :
m_pTex(NULL) 
, m_pVtxBuff(NULL)
, m_pIdxBuff(NULL)
, m_pWaterBump(NULL)
, m_pReflectionMap(NULL)
, m_pShader(NULL)
{
	m_ObjId = d3d::CreateObjectId();
	m_matWorld.SetIdentity();
	m_IncTime = 0;

}


CWater::~CWater()
{
	Clear();

}


//------------------------------------------------------------------------
// 
// [2011/3/22 jjuiddong]
//------------------------------------------------------------------------
void CWater::Init(CTerrain *pTerrain)
{
	Clear();

	m_pShader = new CShader();
	m_pShader->Init("hlsl_water.fx", "water");

// 	D3DVERTEXELEMENT9 decl[] =
// 	{
// 		{0,  0, D3DDECLTYPE_FLOAT3,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
// 		{0, 12, D3DDECLTYPE_FLOAT3,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL,	0},
// 		D3DDECL_END()
// 	};
// 
// 	if (FAILED(hr = g_pDevice->CreateVertexDeclaration(decl, &m_pDecl)))
// 	{
// 		DXTRACE_ERR ("CreateVertexDeclaration", hr);
// 		return;
// 	}

	// 버텍스 버퍼, 인덱스 버퍼 생성
	const int vtxsize = 4;
	g_pDevice->CreateVertexBuffer( vtxsize*sizeof(SVtxNormTex), 0, SVtxNormTex::FVF, 
		D3DPOOL_MANAGED, &m_pVtxBuff, NULL );

	const float depth = -10.f;
	const float width = pTerrain->GetRegionWidth();
	const float height = pTerrain->GetRegionHeight();

	SVtxNormTex *pV;
	m_pVtxBuff->Lock(0, 0, (void**)&pV, 0);
	pV[ 0] = SVtxNormTex( Vector3(-width/2.f, depth, height/2.f), Vector3(0, 1, 0), 0, 0);
	pV[ 1] = SVtxNormTex( Vector3( width/2.f, depth, height/2.f), Vector3(0, 1, 0), 1, 0);
	pV[ 2] = SVtxNormTex( Vector3( width/2.f, depth, -height/2.f), Vector3(0, 1, 0), 1, 1);
	pV[ 3] = SVtxNormTex( Vector3(-width/2.f, depth, -height/2.f), Vector3(0, 1, 0), 0, 1);
	m_pVtxBuff->Unlock();

	const int trisize = 2;
	g_pDevice->CreateIndexBuffer( trisize*3*sizeof(WORD), D3DUSAGE_WRITEONLY, 
		D3DFMT_INDEX16, D3DPOOL_MANAGED, &m_pIdxBuff, NULL );

	WORD *pi = NULL;
	m_pIdxBuff->Lock( 0, 0, (void**)&pi, 0 );
	pi[ 0] = 0; pi[ 1] = 1; pi[ 2] = 3;
	pi[ 3] = 1; pi[ 4] = 2; pi[ 5] = 3;
	m_pIdxBuff->Unlock();

	//D3DXCreateTextureFromFile( g_pDevice, "waves2.dds", &m_pWaterBump );
	D3DXCreateTextureFromFile( g_pDevice, "data/map/texture/gradient_map.jpg", &m_pWaterBump );
	D3DXCreateCubeTextureFromFile( g_pDevice, "data/map/texture/cubemap.dds", &m_pReflectionMap );

}


//------------------------------------------------------------------------
// 
// [2011/3/22 jjuiddong]
//------------------------------------------------------------------------
void CWater::Render()
{
	static const int triangleSize = 2;
	static const int vertexSize = 4;

//	g_pDevice->SetVertexDeclaration( m_pDecl );
	m_pShader->Begin();
	m_pShader->BeginPass(0);

	Matrix44 mWVP;
	g_Camera.GetViewProjMatrix(&mWVP);
	m_pShader->SetMatrix("WorldViewProj", mWVP);

	//set the texturs
	m_pShader->SetTexture("texture0", m_pWaterBump);
	m_pShader->SetTexture("texture1", m_pReflectionMap);

	m_pShader->SetVector("eyePosition", g_Camera.GetEye());

	g_pDevice->SetStreamSource( 0, m_pVtxBuff, 0, sizeof(SVtxNormTex) );
	g_pDevice->SetFVF( SVtxNormTex::FVF );
	g_pDevice->SetIndices( m_pIdxBuff );
	m_pShader->CommitChanges();
	g_pDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0, 0, vertexSize, 0, triangleSize );

	m_pShader->EndPass();
	m_pShader->End();
}


//------------------------------------------------------------------------
// 
// [2011/3/22 jjuiddong]
//------------------------------------------------------------------------
void CWater::RenderDepth()
{

}


//------------------------------------------------------------------------
// 
// [2011/3/22 jjuiddong]
//------------------------------------------------------------------------
BOOL CWater::Update( int elapseTime )
{
	m_IncTime += (float)elapseTime / 1000.f;
	m_pShader->SetFloat("time", m_IncTime);
	return TRUE;
}


//------------------------------------------------------------------------
// 
// [2011/3/22 jjuiddong]
//------------------------------------------------------------------------
Matrix44* CWater::GetWorldTM()
{
	return &m_matWorld;
}


//------------------------------------------------------------------------
// 
// [2011/3/22 jjuiddong]
//------------------------------------------------------------------------
void CWater::Clear()
{
	m_IncTime = 0;

	SAFE_RELEASE(m_pTex);
	SAFE_RELEASE(m_pVtxBuff);
	SAFE_RELEASE(m_pIdxBuff);

	SAFE_DELETE(m_pShader);

	SAFE_RELEASE(m_pWaterBump);
	SAFE_RELEASE(m_pReflectionMap);

}

