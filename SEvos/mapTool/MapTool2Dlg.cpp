// MapTool2Dlg.cpp : implementation file
//

#include "stdafx.h"
#include "MapTool2.h"
#include "MapTool2Dlg.h"
#include "terrainview.h"
#include "mapview.h"

#include "global.h"
#include "game/scene.h"
#include "control.h"

#include "terrainpannel.h"
#include "modelpannel.h"

#include "basepanelframe.h"
#include "terrain/terraineditor.h"

#include "CrTerrainDlg.h"


IDirect3DDevice9 *g_pDevice;
CScene g_Scene( 0 );
CCamera g_Camera;
CFrustum g_Frustum;
float g_Distance = 100.f;
BOOL g_bLoop = TRUE;


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMapTool2Dlg dialog

CMapTool2Dlg::CMapTool2Dlg(CWnd* pParent /*=NULL*/)
	: CDialog(CMapTool2Dlg::IDD, pParent)
	, m_ChkDispQuad(FALSE)
	, m_ChkNormal(FALSE)
	, m_ChkShadow(FALSE)
{
	//{{AFX_DATA_INIT(CMapTool2Dlg)
	m_ChkChunkEdge = TRUE;
	m_ChkShadow = TRUE;
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_bLoop = TRUE;
	m_nDeltaTime = (int)(1000.f / 30.f);

}

void CMapTool2Dlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMapTool2Dlg)
	DDX_Check(pDX, IDC_CHECK_FOG, m_ChkFog);
	DDX_Check(pDX, IDC_CHECK_TEXTURE, m_ChkTexture);
	DDX_Check(pDX, IDC_CHECK_VERTEX, m_ChkVertex);
	DDX_Check(pDX, IDC_CHECK_CHUNKEDGE, m_ChkChunkEdge);
	//}}AFX_DATA_MAP
	DDX_Check(pDX, IDC_CHECK_DISPQUADTREE, m_ChkDispQuad);
	DDX_Check(pDX, IDC_CHECK_NORMAL, m_ChkNormal);
	DDX_Check(pDX, IDC_CHECK_SHADOW, m_ChkShadow);
}

BEGIN_MESSAGE_MAP(CMapTool2Dlg, CDialog)
	//{{AFX_MSG_MAP(CMapTool2Dlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_OPEN, OnButtonOpen)
	ON_BN_CLICKED(IDC_BUTTON_SAVE, OnButtonSave)
	ON_BN_CLICKED(IDC_BUTTON_CREATE, OnButtonCreate)
	ON_BN_CLICKED(IDC_BUTTON_OPTION, OnButtonOption)
	ON_BN_CLICKED(IDC_CHECK_FOG, OnCheckFog)
	ON_BN_CLICKED(IDC_CHECK_TEXTURE, OnCheckTexture)
	ON_BN_CLICKED(IDC_CHECK_VERTEX, OnCheckVertex)
	ON_BN_CLICKED(IDC_CHECK_CHUNKEDGE, OnCheckChunkedge)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_CHECK_DISPQUADTREE, &CMapTool2Dlg::OnBnClickedCheckDispquadtree)
	ON_BN_CLICKED(IDC_CHECK_NORMAL, &CMapTool2Dlg::OnBnClickedCheckNormal)
	ON_BN_CLICKED(IDC_CHECK_SHADOW, &CMapTool2Dlg::OnBnClickedCheckShadow)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMapTool2Dlg message handlers

//-----------------------------------------------------------------------------//
// 출력
//-----------------------------------------------------------------------------//
bool Display( int timeDelta )
{
	if( g_bLoop )
	{
//		g_Scr.Run( timeDelta );
//		g_Scene.Animate( timeDelta );
	}

	if (timeDelta > 1000)
		return true;

	g_Camera.Execute();
	g_Camera.Animate( timeDelta );

	Matrix44 viewproj;
	g_Camera.GetViewProjMatrix( &viewproj );
	g_Frustum.Make( &viewproj );

	g_pDevice->Clear( 0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(200,200,200), 1.0f, 0 );
	g_pDevice->BeginScene();

	g_pTerrainView->Update( timeDelta );
	g_pTerrainView->Render();

	g_pDevice->EndScene();
	g_pDevice->Present( NULL, NULL, NULL, NULL );

	return true;
}


BOOL CMapTool2Dlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	GetCurrentDirectory(sizeof(m_CurrentDirectory), m_CurrentDirectory);

	return TRUE;  // return TRUE  unless you set the focus to a control
}


//-----------------------------------------------------------------------------//
// DX 초기화
//-----------------------------------------------------------------------------//
BOOL CMapTool2Dlg::Init()
{
//	CRect wr;
//	GetWindowRect( wr );
//	MoveWindow( CRect(0,0,wr.Width(), wr.Height()) );
	const int WIDTH = 1034;
	const int HEIGHT = 798;
	MoveWindow( CRect(0,0,WIDTH, HEIGHT) );

	const int VIEW_W = 1024;
	const int VIEW_H = 768;
	g_pTerrainView = new CMapView;
	g_pTerrainView->Create( NULL, NULL, WS_VISIBLE, CRect(5,40,VIEW_W+5,VIEW_H+40), this, 0 );
	if( !d3d::InitDX3D(g_pTerrainView->m_hWnd, VIEW_W, VIEW_H, true, D3DDEVTYPE_HAL, &g_pDevice) )
	{
		::MessageBox(0, "InitD3D() - FAILED", 0, 0);
		return FALSE;
	}

	g_pTerrainView->Init();

	SetRect( &g_ViewPort, 0, 0, VIEW_W, VIEW_H );
	g_Camera.SetViewPort( VIEW_W, VIEW_H );

	if( !Setup() )
	{
		::MessageBox(0, "Setup() - FAILED", 0, 0);
		return FALSE;
	}

	// Init Map
//	SCrTerrain crt;
//	crt.nWidth = 640;
//	crt.nHeight = 640;
//	crt.nVtxPerCol = 65;
//	crt.nVtxPerRow = 65;
//	g_pTerrainView->CreateTerrain( &crt );

	// Panel 생성
	g_pBasePanelFrm = new CBasePanelFrame;
    CString StrClassName = AfxRegisterWndClass( CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS,
		LoadCursor(NULL, IDC_ARROW), (HBRUSH)GetStockObject(COLOR_BTNFACE+1), LoadIcon(NULL, IDI_APPLICATION) );
	g_pBasePanelFrm->CreateEx(0, StrClassName, "패널", 
		WS_POPUP | WS_CAPTION | WS_SYSMENU | MFS_THICKFRAME, CRect(WIDTH+10, 0, WIDTH+240, 800), this );
	g_pBasePanelFrm->ShowWindow(SW_SHOW);

	// Add Listner
	g_pBasePanelFrm->AddPanelListner(g_pTerrainView);

	CTerrainEditor *pmap = g_pTerrainView->GetTerrain();
	pmap->SetDisplayShadow(m_ChkShadow);

	return TRUE;
}


//-----------------------------------------------------------------------------//
// 프로그램 초기화
//-----------------------------------------------------------------------------//
BOOL CMapTool2Dlg::Setup()
{
	g_pDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_CCW );// 기본컬링, CCW
    g_pDevice->SetRenderState( D3DRS_ZENABLE, TRUE );// Z버퍼기능을 켠다.
	g_pDevice->SetRenderState( D3DRS_LIGHTING, TRUE );
//	g_pDevice->SetRenderState( D3DRS_FILLMODE, D3DFILL_WIREFRAME );

/*
    ZeroMemory( &g_Light, sizeof(g_Light) );
    g_Light.Type = D3DLIGHT_DIRECTIONAL;
    g_Light.Direction = D3DXVECTOR3( 0.0f, -1.f, 0.f );
    g_Light.Diffuse.r = 1.f;
	g_Light.Diffuse.g = g_Light.Diffuse.b = 1.f;
	g_Light.Range = 100.0f;
	g_Light.Attenuation1 = 0.4f;
	g_Light.Position = D3DXVECTOR3( 0, 100, 0 );
/**/

	// 월드 행렬 설정
	D3DXMatrixIdentity( &g_matIdentity );

	g_Camera.SetEye( g_DefaultCameraPos );
	g_Camera.SetCamValue( 0.f, 0.f, 0.f, g_Distance );
	g_Camera.RotateY( 8 );
	g_Camera.Execute();

	/////////////////////////////////////////
	CFileLoader::Init( g_pDevice );

//	g_Terrain.LoadTerrain( "SaveMap\\ax.map" );
//	g_Terrain.SetTerrain();

	g_Scene.Create( "data//ui//scene.txt" );
	g_Scene.SwapScene( 0, 4 );

//	g_Control.LoadTerrain( "SaveMap\\ax.map" );

//	g_Control.AddCharacter( CHAR_HERO, "hero", "model//bmm.txt", 
//		"model//character_1.txt", "model//weapon.txt", "data//script//combo.txt" );

	return TRUE;
}


//-----------------------------------------------------------------------------//
// 
//-----------------------------------------------------------------------------//
void CMapTool2Dlg::MainProc()
{
	int nOldTime = timeGetTime();

	while( m_bLoop )
	{
		MSG msg;
		if( PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE) )
		{
			if( !GetMessage(&msg, NULL, 0, 0) ) 
			{
				break;
			}
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
		}

		const int nCurTime = timeGetTime();
//		if( (nCurTime - nOldTime) < m_nDeltaTime )
//			continue;
//			Sleep( m_nDeltaTime - (nCurTime-nOldTime) );
//		nCurTime = timeGetTime();
		const int nDeltaTime = nCurTime - nOldTime;

		Display( nDeltaTime );

		Sleep(0);
		nOldTime = nCurTime;
	}
}


void CMapTool2Dlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CMapTool2Dlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CMapTool2Dlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CMapTool2Dlg::OnCancel() 
{
	m_bLoop = FALSE;	
	CDialog::OnCancel();
}

BOOL CMapTool2Dlg::DestroyWindow() 
{
	CFileLoader::Release();
	g_Scene.Destroy();
	g_pDevice->Release();
	
	return CDialog::DestroyWindow();
}

void CMapTool2Dlg::OnButtonOpen() 
{
	SetCurrentDirectory(m_CurrentDirectory);

	CFileDialog Dlg(TRUE, "map", "MapFile", OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, 
		"map File (*.map)|*.map|All Files (*.*)|*.*||");
	if( IDOK == Dlg.DoModal() )
	{
		g_pTerrainView->FileOpen((char*)(LPCTSTR)Dlg.GetPathName());

		if( !_stricmp( Dlg.GetFileExt(), "map") )
		{
//			::SetCurrentDirectory( g_szCurDir );
/*
			if( !g_pMapView->FileOpen((char*)(LPCTSTR)Dlg.GetPathName()) )
			{
				::AfxMessageBox( "저장 실패" );
			}
			else
			{
				::AfxMessageBox( "Success" );
			}
/**/			
		}
	}	
}

void CMapTool2Dlg::OnButtonSave() 
{
/*
	SetCurrentDirectory(m_CurrentDirectory);

	CFileDialog Dlg(FALSE, "map", "MapFile", OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, 
		"map File (*.map)|*.map|All Files (*.*)|*.*||");
	if( IDOK == Dlg.DoModal() )
	{
		if( !_stricmp( Dlg.GetFileExt(), "map") )
		{
			g_pTerrainView->FileSave((char*)(LPCTSTR)Dlg.GetFileName());
		}
	}	
/**/

	if (g_pTerrainView->FileSave(" "))
	{
		::AfxMessageBox("파일 저장 성공");
	}
	else
	{
		::AfxMessageBox("저장 오류 발생");
	}

}

void CMapTool2Dlg::OnButtonCreate() 
{
	CCrTerrainDlg dlg;
	if (IDOK == dlg.DoModal())
	{
		g_pTerrainView->CreateTerrain(dlg.GetCreateData());
	}	
}

void CMapTool2Dlg::OnButtonOption() 
{
	// TODO: Add your control notification handler code here
	
}

void CMapTool2Dlg::OnCheckFog() 
{
	// TODO: Add your control notification handler code here
	
}

void CMapTool2Dlg::OnCheckTexture() 
{
	UpdateData();
	m_ChkVertex = !m_ChkTexture;
	UpdateData( FALSE );

	int nValue = 0;
	if( m_ChkTexture )	nValue += 1;
	if( m_ChkVertex )	nValue += 2;

	g_pTerrainView->SetOption( 1, nValue );
	g_pTerrainView->SetFocus();	
}

void CMapTool2Dlg::OnCheckVertex() 
{
	UpdateData();
	m_ChkTexture = !m_ChkVertex;
	UpdateData( FALSE );

	int nValue = 0;
	if( m_ChkTexture )	nValue += 1;
	if( m_ChkVertex )	nValue += 2;

	g_pTerrainView->SetOption( 1, nValue );
	g_pTerrainView->SetFocus();
}

void CMapTool2Dlg::OnCheckChunkedge() 
{
	UpdateData();
	CTerrainEditor *pmap = g_pTerrainView->GetTerrain();
	pmap->SetDrawChunkEdgeOption( m_ChkChunkEdge );	
}

void CMapTool2Dlg::OnBnClickedCheckDispquadtree()
{
	UpdateData();
	CTerrainEditor *pmap = g_pTerrainView->GetTerrain();
	pmap->SetDisplayQuadTree(m_ChkDispQuad);
}

void CMapTool2Dlg::OnBnClickedCheckNormal()
{
	UpdateData();
	CTerrainEditor *pmap = g_pTerrainView->GetTerrain();
	pmap->SetDisplayNormal(m_ChkNormal);
}

void CMapTool2Dlg::OnBnClickedCheckShadow()
{
	UpdateData();
	CTerrainEditor *pmap = g_pTerrainView->GetTerrain();
	pmap->SetDisplayShadow(m_ChkShadow);
}
