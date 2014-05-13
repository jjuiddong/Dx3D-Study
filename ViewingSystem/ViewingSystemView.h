// ViewingSystemView.h : interface of the CViewingSystemView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_VIEWINGSYSTEMVIEW_H__3E0F15B0_A739_42CE_8176_C5FDEED76166__INCLUDED_)
#define AFX_VIEWINGSYSTEMVIEW_H__3E0F15B0_A739_42CE_8176_C5FDEED76166__INCLUDED_

#include "ViewingSystemDoc.h"	// Added by ClassView
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CViewingSystemView : public CView
{
protected: // create from serialization only
	CViewingSystemView();
	DECLARE_DYNCREATE(CViewingSystemView)

// Attributes
public:
	CViewingSystemDoc* GetDocument();

	bool m_initState;
	CPoint rButtonDownPoint;
	bool isRightButtonDown;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CViewingSystemView)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CViewingSystemView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CViewingSystemView)
	afx_msg void OnFileOpen();
	afx_msg void OnParallel();
	afx_msg void OnPerspective();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnFlat();
	afx_msg void OnGouraud();
	afx_msg void OnPhong();
	afx_msg void OnWireframe();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnCartoon();
	afx_msg void OnCartoon2();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in ViewingSystemView.cpp
inline CViewingSystemDoc* CViewingSystemView::GetDocument()
   { return (CViewingSystemDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VIEWINGSYSTEMVIEW_H__3E0F15B0_A739_42CE_8176_C5FDEED76166__INCLUDED_)
