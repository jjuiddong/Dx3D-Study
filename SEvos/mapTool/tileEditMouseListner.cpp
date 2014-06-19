
#include "StdAfx.h"
#include "terrain/TerrainEditor.h"
#include "terrain/terrainCursor.h"
#include "MapView.h"
#include "tileEditMouseListner.h"


CTileEditMouseListner::CTileEditMouseListner(CWnd *pParent) 
: CMouseEventListner(pParent)
{
	m_pMapView = (CMapView*)pParent;
	m_bBrush = FALSE;

}


//------------------------------------------------------------------------
// 
// [2011/2/26 jjuiddong]
//------------------------------------------------------------------------
void CTileEditMouseListner::Update(int elapsTick)
{
}


//------------------------------------------------------------------------
// 
// [2011/2/25 jjuiddong]
//------------------------------------------------------------------------
void CTileEditMouseListner::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{

}


//------------------------------------------------------------------------
// 
// [2011/2/25 jjuiddong]
//------------------------------------------------------------------------
void CTileEditMouseListner::OnLButtonDown(UINT nFlags, CPoint point)
{
	CTerrainCursor *pCursor = GetTerrainCursor();
	CTerrainEditor *pTerrain = GetTerrain();

	m_bBrush = TRUE;
	if (!pTerrain->DrawBrush(pCursor))
		m_bBrush = FALSE;
}


//------------------------------------------------------------------------
// 
// [2011/2/25 jjuiddong]
//------------------------------------------------------------------------
void CTileEditMouseListner::OnLButtonUp(UINT nFlags, CPoint point)
{
	m_bBrush = FALSE;

}


//------------------------------------------------------------------------
// 
// [2011/2/25 jjuiddong]
//------------------------------------------------------------------------
void CTileEditMouseListner::OnMouseMove(UINT nFlags, CPoint point)
{
	CTerrainCursor *pCursor = GetTerrainCursor();
	CTerrainEditor *pTerrain = GetTerrain();

	if (m_bBrush) 
		pTerrain->DrawBrush(pCursor);
}


//------------------------------------------------------------------------
// 
// [2011/2/25 jjuiddong]
//------------------------------------------------------------------------
void CTileEditMouseListner::OnRButtonDown(UINT nFlags, CPoint point)
{

}


//------------------------------------------------------------------------
// 
// [2011/2/25 jjuiddong]
//------------------------------------------------------------------------
void CTileEditMouseListner::OnRButtonUp(UINT nFlags, CPoint point)
{
	m_bBrush = FALSE;
}


//------------------------------------------------------------------------
// 
// [2011/2/25 jjuiddong]
//------------------------------------------------------------------------
CTerrainEditor* CTileEditMouseListner::GetTerrain()
{
	return m_pMapView->GetTerrain();
}


//------------------------------------------------------------------------
// 
// [2011/2/25 jjuiddong]
//------------------------------------------------------------------------
CTerrainCursor* CTileEditMouseListner::GetTerrainCursor()
{
	return m_pMapView->GetTerrainCursor();
}




