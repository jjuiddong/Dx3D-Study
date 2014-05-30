#include <windows.h>
#include <string>
#include <fstream>
#include <d3d9.h>
#include <d3dx9.h>
#include "../math/Math.h"
#pragma comment( lib, "d3d9.lib" )
#pragma comment( lib, "d3dx9.lib" )

#include <vector>
#include <map>

using namespace std;

LPDIRECT3DDEVICE9	g_pDevice = NULL;
const int WINSIZE_X = 1024;		//초기 윈도우 가로 크기
const int WINSIZE_Y = 768;	//초기 윈도우 세로 크기
const int WINPOS_X = 0; //초기 윈도우 시작 위치 X
const int WINPOS_Y = 0; //초기 윈도우 시작 위치 Y
D3DLIGHT9 g_Light;
D3DMATERIAL9 g_Mtrl;

LPD3DXMESH g_Mesh;
POINT g_CurPos;
bool g_LButtonDown = false;
Matrix44 g_LocalTm;

// animation
bool g_ani = false;
int g_aniVtxIdx = -1;
Vector3 g_aniSrcPos;
Vector3 g_aniDestPos;
float g_aniAlpha = 0; // 0~1


// 버텍스 구조체
struct Vertex
{
	Vertex() {}
	Vertex(float x0, float y0, float z0) : p(Vector3(x0, y0, z0))
		,u(-100), v(-100)
	{}
	Vector3 p;
	Vector3 n;
	float u, v;
	static const DWORD FVF;
};
//버텍스 구조체 포맷.
const DWORD Vertex::FVF  = D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1;



// 콜백 프로시져 함수 프로토 타입
LRESULT CALLBACK WndProc( HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam );
bool InitDirectX(HWND hWnd);
bool InitVertexBuffer();
void Render(int timeDelta);
bool ReadModelFile( const string &fileName, LPDIRECT3DVERTEXBUFFER9 &vtxBuff, int &vtxSize,  LPDIRECT3DINDEXBUFFER9 &idxBuff, int &faceSize );
void ComputeNormals(LPDIRECT3DVERTEXBUFFER9 vtxBuff, int vtxSize,  LPDIRECT3DINDEXBUFFER9 idxBuff, int faceSize);


int APIENTRY WinMain(HINSTANCE hInstance, 
	HINSTANCE hPrevInstance, 
	LPSTR lpCmdLine, 
	int nCmdShow)
{
	wchar_t className[32] = L"Sample";
	wchar_t windowName[32] = L"Sample";

	//윈도우 클레스 정보 생성
	//내가 이러한 윈도를 만들겠다 라는 정보
	WNDCLASS WndClass;
	WndClass.cbClsExtra = 0;			//윈도우에서 사용하는 여분의 메모리설정( 그냥 0 이다  신경쓰지말자 )
	WndClass.cbWndExtra = 0;			//윈도우에서 사용하는 여분의 메모리설정( 그냥 0 이다  신경쓰지말자 )
	WndClass.hbrBackground = (HBRUSH)GetStockObject(GRAY_BRUSH);		//윈도우 배경색상
	WndClass.hCursor = LoadCursor( NULL, IDC_ARROW );			//윈도우의 커서모양 결정
	WndClass.hIcon = LoadIcon( NULL, IDI_APPLICATION );		//윈도우아이콘모양 결정
	WndClass.hInstance = hInstance;				//프로그램인스턴스핸들 
	WndClass.lpfnWndProc = (WNDPROC)WndProc;			//윈도우 프로시져 함수 포인터
	WndClass.lpszMenuName = NULL;						//메뉴이름 없으면 NULL
	WndClass.lpszClassName = className;				//지금 작성하고 있는 윈도우 클레스의 이름
	WndClass.style	 = CS_HREDRAW | CS_VREDRAW;	//윈도우 그리기 방식 설정 ( 사이즈가 변경될때 화면갱신 CS_HREDRAW | CS_VREDRAW )

	//위에서 작성한 윈도우 클레스정보 등록
	RegisterClass( &WndClass );

	//윈도우 생성
	//생성된 윈도우 핸들을 전역변수 g_hWnd 가 받는다.
	HWND hWnd = CreateWindow(
		className,				//생성되는 윈도우의 클래스이름
		windowName,				//윈도우 타이틀바에 출력되는 이름
		WS_OVERLAPPEDWINDOW,	//윈도우 스타일 WS_OVERLAPPEDWINDOW
		WINPOS_X,				//윈도우 시작 위치 X 
		WINPOS_Y,				//윈도우 시작 위치 Y
		WINSIZE_X,				//윈도우 가로 크기 ( 작업영역의 크기가 아님 )
		WINSIZE_Y,				//윈도우 세로 크기 ( 작업영역의 크기가 아님 )
		GetDesktopWindow(),		//부모 윈도우 핸들 ( 프로그램에서 최상위 윈도우면 NULL 또는 GetDesktopWindow() )
		NULL,					//메뉴 ID ( 자신의 컨트롤 객체의 윈도우인경우 컨트롤 ID 가 된	
		hInstance,				//이 윈도우가 물릴 프로그램 인스턴스 핸들
		NULL					//추가 정보 NULL ( 신경끄자 )
		);

	//윈도우를 정확한 작업영역 크기로 맞춘다
	RECT rcClient = { 0, 0, WINSIZE_X, WINSIZE_Y };
	AdjustWindowRect( &rcClient, WS_OVERLAPPEDWINDOW, FALSE );	//rcClient 크기를 작업 영영으로 할 윈도우 크기를 rcClient 에 대입되어 나온다.

	//윈도우 크기와 윈도우 위치를 바꾸어준다.
	SetWindowPos( hWnd, NULL, 0, 0, rcClient.right - rcClient.left, rcClient.bottom - rcClient.top, 
		SWP_NOZORDER | SWP_NOMOVE );

	if (!InitDirectX(hWnd))
	{
		return 0;
	}

	InitVertexBuffer();
	ShowWindow( hWnd, nCmdShow );


	//메시지 구조체
	MSG msg;		
	ZeroMemory( &msg, sizeof( MSG ) );

	int oldT = GetTickCount();
	while (msg.message != WM_QUIT)
	{
		//PeekMessage 는 메시지 큐에 메시지가 없어도 프로그램이 멈추기 않고 진행이 된다.
		//이때 메시지큐에 메시지가 없으면 false 가 리턴되고 메시지가 있으면 true 가 리턴이된다.
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
			TranslateMessage( &msg ); //눌린 키보드 의 문자를 번역하여 WM_CHAR 메시지를 발생시킨다.
			DispatchMessage( &msg );  //받아온 메시지 정보로 윈도우 프로시져 함수를 실행시킨다.
		}
		else
		{
			const int curT = GetTickCount();
			const int elapseT = curT - oldT;
			if (elapseT > 15)
			{
				oldT = curT;
				Render(elapseT);
			}
		}
	}

	if (g_Mesh)
		g_Mesh->Release();
	if (g_pDevice)
		g_pDevice->Release();
	return 0;
}


//
// 윈도우 프로시져 함수 ( 메시지 큐에서 받아온 메시지를 처리한다 )
//
LRESULT CALLBACK WndProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
	switch (msg)
	{
	case WM_KEYDOWN:
		if (wParam == VK_ESCAPE)
			::DestroyWindow(hWnd);
		if (wParam == VK_TAB)
		{
			static bool flag = true;
			g_pDevice->SetRenderState(D3DRS_CULLMODE, flag? D3DCULL_CCW : D3DCULL_NONE);
			g_pDevice->SetRenderState(D3DRS_FILLMODE, flag? D3DFILL_SOLID : D3DFILL_WIREFRAME);
			flag = !flag;
		}
		break;

	case WM_LBUTTONDOWN:
		{
			g_LButtonDown = true;
			g_CurPos.x = LOWORD(lParam);
			g_CurPos.y = HIWORD(lParam);
		}
		break;

	case WM_LBUTTONUP:
		g_LButtonDown = false;
		break;

	case WM_MOUSEMOVE:
		if (g_LButtonDown)
		{
			POINT pos;
			pos.x = LOWORD(lParam);
			pos.y = HIWORD(lParam);

			const int x = pos.x - g_CurPos.x;
			const int y = pos.y - g_CurPos.y;

			Matrix44 mat1;
			mat1.SetRotationY( -x * 0.01f );
			Matrix44 mat2;
			mat2.SetRotationX( -y * 0.01f );

			g_CurPos = pos;

			g_LocalTm *= (mat1 * mat2);
		}
		break;

	case WM_DESTROY: //윈도우가 파괴된다면..
		PostQuitMessage(0);	//프로그램 종료 요청 ( 메시지 루프를 빠져나가게 된다 )
		break;
	}
	return DefWindowProc( hWnd, msg, wParam, lParam );
}


bool InitDirectX(HWND hWnd)
{
	LPDIRECT3D9 d3d9;
	d3d9 = Direct3DCreate9( D3D_SDK_VERSION );

	// 하드웨어 정보를 가지고 와서 자신의 정점 프로세스 타입을 정하자
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
	if (caps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT)
		vertexProcessing = D3DCREATE_HARDWARE_VERTEXPROCESSING;
	else
		vertexProcessing = D3DCREATE_SOFTWARE_VERTEXPROCESSING;


	//3. D3DPRESENT_PARAMETERS 구조체 정보를 생성
	//내가 이러한 Device 를 만들겟다라는 정보

	D3DPRESENT_PARAMETERS d3dpp;		
	d3dpp.AutoDepthStencilFormat = D3DFMT_D24S8;				//뎁스 버퍼와 스테실 버퍼 크기 뎁스 24bit 스텐실 버퍼 8 비트
	d3dpp.BackBufferCount = 1;						//백버퍼 갯수 그냥 1개
	d3dpp.BackBufferFormat = D3DFMT_A8R8G8B8;			//백버퍼 색상 포맷 알파8비트,레드8비트,그린8비트,블루8비트
	d3dpp.BackBufferHeight = WINSIZE_Y;				//백버퍼 픽셀 크기
	d3dpp.BackBufferWidth = WINSIZE_X;				//백버퍼 픽셀 크기
	d3dpp.EnableAutoDepthStencil = true;						//자동 깊이버퍼 사용 여부 ( 그냥 true )
	d3dpp.Flags = 0;						//추기 플래그 ( 일단 0 )
	d3dpp.FullScreen_RefreshRateInHz	= D3DPRESENT_RATE_DEFAULT;	//화면 주사율 ( 그냥 D3DPRESENT_RATE_DEFAULT 모니터 주사율과 동일시 )
	d3dpp.hDeviceWindow = hWnd;					//Device 가 출력될 윈도우 핸들
	d3dpp.MultiSampleQuality = 0;						//멀티 샘플링 질
	d3dpp.MultiSampleType = D3DMULTISAMPLE_NONE;		//멀티 샘플링 타입 
	d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_ONE;	//화면 전송 간격 ( 그냥 D3DPRESENT_INTERVAL_ONE 모니터 주사율과 동일시 )
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;	//화면 스왑 체인 방식
	d3dpp.Windowed = true;						//윈도우 모드냐? ( 이게 false 면 풀스크린 된다! )

	//4. Device 생성
	if (FAILED(d3d9->CreateDevice(
		D3DADAPTER_DEFAULT,					//주 디스플레이 그래픽 카드 그냥 D3DADAPTER_DEFAULT
		D3DDEVTYPE_HAL,						//디바이스타입 설정 그냥 D3DDEVTYPE_HAL
		hWnd,								//디바이스를 사용할 윈도우 핸들
		vertexProcessing,					//정점 처리 방식에 대한 플레그
		&d3dpp,								//앞에서 정의한 D3DPRESENT_PARAMETERS 구조체 포인터
		&g_pDevice							//얻어올 디바이스 더블포인터
		)))
	{
		//디바이스 생성 실패
		d3d9->Release(); // Deivce 를 만들기 위해 생성된 Direct3D9 객체를 해제
		d3d9 = NULL;

		MessageBoxA( hWnd, "CreateDevice() - FAILED", "FAILED", MB_OK );
		return false;
	}


	//디바이스 생성 성공
	d3d9->Release(); // Deivce 를 만들었으니 넌 더이상 필요없다 ( 사라져라! )
	d3d9 = NULL;
	return true;
}


bool InitVertexBuffer()
{
	D3DXCreateMeshFVF(2, 4, D3DXMESH_DYNAMIC, Vertex::FVF,
		g_pDevice, &g_Mesh);

	LPDIRECT3DVERTEXBUFFER9 vtxBuff;
	g_Mesh->GetVertexBuffer(&vtxBuff);

	Vertex *vertices;
	vtxBuff->Lock(0, 0, (void**)&vertices, 0);
	vertices[ 0] = Vertex(-100, 0, 100);
	vertices[ 1] = Vertex(-100, 0, -100);
	vertices[ 2] = Vertex(100, 0, -100);
	vertices[ 3] = Vertex(-100, 200, -100);

	LPDIRECT3DINDEXBUFFER9 idxBuff;
	g_Mesh->GetIndexBuffer(&idxBuff);
	WORD *indices;
	idxBuff->Lock(0, 0, (void**)&indices, 0);
	indices[ 0] = 0; indices[ 1] = 1; indices[ 2] = 2;
	indices[ 3] = 0; indices[ 4] = 3; indices[ 5] = 1;



	const int numFace = g_Mesh->GetNumFaces();
	vector<int> adjInfo(numFace*3, 0);
	g_Mesh->GenerateAdjacency(0.0, (DWORD*)&adjInfo[0]);

	for (int i=0; i < numFace-1; ++i)
	{
		for (int k=0; k < 3; ++k)
		{
			const int adjFaceIdx = adjInfo[ i*3 + k];
			if (adjFaceIdx < 0)
				continue;

			int selectVtxIdx = -1;
			for (int m=0; m < 3; ++m)
			{
				const int correspondAdjFaceIdx = adjInfo[ adjFaceIdx*3 + m];
				if (i == correspondAdjFaceIdx)
				{
					const int vtxIdxTable[] = {2,0,1};
					selectVtxIdx = indices[ adjFaceIdx*3 + vtxIdxTable[ m]];
					break;
				}
			}

			if (selectVtxIdx < 0)
				continue;

			const int vtxIdxEdge1[3] = {0, 1, 2};
			const int vtxIdxEdge2[3] = {1, 2, 0};
			const int remainVtxIdx[3] = {2,0,1};
			const int vtxIdx1 = indices[ i*3 + vtxIdxEdge1[ k]];
			const int vtxIdx2 = indices[ i*3 + vtxIdxEdge2[ k]];
			const int remIdx = indices[ i*3 + remainVtxIdx[ k]];

			Vector3 contactPos;
			{
				Vector3 v1 = vertices[ vtxIdx2].p - vertices[ vtxIdx1].p;
				Vector3 v2 = vertices[ selectVtxIdx].p  - vertices[ vtxIdx1].p;
				v1.Normalize();
				const float len = v2.Length();
				v2.Normalize();
				contactPos = vertices[ vtxIdx1].p +  v1*(v1.DotProduct(v2) * len);
			}

			Vector3 edgeNormal;
			{
				Vector3 v1 = vertices[ vtxIdx2].p - vertices[ vtxIdx1].p;
				v1.Normalize();
				Vector3 norm = v1.CrossProduct(Vector3(0,1,0));
				norm.Normalize();

				Vector3 v2 = vertices[ remIdx].p - vertices[ vtxIdx1].p;
				v2.Normalize();

				if (v2.DotProduct(norm) > 0)
					norm = -norm;

				edgeNormal = norm;
			}

			// rotate
			Vector3 v1 = vertices[ selectVtxIdx].p  - contactPos;
			v1.Normalize();
			Quaternion quat;
			quat.SetRotationArc(v1, edgeNormal);
			Matrix44 mat = quat.GetMatrix();

			Vector3 mutaualPos = vertices[ selectVtxIdx].p - contactPos;
			Vector3 pos = mutaualPos * mat;
			pos += contactPos;
			//vertices[ selectVtxIdx].p = pos;

			g_ani = true;
			g_aniVtxIdx = selectVtxIdx;
			g_aniSrcPos = vertices[ selectVtxIdx].p;
			g_aniDestPos = pos;
		}
	}

	vtxBuff->Unlock();
	idxBuff->Unlock();



	ZeroMemory(&g_Mtrl, sizeof(g_Mtrl));
	g_Mtrl.Ambient = D3DXCOLOR(1,1,1,1);
	g_Mtrl.Diffuse = D3DXCOLOR(1,1,1,1);
	g_Mtrl.Specular = D3DXCOLOR(1,1,1,1);
	g_Mtrl.Emissive = D3DXCOLOR(0,0,0,1);
	g_Mtrl.Power = 0.f;

	D3DXCOLOR color(1,1,1,1);
	ZeroMemory(&g_Light, sizeof(g_Light));
	g_Light.Type = D3DLIGHT_DIRECTIONAL;
	g_Light.Ambient = color * 0.4f;
	g_Light.Diffuse = color;
	g_Light.Specular = color * 0.6f;
	g_Light.Direction = *(D3DXVECTOR3*)&Vector3(0,-1,0);	

	Matrix44 V;
	Vector3 camPos(0,200,-500);
	Vector3 lookAt(0, 0, 0);
	Vector3 dir = lookAt - camPos;
	dir.Normalize();
	V.SetView(camPos, dir, Vector3(0,1,0));
	g_pDevice->SetTransform(D3DTS_VIEW, (D3DXMATRIX*)&V);

	Matrix44 proj;
	proj.SetProjection(D3DX_PI * 0.5f, (float)WINSIZE_X / (float) WINSIZE_Y, 1.f, 1000.0f) ;
	g_pDevice->SetTransform(D3DTS_PROJECTION, (D3DXMATRIX*)&proj) ;

	g_pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	g_pDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);

	g_pDevice->SetLight(0, &g_Light); // 광원 설정.
	g_pDevice->LightEnable(
		0, // 활성화/ 비활성화 하려는 광원 리스트 내의 요소
		true); // true = 활성화 ， false = 비활성화

	return true;
}


//랜더
void Render(int timeDelta)
{
	//화면 청소
	if (SUCCEEDED(g_pDevice->Clear( 
		0,			//청소할 영역의 D3DRECT 배열 갯수		( 전체 클리어 0 )
		NULL,		//청소할 영역의 D3DRECT 배열 포인터		( 전체 클리어 NULL )
		D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL,	//청소될 버퍼 플레그 ( D3DCLEAR_TARGET 컬러버퍼, D3DCLEAR_ZBUFFER 깊이버퍼, D3DCLEAR_STENCIL 스텐실버퍼
		D3DCOLOR_XRGB(0,0,0),			//컬러버퍼를 청소하고 채워질 색상( 0xAARRGGBB )
		1.0f,				//깊이버퍼를 청소할값 ( 0 ~ 1 0 이 카메라에서 제일가까운 1 이 카메라에서 제일 먼 )
		0					//스텐실 버퍼를 채울값
		)))
	{
		//화면 청소가 성공적으로 이루어 졌다면... 랜더링 시작
		g_pDevice->BeginScene();

		if (g_Mesh)
		{
			if (g_ani)
			{
				if (g_aniAlpha > 1.f)
				{
					g_aniAlpha = 1.f;
					g_ani = false;
				}

				LPDIRECT3DVERTEXBUFFER9 vtxBuff;
				g_Mesh->GetVertexBuffer(&vtxBuff);
				Vertex *vertices;
				vtxBuff->Lock(0, 0, (void**)&vertices, 0);
				Vector3 v = g_aniSrcPos.Interpolate(g_aniDestPos, g_aniAlpha);
				vertices[ g_aniVtxIdx].p = v;
				vtxBuff->Unlock();

				g_aniAlpha += timeDelta * 0.0005f;
			}
		}

		//r.SetTranslate(Vector3(0, 0, -498)); // teapot
		g_pDevice->SetTransform(D3DTS_WORLD, (D3DXMATRIX*)&g_LocalTm);
		g_pDevice->SetMaterial(&g_Mtrl);
		if (g_Mesh)
			g_Mesh->DrawSubset(0);

		//랜더링 끝
		g_pDevice->EndScene();
		//랜더링이 끝났으면 랜더링된 내용 화면으로 전송
		g_pDevice->Present( NULL, NULL, NULL, NULL );
	}
}


bool ReadModelFile( const string &fileName, LPDIRECT3DVERTEXBUFFER9 &vtxBuff, int &vtxSize,  
	LPDIRECT3DINDEXBUFFER9 &idxBuff, int &faceSize )
{
	using namespace std;
	ifstream fin(fileName.c_str());
	if (!fin.is_open())
		return false;

	string exporterVersion;
	fin >>exporterVersion;
	if (exporterVersion != "EXPORTER_V1")
		return false;

	string vtx, eq;
	fin >> vtx >> eq >> vtxSize;

	if (vtxSize <= 0)
		return  false;

	vector<Vertex> tempVtxBuff;
	tempVtxBuff.reserve(vtxSize + vtxSize/2);

	float num1, num2, num3;
	for (int i = 0; i < vtxSize; i++)
	{
		fin >> num1 >> num2 >> num3;
		tempVtxBuff.push_back( Vertex(num1, num2, num3) );
	}


	// 인덱스 버퍼 초기화.
	string idx;
	fin >> idx >> eq >> faceSize;

	vector<int> tempIdxBuff;
	tempIdxBuff.reserve(faceSize);

	if (faceSize > 0)
	{
		int num1, num2, num3;
		for (int i = 0; i < faceSize*3; i+=3)
		{
			fin >> num1 >> num2 >> num3;
			tempIdxBuff.push_back(num1);
			tempIdxBuff.push_back(num2);
			tempIdxBuff.push_back(num3);
		}
	}

	string norm;
	int numNormal;
	fin >> norm >> eq >> numNormal;

	if (numNormal > 0)
	{
		float num1, num2, num3;
		vector<int> vertCount(vtxSize, 0);
		for (int i = 0; i < numNormal; i++)
		{
			fin >> num1 >> num2 >> num3;
			Vector3 n(num1, num2, num3);

			// 법선벡터의 평균을 구해서 할당한다.
			for (int k=0; k < 3; ++k)
			{
				const int vtxIdx = tempIdxBuff[ i*3 + k];
				tempVtxBuff[ vtxIdx].n += n;
				++vertCount[ vtxIdx];
			}
		}

		for (int i=0; i < vtxSize; ++i)
		{
			tempVtxBuff[ i].n /= (float)vertCount[ i];
			tempVtxBuff[ i].n.Normalize();
		}
	}


	string tex;
	int numTex;
	fin >> tex >> eq >> numTex;

	if (numTex > 0)
	{
		float fnum1, fnum2;
		vector<Vector3> texVertices(numTex);
		for (int i = 0; i < numTex; i++)
		{
			fin >> fnum1 >> fnum2;
			texVertices[ i] = Vector3(fnum1, fnum2, 0);
		}

		string strTexFace;
		int numTexFace;
		fin >> strTexFace >> eq >> numTexFace;

		vector<int> texFaces;
		texFaces.reserve(numTexFace*3);
		if (numTexFace > 0)
		{
			int num1, num2, num3;
			for (int i=0; i < numTexFace; ++i)
			{
				fin >> num1 >> num2 >> num3;
				texFaces.push_back( num1 );
				texFaces.push_back( num2 );
				texFaces.push_back( num3 );
			}
		}

		map<int, vector<int> > vtxIdxMap; // vertex index, vertex index array
		for (int i=0; i < vtxSize; ++i)
		{
			vector<int> varray;
			varray.reserve(4);
			varray.push_back(i);
			vtxIdxMap[ i] = varray;
		}

		// 텍스쳐 좌표를 버텍스 버퍼에 저장한다. 
		// 버텍스 버퍼의 uv 값이 초기화 되지 않았다면, 초기화 한다.
		// 버텍스에 하나 이상의 uv값이 존재한다면, 버텍스를 추가하고, 인덱스버퍼를 수정한다.
		for (int i=0; i < (int)texFaces.size(); ++i)
		{
			const Vector3 tex = texVertices[ texFaces[ i]];
			const int vtxIdx = tempIdxBuff[ i];

			bool isFind = false;
			for (int k=0; k < (int)vtxIdxMap[ vtxIdx].size(); ++k)
			{
				const int subVtxIdx = vtxIdxMap[ vtxIdx][ k];

				// 텍스쳐 좌표가 버텍스 버퍼에 저장되어 있다면, index buffer 값을 해당 vertex index 로
				// 설정 한다.
				if ((-100 == tempVtxBuff[ subVtxIdx].u) &&
					(-100 == tempVtxBuff[ subVtxIdx].v))
				{
					tempVtxBuff[ subVtxIdx].u = tex.x;
					tempVtxBuff[ subVtxIdx].v = tex.y;
					isFind = true;
					break;
				}
				else if ((tex.x == tempVtxBuff[ subVtxIdx].u) && 
					(tex.y == tempVtxBuff[ subVtxIdx].v))
				{
					tempIdxBuff[ i] = subVtxIdx;
					isFind = true;
					break;
				}
			}

			// 버텍스 버퍼에 없는 uv 좌표라면, 새 버텍스를 버텍스버퍼에 추가한다.
			// 인덱스 버퍼에도 새로 추가된 버텍스 인덱스값을 넣는다.
			if (!isFind)
			{
				Vertex v = tempVtxBuff[ vtxIdx];
				v.u = tex.x;
				v.v = tex.y;
				tempVtxBuff.push_back(v);
				const int newVtxIdx = tempVtxBuff.size()-1;
				vtxIdxMap[ vtxIdx].push_back( newVtxIdx );
				tempIdxBuff[ i] = newVtxIdx;
			}
		}
	}


	// 버텍스 버퍼 생성.
	if (FAILED(g_pDevice->CreateVertexBuffer( tempVtxBuff.size() * sizeof(Vertex),
		D3DUSAGE_WRITEONLY, Vertex::FVF,
		D3DPOOL_MANAGED, &vtxBuff, NULL)))
	{
		return false;
	}
	// 버텍스 버퍼 초기화.
	Vertex* vertices;
	if (FAILED(vtxBuff->Lock( 0, sizeof(Vertex), (void**)&vertices, 0)))
		return false;
	for (int i = 0; i < (int)tempVtxBuff.size(); i++)
		vertices[ i] = tempVtxBuff[ i];
	vtxBuff->Unlock();

	// 인덱스 버퍼 생성.
	if (FAILED(g_pDevice->CreateIndexBuffer(tempIdxBuff.size()*sizeof(WORD), 
		D3DUSAGE_WRITEONLY,
		D3DFMT_INDEX16,
		D3DPOOL_MANAGED,
		&idxBuff, NULL)))
	{
		return false;
	}
	WORD *indices = NULL;
	idxBuff->Lock(0, 0, (void**)&indices, 0);
	for (int i = 0; i < (int)tempIdxBuff.size(); ++i)
		indices[ i] = tempIdxBuff[ i];
	idxBuff->Unlock();

	return true;
}
