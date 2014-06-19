#pragma once
#include "afxwin.h"

#define WM_NOTIFY_SELECTLIST WM_USER + 1

class CFileListBox : public CDialog
{
public:
	CFileListBox(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CFileListBox();
	enum { IDD = IDD_FILELIST };

protected:
	CString m_FileListName;
	CListBox m_FileListBox;
	char m_FilePath[ MAX_PATH];
	std::list<string> m_ExtendNameList;

public:
	void LoadFile( std::list<std::string> *pFindExt, char *filePath );
	void Refresh();
	void SetFileListName( char *fileListName );
	void SetFilePath( char *filePath );
	void SetExtendNameList( std::list<std::string> *pextList );

	CString GetFileListName() { return m_FilePath + CString("/") + m_FileListName; }
	char* GetFilePath() { return m_FilePath; }
	CString GetSelectItemName();
	CListBox* GetFileListBox() { return &m_FileListBox; }

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedRefresh();
	afx_msg void OnBnClickedNewpath();
	virtual BOOL OnInitDialog();
	afx_msg void OnLbnSelchangeFilelist();
	afx_msg void OnSize(UINT nType, int cx, int cy);
};
