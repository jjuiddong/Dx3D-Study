
// ModelViewerDlg.cpp : 구현 파일
//

#include "stdafx.h"

#include "ModelViewer.h"
#include "ModelView.h"
#include "BasePanelFrame.h"

#include "ModelViewerDlg.h"



CScene g_Scene( 0 );
CCamera g_Camera;
CFrustum g_Frustum;
float g_Distance = 100.f;
BOOL g_bLoop = TRUE;



#ifdef _DEBUG
#define new DEBUG_NEW
#endif


CModelViewerDlg::CModelViewerDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CModelViewerDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CModelViewerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CModelViewerDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDCANCEL, &CModelViewerDlg::OnBnClickedCancel)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BUTTON1, &CModelViewerDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CModelViewerDlg::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON3, &CModelViewerDlg::OnBnClickedButton3)
	ON_BN_CLICKED(IDC_BUTTON4, &CModelViewerDlg::OnBnClickedButton4)
END_MESSAGE_MAP()


BOOL CModelViewerDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	SetIcon(m_hIcon, TRUE);
	SetIcon(m_hIcon, FALSE);
	ShowWindow(SW_SHOWNORMAL);

	return TRUE;
}


void CModelViewerDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this);

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 아이콘을 그립니다.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

HCURSOR CModelViewerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

//-----------------------------------------------------------------------------//
// 출력
//-----------------------------------------------------------------------------//
bool Display( int timeDelta )
{
	g_Camera.Animate( timeDelta );

	Matrix44 viewproj;
	g_Camera.GetViewProjMatrix( &viewproj );
	g_Frustum.Make( &viewproj );

	g_pDevice->Clear( 0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(200,200,200), 1.0f, 0 );
	g_pDevice->BeginScene();

	//	g_Scene.Render();
	g_pModelView->Render();
	g_pModelView->Update( timeDelta );

	g_pDevice->EndScene();
	g_pDevice->Present( NULL, NULL, NULL, NULL );

	return true;
}


//-----------------------------------------------------------------------------//
// DX 초기화
//-----------------------------------------------------------------------------//
BOOL CModelViewerDlg::Init()
{
	const int WIDTH = 1034;
	const int HEIGHT = 798;
	MoveWindow( CRect(0,0,WIDTH, HEIGHT) );

	const int VIEW_W = 1024;
	const int VIEW_H = 768;
	g_pModelView = new CModelView();
	g_pModelView->Create( NULL, NULL, WS_VISIBLE, CRect(5,40,VIEW_W+5,VIEW_H+40), this, 0 );
	if( !d3d::InitDX3D(g_pModelView->m_hWnd, VIEW_W, VIEW_H, true, D3DDEVTYPE_HAL, &g_pDevice) )
	{
		::MessageBox(0, "InitD3D() - FAILED", 0, 0);
		return FALSE;
	}

	g_pModelView->Init();

	SetRect( &g_ViewPort, 0, 0, VIEW_W, VIEW_H );
	g_Camera.SetViewPort( VIEW_W, VIEW_H );

	if( !Setup() )
	{
		::MessageBox(0, "Setup() - FAILED", 0, 0);
		return FALSE;
	}

	// Panel 생성
	g_pBasePanelFrm = new CBasePanelFrame;
	CString StrClassName = AfxRegisterWndClass( CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS,
		LoadCursor(NULL, IDC_ARROW), (HBRUSH)GetStockObject(COLOR_BTNFACE+1), LoadIcon(NULL, IDI_APPLICATION) );
	g_pBasePanelFrm->CreateEx(0, StrClassName, "패널", 
		WS_POPUP | WS_CAPTION | WS_SYSMENU | MFS_THICKFRAME, CRect(WIDTH+10, 0, WIDTH+240, 800), this );
	g_pBasePanelFrm->ShowWindow(SW_SHOW);

	// Add Listner
	g_pBasePanelFrm->AddPanelListner(g_pModelView);

	return TRUE;
}


//------------------------------------------------------------------------
// 
// [2011/2/21 jjuiddong]
//------------------------------------------------------------------------
void CModelViewerDlg::MainProc()
{
	int nOldTime = timeGetTime();

	while( m_bLoop )
	{
		MSG msg;
		if( PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE) )
		{
			if (!GetMessage(&msg, NULL, 0, 0)) break;
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
		}

		int nCurTime = timeGetTime();
		//		if( (nCurTime - nOldTime) < m_nDeltaTime )
		//			continue;
		//			Sleep( m_nDeltaTime - (nCurTime-nOldTime) );
		//		nCurTime = timeGetTime();
		int nDeltaTime = nCurTime - nOldTime;

		Display( nDeltaTime );

		Sleep(0);
		nOldTime = nCurTime;
	}
}


//-----------------------------------------------------------------------------//
// 프로그램 초기화
//-----------------------------------------------------------------------------//
BOOL CModelViewerDlg::Setup()
{
	g_pDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_CCW );// 기본컬링, CCW
    g_pDevice->SetRenderState( D3DRS_ZENABLE, TRUE );// Z버퍼기능을 켠다.
	g_pDevice->SetRenderState( D3DRS_LIGHTING, TRUE );

	// 월드 행렬 설정
	D3DXMatrixIdentity( &g_matIdentity );

	g_Camera.SetEye( g_DefaultCameraPos );
	g_Camera.SetCamValue( 0.f, 0.f, 0.f, g_Distance );
	g_Camera.RotateY( 8 );
	g_Camera.Execute();

	/////////////////////////////////////////
	CFileLoader::Init( g_pDevice );

	return TRUE;
}


void CModelViewerDlg::OnBnClickedCancel()
{
	m_bLoop = FALSE;
	OnCancel();
}

void CModelViewerDlg::OnDestroy()
{
	CDialog::OnDestroy();
	CFileLoader::Release();
}

void CModelViewerDlg::OnBnClickedButton1()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}

void CModelViewerDlg::OnBnClickedButton2()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}

void CModelViewerDlg::OnBnClickedButton3()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}

void CModelViewerDlg::OnBnClickedButton4()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}
