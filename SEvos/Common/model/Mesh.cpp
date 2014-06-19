
#include "stdafx.h"
//#include "global.h"
#include "mesh.h"
#include "..//common//fileloader.h"
#include "track.h"


CMesh::CMesh():
m_pVtxBuff(NULL), m_pIdxBuff(NULL), m_pLoader(NULL), 
m_pMtrl(NULL), m_pTex(NULL),
m_nId(0), m_pBox(NULL), m_isDynamicLoad(FALSE)
{
	m_matAniTM.SetIdentity();

}


CMesh::CMesh( SMeshLoader *pLoader, BOOL bCollision ) :
m_pVtxBuff(NULL), m_pIdxBuff(NULL), m_pLoader(NULL), 
m_pMtrl(NULL), m_pTex(NULL), m_pBox(NULL),
m_nId(0), m_isDynamicLoad(FALSE)
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
	if (m_isDynamicLoad) {
		SAFE_RELEASE( m_pVtxBuff );
		SAFE_RELEASE( m_pIdxBuff );
		m_isDynamicLoad = FALSE;
	}
}


//-----------------------------------------------------------------------------//
// 버텍스 버퍼, 인덱스 버퍼를 동적으로 생성한다.
// 
//-----------------------------------------------------------------------------//
BOOL CMesh::Create( int id, int vtxBufSize, int stride, DWORD fvf, int idxBufSize )
{
	if (vtxBufSize <= 0 || stride <= 0) return FALSE;

	HRESULT result;
	result = g_pDevice->CreateVertexBuffer( vtxBufSize*stride, 0, fvf, D3DPOOL_MANAGED, &m_pVtxBuff, NULL );
	if (D3D_OK != result)
		return FALSE;

	if (idxBufSize > 0) {
		result = g_pDevice->CreateIndexBuffer( idxBufSize*sizeof(WORD), D3DUSAGE_WRITEONLY, 
				D3DFMT_INDEX16, D3DPOOL_MANAGED, &m_pIdxBuff, NULL );
		if (D3D_OK != result) 
			return FALSE;
	}

	m_isDynamicLoad = TRUE;

	m_nId = id;
	m_pTex = NULL;
	m_pMtrl = NULL;
	m_pLoader = NULL;

	m_VtxInfo.nSize = vtxBufSize;
	m_VtxInfo.nTriangleCount = idxBufSize;
	m_VtxInfo.nStride = stride;
	m_VtxInfo.dwFVF = fvf;
	m_VtxInfo.pV = NULL;

	return TRUE;
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
		m_VtxInfo.nTriangleCount = pLoader->i.size;
		m_VtxInfo.pV = (BYTE*)pLoader->vn.pV;
	}
	else // FVF = D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1
	{
		m_VtxInfo.dwFVF = SVtxNormTex::FVF;
		m_VtxInfo.nStride = sizeof( SVtxNormTex );
		m_VtxInfo.nSize = pLoader->vnt.size;
		m_VtxInfo.nTriangleCount = pLoader->i.size;
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
	if( 0 >= m_VtxInfo.nSize ) return;
	if( m_pMtrl ) g_pDevice->SetMaterial( &m_pMtrl->mtrl );

	g_pDevice->SetSamplerState( 0, D3DSAMP_MIPFILTER, D3DTEXF_POINT );

	g_pDevice->SetTexture( 0, m_pTex );
	g_pDevice->MultiplyTransform( D3DTS_WORLD, (D3DXMATRIX*)&m_matAniTM );
	g_pDevice->SetStreamSource( 0, m_pVtxBuff, 0, m_VtxInfo.nStride );
	g_pDevice->SetFVF( m_VtxInfo.dwFVF );
	g_pDevice->SetIndices( m_pIdxBuff );
	g_pDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0, 0, m_VtxInfo.nSize, 0, m_VtxInfo.nTriangleCount );

	if (m_IsRenderCollisionBox && m_pBox)
		m_pBox->Render_Hierarchy();

}


//------------------------------------------------------------------------
// D3DPT_LINESTRIP 타입으로 출력한다.
// [2011/2/14 jjuiddong]
//------------------------------------------------------------------------
void CMesh::RenderLineStrip()
{
	g_pDevice->SetRenderState( D3DRS_LIGHTING, FALSE );
	g_pDevice->SetStreamSource( 0, m_pVtxBuff, 0, m_VtxInfo.nStride );
	g_pDevice->SetFVF( m_VtxInfo.dwFVF );
	g_pDevice->DrawPrimitive( D3DPT_LINESTRIP, 0, m_VtxInfo.nSize);
	g_pDevice->SetRenderState( D3DRS_LIGHTING, TRUE );
}


void CMesh::UpdateCollisionBox( Matrix44 *pMat )
{
	if (m_pBox)
	{
		m_pBox->SetWorldTM( &(m_matAniTM * *pMat) );
		m_pBox->Update();
	}
}


//-----------------------------------------------------------------------------//
// 버텍스 갯수를 설정한다. (동적으로 메쉬가 움직일때 필요한 함수다.)
// 실제 버퍼 갯수가 바뀌는 것은 아니다.
//-----------------------------------------------------------------------------//
void CMesh::setVtxBuffCount(int vtxCount)
{
	m_VtxInfo.nSize = vtxCount;
}


//-----------------------------------------------------------------------------//
// 인덱스 버퍼 갯수를 설정한다. (동적으로 메쉬가 움직일때 필요한 함수다.)
// 실제 버퍼 갯수가 바뀌는 것은 아니다.
//-----------------------------------------------------------------------------//
void CMesh::setIdxBuffCount(int idxCount)
{
	m_VtxInfo.nTriangleCount = idxCount;
}

int CMesh::getVtxBuffCount()
{
	return m_VtxInfo.nSize;
}
int CMesh::getIdxBuffCount()
{
	return m_VtxInfo.nTriangleCount;
}


//------------------------------------------------------------------------
// 
// [2011/2/26 jjuiddong]
//------------------------------------------------------------------------
BOOL CMesh::Pick(const Matrix44 &parentTM, const Vector2 &pickPos)
{
	BOOL isPick = FALSE;
	if (m_pBox)
	{
		m_pBox->SetWorldTM((Matrix44*)&parentTM);
		isPick = m_pBox->Pick(pickPos);
		Matrix44 mat;
		mat.SetIdentity();
		m_pBox->SetWorldTM(&mat);
	}
	return isPick;
}
