#if !defined(AFX_BASEPANELFRAME_H__6AB5C2F7_3515_4FEB_BE8B_BAF3E1900887__INCLUDED_)
#define AFX_BASEPANELFRAME_H__6AB5C2F7_3515_4FEB_BE8B_BAF3E1900887__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// BasePanelFrame.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CBasePanelFrame frame

class CTabBase;
class CBasePanelFrame : public CMiniFrameWnd
{
//	DECLARE_DYNCREATE(CBasePanelFrame)
public:
	CBasePanelFrame();           // protected constructor used by dynamic creation

// Attributes
public:
	CTabBase *m_pTab;

// Operations
public:
	void AddPanelListner(IPanelListner *plistner);

protected:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBasePanelFrame)
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CBasePanelFrame();

	// Generated message map functions
	//{{AFX_MSG(CBasePanelFrame)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BASEPANELFRAME_H__6AB5C2F7_3515_4FEB_BE8B_BAF3E1900887__INCLUDED_)
