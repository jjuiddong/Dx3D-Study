
#include "global.h"
#include "scenemenu.h"
#include "uicontrol.h"


CMenuScene::CMenuScene( int id )
: CScene(id), m_MainMenu(NEWGAME)
{

}


//-----------------------------------------------------------------------------//
// 키보드 입력 처리
//-----------------------------------------------------------------------------//
void CMenuScene::KeyDown( int Key )
{
	const int MAINBTN_H = 50;

	switch( Key )
	{
	case VK_ESCAPE:
		::PostQuitMessage( 0 );
		break;

	case VK_DOWN:
		{
			if( MAX_MAINMENU <= m_MainMenu+1 ) return;

			m_MainMenu = (MAIN_BTN)(m_MainMenu + 1);
			CUIControl *pui = FindUIControl( 111 );
			RECT rt;
			pui->GetRect( &rt );
			rt.top += MAINBTN_H;
			rt.bottom += MAINBTN_H;
			pui->SetRect( &rt );
		}
		break;

	case VK_UP:
		{
			if( 0 > m_MainMenu-1 ) return;

			m_MainMenu = (MAIN_BTN)(m_MainMenu - 1);
			CUIControl *pui = FindUIControl( 111 );
			RECT rt;
			pui->GetRect( &rt );
			rt.top -= MAINBTN_H;
			rt.bottom -= MAINBTN_H;
			pui->SetRect( &rt );
		}
		break;

	case VK_RETURN:
		{
			switch( m_MainMenu )
			{
			case NEWGAME: m_pParent->SwapScene( 0, 4 ); break;
			case LOAD: m_pParent->SwapScene( 0, 5 ); break;
			case OPTION:
				break;
			}
		}
		break;

	}
	

}


//-----------------------------------------------------------------------------//
// 버튼메세지 처리
//-----------------------------------------------------------------------------//
void CMenuScene::ButtonUp( int ButtonId )
{
	switch( ButtonId )
	{
	case 103:
		{
			m_pParent->SwapScene( 0, 4 );
		}
		break;
	}

}
