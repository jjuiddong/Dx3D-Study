/**-----------------------------------------------------------------------------
*  베이스 프레임
*------------------------------------------------------------------------------
*/
#include "defines.h"
#include "terrain.h"


/**-----------------------------------------------------------------------------
*  전역변수
*------------------------------------------------------------------------------
*/
LPDIRECT3D9					g_pD3D			=	NULL; /// D3D 디바이스를 생성할 D3D객체변수
LPDIRECT3DDEVICE9			g_pd3dDevice	=	NULL; /// 렌더링에 사용될 D3D디바이스
ID3DXFont*					g_Font			=	NULL;
CTerrain*					g_pTerrain		=	NULL;
bool						g_bWireMode		=   false;

RECT						g_rtClient;

// Variables for the frames per second counter
LARGE_INTEGER				g_nTimefeq;
LARGE_INTEGER				g_nStart;
LARGE_INTEGER				g_nEnd;
FLOAT						g_fFps = 0.0f;

//mouse cursor
FLOAT						g_MousePosX = 0.0f;
FLOAT						g_MousePosY = 0.0f;


/**-----------------------------------------------------------------------------
* 함수선언
*------------------------------------------------------------------------------
*/

HRESULT InitD3D( HWND hWnd );
HRESULT InitGeometry();
void	InitFPS();
HRESULT InitApp();
void	Cleanup();
void	SetupCamera();
void	SetDefaultStates();
void	CreateFont(); 
void	DrawFPS();
void	DrawText();
void	Update();
void	Render();


/**-----------------------------------------------------------------------------
* Direct3D 초기화
*------------------------------------------------------------------------------
*/
HRESULT InitD3D( HWND hWnd )
{
	/// 디바이스를 생성하기위한 D3D객체 생성
	if( NULL == ( g_pD3D = Direct3DCreate9( D3D_SDK_VERSION ) ) )
		return E_FAIL;

	/// 디바이스를 생성할 구조체
	D3DPRESENT_PARAMETERS d3dpp;
	ZeroMemory( &d3dpp, sizeof(d3dpp) );

	D3DDISPLAYMODE d3ddm;
	if(FAILED(g_pD3D->GetAdapterDisplayMode(0, &d3ddm))) {
		MessageBox(NULL, "GetAdapterDisplayMode failed", "Error", MB_OK);
		return false;
	}

	d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
	d3dpp.BackBufferCount		 = 1;
	d3dpp.BackBufferFormat		 = d3ddm.Format;
	d3dpp.BackBufferHeight		 = HEIGHT;
	d3dpp.BackBufferWidth		 = WIDTH;
	d3dpp.EnableAutoDepthStencil = TRUE;
	d3dpp.hDeviceWindow			 = hWnd;
	d3dpp.PresentationInterval	 = D3DPRESENT_INTERVAL_IMMEDIATE;
	d3dpp.SwapEffect			 = D3DSWAPEFFECT_DISCARD;
	d3dpp.Windowed				 = TRUE;


	/// 디바이스 생성
	if( FAILED( g_pD3D->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd,
		D3DCREATE_SOFTWARE_VERTEXPROCESSING,
		&d3dpp, &g_pd3dDevice ) ) )
	{
		return E_FAIL;
	}

	return S_OK;
}

/**-----------------------------------------------------------------------------
* 기하정보 초기화
*------------------------------------------------------------------------------
*/
HRESULT InitGeometry()
{	
	/*if( FAILED( g_pd3dDevice->CreateVertexBuffer( 4*sizeof(CUSTOMVERTEX),
		0, D3DFVF_CUSTOMVERTEX,
		D3DPOOL_DEFAULT, &g_pVB, NULL ) ) )
	{
		return E_FAIL;
	}

	VOID* pVertices;
	if( FAILED( g_pVB->Lock( 0, sizeof(g_Vertices), (void**)&pVertices, 0 ) ) )
		return E_FAIL;
	memcpy( pVertices, g_Vertices, sizeof(g_Vertices) );
	g_pVB->Unlock();*/

	
	g_pTerrain = new CTerrain( g_pd3dDevice );
	g_pTerrain->LoadTextures();
	g_pTerrain->CreateAlphaMaps();
	g_pTerrain->TerrainInit( MAPWIDTH, MAPHEIGHT, 1.0f);

	
	return S_OK;
}

/**-----------------------------------------------------------------------------
* 카메라 행렬 설정
*------------------------------------------------------------------------------
*/
void SetupCamera()
{
	/// 월드행렬 설정
	D3DXMATRIXA16 matWorld;
	D3DXMatrixIdentity( &matWorld );
	g_pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );

	/*D3DXVECTOR3 vEyePt(0.0f, 15.0f,-22.0f );
	D3DXVECTOR3 vLookatPt(0.0f, 0.0f, 0.0f);
	D3DXVECTOR3 vUpVec(0.0f, 1.0f, 0.0f);*/

	/*D3DXVECTOR3 vEyePt(0.23f, 1.5f, 0.401f);
	D3DXVECTOR3 vLookatPt(0.23f, 0.0, 0.4f);
	D3DXVECTOR3 vUpVec(0, 1, 0);*/

	D3DXVECTOR3 vEyePt(float((MAPWIDTH)/2), 20.0f, -10.0f);
	D3DXVECTOR3 vLookatPt(float((MAPWIDTH)/2), 10.0f, 0.0f);
	D3DXVECTOR3 vUpVec(0.0f, 1.0f, 0.0f);

	/*D3DXVECTOR3 vEyePt(float((MAPWIDTH)/2), 7.0f, -1.0f);
	D3DXVECTOR3 vLookatPt(float((MAPWIDTH)/2), 5.0f, 0.0f);
	D3DXVECTOR3 vUpVec(0.0f, 1.0f, 0.0f);*/

	D3DXMATRIXA16 matView;
	D3DXMatrixLookAtLH( &matView, &vEyePt, &vLookatPt, &vUpVec );
	g_pd3dDevice->SetTransform( D3DTS_VIEW, &matView );

	/// 프로젝션 행렬 설정
	D3DXMATRIXA16 matProj;
	FLOAT fWidth = WIDTH;
	FLOAT fHeight = HEIGHT;	
	D3DXMatrixPerspectiveFovLH(&matProj, D3DX_PI/4.0f, fWidth/fHeight, 1.0f, 100.0f);
	g_pd3dDevice->SetTransform( D3DTS_PROJECTION, &matProj );

}

/**-----------------------------------------------------------------------------
* 초기화 객체들 소거
*------------------------------------------------------------------------------
*/
void Cleanup()
{
	SAFE_RELEASE_( g_Font );
	SAFE_RELEASE_( g_pd3dDevice );
	SAFE_DELETE_( g_pTerrain );
}


//-----------------------------------------------------------------------------
// Set the render and sampler states and the three matrices
//-----------------------------------------------------------------------------
void SetDefaultStates() 
{

	g_pd3dDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
	g_pd3dDevice->SetRenderState(D3DRS_ZENABLE, TRUE);

	// Allow multiple passes to blend together correctly
	g_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);

	g_pd3dDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	g_pd3dDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	// Allow multiple passes to blend together correctly
	/*g_pd3dDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	g_pd3dDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	g_pd3dDevice->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);
	// Prevent some ugliness with the alphamaps
	g_pd3dDevice->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
	g_pd3dDevice->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);

	g_pd3dDevice->SetSamplerState(1, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	g_pd3dDevice->SetSamplerState(1, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	g_pd3dDevice->SetSamplerState(1, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);*/
	
}




//-----------------------------------------------------------------------------
// Create the font used to render text
//-----------------------------------------------------------------------------
void CreateFont() 
{
	if(FAILED(D3DXCreateFont(g_pd3dDevice, 20, 0, FW_BOLD, 1, FALSE, 
		                     DEFAULT_CHARSET, 
							 OUT_DEFAULT_PRECIS, 
							 DEFAULT_QUALITY, 
							 DEFAULT_PITCH | FF_DONTCARE,
							 "Arial", 
							 &g_Font))) 
	{
		MessageBox(NULL, "D3DXCreateFont failed", "Warning", MB_OK);
	}
}

void InitFPS()
{
	QueryPerformanceFrequency( &g_nTimefeq );
	QueryPerformanceCounter( &g_nStart );
}

void DrawFPS()
{
	static int		FrameCnt    = 0;
	static FLOAT    fPassed = 0;
	char szText[256];
	

	QueryPerformanceCounter(&g_nEnd);

	fPassed += ((FLOAT)(g_nEnd.QuadPart - g_nStart.QuadPart) / (FLOAT)g_nTimefeq.QuadPart);
	FrameCnt++;

	if (fPassed > 1.0f)
	{
		g_fFps = (FLOAT)FrameCnt / fPassed;
		fPassed = 0.0f;
		FrameCnt = 0;
	}

	sprintf( szText, "FPS : %.2f", g_fFps );

	RECT rect;
	SetRect(&rect,0,0,200,20);
	g_Font->DrawText( NULL, szText, -1, &rect, DT_LEFT,D3DXCOLOR(255,0,0,255) );

	QueryPerformanceCounter(&g_nStart);


}

void DrawText()
{
	char szText[256];
	RECT rect;
	
	int		nCurSelIndex = g_pTerrain->GetCurSelIndex();
	int*	pIndex = g_pTerrain->GetDrawIndex();
	float	BrushSize = g_pTerrain->GetBrushSize();
	float	SmoothSize = g_pTerrain->GetSmoothSize();
	

	SetRect(&rect,0,20,400,40);
	sprintf( szText, "Current Selected Texture(Key 1~4) : %d  \n", nCurSelIndex+1 );
	g_Font->DrawText( NULL, szText, -1, &rect, DT_LEFT,D3DXCOLOR(255,255,0,255) );

	SetRect(&rect,0,40,600,60);
	sprintf( szText, "Texture Draw order ( UpKey ++ DownKey -- ) : %d > %d > %d > %d\n", pIndex[0]+1, pIndex[1]+1, pIndex[2]+1, pIndex[3]+1 );
	g_Font->DrawText( NULL, szText, -1, &rect, DT_LEFT,D3DXCOLOR(255,255,0,255) );

	SetRect(&rect,0,60,400,100);
	sprintf( szText, "BrushSize   ('Q' ++ , 'W' -- )  : %d  \nSmoothSize ('A' ++ , 'S' --  )  : %d \n", int(BrushSize), int(SmoothSize));
	g_Font->DrawText( NULL, szText, -1, &rect, DT_LEFT,D3DXCOLOR(255,255,0,255) );

	SetRect(&rect,0,100,400,140);
	sprintf( szText, "Wireframe : press ' I ' key \n");
	g_Font->DrawText( NULL, szText, -1, &rect, DT_LEFT,D3DXCOLOR(255,255,0,255) );
}



HRESULT InitApp()
{	
	/// 카메라 행렬설정
	SetupCamera();
	CreateFont();
	InitFPS();
	SetDefaultStates();

	return S_OK;
}


void BaseTextureRender()
{
	g_pTerrain->TerrainRender();
}




/**-----------------------------------------------------------------------------
* 화면 그리기
*------------------------------------------------------------------------------
*/
void Render()
{
	/// 후면버퍼와 Z버퍼 초기화
	g_pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0,0,255), 1.0f, 0 );

	/// 렌더링 시작
	if( SUCCEEDED( g_pd3dDevice->BeginScene() ) )
	{
		if( g_bWireMode )
			g_pd3dDevice->SetRenderState( D3DRS_FILLMODE , D3DFILL_WIREFRAME );
		else
			g_pd3dDevice->SetRenderState( D3DRS_FILLMODE , D3DFILL_SOLID );

		BaseTextureRender();
		g_pTerrain->AlphaTextureRender();
		g_pTerrain->MiniAlphaTextureRender();
		DrawFPS();
		DrawText();

		float BrushSize = g_pTerrain->GetBrushSize();
		float SmoothSize = g_pTerrain->GetSmoothSize();
		g_pTerrain->DrawPickCircle(30, SmoothSize ,0xffffff00);
		g_pTerrain->DrawPickCircle(30, BrushSize ,0xffff0000);
	
		/// 렌더링 종료
		g_pd3dDevice->EndScene();
	}

	/// 후면버퍼를 보이는 화면으로!
	g_pd3dDevice->Present( NULL, NULL, NULL, NULL );
}


/**-----------------------------------------------------------------------------
* 업데이트 <- 업데이트와 렌더를 따로 구분한다.
*------------------------------------------------------------------------------
*/
void Update()
{
	g_pTerrain->PickingProcess( g_rtClient, g_MousePosX, g_MousePosY );
	if( g_pTerrain->GetClicked() )
		g_pTerrain->DrawAlphamaps( g_pTerrain->GetCurSelIndex() );
}


/**-----------------------------------------------------------------------------
* 윈도우 프로시져
*------------------------------------------------------------------------------
*/

LRESULT WINAPI MsgProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
	switch( msg )
	{
		case WM_DESTROY		:
		{
			Cleanup();
			PostQuitMessage( 0 );
		}
		return 0;
		case WM_KEYDOWN		:
		{
			switch(wParam)
			{
				case VK_ESCAPE :
				{
					Cleanup();
					PostQuitMessage( 0 );
				}
				return 0;	
				case '1'		:  g_pTerrain->SetCurSelIndex( 0 );			break;
				case '2'		:  g_pTerrain->SetCurSelIndex( 1 );			break;
				case '3'		:  g_pTerrain->SetCurSelIndex( 2 );			break;
				case '4'		:  g_pTerrain->SetCurSelIndex( 3 );			break;
				case VK_UP		:
				{	
					for( int i=0 ; i< MAXTEXNUM ; ++i )
					{
						int* pIndex = g_pTerrain->GetDrawIndex();

						if( i>0 && g_pTerrain->GetCurSelIndex() == pIndex[i] )
						{
							int nTemp = pIndex[i];
							pIndex[i] = pIndex[i-1];
							pIndex[i-1] = nTemp;
							break;
						}
					}	
				}
				break;
				case VK_DOWN	:
				{
					for( int i=0 ; i< MAXTEXNUM ; ++i )
					{
						int* pIndex = g_pTerrain->GetDrawIndex();

						if( i<MAXTEXNUM-1 && g_pTerrain->GetCurSelIndex() == pIndex[i] )
						{
							int nTemp = pIndex[i];
							pIndex[i] = pIndex[i+1];
							pIndex[i+1] = nTemp;
							break;
						}
					}	
				}
				break;
				case 'Q'	:
				{
					float BrushSize = g_pTerrain->GetBrushSize();
					BrushSize += 1.0f;
					g_pTerrain->SetBrushSize( BrushSize );
					if( 5.0f < BrushSize )
						g_pTerrain->SetBrushSize( 5.0f);
				}
				break;
				case 'W'	:
				{
					float BrushSize = g_pTerrain->GetBrushSize();
					float SmoothSize = g_pTerrain->GetSmoothSize();
					BrushSize -= 1.0f;
					g_pTerrain->SetBrushSize( BrushSize );
					if( 1.0f > BrushSize )
						g_pTerrain->SetBrushSize( 1.0f);
					
					if( BrushSize < SmoothSize ) 
						g_pTerrain->SetBrushSize( SmoothSize );
				}
				break;
				case 'A'	:
				{
					float BrushSize = g_pTerrain->GetBrushSize();
					float SmoothSize = g_pTerrain->GetSmoothSize();
					SmoothSize += 1.0f;
					g_pTerrain->SetSmoothSize( SmoothSize );
					if( 5.0f < SmoothSize )
						g_pTerrain->SetSmoothSize( 5.0f );
					
					if(SmoothSize >= BrushSize )
						g_pTerrain->SetSmoothSize( BrushSize );
				}
				break;
				case 'S'	:
				{
					float SmoothSize = g_pTerrain->GetSmoothSize();
					SmoothSize -= 1.0f;
					g_pTerrain->SetSmoothSize( SmoothSize );
					if( 1.0f > SmoothSize )
						g_pTerrain->SetSmoothSize( 1.0f );
				}
				break;
				case 'I'	:
					{
						if( g_bWireMode )
							g_bWireMode = false;
						else
							g_bWireMode = true;
					}
					break;
				
			}
		}
		break;
		case WM_LBUTTONDOWN	: 
		{
			g_pTerrain->SetClicked( TRUE );
		}
		break;
		case WM_LBUTTONUP	: 
		{
			g_pTerrain->SetClicked( FALSE );
		}
		break;
		case WM_MOUSEMOVE : 
		{
			g_MousePosX = LOWORD( lParam );
			g_MousePosY = HIWORD( lParam );
		}
		break;
	}

	return DefWindowProc( hWnd, msg, wParam, lParam );
}


/**-----------------------------------------------------------------------------
* 프로그램 시작점
*------------------------------------------------------------------------------
*/
INT WINAPI WinMain( HINSTANCE hInst, HINSTANCE, LPSTR, INT )
{
	/// 윈도우 클래스 등록
	WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, MsgProc, 0L, 0L,
		GetModuleHandle(NULL), NULL, NULL, NULL, NULL,
		"BasicFrame", NULL };
	RegisterClassEx( &wc );

	/// 윈도우 생성
	HWND hWnd = CreateWindow( "BasicFrame", "TextureSplatting",
		WS_OVERLAPPEDWINDOW, 100, 100, WIDTH, HEIGHT,
		GetDesktopWindow(), NULL, wc.hInstance, NULL );

	GetClientRect( hWnd, &g_rtClient );

	/// Direct3D 초기화
	if( SUCCEEDED( InitD3D( hWnd ) ) )
	{
		if( SUCCEEDED( InitGeometry() ) )
		{
			/// 윈도우 출력
			ShowWindow( hWnd, SW_SHOWDEFAULT );
			UpdateWindow( hWnd );

			InitApp();

			/// 메시지 루프
			MSG msg;
			ZeroMemory( &msg, sizeof(msg) );
			while( msg.message!=WM_QUIT )
			{
				/// 메시지큐에 메시지가 있으면 메시지 처리
				if( PeekMessage( &msg, NULL, 0U, 0U, PM_REMOVE ) )
				{
					TranslateMessage( &msg );
					DispatchMessage( &msg );
				}
				else
				{
					/// 처리할 메시지가 없으면 Render()함수 호출
					Update();
					Render();
				}
			}
		}
	}

	/// 등록된 클래스 소거
	UnregisterClass( "D3D Tutorial", wc.hInstance );
	return 0;
}
