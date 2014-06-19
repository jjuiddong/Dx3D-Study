// MapTool2.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "MapTool2.h"
#include "MapTool2Dlg.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMapTool2App

BEGIN_MESSAGE_MAP(CMapTool2App, CWinApp)
	//{{AFX_MSG_MAP(CMapTool2App)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMapTool2App construction

CMapTool2App::CMapTool2App()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CMapTool2App object

CMapTool2App theApp;



/////////////////////////////////////////////////////////////////////////////
// CMapTool2App initialization

BOOL CMapTool2App::InitInstance()
{
	g_Dbg.Create( DBGLIB_CONSOL );

	CMapTool2Dlg *pDlg = new CMapTool2Dlg;
	pDlg->Create( IDD_MAPTOOL2_DIALOG );
	if( !pDlg->Init() )
	{
		delete pDlg;
		return FALSE;
	}

	pDlg->ShowWindow( SW_SHOW );
	pDlg->MainProc();

	pDlg->DestroyWindow();
	delete pDlg;
/**/
	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}
