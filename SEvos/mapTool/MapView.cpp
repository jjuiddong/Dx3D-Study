// MapView.cpp : implementation file
//

#include "stdafx.h"
#include "MapTool2.h"
#include "terrainpannel.h"
#include "terrain/terraineditor.h"
#include "tilepanel.h"
#include "terrain/terrainCursor.h"
#include "common//light.h"
#include "common/utility.h"
#include "ModelPannel.h"

#include "common/mouseEventListner.h"
#include "modelEditMouseListner.h"
#include "terrainEditMouseListner.h"
#include "tileEditMouseListner.h"

#include "MapView.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



/////////////////////////////////////////////////////////////////////////////
// CMapView
CMapView::CMapView():
	m_pCursor(NULL), 
	//	m_pCamera(NULL), 
//	m_vLight(0,0,1), 
	m_bAlt( FALSE ),
	m_pDxFont(NULL), m_pTextSprite(NULL),
	m_EditMode( EM_NORMAL )
{
	m_fRMouseZ = 20.f;
	m_ElapseTime = 0;
	m_Fps = 0;
	sprintf_s( m_szFps, sizeof(m_szFps), "fps: 0" );

	m_pTerrain = new CTerrainEditor();
	m_pCursor = new CTerrainCursor(m_pTerrain);

}

CMapView::~CMapView()
{
	SAFE_DELETE( m_pTerrain );
	SAFE_DELETE( m_pCursor );

	SAFE_RELEASE( m_pTextSprite );
	SAFE_RELEASE( m_pDxFont );

	for (int i=0; i < EM_MAX_SIZE; ++i)
		delete m_pMouseEventListner[i];


}


BEGIN_MESSAGE_MAP(CMapView, CView)
	//{{AFX_MSG_MAP(CMapView)
	ON_WM_ERASEBKGND()
	ON_WM_MOUSEWHEEL()
	ON_WM_LBUTTONDOWN()
	ON_WM_RBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_WM_RBUTTONUP()
	ON_WM_KEYDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMapView drawing

void CMapView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: add draw code here
}

BOOL CMapView::OnEraseBkgnd(CDC* pDC) 
{
	return TRUE;
}

BOOL CMapView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt) 
{
	if( 0 == nFlags )
		m_Mouse.SetMouseWheel( -zDelta );
	return CView::OnMouseWheel(nFlags, zDelta, pt);
}

void CMapView::OnLButtonDown(UINT nFlags, CPoint point) 
{
	if (!m_pCursor) return;

	SetFocus();
	SetCapture();
	m_pCursor->UpdateCursor(point);

	switch( m_EditMode )
	{
	case EM_NORMAL:
		{
			m_ptClickPos = point;
			CChunk *pchunk = m_pTerrain->SetChunkFocusing(m_pCursor);
			g_pTilePanel->SetCurrentChunk(pchunk);
		}
		break;
	}

	m_pMouseEventListner[ m_EditMode]->OnLButtonDown(nFlags, point);

	CView::OnLButtonDown(nFlags, point);
}


void CMapView::OnRButtonDown(UINT nFlags, CPoint point) 
{
	if (!m_pCursor) return;
	SetFocus();	
	m_pMouseEventListner[ m_EditMode]->OnRButtonDown(nFlags, point);

	CView::OnRButtonDown(nFlags, point);
}

void CMapView::OnRButtonUp(UINT nFlags, CPoint point) 
{
	if (!m_pCursor) return;
	ReleaseCapture();
	m_pMouseEventListner[ m_EditMode]->OnRButtonUp(nFlags, point);

	CView::OnRButtonUp(nFlags, point);
}


//------------------------------------------------------------------------
// 
// [2011/2/24 jjuiddong]
//------------------------------------------------------------------------
void CMapView::OnMouseMove(UINT nFlags, CPoint point) 
{
	if (!m_pCursor) return;
	SetFocus();
	m_pCursor->UpdateCursor(point);
	m_pMouseEventListner[ m_EditMode]->OnMouseMove(nFlags, point);

	CView::OnMouseMove(nFlags, point);
}

void CMapView::OnLButtonUp(UINT nFlags, CPoint point) 
{
	ReleaseCapture();
	m_pMouseEventListner[ m_EditMode]->OnLButtonUp(nFlags, point);

	CView::OnLButtonUp(nFlags, point);
}

/////////////////////////////////////////////////////////////////////////////
// CMapView diagnostics

#ifdef _DEBUG
void CMapView::AssertValid() const
{
	CView::AssertValid();
}

void CMapView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMapView message handlers


//-----------------------------------------------------------------------------//
// 
//-----------------------------------------------------------------------------//
BOOL CMapView::Init()
{
//	m_pTerrain = new CMapEditor;

//	RECT r;
	m_Mouse.Init( m_hWnd );
//	_SD_CScreen::GetViewPort( &r );
	CRect r;
	GetClientRect( r );
	m_Mouse.SetViewport( 0, 0, r.Width(), r.Height() );

	m_Mouse.Update();
	m_nRMouseX = m_Mouse.GetMouseX();
	m_nRMouseY = m_Mouse.GetMouseY();

//	m_Box.Box( 10, 10, 100, 100, 0xff000000 );

	// create sprite
	HRESULT hr;
    if( FAILED(hr = D3DXCreateSprite(g_pDevice, &m_pTextSprite)) )
        return FALSE;
	// create text
	hr = D3DXCreateFont( g_pDevice, 18, 0, FW_BOLD, 1, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, 
						DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "굴림", &m_pDxFont );
	if( FAILED(hr) )
		return FALSE;

	m_pCursor->Init();

	m_pMouseEventListner[ EM_NORMAL] = new CMouseEventListner(this);
	m_pMouseEventListner[ EM_TERRAIN] = new CTerrainEditMouseListner(this);
	m_pMouseEventListner[ EM_TILE] = new CTileEditMouseListner(this);
	m_pMouseEventListner[ EM_MODEL] = new CModelEditMouseListner(this);

	return TRUE;
}


//-----------------------------------------------------------------------------//
// 
//-----------------------------------------------------------------------------//
BOOL CMapView::FileOpen( char *szFileName )
{
	if (!m_pTerrain)
	{
		m_pTerrain = new CTerrainEditor;
		m_pTerrain->SetFog( 0x0000cccc, 10.0f, 200.f );
	}	
	m_pTerrain->Load(szFileName);

	// file path 저장
	GetFilePath(szFileName, m_CreateFilePath);

	return TRUE;
}
	

//-----------------------------------------------------------------------------//
// 
//-----------------------------------------------------------------------------//
BOOL CMapView::FileSave( char *szFileName )
{
	if (!m_pTerrain)
		return FALSE;

	if (!m_pTerrain->IsLoaded())
		return FALSE;

	char fullFileName[ MAX_PATH];
	sprintf_s(fullFileName, sizeof(fullFileName), "%s/mapFile.map", m_CreateFilePath );

	return m_pTerrain->Save( fullFileName );
}


//-----------------------------------------------------------------------------//
// 
//-----------------------------------------------------------------------------//
BOOL CMapView::CreateTerrain( SCrTerrain *pCrTerrain )
{
	SAFE_DELETE( m_pTerrain );

	m_pTerrain = new CTerrainEditor;
	const float cellSize = (float)pCrTerrain->nWidth / (float)(pCrTerrain->nVtxPerRow-1);
	m_pTerrain->Create(pCrTerrain->nVtxPerRow, pCrTerrain->nVtxPerCol, cellSize, 1.f, pCrTerrain->szTexture);
	m_pTerrain->SetFog( 0x0000cccc, 10.0f, 200.f );
	m_pCursor->SetTerrain(m_pTerrain);

	// file path 저장
	strcpy_s(m_CreateFilePath, sizeof(m_CreateFilePath), pCrTerrain->szFilePath);

	// 디렉토리 생성
	BOOL result = CreateDirectory(pCrTerrain->szFilePath, NULL);
//	if (!PathFileExists(szChunkAlphaFilePath))

	return TRUE;
}


//-----------------------------------------------------------------------------//
// 
//-----------------------------------------------------------------------------//
void CMapView::Render()
{
	if (m_pTerrain)
		m_pTerrain->Render();

	switch( m_EditMode )
	{
	case EM_TERRAIN:
	case EM_TILE:
		if (m_pCursor)
			m_pCursor->Render();
		break;
	}
	
	RenderFps();
	d3d::DrawXYZ();

}


//-----------------------------------------------------------------------------//
// 
//-----------------------------------------------------------------------------//
void CMapView::Update( int nElapsTick )
{
	CalcFPS(nElapsTick);
	MouseProc();
	KeyProc( nElapsTick );

	if (m_pTerrain)
		m_pTerrain->Update( nElapsTick );
	if (m_pCursor)
		m_pCursor->Update(nElapsTick);

	m_pMouseEventListner[ m_EditMode]->Update(nElapsTick);
}


//-----------------------------------------------------------------------------//
// HoverObject 설정됨
//-----------------------------------------------------------------------------//
void CMapView::SelectObj( OBJ_TYPE eOType, char *szObjName )
{
/*
	if( m_bHover )
	{
		if( m_strObjName == szObjName )
			return;
	}

	m_bHover = TRUE;
	m_strObjName = szObjName;
//	m_pTerrain->HoverCancel();
//	m_pTerrain->SetHover( eOType, szObjName );
/**/
}


//-----------------------------------------------------------------------------//
// 
//-----------------------------------------------------------------------------//
void CMapView::MouseProc()
{
	if (!m_pTerrain) return;
	if (!m_pCursor) return;

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
		CLight *pLight = m_pTerrain->GetLight();
//		Vector3 dir = pLight->GetDirection();
//		dir.RotateZ( mx * 0.1f );
//		dir.RotateX( my * 0.1f );
//		pLight->SetDirection(dir);

		Vector3 pos = m_pCursor->GetCursorPos();
		pos.Normalize();
		pos *= 500;
		pos.y = 500;
		pLight->SetPosition(pos);
	}

	if (::GetAsyncKeyState(VK_SHIFT) & 0x8000f)
	{
		CLight *pLight = m_pTerrain->GetLight();
		const Vector3 pos = m_pCursor->GetCursorPos();
		const Vector3 lightPos = pLight->GetPosition();
//		pLight->SetPosition(Vector3(pos.x, lightPos.y, pos.z));
//		pLight->SetRange(1000.f);
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
void CMapView::KeyProc( int nElapsTick )
{
	BOOL keyMap[ 256];
	ZeroMemory(keyMap, sizeof(keyMap));

	if( ::GetAsyncKeyState(VK_SPACE) & 0x8000f )
		keyMap[ VK_SPACE] = TRUE;
	if( ::GetAsyncKeyState('W') & 0x8000f )
		keyMap[ 'W'] = TRUE;
	if( ::GetAsyncKeyState('S') & 0x8000f )
		keyMap[ 'S'] = TRUE;
	if( ::GetAsyncKeyState('A') & 0x8000f )
		keyMap[ 'A'] = TRUE;
	if( ::GetAsyncKeyState('D') & 0x8000f )
		keyMap[ 'D'] = TRUE;
	if( ::GetAsyncKeyState(VK_MENU) & 0x8000f ) m_bAlt = TRUE;
	else										m_bAlt = FALSE;

	BOOL isKeyDown = FALSE;
	if (keyMap[ VK_SPACE] || keyMap[ 'W'] || keyMap[ 'S'] || keyMap[ 'A'] || keyMap[ 'D'])
	{
		isKeyDown = TRUE;
	}

	if (!isKeyDown)
	{
		// y값만 변경하고 리턴한다.
		Vector3 vCamPos = g_Camera.GetPosition();
		vCamPos.y = m_fRMouseZ;
		g_Camera.SetPosition( vCamPos );
		return;	
	}

	Vector3 vPos = g_Camera.GetPosition();
	Vector3	vDir = g_Camera.GetFront();
	Vector3	vRight = g_Camera.GetRight();

	vDir.y = 0.f;
	vDir.Normalize();
	vRight.y = 0.f;
	vRight.Normalize();

	float fOffset = 10.f * (float)nElapsTick;
	if (keyMap[ VK_SPACE])
		fOffset *= 0.1f;
	if (keyMap[ 'W'])
		vPos += vDir * fOffset;
	if (keyMap[ 'S'])
		vPos -= vDir * fOffset;
	if (keyMap[ 'A'])
		vPos -= vRight * fOffset;
	if (keyMap[ 'D'])
		vPos += vRight * fOffset;

	g_Camera.SetPosition( vPos );
}


//-----------------------------------------------------------------------------//
// option id 1 = 출력방식 
//			value 1 = texture 출력(default)
//				  2 = vertex 출력
//				  3 = texture + vertex 출력
//-----------------------------------------------------------------------------//
BOOL CMapView::SetOption( int nOptId, int nValue )
{
	if( 1 == nOptId )
	{
		// 일단 임시 코드
		int nRT = 0;
//		if( 1 & nValue )	nRT |= D3DPT_LINELIST;
//		if( 2 & nValue )	nRT |= D3DPT_TRIANGLELIST;

		if( 1 == nValue )
			m_pTerrain->SetRenderType( D3DPT_TRIANGLELIST );
		if( 2 == nValue )
			m_pTerrain->SetRenderType( D3DPT_LINELIST );
	}
	else if( 2 == nOptId )
	{
//		m_pTerrain->FogEnable( nValue );
	}

	return TRUE;
}


//-----------------------------------------------------------------------------//
// 
//-----------------------------------------------------------------------------//
void CMapView::TimerEvent( int nTimerId )
{

}


//-----------------------------------------------------------------------------//
// 에디트모드 변경
//-----------------------------------------------------------------------------//
void CMapView::ChangedEditMode( EDIT_MODE eMode )
{
	m_EditMode = eMode;

	switch( eMode )
	{
	case EM_NORMAL:
//		m_pTerrain->HoverCancel();
		break;

	case EM_TILE:
		break;

	case EM_MODEL:
		break;

	case EM_TERRAIN:
		break;
	}
}


//-----------------------------------------------------------------------------//
// KeyProc
//-----------------------------------------------------------------------------//
void CMapView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	if( VK_ESCAPE == nChar )
	{
		m_pTerrain->SelectModelCancel();
	}

	CView::OnKeyDown(nChar, nRepCnt, nFlags);
}


//-----------------------------------------------------------------------------//
// fps 출력
//-----------------------------------------------------------------------------//
void CMapView::RenderFps()
{
	m_pTextSprite->Begin( D3DXSPRITE_ALPHABLEND | D3DXSPRITE_SORT_TEXTURE );
	CRect rc(10,10,200,200);
    m_pDxFont->DrawText( m_pTextSprite, m_szFps, -1, &rc,
                        DT_NOCLIP, D3DXCOLOR( 1.0f, 1.0f, 1.0f, 1.0f ) );
	m_pTextSprite->End();
}


//------------------------------------------------------------------------
// 
// [2011/2/24 jjuiddong]
//------------------------------------------------------------------------
void CMapView::Update(IPanelObserver *pObserver)
{
	if (!m_pTerrain) return;

	PANEL_TYPE type = pObserver->GetPanelType();
	switch (type)
	{
	case TILE:
		{
			CTilePanel *pPanel = (CTilePanel*)pObserver;
			m_EditMode = pPanel->IsEditTileMode()? EM_TILE : EM_NORMAL;
		}
		break;

	case TERRAIN:
		{
			CTerrainPanel *pPanel = (CTerrainPanel*)pObserver;
			m_EditMode = pPanel->IsEditTerrainMode()? EM_TERRAIN : EM_NORMAL;
		}
		break;

	case MODEL:
		{
			m_EditMode = EM_MODEL;
			CModelPanel *pPanel = (CModelPanel*)pObserver;
			m_pTerrain->SetSelectModel(OT_MODEL, (char*)(LPCTSTR)pPanel->GetSelectModelName());
		}
		break;
	}

}


//------------------------------------------------------------------------
// 
// [2011/2/24 jjuiddong]
//------------------------------------------------------------------------
void CMapView::CalcFPS( int nElapsTick )
{
	++m_Fps;
	m_ElapseTime += nElapsTick;
	if( 1000 <= m_ElapseTime )
	{
		sprintf_s( m_szFps, sizeof(m_szFps), "fps: %d", m_Fps );
		m_Fps = 0;
		m_ElapseTime = 0;
	}
}

