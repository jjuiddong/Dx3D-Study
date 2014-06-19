
#include "stdafx.h"
#include "scene.h"
#include "uicontrol.h"



CUIControl::CUIControl() : m_bVisible(TRUE)
{

}


CUIControl::~CUIControl() 
{

}


//-----------------------------------------------------------------------------//
// 콘트롤 생성
//-----------------------------------------------------------------------------//
BOOL CUIControl::Create( SControl *pCtrl )
{
	m_Type = pCtrl->type;
	m_Id = pCtrl->id;

	int w = pCtrl->w;
	int h = pCtrl->h;
	if( 0 < pCtrl->cw ) w = pCtrl->cw;
	if( 0 < pCtrl->ch ) h = pCtrl->ch;

	m_Rect.left = pCtrl->x;
	m_Rect.top = pCtrl->y;
	m_Rect.right = pCtrl->x + w;
	m_Rect.bottom = pCtrl->y + h;

	SetVisible( pCtrl->visible );

	return TRUE;
}

void CUIControl::SendMessage( SUIMsg *pMsg )
{
	MsgProc( pMsg );
}


BOOL CUIControl::InRect( POINT *pt )
{
	return ::PtInRect( &m_Rect, *pt );
}



///////////////////////////////////////////////////////////////////////////////////
// CUIImage
///////////////////////////////////////////////////////////////////////////////////

CUIImage::CUIImage() : m_nIdx(0)
{

}


CUIImage::~CUIImage()
{

}


//-----------------------------------------------------------------------------//
// 이미지클래스 생성
//-----------------------------------------------------------------------------//
BOOL CUIImage::Create( SControl *pCtrl )
{
	CUIControl::Create( pCtrl );

	m_pTex = CFileLoader::LoadTexture( pCtrl->file );
	if( !m_pTex ) return FALSE;

	m_Width = pCtrl->w;
	m_Height = pCtrl->h;

	SetImageIndex( 0 );

	return TRUE;
}


//-----------------------------------------------------------------------------// 
// 출력
//-----------------------------------------------------------------------------//
void CUIImage::Render()
{
	if( !m_bVisible ) return;

	g_pDevice->SetSamplerState( 0, D3DSAMP_MIPFILTER, D3DTEXF_NONE );

	g_pDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE ); 
	g_pDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA ); 
	g_pDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA ); 

	g_pDevice->SetTransform( D3DTS_WORLD, &g_matIdentity ); // identity matrix
	g_pDevice->SetTexture( 0, m_pTex );
	g_pDevice->SetFVF( SVtxRhwTex::FVF );
	g_pDevice->DrawPrimitiveUP( D3DPT_TRIANGLELIST, 2, m_V, sizeof(SVtxRhwTex) );

	g_pDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE ); 

}


//-----------------------------------------------------------------------------//
// 에니메이션
//-----------------------------------------------------------------------------//
void CUIImage::Animate( int nTimeDelta )
{
	if( !m_bVisible ) return;

}


//-----------------------------------------------------------------------------//
// 메세지 처리
//-----------------------------------------------------------------------------//
BOOL CUIImage::MsgProc( SUIMsg *pMsg ) 
{
	switch( pMsg->msg )
	{
	case UIMSG_SETINDEX:	SetImageIndex( pMsg->wparam ); break;
	}

	return TRUE; 
}


//-----------------------------------------------------------------------------//
// 이미지가 가로로 한개이상 연속으로 되어있을때 출력될 이미지 크기와 원본 이미지 크기를
// 계산해서 출력한다.
// x = (w * nIdx) / ImageWidth
//-----------------------------------------------------------------------------//
void CUIImage::SetImageIndex( int nIdx )
{
	const float RHW = 1.f;
	float x = (float)m_Rect.left;
	float y = (float)m_Rect.top;
	float w = (float)(m_Rect.right - m_Rect.left);
	float h = (float)(m_Rect.bottom - m_Rect.top);

	m_nIdx = nIdx;

	float tl = (float)(w * nIdx) / (float)m_Width;
	float tu = (float)w / (float)m_Width;

	Vector3 v[] = 
	{
		Vector3( x, y, 0 ),
		Vector3( x+w, y, 0 ),
		Vector3( x, y+h, 0 ),
		Vector3( x+w, y+h, 0 ),
	};
	int idx[] = {0,1,2, 1,3,2};
	float uv[] = { tl,0, tl+tu,0, tl,1, tl+tu,0, tl+tu,1, tl,1, };

	for( int i=0; i < 6; ++i )
	{
		m_V[ i].v = v[ idx[ i]];
		m_V[ i].rhw = RHW;
		m_V[ i].tu = uv[ i*2];
		m_V[ i].tv = uv[ i*2+1];
	}

}


//-----------------------------------------------------------------------------//
// UI 창크기를 설정한다.
//-----------------------------------------------------------------------------//
void CUIImage::SetRect( RECT *pRect )
{
	CUIControl::SetRect( pRect );
	SetImageIndex( m_nIdx );
}


///////////////////////////////////////////////////////////////////////////////////
// CUIButton
///////////////////////////////////////////////////////////////////////////////////


CUIButton::CUIButton()
{
	m_pImg = NULL;

}


CUIButton::~CUIButton()
{
	SAFE_DELETE( m_pImg );

}


//-----------------------------------------------------------------------------//
// 버튼 클래스 생성
//-----------------------------------------------------------------------------//
BOOL CUIButton::Create( SControl *pCtrl )
{
	CUIControl::Create( pCtrl );
	m_pImg = new CUIImage;
	m_pImg->Create( pCtrl );

	return TRUE;
}


//-----------------------------------------------------------------------------//
// 출력
//-----------------------------------------------------------------------------//
void CUIButton::Render()
{
	m_pImg->Render();

}


//-----------------------------------------------------------------------------//
// 에니메이션
//-----------------------------------------------------------------------------//
void CUIButton::Animate( int nTimeDelta )
{

}


//-----------------------------------------------------------------------------//
// 메세지 처리
//-----------------------------------------------------------------------------//
BOOL CUIButton::MsgProc( SUIMsg *pMsg )
{
	switch( pMsg->msg )
	{
	case UIMSG_LCLICK:	m_pImg->SetImageIndex( 1 ); break;
	case UIMSG_LCLICKUP:m_pImg->SetImageIndex( 0 ); break;
	}
	return TRUE;
}

