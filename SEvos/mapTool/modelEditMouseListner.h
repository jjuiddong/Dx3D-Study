
#ifndef __MODELEDITMOUSELISTNER_H__
#define __MODELEDITMOUSELISTNER_H__

#include "common/mouseEventListner.h"

class CMapView;
class CModelEditMouseListner : public CMouseEventListner
{
public:
	CModelEditMouseListner(CWnd *pParent);
	virtual ~CModelEditMouseListner() {}

protected:
	CMapView *m_pMapView;

public:
	virtual void OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	virtual void OnLButtonDown(UINT nFlags, CPoint point);
	virtual void OnLButtonUp(UINT nFlags, CPoint point);
	virtual void OnMouseMove(UINT nFlags, CPoint point);
	virtual void OnRButtonDown(UINT nFlags, CPoint point);
	virtual void OnRButtonUp(UINT nFlags, CPoint point);

	CTerrainEditor* GetTerrain();
	CTerrainCursor* GetTerrainCursor();

};

#endif
