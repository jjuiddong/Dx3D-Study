
#ifndef __TERRAINEDITMOUSELISTNER_H__
#define __TERRAINEDITMOUSELISTNER_H__

#include "common/mouseEventListner.h"


class CTerrainEditor;
class CTerrainCursor;
class CTerrainEditMouseListner : public CMouseEventListner
{
public:
	CTerrainEditMouseListner(CWnd *pParent);
	virtual ~CTerrainEditMouseListner() {}

protected:
	CMapView *m_pMapView;
	BOOL m_bBrush;

public:
	virtual void Update(int elapsTick);
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
