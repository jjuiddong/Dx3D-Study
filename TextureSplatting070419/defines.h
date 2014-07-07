//#include <mmsystem.h>
#include <stdio.h>
#include <windows.h>


#include <d3d9.h>
#include <d3dx9.h>

#pragma comment(lib,"d3d9.lib")
#pragma comment(lib,"d3dx9.lib")


#define MAPWIDTH		32
#define MAPHEIGHT		32
#define WIDTH			800
#define HEIGHT			600
#define MAXTEXNUM		4
#define TEXALPHASIZE	512
#define MINIALPHASIZE	128



//유틸리티 매크로

#define Random(num) (int)(((long)rand() * (num)) / (RAND_MAX + 1))
#define Randomize() srand((unsigned)time(NULL))
#define RandomA(s,e) ((s) + (Random((e) - (s) + 1)))

#define SAFE_DELETE_(p)       {if(p) {delete (p);     (p)=NULL;}}
#define SAFE_DELETE_ARRAY_(p) {if(p) {delete[] (p);   (p)=NULL;}}
#define SAFE_RELEASE_(p)      {if(p) {(p)->Release(); (p)=NULL;}}




/*
#define D3DFVF_TEXTUREVERTEX			( D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1)
#define D3DFVF_CUBEVERTEX				( D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1)
#define D3DFVF_BILLBOARDVERTEX			( D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1)
#define D3DFVF_TERRAIN_FORMAT1_VERTEX	( D3DFVF_XYZ | D3DFVF_TEX1)
#define D3DFVF_CYLINDERVERTEX			( D3DFVF_XYZ | D3DFVF_NORMAL )

//키보드 상태
#define MOVE_LEFT					((BYTE) 0x01)
#define MOVE_RIGHT					((BYTE) 0x02)
#define MOVE_UP						((BYTE) 0x03)	
#define MOVE_DOWN		 			((BYTE) 0x04)
#define ROTATION_LEFT_Y				((BYTE) 0x05)
#define ROTATION_RIGHT_Y			((BYTE) 0x06)
#define ROTATION_UP_X				((BYTE) 0x07)
#define ROTATION_DOWN_X				((BYTE) 0x08)
#define ZOOM_IN						((BYTE) 0x09)
#define ZOOM_OUT					((BYTE) 0x10)


//마우스상태
#define MOUSE_CURSER_UP				((BYTE) 0x01)
#define MOUSE_CURSER_DOWN			((BYTE) 0x02)
#define MOUSE_CURSER_LEFT			((BYTE) 0x03)
#define MOUSE_CURSER_RIGHT			((BYTE) 0x04)


#define DRAW_OPTION_TEX				((BYTE) 0x00)
#define DRAW_OPTION_BOX				((BYTE) 0x01)

#define DRAW_TYPE_BOX				((BYTE) 0x00)
#define DRAW_TYPE_BILLBOARD			((BYTE) 0x01)


#define ITEMTYPE_IMAGE				((BYTE) 0x00)
#define ITEMTYPE_TEXTURE			((BYTE) 0x01)
#define ITEMTYPE_COLLISION			((BYTE) 0x02)

//충돌상태

#define COLLISIONUP					((BYTE) 0x01)
#define COLLISIONDOWN				((BYTE) 0x02)
#define COLLISIONLEFT				((BYTE) 0x03)
#define COLLISIONRIGHT				((BYTE) 0x04)
#define COLLISIONHEIGHTUP			((BYTE) 0x05)
#define COLLISIONHEIGHTDOWN			((BYTE) 0x06)*/




