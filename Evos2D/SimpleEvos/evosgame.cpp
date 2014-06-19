
#include "stdafx.h"
#include "../Common/global.h"
#include "../Common/evos/evosmap.h"
#include "messagesystem.h"
#include "unit.h"
#include "pathfindmanager.h"
#include "unitcontrol.h"
#include "SimpleEvos.h"
#include "luascript.h"
#include "dbg.h"
#include "global.h"
#include "evosgame.h"


using namespace std;

//------------------------------------------------------------------------
// 
// [2011/1/13 jjuiddong]
//------------------------------------------------------------------------
CEvosGame::CEvosGame()
{
	m_pMap = new CEvosMap();
	m_pMap->SetIsShowID(false);

	m_pPathFinderManager = new CPathFindManager(1000);
	m_pUnitControl = new CUnitControl();
	m_pLuaScript = new CLuaScript();

}


//------------------------------------------------------------------------
// 
// [2011/1/13 jjuiddong]
//------------------------------------------------------------------------
CEvosGame::~CEvosGame() 
{
	Clear();

}


//------------------------------------------------------------------------
// 클래스 초기화
// [2011/1/13 jjuiddong]
//------------------------------------------------------------------------
void CEvosGame::Init()
{
	m_pMap->Read("test3.txt");

	// unit1
	CUnit *pu1 = new CUnit(Vector2D(70, 80));
	m_pUnitControl->AddUnit(pu1);

	// unit2
	CUnit *pu2 = new CUnit(Vector2D(270, 80));
//	m_pUnitControl->AddUnit(pu2);
}


//------------------------------------------------------------------------
// 
// [2011/1/13 jjuiddong]
//------------------------------------------------------------------------
void CEvosGame::Update( const float timeDelta )
{
	m_pMap->Update(timeDelta);
	m_pUnitControl->Update(timeDelta);
	m_pPathFinderManager->Update(timeDelta);

	CMessageSystem::Get()->Update(timeDelta);
}


//------------------------------------------------------------------------
// 출력
// [2011/1/13 jjuiddong]
//------------------------------------------------------------------------
void CEvosGame::Render(HDC hdc)
{
	gdi->StartDrawing(hdc);

	m_pMap->Render();
	m_pUnitControl->Render();

	gdi->StopDrawing(hdc);
}


//------------------------------------------------------------------------
// 
// [2011/1/13 jjuiddong]
//------------------------------------------------------------------------
void CEvosGame::Clear()
{
	SAFE_DELETE(m_pMap);
	CMessageSystem::Release();
	SAFE_DELETE(m_pUnitControl);
	SAFE_DELETE(m_pLuaScript);
	SAFE_DELETE(m_pPathFinderManager);
}


//------------------------------------------------------------------------
// 
// [2011/1/14 jjuiddong]
//------------------------------------------------------------------------
void CEvosGame::KeyDownEvent( int key )
{
	switch (key)
	{
	case 'R':
		{
			static bool isShow = false;
			m_pMap->SetIsShowID(isShow);
			isShow = !isShow;
			InvalidateRect(g_hWnd, NULL, TRUE);
		}
		break;
	}

}


//------------------------------------------------------------------------
// 
// [2011/1/14 jjuiddong]
//------------------------------------------------------------------------
void CEvosGame::MouseEvent( int windowEvent, POINT pos)
{
	switch (windowEvent)
	{
	case WM_RBUTTONDOWN:
		{
			CUnit *punit = m_pUnitControl->GetUnit(0);
			if (punit)
			{
				punit->Move(Vector2D(pos.x, pos.y));
			}
		}
		break;
	}

}

