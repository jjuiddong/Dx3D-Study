
#include "StdAfx.h"
#include "Terrain.h"
#include "terrainCursor.h"
#include "common/light.h"
#include "common/shader.h"
#include "model/dispObject.h"
#include "water.h"
#include "chunkManager.h"


const int g_alphaTextureSize = 64;

CChunkManager::CChunkManager() : 
m_pChunk(NULL)
, m_pTerrain(NULL)
, m_pLoader(NULL)
, m_pShader(NULL)
, m_pTerrainTex(NULL)
, m_pTerrainSurf(NULL)
, m_pTerrainDepth(NULL)
, m_pTerrainDepthSurf(NULL)
, m_ChunkSize(0)
, m_pWater(NULL)
{
	m_matWorld.SetIdentity();
	m_ObjId = d3d::CreateObjectId();

}


CChunkManager::~CChunkManager()
{
	Clear();

}


//-----------------------------------------------------------------------------//
// 청크 초기화
//-----------------------------------------------------------------------------//
void CChunkManager::Init(CTerrain *pTerrain, int colChunkCount, int rowChunkCount)
{
	Clear();

	if (!pTerrain)
		return;

	// init variable
	m_pTerrain = pTerrain;
	m_ColChunkCount = colChunkCount;
	m_RowChunkCount = rowChunkCount;
	m_ChunkSize = colChunkCount * rowChunkCount;

	m_ColumnCellCountPerChunk = pTerrain->GetColumnCellCount() / colChunkCount;
	m_RowCellCountPerChunk = pTerrain->GetRowCellCount() / rowChunkCount;
	m_ColumnVtxCountPerChunk = m_ColumnCellCountPerChunk + 1;
	m_RowVtxCountPerChunk = m_RowCellCountPerChunk + 1;

	// init texture, surface
	D3DVIEWPORT9 viewPort;
	g_pDevice->GetViewport(&viewPort);
	m_TerrainMapWidth = viewPort.Width;
	m_TerrainMapHeight = viewPort.Height;

	if (FAILED(g_pDevice->CreateTexture(m_TerrainMapWidth, m_TerrainMapHeight, 1, 
		D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8,
		D3DPOOL_DEFAULT, &m_pTerrainTex, NULL)))
		return;

	if (FAILED(m_pTerrainTex->GetSurfaceLevel(0, &m_pTerrainSurf)))
		return;

 	if (FAILED(g_pDevice->CreateTexture(m_TerrainMapWidth, m_TerrainMapHeight, 1, 
 		D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8,
 		D3DPOOL_DEFAULT, &m_pTerrainDepth, NULL)))
 		return;
 
 	if (FAILED(m_pTerrainDepth->GetSurfaceLevel(0, &m_pTerrainDepthSurf)))
 		return;

	if (FAILED(g_pDevice->CreateDepthStencilSurface(
		m_TerrainMapWidth, m_TerrainMapHeight, 
		D3DFMT_D16, D3DMULTISAMPLE_NONE, 0,
		TRUE, &m_pZ, NULL)))
		return;

	// 투영공간에서 텍스처공간으로 변환
	const float fOffsetX = 0.5f + (0.5f / (float)m_TerrainMapWidth);
	const float fOffsetY = 0.5f + (0.5f / (float)m_TerrainMapHeight);
	Matrix44 mScaleBias(	
		0.5f,		0.0f,	0.0f,   0.0f,
		0.0f,		-0.5f,	0.0f,   0.0f,
		0.0f,		0.0f,	0.0f,	0.0f,
		fOffsetX, fOffsetY, 0.0f,   1.0f );

	m_mScaleBias = mScaleBias;

	// init water
	m_pWater = new CWater();
	m_pWater->Init(pTerrain);

	// init shader
	m_pShader = new CShader();
	m_pShader->Init("hlsl_geometry.fx", "TShader");

	// init chunk
	m_pChunk = new CChunk[ colChunkCount * rowChunkCount];

	const int chunk_width = (int)((float)m_ColumnCellCountPerChunk * pTerrain->GetCellSize());
	const int chunk_height = (int)((float)m_RowCellCountPerChunk * pTerrain->GetCellSize());

	for( int cy=0; cy < rowChunkCount; ++cy )
	{
		for( int cx=0; cx < colChunkCount; ++cx )
		{
			const int startx = ((chunk_width * cx) - (colChunkCount*chunk_width)/2);
			const int starty = ((rowChunkCount*chunk_height)/2) - (chunk_height * cy);
			const int endx = startx + chunk_width;
			const int endy = starty - chunk_height;

			const int index = cx + cy * rowChunkCount;
			m_pChunk[ index].Init(cx, cy, startx, starty, chunk_width, chunk_height, 
				pTerrain->GetCellSize(), g_alphaTextureSize );
		}
	}
}


//------------------------------------------------------------------------
// 
// [2011/2/28 jjuiddong]
//------------------------------------------------------------------------
void CChunkManager::Load(CTerrain *pTerrain, SChunkGroupLoader *pLoader)
{
	Init(pTerrain, pLoader->colChunkCount, pLoader->rowChunkCount);

	for (int i=0; i < pLoader->chunkCount; ++i)
	{
		m_pChunk[ i].LoadLayer(&pLoader->pchunk[ i]);
	}
}


//------------------------------------------------------------------------
// 
// [2011/3/14 jjuiddong]
//------------------------------------------------------------------------
void CChunkManager::SetRenderState()
{
	g_pDevice->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
	g_pDevice->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);
	g_pDevice->SetSamplerState(1, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
	g_pDevice->SetSamplerState(1, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);

	g_pDevice->SetRenderState( D3DRS_LIGHTING, TRUE );
	g_pDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
	g_pDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
	g_pDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );

	g_pDevice->SetTextureStageState( 0, D3DTSS_TEXCOORDINDEX, 0 );
	g_pDevice->SetTextureStageState( 1, D3DTSS_TEXCOORDINDEX, 0 );

	g_pDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
	g_pDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
	g_pDevice->SetSamplerState( 1, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
	g_pDevice->SetSamplerState( 1, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );

	g_pDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE);
	g_pDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	g_pDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1 );
	g_pDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );

	g_pDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );
	g_pDevice->SetTextureStageState( 1, D3DTSS_COLORARG1, D3DTA_CURRENT );
	g_pDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1 );
	g_pDevice->SetTextureStageState( 1, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
}

/*
//------------------------------------------------------------------------
// 
// [2011/3/25 jjuiddong]
//------------------------------------------------------------------------
void CChunkManager::RenderFixedPipeline()
{
	Matrix44 mat;
	mat.SetIdentity();
	g_pDevice->SetTransform( D3DTS_WORLD, (D3DXMATRIX*)&mat );

	SetRenderState();

	for (int i=0; i < m_ChunkSize; ++i)
		m_pChunk[ i].Render();

	g_pDevice->SetTextureStageState( 0, D3DTSS_TEXCOORDINDEX, 0 );
	g_pDevice->SetTextureStageState( 1, D3DTSS_TEXCOORDINDEX, 1 );
	g_pDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
	g_pDevice->SetRenderState( D3DRS_LIGHTING, FALSE );
}
/**/

//------------------------------------------------------------------------
// isRenderTerrainBuffer : true 이면 m_pTerrainSurf 버퍼에 지형을 출력한다.
// isRenderDeptBuffer : true 이면 깊이버퍼에 깊이값을 저장한다.
// [2011/2/28 jjuiddong]
//------------------------------------------------------------------------
void CChunkManager::RenderChunks(BOOL isRenderTerrainBuffer, BOOL isRenderDeptBuffer)
{
	LPDIRECT3DSURFACE9 pOldBackBuffer = NULL;
	LPDIRECT3DSURFACE9 pOldZBuffer = NULL;
	if (isRenderTerrainBuffer)
	{
		g_pDevice->GetRenderTarget(0, &pOldBackBuffer);
		//g_pDevice->GetDepthStencilSurface(&pOldZBuffer);
		g_pDevice->SetRenderTarget(0, m_pTerrainSurf);
		//g_pDevice->SetDepthStencilSurface(m_pZ);
		g_pDevice->Clear(0L, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0xffffffff, 1.0f, 0L);
	}

	m_pShader->Begin();

	Matrix44 mWVP;
	g_Camera.GetViewProjMatrix(&mWVP);
	m_pShader->SetMatrix("mWVP", mWVP);

	Matrix44 mWVPB = mWVP * m_mScaleBias;
	m_pShader->SetMatrix("mWVPB", mWVPB);

	// 광원방향
	CLight *pLight = m_pTerrain->GetLight();
	Vector3 lightPos = pLight->GetDirection();
	D3DXVECTOR4 v(lightPos.x, lightPos.y, lightPos.z, -0.3f); // -0.3f 환경광의 강도
	m_pShader->SetVector( "vLightDir", v);

	// 시점
	Vector3 eyePos = g_Camera.GetPosition();
	m_pShader->SetVector( "vEyePos", eyePos);

	for (int i=0; i < m_ChunkSize; ++i)
		m_pChunk[ i].Render(m_pShader, 0);

// 	g_pDevice->SetRenderTarget(0, m_pTerrainDepthSurf);
// 	g_pDevice->Clear(0L, NULL, D3DCLEAR_TARGET, 0x00, 1.0f, 0L);

	m_pShader->End();

	m_pWater->Render();

	if (isRenderDeptBuffer)
	{
// 		g_pDevice->SetRenderTarget(0, m_pTerrainDepthSurf);
// 		g_pDevice->Clear(0L, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0x00, 1.0f, 0L);
// 
// 		for (int i=0; i < m_ChunkSize; ++i)
// 			m_pChunk[ i].Render(m_pShader, 1);
	}

	if (isRenderTerrainBuffer)
	{
		g_pDevice->SetRenderTarget(0, pOldBackBuffer);
		//g_pDevice->SetDepthStencilSurface(pOldZBuffer);
		pOldBackBuffer->Release();
		//pOldZBuffer->Release();
	}
}


//------------------------------------------------------------------------
// 
// [2011/3/28 jjuiddong]
//------------------------------------------------------------------------
void CChunkManager::RenderDebugTexture()
{
/*
	g_pDevice->SetRenderState( D3DRS_LIGHTING, FALSE);

	g_pDevice->SetTextureStageState(0,D3DTSS_COLOROP,	D3DTOP_SELECTARG1);
	g_pDevice->SetTextureStageState(0,D3DTSS_COLORARG1,	D3DTA_TEXTURE);
	g_pDevice->SetTextureStageState(1,D3DTSS_COLOROP,   D3DTOP_DISABLE);
	for (int i=0; i < 1; ++i)
	{
		float y = 128;
		float scale = 128;
		SVtxRhwTex Vertex[4] = {
			// x  y  z rhw tu tv
			{Vector3(0,			y*(i+3),		0), 1, 0, 0,},
			{Vector3(scale,		y*(i+3),		0), 1, 1, 0,},
			{Vector3(scale,	y*(i+3)+scale,	0), 1, 1, 1,},
			{Vector3(0,		y*(i+3)+scale,	0), 1, 0, 1,},
		};

		LPDIRECT3DTEXTURE9 ptex;
		if (i==0) ptex = m_pTerrainDepth;
		g_pDevice->SetTexture( 0, ptex);
		g_pDevice->SetVertexShader(NULL);
		g_pDevice->SetFVF( D3DFVF_XYZRHW | D3DFVF_TEX1 );
		g_pDevice->SetPixelShader(0);
		g_pDevice->DrawPrimitiveUP( D3DPT_TRIANGLEFAN, 2, Vertex, sizeof(SVtxRhwTex) );
	}

	g_pDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
/**/
}



//------------------------------------------------------------------------
// 
// [2011/3/28 jjuiddong]
//------------------------------------------------------------------------
void CChunkManager::Render()
{
	RenderChunks(TRUE, TRUE);
}


//------------------------------------------------------------------------
// 타겟버퍼에 바로 출력한다.
// [2011/3/28 jjuiddong]
//------------------------------------------------------------------------
void CChunkManager::RenderDirect()
{
	RenderChunks(FALSE, TRUE);
}


//------------------------------------------------------------------------
// [2011/3/15 jjuiddong]
//------------------------------------------------------------------------
void CChunkManager::RenderDepth()
{
	Matrix44 mat;
	mat.SetIdentity();
	g_pDevice->SetTransform( D3DTS_WORLD, (D3DXMATRIX*)&mat );

	for (int i=0; i < m_ChunkSize; ++i)
		m_pChunk[ i].RenderDepth();

	m_pWater->Render();
}

//------------------------------------------------------------------------
// 
// [2011/2/28 jjuiddong]
//------------------------------------------------------------------------
void CChunkManager::RenderEdge()
{
	for (int i=0; i < m_ChunkSize; ++i)
		m_pChunk[ i].RenderEdge();

	for (int i=0; i < m_ChunkSize; ++i)
	{
		if (m_pChunk[ i].IsFocus())
			m_pChunk[ i].RenderEdge();
	}
}


//------------------------------------------------------------------------
// 
// [2011/2/28 jjuiddong]
//------------------------------------------------------------------------
BOOL CChunkManager::Update( int elapseTime )
{
	if (m_pWater)
		m_pWater->Update(elapseTime);

	return TRUE;
}


//------------------------------------------------------------------------
// 
// [2011/2/28 jjuiddong]
//------------------------------------------------------------------------
void CChunkManager::UpdateEdge()
{
	for (int i=0; i < m_ChunkSize; ++i)
		m_pChunk[ i].UpdateEdge();
}


//------------------------------------------------------------------------
// 
// [2011/2/28 jjuiddong]
//------------------------------------------------------------------------
void CChunkManager::SetHeight( CTerrainCursor *pcursor, int nElapsTick )
{
	for (int i=0; i < m_ChunkSize; ++i)
		m_pChunk[ i].ModifyHeight(pcursor, nElapsTick);
}


//------------------------------------------------------------------------
// 
// [2011/2/28 jjuiddong]
//------------------------------------------------------------------------
void CChunkManager::Clear()
{
	SAFE_DELETE(m_pWater);
	SAFE_DELETE(m_pShader);
	SAFE_ADELETE(m_pChunk);
	SAFE_RELEASE(m_pTerrainTex);
	SAFE_RELEASE(m_pTerrainSurf);
	SAFE_RELEASE(m_pTerrainDepth);
	SAFE_RELEASE(m_pTerrainDepthSurf);

	DeleteChunkGroupLoader(m_pLoader);

}


//------------------------------------------------------------------------
// 
// [2011/2/28 jjuiddong]
//------------------------------------------------------------------------
CChunk* CChunkManager::GetChunkFromPos( Vector3 pickPos, float *pu, float *pv ) // pu = NULL, pv = NULL
{
	// 어느 청크에 클릭되었는지 찾는다. x,z값으로 찾을수있음
	const float chunk_width = m_pChunk[0].GetRegionWidth();
	const float chunk_height = m_pChunk[0].GetRegionHeight();
	const float width = m_pTerrain->GetRegionWidth();
	const float height = m_pTerrain->GetRegionHeight();

	int cx_idx = (int)((pickPos.x + (width / 2.f)) / chunk_width);
	int cy_idx = (int)((pickPos.z + (height / 2.f)) / chunk_height);

	//-----------------------------------------------------------------
	// 맵의 오른쪽 가장 끝부분과 가장 아랫부분을 클릭되었을때 무시되는 
	// 문제를 해결함
	if( m_ColChunkCount == cx_idx )
	{
		if( 0.1f > fabs((pickPos.x + (width / 2.f)) - width) )
			cx_idx -= 1;
	}
	if( m_RowChunkCount == cy_idx )
	{
		if( 0.1f > fabs((pickPos.z + (height / 2.f)) - height) )
			cy_idx -= 1;
	}
	//-----------------------------------------------------------------

	if( (0 > cx_idx) || (m_ColChunkCount < cx_idx) ) return NULL;
	if( (0 > cy_idx) || (m_RowChunkCount < cy_idx) ) return NULL;

	// 오른쪽 위가 0,0을 나타내기 때문에 인덱스계산을 한번더 해주어야 한다.
	cy_idx = m_RowChunkCount - cy_idx - 1;

	// 텍스쳐 uv좌표를 얻는다.
	const float chunk_x = (cx_idx * chunk_width);
	const float chunk_y = ((m_RowChunkCount - cy_idx - 1) * chunk_height);
	const float pick_u = (pickPos.x + (width / 2.f)) - chunk_x;
	const float pick_v = (pickPos.z + (height / 2.f)) - chunk_y;
	if( pu ) 
		*pu = pick_u / chunk_width;
	if( pv )
		*pv = 1.f - (pick_v / chunk_height);

	const int index = cx_idx + (cy_idx * m_RowChunkCount);
	return &m_pChunk[ index];
}


//------------------------------------------------------------------------
// 
// [2011/2/28 jjuiddong]
//------------------------------------------------------------------------
CChunk* CChunkManager::GetChunkFromScreenPos( Vector2 screenPos, float *pu, float *pv ) // pu,pv=NULL
{
	if (!m_pTerrain) return FALSE;

	Vector3 pick;
	if (!m_pTerrain->Pick(&screenPos, &pick))
		return NULL;

	return GetChunkFromPos( pick, pu, pv );
}


//------------------------------------------------------------------------
// 
// [2011/2/28 jjuiddong]
//------------------------------------------------------------------------
void CChunkManager::UpdateChunkToTerrain()
{
	for (int i=0; i < m_ChunkSize; ++i)
		m_pChunk[ i].UpdateChunkToMainTerrain(m_pTerrain);
}


//------------------------------------------------------------------------
// 
// [2011/2/28 jjuiddong]
//------------------------------------------------------------------------
void CChunkManager::UpdateTerrainToChunk()
{
	for (int i=0; i < m_ChunkSize; ++i)
		m_pChunk[ i].UpdateMainTerrainToChunk(m_pTerrain);
}


//------------------------------------------------------------------------
// 
// [2011/2/28 jjuiddong]
//------------------------------------------------------------------------
CChunk* CChunkManager::SetFocus( CTerrainCursor *pcursor )
{
	CChunk *pchunk = GetChunkFromPos( pcursor->GetCursorPos() );
	for (int i=0; i < m_ChunkSize; ++i)
		m_pChunk[ i].SetFocus(FALSE);

	if (!pchunk)
	{
		return NULL;
	}

	pchunk->SetFocus(TRUE);
	return pchunk;
}


//-----------------------------------------------------------------------------//
// 청크 축을 기준으로 청크정보를 리턴한다.
// x,y: m_Chunk[ x][ y]
//-----------------------------------------------------------------------------//
CChunk* CChunkManager::GetChunkFrom( int x, int y )
{
	if (!m_pChunk) return NULL;
	if ((0 > x) || (x >= m_ColChunkCount)) return NULL;
	if ((0 > y) || (y >= m_RowChunkCount)) return NULL;

	const int index = x + y * m_RowChunkCount;
	return &m_pChunk[ index];
}


//------------------------------------------------------------------------
// 
// [2011/2/28 jjuiddong]
//------------------------------------------------------------------------
BOOL CChunkManager::DrawBrush( CTerrainCursor *pcursor )
{
	// focus flag 초기화
	int i=0;
	for (int i=0; i < m_ChunkSize; ++i)
		m_pChunk[ i].SetFocus(FALSE);

	for (int i=0; i < m_ChunkSize; ++i)
		m_pChunk[ i].DrawBrush(pcursor);

	return TRUE;
}


//------------------------------------------------------------------------
// 
// [2011/2/28 jjuiddong]
//------------------------------------------------------------------------
SChunkGroupLoader* CChunkManager::GetChunkGroupLoader( char *szAlphaTextureWriteLocatePath )
{
	if (!m_pLoader)
	{
		m_pLoader = NewChunkGroupLoader();
	}

	m_pLoader->chunkCount = m_ChunkSize;
	m_pLoader->rowChunkCount = m_RowChunkCount;
	m_pLoader->colChunkCount = m_ColChunkCount;
	m_pLoader->colCellCountPerChunk = m_ColumnCellCountPerChunk;
	m_pLoader->rowCellCountPerChunk = m_RowCellCountPerChunk;
	m_pLoader->alphaTextureSize = g_alphaTextureSize;

	for (int i=0; i < m_ChunkSize; ++i)
	{
		m_pChunk[ i].GetChunkLoader(szAlphaTextureWriteLocatePath, &m_pLoader->pchunk[ i]);
	}

	return m_pLoader;
}


//------------------------------------------------------------------------
// 
// [2011/2/28 jjuiddong]
//------------------------------------------------------------------------
SChunkGroupLoader* CChunkManager::NewChunkGroupLoader()
{
	SChunkGroupLoader *pLoader = new SChunkGroupLoader();
	pLoader->pchunk = new SChunkLoader[ m_ChunkSize];
	for (int i=0; i < m_ChunkSize; ++i)
	{
		pLoader->pchunk[ i].pLayer = new SLayerLoader[ CChunk::MAX_LAYER_CNT];
	}
	return pLoader;
}


//------------------------------------------------------------------------
// 
// [2011/2/28 jjuiddong]
//------------------------------------------------------------------------
void CChunkManager::DeleteChunkGroupLoader( SChunkGroupLoader *pLoader)
{
	if (!pLoader) return;

	for (int i=0; i < m_ChunkSize; ++i)
	{
		SAFE_ADELETE( pLoader->pchunk[ i].pLayer);
	}
	SAFE_ADELETE(pLoader->pchunk);
	SAFE_DELETE(pLoader);
}

