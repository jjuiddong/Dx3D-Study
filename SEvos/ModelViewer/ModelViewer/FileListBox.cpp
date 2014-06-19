// FileListBox.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "ModelViewer.h"
#include "common/utility.h"
#include "FileListBox.h"

using namespace std;


CFileListBox::CFileListBox(CWnd* pParent /*=NULL*/)
: CDialog(CFileListBox::IDD, pParent)
	, m_FileListName(_T(""))
{

}

CFileListBox::~CFileListBox()
{
}

void CFileListBox::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_FILELIST, m_FileListBox);
	DDX_Text(pDX, IDC_LISTNAME, m_FileListName);
}


BEGIN_MESSAGE_MAP(CFileListBox, CDialog)
	ON_BN_CLICKED(IDC_REFRESH, &CFileListBox::OnBnClickedRefresh)
	ON_BN_CLICKED(IDC_NEWPATH, &CFileListBox::OnBnClickedNewpath)
	ON_LBN_SELCHANGE(IDC_FILELIST, &CFileListBox::OnLbnSelchangeFilelist)
	ON_WM_SIZE()
END_MESSAGE_MAP()


//------------------------------------------------------------------------
// 
// [2011/2/22 jjuiddong]
//------------------------------------------------------------------------
void CFileListBox::OnBnClickedRefresh()
{
	Refresh();
}


//------------------------------------------------------------------------
// 
// [2011/2/22 jjuiddong]
//------------------------------------------------------------------------
void CFileListBox::OnBnClickedNewpath()
{

}


//------------------------------------------------------------------------
// 
// [2011/2/22 jjuiddong]
//------------------------------------------------------------------------
BOOL CFileListBox::OnInitDialog()
{
	CDialog::OnInitDialog();
	return TRUE;
}


//------------------------------------------------------------------------
// 
// [2011/2/22 jjuiddong]
//------------------------------------------------------------------------
void CFileListBox::LoadFile( std::list<std::string> *pFindExt, char *filePath )
{
	list<string> fileList;
	CollectFile(pFindExt, filePath, &fileList);

	while (LB_ERR != m_FileListBox.DeleteString(0)) {}

	list<string>::iterator it = fileList.begin();
	while( fileList.end() != it )
		m_FileListBox.AddString( it++->c_str() );
}


//------------------------------------------------------------------------
// 
// [2011/2/22 jjuiddong]
//------------------------------------------------------------------------
void CFileListBox::Refresh()
{
	LoadFile(&m_ExtendNameList, m_FilePath);
}


//------------------------------------------------------------------------
// 
// [2011/2/22 jjuiddong]
//------------------------------------------------------------------------
void CFileListBox::SetFileListName( char *fileListName )
{
	m_FileListName = fileListName;
	UpdateData(FALSE);
}


//------------------------------------------------------------------------
// 
// [2011/2/22 jjuiddong]
//------------------------------------------------------------------------
void CFileListBox::SetFilePath( char *filePath )
{
	strcpy_s(m_FilePath, sizeof(m_FilePath), filePath);
}


//------------------------------------------------------------------------
// 
// [2011/2/22 jjuiddong]
//------------------------------------------------------------------------
void CFileListBox::SetExtendNameList( std::list<std::string> *pextList )
{
	if (!pextList) return;

	m_ExtendNameList.clear();
	list<string>::iterator itor = pextList->begin();
	while (pextList->end() != itor)
		m_ExtendNameList.push_back(*itor++);
}


//------------------------------------------------------------------------
// 
// [2011/2/22 jjuiddong]
//------------------------------------------------------------------------
CString CFileListBox::GetSelectItemName()
{
	const int selIndex = m_FileListBox.GetCurSel();
	if (selIndex < 0) return "";
	
	CString filename;
	m_FileListBox.GetText( selIndex, filename );
	return m_FilePath + CString("/") +  filename;
}


//------------------------------------------------------------------------
// 리스트박스가 선택될 때 호출된다.
// [2011/2/22 jjuiddong]
//------------------------------------------------------------------------
void CFileListBox::OnLbnSelchangeFilelist()
{
	// 부모윈도우에게 리스트가 바꼈다고 알려준다.
	GetParent()->PostMessage(WM_NOTIFY_SELECTLIST, (WPARAM)this);
}


//------------------------------------------------------------------------
// 
// [2011/2/23 jjuiddong]
//------------------------------------------------------------------------
void CFileListBox::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	CWnd *plistName = GetDlgItem(IDC_LISTNAME);
	CWnd *pfileList = GetDlgItem(IDC_FILELIST);
	CWnd *prefresh = GetDlgItem(IDC_REFRESH);
	CWnd *pnewPath = GetDlgItem(IDC_NEWPATH);

	if (!plistName || !pfileList || !prefresh || !pnewPath)
		return;

	CRect windowRect, listNameRect, fileListRect, refreshRect, newPathRect;
	GetClientRect(windowRect);
	plistName->GetClientRect(listNameRect);
	pfileList->GetClientRect(fileListRect);
	prefresh->GetClientRect(refreshRect);
	pnewPath->GetClientRect(newPathRect);

	int h = 2;
	listNameRect.OffsetRect(0, h);

	h += listNameRect.Height();
	h += 5;

	fileListRect.OffsetRect(0, h);
	int height = windowRect.Height() - listNameRect.Height() - refreshRect.Height() - 12;
	fileListRect.bottom = fileListRect.top + height;
	fileListRect.right = windowRect.Width();

	h += height;
	h += 5;

	refreshRect.OffsetRect(0, h);
	newPathRect.OffsetRect(refreshRect.Width()+10, h);

	plistName->MoveWindow(listNameRect);
	pfileList->MoveWindow(fileListRect);
	prefresh->MoveWindow(refreshRect);
	pnewPath->MoveWindow(newPathRect);
}
