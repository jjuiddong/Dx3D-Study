// ViewPanel.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "ModelViewer.h"
#include "modelFileListBox.h"
#include "aniFileListBox.h"
#include "ViewPanel.h"


// CViewPanel 대화 상자입니다.

CViewPanel::CViewPanel(CWnd* pParent /*=NULL*/) :
	CPanelBase(VIEW, CViewPanel::IDD, pParent)
{

}

CViewPanel::~CViewPanel()
{
	SAFE_DELETE(m_pModelFileListBox);
	SAFE_DELETE(m_pAniFileListBox);

}

void CViewPanel::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CViewPanel, CDialog)
	ON_MESSAGE(WM_NOTIFY_SELECTLIST, &CViewPanel::OnLbnSelchangeFilelist)
END_MESSAGE_MAP()


BOOL CViewPanel::OnInitDialog()
{
	CDialog::OnInitDialog();

	CRect modelRect = GetChildRect(IDC_MODEL_LISTBOX);
	m_pModelFileListBox = new CModelFielListBox();
	m_pModelFileListBox->Create(CModelFielListBox::IDD, this);
	m_pModelFileListBox->MoveWindow(modelRect);
	m_pModelFileListBox->ShowWindow(SW_SHOW);

	CRect aniRect = GetChildRect(IDC_ANI_LISTBOX);
	m_pAniFileListBox = new CAniFileListBox();
	m_pAniFileListBox->Create(CAniFileListBox::IDD, this);
	m_pAniFileListBox->MoveWindow(aniRect);
	m_pAniFileListBox->ShowWindow(SW_SHOW);

	return TRUE; 
}


//------------------------------------------------------------------------
// 
// [2011/2/22 jjuiddong]
//------------------------------------------------------------------------
CString CViewPanel::GetSelectModelName()
{
	return m_pModelFileListBox->GetSelectItemName();
}


//------------------------------------------------------------------------
// 
// [2011/2/22 jjuiddong]
//------------------------------------------------------------------------
CString CViewPanel::GetSelectAnimationName()
{
	return m_pAniFileListBox->GetSelectItemName();
}


//------------------------------------------------------------------------
// 파일리스트가 선택되면 호출된다.
// [2011/2/22 jjuiddong]
//------------------------------------------------------------------------
LRESULT CViewPanel::OnLbnSelchangeFilelist(WPARAM wParam, LPARAM lParam)
{
	Update();
	return 0;
}
