
#include "StdAfx.h"
#include "MapView.h"
#include "terrain/TerrainEditor.h"
#include "terrain/terrainCursor.h"
#include "modelEditMouseListner.h"


CModelEditMouseListner::CModelEditMouseListner(CWnd *pParent) 
	: CMouseEventListner(pParent)
{
	m_pMapView = (CMapView*)pParent;

}


//------------------------------------------------------------------------
// 
// [2011/2/25 jjuiddong]
//------------------------------------------------------------------------
void CModelEditMouseListner::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{

}


//------------------------------------------------------------------------
// 
// [2011/2/25 jjuiddong]
//------------------------------------------------------------------------
void CModelEditMouseListner::OnLButtonDown(UINT nFlags, CPoint point)
{
	CTerrainCursor *pCursor = GetTerrainCursor();
	CTerrainEditor *pTerrain = GetTerrain();
	if (pTerrain->IsSelected())
	{
		pTerrain->LocateModel(pCursor);
	}
	else
	{
		pTerrain->Capture(Vector2((float)point.x, (float)point.y));
	}
}


//------------------------------------------------------------------------
// 
// [2011/2/25 jjuiddong]
//------------------------------------------------------------------------
void CModelEditMouseListner::OnLButtonUp(UINT nFlags, CPoint point)
{

}


//------------------------------------------------------------------------
// 
// [2011/2/25 jjuiddong]
//------------------------------------------------------------------------
void CModelEditMouseListner::OnMouseMove(UINT nFlags, CPoint point)
{
	CTerrainCursor *pCursor = GetTerrainCursor();
	CTerrainEditor *pTerrain = GetTerrain();
	pTerrain->MoveSelectModel(pCursor);
}


//------------------------------------------------------------------------
// 
// [2011/2/25 jjuiddong]
//------------------------------------------------------------------------
void CModelEditMouseListner::OnRButtonDown(UINT nFlags, CPoint point)
{

}


//------------------------------------------------------------------------
// 
// [2011/2/25 jjuiddong]
//------------------------------------------------------------------------
void CModelEditMouseListner::OnRButtonUp(UINT nFlags, CPoint point)
{

}


//------------------------------------------------------------------------
// 
// [2011/2/25 jjuiddong]
//------------------------------------------------------------------------
CTerrainEditor* CModelEditMouseListner::GetTerrain()
{
	return m_pMapView->GetTerrain();
}


//------------------------------------------------------------------------
// 
// [2011/2/25 jjuiddong]
//------------------------------------------------------------------------
CTerrainCursor* CModelEditMouseListner::GetTerrainCursor()
{
	return m_pMapView->GetTerrainCursor();
}

