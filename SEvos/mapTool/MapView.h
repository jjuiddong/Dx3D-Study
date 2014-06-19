#if !defined(AFX_MAPVIEW_H__8B14532C_A0C8_42ED_B6A3_EAD855F34A4B__INCLUDED_)
#define AFX_MAPVIEW_H__8B14532C_A0C8_42ED_B6A3_EAD855F34A4B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MapView.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CMapView view
#include "common/mouseinput.h"
#include "mfc/panelListner.h"


class CLight;
class CCamera;
class CTerrainEditor;
class CTerrainCursor;
class CMouseEventListner;
class CMapView : public CView, public IPanelListner
{
public:
	CMapView();
	~CMapView();

protected:
	enum EIDT_ORDER { TERRAIN_UP, TERRAIN_DOWN };

	EDIT_MODE m_EditMode;
	CTerrainEditor *m_pTerrain;
	CTerrainCursor *m_pCursor;

	// mouse
	CMouseEventListner *m_pMouseEventListner[ EM_MAX_SIZE];
	CMouseInput m_Mouse;
	int m_nRMouseX;
	int m_nRMouseY;
	float m_fRMouseZ;

	// file path
	char m_CreateFilePath[ MAX_PATH];

	// drag
	CPoint m_ptClickPos;
	BOOL m_bAlt;

	// fps
	int m_ElapseTime;
	int m_Fps;
	char m_szFps[ 64];
	ID3DXSprite *m_pTextSprite;
	ID3DXFont *m_pDxFont;

public:
	BOOL Init();
	void Render();
	void Update( int nElapsTick );
	virtual void Update(IPanelObserver *pObserver);
	BOOL FileOpen( char *szFileName );
	BOOL FileSave( char *szFileName );
	BOOL CreateTerrain( SCrTerrain *pCrTerrain );
	void SelectObj( OBJ_TYPE eOType, char *szObjName );
	BOOL SetOption( int nOptId, int nValue );
	void TimerEvent( int nTimerId );
	void ChangedEditMode( EDIT_MODE eMode );
	CTerrainEditor* GetTerrain() { return m_pTerrain; }
	CTerrainCursor* GetTerrainCursor() { return m_pCursor; }

protected:
	void MouseProc();
	void KeyProc( int nElapsTick );
	void RenderFps();
	void CalcFPS(int nElapsTick );

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMapView)
	protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	//}}AFX_VIRTUAL

// Implementation
protected:
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(CMapView)
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MAPVIEW_H__8B14532C_A0C8_42ED_B6A3_EAD855F34A4B__INCLUDED_)
