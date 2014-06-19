// ModelPanel.cpp : implementation file
//

#include "stdafx.h"
#include "MapTool2.h"
#include "modelFileListBox.h"
#include "MapView.h"
#include "ModelPannel.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CModelPanel dialog


CModelPanel::CModelPanel(CWnd* pParent /*=NULL*/)
	: CPanelBase(MODEL, CModelPanel::IDD, pParent)
{
	m_pModelFileListBox = NULL;

}


CModelPanel::~CModelPanel()
{
	SAFE_DELETE(m_pModelFileListBox);

}


void CModelPanel::DoDataExchange(CDataExchange* pDX)
{
	CPanelBase::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CModelPanel)
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CModelPanel, CPanelBase)
	//{{AFX_MSG_MAP(CModelPanel)
	ON_BN_CLICKED(IDC_BUTTON_REFLESH, OnButtonReflesh)
	ON_MESSAGE(WM_NOTIFY_SELECTLIST, &CModelPanel::OnLbnSelchangeFilelist)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CModelPanel message handlers

BOOL CModelPanel::OnInitDialog() 
{
	CPanelBase::OnInitDialog();

	CRect modelRect = GetChildRect(IDC_MODELLIST_RECT);
	m_pModelFileListBox = new CModelFielListBox();
	m_pModelFileListBox->Create(CModelFielListBox::IDD, this);
	m_pModelFileListBox->MoveWindow(modelRect);
	m_pModelFileListBox->ShowWindow(SW_SHOW);

	return TRUE;
}


void CModelPanel::OnButtonReflesh() 
{

}


void CModelPanel::LoadTable()
{
/*
	list<string> extlist, filelist;
	extlist.push_back( "bmp" );
	extlist.push_back( "tga" );
	extlist.push_back( "jpg" );
	extlist.push_back( "tng" );

	CollectFile( &extlist, szDirectory, &filelist );

	while( LB_ERR != m_TileList.DeleteString(0) ) {} // List Clear

	list<string>::iterator it = filelist.begin();
	while( filelist.end() != it )
		m_TileList.AddString( it++->c_str() );
/**/

//	m_FileTree.OpenModelTable( (char*)g_szModelTable );
//	SD_CTableMgr::LoadModelTable( (char*)g_szModelTable );

}


//------------------------------------------------------------------------
// 파일리스트가 선택되면 호출된다.
// [2011/2/22 jjuiddong]
//------------------------------------------------------------------------
LRESULT CModelPanel::OnLbnSelchangeFilelist(WPARAM wParam, LPARAM lParam)
{
	Update();
	return 0;
}

//------------------------------------------------------------------------
// 
// [2011/2/22 jjuiddong]
//------------------------------------------------------------------------
CString CModelPanel::GetSelectModelName()
{
	return m_pModelFileListBox->GetSelectItemName();
}


//------------------------------------------------------------------------
// 
// [2011/3/1 jjuiddong]
//------------------------------------------------------------------------
void CModelPanel::OnShowWindow()
{

}


//------------------------------------------------------------------------
// 
// [2011/3/1 jjuiddong]
//------------------------------------------------------------------------
void CModelPanel::OnHideWindow()
{
	// 선택된 리스트를 초기화 한다.
	// 패널이 열릴 때 선택된 아이템으로 로드하기때문에
	// 패널이 닫힐 때 초기화를 해주어야 한다.
	CListBox *fileListBox = m_pModelFileListBox->GetFileListBox();
	fileListBox->SetCurSel(-1);

	UpdateData(FALSE);
}
