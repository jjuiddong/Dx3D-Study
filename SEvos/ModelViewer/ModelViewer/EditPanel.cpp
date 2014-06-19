// EditPanel.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "ModelViewer.h"
#include "EditPanel.h"


// CEditPanel 대화 상자입니다.

CEditPanel::CEditPanel(CWnd* pParent /*=NULL*/)
	: CPanelBase(MODEL, CEditPanel::IDD, pParent)
{

}

CEditPanel::~CEditPanel()
{
}

void CEditPanel::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CEditPanel, CDialog)
END_MESSAGE_MAP()


// CEditPanel 메시지 처리기입니다.
