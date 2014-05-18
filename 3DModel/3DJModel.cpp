
#include "global.h"
#include "fileloader.h"
#include "model.h"
#include "tree.h"
#include "math.h"
#include "camera.h"
#include "sound.h"
#include "control.h"
#include "character.h"
#include "ai_object.h"
#include "drawtext.h"
#include "scene.h"


IDirect3DDevice9 *g_pDevice;
const int Width = 800;
const int Height = 600;
//const int Width = 300;
//const int Height = 300;

const float Inteval = 1.f;
HWND g_hWnd;
D3DXMATRIXA16	g_matView;
D3DXMATRIXA16	g_matProj;
float g_Distance = 100.f;
D3DXVECTOR3 g_vEyePt( 0.0f, 100.0f, g_Distance );
D3DXVECTOR3 g_vLookatPt( 0.0f, 0.0f, 0.0f );
D3DXVECTOR3 g_vUpVec( 0.0f, 1.0f, 0.0f );
Quaternion g_Rotate(0,0,0,0);
D3DLIGHT9 g_Light;

CDrawText g_DrawText;
CScene g_Scene( 0 );
CCamera g_Camera;
BOOL g_bLoop = TRUE;
float g_angle = 0.f;


//-----------------------------------------------------------------------------//
// 프로그램시작전 초기화
//-----------------------------------------------------------------------------//
BOOL Setup()
{
	g_pDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_CCW );// 기본컬링, CCW
    g_pDevice->SetRenderState( D3DRS_ZENABLE, TRUE );// Z버퍼기능을 켠다.
//	g_pDevice->SetRenderState( D3DRS_LIGHTING, TRUE );
//	g_pDevice->SetRenderState( D3DRS_FILLMODE, D3DFILL_WIREFRAME );

    ZeroMemory( &g_Light, sizeof(g_Light) );
    g_Light.Type = D3DLIGHT_DIRECTIONAL;
    g_Light.Direction = D3DXVECTOR3( 0.0f, -1.f, 0.f );
    g_Light.Diffuse.r = 1.f;
	g_Light.Diffuse.g = g_Light.Diffuse.b = 1.f;
	g_Light.Range = 100.0f;
	g_Light.Attenuation1 = 0.4f;
	g_Light.Position = D3DXVECTOR3( 0, 0, 0 );
    g_pDevice->SetLight( 0, &g_Light );
    g_pDevice->LightEnable( 0, TRUE );

	// 월드 행렬 설정
	D3DXMatrixIdentity( &g_matIdentity );

	g_Camera.SetEye( g_DefaultCameraPos );
	g_Camera.SetCamValue( 0.f, 0.f, 0.f, g_Distance );
	g_Camera.RotateY( 8 );
	g_Camera.Execute();

//	g_Sound.Init( g_hWnd );
//	g_Sound.Load( 0, "sound//bgm.wav" );

	g_DrawText.Create();
//	g_DrawText.DrawText( 0, 0, "aaaa" );

	/////////////////////////////////////////
	CFileLoader::Init( g_pDevice );

//	g_Terrain.LoadTerrain( "SaveMap\\ax.map" );
//	g_Terrain.SetTerrain();

	g_Scene.Create( "ui//scene.txt" );
	g_Scene.SwapScene( 0, 3 );

	return TRUE;
}


//-----------------------------------------------------------------------------//
// 데이타 모두제거됨
//-----------------------------------------------------------------------------//
void Cleanup()
{
	CFileLoader::Release();
	g_Scene.Destroy();

}


//-----------------------------------------------------------------------------//
// 출력
//-----------------------------------------------------------------------------//
bool Display( int timeDelta )
{
	if( g_bLoop )
	{
		g_Scr.Run( timeDelta );
		g_Scene.Animate( timeDelta );
	}

	g_Camera.Animate( timeDelta );

	g_pDevice->Clear( 0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(200,200,200), 1.0f, 0 );
//	g_pDevice->Clear( 0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(250,250,250), 1.0f, 0 );
	g_pDevice->BeginScene();
	g_Scene.Render();
//	g_DrawText.Render();
	g_pDevice->EndScene();
	g_pDevice->Present( NULL, NULL, NULL, NULL );

	return true;
}


// WndProc
LRESULT CALLBACK d3d::WndProc( HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
	switch( msg )
	{
	case WM_DESTROY:
		::PostQuitMessage(0);
		break;

	case WM_KEYDOWN:
		switch( wParam )
		{
//		case VK_ESCAPE:
//			::DestroyWindow(hwnd);
//			break;
		case VK_TAB:
			g_bLoop = !g_bLoop;
			break;

		case '1':
			g_Light.Direction = D3DXVECTOR3(1.f, 0.f, 0.f);
			g_pDevice->SetLight( 0, &g_Light );
			break;

		case '2':
			g_Light.Direction = D3DXVECTOR3(0.f, 0.f, 1.f );
			g_pDevice->SetLight( 0, &g_Light );
			break;

		case VK_UP:
			g_Camera.RotateX( 4 );

			g_Camera.Execute();
			break;

		case VK_DOWN:
			g_Camera.RotateX( -4 );
			g_Camera.Execute();
			break;

		case VK_LEFT:
			g_Camera.RotateY( 4 );
			g_Camera.Execute();
			break;

		case VK_RIGHT:
			g_Camera.RotateY( -4 );
			g_Camera.Execute();
			break;

		case 'O':
			{
				Vector3 p = g_Camera.GetEye();
				p.y += 1.f;
				g_Camera.SetEye( p );
				g_Camera.Execute();
			}
			break;

		case 'P':
			{
				Vector3 p = g_Camera.GetEye();
				p.y -= 1.f;
				g_Camera.SetEye( p );
				g_Camera.Execute();
			}
			break;
/**/
//		case VK_RETURN:
//			g_Sound.Play( 0 );
//			break;

		}
	}

	return ::DefWindowProc(hwnd, msg, wParam, lParam);
}


// WinMain
int WINAPI WinMain(HINSTANCE hinstance, HINSTANCE prevInstance, PSTR cmdLine, int showCmd )
{
	g_Dbg.Create( DBGLIB_CONSOL );
	g_Camera.SetViewPort( Width, Height );

	if(!(g_hWnd=d3d::InitD3D(hinstance, Width, Height, true, D3DDEVTYPE_HAL, &g_pDevice )) )
	{
		::MessageBox(0, "InitD3D() - FAILED", 0, 0);
		return 0;
	}

	if(!Setup())
	{
		::MessageBox(0, "Setup() - FAILED", 0, 0);
		return 0;
	}

	::SetWindowPos(g_hWnd, HWND_TOPMOST, 0, 0, Width, Height, SWP_SHOWWINDOW );
	::SetFocus( g_hWnd );
	::SetWindowPos(g_hWnd, HWND_NOTOPMOST, 0, 0, Width, Height, SWP_SHOWWINDOW );

	d3d::EnterMsgLoop( &g_Scene, Display );

	Cleanup();

	g_pDevice->Release();

	return 0;
}

