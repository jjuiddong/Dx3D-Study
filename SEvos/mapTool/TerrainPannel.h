#if !defined(AFX_TERRAINPANEL_H__DD12BD8A_F8DC_4406_8B75_6EB32A6A1A87__INCLUDED_)
#define AFX_TERRAINPANEL_H__DD12BD8A_F8DC_4406_8B75_6EB32A6A1A87__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TerrainPanel.h : header file
#include "mfc/panelBase.h"

/////////////////////////////////////////////////////////////////////////////
// CTerrainPanel dialog

class CTerrainPanel : public CPanelBase
{
// Construction
public:
	CTerrainPanel(CWnd* pParent = NULL);   // standard constructor
	virtual ~CTerrainPanel();

// Dialog Data
	//{{AFX_DATA(CTerrainPanel)
	enum { IDD = IDD_DLG_TERRAIN };
	int		m_nHOffset;
	BOOL	m_EditTerrain;
	CString	m_strHOffset;
	CString	m_strBrushSize;
	int		m_nBrushSize;
	CString	m_strCursorPos;
	CString m_strLightDirection;
	BOOL	m_bUpDown;
	//}}AFX_DATA

public:
	BOOL IsEditTerrainMode();
	void UpdateCursorInfo();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTerrainPanel)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void OnShowWindow();
	void OnHideWindow();

	// Generated message map functions
	//{{AFX_MSG(CTerrainPanel)
	virtual BOOL OnInitDialog();
	afx_msg void OnReleasedcaptureSliderHoffset(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnCheckEdit();
	afx_msg void OnReleasedcaptureSliderBrushsize(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnCheckUpdown();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TERRAINPANEL_H__DD12BD8A_F8DC_4406_8B75_6EB32A6A1A87__INCLUDED_)
