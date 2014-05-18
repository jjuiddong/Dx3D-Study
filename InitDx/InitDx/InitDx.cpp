// InitDx.cpp : 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"
#include "InitDx.h"
#include <d3d9.h>
#pragma comment( lib, "d3d9.lib" )

#define MAX_LOADSTRING 100

// 전역 변수:
HINSTANCE hInst;								// 현재 인스턴스입니다.
TCHAR szTitle[MAX_LOADSTRING];					// 제목 표시줄 텍스트입니다.
TCHAR szWindowClass[MAX_LOADSTRING];			// 기본 창 클래스 이름입니다.
HWND g_hWnd;
LPDIRECT3DDEVICE9	g_pDevice;

// 이 코드 모듈에 들어 있는 함수의 정방향 선언입니다.
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

 	// TODO: 여기에 코드를 입력합니다.
	MSG msg;
	HACCEL hAccelTable;

	// 전역 문자열을 초기화합니다.
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_INITDX, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// 응용 프로그램 초기화를 수행합니다.
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_INITDX));

	// 기본 메시지 루프입니다.
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int) msg.wParam;
}



//
//  함수: MyRegisterClass()
//
//  목적: 창 클래스를 등록합니다.
//
//  설명:
//
//    Windows 95에서 추가된 'RegisterClassEx' 함수보다 먼저
//    해당 코드가 Win32 시스템과 호환되도록
//    하려는 경우에만 이 함수를 사용합니다. 이 함수를 호출해야
//    해당 응용 프로그램에 연결된
//    '올바른 형식의' 작은 아이콘을 가져올 수 있습니다.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_INITDX));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_INITDX);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

//
//   함수: InitInstance(HINSTANCE, int)
//
//   목적: 인스턴스 핸들을 저장하고 주 창을 만듭니다.
//
//   설명:
//
//        이 함수를 통해 인스턴스 핸들을 전역 변수에 저장하고
//        주 프로그램 창을 만든 다음 표시합니다.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd;

   hInst = hInstance; // 인스턴스 핸들을 전역 변수에 저장합니다.

   g_hWnd = hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

   if (!hWnd)
   {
      return FALSE;
   }


   LPDIRECT3D9			d3d9;
   d3d9 = Direct3DCreate9( D3D_SDK_VERSION );



   //2. 하드웨어 정보를 가지고 와서 자신의 정점 프로세스 타입을 정하자
   D3DCAPS9 caps;

   //Direct3D9 객체 통해 비디오 카드의 하드웨어 정보를 가지고 온다.
   d3d9->GetDeviceCaps(
	   D3DADAPTER_DEFAULT,			//주 디스플레이 그래픽 카드 그냥 D3DADAPTER_DEFAULT
	   D3DDEVTYPE_HAL,				//디바이스타입 설정 그냥 D3DDEVTYPE_HAL
	   &caps						//디바이스 정보를 받아올 D3DCAPS9 포인터
	   );

   //정점계산 처리방식을 지정할 플레그 값
   int vertexProcessing = 0;

   //정점 위치와 광원 계산시 하드웨어 사용이 가능한가
   if( caps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT )
	   vertexProcessing = D3DCREATE_HARDWARE_VERTEXPROCESSING;
   else
	   vertexProcessing = D3DCREATE_SOFTWARE_VERTEXPROCESSING;



   //3. D3DPRESENT_PARAMETERS 구조체 정보를 생성
   //내가 이러한 Device 를 만들겟다라는 정보


   D3DPRESENT_PARAMETERS d3dpp;		
   d3dpp.AutoDepthStencilFormat		= D3DFMT_D24S8;				//뎁스 버퍼와 스테실 버퍼 크기 뎁스 24bit 스텐실 버퍼 8 비트
   d3dpp.BackBufferCount				= 1;						//백버퍼 갯수 그냥 1개
   d3dpp.BackBufferFormat				= D3DFMT_A8R8G8B8;			//백버퍼 색상 포맷 알파8비트,레드8비트,그린8비트,블루8비트
   d3dpp.BackBufferHeight				= 800;				//백버퍼 픽셀 크기
   d3dpp.BackBufferWidth				= 600;				//백버퍼 픽셀 크기
   d3dpp.EnableAutoDepthStencil		= true;						//자동 깊이버퍼 사용 여부 ( 그냥 true )
   d3dpp.Flags							= 0;						//추기 플래그 ( 일단 0 )
   d3dpp.FullScreen_RefreshRateInHz	= D3DPRESENT_RATE_DEFAULT;	//화면 주사율 ( 그냥 D3DPRESENT_RATE_DEFAULT 모니터 주사율과 동일시 )
   d3dpp.hDeviceWindow					= g_hWnd;					//Device 가 출력될 윈도우 핸들
   d3dpp.MultiSampleQuality			= 0;						//멀티 샘플링 질
   d3dpp.MultiSampleType				= D3DMULTISAMPLE_NONE;		//멀티 샘플링 타입 
   d3dpp.PresentationInterval			= D3DPRESENT_INTERVAL_ONE;	//화면 전송 간격 ( 그냥 D3DPRESENT_INTERVAL_ONE 모니터 주사율과 동일시 )
   d3dpp.SwapEffect					= D3DSWAPEFFECT_DISCARD;	//화면 스왑 체인 방식
   d3dpp.Windowed						= true;						//윈도우 모드냐? ( 이게 false 면 풀스크린 된다! )



   //4. Device 생성
   if( FAILED( d3d9->CreateDevice(
	   D3DADAPTER_DEFAULT,					//주 디스플레이 그래픽 카드 그냥 D3DADAPTER_DEFAULT
	   D3DDEVTYPE_HAL,						//디바이스타입 설정 그냥 D3DDEVTYPE_HAL
	   g_hWnd,								//디바이스를 사용할 윈도우 핸들
	   vertexProcessing,					//정점 처리 방식에 대한 플레그
	   &d3dpp,								//앞에서 정의한 D3DPRESENT_PARAMETERS 구조체 포인터
	   &g_pDevice							//얻어올 디바이스 더블포인터
	   ) ) )
   {
	   //디바이스 생성 실패

	   d3d9->Release(); // Deivce 를 만들기 위해 생성된 Direct3D9 객체를 해제
	   d3d9 = NULL;

	   MessageBoxA( g_hWnd, "CreateDevice() - FAILED", "FAILED", MB_OK );
	   return E_FAIL;
   }


   //디바이스 생성 성공
   d3d9->Release(); // Deivce 를 만들었으니 넌 더이상 필요없다 ( 사라져라! )
   d3d9 = NULL;


   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  함수: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  목적: 주 창의 메시지를 처리합니다.
//
//  WM_COMMAND	- 응용 프로그램 메뉴를 처리합니다.
//  WM_PAINT	- 주 창을 그립니다.
//  WM_DESTROY	- 종료 메시지를 게시하고 반환합니다.
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;

	switch (message)
	{
	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// 메뉴 선택을 구문 분석합니다.
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_PAINT:
		{
			//화면 청소
			if( SUCCEEDED( g_pDevice->Clear( 
				0,			//청소할 영역의 D3DRECT 배열 갯수		( 전체 클리어 0 )
				NULL,		//청소할 영역의 D3DRECT 배열 포인터		( 전체 클리어 NULL )
				D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL,	//청소될 버퍼 플레그 ( D3DCLEAR_TARGET 컬러버퍼, D3DCLEAR_ZBUFFER 깊이버퍼, D3DCLEAR_STENCIL 스텐실버퍼
				0xffffff00,			//컬러버퍼를 청소하고 채워질 색상( 0xAARRGGBB )
				1.0f,				//깊이버퍼를 청소할값 ( 0 ~ 1 0 이 카메라에서 제일가까운 1 이 카메라에서 제일 먼 )
				0					//스텐실 버퍼를 채울값
				) ) )
			{
				//화면 청소가 성공적으로 이루어 졌다면... 랜더링 시작
				g_pDevice->BeginScene();

				//여기서부터 랜더링 명령이 실행된다..




				//랜더링 끝
				g_pDevice->EndScene();


				//랜더링이 끝났으면 랜더링된 내용 화면으로 전송
				g_pDevice->Present( NULL, NULL, NULL, NULL );
			}

		}
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// 정보 대화 상자의 메시지 처리기입니다.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}
