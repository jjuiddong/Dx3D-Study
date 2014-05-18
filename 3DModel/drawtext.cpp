
#include "global.h"
#include "drawtext.h"

using namespace std;

SVector3s g_TextIdx[ 2];
D3DXMATRIXA16 matWorld;

CDrawText::CDrawText() : m_pDxFont(NULL)
{

	g_TextIdx[ 0].x = 0;
	g_TextIdx[ 0].y = 1;
	g_TextIdx[ 0].z = 3;

	g_TextIdx[ 1].x = 0;
	g_TextIdx[ 1].y = 2;
	g_TextIdx[ 1].z = 3;
/**/
/*
	g_TextIdx[ 0].x = 0;
	g_TextIdx[ 0].y = 3;
	g_TextIdx[ 0].z = 1;

	g_TextIdx[ 1].x = 1;
	g_TextIdx[ 1].y = 3;
	g_TextIdx[ 1].z = 2;
/**/

	D3DXMatrixIdentity( &matWorld );
}


CDrawText::~CDrawText() 
{
	Clear();
}


//-----------------------------------------------------------------------------//
// 클래스 생성 및 초기화
// 폰트 초기화, LPD3DXFONT 생성
//-----------------------------------------------------------------------------//
BOOL CDrawText::Create()
{
    if( FAILED(d3d::CreateD3DXFont(g_pDevice, &m_pDxFont, "Arial", 18)) )
        return FALSE;

	return TRUE;
}


//-----------------------------------------------------------------------------//
// 텍스트 출력
// x,y 좌표에 이미 텍스트가 있다면 거기에 텍스트를 저장한다.
// 없다면 bmp를 통해서 텍스트를 등록한후 텍스쳐로 로딩한다.
//-----------------------------------------------------------------------------//
void CDrawText::DrawText( int X, int Y, char *pString, int FontId ) // FontId=0
{
	const int WIDTH = 50;

	SText *ptext;
	int key = MAKELONG( X, Y );
	TextMap::iterator it = m_Textmap.find( key );
	if( m_Textmap.end() == it )
	{
		IDirect3DTexture9 *ptex = NULL;
		if( D3D_OK != D3DXCreateTexture(g_pDevice, WIDTH, 30, 0, D3DUSAGE_RENDERTARGET, 
										D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &ptex) )
			return;

		ptext = new SText;
		ptext->x = X;
		ptext->y = Y;
		ptext->ptex = ptex;
		ptext->v[ 0].v = Vector3( X, Y, 0 );
//		ptext->v[ 0].n = Vector3( 0, 0, -1 );
//		ptext->v[ 0].tu = 0.f;
//		ptext->v[ 0].tv = 0.f;
		ptext->v[ 1].v = Vector3( X+WIDTH, Y, 0 );
//		ptext->v[ 1].n = Vector3( 0, 0, -1 );
//		ptext->v[ 1].tu = 1.f;
//		ptext->v[ 1].tv = 0.f;
		ptext->v[ 2].v = Vector3( X+WIDTH, Y+30, 0 );
//		ptext->v[ 2].n = Vector3( 0, 0, -1 );
//		ptext->v[ 2].tu = 1.f;
//		ptext->v[ 2].tv = 1.f;
		ptext->v[ 3] = ptext->v[ 0];
		ptext->v[ 4] = ptext->v[ 2];
		ptext->v[ 5].v = Vector3( X, Y+30, 0 );
//		ptext->v[ 5].n = Vector3( 0, 0, -1 );
//		ptext->v[ 5].tu = 0.f;
//		ptext->v[ 5].tv = 1.f;		
		
		m_Textmap.insert( TextMap::value_type(key,ptext) );
	}
	else 
	{
		ptext = it->second;
	}

	SetText( FontId, ptext, pString );
}


//-----------------------------------------------------------------------------//
// 
//-----------------------------------------------------------------------------//
void CDrawText::SetText( int FontId, SText *pTextData, char *pString )
{
	D3DLOCKED_RECT rect;
	pTextData->ptex->LockRect( 0, &rect, NULL, D3DLOCK_DISCARD );

	pTextData->ptex->UnlockRect( 0 );
}


//-----------------------------------------------------------------------------//
// 출력
//-----------------------------------------------------------------------------//
void CDrawText::Render()
{
	if( !m_pDxFont ) return;

	g_pDevice->SetRenderState( D3DRS_POINTSCALEENABLE, TRUE );
	g_pDevice->SetTransform( D3DTS_WORLD, &matWorld );

	TextMap::iterator it = m_Textmap.begin();
	while( m_Textmap.end() != it )
	{
		SText *p = it++->second;

		g_pDevice->SetFVF( SVtxDiff::FVF );
		g_pDevice->DrawPrimitiveUP( D3DPT_LINELIST, 3, p->v, sizeof(SVtxDiff) );	

//		g_pDevice->SetTexture( 0, p->ptex );
//		g_pDevice->SetFVF( SVtxNormTex::FVF );
//		g_pDevice->DrawIndexedPrimitiveUP( D3DPT_TRIANGLELIST, 0, 4, 2, g_TextIdx, 
//										   D3DFMT_INDEX16, p->v, sizeof(SVtxNormTex) );


//		g_pDevice->SetStreamSource( 0, m_pVtxBuff, 0, m_VtxInfo.nStride );
//		g_pDevice->SetFVF( SVtxNormTex::FVF );
//		g_pDevice->SetIndices( m_pIdxBuff );
//		g_pDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0, 0, m_VtxInfo.nSize, 0, m_pLoader->i.size );
	}

	g_pDevice->SetRenderState( D3DRS_POINTSCALEENABLE, TRUE );




	RECT rc;
	SetRect( &rc, 60, 10, 0, 0 );
/*
	m_pDxFont->Begin();

	m_pDxFont->DrawText( "한글 출력", -1, &rc, DT_SINGLELINE, D3DCOLOR_ARGB(255, 255, 0, 255) );
	rc.top += 25;
	m_pDxFont->DrawText( "한글 출력", -1, &rc, DT_SINGLELINE, D3DCOLOR_ARGB(255, 255, 0, 255) );
	rc.top += 25;
	m_pDxFont->DrawText( "한글 출력", -1, &rc, DT_SINGLELINE, D3DCOLOR_ARGB(255, 255, 0, 255) );
	rc.top += 25;
	m_pDxFont->DrawText( "한글 출력", -1, &rc, DT_SINGLELINE, D3DCOLOR_ARGB(255, 255, 0, 255) );
	rc.top += 25;
	m_pDxFont->DrawText( "한글 출력", -1, &rc, DT_SINGLELINE, D3DCOLOR_ARGB(255, 255, 0, 255) );
	rc.top += 25;
	m_pDxFont->DrawText( "한글 출력", -1, &rc, DT_SINGLELINE, D3DCOLOR_ARGB(255, 255, 0, 255) );
	rc.top += 25;
	m_pDxFont->DrawText( "한글 출력", -1, &rc, DT_SINGLELINE, D3DCOLOR_ARGB(255, 255, 0, 255) );
	rc.top += 25;
	m_pDxFont->DrawText( "한글 출력", -1, &rc, DT_SINGLELINE, D3DCOLOR_ARGB(255, 255, 0, 255) );
	rc.top += 25;
	m_pDxFont->DrawText( "한글 출력", -1, &rc, DT_SINGLELINE, D3DCOLOR_ARGB(255, 255, 0, 255) );
	rc.top += 25;
	m_pDxFont->DrawText( "한글 출력", -1, &rc, DT_SINGLELINE, D3DCOLOR_ARGB(255, 255, 0, 255) );
	rc.top += 25;
	m_pDxFont->DrawText( "한글 출력", -1, &rc, DT_SINGLELINE, D3DCOLOR_ARGB(255, 255, 0, 255) );

	m_pDxFont->End();
/**/

}


//-----------------------------------------------------------------------------//
// 저장된 메모리 제거
//-----------------------------------------------------------------------------//
void CDrawText::Clear()
{
	TextMap::iterator it = m_Textmap.begin();
	while( m_Textmap.end() != it )
	{
		SText *p = it++->second;
		SAFE_RELEASE( p->ptex );
		delete p;
	}
	m_Textmap.clear();

	list< SText* >::iterator i = m_Garbage.begin();
	while( m_Garbage.end() != i )
	{
		SText *p = *i++;
		SAFE_RELEASE( p->ptex );
		delete p;
	}
	m_Garbage.clear();

	SAFE_RELEASE( m_pDxFont );
}
