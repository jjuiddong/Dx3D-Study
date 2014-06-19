
#include "stdafx.h"
#include "terrain/TerrainEditor.h"
#include "terrain/terrainCursor.h"
#include "MapView.h"
#include "TerrainPannel.h"
#include "terrainEditMouseListner.h"


CTerrainEditMouseListner::CTerrainEditMouseListner(CWnd *pParent) 
: CMouseEventListner(pParent)
{
	m_pMapView = (CMapView*)pParent;
	m_bBrush = FALSE;

}


//------------------------------------------------------------------------
// 
// [2011/2/26 jjuiddong]
//------------------------------------------------------------------------
void CTerrainEditMouseListner::Update(int elapsTick)
{
	CTerrainCursor *pCursor = GetTerrainCursor();
	CTerrainEditor *pTerrain = GetTerrain();

	if( m_bBrush )
	{
		pTerrain->SetHeight( pCursor, elapsTick);
		pCursor->UpdateCursor();
	}

//	g_pTerrainPanel->UpdateCursorInfo();
}


//------------------------------------------------------------------------
// 
// [2011/2/25 jjuiddong]
//------------------------------------------------------------------------
void CTerrainEditMouseListner::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{

}


//------------------------------------------------------------------------
// 
// [2011/2/25 jjuiddong]
//------------------------------------------------------------------------
void CTerrainEditMouseListner::OnLButtonDown(UINT nFlags, CPoint point)
{
	m_bBrush = TRUE;
}


//------------------------------------------------------------------------
// 
// [2011/2/25 jjuiddong]
//------------------------------------------------------------------------
void CTerrainEditMouseListner::OnLButtonUp(UINT nFlags, CPoint point)
{
	m_bBrush = FALSE;

}


//------------------------------------------------------------------------
// 
// [2011/2/25 jjuiddong]
//------------------------------------------------------------------------
void CTerrainEditMouseListner::OnMouseMove(UINT nFlags, CPoint point)
{
	CTerrainCursor *pCursor = GetTerrainCursor();
	CTerrainEditor *pTerrain = GetTerrain();

	if( m_bBrush )
	{
		Vector3 vTarget;
		if( !pTerrain->Pick( &Vector2((float)point.x, (float)point.y), &vTarget ) )
		{
			m_bBrush = FALSE;
		}
	}
}


//------------------------------------------------------------------------
// 
// [2011/2/25 jjuiddong]
//------------------------------------------------------------------------
void CTerrainEditMouseListner::OnRButtonDown(UINT nFlags, CPoint point)
{

}


//------------------------------------------------------------------------
// 
// [2011/2/25 jjuiddong]
//------------------------------------------------------------------------
void CTerrainEditMouseListner::OnRButtonUp(UINT nFlags, CPoint point)
{
	m_bBrush = FALSE;
}


//------------------------------------------------------------------------
// 
// [2011/2/25 jjuiddong]
//------------------------------------------------------------------------
CTerrainEditor* CTerrainEditMouseListner::GetTerrain()
{
	return m_pMapView->GetTerrain();
}


//------------------------------------------------------------------------
// 
// [2011/2/25 jjuiddong]
//------------------------------------------------------------------------
CTerrainCursor* CTerrainEditMouseListner::GetTerrainCursor()
{
	return m_pMapView->GetTerrainCursor();
}


