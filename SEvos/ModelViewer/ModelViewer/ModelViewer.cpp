
// ModelViewer.cpp : 응용 프로그램에 대한 클래스 동작을 정의합니다.
//

#include "stdafx.h"
#include "ModelViewer.h"
#include "ModelViewerDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CModelViewerApp

BEGIN_MESSAGE_MAP(CModelViewerApp, CWinAppEx)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CModelViewerApp 생성

CModelViewerApp::CModelViewerApp()
{
	// TODO: 여기에 생성 코드를 추가합니다.
	// InitInstance에 모든 중요한 초기화 작업을 배치합니다.
}


// 유일한 CModelViewerApp 개체입니다.

CModelViewerApp theApp;


// CModelViewerApp 초기화

BOOL CModelViewerApp::InitInstance()
{
//	CWinAppEx::InitInstance();

	CModelViewerDlg *pDlg = new CModelViewerDlg();
	pDlg->Create( IDD_MODELVIEWER_DIALOG );
	if( !pDlg->Init() )
	{
		delete pDlg;
		return FALSE;
	}

	pDlg->ShowWindow( SW_SHOW );
	pDlg->MainProc();

	pDlg->DestroyWindow();
	g_pDevice->Release();
	delete pDlg;

	return FALSE;
}
