// SEvosMapToolView.cpp : CSEvosMapToolView 클래스의 구현
//

#include "stdafx.h"
#include "SEvosMapTool.h"

#include "SEvosMapToolDoc.h"
#include "SEvosMapToolView.h"

#include "../Common/global.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CSEvosMapToolView

IMPLEMENT_DYNCREATE(CSEvosMapToolView, CView)

BEGIN_MESSAGE_MAP(CSEvosMapToolView, CView)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_COMMAND(ID_EDIT_BLOCK, &CSEvosMapToolView::OnEditBlock)
	ON_COMMAND(ID_EDIT_GENERATE_NAVI, &CSEvosMapToolView::OnEditGenerateNavi)
	ON_COMMAND(ID_FILE_SAVE, &CSEvosMapToolView::OnFileSave)
	ON_COMMAND(ID_EDIT_RESET, &CSEvosMapToolView::OnEditReset)
	ON_COMMAND(ID_FILE_OPEN, &CSEvosMapToolView::OnFileOpen)
	ON_COMMAND(ID_EDIT_IDSHOW, &CSEvosMapToolView::OnEditIdshow)
END_MESSAGE_MAP()

// CSEvosMapToolView 생성/소멸

CSEvosMapToolView::CSEvosMapToolView()
{
	m_EditMode = EDIT_NORMAL;
	m_pFocusBlock = NULL;
	m_pFocusWall = NULL;
	m_IsRenderID = TRUE;

	m_OffsetPos = CPoint(0,0);
	m_MapSize = CSize(500, 500);

	m_MapEditor.SetOffset(Vector2D(m_OffsetPos.x, m_OffsetPos.y));

}

CSEvosMapToolView::~CSEvosMapToolView()
{
	SAFE_DELETE(m_pFocusWall);
	SAFE_DELETE(m_pFocusBlock);

}


BOOL CSEvosMapToolView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: CREATESTRUCT cs를 수정하여 여기에서
	//  Window 클래스 또는 스타일을 수정합니다.

	return CView::PreCreateWindow(cs);
}

// CSEvosMapToolView 그리기

void CSEvosMapToolView::OnDraw(CDC* pDC)
{
	CSEvosMapToolDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	gdi->StartDrawing(pDC->GetSafeHdc());

	CRect maprect;
	maprect.SetRect(m_OffsetPos, CPoint(m_MapSize.cx + m_OffsetPos.x, m_MapSize.cy + m_OffsetPos.y));
	Rectangle(pDC->GetSafeHdc(), maprect.left, maprect.top, maprect.right,maprect.bottom);

	if (m_pFocusWall) 
	{
		m_pFocusWall->Render();
	}
	if (m_pFocusBlock)
	{
		m_pFocusBlock->Render();
	}

	m_MapEditor.Render();

	gdi->StopDrawing(pDC->GetSafeHdc());
}


#ifdef _DEBUG
void CSEvosMapToolView::AssertValid() const
{
	CView::AssertValid();
}

void CSEvosMapToolView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CSEvosMapToolDoc* CSEvosMapToolView::GetDocument() const // 디버그되지 않은 버전은 인라인으로 지정됩니다.
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CSEvosMapToolDoc)));
	return (CSEvosMapToolDoc*)m_pDocument;
}
#endif //_DEBUG


// CSEvosMapToolView 메시지 처리기
void CSEvosMapToolView::OnLButtonDown(UINT nFlags, CPoint point)
{
	// 아직 아무일 없음

	CView::OnLButtonDown(nFlags, point);
}


//
//
void CSEvosMapToolView::OnLButtonUp(UINT nFlags, CPoint point)
{
	if (EDIT_NORMAL == GetEditMode()) return;

	Vector2D vTo(point.x, point.y);
//	vTo -= Vector2D(m_OffsetPos.x, m_OffsetPos.y);

	switch (GetEditMode())
	{
	case EDIT_NORMAL:
		break;

	case EDIT_BLOCK_FROM:
		m_pFocusWall = new Wall2D();
		m_pFocusWall->SetFrom(vTo);
		m_pFocusWall->SetTo(vTo);
		SetEditMode(EDIT_BLOCK_TO);
		break;

	case EDIT_BLOCK_TO:
		{
			m_pFocusWall->SetTo(vTo);

			if (!m_pFocusBlock)
			{
				m_pFocusBlock = new CBlock();
			}

			m_pFocusBlock->AddWall(m_pFocusWall);

			m_pFocusWall = new Wall2D(vTo, vTo);

			SetEditMode(EDIT_BLOCK_TO);
		}
		break;
	}

	InvalidateRect(NULL);
	CView::OnLButtonUp(nFlags, point);
}


//
//
void CSEvosMapToolView::OnMouseMove(UINT nFlags, CPoint point)
{
	if (EDIT_NORMAL == GetEditMode()) return;

	if (m_pFocusWall) 
	{
		Vector2D vTo(point.x, point.y);
//		vTo -= Vector2D(m_OffsetPos.x, m_OffsetPos.y);
		m_pFocusWall->SetTo(vTo);
	}

	InvalidateRect(NULL);
	CView::OnMouseMove(nFlags, point);
}


//
//
void CSEvosMapToolView::SetEditMode(EDIT_MODE nextMode)
{
	m_EditMode = nextMode;

	switch (nextMode)
	{
	case EDIT_NORMAL:
		{
			if (m_pFocusWall)
			{
				delete m_pFocusWall;
				m_pFocusWall = NULL;
				InvalidateRect(NULL);
			}
		}
		break;
	}
}


// Block의 마지막 포인트를 처음과 연결시켜 Wall을 생성해서 추가시킨다.
void CSEvosMapToolView::SetBlockEndPointToStartPoint()
{
	if (!m_pFocusBlock) return;

	Wall2D *pbegin = m_pFocusBlock->GetBeginWall();
	Wall2D *pend = m_pFocusBlock->GetEndWall();

	if (pbegin == NULL || pend == NULL) return;
	if (pbegin == pend) return;

	Wall2D *wall = new Wall2D(pbegin->From(), pend->To());
	m_pFocusBlock->AddWall(wall);
}


// 
//
BOOL CSEvosMapToolView::PreTranslateMessage(MSG* pMsg)
{
	switch(pMsg->message)
	{
	case WM_KEYDOWN:
		{
			if (pMsg->wParam == VK_ESCAPE)
			{
				if (GetEditMode() == EDIT_BLOCK_TO)
				{
					SetBlockEndPointToStartPoint();
					m_MapEditor.AddBlock(m_pFocusBlock);
					m_pFocusBlock = NULL;
				}

				SetEditMode(EDIT_NORMAL);
			}
		}
		break;
	}

	return CView::PreTranslateMessage(pMsg);
}


//
//
void CSEvosMapToolView::OnEditBlock()
{
	m_pFocusWall = NULL;
	SetEditMode(EDIT_BLOCK_FROM);

}

void CSEvosMapToolView::OnEditGenerateNavi()
{
	m_MapEditor.GenerateNaviMap();
	InvalidateRect(NULL);
}

void CSEvosMapToolView::OnFileSave()
{
	CFileDialog Dlg(FALSE, "map", "mapfile", OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, "map File (*.txt)|*.txt|All Files (*.*)|*.*||");	
	if (IDOK == Dlg.DoModal())
	{
		m_MapEditor.Write((char*)(LPCTSTR)Dlg.GetFileName());
		InvalidateRect(NULL);
	}
}

void CSEvosMapToolView::OnEditReset()
{
	m_MapEditor.Reset();
	InvalidateRect(NULL);
}

void CSEvosMapToolView::OnFileOpen()
{
	CFileDialog Dlg(TRUE, "map", "MapFile", OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, "map File (*.txt)|*.txt|All Files (*.*)|*.*||");	
	if (IDOK == Dlg.DoModal())
	{
		m_MapEditor.Read((char*)(LPCTSTR)Dlg.GetFileName());
		InvalidateRect(NULL);
	}
}

void CSEvosMapToolView::OnEditIdshow()
{
	m_IsRenderID = !m_IsRenderID;
	m_MapEditor.SetIsShowID(m_IsRenderID);
	InvalidateRect(NULL);
}
