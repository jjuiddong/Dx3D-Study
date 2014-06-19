
#include "stdafx.h"
#include <fstream>
#include <cmath>
#include <algorithm>
#include "../model//model.h"
#include "quadtree.h"
#include "chunkManager.h"
#include "../common/utility.h"
#include "common/skyBox.h"
#include "common/light.h"
#include "shadowsystem.h"
#include "water.h"
#include "terrain.h"

using namespace std;

int CTerrain::m_nTModelId = 0;
CTerrain::CTerrain() : 
m_pChunkMng(NULL),
m_pSkyBox(NULL),
m_pLight(NULL),
m_pTexture(NULL),
m_pDynamicIdxBuff(NULL),
m_bFog(FALSE),
m_nRenderType( D3DPT_TRIANGLELIST ),
m_pQuadTree(NULL),
m_bLoaded(FALSE),
m_pShadowSystem(NULL),
m_IsDisplayShadow(TRUE),
m_IsDisplayNormal(FALSE),
m_IsDisplayQuadTree(FALSE)
{

}


CTerrain::~CTerrain()
{
	Clear();

}


//------------------------------------------------------------------------
// 
// [2011/2/20 jjuiddong]
//------------------------------------------------------------------------
BOOL CTerrain::Create( int nVtxRow, int nVtxCol, float cellsize, float fHeightScale, char *textureFileName )
{
	Clear();

	m_bLoaded = TRUE;
	const int cellPerRow = nVtxRow - 1;
	const int cellPerCol = nVtxCol - 1;
	const int width = (int)((float)cellPerRow * cellsize);
	const int height = (int)((float)cellPerCol * cellsize);
	const int startx = -width / 2;
	const int starty = height / 2;
	CRegion::Init(startx, starty, width, height, cellsize);

	m_fHeightScale = fHeightScale;

	if (textureFileName)
		m_pTexture = CFileLoader::LoadTexture(textureFileName);

	m_pChunkMng = new CChunkManager();
	m_pChunkMng->Init(this, CHUNK_COL, CHUNK_ROW);

	// Init QuadTree
	SAFE_DELETE( m_pQuadTree );
	m_pQuadTree = new CQuadTree( nVtxCol, nVtxRow );

	SVtxNormTex *pv = NULL;
	m_pvtxBuff->Lock( 0, 0, (void**)&pv, 0 );
	m_pQuadTree->Build( pv );
	m_pvtxBuff->Unlock();

	// init skybox
	m_pSkyBox = new CSkyBox();
	m_pSkyBox->Init((char*)g_szSkyBoxPath);

	// init light
	m_pLight = new CLight();
	m_pLight->SetDirectionalLight(d3d::WHITE);
	m_pLight->SetPosition(Vector3(500, 500, 0));
	m_pLight->SetDirection(Vector3(0, -1.f, 0));

	// init shadow
	m_pShadowSystem = new CShadowSystem();
	m_pShadowSystem->Init();

	return TRUE;
}


//-----------------------------------------------------------------------------//
// HeightFile Load (Raw format)
// szHeightFileName : Raw FileName
//-----------------------------------------------------------------------------//
BOOL CTerrain::Load( char *szHeightFileName, int nVtxRow, int nVtxCol, float cellsize, float fHeightScale )
{
	return Create(nVtxRow, nVtxCol, cellsize, fHeightScale, "map//tile//detail.tga" );
}


//-----------------------------------------------------------------------------//
// 
//-----------------------------------------------------------------------------//
BOOL CTerrain::Load( char *szFileName )
{
/*
	CLinearMemLoader loader;
	loader.LoadTokenFile( "data//script//token_map.txt" );

	FILE *fp = fopen( szFileName, "rb" );
	// header
	if( 'm' == fgetc(fp) && 'a' == fgetc(fp) && 'p' == fgetc(fp) )
		m_pTLoader = (STerrainLoader*)loader.Read( fp, "TERRAIN" );
	fclose( fp );

	if( !m_pTLoader ) return FALSE;

	m_fHeightScale = 1.0f;
	m_nVtxCount = m_pTLoader->nVtxPerRow * m_pTLoader->nVtxPerCol;
	m_nTriCount = m_pTLoader->nCellPerRow * m_pTLoader->nCellPerCol * 2;

	if( !CreateVertex() )
		return FALSE;

	if( !CreateIndex() )
		return FALSE;
/**/
	return TRUE;
}


//-----------------------------------------------------------------------------//
// Fog 설정
//-----------------------------------------------------------------------------//
void CTerrain::SetFog( DWORD dwColor, float fMin, float fMax )
{
//	m_bFog = TRUE;
//	GetD3D()->SetFog( dwColor, fMin, fMax );
}


//-----------------------------------------------------------------------------//
// map에 적용될 txture 파일을 로드한다.
//-----------------------------------------------------------------------------//
BOOL CTerrain::LoadTexture( char *szFileName )
{
/*
	m_pTexture = new Texture::CTexture;
	if( !Texture::ReadInFile("map", szFileName, Texture::NORMAL, m_pTexture) )
		return FALSE;
/**/
	return TRUE;
}


//-----------------------------------------------------------------------------//
// Raw 파일읽기
//-----------------------------------------------------------------------------//
BOOL CTerrain::ReadRawFile( char *szFileName )
{
/*
	vector<BYTE> in( m_nVtxCount );
	ifstream inFile( szFileName, ios_base::binary );
	if( inFile == 0 ) return FALSE;
	inFile.read( (char*)&in[0], in.size() );
	inFile.close();

//	m_vecHeight.resize( m_nVtxCount );
//	for( int i = 0; i < in.size(); i++ )
//		m_vecHeight[ i] = (float)in[ i];
/**/

	return TRUE;
}


//------------------------------------------------------------------------
// 
// [2011/2/15 jjuiddong]
//------------------------------------------------------------------------
void CTerrain::CreateIndexBuffer()
{
	CRegion::CreateIndexBuffer();
	g_pDevice->CreateIndexBuffer( GetTriangleCount()*sizeof(WORD), D3DUSAGE_WRITEONLY, 
		D3DFMT_INDEX16, D3DPOOL_MANAGED, &m_pDynamicIdxBuff, NULL );
}


//-----------------------------------------------------------------------------//
// 출력
//-----------------------------------------------------------------------------//
void CTerrain::Render()
{
	if( !m_bLoaded ) return;

	m_pLight->LightOn();

	if (m_pSkyBox)
		m_pSkyBox->Render();

	Matrix44 mat;
	mat.SetIdentity();
	g_pDevice->SetTransform( D3DTS_WORLD, (D3DMATRIX*)&mat );

	SetRenderState();
	if (m_IsDisplayShadow)
	{
		RenderShadow();
	}
	else
	{
		RenderNotShadow();
	}

	m_pLight->LightOff();
}


//------------------------------------------------------------------------
// 지형과 모델을 출력한다. 그림자도 출력된다.
// [2011/3/15 jjuiddong]
//------------------------------------------------------------------------
void CTerrain::RenderShadow()
{
	m_pShadowSystem->BeginDepth(m_pLight);
		RenderStructureToDepthBuffer();
	m_pShadowSystem->EndDepth();

//  	m_pShadowSystem->BeginTexture();
//  		m_pChunkMng->Render();
//  	m_pShadowSystem->EndTexture();

	m_pChunkMng->Render();
	m_pShadowSystem->SetTerrainTexture(m_pChunkMng->GetTerrainTexture());

	m_pShadowSystem->BeginRenderShadow();
		m_pShadowSystem->RenderScreen(m_pChunkMng);
	m_pShadowSystem->EndRenderShadow();

	// RenderStructure()를 호출하기 전에 edge를 그려야 한다.
	m_pShadowSystem->BeginEdge();
		RenderStructureEdge();
	m_pShadowSystem->EndEdge();

	RenderStructure();

	if (m_IsDisplayNormal)
		RenderNormal();

	m_pShadowSystem->RenderDebugTexture();
	m_pChunkMng->RenderDebugTexture();
}


//------------------------------------------------------------------------
// 모델의 엣지를 출력한다.
// [2011/3/16 jjuiddong]
//------------------------------------------------------------------------
void CTerrain::RenderPostEffect()
{
	if (!m_bLoaded) return;

	m_pShadowSystem->BeginEdge();
		RenderStructureEdge();
	m_pShadowSystem->EndEdge();
}


//------------------------------------------------------------------------
// 지형과 모델을 출력한다. 그림자는 제외된다.
// [2011/3/15 jjuiddong]
//------------------------------------------------------------------------
void CTerrain::RenderNotShadow()
{
	if (m_IsDisplayQuadTree)
	{
		RenderQuadTree();
	}
	else
	{
		m_pChunkMng->RenderDirect();
	}

	if (m_IsDisplayNormal)
		RenderNormal();
	
	RenderStructure();
}


//------------------------------------------------------------------------
// 
// [2011/3/15 jjuiddong]
//------------------------------------------------------------------------
void CTerrain::RenderQuadTree()
{
	IDirect3DVertexBuffer9 *pvtxBuff = GetVertexBuffer();
	IDirect3DIndexBuffer9 *pidxBuff = GetIndexBuffer();
	g_pDevice->SetMaterial( &m_mtrl );
	g_pDevice->SetTexture( 0, m_pTexture );
	g_pDevice->SetFVF( SVtxNormTex::FVF );
	g_pDevice->SetStreamSource( 0, pvtxBuff, 0, sizeof(SVtxNormTex) );

/*
	SVtxNormTex *pv = NULL;
	pvtxBuff->Lock( 0, 0, (void**)&pv, 0 );
	WORD *pi = NULL;
	m_pDynamicIdxBuff->Lock( 0, 0, (void**)&pi, 0 );
	int cnt = 0;
	cnt = m_pQuadTree->GenerateIndex( pi, pv, &g_Frustum, 0.01f );
	m_pDynamicIdxBuff->Unlock();
	pvtxBuff->Unlock();

	g_pDevice->SetIndices( m_pDynamicIdxBuff );
	g_pDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0, 0, GetVertexCount(), 0, cnt );
/**/
	g_pDevice->SetIndices( pidxBuff );
	g_pDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0, 0, GetVertexCount(), 0, GetTriangleCount() );
}


//-----------------------------------------------------------------------------//
// 에니메이션
//-----------------------------------------------------------------------------//
void CTerrain::Update( int nElapsTick )
{
	if (m_pSkyBox)
		m_pSkyBox->Update(nElapsTick);

	if (m_pChunkMng)
		m_pChunkMng->Update(nElapsTick);

	ModelItor itor = m_DynList.begin();
	while( m_DynList.end() != itor )
		(itor++)->pModel->Update( nElapsTick );
}


//-----------------------------------------------------------------------------//
// row, col위치에서 법선벡터를 구한후 pvLightDir와의 내적값을 리턴한다.
// returnvlaue = 0~1
//-----------------------------------------------------------------------------//
float CTerrain::CalcShade( int nCellRow, int nCellCol, Vector3 *pvLightDir )
{
	Vector3 n = GetNormalVector(nCellRow, nCellCol);
	pvLightDir->Normalize();

	float fCosine = n.DotProduct( *pvLightDir );
	if( fCosine < .0f )
		fCosine = .0f;

	return fCosine;
}


//-----------------------------------------------------------------------------//
// renderstate
//-----------------------------------------------------------------------------//
void CTerrain::SetRenderState()
{
	g_pDevice->SetRenderState( D3DRS_FILLMODE, (D3DPT_LINELIST==m_nRenderType)? D3DFILL_WIREFRAME : D3DFILL_SOLID );
	g_pDevice->SetRenderState( D3DRS_LIGHTING, TRUE );
	g_pDevice->SetRenderState( D3DRS_FOGENABLE, m_bFog );
	g_pDevice->SetRenderState( D3DRS_SPECULARENABLE, FALSE);
	g_pDevice->SetRenderState( D3DRS_NORMALIZENORMALS, TRUE );
    g_pDevice->SetRenderState( D3DRS_AMBIENT, 0x00000000 );

	g_pDevice->SetRenderState( D3DRS_ZENABLE, D3DZB_TRUE);
	g_pDevice->SetRenderState( D3DRS_ZWRITEENABLE, TRUE);

	g_pDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
	g_pDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );

	//g_pDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_CCW );
    //g_pDevice->SetRenderState( D3DRS_AMBIENT, 0x00202020 );

}


//-----------------------------------------------------------------------------//
// pAdd 모델이 복사되어 추가된다.
//-----------------------------------------------------------------------------//
BOOL CTerrain::AddTerrainStructure( Vector3 vPos, OBJ_TYPE eOType, char *szObjName, CModel *pAdd )
{
	STerrainStructure tmodel;
	tmodel.vPos = Vector2(vPos.x, vPos.y);

//	CModel *pModel = pAdd->Clone();
//	if (!pModel)
//		return FALSE;

	tmodel.nId = ++m_nTModelId;
	strcpy_s( tmodel.szObjName, sizeof(tmodel.szObjName), szObjName );

//	Matrix44 *pMat = pModel->GetWorldTM();
//	pMat->SetWorld(vPos);
//	pModel->SetWorldTM(pMat);

	pAdd->SetPos(vPos);
	tmodel.pModel = pAdd;
	tmodel.pModel->SetIsRenderCollisionBox(FALSE);

	if (OT_MODEL == eOType)
		m_DynList.push_back( tmodel );
	else if (OT_RIGID == eOType)
		m_RigidList.push_back( tmodel );

	return TRUE;
}


//-----------------------------------------------------------------------------//
// 
//-----------------------------------------------------------------------------//
BOOL CTerrain::DelTerrainStructure( OBJ_TYPE eOType, int nId )
{
/*
	if( OT_MODEL == eOType )
	{
		ModelItor itor = m_DynList.begin();
		while( m_DynList.end() != itor )
		{
			if( nId == (itor)->nId )
			{
				delete itor->pModel;
				m_DynList.erase( itor );
				break;				
			}
			++itor;
		}
	}

	if( OT_RIGID == eOType )
	{
		ModelItor itor = m_RigidList.begin();
		while( m_RigidList.end() != itor )
		{
			if( nId == (itor)->nId )
			{
				delete itor->pModel;
				m_RigidList.erase( itor );
				break;				
			}
			++itor;
		}
	}
/**/
	return TRUE;
}


//------------------------------------------------------------------------
// 
// [2011/2/20 jjuiddong]
//------------------------------------------------------------------------
void CTerrain::Clear()
{
	CRegion::Clear();

	SAFE_RELEASE( m_pDynamicIdxBuff );

	ModelItor itor = m_DynList.begin();
	while( m_DynList.end() != itor )
		delete (itor++)->pModel;
	m_DynList.clear();

	itor = m_RigidList.begin();
	while( m_RigidList.end() != itor )
		delete (itor++)->pModel;
	m_RigidList.clear();

	//	SAFE_ADELETE( m_pTLoader );
	SAFE_DELETE( m_pQuadTree );

	SAFE_DELETE(m_pChunkMng);

	SAFE_DELETE(m_pSkyBox);
	SAFE_DELETE(m_pLight);

	SAFE_DELETE(m_pShadowSystem);

}


//------------------------------------------------------------------------
// 
// [2011/2/21 jjuiddong]
//------------------------------------------------------------------------
void CTerrain::UpdateEdge()
{
	CRegion::UpdateEdge();
	if (m_pChunkMng)
		m_pChunkMng->UpdateEdge();
}


//------------------------------------------------------------------------
// 
// [2011/2/24 jjuiddong]
//------------------------------------------------------------------------
void CTerrain::RenderStructure()
{
	ModelItor itor = m_DynList.begin();
	while( m_DynList.end() != itor )
		(itor++)->pModel->Render();
	itor = m_RigidList.begin();
	while( m_RigidList.end() != itor )
		(itor++)->pModel->Render();
}


void CTerrain::RenderStructureToDepthBuffer()
{
	ModelItor itor = m_DynList.begin();
	while( m_DynList.end() != itor )
		m_pShadowSystem->RenderDepthBuffer( (itor++)->pModel );
	itor = m_RigidList.begin();
	while( m_RigidList.end() != itor )
		m_pShadowSystem->RenderDepthBuffer( (itor++)->pModel );
}


void CTerrain::RenderStructureEdge()
{
	ModelItor itor = m_DynList.begin();
	while( m_DynList.end() != itor )
		m_pShadowSystem->RenderModelEdge( (itor++)->pModel );
	itor = m_RigidList.begin();
	while( m_RigidList.end() != itor )
		m_pShadowSystem->RenderModelEdge( (itor++)->pModel );
}
