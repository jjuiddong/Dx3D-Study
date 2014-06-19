#if !defined(AFX_TILEPANEL_H__B856DC9F_BA68_4AA3_9E98_118FB04F00DA__INCLUDED_)
#define AFX_TILEPANEL_H__B856DC9F_BA68_4AA3_9E98_118FB04F00DA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TilePanel.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CTilePanel dialog
#include "layerlistctrl.h"
#include "terrain/chunk.h"
#include "mfc/panelBase.h"


class CTilePanel : public CPanelBase
{
// Construction
public:
	CTilePanel(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CTilePanel)
	enum { IDD = IDD_DLG_TILE };
	CLayerListCtrl	m_LayerList;
	CSliderCtrl	m_BrushSpeed;	// 1 ~ 100
	CSliderCtrl	m_BrushSize;	// 1 ~ 30
	CSliderCtrl	m_BrushCenter;	// 1 ~ 30
	CListBox m_TileList;
	CString	m_strBrushCenter;
	CString	m_strBrushSize;
	CString	m_strBrushSpeed;
	CString	m_strChunkPos;
	CString	m_strMaxLayerCount;
	CString	m_strTileDirPath;
	BOOL	m_chkEditTileMode;
	CString	m_strCenterAlpha;
	CString	m_strEdgeAlpha;
	BOOL	m_chkErase;
	BOOL	m_chkMatchTexture;
	//}}AFX_DATA

protected:
	CChunk *m_pCurrentChunk;
	CRect m_PannelViewPort;
	BOOL m_bDrag;
	CPoint m_ptClickPos;

public:
	void SetCurrentChunk( CChunk *pchunk );
	BOOL IsEditTileMode();

protected:
	void RefreshTileList( char *szDirectory );
	CString MakeLayerListString( int layeridx, CChunk::SLayer *player );
	void SetLayerFlag( CChunk *pchunk, DWORD flag, BOOL onoff );

public:
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTilePanel)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual void OnShowWindow();
	virtual void OnHideWindow();

	// Generated message map functions
	//{{AFX_MSG(CTilePanel)
	afx_msg void OnButtonRefresh();
	afx_msg void OnButtonDeletelayer();
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeListTile();
	afx_msg void OnCheckTileedit();
	afx_msg void OnChangeEditCenterAlpha();
	afx_msg void OnChangeEditEdgeAlpha();
	afx_msg void OnButtonShowlayer();
	afx_msg void OnButtonEditlayer();
	afx_msg void OnButtonHidelayer();
	afx_msg void OnButtonLocklayer();
	afx_msg void OnCheckErasebrush();
	afx_msg void OnCheckMatchTexture();
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnNMCustomdrawSliderBrushsize(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMCustomdrawSliderBrushcenter(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMCustomdrawSliderBrushspeed(NMHDR *pNMHDR, LRESULT *pResult);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TILEPANEL_H__B856DC9F_BA68_4AA3_9E98_118FB04F00DA__INCLUDED_)
