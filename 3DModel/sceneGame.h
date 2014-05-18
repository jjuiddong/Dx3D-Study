//-----------------------------------------------------------------------------//
// 2008-03-25  programer: jaejung ¦®(¢Á¬Õ¢Á)¦°
// 
//-----------------------------------------------------------------------------//

#if !defined(__SCENEGAME_H__)
#define __SCENEGAME_H__

#include "scene.h"


class CTitleScene : public CScene
{
public:
	CTitleScene( int id ):CScene(id) {}

};


class CLoadScene : public CScene
{
public:
	CLoadScene( int id ):CScene(id) {}
protected:
public:
	void KeyDown( int Key )
	{
		switch( Key )
		{
		case VK_ESCAPE:
			m_pParent->SwapScene( 0, 3 );
			break;
		}
	}
};


class CUIImage;
class CGameScene : public CScene
{
public:
	CGameScene( int id ):CScene(id) {}
protected:

public:
	virtual void Render();
	virtual void Animate( int nDelta );
	virtual void KeyDown( int Key );
protected:
	virtual BOOL DefaultMsgProc( SUIMsg *pMsg );
};

#endif // __SCENEGAME_H__
