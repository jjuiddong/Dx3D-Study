#if !defined(AFX_RTERRAINDLG_H__D502928F_4C33_45CB_87D9_A57464BAFAF3__INCLUDED_)
#define AFX_RTERRAINDLG_H__D502928F_4C33_45CB_87D9_A57464BAFAF3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// rTerrainDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CCrTerrainDlg dialog

class CCrTerrainDlg : public CDialog
{
// Construction
public:
	CCrTerrainDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CCrTerrainDlg)
	enum { IDD = IDD_DLG_CREATETERRAIN };
		// NOTE: the ClassWizard will add data members here
	int m_CellSize;
	CString m_strTextureName;
	//}}AFX_DATA

protected:
	SCrTerrain m_CreateData;

public:
	SCrTerrain* GetCreateData() { return &m_CreateData; }

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCrTerrainDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CCrTerrainDlg)
	virtual void OnOK();
	virtual void OnCancel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnBnClickedButtonTexture();
	virtual BOOL OnInitDialog();
	CString m_strWidth;
	CString m_strHeight;
	CString m_strVtxPerRow;
	CString m_strVtxPerCol;
	CString m_strFilePath;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_RTERRAINDLG_H__D502928F_4C33_45CB_87D9_A57464BAFAF3__INCLUDED_)
