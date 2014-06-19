
#include "StdAfx.h"
#include "ModelViewer.h"
#include "model/Model.h"
#include "ViewPanel.h"
#include "common/light.h"
#include "ModelView.h"



// Cross Line 출력
inline void DrawXYZ()
{
	g_pDevice->SetRenderState( D3DRS_LIGHTING, FALSE );

	g_pDevice->SetTransform( D3DTS_WORLD, &g_matIdentity ); // identity matrix

	const int cross_size = 6;
	SVtxDiff crossline[ cross_size];
	// x axis
	crossline[ 0].v = Vector3( 0.f, 0.f, 0.f );
	crossline[ 1].v = Vector3( 100.f, 0.f, 0.f );
	crossline[ 0].c = D3DXCOLOR( 1, 0, 0, 0 );
	crossline[ 1].c = D3DXCOLOR( 1, 0, 0, 0 );
	// y axis
	crossline[ 2].v = Vector3( 0.f, 0.f, 0.f );
	crossline[ 3].v = Vector3( 0.f, 100.f, 0.f );
	crossline[ 2].c = D3DXCOLOR( 0, 1, 0, 0 );
	crossline[ 3].c = D3DXCOLOR( 0, 1, 0, 0 );
	// z axis
	crossline[ 4].v = Vector3( 0.f, 0.f, 0.f );
	crossline[ 5].v = Vector3( 0.f, 0.f, 100.f );
	crossline[ 4].c = D3DXCOLOR( 0, 0, 1, 0 );
	crossline[ 5].c = D3DXCOLOR( 0, 0, 1, 0 );
	g_pDevice->SetFVF( SVtxDiff::FVF );
	g_pDevice->DrawPrimitiveUP( D3DPT_LINELIST, cross_size/2, crossline, sizeof(SVtxDiff) );


	SVtxDiff tri[ 9];
	tri[ 0].v = Vector3( 0.f, 0.f, 0.f );
	tri[ 0].c = D3DXCOLOR( 1, 0, 0, 0 );
	tri[ 2].v = Vector3( 10.f, 0.f, 0.f );
	tri[ 2].c = D3DXCOLOR( 0, 1, 0, 0 );
	tri[ 1].v = Vector3( 5.f, 5.f, 0.f );
	tri[ 1].c = D3DXCOLOR( 0, 0, 1, 0 );

	tri[ 3].v = Vector3( 0.f, 0.f, 0.f );
	tri[ 3].c = D3DXCOLOR( 1, 0, 0, 0 );
	tri[ 5].v = Vector3( -5.f, 5.f, 0.f );
	tri[ 5].c = D3DXCOLOR( 0, 0, 1, 0 );
	tri[ 4].v = Vector3( -10.f, 0.f, 0.f );
	tri[ 4].c = D3DXCOLOR( 0, 1, 0, 0 );

	tri[ 6].v = Vector3( 0.f, 0.f, 0.f );
	tri[ 6].c = D3DXCOLOR( 1, 0, 0, 0 );
	tri[ 8].v = Vector3( 5.f, 5.f, 0.f );
	tri[ 8].c = D3DXCOLOR( 0, 1, 0, 0 );
	tri[ 7].v = Vector3( -5.f, 5.f, 0.f );
	tri[ 7].c = D3DXCOLOR( 0, 0, 1, 0 );
	g_pDevice->SetFVF( SVtxDiff::FVF );
	g_pDevice->DrawPrimitiveUP( D3DPT_TRIANGLELIST, 3, tri, sizeof(SVtxDiff) );

	g_pDevice->SetRenderState( D3DRS_LIGHTING, TRUE );
}


CModelView::CModelView(void)
{
	m_pModel = new CModel();
	m_fRMouseZ = 20.f;
	m_ElapseTime = 0;
	m_Fps = 0;
	sprintf_s( m_szFps, sizeof(m_szFps), "fps: 0" );
}

CModelView::~CModelView(void)
{
	SAFE_DELETE( m_pModel );
	SAFE_DELETE( m_pLight );

	SAFE_RELEASE( m_pTextSprite );
	SAFE_RELEASE( m_pDxFont );

}

BEGIN_MESSAGE_MAP(CModelView, CView)
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEWHEEL()
	ON_WM_MOUSEMOVE()
END_MESSAGE_MAP()

BOOL CModelView::OnEraseBkgnd(CDC* pDC)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	return CView::OnEraseBkgnd(pDC);
}

void CModelView::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	CView::OnLButtonDown(nFlags, point);
}

void CModelView::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	CView::OnLButtonUp(nFlags, point);
}

BOOL CModelView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	if( 0 == nFlags )
		m_Mouse.SetMouseWheel( -zDelta );
	return CView::OnMouseWheel(nFlags, zDelta, pt);
}

void CModelView::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	CView::OnMouseMove(nFlags, point);
}

void CModelView::OnDraw(CDC* /*pDC*/)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
}


//------------------------------------------------------------------------
// 
// [2011/2/21 jjuiddong]
//------------------------------------------------------------------------
BOOL CModelView::Init()
{

	CRect r;
	GetClientRect( r );
	m_Mouse.Init( m_hWnd );
	m_Mouse.SetViewport( 0, 0, r.Width(), r.Height() );
	m_Mouse.Update();
	m_nRMouseX = m_Mouse.GetMouseX();
	m_nRMouseY = m_Mouse.GetMouseY();


	// create sprite
	HRESULT hr;
	if( FAILED(hr = D3DXCreateSprite(g_pDevice, &m_pTextSprite)) )
		return FALSE;
	// create text
	hr = D3DXCreateFont( g_pDevice, 18, 0, FW_BOLD, 1, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, 
		DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "굴림", &m_pDxFont );
	if( FAILED(hr) )
		return FALSE;

	m_pLight = new CLight();
	m_pLight->SetDirectionalLight(d3d::WHITE);
	m_pLight->SetPosition(Vector3(0, 10, 0));
	m_pLight->SetDirection(Vector3(0, -1.f, 0));

	return TRUE;
}


//------------------------------------------------------------------------
// 
// [2011/2/21 jjuiddong]
//------------------------------------------------------------------------
void CModelView::Render()
{
	RenderFps();
	DrawXYZ();
	MouseProc();

	m_pLight->LightOn();

	

	m_pModel->Render();

	m_pLight->LightOff();
}


//------------------------------------------------------------------------
// 
// [2011/2/21 jjuiddong]
//------------------------------------------------------------------------
void CModelView::Update( int nElapsTick )
{
	KeyProc( nElapsTick );

	m_pModel->Update(nElapsTick);

	// fps 계산 ---------------------------------------
	++m_Fps;
	m_ElapseTime += nElapsTick;
	if( 1000 <= m_ElapseTime )
	{
		sprintf_s( m_szFps, sizeof(m_szFps), "fps: %d", m_Fps );
		m_Fps = 0;
		m_ElapseTime = 0;
	}
	//--------------------------------------------------

}


//------------------------------------------------------------------------
// 
// [2011/2/22 jjuiddong]
//------------------------------------------------------------------------
void CModelView::Update(IPanelObserver *pObserver)
{
	PANEL_TYPE type = pObserver->GetPanelType();
	switch (type)
	{
	case VIEW:
		{
			CViewPanel *pviewPanel = (CViewPanel*)pObserver;
			m_pModel->LoadModel((char*)(LPCTSTR)pviewPanel->GetSelectModelName());
			m_pModel->LoadAni((char*)(LPCTSTR)pviewPanel->GetSelectAnimationName());
			m_pModel->SetAnimation(0, 0, TRUE, TRUE, TRUE);
		}
		break;
	}
}


//------------------------------------------------------------------------
// 
// [2011/2/22 jjuiddong]
//------------------------------------------------------------------------
void CModelView::Clear()
{
	SAFE_DELETE(m_pModel);

}


//-----------------------------------------------------------------------------//
// fps 출력
//-----------------------------------------------------------------------------//
void CModelView::RenderFps()
{
	m_pTextSprite->Begin( D3DXSPRITE_ALPHABLEND | D3DXSPRITE_SORT_TEXTURE );

	CRect rc(10,10,200,200);
	m_pDxFont->DrawText( m_pTextSprite, m_szFps, -1, &rc,
		DT_NOCLIP, D3DXCOLOR( 1.0f, 1.0f, 1.0f, 1.0f ) );

	m_pTextSprite->End();
}


//-----------------------------------------------------------------------------//
// 
//-----------------------------------------------------------------------------//
void CModelView::MouseProc()
{
	g_Camera.Execute();
	m_Mouse.Update();

	// Push R Button
	if (m_Mouse.GetMouseR())
	{
		int mx = m_Mouse.GetMouseX() - m_nRMouseX;
		int my = m_Mouse.GetMouseY() - m_nRMouseY;
		g_Camera.RotateY( (float)mx * 0.2f );
		g_Camera.RotateX( (float)my * 0.2f );
	} //if

	if (::GetAsyncKeyState(VK_CONTROL) & 0x8000f)
	{
		int mx = m_Mouse.GetMouseX() - m_nRMouseX;
		int my = m_Mouse.GetMouseY() - m_nRMouseY;
		Vector3 dir = m_pLight->GetDirection();
		dir.RotateZ( mx * 0.1f );
		dir.RotateX( my * 0.1f );
		m_pLight->SetDirection(dir);
		//		m_pTerrain->GenerateTexture();
		//		m_pTerrain->CalcLight( &m_vLight );
	}

	if (::GetAsyncKeyState(VK_SHIFT) & 0x8000f)
	{
		//const Vector3 pos = m_pCursor->GetCursorPos();
		//const Vector3 lightPos = m_pLight->GetPosition();
		//m_pLight->SetPosition(Vector3(pos.x, lightPos.y, pos.z));
		//m_pLight->SetRange(1000.f);
	}

	//	Vector3 vCamPos = g_Camera.GetPosition();
	//	vCamPos.z = m_pTerrain->GetHeight(vCamPos.x, vCamPos.y) + m_fRMouseZ;
	//	vCamPos.z = m_fRMouseZ;
	//	g_Camera.SetPosition( vCamPos );

	m_nRMouseX = m_Mouse.GetMouseX();
	m_nRMouseY = m_Mouse.GetMouseY();

	if( m_Mouse.GetMouseZ() )
	{
		m_fRMouseZ += (float)m_Mouse.GetMouseZ() * 0.08f;
		g_Dbg.Console( "%f\n", m_fRMouseZ );
		if( 0.f > m_fRMouseZ ) 
			m_fRMouseZ = 0.f;
	}
}



//-----------------------------------------------------------------------------//
// key 입력
//-----------------------------------------------------------------------------//
void CModelView::KeyProc( int nElapsTick )
{
	if (nElapsTick > 1000) return;

	g_Camera.Execute();
	m_Mouse.Update();

	Vector3 vCamPos = g_Camera.GetPosition();
	vCamPos.y = m_fRMouseZ;
	g_Camera.SetPosition( vCamPos );

	Vector3 vPos = g_Camera.GetPosition();
	Vector3	vDir = g_Camera.GetFront();
	Vector3	vRight = g_Camera.GetRight();

	vDir.y = 0.f;
	vDir.Normalize();
	vRight.y = 0.f;
	vRight.Normalize();

	float fOffset = 50.f * (float)nElapsTick;
	if( ::GetAsyncKeyState( VK_SPACE ) & 0x8000f )
		fOffset *= 0.1f;
	if( ::GetAsyncKeyState('W') & 0x8000f )
		vPos += vDir * fOffset;
	if( ::GetAsyncKeyState('S') & 0x8000f )
		vPos -= vDir * fOffset;
	if( ::GetAsyncKeyState('A') & 0x8000f )
		vPos -= vRight * fOffset;
	if( ::GetAsyncKeyState('D') & 0x8000f )
		vPos += vRight * fOffset;

	g_Camera.SetPosition( vPos );
}