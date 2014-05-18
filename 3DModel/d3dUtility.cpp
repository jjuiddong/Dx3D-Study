//////////////////////////////////////////////////////////////////////////////////////////////////
// 
// File: d3dutility.cpp
// 
// Author: Frank Luna (C) All Rights Reserved
//
// System: AMD Athlon 1800+ XP, 512 DDR, Geforce 3, Windows XP, MSVC++ 7.0 
//
// Desc: Provides utility functions for simplifying common tasks.
//          
//////////////////////////////////////////////////////////////////////////////////////////////////

#include "global.h"
#include "d3dUtility.h"
#include "scene.h"


HWND d3d::InitD3D( HINSTANCE hInstance, int width, int height, bool windowed, 
				   D3DDEVTYPE deviceType, IDirect3DDevice9** device)
{
	//
	// Create the main application window.
	//

	WNDCLASS wc;

	wc.style         = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc   = (WNDPROC)d3d::WndProc; 
	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = 0;
	wc.hInstance     = hInstance;
	wc.hIcon         = LoadIcon(0, IDI_APPLICATION);
	wc.hCursor       = LoadCursor(0, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wc.lpszMenuName  = 0;
	wc.lpszClassName = "Direct3D9App";

	if( !RegisterClass(&wc) )
	{
		::MessageBox(0, "RegisterClass() - FAILED", 0, 0);
		return NULL;
	}
		
	HWND hwnd = 0;
	hwnd = ::CreateWindow("Direct3D9App", "Direct3D9App", 
//		WS_EX_TOPMOST,
		WS_POPUP,
		0, 0, width, height,
		0 /*parent hwnd*/, 0 /* menu */, hInstance, 0 /*extra*/); 

	if( !hwnd )
	{
		::MessageBox(0, "CreateWindow() - FAILED", 0, 0);
		return NULL;
	}

	::ShowWindow(hwnd, SW_SHOW);
	::UpdateWindow(hwnd);

	//
	// Init D3D: 
	//

	HRESULT hr = 0;

	// Step 1: Create the IDirect3D9 object.
	if( !InitDX3D( hwnd, width, height, windowed, deviceType, device) )
		return NULL;

/*
	IDirect3D9* d3d9 = 0;
    d3d9 = Direct3DCreate9(D3D_SDK_VERSION);

    if( !d3d9 )
	{
		::MessageBox(0, "Direct3DCreate9() - FAILED", 0, 0);
		return NULL;
	}

	// Step 2: Check for hardware vp.

	D3DCAPS9 caps;
	d3d9->GetDeviceCaps(D3DADAPTER_DEFAULT, deviceType, &caps);

	int vp = 0;
	if( caps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT )
		vp = D3DCREATE_HARDWARE_VERTEXPROCESSING;
	else
		vp = D3DCREATE_SOFTWARE_VERTEXPROCESSING;

	// Step 3: Fill out the D3DPRESENT_PARAMETERS structure.
 
	D3DPRESENT_PARAMETERS d3dpp;
	d3dpp.BackBufferWidth            = width;
	d3dpp.BackBufferHeight           = height;
	d3dpp.BackBufferFormat           = D3DFMT_A8R8G8B8;
	d3dpp.BackBufferCount            = 1;
	d3dpp.MultiSampleType            = D3DMULTISAMPLE_NONE;
	d3dpp.MultiSampleQuality         = 0;
	d3dpp.SwapEffect                 = D3DSWAPEFFECT_DISCARD; 
	d3dpp.hDeviceWindow              = hwnd;
	d3dpp.Windowed                   = windowed;
	d3dpp.EnableAutoDepthStencil     = true; 
	d3dpp.AutoDepthStencilFormat     = D3DFMT_D24S8;
	d3dpp.Flags                      = 0;
	d3dpp.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;
	d3dpp.PresentationInterval       = D3DPRESENT_INTERVAL_IMMEDIATE;

	// Step 4: Create the device.

	hr = d3d9->CreateDevice(
		D3DADAPTER_DEFAULT, // primary adapter
		deviceType,         // device type
		hwnd,               // window associated with device
		vp,                 // vertex processing
	    &d3dpp,             // present parameters
	    device);            // return created device

	if( FAILED(hr) )
	{
		// try again using a 16-bit depth buffer
		d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
		
		hr = d3d9->CreateDevice( D3DADAPTER_DEFAULT, deviceType, hwnd, vp, &d3dpp, device );
		if( FAILED(hr) )
		{
			d3d9->Release(); // done with d3d9 object
			::MessageBox(0, "CreateDevice() - FAILED", 0, 0);
			return NULL;
		}
	}

	d3d9->Release(); // done with d3d9 object
/**/
	
	return hwnd;
}


//-----------------------------------------------------------------------------//
// DirectX 3D Init
//-----------------------------------------------------------------------------//
BOOL d3d::InitDX3D( HWND hwnd, int width, int height, 
		bool windowed, D3DDEVTYPE deviceType, IDirect3DDevice9** device )
{
	HRESULT hr = 0;

	// Step 1: Create the IDirect3D9 object.

	IDirect3D9* d3d9 = 0;
    d3d9 = Direct3DCreate9(D3D_SDK_VERSION);

    if( !d3d9 )
	{
		::MessageBox(0, "Direct3DCreate9() - FAILED", 0, 0);
		return FALSE;
	}

	// Step 2: Check for hardware vp.

	D3DCAPS9 caps;
	d3d9->GetDeviceCaps(D3DADAPTER_DEFAULT, deviceType, &caps);

	int vp = 0;
	if( caps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT )
		vp = D3DCREATE_HARDWARE_VERTEXPROCESSING;
	else
		vp = D3DCREATE_SOFTWARE_VERTEXPROCESSING;

	// Step 3: Fill out the D3DPRESENT_PARAMETERS structure.
 
	D3DPRESENT_PARAMETERS d3dpp;
	d3dpp.BackBufferWidth            = width;
	d3dpp.BackBufferHeight           = height;
	d3dpp.BackBufferFormat           = D3DFMT_A8R8G8B8;
	d3dpp.BackBufferCount            = 1;
	d3dpp.MultiSampleType            = D3DMULTISAMPLE_NONE;
	d3dpp.MultiSampleQuality         = 0;
	d3dpp.SwapEffect                 = D3DSWAPEFFECT_DISCARD; 
	d3dpp.hDeviceWindow              = hwnd;
	d3dpp.Windowed                   = windowed;
	d3dpp.EnableAutoDepthStencil     = TRUE; 
	d3dpp.AutoDepthStencilFormat     = D3DFMT_D24S8;
	d3dpp.Flags                      = 0;
	d3dpp.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;
	d3dpp.PresentationInterval       = D3DPRESENT_INTERVAL_IMMEDIATE;

	// Step 4: Create the device.

	hr = d3d9->CreateDevice(
		D3DADAPTER_DEFAULT, // primary adapter
		deviceType,         // device type
		hwnd,               // window associated with device
		vp,                 // vertex processing
	    &d3dpp,             // present parameters
	    device);            // return created device

	if( FAILED(hr) )
	{
		// try again using a 16-bit depth buffer
		d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
		
		hr = d3d9->CreateDevice( D3DADAPTER_DEFAULT, deviceType, hwnd, vp, &d3dpp, device );
		if( FAILED(hr) )
		{
			d3d9->Release(); // done with d3d9 object
			::MessageBox(0, "CreateDevice() - FAILED", 0, 0);
			return FALSE;
		}
	}

	d3d9->Release(); // done with d3d9 object

	return TRUE;
}


int d3d::EnterMsgLoop( CScene *pScene, bool (*ptr_display)(int timeDelta) )
{
	MSG msg;
	::ZeroMemory(&msg, sizeof(MSG));

	static float lastTime = (float)timeGetTime();
	static int frame = 0;
	static int frame_time = 0;

	while(msg.message != WM_QUIT)
	{
		if(::PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
			pScene->MessageProc( &msg );
		}	
		else
        {
			g_CurTime   = timeGetTime();
			g_DeltaTime = (g_CurTime - lastTime);

#if defined(_PRINT_FRAME)
			if( (int)g_CurTime - frame_time > 5000 )
			{
				g_Dbg.Console( "--------------------------------------------\n" );
				g_Dbg.Console( "frame: %1.1f\n", (float)(frame / 5.f) );
				g_Dbg.Console( "--------------------------------------------\n" );
				frame_time = g_CurTime;
				frame = 0;
			}
#endif // _PRINT_FRAME

			if( g_DeltaTime < 10 )
			{
				Sleep( 1 );
			}
			else
			{
#if defined(_PRINT_FRAME)
				++frame;
#endif // _PRINT_FRAME
				ptr_display(g_DeltaTime);
				lastTime = g_CurTime;
			}
        }
    }
    return msg.wParam;
}

D3DLIGHT9 d3d::InitDirectionalLight(D3DXVECTOR3* direction, D3DXCOLOR* color)
{
	D3DLIGHT9 light;
	::ZeroMemory(&light, sizeof(light));

	light.Type      = D3DLIGHT_DIRECTIONAL;
	light.Ambient   = *color * 0.4f;
	light.Diffuse   = *color;
	light.Specular  = *color * 0.6f;
	light.Direction = *direction;

	return light;
}

D3DLIGHT9 d3d::InitPointLight(D3DXVECTOR3* position, D3DXCOLOR* color)
{
	D3DLIGHT9 light;
	::ZeroMemory(&light, sizeof(light));

	light.Type      = D3DLIGHT_POINT;
	light.Ambient   = *color * 0.4f;
	light.Diffuse   = *color;
	light.Specular  = *color * 0.6f;
	light.Position  = *position;
	light.Range        = 1000.0f;
	light.Falloff      = 1.0f;
	light.Attenuation0 = 1.0f;
	light.Attenuation1 = 0.0f;
	light.Attenuation2 = 0.0f;

	return light;
}

D3DLIGHT9 d3d::InitSpotLight(D3DXVECTOR3* position, D3DXVECTOR3* direction, D3DXCOLOR* color)
{
	D3DLIGHT9 light;
	::ZeroMemory(&light, sizeof(light));

	light.Type      = D3DLIGHT_SPOT;
	light.Ambient   = *color * 0.4f;
	light.Diffuse   = *color;
	light.Specular  = *color * 0.6f;
	light.Position  = *position;
	light.Direction = *direction;
	light.Range        = 1000.0f;
	light.Falloff      = 1.0f;
	light.Attenuation0 = 1.0f;
	light.Attenuation1 = 0.0f;
	light.Attenuation2 = 0.0f;
	light.Theta        = 0.5f;
	light.Phi          = 0.7f;

	return light;
}

D3DMATERIAL9 d3d::InitMtrl(D3DXCOLOR a, D3DXCOLOR d, D3DXCOLOR s, D3DXCOLOR e, float p)
{
	D3DMATERIAL9 mtrl;
	mtrl.Ambient  = a;
	mtrl.Diffuse  = d;
	mtrl.Specular = s;
	mtrl.Emissive = e;
	mtrl.Power    = p;
	return mtrl;
}

d3d::BoundingBox::BoundingBox()
{
	// infinite small 
	_min.x = d3d::INFINITY;
	_min.y = d3d::INFINITY;
	_min.z = d3d::INFINITY;

	_max.x = -d3d::INFINITY;
	_max.y = -d3d::INFINITY;
	_max.z = -d3d::INFINITY;
}

bool d3d::BoundingBox::isPointInside(D3DXVECTOR3& p)
{
	if( p.x >= _min.x && p.y >= _min.y && p.z >= _min.z &&
		p.x <= _max.x && p.y <= _max.y && p.z <= _max.z )
	{
		return true;
	}
	else
	{
		return false;
	}
}

d3d::BoundingSphere::BoundingSphere()
{
	_radius = 0.0f;
}

float d3d::GetRandomFloat(float lowBound, float highBound)
{
	if( lowBound >= highBound ) // bad input
		return lowBound;

	// get random float in [0, 1] interval
	float f = (rand() % 10000) * 0.0001f; 

	// return float in [lowBound, highBound] interval. 
	return (f * (highBound - lowBound)) + lowBound; 
}

void d3d::GetRandomVector(
	  D3DXVECTOR3* out,
	  D3DXVECTOR3* min,
	  D3DXVECTOR3* max)
{
	out->x = GetRandomFloat(min->x, max->x);
	out->y = GetRandomFloat(min->y, max->y);
	out->z = GetRandomFloat(min->z, max->z);
}

DWORD d3d::FtoDw(float f)
{
	return *((DWORD*)&f);
}


//-----------------------------------------------------------------------------//
// Bouding Box를 생성해서 리턴한다.
// pLoader : 대상 mesh
//-----------------------------------------------------------------------------//
void d3d::CreateBox( SMeshLoader *pLoader, Box *pReval )
{
	int stride, size;
	BYTE *p;
	if( 0 < pLoader->vn.size ) // FVF = D3DFVF_XYZ | D3DFVF_NORMAL
	{
		stride = sizeof( SVtxNorm );
		size = pLoader->vn.size;
		p = (BYTE*)pLoader->vn.pV;
	}
	else // FVF = D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1
	{
		stride = sizeof( SVtxNormTex );
		size = pLoader->vnt.size;
		p = (BYTE*)pLoader->vnt.pV;
	}
	if( 0 >= size ) 
	{
		pReval->SetBox( &Vector3(0,0,0), &Vector3(0,0,0) );
		return;
	}

	Vector3 _min = *(Vector3*)(p+(0*stride));
	Vector3 _max = *(Vector3*)(p+(1*stride));
	for( int i=0; i < size ; i++ )
	{
		Vector3 *v = (Vector3*)(p+(i*stride));

		if( _min.x > v->x )
			_min.x = v->x;
		else if( _max.x < v->x )
			_max.x = v->x;
		if( _min.y > v->y )
			_min.y = v->y;
		else if( _max.y < v->y )
			_max.y = v->y;
		if( _min.z > v->z )
			_min.z = v->z;
		else if( _max.z < v->z )
			_max.z = v->z;
	}

	pReval->SetBox( &_min, &_max );
}


//-----------------------------------------------------------------------------//
// Bounding Sphere를 생성한다.
// pBox의 중점을 잡아 가장 먼 꼭지점을 반지름으로 하는 Sphere를 생성한다.
//-----------------------------------------------------------------------------//
void d3d::CreateSphere( Box *pBox, Sphere *pReval )
{
	if( !pBox ) return;

	Vector3 center;
	center.x = (pBox->m_Min.x + pBox->m_Max.x) / 2.f;
	center.y = (pBox->m_Min.y + pBox->m_Max.y) / 2.f;
	center.z = (pBox->m_Min.z + pBox->m_Max.z) / 2.f;

	float r = 0;
	if( r < pBox->m_Max.x - center.x )
		r = pBox->m_Max.x - center.x;
	if( r < pBox->m_Max.y - center.y )
		r = pBox->m_Max.y - center.y;
	if( r < pBox->m_Max.z - center.z )
		r = pBox->m_Max.z - center.z;

	pReval->SetSphere( r, center );
}



//-----------------------------------------------------------------------------//
// unique한 오브젝트 아이디를 생성한다.
//-----------------------------------------------------------------------------//
int d3d::CreateObjectId()
{
	return g_ObjId++;
}


//-----------------------------------------------------------------------------//
// LPD3DXFONT 를 생성한다.
//-----------------------------------------------------------------------------//
HRESULT d3d::CreateD3DXFont( IDirect3DDevice9 *pDevice, LPD3DXFONT* ppd3dxFont, TCHAR* pstrFont, DWORD dwSize )
{
/*    HRESULT hr;
    LPD3DXFONT pd3dxFontNew = NULL;
    HDC hDC;
    INT nHeight;
    HFONT hFont;

    hDC = GetDC( NULL );
    nHeight = -MulDiv( dwSize, GetDeviceCaps(hDC, LOGPIXELSY), 72 );
    ReleaseDC( NULL, hDC );
    hFont = CreateFont( nHeight, 0, 0, 0, FW_DONTCARE, false, false, false, 
        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
        DEFAULT_PITCH | FF_DONTCARE, pstrFont );
    if( hFont == NULL )
        return E_FAIL;
    hr = D3DXCreateFont( pDevice, hFont, &pd3dxFontNew );
    DeleteObject( hFont );

    if( SUCCEEDED( hr ) )
        *ppd3dxFont = pd3dxFontNew;
/**/
//    return hr;
	return NULL;
}
