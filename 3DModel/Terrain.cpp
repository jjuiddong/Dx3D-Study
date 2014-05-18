// Terrain.cpp: implementation of the CTerrain class.
//
//////////////////////////////////////////////////////////////////////

#include "Global.h"
#include "Terrain.h"
#include "LinearMemLoader.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CLinearMemLoader m_Loader;

CTerrain::CTerrain()
{
	m_pTile = NULL;	
	m_pVB = NULL;
	m_pBlockVB = NULL;
	m_pAlphaTex = NULL;
	m_pOverTex = NULL;

}

CTerrain::~CTerrain()
{
	Clear();
}

BOOL CTerrain::LoadTerrain( char *szFileName )
{
	m_Loader.LoadTokenFile( "script//def.txt" );	
	m_pTile = (STileForm*)m_Loader.ReadScript( szFileName, "TILEFORM" );
	return TRUE;
}

BOOL CTerrain::CreateTile()
{
	// 베이스 텍스처 생성
	D3DXCreateTextureFromFile( g_pDevice, m_pTile->strTextureName, &m_pBaseTex );
	
	m_pVB = new LPDIRECT3DVERTEXBUFFER9[m_pTile->iTileCount];
	m_pBlockVB = new LPDIRECT3DVERTEXBUFFER9[m_pTile->iTileCount];

	// 타일 버텍스 버퍼 생성
	for( int i  = 0 ; i < m_pTile->iTileCount ; i++)
	{
		if(FAILED(g_pDevice->CreateVertexBuffer(m_pTile->pTile[i].iVtxSize*sizeof(SVtxNormTex),
			0, SVtxNormTex::FVF, D3DPOOL_MANAGED, &m_pVB[i], NULL)))
			return FALSE;

		SVtxNormTex* vertex;

		m_pVB[i]->Lock(0, 0, (void**)&vertex, 0);

		for( int j = 0 ; j < m_pTile->pTile[i].iVtxSize ; j ++)
		{
			*vertex++ = m_pTile->pTile[i].sVertex[j];	
		}

		m_pVB[i]->Unlock();

		if( m_pTile->pTile[i].iBlockSize != 0)
		{
			if(FAILED(g_pDevice->CreateVertexBuffer(m_pTile->pTile[i].iBlockSize*sizeof(SVtxNormTex),
				0, SVtxNormTex::FVF, D3DPOOL_MANAGED, &m_pBlockVB[i], NULL)))
				return FALSE;

			m_pBlockVB[i]->Lock(0, 0, (void**)&vertex, 0);
			
			for( j = 0 ; j < m_pTile->pTile[i].iBlockSize ; j ++)
			{
				*vertex++ = m_pTile->pTile[i].sBlock[j];	
			}
			
			m_pBlockVB[i]->Unlock();

		}

	}

	// 인덱스 버퍼 생성
	if(FAILED(g_pDevice->CreateIndexBuffer( pow(m_pTile->iDetail,2)*sizeof(MYINDEX)*2,
		0, D3DFMT_INDEX16, D3DPOOL_MANAGED, &m_pIB, NULL)))
		return FALSE;

	MYINDEX* index;

	m_pIB->Lock(0, 0, (void**)&index, 0);

	for(int z = 0 ; z < m_pTile->iDetail ; z++)
	{
		for(int x = 0 ; x < m_pTile->iDetail ; x++)
		{
			MYINDEX i;

			i._0 = z*(m_pTile->iDetail+1) + x;
			i._1 = (z+1)*(m_pTile->iDetail+1) + x;
			i._2 = z*(m_pTile->iDetail+1) + x + 1;
			
			*index++ = i;
			
			i._0 = (z+1)*(m_pTile->iDetail+1) + x;
			i._1 = (z+1)*(m_pTile->iDetail+1) + x + 1;
			i._2 = z*(m_pTile->iDetail+1) + x + 1;
			
			*index++ = i;
		}
	}

	m_pIB->Unlock();
/*
	// 블록 인덱스 버퍼 생성
	if(FAILED(g_pDevice->CreateIndexBuffer( 6*sizeof(MYINDEX)*2,
		0, D3DFMT_INDEX16, D3DPOOL_MANAGED, &m_pBlockIB, NULL)))
		return FALSE;
		
	m_pBlockIB->Lock(0, 0, (void**)&index, 0);
	
	MYINDEX idex;
	
	idex._0 = 3;	idex._1 = 0;	idex._2 = 1;
	*index++ = idex;
	
	idex._0 = 0;	idex._1 = 3;	idex._2 = 2;
	*index++ = idex;
	
	idex._0 = 7;	idex._1 = 4;	idex._2 = 6;
	*index++ = idex;
	
	idex._0 = 4;	idex._1 = 7;	idex._2 = 5;
	*index++ = idex;
	
	idex._0 = 5;	idex._1 = 0;	idex._2 = 4;
	*index++ = idex;
	
	idex._0 = 0;	idex._1 = 5;	idex._2 = 1;
	*index++ = idex;
	
	idex._0 = 7;	idex._1 = 1;	idex._2 = 5;
	*index++ = idex;
	
	idex._0 = 1;	idex._1 = 7;	idex._2 = 3;
	*index++ = idex;
	
	idex._0 = 6;	idex._1 = 3;	idex._2 = 7;
	*index++ = idex;
	
	idex._0 = 3;	idex._1 = 6;	idex._2 = 2;
	*index++ = idex;
	
	idex._0 = 4;	idex._1 = 2;	idex._2 = 6;
	*index++ = idex;
	
	idex._0 = 2;	idex._1 = 4;	idex._2 = 0;
	*index++ = idex;
				
	m_pBlockIB->Unlock();
*/
	return TRUE;
}

BOOL CTerrain::CreateModel()
{
	if( m_pTile->sModel.iCount <= 0 )
		return TRUE;
	
	CModel* m_Model;
	for( int i = 0 ; i < m_pTile->sModel.iCount ; i++ )
	{
		m_Model = new CModel();
		m_Model->LoadModel( m_pTile->sModel.sObject[i].strName );

		m_Model->SetSize(m_pTile->sModel.sObject[i].size);
		m_Model->SetDirection( Quaternion(m_pTile->sModel.sObject[i].qx, m_pTile->sModel.sObject[i].qy, m_pTile->sModel.sObject[i].qz, m_pTile->sModel.sObject[i].qw));
 		m_Model->SetPos( Vector3(m_pTile->sModel.sObject[i].x, m_pTile->sModel.sObject[i].y, m_pTile->sModel.sObject[i].z));
		
		m_lstModel.push_back( m_Model );
	}

	return TRUE;
}

BOOL CTerrain::CreateOverTexture()
{
	m_pOverTex = new LPDIRECT3DTEXTURE9[m_pTile->iTileCount];

	for(int i = 0 ; i < m_pTile->iTileCount ; i++)
	{	
		m_pOverTex[i] = NULL;
		D3DXCreateTextureFromFile( g_pDevice, m_pTile->pTile[i].strTextureName, &m_pOverTex[i]);
	}

	return TRUE;
}

BOOL CTerrain::CreateAlphaTexture()
{
	m_pAlphaTex = new LPDIRECT3DTEXTURE9[m_pTile->iTileCount];

	D3DLOCKED_RECT d3drc;

	for(int index = 0 ; index < m_pTile->iTileCount ; index++)
	{
		if( FAILED( D3DXCreateTexture( g_pDevice, m_pTile->iSize, m_pTile->iSize,
			1, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, &m_pAlphaTex[index] ) ) )
		{
			return FALSE;
		}

		if( FAILED(m_pAlphaTex[index]->LockRect(0, &d3drc, NULL, 0)) )
			return FALSE;

		BYTE* defBits = (BYTE*)d3drc.pBits;
		
		int i=0;
		
		for(int y=0 ; y<m_pTile->iSize ; y++ )
		{
			for(int x=0 ; x<m_pTile->iSize ; x++ )
			{
				// 검정색으로 초기화
				defBits[i++] = 0x00;	// B
				defBits[i++] = 0x00;	// G
				defBits[i++] = 0x00;	// R
				defBits[i++] = (BYTE)m_pTile->pTile[index].sAlphaTex.pCol[y].pAlpha[x];	// A
			}
		}

		if( FAILED(m_pAlphaTex[index]->UnlockRect(0)) )
			return FALSE;
	}

	return TRUE;
}

BOOL CTerrain::SetTerrain()
{
	// 조명 설정
	D3DLIGHT9 light;
	D3DXVECTOR3 vecLightDirUnnormalized(1, 1, 1);
    ZeroMemory( &light, sizeof(D3DLIGHT9) );
    light.Type        = D3DLIGHT_DIRECTIONAL;
    light.Diffuse.r   = 1.0f;
    light.Diffuse.g   = 1.0f;
    light.Diffuse.b   = 1.0f;
    D3DXVec3Normalize( (D3DXVECTOR3*)&light.Direction, &vecLightDirUnnormalized );
    light.Position.x  = 1.0f;
    light.Position.y  = 1.0f;
    light.Position.z  = 1.0f;
    light.Range       = 1000.0f;

	// 재질 설정
	m_Mtrl.Diffuse.r = m_Mtrl.Ambient.r = 1.0;
	m_Mtrl.Diffuse.g = m_Mtrl.Ambient.g = 1.0;
	m_Mtrl.Diffuse.b = m_Mtrl.Ambient.b = 1.0;
	m_Mtrl.Diffuse.a = m_Mtrl.Ambient.a = 1.0;

	if( FAILED(CreateTile()) )
		return FALSE;
	if( FAILED(CreateModel()) )
		return FALSE;
	if( FAILED(CreateAlphaTexture() ))
		return FALSE;
	if( FAILED(CreateOverTexture() ))
		return FALSE;
		
	return TRUE;
}

void CTerrain::Render()
{
	//g_pDevice->SetMaterial(&m_Mtrl);

	// 월드 메트릭스를 원점으로 셋팅
	Matrix44 matWorld;
	matWorld.SetIdentity();
	g_pDevice->SetTransform( D3DTS_WORLD, (D3DXMATRIX*)&matWorld );

	// 버텍스 버퍼 셋팅
	g_pDevice->SetFVF(SVtxNormTex::FVF);

	g_pDevice->SetTextureStageState( 0, D3DTSS_TEXCOORDINDEX, 0);
    g_pDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
    g_pDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
    g_pDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
	g_pDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1 );
    g_pDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
    g_pDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );
    g_pDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
    g_pDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );

	// 베이스 텍스처 설정
	g_pDevice->SetTexture(0, m_pBaseTex);

	// 조명을 킨다
//	g_pDevice->LightEnable(0, FALSE);
//	g_pDevice->LightEnable(1, TRUE);
/*
	// 블럭 출력
	for(int i = 0 ; i < m_pTile->iTileCount ; i++)
	{
		if(m_pTile->pTile[i].iBlockSize != 0)
		{
			
			g_pDevice->SetStreamSource(0, m_pBlockVB[i], 0, sizeof(SVtxNormTex));
			g_pDevice->SetIndices( m_pBlockIB );
			g_pDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0, 0, m_pTile->pTile[i].iBlockSize, 0, 12);
		}
	}
*/
	// 베이스 타일 그리기
	for(int i = 0 ; i < m_pTile->iTileCount ; i++)
	{
		g_pDevice->SetStreamSource(0, m_pVB[i], 0, sizeof(SVtxNormTex));
		g_pDevice->SetIndices( m_pIB );
		g_pDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0, 0, m_pTile->pTile[i].iVtxSize, 0, 2*pow(m_pTile->iDetail, 2));
	}

	// 알파 텍스처 설정
	g_pDevice->SetTextureStageState( 1, D3DTSS_TEXCOORDINDEX, 0);
	g_pDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );
	g_pDevice->SetTextureStageState( 1, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	g_pDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1 );
	g_pDevice->SetTextureStageState( 1, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
	
	// 덮어쓸 텍스처 설정
	g_pDevice->SetTextureStageState( 2, D3DTSS_TEXCOORDINDEX, 0);
	g_pDevice->SetTextureStageState( 2, D3DTSS_COLOROP,   D3DTOP_MODULATE );
	g_pDevice->SetTextureStageState( 2, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	g_pDevice->SetTextureStageState( 2, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
	g_pDevice->SetTextureStageState( 2, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1 );	
	g_pDevice->SetTextureStageState( 2, D3DTSS_ALPHAARG1, D3DTA_CURRENT );
	
	// 알파 블랜딩 셋팅
	g_pDevice->SetRenderState( D3DRS_SRCBLEND,   D3DBLEND_SRCALPHA );
	g_pDevice->SetRenderState( D3DRS_DESTBLEND,   D3DBLEND_INVSRCALPHA );
	g_pDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,   TRUE );

	// 덮어 쓸 타일 출력
	for(i = 0 ; i < m_pTile->iTileCount ; i++)
	{
		/*
		if( m_pOverTex[i] != NULL )
		{
			g_pDevice->SetTexture(2, m_pOverTex[i]);
		}
		*/
		g_pDevice->SetTexture(1, m_pAlphaTex[i]);
		g_pDevice->SetTexture(2, m_pOverTex[0]);
		g_pDevice->SetStreamSource(0, m_pVB[i], 0, sizeof(SVtxNormTex));
		g_pDevice->SetIndices( m_pIB );
		g_pDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0, 0, m_pTile->pTile[i].iVtxSize, 0, 2*pow(m_pTile->iDetail, 2));
		
	}

	// 알파 블랜딩 해제
	g_pDevice->SetTextureStageState( 1, D3DTSS_COLOROP, D3DTOP_DISABLE );
	g_pDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP, D3DTOP_DISABLE );
	g_pDevice->SetTextureStageState( 2, D3DTSS_COLOROP, D3DTOP_DISABLE );
	g_pDevice->SetTextureStageState( 2, D3DTSS_ALPHAOP, D3DTOP_DISABLE );
	
	// 알파 사용을 중지 한다.
	g_pDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );

	// 오브젝트 출력
	m_iterModel = m_lstModel.begin();
	while( m_iterModel != m_lstModel.end() )
	{
		(*m_iterModel)->Render();
		m_iterModel++;
	}

	// 조명을 끈다
//	g_pDevice->LightEnable(1, FALSE);
//	g_pDevice->LightEnable(0, TRUE);
}

void CTerrain::Clear()
{
	SAFE_ADELETE(m_pTile);
	SAFE_DELETE(m_pVB);
	SAFE_ADELETE(m_pBlockVB);
	SAFE_ADELETE(m_pAlphaTex);
	SAFE_ADELETE(m_pOverTex);

	m_iterModel = m_lstModel.begin();
	while( m_iterModel != m_lstModel.end() )
	{
		delete (*m_iterModel);
		m_iterModel++;
	}
	m_lstModel.clear();
}


// 0: 이동가능, 1: 이동불가
int CTerrain::GetAttribute()
{
	if( m_iTileNum < 0 || m_iTileNum >= m_pTile->iTileCount )
		return 1;

	return m_pTile->pTile[m_iTileNum].iAttribute;
}

float CTerrain::GetHeight(float x, float z)
{	
	int indexX;
	int indexZ;

	indexX = x/m_pTile->iSize;
	indexZ = z/m_pTile->iSize;

	if( indexX < 0 || indexX >= m_pTile->iWidth || indexZ < 0 || indexZ >= m_pTile->iHeight )
		return 0;

	m_iTileNum = indexZ*m_pTile->iWidth+indexX;
/*
	return IntersectTriangle(Vector3(x,0,z), 
		m_pTile->pTile[m_iTileNum].sVertex[1].v, 
		m_pTile->pTile[m_iTileNum].sVertex[2].v, 
		m_pTile->pTile[m_iTileNum].sVertex[3].v);
*/
	indexX = (int)x%m_pTile->iSize;
	indexZ = (int)z%m_pTile->iSize;

	int ud = indexX + indexZ;
	if( ud >= m_pTile->iSize )
	{
		return IntersectTriangle(Vector3(x,0,z), m_pTile->pTile[m_iTileNum].sVertex[1].v, m_pTile->pTile[m_iTileNum].sVertex[2].v, m_pTile->pTile[m_iTileNum].sVertex[3].v);	
	}else
	{
		return IntersectTriangle(Vector3(x,0,z), m_pTile->pTile[m_iTileNum].sVertex[0].v, m_pTile->pTile[m_iTileNum].sVertex[1].v, m_pTile->pTile[m_iTileNum].sVertex[2].v);
	}
/**/
	// 블럭 방식일 경우
//	return m_pTile->pTile[index].sVertex->v.y;
}

float CTerrain::IntersectTriangle(Vector3 Origin, Vector3 v0, Vector3 v1, Vector3 v2)
{
	float Dist;
	float det;
	Vector3 tvec;
	Vector3 pvec(0, 1, 0);
	Vector3 qvec;
	Vector3 edge1 = v1 - v0;
	Vector3 edge2 = v2 - v0;
	pvec = pvec.CrossProduct(edge2);
	
	det = edge1.DotProduct(pvec);
	
	tvec = Origin - v0;
	
	qvec = tvec.CrossProduct(edge1);
	
	Dist = edge2.DotProduct(qvec);

	Dist *= (1.0f / det);
	
	return Dist;
}

