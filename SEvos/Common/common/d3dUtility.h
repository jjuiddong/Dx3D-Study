//////////////////////////////////////////////////////////////////////////////////////////////////
// 
// File: d3dutility.h
// 
// Author: Frank Luna (C) All Rights Reserved
//
// System: AMD Athlon 1800+ XP, 512 DDR, Geforce 3, Windows XP, MSVC++ 7.0 
//
// Desc: Provides utility functions for simplifying common tasks.
//          
//////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __d3dUtilityH__
#define __d3dUtilityH__

#pragma warning (disable: 4786)
#include <d3dx9.h>
#include <string>
#include <limits>

class CScene;
class Box;

namespace d3d
{
	//
	// Init
	//
	HWND InitD3D(
		HINSTANCE hInstance,       // [in] Application instance.
		int width, int height,     // [in] Backbuffer dimensions.
		bool windowed,             // [in] Windowed (true)or full screen (false).
		D3DDEVTYPE deviceType,     // [in] HAL or REF
		IDirect3DDevice9** device);// [out]The created device.

	BOOL InitDX3D( HWND hwnd, int width, int height, 
		bool windowed, D3DDEVTYPE deviceType, IDirect3DDevice9** device /*out*/ );

	int EnterMsgLoop( CScene *pScene,
		bool (*ptr_display)(int timeDelta));

//	LRESULT CALLBACK WndProc( HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

	//
	// Cleanup
	//
	template<class T> void Release(T t)
	{
		if( t )
		{
			t->Release();
			t = 0;
		}
	}
		
	template<class T> void Delete(T t)
	{
		if( t )
		{
			delete t;
			t = 0;
		}
	}

	//
	// Colors
	//
	const D3DXCOLOR      WHITE( D3DCOLOR_XRGB(255, 255, 255) );
	const D3DXCOLOR      BLACK( D3DCOLOR_XRGB(  0,   0,   0) );
	const D3DXCOLOR        RED( D3DCOLOR_XRGB(255,   0,   0) );
	const D3DXCOLOR      GREEN( D3DCOLOR_XRGB(  0, 255,   0) );
	const D3DXCOLOR       BLUE( D3DCOLOR_XRGB(  0,   0, 255) );
	const D3DXCOLOR     YELLOW( D3DCOLOR_XRGB(255, 255,   0) );
	const D3DXCOLOR       CYAN( D3DCOLOR_XRGB(  0, 255, 255) );
	const D3DXCOLOR    MAGENTA( D3DCOLOR_XRGB(255,   0, 255) );

	//
	// Lights
	//

	D3DLIGHT9 InitDirectionalLight(D3DXVECTOR3* direction, D3DXCOLOR* color);
	D3DLIGHT9 InitPointLight(D3DXVECTOR3* position, D3DXCOLOR* color);
	D3DLIGHT9 InitSpotLight(D3DXVECTOR3* position, D3DXVECTOR3* direction, D3DXCOLOR* color);

	//
	// Materials
	//

	D3DMATERIAL9 InitMtrl(D3DXCOLOR a, D3DXCOLOR d, D3DXCOLOR s, D3DXCOLOR e, float p);

	const D3DMATERIAL9 WHITE_MTRL  = InitMtrl(WHITE, WHITE, WHITE, BLACK, 2.0f);
	const D3DMATERIAL9 RED_MTRL    = InitMtrl(RED, RED, RED, BLACK, 2.0f);
	const D3DMATERIAL9 GREEN_MTRL  = InitMtrl(GREEN, GREEN, GREEN, BLACK, 2.0f);
	const D3DMATERIAL9 BLUE_MTRL   = InitMtrl(BLUE, BLUE, BLUE, BLACK, 2.0f);
	const D3DMATERIAL9 YELLOW_MTRL = InitMtrl(YELLOW, YELLOW, YELLOW, BLACK, 2.0f);

	//
	// Bounding Objects
	//

	struct BoundingBox
	{
		BoundingBox();

		bool isPointInside(D3DXVECTOR3& p);

		D3DXVECTOR3 _min;
		D3DXVECTOR3 _max;
	};

	struct BoundingSphere
	{
		BoundingSphere();

		D3DXVECTOR3 _center;
		float       _radius;
	};

	//
	// Constants
	//

	const float INFINITY = FLT_MAX;
	const float EPSILON  = 0.001f;

	//
	// Drawing
	//

	//
	// Randomness
	//

	// Desc: Return random float in [lowBound, highBound] interval.
	float GetRandomFloat(float lowBound, float highBound);
	
	// Desc: Returns a random vector in the bounds specified by min and max.
	void GetRandomVector( D3DXVECTOR3* out, D3DXVECTOR3* min, D3DXVECTOR3* max);

	//
	// Conversion
	//
	DWORD FtoDw(float f);


	// Etc
	void DrawXYZ();

	void GetPickRay( int nX, int nY, Vector3* pvOrig, Vector3* pvDir );

	// Bounding Box 持失
	void CreateBox( SMeshLoader *pLoader, Box *pReval );
	// Bounding Sphere 持失
	void CreateSphere( Box *pBox, Sphere *pReval );
	// LPD3DXFONT 持失
	HRESULT CreateD3DXFont( IDirect3DDevice9 *pDevice, LPD3DXFONT* ppd3dxFont, TCHAR* pstrFont, DWORD dwSize );

	static int g_ObjId = 0;
	int CreateObjectId();
}

////////////////////////////////////////////////////////////////////////////////
#define PI	3.141592f
static float AngleToRad( float angle ) { return (((float)angle*(float)PI)/180.f); }

// MACROS
#define SAFE_RELEASE(p)		if(p){ (p)->Release(); p = NULL; }


#endif // __d3dUtilityH__
