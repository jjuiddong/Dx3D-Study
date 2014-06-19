
#include "stdafx.h"
#include "sceneGame.h"
#include "control.h"
#include "uicontrol.h"

float g_flag1 = 0.f;

/*
void CallFunc( int nFuncID, ns_script::CProcessor *pProcessor )
{
	switch( nFuncID )
	{
	case ns_script::LoadPointer:
		{
			CCharacter *pc = (CCharacter*)pProcessor->GetArgumentClass( 0 );
			int id = (int)pProcessor->GetArgumentFloat( 1 );
			switch( id )
			{
			case 10000:
				pProcessor->SetReturnValue( &g_flag1 );
				break;

			// CCharacter::x
			case 100:
				{
					Matrix44 *pmat = pc->GetWorldTM();
					pProcessor->SetReturnValue( &pmat->_41 );
				}
				break;
			// CCharacter::y
			case 101:
				{
					Matrix44 *pmat = pc->GetWorldTM();
					pProcessor->SetReturnValue( &pmat->_43 );
				}				
				break;
			}
		}
		break;

	// CreateHero
	case 1010:
		{
			char *name = pProcessor->GetArgumentStr( 0 );
			char *model = pProcessor->GetArgumentStr( 1 );
			char *ani = pProcessor->GetArgumentStr( 2 );
			char *weapon = pProcessor->GetArgumentStr( 3 );
			char *combo = pProcessor->GetArgumentStr( 4 );

			CCharacter *pch = g_Control.AddCharacter( CHAR_HERO, name, model, ani, weapon, combo );
			pProcessor->SetReturnValue( pch );
		}
		break;

	// @@CCharacter::SetPos(x,y)
	case 111:
		{
			CCharacter *pc = (CCharacter*)pProcessor->GetArgumentClass( 0 );
			float x = pProcessor->GetArgumentFloat( 1 );
			float z = pProcessor->GetArgumentFloat( 2 );
			float y = 0;
			if( g_Control.GetTerrain() )
				y = g_Control.GetTerrain()->GetHeight( x, z );

			pc->SetPos( Vector3(x,y,z) );
		}
		break;

	// CreateEnemy
	case 1011:
		{
			char *name = pProcessor->GetArgumentStr( 0 );
			char *model = pProcessor->GetArgumentStr( 1 );
			char *ani = pProcessor->GetArgumentStr( 2 );
			char *weapon = pProcessor->GetArgumentStr( 3 );
			char *combo = pProcessor->GetArgumentStr( 4 );
			char *ai = pProcessor->GetArgumentStr( 5 );

			CCharacter *pch = g_Control.AddCharacter( CHAR_ENEMY, name, model, ani, weapon, combo, ai );
			pProcessor->SetReturnValue( pch );
		}
		break;

	// GetAI
	case 1021:
		g_Dbg.Console( "%s\n", pProcessor->GetArgumentStr(0) );
		break;

	// SetTerrain
	case 1031:
		{
//			g_Control.LoadTerrain( pProcessor->GetArgumentStr(0) );
			g_Control.LoadTerrain( "SaveMap\\ax.map" );
		}
		break;
	}
}
/**/

//-----------------------------------------------------------------------------//
// 메세지 처리
//-----------------------------------------------------------------------------//
BOOL CGameScene::DefaultMsgProc( SUIMsg *pMsg )
{
	CScene::DefaultMsgProc( pMsg );

	switch( pMsg->msg )
	{
	case UIMSG_SWAPPED:
		{
//			if( !g_Scr.Execute("data//script//ScenarioScript.txt", CallFunc, "실행 인자값") )
			{
//				return FALSE;
			}
		}
		break;
	}

	return TRUE;
}

// Cross Line 출력
inline void DrawXYZ()
{
	g_pDevice->SetRenderState( D3DRS_LIGHTING, FALSE );

	g_pDevice->SetTransform( D3DTS_WORLD, &g_matIdentity ); // identity matrix

	const int cross_size = 6;
	SVtxDiff crossline[ cross_size];
	// x axis
	crossline[ 0].v = Vector3( 0.f, 0.f, 0.f );
	crossline[ 1].v = Vector3( 100.f, 0.f, 0.f );
	crossline[ 0].c = D3DXCOLOR( 1, 0, 0, 0 );
	crossline[ 1].c = D3DXCOLOR( 1, 0, 0, 0 );
	// y axis
	crossline[ 2].v = Vector3( 0.f, 0.f, 0.f );
	crossline[ 3].v = Vector3( 0.f, 100.f, 0.f );
	crossline[ 2].c = D3DXCOLOR( 0, 1, 0, 0 );
	crossline[ 3].c = D3DXCOLOR( 0, 1, 0, 0 );
	// z axis
	crossline[ 4].v = Vector3( 0.f, 0.f, 0.f );
	crossline[ 5].v = Vector3( 0.f, 0.f, 100.f );
	crossline[ 4].c = D3DXCOLOR( 0, 0, 1, 0 );
	crossline[ 5].c = D3DXCOLOR( 0, 0, 1, 0 );
	g_pDevice->SetFVF( SVtxDiff::FVF );
	g_pDevice->DrawPrimitiveUP( D3DPT_LINELIST, cross_size/2, crossline, sizeof(SVtxDiff) );	

	g_pDevice->SetRenderState( D3DRS_LIGHTING, TRUE );
}


void CGameScene::Render()
{
	DrawXYZ();
	g_Control.Render();
	CScene::Render();
}


void CGameScene::Animate( int nDelta )
{
	CScene::Animate( nDelta );
	g_Control.Animate( nDelta );

	static int oldtime = 0;
	oldtime += nDelta;
	if( 30 > oldtime ) return;
	oldtime = 0;

	// 테스트 코드
//	static float right = 0;
//	right += ((float)nDelta * 0.01f);
//	RECT rt = m_RtLife;
//	rt.right = m_RtLife.right - right;
//	m_pHeroLife->SetRect( &rt );
/*
	// Key입력
	int key = 0;
	if( ::GetAsyncKeyState('A') )
	{
		key |= KEY_B;
	}
	if( ::GetAsyncKeyState('D') )
	{
		key |= KEY_F;
	}
	if( ::GetAsyncKeyState('W') )
	{
		key |= KEY_U;
	}
	if( ::GetAsyncKeyState('S') )
	{
		key |= KEY_D;
	}
	if( ::GetAsyncKeyState('J') )
	{
		key |= KEY_A;
	}
	if( ::GetAsyncKeyState('U') )
	{
		key |= KEY_S;
	}
	if( ::GetAsyncKeyState('P') )
	{
		g_test += 10;
	}
	if( ::GetAsyncKeyState('O') )
	{
		g_test -= 10;
	}

	if( key )
	{
		g_Control.Command( SMsg(MSG_KEYDOWN, NULL, key, g_CurTime) );
	}
/**/

}


//-----------------------------------------------------------------------------//
// 키보드 입력 처리
//-----------------------------------------------------------------------------//
void CGameScene::KeyDown( int Key )
{
	switch( Key )
	{
	case VK_ESCAPE:
		::PostQuitMessage( 0 );
		break;
	case VK_RETURN:
		{
			static BOOL b = TRUE;
			g_pDevice->SetRenderState( D3DRS_FILLMODE, (b)? D3DFILL_WIREFRAME:D3DFILL_SOLID );
			b = !b;
		}
		break;
	}
}