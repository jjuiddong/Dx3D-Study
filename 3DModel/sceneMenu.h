//-----------------------------------------------------------------------------//
// 2008-03-31  programer: jaejung ¦®(¢Á¬Õ¢Á)¦°
// 
//-----------------------------------------------------------------------------//

#if !defined(__SCENEMENU_H__)
#define __SCENEMENU_H__


#include "scene.h"

class CMenuScene : public CScene
{
public:
	CMenuScene( int id );

protected:
	enum MAIN_BTN { NEWGAME, LOAD, OPTION, MAX_MAINMENU };
	MAIN_BTN m_MainMenu;

public:
	virtual void KeyDown( int Key );
	virtual void ButtonUp( int ButtonId );

};

#endif // __SCENEMENU_H__
