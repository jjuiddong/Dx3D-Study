
#include "stdafx.h"
#include "Global.h"
#include "TabBase.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BEGIN_MESSAGE_MAP(CTabBase, CTabCtrl)
	//{{AFX_MSG_MAP(CTabBase)
	ON_NOTIFY_REFLECT(TCN_SELCHANGE, OnSelchange)
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTabBase message handlers


CTabBase::~CTabBase()
{ 
	ReleaseList( m_pHead ); 
	m_pCurView = NULL; 
	m_pBaseWnd->DestroyWindow();
	SAFE_DELETE(m_pBaseWnd);
	
}


//-----------------------------------------------------------------------------//
// Name: 
// Desc: View 추가
// Date: (이재정 2002-07-04 14:7)
// Update : 
//-----------------------------------------------------------------------------//
BOOL CTabBase::AddView( LPVIEWLIST pList )
{
	LPVIEWLIST pAddList = new VIEWLIST;
	*pAddList = *pList;
	pAddList->pNext = NULL;

	if( !m_pHead )
	{
		// 리스트가 비었다면 처음에 추가
		m_pHead = pAddList;
	}
	else
	{
		// 리스트 마지막에 추가
		LPVIEWLIST pTemp = m_pHead;
		while( pTemp->pNext )		
			pTemp = pTemp->pNext;
		pTemp->pNext = pAddList;
	}

	// Tab 추가
	InsertItem( TCIF_TEXT | TCIF_PARAM | TCIF_IMAGE, pList->nTabID, pList->szText, 0, pList->nTabID );
	return TRUE;
}


//-----------------------------------------------------------------------------//
// Name: 
// Desc: 검색
// Date: (이재정 2002-07-05 13:21)
// Update : 
//-----------------------------------------------------------------------------//
LPVIEWLIST CTabBase::FindList( UINT nTabID )
{
	LPVIEWLIST pList = m_pHead;
	while( pList )
	{
		if( nTabID == pList->nTabID )
			return pList;
		pList = pList->pNext;
	}
	return NULL;
}


//-----------------------------------------------------------------------------//
// Name: 
// Desc: nTabID 의 Tab으로 바뀐다
// Date: (이재정 2002-07-05 13:19)
// Update : 
//-----------------------------------------------------------------------------//
void CTabBase::ShowView( UINT nTabID )
{
	LPVIEWLIST pList = FindList( nTabID );
	if( !pList ) return;

	if( m_pCurView )
	{
		if( m_pCurView->pWnd )
			m_pCurView->pWnd->ShowWindow( SW_HIDE );
		if( m_pCurView->pOtherWnd )
			m_pCurView->pOtherWnd->ShowWindow( SW_HIDE );
	}

	if( pList->pWnd )
	{
		if( pList->pWnd )
			pList->pWnd->ShowWindow( SW_SHOW );
		if( pList->pOtherWnd )
			pList->pOtherWnd->ShowWindow( SW_SHOW );
	}
	m_pCurView = pList;

}


//-----------------------------------------------------------------------------//
// Name: 
// Desc: 리스트 삭제 (재귀로 삭제한다.)
// Date: (이재정 2002-07-04 14:20)
// Update : 
//-----------------------------------------------------------------------------//
void CTabBase::ReleaseList( LPVIEWLIST pList )
{
	if( !pList ) return;
	if( pList->pNext )
		ReleaseList( pList->pNext );
	if( pList->pWnd ) delete pList->pWnd;
	if( pList->pOtherWnd ) delete pList->pOtherWnd;
	delete pList;
}


//-----------------------------------------------------------------------------//
// Name: Tab 눌렀을때..
// Desc: 
// Date: (이재정 2003-09-19 17:32)
// Update: 
//-----------------------------------------------------------------------------//
void CTabBase::OnSelchange(NMHDR* pNMHDR, LRESULT* pResult)
{
/*
	// 현재커서의 Tab을 얻는다.
	POINT pos;
	GetCursorPos( &pos );
	ScreenToClient( &pos );
	TCHITTESTINFO info;
	info.flags = TCHT_ONITEM;
	info.pt = pos;
	int nItem = HitTest( &info );
	if( nItem < 0 )
		return;

	TCITEM TabItem;
	TabItem.mask = TCIF_PARAM;
	GetItem( nItem, &TabItem );
	ShowView( TabItem.lParam );
/**/

	int nItem = GetCurSel();
	if( nItem < 0 )
		return;

	TCITEM TabItem;
	TabItem.mask = TCIF_PARAM;
	GetItem( nItem, &TabItem );
	ShowView( TabItem.lParam );

	*pResult = 0;
}

void CTabBase::Clear()
{
	ReleaseList( m_pHead );
	m_pHead = NULL;
	m_pCurView = NULL;
}


BOOL CTabBase::PreTranslateMessage(MSG* pMsg) 
{
	// TODO: Add your specialized code here and/or call the base class
	switch( pMsg->message )
	{
	case WM_KEYDOWN:
		{
			if( VK_TAB == pMsg->wParam )
			{
				if( (CWnd*)this == GetFocus() )
				{
					if( m_pCurView->pWnd )
						::SetFocus( m_pCurView->pWnd->m_hWnd );
				}
				else if( (CWnd*)m_pCurView->pWnd == GetFocus() )
					SetFocus();
				return TRUE;
			}
			if( VK_CONTROL == pMsg->wParam )
			{
			}
		}
		break;
	}

	return CTabCtrl::PreTranslateMessage(pMsg);
}

int CTabBase::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CTabCtrl::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	m_pBaseWnd = new CWnd;
	m_pBaseWnd->Create( NULL, NULL, WS_VISIBLE | WS_CHILD, 
		CRect(5,25,lpCreateStruct->cx, lpCreateStruct->cy), this, 0 );
	
	return 0;
}


//-----------------------------------------------------------------------------//
// 탭 다이얼로그 내부의 뷰의 자식으로 뷰를 생성한다.
//-----------------------------------------------------------------------------//
BOOL CTabBase::CreateView( CDialog *pwnd, UINT dlgid )
{
	if( !m_pBaseWnd ) return FALSE;
	pwnd->Create( dlgid, m_pBaseWnd );
	return TRUE;
}
