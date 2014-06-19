// TerrainView.cpp : implementation file
//

#include "stdafx.h"
#include "MapTool2.h"
#include "TerrainView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTerrainView


CTerrainView::CTerrainView()
{
}

CTerrainView::~CTerrainView()
{
}


BEGIN_MESSAGE_MAP(CTerrainView, CView)
	//{{AFX_MSG_MAP(CTerrainView)
	ON_WM_ERASEBKGND()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTerrainView drawing

void CTerrainView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: add draw code here
}

/////////////////////////////////////////////////////////////////////////////
// CTerrainView diagnostics

#ifdef _DEBUG
void CTerrainView::AssertValid() const
{
	CView::AssertValid();
}

void CTerrainView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CTerrainView message handlers

BOOL CTerrainView::OnEraseBkgnd(CDC* pDC) 
{
	return TRUE;
}


