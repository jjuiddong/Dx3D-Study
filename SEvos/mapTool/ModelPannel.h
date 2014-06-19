#if !defined(AFX_MODELPANEL_H__0557E951_EF5F_4963_9BBD_63BFF0EFA6E1__INCLUDED_)
#define AFX_MODELPANEL_H__0557E951_EF5F_4963_9BBD_63BFF0EFA6E1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "mfc/panelBase.h"

class CModelFielListBox;
class CModelPanel : public CPanelBase
{
public:
	CModelPanel(CWnd* pParent = NULL);   // standard constructor
	virtual ~CModelPanel();
	enum { IDD = IDD_DLG_MODEL };

protected:
	CModelFielListBox *m_pModelFileListBox;

public:
	CString GetSelectModelName();

protected:
	void LoadTable();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

protected:
	void OnShowWindow();
	void OnHideWindow();
	afx_msg void OnButtonReflesh();
	virtual BOOL OnInitDialog();
	afx_msg LRESULT OnLbnSelchangeFilelist(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
};

#endif
