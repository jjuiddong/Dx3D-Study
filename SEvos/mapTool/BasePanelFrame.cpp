// BasePanelFrame.cpp : implementation file
//

#include "stdafx.h"
#include "maptool2.h"
#include "BasePanelFrame.h"

#include "tabbase.h"
#include "terrainpannel.h"
#include "tilepanel.h"
#include "modelpannel.h"


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
	CTerrainPanel *pTerrainPanel = new CTerrainPanel;
	m_pTab->CreateView( pTerrainPanel, IDD_DLG_TERRAIN );
	pTerrainPanel->MoveWindow( rect );

	CTilePanel *pTilePanel = new CTilePanel;
	m_pTab->CreateView( pTilePanel, IDD_DLG_TILE );
	pTilePanel->MoveWindow( rect );

	CModelPanel *pModelPanel = new CModelPanel;
	m_pTab->CreateView( pModelPanel, IDD_DLG_MODEL );
	pModelPanel->MoveWindow( rect );

	m_pTab->AddView( &VIEWLIST(0,0,"타일",pTilePanel,NULL) );
	m_pTab->AddView( &VIEWLIST(1,1,"지형",pTerrainPanel,NULL) );
	m_pTab->AddView( &VIEWLIST(2,2,"Model",pModelPanel,NULL) );

	m_pTab->ShowView( 0 );

	g_pTilePanel = pTilePanel;

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
