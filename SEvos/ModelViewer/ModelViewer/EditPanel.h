#pragma once
#include "mfc/panelBase.h"


// CEditPanel 대화 상자입니다.

class CEditPanel : public CPanelBase
{

public:
	CEditPanel(CWnd* pParent = NULL);
	virtual ~CEditPanel();
	enum { IDD = IDD_EDIT_PANEL };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
};
