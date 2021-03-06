// MultiBlending.cpp : 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"
#include "etc/d3dUtility.h"


IDirect3DDevice9 *g_pDevice;
const int WIDTH = 640;
const int HEIGHT = 480;
D3DXVECTOR3 g_vAxis(0,0,0);
float g_fAngle = 0;
D3DXMATRIX g_mCamera;

ID3DXMesh *g_pMesh;
IDirect3DVertexBuffer9 *g_pvtxBuff1;
IDirect3DVertexBuffer9 *g_pvtxBuff2;
IDirect3DIndexBuffer9 *g_pidxBuff1;
D3DMATERIAL9 g_Mtrl1;
D3DMATERIAL9 g_Mtrl2;

IDirect3DTexture9 *g_pTex1;
IDirect3DTexture9 *g_pTex2;
IDirect3DTexture9 *g_pAlphaTex1;

float g_fLightAngle = 0;
const int alphaWidth = 64;
const int alphaHeight = 64;

typedef struct D3DLVERTEX
{
	enum { FVF =  D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1 }; // 5
	Vector3 v;
	Vector3 n;
	float tu, tv;
	D3DLVERTEX() {}
	D3DLVERTEX(const Vector3 &v0, const Vector3 &n0, DWORD color0, float tu0, float tv0)
	{
		v = v0;
		n = n0;
		tu = tu0;
		tv = tv0;
	}
} D3DLVERTEX, *LPD3DLVERTEX;

void InitMaterial();
bool Display( float timeDelta );

BOOL Setup()
{
	InitMaterial();

	D3DXCreateTeapot( g_pDevice, &g_pMesh, 0 );
	//////////////////////////////////////////////////////////////////////////////

	D3DXVECTOR3 pos(0,0,-5);
	D3DXVECTOR3 target(0,0,0);
	D3DXVECTOR3 up(0,1,0);
	D3DXMatrixLookAtLH( &g_mCamera, &pos, &target, &up );
	g_pDevice->SetTransform( D3DTS_VIEW, &g_mCamera );

	D3DXMATRIX proj;
	//	D3DXMatrixPerspectiveFovLH( &proj, D3DX_PI *.5f, (float)WIDTH/(float)HEIGHT, 1.0f, 1000.0f );
	D3DXMatrixPerspectiveFovLH( &proj, D3DX_PI *.25f, (float)WIDTH/(float)HEIGHT, 1.0f, 1000.0f );
	g_pDevice->SetTransform( D3DTS_PROJECTION, &proj );

	D3DXCreateTextureFromFile( g_pDevice, "crate.jpg", &g_pTex1);
	D3DXCreateTextureFromFile( g_pDevice, "Maksim022.bmp", &g_pTex2);
	D3DXCreateTextureFromFile( g_pDevice, "alpha2.jpg", &g_pAlphaTex1);
	//g_pDevice->CreateTexture( alphaWidth, alphaHeight, 1, 0, D3DFMT_A8, D3DPOOL_MANAGED, &g_pAlphaTex1, NULL );


	g_pDevice->CreateVertexBuffer( 6*sizeof(SVtxNormTex), 0, D3DLVERTEX::FVF, D3DPOOL_MANAGED, &g_pvtxBuff1, NULL );
	g_pDevice->CreateVertexBuffer( 6*sizeof(SVtxNormTex), 0, D3DLVERTEX::FVF, D3DPOOL_MANAGED, &g_pvtxBuff2, NULL );

	D3DLVERTEX *pv;
	// A vertex
	g_pvtxBuff1->Lock(0, 0, (void**)&pv, 0);
	pv[ 0] = D3DLVERTEX(Vector3(-1.0f, -1.0f, .25f), Vector3(0,0,-1), D3DXCOLOR(1,0,0,0), 0, 1);
	pv[ 1] = D3DLVERTEX(Vector3(-1.0f,  1.0f, .25f), Vector3(0,0,-1), D3DXCOLOR(1,0,0,0), 0, 0);
	pv[ 2] = D3DLVERTEX(Vector3( 1.0f,  1.0f, .25f), Vector3(0,0,-1), D3DXCOLOR(1,0,0,0), 1, 0);
	pv[ 3] = D3DLVERTEX(Vector3(-1.0f, -1.0f, .25f), Vector3(0,0,-1), D3DXCOLOR(1,0,0,0), 0, 1);
	pv[ 4] = D3DLVERTEX(Vector3( 1.0f,  1.0f, .25f), Vector3(0,0,-1), D3DXCOLOR(1,0,0,0), 1, 0);
	pv[ 5] = D3DLVERTEX(Vector3( 1.0f, -1.0f, .25f), Vector3(0,0,-1), D3DXCOLOR(1,0,0,0), 1, 1);
	g_pvtxBuff1->Unlock();

	// B vertex
	g_pvtxBuff2->Lock(0, 0, (void**)&pv, 0);
	pv[ 0] = D3DLVERTEX(Vector3(1.0f, -1.0f, 1.25f), Vector3(0,0,-1), D3DXCOLOR(0,0,1,0), 0, 1);
	pv[ 1] = D3DLVERTEX(Vector3(1.0f,  1.0f, 1.25f), Vector3(0,0,-1), D3DXCOLOR(0,0,1,0), 0, 0);
	pv[ 2] = D3DLVERTEX(Vector3(3.0f,  1.0f, 1.25f), Vector3(0,0,-1), D3DXCOLOR(0,0,1,0), 1, 0);
	pv[ 3] = D3DLVERTEX(Vector3(1.0f, -1.0f, 1.25f), Vector3(0,0,-1), D3DXCOLOR(0,0,1,0), 0, 1);
	pv[ 4] = D3DLVERTEX(Vector3(3.0f,  1.0f, 1.25f), Vector3(0,0,-1), D3DXCOLOR(0,0,1,0), 1, 0);
	pv[ 5] = D3DLVERTEX(Vector3(3.0f, -1.0f, 1.25f), Vector3(0,0,-1), D3DXCOLOR(0,0,1,0), 1, 1);
	g_pvtxBuff2->Unlock();

	g_pDevice->CreateIndexBuffer( 6*sizeof(WORD), D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, D3DPOOL_MANAGED, &g_pidxBuff1, NULL );
	WORD *pi = NULL;
	g_pidxBuff1->Lock(0, 0, (void**)&pi, 0);
	pi[ 0] = 0;
	pi[ 1] = 1;
	pi[ 2] = 2;
	pi[ 3] = 3;
	pi[ 4] = 4;
	pi[ 5] = 5;
	g_pidxBuff1->Unlock();
	
	return TRUE;
}

void InitMaterial()
{
	//////////////////////////////////////////////////////////////////////////////
	// A material
	g_Mtrl1.Diffuse = D3DXCOLOR( 1.0f, 1.0f, 1.0f, 1.0f );
	g_Mtrl1.Ambient = D3DXCOLOR( 1.0f, 1.0f, 1.0f, 1.0f );
	g_Mtrl1.Specular = D3DXCOLOR( 1.0f, 1.0f, 1.0f, 1.0f );
	g_Mtrl1.Emissive = D3DXCOLOR( .0f, .0f, .0f, 1.0f );
	g_Mtrl1.Diffuse.a = 0.7f;
	g_Mtrl1.Power = 5.0f;

	// B material
	g_Mtrl2.Diffuse = D3DXCOLOR( 1.0f, 1.0f, 1.0f, 1.0f );
	g_Mtrl2.Ambient = D3DXCOLOR( 1.0f, 1.0f, 1.0f, 1.0f );
	g_Mtrl2.Specular = D3DXCOLOR( 1.0f, 1.0f, 1.0f, 1.0f );
	g_Mtrl2.Emissive = D3DXCOLOR( .0f, .0f, .0f, 1.0f );
	g_Mtrl2.Diffuse.a = 1.0f;
	g_Mtrl2.Power = 5.0f;

}

void Cleanup()
{
	g_pvtxBuff1->Release();
	g_pvtxBuff2->Release();
	g_pTex1->Release();
	g_pTex2->Release();
	g_pAlphaTex1->Release();
	g_pidxBuff1->Release();

}

// WndProc
//
LRESULT CALLBACK d3d::WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch( msg )
	{
	case WM_DESTROY:
		::PostQuitMessage(0);
		break;

	case WM_KEYDOWN:
		if( wParam == VK_ESCAPE )
			::DestroyWindow(hwnd);
		else if( wParam == VK_UP )
		{
			g_vAxis.x = 1;
			g_fAngle += .1f;
		}
		else if( wParam == VK_DOWN )
		{
			g_vAxis.x = 1;
			g_fAngle -= .1f;
		}
		else if( wParam == VK_LEFT )
		{
			g_vAxis.y = 1;
			g_fAngle -= .1f;
			g_fLightAngle -= .1f;
		}
		else if( wParam == VK_RIGHT )
		{
			g_vAxis.y = 1;
			g_fAngle += .1f;
			g_fLightAngle += .1f;
		}

		break;
	}
	return ::DefWindowProc(hwnd, msg, wParam, lParam);
}


//
// WinMain
//
int WINAPI WinMain(HINSTANCE hinstance, HINSTANCE prevInstance, PSTR cmdLine, int showCmd)
{
	if(!d3d::InitD3D(hinstance, WIDTH, HEIGHT, true, D3DDEVTYPE_HAL, &g_pDevice ))
	{
		::MessageBox(0, "InitD3D() - FAILED", 0, 0);
		return 0;
	}
	if(!Setup())
	{
		::MessageBox(0, "Setup() - FAILED", 0, 0);
		return 0;
	}
	d3d::EnterMsgLoop( Display );
	Cleanup();
	g_pDevice->Release();
	return 0;
}


bool Display( float timeDelta )
{
	if( !g_pDevice ) return false;

	D3DXMATRIX mRot;
	D3DXMatrixRotationAxis( &mRot, &g_vAxis, g_fAngle );

	D3DXMATRIX mView;
	D3DXMatrixMultiply( &mView, &mRot, &g_mCamera );
	//	g_pDevice->SetTransform( D3DTS_VIEW, &mView );
	//	g_pDevice->SetTransform( D3DTS_WORLD, &mView );

	D3DLIGHT9 light;
	D3DXVECTOR3 position( 1 * cosf(g_fLightAngle), 1 * sinf(g_fLightAngle), -.5f );
	D3DXCOLOR c = d3d::WHITE;
	light = d3d::InitPointLight(&position, &c);
	g_pDevice->SetLight( 0, &light );
	g_pDevice->LightEnable( 0, true );

	InitMaterial();

	g_pDevice->Clear( 0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(200,200,200), 1.0f, 0 );
	g_pDevice->BeginScene();

	g_pDevice->SetRenderState( D3DRS_LIGHTING, TRUE);
	g_pDevice->SetRenderState( D3DRS_AMBIENT, 0x00202020 );

	g_pDevice->SetRenderState( D3DRS_NORMALIZENORMALS, TRUE );
	g_pDevice->SetRenderState( D3DRS_SPECULARENABLE, TRUE );

	g_pDevice->SetMaterial( &g_Mtrl1 );

	g_pDevice->SetTexture( 0, g_pTex1 );
	g_pDevice->SetTexture( 1, g_pAlphaTex1);

	g_pDevice->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
	g_pDevice->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);

	g_pDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	g_pDevice->SetSamplerState(1, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);

//	g_pDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE );
//	g_pDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1 );
//	g_pDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
//	g_pDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );

	g_pDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
	g_pDevice->SetTextureStageState( 0, D3DTSS_TEXCOORDINDEX, 0 );
	g_pDevice->SetTextureStageState( 1, D3DTSS_TEXCOORDINDEX, 0 );

	g_pDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );
	g_pDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	g_pDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1 );
	g_pDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );

	g_pDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_MODULATE);
	g_pDevice->SetTextureStageState( 1, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	g_pDevice->SetTextureStageState( 1, D3DTSS_COLORARG2, D3DTA_CURRENT);
	g_pDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
	g_pDevice->SetTextureStageState( 2, D3DTSS_COLOROP, D3DTOP_DISABLE);
	g_pDevice->SetTextureStageState( 2, D3DTSS_ALPHAOP, D3DTOP_DISABLE);

/*
	g_pDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );
	g_pDevice->SetTextureStageState( 1, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	g_pDevice->SetTextureStageState( 1, D3DTSS_COLORARG2, D3DTA_CURRENT );
	g_pDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1 );
	g_pDevice->SetTextureStageState( 1, D3DTSS_ALPHAARG1, D3DTA_CURRENT );
/**/

	g_pDevice->SetStreamSource( 0, g_pvtxBuff1, 0, sizeof(D3DLVERTEX) );
	g_pDevice->SetFVF( D3DLVERTEX::FVF );
	g_pDevice->SetIndices( g_pidxBuff1 );
	g_pDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0, 0, 6, 0, 2);

	//g_pDevice->DrawPrimitive( D3DPT_TRIANGLELIST, 0, 2 );
	//g_pDevice->SetMaterial( &g_Mtrl2 );
	//g_pMesh->DrawSubset( 0 );

	g_pDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE);
	g_pDevice->SetTexture( 0, NULL);
	g_pDevice->SetTexture( 1, NULL);

	g_pDevice->EndScene();
	g_pDevice->Present( 0, 0, 0, 0 );

	return true;
}

