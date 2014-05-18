
#include "global.h"
#include "scene.h"
#include "sceneGame.h"
#include "sceneMenu.h"
#include "ui//scenelist.txt"
#include "fileloader.h"
#include "uicontrol.h"


const int MAX_SCENE = 16;
static CScene *s_Scene[ MAX_SCENE];	// 모든 씬을 저장한다.
static int s_SceneMap[ 256];	// class id -> scene index
static int s_SceneCount = 0;

CScene::CScene( int Id ) : m_Id(Id),m_pCurScene(NULL), m_pParent(NULL), m_pFocusCtrl(NULL)
{
	static BOOL bInit = FALSE;
	if( !bInit )
	{
		ZeroMemory( s_SceneMap, sizeof(s_SceneMap) );
		bInit = TRUE;
	}

}


CScene::~CScene() 
{

}


//-----------------------------------------------------------------------------//
// 씬 클래스 생성
// 스크립트에서 UI정보를 읽어들여 씬클래스를 생성한다. 
// pFileName : UI 스크립트 파일
// pParent : 부모 씬
//-----------------------------------------------------------------------------//
BOOL CScene::Create( char *pFileName, CScene *pParent ) // pParent=NULL
{
	SScene *pscene = CFileLoader::LoadUI( pFileName );
	Create( pscene, pParent );

	return TRUE;
}


//-----------------------------------------------------------------------------//
// 씬 클래스 생성
//-----------------------------------------------------------------------------//
BOOL CScene::Create( SScene *pScene, CScene *pParent ) // pParent=NULL
{
	m_pParent = pParent;
	RegisterScene( this );

	// 콘트롤 생성
	for( int i=0; i < pScene->csize; ++i )
	{
		SControl *pc = &pScene->pc[ i];
		AddControl( pc );
	}

	// 자식 씬 생성
	for( i=0; i < pScene->ssize; ++i )
	{
		SScene *ps = &pScene->ps[ i];
		CScene *s = FindClass( ps->id );
		if( !s ) s = new CScene( ps->id );
		s->Create( ps, this );
		AddChildScene( s );
	}
	return TRUE;
}



//-----------------------------------------------------------------------------//
// 출력
//-----------------------------------------------------------------------------//
void CScene::Render()
{
	if( m_pCurScene )
	{
		m_pCurScene->Render();
	}
	else
	{
		std::list< CUIControl* >::iterator it = m_Control.begin();
		while( m_Control.end() != it )
		{
			CUIControl *pctrl = *it++;
			pctrl->Render();
		}
	}

}


//-----------------------------------------------------------------------------//
// 에니메이션 처리 + 씬메세지 처리
//-----------------------------------------------------------------------------//
void CScene::Animate( int nDelta )
{
	if( m_pCurScene )
	{
		m_pCurScene->Animate( nDelta );
		m_pCurScene->UIMsgProc();
	}
	else
	{
		
	}
}


//-----------------------------------------------------------------------------//
// 윈도우 메세지만 처리한다.
//-----------------------------------------------------------------------------//
void CScene::MessageProc( MSG *pMsg )
{
	if( m_pCurScene )
	{
		m_pCurScene->MessageProc( pMsg );
	}
	else
	{
		if( WM_KEYFIRST <= pMsg->message && WM_KEYLAST > pMsg->message )
		{
//			KeyDown( pMsg->wParam );
		}
		if( WM_MOUSEFIRST <= pMsg->message && WM_MOUSELAST > pMsg->message )
		{
			POINT pt;
			pt.x = LOWORD( pMsg->lParam );
			pt.y = HIWORD( pMsg->lParam );

			if( (WM_LBUTTONDOWN == pMsg->message) || (WM_LBUTTONUP == pMsg->message) )
			{
				CUIControl *pui = NULL;
				std::list< CUIControl* >::reverse_iterator it = m_Control.rbegin();
				while( m_Control.rend() != it )
				{
					CUIControl *p = *it++;
					if( p->GetVisible() && p->InRect(&pt) )
					{
						pui = p;
						break;
					}
				}

				if( pui && (WM_LBUTTONDOWN == pMsg->message) )
				{
					pui->SendMessage( &SUIMsg(pui->GetId(), UIMSG_LCLICK, 0, 0) );
					m_pFocusCtrl = pui;
				}
				else if( (WM_LBUTTONUP == pMsg->message) )
				{
					if( pui && (m_pFocusCtrl == pui) )
					{
						pui->SendMessage( &SUIMsg(pui->GetId(), UIMSG_LCLICKUP, 0, 0 ) );
						ButtonUp( pui->GetId() );
					}
					else 
					{
						if( pui ) pui->SendMessage( &SUIMsg(pui->GetId(), UIMSG_LCLICKUP, 0, 0 ) );
						if( m_pFocusCtrl ) m_pFocusCtrl->SendMessage( &SUIMsg(m_pFocusCtrl->GetId(), UIMSG_LCLICKUP, 0, 0 ) );
					}

					m_pFocusCtrl = NULL;
				}
			}
		}
		else if( WM_KEYDOWN == pMsg->message )
		{
			KeyDown( pMsg->wParam );
		}
	}
}


//-----------------------------------------------------------------------------//
// UI 메세지를 처리한다.
//-----------------------------------------------------------------------------//
BOOL CScene::UIMsgProc()
{
	const int MAX_PROC = 5;
	int count = 0;

	while( !m_MsgQ.empty() && MAX_PROC > count++ )
	{
		DefaultMsgProc( &m_MsgQ.front() );
		m_MsgQ.pop();
	}

	return TRUE;
}


//-----------------------------------------------------------------------------//
// 씬메세지를 처리.
//-----------------------------------------------------------------------------//
BOOL CScene::DefaultMsgProc( SUIMsg *pMsg )
{
	switch( pMsg->msg )
	{
	case UIMSG_COMMAND:
		{
			CUIControl *pui = FindUIControl( pMsg->classid );
			if( !pui ) break;

			switch( pMsg->wparam )
			{
			case UICMD_SETIMAGEINDEX:
					pui->SendMessage( &SUIMsg(pMsg->classid, UIMSG_SETINDEX, pMsg->lparam, 0 ) );
					break;
			}
		}
		break;

	// 콘트롤에서만 처리한다.
	case UIMSG_SETVISIBLE:
		{
			CUIControl *pui = FindUIControl( pMsg->classid );
			if( !pui ) break;
			pui->SetVisible( pMsg->wparam );
		}
		break;
	}

	return TRUE;
}


//-----------------------------------------------------------------------------//
// 키보드 메세지 처리
//-----------------------------------------------------------------------------//
void CScene::KeyDown( int Key )
{
	// 자식 씬에서 처리된다.
}


//-----------------------------------------------------------------------------//
// 버튼 메세지 처리
//-----------------------------------------------------------------------------//
void CScene::ButtonDown( int ButtonId )
{
	// 자식 씬에서 처리된다.
}


//-----------------------------------------------------------------------------//
// 버튼 메세지 처리
//-----------------------------------------------------------------------------//
void CScene::ButtonUp( int ButtonId )
{
	// 자식 씬에서 처리된다.
}


//-----------------------------------------------------------------------------//
// 컨트롤 추가
//-----------------------------------------------------------------------------//
void CScene::AddControl( SControl *pCtrl )
{
	CUIControl *p = NULL;
	switch( pCtrl->type )
	{
	case UI_IMAGE: p = new CUIImage(); break;
	case UI_BUTTON: p = new CUIButton(); break;
	}
	if( !p ) return;

	p->Create( pCtrl );
	m_Control.push_back( p );
}


//-----------------------------------------------------------------------------//
// 씬이 제거될때 호출된다.
// 내부에 동적으로 생성된 콘트롤과 씬들을 제거한다.
//-----------------------------------------------------------------------------//
void CScene::Destroy()
{
	// UIControl 제거
	std::list< CUIControl* >::iterator it = m_Control.begin();
	while( m_Control.end() != it )
	{
		CUIControl *pui = *it++;
		SAFE_DELETE( pui );
	}

	// 자식씬 Destroy
	std::list< CScene* >::iterator i = m_ChildScene.begin();
	while( m_ChildScene.end() != i )
	{
		CScene *ps = *i++;
		ps->Destroy();
		SAFE_DELETE( ps );
	}

}


//-----------------------------------------------------------------------------//
// 받은 메세지를 처리한다.
//-----------------------------------------------------------------------------//
void CScene::SendMessage( SUIMsg *pMsg )
{
	if( m_pCurScene )
	{
		m_pCurScene->SendMessage( pMsg );
	}
	else
	{
		DefaultMsgProc( pMsg );
	}
}


//-----------------------------------------------------------------------------//
// 받은 메세지는 큐에 저장한후 UIMsgProc()에서 처리된다.
//-----------------------------------------------------------------------------//
void CScene::PostMessage( SUIMsg *pMsg )
{
	if( m_pCurScene )
	{
		m_pCurScene->PostMessage( pMsg );
	}
	else
	{
		m_MsgQ.push( *pMsg );
	}
}


//-----------------------------------------------------------------------------//
// 화면에 보여줄 자식씬을 교체한다.
// CurSceneId: 현재 씬 (설정하지 않아도 된다.)
// NextSceneId: 보여줄 씬
//-----------------------------------------------------------------------------//
void CScene::SwapScene( int CurSceneId, int NextSceneId )
{
//	if( m_pCurScene )
	{
//		m_pCurScene->SwapScene( CurSceneId, NextSceneId );
	}
//	else
	{
		CScene *p = FindClass( NextSceneId );
		if( !p ) return;
		p->SendMessage( &SUIMsg(p->m_Id, UIMSG_SWAPPED, 0, 0) );
		m_pCurScene = p;
	}

}


//-----------------------------------------------------------------------------//
// 테이블에서 씬을 찾는다.
//-----------------------------------------------------------------------------//
CScene* CScene::FindClass( int Id )
{
	const int size = sizeof(g_SceneTable) / sizeof(CScene*);
	for( int i=0; i < size; ++i )
	{
		if( g_SceneTable[ i]->GetID() == Id )
			return g_SceneTable[ i];
	}
	int idx = s_SceneMap[ Id];
	if( 0 >= idx ) return NULL;
	return s_Scene[ idx];
}


//-----------------------------------------------------------------------------//
// 씬에 등록된 UIControl만 검사한다.
//-----------------------------------------------------------------------------//
CUIControl* CScene::FindUIControl( int Id )
{
	std::list< CUIControl* >::iterator it = m_Control.begin();
	while( m_Control.end() != it )
	{
		CUIControl *pui = *it++;
		if( pui->GetId() == Id )
			return pui;
	}
	return NULL;
}


//-----------------------------------------------------------------------------//
// 자식씬에 추가
//-----------------------------------------------------------------------------//
BOOL CScene::AddChildScene( CScene *pScene )
{
	m_ChildScene.push_back( pScene );
	return TRUE;
}


//-----------------------------------------------------------------------------//
// 씬을 빠르게 접근하기위해서 임시 메모리에 저장한다
// s_Scene[] 1번 인덱스부터 씬이 저장된다. 이는 s_SceneMap[] 값이 0일때 설정되지
// 않았다는것을 나타내기 위해서다.
//-----------------------------------------------------------------------------//
void CScene::RegisterScene( CScene *pScene )
{
	s_SceneMap[ pScene->GetID()] = s_SceneCount+1;
	s_Scene[ s_SceneCount+1] = pScene;
	++s_SceneCount;
}
