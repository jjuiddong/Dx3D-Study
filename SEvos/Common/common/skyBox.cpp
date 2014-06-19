
#include "StdAfx.h"
#include "skyBox.h"



CSkyBox::CSkyBox() : 
m_pVtxBuffer(NULL)
{
	ZeroMemory(m_pTexture, sizeof(m_pTexture));

}


CSkyBox::~CSkyBox()
{
	SAFE_RELEASE(m_pVtxBuffer);

}


//------------------------------------------------------------------------
// textureFilePath : 이 파일 경로에 skybox_top, skybox_front, skybox_back, 
//				skybox_left, skybox_right, skybox_bottom.jpg 파일이 있어야 한다.
// [2011/3/1 jjuiddong]
//------------------------------------------------------------------------
BOOL CSkyBox::Init(char *szTextureFilePath)
{
	char *textureFileName[] = 
	{
		"skybox_front.jpg", "skybox_back.jpg", "skybox_left.jpg", 
		"skybox_right.jpg", "skybox_top.jpg", "skybox_bottom.jpg"
	};

	for (int i=0; i < MAX_FACE; ++i)
	{
		char fileName[ MAX_PATH];
		strcpy_s(fileName, sizeof(fileName), szTextureFilePath);
		strcat_s(fileName, sizeof(fileName), "/");
		strcat_s(fileName, sizeof(fileName), textureFileName[ i]);
		IDirect3DTexture9 *ptex = CFileLoader::LoadTexture(fileName);
		if (!ptex)
		{
			return FALSE;
		}
		m_pTexture[ i] = ptex;
	}

	if (!CreateVertexBuffer())
		return FALSE;


	return TRUE;
}


//------------------------------------------------------------------------
// 
// [2011/3/1 jjuiddong]
//------------------------------------------------------------------------
BOOL CSkyBox::CreateVertexBuffer()
{
	// Example diagram of "front" quad
	// The numbers are vertices
	// 
	// 2  __________ 4
	//   |\         |
	//	 |  \       |
	//   |    \     |
	//   |      \   |
	// 1 |        \ | 3
	//	  ----------	 
	const float size = 300;
	SVtxTex SkyboxMesh[24] =
	{
		// Front quad, NOTE: All quads face inward
		SVtxTex(-size, -size,  size,  0.0f, 1.0f ),
		SVtxTex(-size,  size,  size,  0.0f, 0.0f ),
		SVtxTex( size, -size,  size,  1.0f, 1.0f ),
		SVtxTex( size,  size,  size,  1.0f, 0.0f ),

		// Back quad
		SVtxTex( size, -size, -size,  0.0f, 1.0f ),
		SVtxTex( size,  size, -size,  0.0f, 0.0f ),
		SVtxTex(-size, -size, -size,  1.0f, 1.0f ),
		SVtxTex(-size,  size, -size,  1.0f, 0.0f ),

		// Left quad
		SVtxTex(-size, -size, -size,  0.0f, 1.0f ),
		SVtxTex(-size,  size, -size,  0.0f, 0.0f ),
		SVtxTex(-size, -size,  size,  1.0f, 1.0f ),
		SVtxTex(-size,  size,  size,  1.0f, 0.0f ),

		// Right quad
		SVtxTex( size, -size,  size,  0.0f, 1.0f ),
		SVtxTex( size,  size,  size,  0.0f, 0.0f ),
		SVtxTex( size, -size, -size,  1.0f, 1.0f ),
		SVtxTex( size,  size, -size,  1.0f, 0.0f ),

		// Top quad
		SVtxTex(-size,  size,  size,  0.0f, 1.0f ),
		SVtxTex(-size,  size, -size,  0.0f, 0.0f ),
		SVtxTex( size,  size,  size,  1.0f, 1.0f ),
		SVtxTex( size,  size, -size,  1.0f, 0.0f ),

		// Bottom quad
		SVtxTex(-size, -size, -size,  0.0f, 1.0f ),
		SVtxTex(-size, -size,  size,  0.0f, 0.0f ),
		SVtxTex( size, -size, -size,  1.0f, 1.0f ),
		SVtxTex( size, -size,  size,  1.0f, 0.0f ),
	};

	const int vtxSize = 24;
	g_pDevice->CreateVertexBuffer( vtxSize*sizeof(SVtxTex), 0, SVtxTex::FVF, 
		D3DPOOL_MANAGED, &m_pVtxBuffer, NULL );

	SVtxTex *pv;
	m_pVtxBuffer->Lock( 0, sizeof(SVtxTex)*vtxSize, (void**)&pv, 0 );
    memcpy( pv, SkyboxMesh, sizeof(SVtxTex) * 24 );
	m_pVtxBuffer->Unlock();

	return TRUE;
}


//------------------------------------------------------------------------
// 
// [2011/3/1 jjuiddong]
//------------------------------------------------------------------------
void CSkyBox::Update(int elapseTime)
{


}


//------------------------------------------------------------------------
// 
// [2011/3/1 jjuiddong]
//------------------------------------------------------------------------
void CSkyBox::SetRenderState()
{
	g_pDevice->SetRenderState( D3DRS_ZENABLE, D3DZB_FALSE );
	g_pDevice->SetRenderState( D3DRS_ZWRITEENABLE, FALSE );
	g_pDevice->SetRenderState( D3DRS_LIGHTING, FALSE );
	g_pDevice->SetRenderState( D3DRS_FOGENABLE, FALSE );

	g_pDevice->SetSamplerState( 0, D3DSAMP_ADDRESSU,  D3DTADDRESS_CLAMP );
	g_pDevice->SetSamplerState( 0, D3DSAMP_ADDRESSV,  D3DTADDRESS_CLAMP );

/*
	g_pDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE);
	g_pDevice->SetRenderState( D3DRS_FOGENABLE, FALSE );

	g_pDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA );
	g_pDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
	g_pDevice->SetRenderState( D3DRS_ALPHATESTENABLE, FALSE );

/*
	// Set TFactor
	g_pDevice->SetRenderState( D3DRS_TEXTUREFACTOR, 0xFFFFFFFF );

	// Texture 관련 세팅
	g_pDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_MODULATE );
	g_pDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TFACTOR );
	g_pDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_TEXTURE );

	g_pDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_MODULATE );
	g_pDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TFACTOR );
	g_pDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_TEXTURE );

	g_pDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_POINT );
	g_pDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_POINT );

	g_pDevice->SetSamplerState( 0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP );
	g_pDevice->SetSamplerState( 0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP );

	// TFactor
	g_pDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE	);
	g_pDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE 	);
/**/
}


//------------------------------------------------------------------------
// 
// [2011/3/1 jjuiddong]
//------------------------------------------------------------------------
void CSkyBox::Render()
{
	SetRenderState();

	D3DXMATRIX matView, matViewSave, matWorld;
	g_pDevice->GetTransform( D3DTS_VIEW, &matViewSave );
	matView = matViewSave;
	matView._41 = 0.0f; matView._42 = -0.4f; matView._43 = 0.0f;
	g_pDevice->SetTransform( D3DTS_VIEW, &matView );
	// Set a default world matrix
	D3DXMatrixIdentity(&matWorld);
	g_pDevice->SetTransform( D3DTS_WORLD, &matWorld);

	// render
	g_pDevice->SetFVF(SVtxTex::FVF);
	g_pDevice->SetStreamSource( 0, m_pVtxBuffer, 0, sizeof(SVtxTex));
	for (int i = 0 ; i < MAX_FACE; i++)
	{
		g_pDevice->SetTexture(0, m_pTexture[ i]);
		g_pDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, i*4, 2 );
	}

	g_pDevice->SetTransform( D3DTS_VIEW, &matViewSave );
}


//------------------------------------------------------------------------
// 
// [2011/3/1 jjuiddong]
//------------------------------------------------------------------------
void CSkyBox::Clear()
{
	SAFE_RELEASE(m_pVtxBuffer);
}

