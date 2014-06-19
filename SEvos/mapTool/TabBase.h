//-----------------------------------------------------------------------------//
// 2009-08-16  programer: jaejung ┏(⊙д⊙)┛
// 
// 예전에 있던 CTabBase 클래스의 기능을 조금 추가했다.
// 
//-----------------------------------------------------------------------------//

#if !defined(AFX_TABBASE_H__18E120CD_4444_40C5_A3B9_73BA3235B279__INCLUDED_)
#define AFX_TABBASE_H__18E120CD_4444_40C5_A3B9_73BA3235B279__INCLUDED_

/*
	TabControl Base Class
	Tab추가, Tab관리
*/

typedef struct tagVIEWLIST
{
	UINT		nTabID;			// Tab ID
	int			nItem;			// TabItem 값
	char		szText[ 32];	// 탭 문자
	CWnd		*pWnd;			// 해당탭의 뷰
	CWnd		*pOtherWnd;		// 해당탭의 뷰
	tagVIEWLIST	*pNext;			// 다음 리스트
	tagVIEWLIST() {}
	tagVIEWLIST( UINT id, int item, char *text, CWnd *pwnd, CWnd *pother ):
	nTabID(id), nItem(item), pWnd(pwnd), pOtherWnd(pother)
		{ if( text ) strcpy_s( szText, sizeof(szText), text ); }
} VIEWLIST, *LPVIEWLIST;

class CTabBase : public CTabCtrl
{
public:
	CTabBase() : m_pHead(NULL),m_pCurView(NULL) {}
	virtual ~CTabBase();
protected:
	LPVIEWLIST	m_pHead;		// List의 첫번째 포인터
	LPVIEWLIST	m_pCurView;		// 현재 출력되고 있는 List
	CWnd		*m_pBaseWnd;
public:
	BOOL		CreateView( CDialog *pwnd, UINT dlgid );
	BOOL		AddView( LPVIEWLIST pList );
	void		ShowView( UINT nTabID );
	LPVIEWLIST	GetRootView() const { return m_pHead; }
	LPVIEWLIST	GetCurrentView() const { return m_pCurView; }
	void		Clear();
protected:
	LPVIEWLIST	FindList( UINT nTabID );
	void		ReleaseList( LPVIEWLIST pList );
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTabBase)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL
protected:
	//{{AFX_MSG(CTabBase)
	afx_msg void OnSelchange(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

#endif // !defined(AFX_TABBASE_H__18E120CD_4444_40C5_A3B9_73BA3235B279__INCLUDED_)
