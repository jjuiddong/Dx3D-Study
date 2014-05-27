// AlphaBlendUsingStage.cpp : 응용 프로그램에 대한 진입점을 정의합니다.
// Gigi Rivera

#include "stdafx.h"
#include "etc/d3dUtility.h"


IDirect3DDevice9 *g_pDevice;
const int WIDTH = 640;
const int HEIGHT = 480;
D3DXVECTOR3 g_vAxis(1,0,0);
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
IDirect3DTexture9 *g_pAlphaTex2;

float g_fLightAngle = 0;
const int alphaWidth = 64;
const int alphaHeight = 64;
const int vertexSize = 5;
const int indexSize = 12;

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
void InitAlphaTexture();
bool Display( float timeDelta );
Vector3 GetNormalVector(Vector3 &v1, Vector3 &v2, Vector3 &v3 );
void InitNormal();

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

	InitAlphaTexture();

	g_pDevice->CreateVertexBuffer( vertexSize*sizeof(SVtxNormTex), 0, D3DLVERTEX::FVF, D3DPOOL_MANAGED, &g_pvtxBuff1, NULL );
	g_pDevice->CreateVertexBuffer( vertexSize*sizeof(SVtxNormTex), 0, D3DLVERTEX::FVF, D3DPOOL_MANAGED, &g_pvtxBuff2, NULL );

	D3DLVERTEX *pv;
	// A vertex
	g_pvtxBuff1->Lock(0, 0, (void**)&pv, 0);
	pv[ 0] = D3DLVERTEX(Vector3(-1.0f, -1.0f, .25f), Vector3(0,0,-1), D3DXCOLOR(1,0,0,0), 0, 1);
	pv[ 1] = D3DLVERTEX(Vector3(-1.0f,  1.0f, .25f), Vector3(0,0,-1), D3DXCOLOR(1,0,0,0), 0, 0);
	pv[ 2] = D3DLVERTEX(Vector3( 0.0f,  0.0f, -1.25f), Vector3(0,0,-1), D3DXCOLOR(1,0,0,0), 0.5f, 0.5f);
	pv[ 3] = D3DLVERTEX(Vector3( 1.0f,  1.0f, .25f), Vector3(0,0,-1), D3DXCOLOR(1,0,0,0), 1, 0);
	pv[ 4] = D3DLVERTEX(Vector3( 1.0f,  -1.0f, .25f), Vector3(0,0,-1), D3DXCOLOR(1,0,0,0), 1, 1);
	g_pvtxBuff1->Unlock();

	// B vertex
	g_pvtxBuff2->Lock(0, 0, (void**)&pv, 0);
	pv[ 0] = D3DLVERTEX(Vector3(1.0f, -1.0f, 1.25f), Vector3(0,0,-1), D3DXCOLOR(0,0,1,0), 0, 1);
	pv[ 1] = D3DLVERTEX(Vector3(1.0f,  1.0f, 1.25f), Vector3(0,0,-1), D3DXCOLOR(0,0,1,0), 0, 0);
	pv[ 2] = D3DLVERTEX(Vector3(3.0f,  1.0f, 1.25f), Vector3(0,0,-1), D3DXCOLOR(0,0,1,0), 1, 0);
	pv[ 3] = D3DLVERTEX(Vector3(3.0f,  -1.0f, 1.25f), Vector3(0,0,-1), D3DXCOLOR(0,0,1,0), 1, 1);
	g_pvtxBuff2->Unlock();

	//
	// 1     3
	//    2
	// 0     4
	// 
	g_pDevice->CreateIndexBuffer( indexSize*sizeof(WORD), D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, D3DPOOL_MANAGED, &g_pidxBuff1, NULL );
	WORD *pi = NULL;
	g_pidxBuff1->Lock(0, 0, (void**)&pi, 0);
	pi[ 0] = 0; pi[ 1] = 1; pi[ 2] = 2;
	pi[ 3] = 1; pi[ 4] = 3; pi[ 5] = 2;
	pi[ 6] = 2; pi[ 7] = 3; pi[ 8] = 4;
	pi[ 9] = 2; pi[ 10] = 4; pi[ 11] = 0;
	g_pidxBuff1->Unlock();
	
	InitNormal();

	return TRUE;
}

void InitNormal()
{
	D3DLVERTEX *pv;
	WORD *pi = NULL;
	g_pvtxBuff1->Lock(0, 0, (void**)&pv, 0);
	g_pidxBuff1->Lock(0, 0, (void**)&pi, 0);

	for (int i=0; i < indexSize; i+=3)
	{
		Vector3 v1 = pv[ pi[i]].v;
		Vector3 v2 = pv[ pi[i+1]].v;
		Vector3 v3 = pv[ pi[i+2]].v;
		Vector3 n = GetNormalVector(v1, v2, v3);
		
		pv[ pi[i]].n = n;
		pv[ pi[i+1]].n = n;
		pv[ pi[i+2]].n = n;
	}
	g_pvtxBuff1->Unlock();
	g_pidxBuff1->Unlock();
}


Vector3 GetNormalVector(Vector3 &v1, Vector3 &v2, Vector3 &v3 )
{
	Vector3 v12 = v2 - v1;
	Vector3 v13 = v3 - v1;
	v12.Normalize();
	v13.Normalize();
	Vector3 n = v13.CrossProduct(v12);
	return -n;
}


void InitMaterial()
{
	//////////////////////////////////////////////////////////////////////////////
	// A material
	g_Mtrl1.Diffuse = D3DXCOLOR( 1.0f, 1.0f, 1.0f, 1.0f );
	g_Mtrl1.Ambient = D3DXCOLOR( 1.0f, 1.0f, 1.0f, 1.0f );
	g_Mtrl1.Specular = D3DXCOLOR( 1.0f, 1.0f, 1.0f, 1.0f );
	g_Mtrl1.Emissive = D3DXCOLOR( .0f, .0f, .0f, 1.0f );
	g_Mtrl1.Power = 5.0f;

	// B material
	g_Mtrl2.Diffuse = D3DXCOLOR( 1.0f, 1.0f, 1.0f, 1.0f );
	g_Mtrl2.Ambient = D3DXCOLOR( 1.0f, 1.0f, 1.0f, 1.0f );
	g_Mtrl2.Specular = D3DXCOLOR( 1.0f, 1.0f, 1.0f, 1.0f );
	g_Mtrl2.Emissive = D3DXCOLOR( .0f, .0f, .0f, 1.0f );
	g_Mtrl2.Diffuse.a = 1.0f;
	g_Mtrl2.Power = 5.0f;

}

void InitAlphaTexture()
{
	//D3DXCreateTextureFromFile( g_pDevice, "alpha2.jpg", &g_pAlphaTex1);
	g_pDevice->CreateTexture( alphaWidth, alphaHeight, 1, 0, D3DFMT_A8, D3DPOOL_MANAGED, &g_pAlphaTex1, NULL );
	g_pDevice->CreateTexture( alphaWidth, alphaHeight, 1, 0, D3DFMT_A8, D3DPOOL_MANAGED, &g_pAlphaTex2, NULL );

}

void UpdateAlphaTexture()
{
	D3DLOCKED_RECT lockrect;
	g_pAlphaTex1->LockRect( 0, &lockrect, NULL, 0 );
	BYTE *pbits = (BYTE*)lockrect.pBits;
	//memset( lockrect.pBits, 0x00, lockrect.Pitch*alphaHeight );
	memset( pbits, 0xff, lockrect.Pitch*10 );
	pbits += lockrect.Pitch*10;
	memset( pbits, 0xee, lockrect.Pitch*10 );
	pbits += lockrect.Pitch*10;
	memset( pbits, 0xdd, lockrect.Pitch*10 );
	pbits += lockrect.Pitch*10;
	memset( pbits, 0xcc, lockrect.Pitch*10 );
	pbits += lockrect.Pitch*10;
	memset( pbits, 0x99, lockrect.Pitch*10 );
	pbits += lockrect.Pitch*10;
	memset( pbits, 0x55, lockrect.Pitch*10 );
	g_pAlphaTex1->UnlockRect(0);


	// Alpha2
	g_pAlphaTex2->LockRect( 0, &lockrect, NULL, 0 );
	pbits = (BYTE*)lockrect.pBits;
	for (int y=0; y < alphaHeight; ++y)
	{
		pbits += lockrect.Pitch;
		memset( pbits, 0x55, lockrect.Pitch/2 );
	}
	g_pAlphaTex2->UnlockRect(0);

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
		switch(wParam)
		{
		case VK_ESCAPE:
			::DestroyWindow(hwnd);
			break;
		case '1':
			g_vAxis = D3DXVECTOR3(0,0,0);
			g_vAxis.x = 1.f;
			break;
		case '2':
			g_vAxis = D3DXVECTOR3(0,0,0);
			g_vAxis.y = 1.f;
			break;
		case '3':
			g_vAxis = D3DXVECTOR3(0,0,0);
			g_vAxis.z = 1.f;
			break;
		case VK_UP:
			g_fAngle += .1f;
			break;
		case VK_DOWN:
			g_fAngle -= .1f;
			break;
		case VK_LEFT:
			g_fLightAngle -= .1f;
			break;
		case VK_RIGHT:
			g_fLightAngle += .1f;
			break;
		}
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

void AlphaBlending(IDirect3DTexture9 *pAlphaTex, IDirect3DTexture9 *pTex)
{
	g_pDevice->SetTexture( 0, pAlphaTex);
	g_pDevice->SetTexture( 1, pTex );

	g_pDevice->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
	g_pDevice->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);

	g_pDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
	g_pDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
	g_pDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
	g_pDevice->SetTextureStageState( 0, D3DTSS_TEXCOORDINDEX, 0 );
	g_pDevice->SetTextureStageState( 1, D3DTSS_TEXCOORDINDEX, 0 );

	g_pDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );
	g_pDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	g_pDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1 );
	g_pDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );

	g_pDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );
	g_pDevice->SetTextureStageState( 1, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	g_pDevice->SetTextureStageState( 1, D3DTSS_COLORARG2, D3DTA_CURRENT );
	g_pDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1 );
	g_pDevice->SetTextureStageState( 1, D3DTSS_ALPHAARG1, D3DTA_CURRENT );

	g_pDevice->SetStreamSource( 0, g_pvtxBuff1, 0, sizeof(D3DLVERTEX) );
	g_pDevice->SetFVF( D3DLVERTEX::FVF );
	g_pDevice->SetIndices( g_pidxBuff1 );
	g_pDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0, 0, vertexSize, 0, indexSize/3);
	//g_pDevice->DrawPrimitive( D3DPT_TRIANGLELIST, 0, 2 );

}

void RenderNormal(IDirect3DVertexBuffer9 *pvtxBuff)
{
	g_pDevice->SetRenderState( D3DRS_LIGHTING, FALSE );
	SVtxNormTex *pv;
	pvtxBuff->Lock(0, 0, (void**)&pv, 0);
	for (int i=0; i < vertexSize; ++i)
	{
		const int cross_size = 2;
		SVtxDiff crossline[ cross_size];
		// normal
		crossline[ 0].v = pv[ i].v;
		crossline[ 1].v = pv[ i].v + pv[ i].n * 2.f;
		crossline[ 0].c = D3DXCOLOR( 1, 1, 1, 0 );
		crossline[ 1].c = D3DXCOLOR( 1, 1, 1, 0 );
		g_pDevice->SetFVF( SVtxDiff::FVF );
		g_pDevice->DrawPrimitiveUP( D3DPT_LINELIST, cross_size/2, crossline, sizeof(SVtxDiff) );
	}
	pvtxBuff->Unlock();
	g_pDevice->SetRenderState( D3DRS_LIGHTING, TRUE );
}



bool Display( float timeDelta )
{
	if( !g_pDevice ) return false;

	D3DXMATRIX mRot;
	D3DXMatrixRotationAxis( &mRot, &g_vAxis, g_fAngle );

	D3DXMATRIX mView;
	D3DXMatrixMultiply( &mView, &mRot, &g_mCamera );
//	g_pDevice->SetTransform( D3DTS_VIEW, &mView );
	g_pDevice->SetTransform( D3DTS_WORLD, &mView );

	D3DLIGHT9 light;
	D3DXCOLOR c = d3d::WHITE;
	//D3DXVECTOR3 position( 10 * cosf(g_fLightAngle), 10 * sinf(g_fLightAngle), -.5f );
	//position.x -= 1.f;
	//light.Position = D3DXVECTOR3(0, 0, -10.f);
	//D3DXVECTOR3 directioon( 0.0f, 1.0f, 0.f );
	//light = d3d::InitPointLight(&position, &c);

	D3DXVECTOR3 directioon( cosf(g_fLightAngle), sinf(g_fLightAngle), 0.f );
	light = d3d::InitDirectionalLight(&directioon, &c);

	g_pDevice->SetLight( 0, &light );
	g_pDevice->LightEnable( 0, true );

	InitMaterial();
	UpdateAlphaTexture();

	g_pDevice->Clear( 0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(200,200,200), 1.0f, 0 );
	g_pDevice->BeginScene();

	g_pDevice->SetRenderState( D3DRS_LIGHTING, TRUE);
	g_pDevice->SetRenderState( D3DRS_AMBIENT, 0x00202020 );

	g_pDevice->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
	g_pDevice->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);

	g_pDevice->SetRenderState( D3DRS_NORMALIZENORMALS, TRUE );
	g_pDevice->SetRenderState( D3DRS_SPECULARENABLE, FALSE );

	//g_pDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_CCW );
	//g_pDevice->SetRenderState( D3DRS_ZENABLE, TRUE );
	//g_pDevice->SetRenderState( D3DRS_FOGENABLE, FALSE );
	//g_pDevice->SetRenderState( D3DRS_ZENABLE, D3DZB_TRUE );
	//g_pDevice->SetRenderState( D3DRS_ZWRITEENABLE, TRUE );
	g_pDevice->SetRenderState( D3DRS_SPECULARENABLE, TRUE );

//	g_pDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
//	g_pDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );

//	g_pDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_MODULATE );
//	g_pDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
//	g_pDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
//	g_pDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_DISABLE );


	g_pDevice->SetMaterial( &g_Mtrl1 );

	AlphaBlending(g_pAlphaTex1, g_pTex1);
	AlphaBlending(g_pAlphaTex2, g_pTex2);

//	g_pDevice->SetMaterial( &g_Mtrl2 );
//	g_pMesh->DrawSubset( 0 );

	g_pDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE);
	g_pDevice->SetTexture( 0, NULL);
	g_pDevice->SetTexture( 1, NULL);

	RenderNormal(g_pvtxBuff1);

	g_pDevice->EndScene();
	g_pDevice->Present( 0, 0, 0, 0 );

	return true;
}

