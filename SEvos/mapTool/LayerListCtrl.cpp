// LayerListCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "maptool2.h"
#include "LayerListCtrl.h"

#include "mapview.h"
#include "terrain/terraineditor.h"
#include "tilepanel.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CLayerListCtrl

CLayerListCtrl::CLayerListCtrl():
m_bDrag(FALSE),
m_pCurrentChunk(NULL)
{
}

CLayerListCtrl::~CLayerListCtrl()
{
}


BEGIN_MESSAGE_MAP(CLayerListCtrl, CListCtrl)
	//{{AFX_MSG_MAP(CLayerListCtrl)
	ON_NOTIFY_REFLECT(LVN_BEGINDRAG, OnBegindrag)
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLayerListCtrl message handlers


void CLayerListCtrl::OnBegindrag(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
/*
	POSITION pos = GetFirstSelectedItemPosition();
	while( pos )
	{
		int nItem = GetNextSelectedItem( pos );
//		pterrain->DeleteLayer( m_pCurrentChunk, nItem - ndelcnt++ );
	}
/**/
	m_bDrag = TRUE;
	m_SelectItem = pNMListView->iItem;
	
	*pResult = 0;
}

void CLayerListCtrl::OnMouseMove(UINT nFlags, CPoint point) 
{
	if( m_bDrag )
	{
//		int item = HitTest( point );
//		if( 0 > item ) return;
	}	
	
	CListCtrl::OnMouseMove(nFlags, point);
}

void CLayerListCtrl::OnLButtonUp(UINT nFlags, CPoint point) 
{
	if( m_bDrag )
	{
		m_bDrag = FALSE;
		if( !m_pCurrentChunk ) return;

//		int item = HitTest( point );
//		if( 0 > item )
		{
//			m_SelectItem = -1;
//			return;
		}

//		CMapEditor *pterrain = g_pTerrainView->GetTerrain();
//		pterrain->MoveLayer( m_pCurrentChunk, m_SelectItem, item );
		// 레이어 리스트 업데이트
//		g_pTilePanel->SetCurrentChunk( m_pCurrentChunk );
	}

	CListCtrl::OnLButtonUp(nFlags, point);
}


//-----------------------------------------------------------------------------//
// 청크 설정
//-----------------------------------------------------------------------------//
void CLayerListCtrl::SetCurrentChunk( CChunk *pchunk )
{
	m_pCurrentChunk = pchunk;
}
