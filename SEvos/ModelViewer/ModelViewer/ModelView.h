#pragma once
#include "afxwin.h"
#include "mfc/panelListner.h"
#include "common/mouseinput.h"


class CLight;
class CModel;
class CModelView : public CView, public IPanelListner
{
public:
	CModelView(void);
	~CModelView(void);

protected:
	CModel *m_pModel;
	CLight *m_pLight;

	// mouse
	CMouseInput m_Mouse;
	int m_nRMouseX;
	int m_nRMouseY;
	float m_fRMouseZ;

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
	void Clear();

protected:
	void MouseProc();
	void KeyProc( int nElapsTick );
	void RenderFps();

public:
	DECLARE_MESSAGE_MAP()
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
protected:
	virtual void OnDraw(CDC* /*pDC*/);
};
