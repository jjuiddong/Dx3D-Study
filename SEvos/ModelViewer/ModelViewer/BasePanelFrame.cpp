// BasePanelFrame.cpp : implementation file
//

#include "stdafx.h"
#include "ModelView.h"
#include "ModelViewer.h"

#include "tabbase.h"
#include "ViewPanel.h"
#include "EditPanel.h"

#include "BasePanelFrame.h"



#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CBasePanelFrame

//IMPLEMENT_DYNCREATE(CBasePanelFrame, CMiniFrameWnd)

CBasePanelFrame::CBasePanelFrame():m_pTab(NULL)
{
}

CBasePanelFrame::~CBasePanelFrame()
{
	SAFE_DELETE( m_pTab );

}


BEGIN_MESSAGE_MAP(CBasePanelFrame, CMiniFrameWnd)
	//{{AFX_MSG_MAP(CBasePanelFrame)
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBasePanelFrame message handlers

int CBasePanelFrame::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CMiniFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	m_pTab = new CTabBase;
	m_pTab->Create( WS_CHILD | WS_VISIBLE | WS_TABSTOP | TCS_TABS, 
		CRect(0,0,lpCreateStruct->cx,lpCreateStruct->cy), this, 0 );

	CRect rect( 0, 0, 210, 700 );
	CViewPanel *pViewPanel = new CViewPanel;
	m_pTab->CreateView( pViewPanel, CViewPanel::IDD );
	pViewPanel->MoveWindow( rect );

	CEditPanel *pEditPanel = new CEditPanel;
	m_pTab->CreateView( pEditPanel, CEditPanel::IDD );
	pEditPanel->MoveWindow( rect );

	m_pTab->AddView( &VIEWLIST(0,0,"ºä", pViewPanel, NULL) );
	m_pTab->AddView( &VIEWLIST(1,1,"¸ðµ¨", pEditPanel, NULL) );

	m_pTab->ShowView(0);

	return 0;
}


//------------------------------------------------------------------------
// 
// [2011/2/22 jjuiddong]
//------------------------------------------------------------------------
void CBasePanelFrame::AddPanelListner(IPanelListner *plistner)
{
	LPVIEWLIST pNode = m_pTab->GetRootView();
	while (pNode)
	{
		CPanelBase *panel = (CPanelBase*)pNode->pWnd;
		panel->Add(plistner);
		pNode = pNode->pNext;
	}
}
