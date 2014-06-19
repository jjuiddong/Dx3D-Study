
#ifndef __PANELBASE_H__
#define __PANELBASE_H__

#include "afxwin.h"
#include "panelListner.h"


class CPanelBase : public CDialog, public IPanelObserver
{
public:
	CPanelBase(PANEL_TYPE type, UINT id, CWnd* pParent = NULL); 
	virtual ~CPanelBase() {}

protected:

public:
	CRect GetChildRect(UINT id);

protected:
	virtual void OnShowWindow();
	virtual void OnHideWindow();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	DECLARE_MESSAGE_MAP()
};

#endif
