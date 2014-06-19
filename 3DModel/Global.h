//-----------------------------------------------------------------------------//
// 2005-12-01  programer: jaejung ┏(⊙д⊙)┛
// 
//-----------------------------------------------------------------------------//

#if !defined(__GLOBAL_H__)
#define __GLOBAL_H__

#define _AFX_SECURE_NO_WARNINGS 
#define _ATL_SECURE_NO_WARNINGS

#pragma warning (disable: 4996)	// strcpy 경고 제거


#include <windows.h>
#pragma warning (disable: 4786)
#include <string>
#include <list>
#include <queue>
#include <map>
#include "d3dx9.h"
#include "d3dx9tex.h"
#include "math.h"
#include "structdef.h"
#include "d3dutility.h"
#include "combo.h"
//#include "control.h"
#include "script.h"
#include "script_machine.h"
#include "terrain.h"

#include "dbg.h"

//#define _TEST_MODE	1	// 게임개발중 테스트를 위해서 정의됨, 릴리즈시 제거된다.
#define _PRINT_FRAME	1	// 프레임 출력

extern int g_test;

extern IDirect3DDevice9 *g_pDevice;
extern ns_script::CScript g_Scr;

extern int g_CurTime;	// 현재 시간 (millisecond단위 timeGetTime() 값)
extern int g_DeltaTime;	// 전프레임에서 경과 시간
extern D3DXMATRIXA16 g_matIdentity;	// 단위행렬
extern Vector3 g_DefaultCameraPos;


const float VECTOR_EPSILON = 0.05f;

// MACROS
#define SAFE_RELEASE(p)		if(p){ (p)->Release(); p = NULL; }
#define SAFE_DELETE(p)		{if(p){ delete (p); (p) = NULL; }}
#define SAFE_ADELETE(p)		{if(p){ delete[] (p); (p) = NULL; }} // array delete


// 본에 저장된 충돌박스 노드
static char *g_CollisionNodeName = "Bip01 Spine1";
static char *g_BoneCenterNodeName = "Bip01";

// Animation Frame 수정
// 3DMax 에서는 1초에 30 frame 을 기준으로 작업하기 때문에
// 1초에 1000frame을 사용하는 게임프로그램에 맞게 수정되어야 한다.
const float FRAME = (1000.f / 30.f);



#endif // __JGLOBAL_H__
