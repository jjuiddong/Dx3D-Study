
#include "global.h"
#include "mesh.h"
#include "fileloader.h"
#include "track.h"


CMesh::CMesh():
m_pVtxBuff(NULL), m_pIdxBuff(NULL), m_pLoader(NULL), 
m_pMtrl(NULL), m_pTex(NULL),
m_nId(0), m_pBox(NULL)
{
	m_matAniTM.SetIdentity();

}


CMesh::CMesh( SMeshLoader *pLoader, BOOL bCollision ) :
m_pVtxBuff(NULL), m_pIdxBuff(NULL), m_pLoader(NULL), 
m_pMtrl(NULL), m_pTex(NULL), m_pBox(NULL),
m_nId(0)
{
	m_matAniTM.SetIdentity();
	LoadMesh( pLoader );

	if( bCollision )
	{
		// 충돌박스를 생성한다.
		m_pBox = new Box;
		d3d::CreateBox( pLoader, m_pBox );
	}
}


CMesh::~CMesh()
{
	SAFE_DELETE( m_pBox );

}


//-----------------------------------------------------------------------------//
// Mesh, Palette 데이타 로드
//-----------------------------------------------------------------------------//
BOOL CMesh::LoadMesh( SMeshLoader *pLoader )
{
	m_nId = pLoader->id;
	m_pLoader = pLoader;
	m_pMtrl = m_pLoader->pMtrl;
	m_pVtxBuff = pLoader->pvtxbuff;
	m_pIdxBuff = pLoader->pidxbuff;

	if( 0 < pLoader->vn.size ) // FVF = D3DFVF_XYZ | D3DFVF_NORMAL
	{
		m_VtxInfo.dwFVF = SVtxNorm::FVF;
		m_VtxInfo.nStride = sizeof( SVtxNorm );
		m_VtxInfo.nSize = pLoader->vn.size;
		m_VtxInfo.pV = (BYTE*)pLoader->vn.pV;
	}
	else // FVF = D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1
	{
		m_VtxInfo.dwFVF = SVtxNormTex::FVF;
		m_VtxInfo.nStride = sizeof( SVtxNormTex );
		m_VtxInfo.nSize = pLoader->vnt.size;
		m_VtxInfo.pV = (BYTE*)pLoader->vnt.pV;
	}

	// texture 생성
	if( m_pMtrl )
	{
		// texture file이 등록되어 있다면
		if( m_pMtrl->szFileName[ 0] )
		{
			m_pTex = CFileLoader::LoadTexture( m_pMtrl->szFileName );
			g_pDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
			g_pDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
		}
	}

	return TRUE;
}


//-----------------------------------------------------------------------------//
// 출력
//-----------------------------------------------------------------------------//
void CMesh::Render()
{
	if( g_test <= 0 )
		g_test = m_pLoader->i.size;

	if( 0 >= m_VtxInfo.nSize ) return;
	if( m_pMtrl ) g_pDevice->SetMaterial( &m_pMtrl->mtrl );

	g_pDevice->SetSamplerState( 0, D3DSAMP_MIPFILTER, D3DTEXF_POINT );

	g_pDevice->SetTexture( 0, m_pTex );
	g_pDevice->MultiplyTransform( D3DTS_WORLD, (D3DXMATRIX*)&m_matAniTM );
	g_pDevice->SetStreamSource( 0, m_pVtxBuff, 0, m_VtxInfo.nStride );
	g_pDevice->SetFVF( m_VtxInfo.dwFVF );
	g_pDevice->SetIndices( m_pIdxBuff );
	g_pDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0, 0, m_VtxInfo.nSize, 0, m_pLoader->i.size );

#if defined(_TEST_MODE)
	if( m_pBox ) m_pBox->Render();
#endif // _TEST_MODE

}


void CMesh::UpdateCollisionBox( Matrix44 *pMat )
{
	if( m_pBox ) 
	{
		m_pBox->SetWorldTM( &(m_matAniTM * *pMat) );
		m_pBox->Update();	
	}
}
