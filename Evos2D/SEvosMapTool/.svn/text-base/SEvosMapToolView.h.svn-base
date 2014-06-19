// SEvosMapToolView.h : CSEvosMapToolView 클래스의 인터페이스
//


#pragma once

#include "../Common/global.h"
#include "mapeditor.h"

class CSEvosMapToolView : public CView
{
protected:
	CSEvosMapToolView();
	DECLARE_DYNCREATE(CSEvosMapToolView)

protected:
	enum EDIT_MODE { EDIT_NORMAL, EDIT_BLOCK_FROM, EDIT_BLOCK_TO };

	EDIT_MODE m_EditMode;
	CMapEditor m_MapEditor;
	CPoint m_OffsetPos;
	CSize m_MapSize;

	bool m_IsRenderID;
	Wall2D *m_pFocusWall;
	CBlock *m_pFocusBlock;

public:
	CSEvosMapToolDoc* GetDocument() const;

public:
	virtual void OnDraw(CDC* pDC);
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	void SetEditMode(EDIT_MODE mode);
	EDIT_MODE GetEditMode() { return m_EditMode; }
	void SetBlockEndPointToStartPoint();

public:
	virtual ~CSEvosMapToolView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	
// 생성된 메시지 맵 함수
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnEditBlock();
	afx_msg void OnEditGenerateNavi();
	afx_msg void OnFileSave();
	afx_msg void OnEditReset();
	afx_msg void OnFileOpen();
	afx_msg void OnEditIdshow();
};

#ifndef _DEBUG  // SEvosMapToolView.cpp의 디버그 버전
inline CSEvosMapToolDoc* CSEvosMapToolView::GetDocument() const
   { return reinterpret_cast<CSEvosMapToolDoc*>(m_pDocument); }
#endif

