#if !defined(AFX_LAYERLISTCTRL_H__20D8D689_0212_43A2_86A2_2063A139B101__INCLUDED_)
#define AFX_LAYERLISTCTRL_H__20D8D689_0212_43A2_86A2_2063A139B101__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// LayerListCtrl.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CLayerListCtrl window
#include "global.h"

class CChunk;
class CLayerListCtrl : public CListCtrl
{
// Construction
public:
	CLayerListCtrl();

protected:
	BOOL m_bDrag;
	int m_SelectItem;
	CChunk *m_pCurrentChunk;

public:
	void SetCurrentChunk( CChunk *pchunk );

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLayerListCtrl)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CLayerListCtrl();

	// Generated message map functions
protected:
	//{{AFX_MSG(CLayerListCtrl)
	afx_msg void OnBegindrag(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LAYERLISTCTRL_H__20D8D689_0212_43A2_86A2_2063A139B101__INCLUDED_)
