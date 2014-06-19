
#ifndef __MOUSEEVENTLISTNER_H__
#define __MOUSEEVENTLISTNER_H__

//------------------------------------------------------------------------
// 마우스 이벤트를 처리하는 리스너다.
// 이 클래스는 아무일도 하지 않는다.
// [2011/2/25 jjuiddong]
//------------------------------------------------------------------------

class CMouseEventListner
{
public:
	CMouseEventListner(CWnd *parent) : m_pParent(parent) {}
	virtual~CMouseEventListner() {}

protected:
	CWnd *m_pParent;

public:
	CWnd* GetParent() { return m_pParent; }
	virtual void Update(int elapsTick) {}
	virtual void OnMouseWheel(UINT nFlags, short zDelta, CPoint pt) {}
	virtual void OnLButtonDown(UINT nFlags, CPoint point) {}
	virtual void OnLButtonUp(UINT nFlags, CPoint point) {}
	virtual void OnMouseMove(UINT nFlags, CPoint point) {}
	virtual void OnRButtonDown(UINT nFlags, CPoint point) {}
	virtual void OnRButtonUp(UINT nFlags, CPoint point) {}

};

#endif
