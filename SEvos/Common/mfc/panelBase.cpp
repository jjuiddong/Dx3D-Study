
#include "stdafx.h"
#include "panelBase.h"



CPanelBase::CPanelBase(PANEL_TYPE type, UINT id, CWnd* pParent)  // pParent = NULL
	: CDialog(id, pParent), IPanelObserver(type)
{

}

BEGIN_MESSAGE_MAP(CPanelBase, CDialog)
	//{{AFX_MSG_MAP(CTerrainPanel)
	ON_WM_SHOWWINDOW()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


//------------------------------------------------------------------------
// 
// [2011/2/21 jjuiddong]
//------------------------------------------------------------------------
CRect CPanelBase::GetChildRect(UINT id)
{
	CWnd *pitem = GetDlgItem(id);
	if (!pitem) return CRect(0,0,0,0);

	CRect parentRect;
	GetWindowRect(&parentRect);

	CRect curRect;
	pitem->GetWindowRect(&curRect);
	CRect rect(curRect.left - parentRect.left, curRect.top-parentRect.top, 0, 0);

	rect.right = rect.left + curRect.Width();
	rect.bottom = rect.top + curRect.Height();
	return rect;
}


//------------------------------------------------------------------------
// 
// [2011/2/26 jjuiddong]
//------------------------------------------------------------------------
void CPanelBase::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CDialog::OnShowWindow(bShow, nStatus);
	if( bShow ) 
	{
		OnShowWindow();
		Update(); // listner에게 알림
	} 
	else
	{
		OnHideWindow();
	}
}


//------------------------------------------------------------------------
// 
// [2011/3/1 jjuiddong]
//------------------------------------------------------------------------
void CPanelBase::OnShowWindow()
{
	g_Dbg.Console("show window\n");

}


//------------------------------------------------------------------------
// 
// [2011/3/1 jjuiddong]
//------------------------------------------------------------------------
void CPanelBase::OnHideWindow()
{
	g_Dbg.Console("hide window\n");

}
