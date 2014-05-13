/ ViewingSystemView.cpp : implementation of the CViewingSystemView class
//

#include "stdafx.h"
#include "ViewingSystem.h"

#include "ViewingSystemDoc.h"
#include "ViewingSystemView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CViewingSystemView

IMPLEMENT_DYNCREATE(CViewingSystemView, CView)

BEGIN_MESSAGE_MAP(CViewingSystemView, CView)
	//{{AFX_MSG_MAP(CViewingSystemView)
	ON_COMMAND(ID_FILE_OPEN, OnFileOpen)
	ON_COMMAND(ID_PARALLEL, OnParallel)
	ON_COMMAND(ID_PERSPECTIVE, OnPerspective)
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSEWHEEL()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_COMMAND(ID_FLAT, OnFlat)
	ON_COMMAND(ID_GOURAUD, OnGouraud)
	ON_COMMAND(ID_PHONG, OnPhong)
	ON_COMMAND(ID_WIREFRAME, OnWireframe)
	ON_WM_ERASEBKGND()
	ON_WM_KEYDOWN()
	ON_COMMAND(ID_CARTOON, OnCartoon)
	ON_COMMAND(ID_CARTOON2, OnCartoon2)
	//}}AFX_MSG_MAP
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CView::OnFilePrintPreview)
END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////
// CViewingSystemView construction/destruction

CViewingSystemView::CViewingSystemView()
{
	// TODO: add construction code here

	m_initState = true;

	rButtonDownPoint = CPoint(0, 0);
	isRightButtonDown = false;
}

CViewingSystemView::~CViewingSystemView()
{
}

BOOL CViewingSystemView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CViewingSystemView drawing

void CViewingSystemView::OnDraw(CDC* pDC)
{
	CViewingSystemDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	// TODO: add draw code for native data here

	if (m_initState)
	{
		GetClientRect(&pDoc->win);

		CBitmap bmp;
		bmp.CreateBitmap(pDoc->win.Width(), pDoc->win.Height(), 1, 32, NULL);
		pDoc->memdc.CreateCompatibleDC(pDC);
		pDoc->memdc.SelectObject( bmp );
		pDoc->memdc.ExtFloodFill(0, 0, RGB(255, 255, 255), FLOODFILLBORDER);

		//pDoc->zBuffer = new int[pDoc->win.Width() * pDoc->win.Height()];
		//for (int i = 0; i < pDoc->win.Width() * pDoc->win.Height(); i++)
			//pDoc->zBuffer[i] = -1;


		m_initState = false;
	}

	pDC->BitBlt(pDoc->win.left, pDoc->win.top, pDoc->win.Width(), pDoc->win.Height(), &pDoc->memdc, 0, 0, SRCCOPY);

}

/////////////////////////////////////////////////////////////////////////////
// CViewingSystemView printing

BOOL CViewingSystemView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CViewingSystemView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CViewingSystemView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}

/////////////////////////////////////////////////////////////////////////////
// CViewingSystemView diagnostics

#ifdef _DEBUG
void CViewingSystemView::AssertValid() const
{
	CView::AssertValid();
}

void CViewingSystemView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CViewingSystemDoc* CViewingSystemView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CViewingSystemDoc)));
	return (CViewingSystemDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CViewingSystemView message handlers

void CViewingSystemView::OnFileOpen() 
{
	// TODO: Add your command handler code here
	CViewingSystemDoc* pDoc = GetDocument();

	char szFilter[] = "Image (*.dat) |*.dat| All Files(*.*)|*.*||";

	CFileDialog dlg(TRUE, NULL, NULL, OFN_HIDEREADONLY, szFilter);

	if (dlg.DoModal() == IDOK)
		pDoc->FileOpen(dlg.GetPathName());

	Invalidate();
}

void CViewingSystemView::OnParallel() 
{
	// TODO: Add your command handler code here

	CViewingSystemDoc* pDoc = GetDocument();

	pDoc->m_viewingMode = PARALLEL;
	pDoc->Rendering();
	Invalidate();
}

void CViewingSystemView::OnPerspective() 
{
	// TODO: Add your command handler code here
	
	CViewingSystemDoc* pDoc = GetDocument();

	pDoc->m_viewingMode = PERSPECTIVE;
	pDoc->Rendering();
	Invalidate();
}

BOOL CViewingSystemView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt) 
{
	// TODO: Add your message handler code here and/or call default

	CViewingSystemDoc* pDoc = GetDocument();

	pDoc->m_eye.z = pDoc->m_eye.z - zDelta/4;

	pDoc->Rendering();

	Invalidate(false);
	
	return CView::OnMouseWheel(nFlags, zDelta, pt);
}

void CViewingSystemView::OnRButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default

	rButtonDownPoint = point;
	isRightButtonDown = true;
	SetCapture();
	
	CView::OnRButtonDown(nFlags, point);
}

void CViewingSystemView::OnRButtonUp(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default

	if (isRightButtonDown)
	{
		CViewingSystemDoc* pDoc = GetDocument();

		pDoc->m_eye.x = pDoc->m_eye.x - (point.x - rButtonDownPoint.x);
		pDoc->m_eye.y = pDoc->m_eye.y + (point.y - rButtonDownPoint.y);

		isRightButtonDown = false;
		ReleaseCapture();
	}
	
	CView::OnRButtonUp(nFlags, point);
}

void CViewingSystemView::OnMouseMove(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default

	CViewingSystemDoc* pDoc = GetDocument();

	if (isRightButtonDown)
	{
		pDoc->m_eye.x = pDoc->m_eye.x - (point.x - rButtonDownPoint.x) ;
		pDoc->m_eye.y = pDoc->m_eye.y + (point.y - rButtonDownPoint.y);

		rButtonDownPoint = point;

		pDoc->Rendering();

		Invalidate(false);
	}
	
	CView::OnMouseMove(nFlags, point);
}

void CViewingSystemView::OnWireframe() 
{
	// TODO: Add your command handler code here

	CViewingSystemDoc* pDoc = GetDocument();

	pDoc->m_shadingMode = WIREFRAME;

	pDoc->Rendering();

	Invalidate(false);
}

void CViewingSystemView::OnFlat() 
{
	// TODO: Add your command handler code here

	CViewingSystemDoc* pDoc = GetDocument();

	pDoc->m_shadingMode = FLAT;

	pDoc->Rendering();

	Invalidate(true);
}

void CViewingSystemView::OnGouraud() 
{
	// TODO: Add your command handler code here
	
	CViewingSystemDoc* pDoc = GetDocument();

	pDoc->m_shadingMode = GOURAUD;

	pDoc->Rendering();

	Invalidate(false);
}

void CViewingSystemView::OnPhong() 
{
	// TODO: Add your command handler code here
	
	CViewingSystemDoc* pDoc = GetDocument();

	pDoc->m_shadingMode = PHONG;

	pDoc->Rendering();

	Invalidate(false);
}


void CViewingSystemView::OnCartoon() 
{
	// TODO: Add your command handler code here
	CViewingSystemDoc* pDoc = GetDocument();

	pDoc->m_shadingMode = CARTOON;

	pDoc->Rendering();

	Invalidate(false);

}

void CViewingSystemView::OnCartoon2() 
{
	// TODO: Add your command handler code here
	CViewingSystemDoc* pDoc = GetDocument();

	pDoc->m_shadingMode = CARTOON2;

	pDoc->Rendering();

	Invalidate(false);

}


BOOL CViewingSystemView::OnEraseBkgnd(CDC* pDC) 
{
	return TRUE;
}

void CViewingSystemView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	// TODO: Add your message handler code here and/or call default
	const float MOV_LIGHT = 100.f;

	switch( nChar )
	{
	case VK_LEFT:
		{
			CViewingSystemDoc* pDoc = GetDocument();
			pDoc->m_light.x -= MOV_LIGHT;
			pDoc->Rendering();
			Invalidate();
		}
		break;

	case VK_RIGHT:
		{
			CViewingSystemDoc* pDoc = GetDocument();
			pDoc->m_light.x += MOV_LIGHT;
			pDoc->Rendering();
			Invalidate();
		}
		break;

	case VK_UP:
		{
			CViewingSystemDoc* pDoc = GetDocument();
			pDoc->m_light.y += MOV_LIGHT;
			pDoc->Rendering();
			Invalidate();
		}
		break;

	case VK_DOWN:
		{
			CViewingSystemDoc* pDoc = GetDocument();
			pDoc->m_light.y -= MOV_LIGHT;
			pDoc->Rendering();
			Invalidate();
		}
		break;
	}
	
	CView::OnKeyDown(nChar, nRepCnt, nFlags);
}


