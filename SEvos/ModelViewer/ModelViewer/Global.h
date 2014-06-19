//-----------------------------------------------------------------------------//
// 2005-12-01  programer: jaejung ┏(⊙д⊙)┛
// 
//-----------------------------------------------------------------------------//

#if !defined(__GLOBAL_H__)
#define __GLOBAL_H__

#include "stdafx.h"
#include <windows.h>
#pragma warning (disable: 4786)
#include <string>
#include <list>
#include <queue>
#include <map>
#include <Mmsystem.h>
#include "d3dx9.h"
#include "d3dx9tex.h"

#include "math/math.h"

#include "game/combo.h"

#include "common/structdef.h"
#include "common/d3dutility.h"
#include "common/camera.h"
#include "common/frustum.h"
#include "common/linearmemloader.h"
#include "common/fileloader.h"
#include "common/mmtimers.h"
#include "common/dbg.h"

#include "game/scene.h"
#include "scriptlib/script.h"
#include "scriptlib/script_machine.h"

#include "model//mesh.h"



//#define _TEST_MODE	1	// 게임개발중 테스트를 위해서 정의됨, 릴리즈시 제거된다.
#define _PRINT_FRAME	1	// 프레임 출력

extern int g_test;

extern IDirect3DDevice9 *g_pDevice;
extern ns_script::CScript g_Scr;

extern int g_CurTime;	// 현재 시간 (millisecond단위 timeGetTime() 값)
extern int g_DeltaTime;	// 전프레임에서 경과 시간
extern D3DXMATRIXA16 g_matIdentity;	// 단위행렬
extern Vector3 g_DefaultCameraPos;
extern RECT g_ViewPort;
extern CFrustum g_Frustum;


const float VECTOR_EPSILON = 0.05f;

// MACROS
#define SAFE_RELEASE(p)		if(p){ (p)->Release(); p = NULL; }
#define SAFE_DELETE(p)		{if(p){ delete (p); (p) = NULL; }}
#define SAFE_ADELETE(p)		{if(p){ delete[] (p); (p) = NULL; }} // array delete
#define IS_IN_RANGE(value,r0,r1) (( ((r0) <= (value)) && ((value) <= (r1)) ) ? 1 : 0)

// 본에 저장된 충돌박스 노드
static char *g_CollisionNodeName = "Bip01 Spine1";
static char *g_BoneCenterNodeName = "Bip01";

// Animation Frame 수정
// 3DMax 에서는 1초에 30 frame 을 기준으로 작업하기 때문에
// 1초에 1000frame을 사용하는 게임프로그램에 맞게 수정되어야 한다.
const float FRAME = (1000.f / 30.f);



/////////////////////////////////////////////////////////////////////////////////
// definition


// object 속성
enum OBJ_TYPE
{ 
	OT_TILE,
	OT_MODEL,
	OT_RIGID,
};

// MapEditor 상태
enum EDIT_MODE
{
	EM_NORMAL,	// 기본 상태
	EM_TILE,	// 타일 생성 모드
	EM_TERRAIN,	// 지형 에디트 모드
	EM_MODEL,	// 모델 추가 모드
};

// MapView Cursor Type
enum CURSOR_TYPE
{
	CT_SQUARE,
	CT_CIRCLE,
};

typedef struct _tagSCrTerrain
{
	int nWidth;
	int nHeight;
	int nVtxPerRow;
	int nVtxPerCol;
	char szTexture[ MAX_PATH];

} SCrTerrain;

#pragma pack(1) // 1byte 정렬
	typedef struct _tagSTModelLoader
	{
		_tagSTModelLoader() {}
		_tagSTModelLoader( Vector3 v, int n, char *s ) : vPos(v), nModelIdx(n)
		{ if(s) strcpy_s( szObjName, sizeof(szObjName),s ); }

		Vector3 vPos;
		int nModelIdx; // 아직 안씀
		char szObjName[ 64];

	} STModelLoader;

	typedef struct _tagSTerrainLoader
	{
		int nWidth;
		int nHeight;
		int nVtxPerRow;
		int nVtxPerCol;
		int nCellPerRow;
		int nCellPerCol;
		int nCellSize;
		int nMapSize;
		float *pMap;
		int nRigidSize;
		STModelLoader *pRigid;
		int nDynSize;
		STModelLoader *pDyn;

	} STerrainLoader;
#pragma pack() // default


//class CTerrainPanel;
class CModelView;
class CBasePanelFrame;
//class CTilePanel;
//extern CTerrainPanel *g_pTerrainPanel;
//extern CTilePanel *g_pTilePanel;
extern CBasePanelFrame *g_pBasePanelFrm;
extern CModelView *g_pModelView;

//extern EDIT_MODE g_eEditMode;
//const static char *g_szTilePath = "data//map//tile";
//const static char *g_szModelTable = "data//model";



#endif // __JGLOBAL_H__
