// MapTool2Dlg.h : header file
//

#if !defined(AFX_MAPTOOL2DLG_H__18F1119E_869D_4B9F_AC6D_CE716F8A0F95__INCLUDED_)
#define AFX_MAPTOOL2DLG_H__18F1119E_869D_4B9F_AC6D_CE716F8A0F95__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CMapTool2Dlg dialog
#include "global.h"
#include "tabbase.h"


class CMapTool2Dlg : public CDialog
{
// Construction
public:
	CMapTool2Dlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CMapTool2Dlg)
	enum { IDD = IDD_MAPTOOL2_DIALOG };
	int m_nBitCount;
	BOOL	m_ChkFog;
	BOOL	m_ChkTexture;
	BOOL	m_ChkVertex;
	BOOL	m_ChkChunkEdge;
	BOOL	m_ChkDispQuad;
	BOOL	m_ChkNormal;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMapTool2Dlg)
public:
	virtual BOOL DestroyWindow();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

protected:
	BOOL m_bLoop;
	int m_nDeltaTime;
	char m_CurrentDirectory[ MAX_PATH];

public:
	BOOL Init();
	void MainProc();

protected:	
	BOOL Setup();

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CMapTool2Dlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	virtual void OnCancel();
	afx_msg void OnButtonOpen();
	afx_msg void OnButtonSave();
	afx_msg void OnButtonCreate();
	afx_msg void OnButtonOption();
	afx_msg void OnCheckFog();
	afx_msg void OnCheckTexture();
	afx_msg void OnCheckVertex();
	afx_msg void OnCheckChunkedge();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedCheckDispquadtree();
	afx_msg void OnBnClickedCheckNormal();
	afx_msg void OnBnClickedCheckShadow();
	BOOL m_ChkShadow;
};

bool Display( int timeDelta );

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MAPTOOL2DLG_H__18F1119E_869D_4B9F_AC6D_CE716F8A0F95__INCLUDED_)
