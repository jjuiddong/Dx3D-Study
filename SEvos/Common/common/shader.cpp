
#include "StdAfx.h"
#include "shader.h"



CShader::CShader() :
m_pEffect(NULL) 
, m_hTechnique(NULL)
, m_pDecl(NULL)
{

}


CShader::~CShader()
{
	Clear();
}


//------------------------------------------------------------------------
// 
// [2011/3/23 jjuiddong]
//------------------------------------------------------------------------
void CShader::Clear()
{
	SAFE_RELEASE(m_pEffect);
	SAFE_RELEASE(m_pDecl);
}


//------------------------------------------------------------------------
// 
// [2011/3/23 jjuiddong]
//------------------------------------------------------------------------
BOOL CShader::Init(char *szFileName, char *szTechnique)
{
	// 쉐이더 파일 읽기
	HRESULT hr;
	LPD3DXBUFFER pErr;
	if (FAILED(hr = D3DXCreateEffectFromFile(g_pDevice, szFileName, NULL, NULL, 
		D3DXSHADER_DEBUG , NULL, &m_pEffect, &pErr))) {
			MessageBox( NULL, (LPCTSTR)pErr->GetBufferPointer(), "ERROR", MB_OK);
			//DXTRACE_ERR( "CreateEffectFromFile", hr );
			return FALSE;
	}

	m_hTechnique = m_pEffect->GetTechniqueByName( "TShader" );

	return TRUE;
}


//------------------------------------------------------------------------
// 
// [2011/3/23 jjuiddong]
//------------------------------------------------------------------------
void CShader::Begin()
{
	if (!m_pEffect) return;

	m_pEffect->Begin(NULL, 0);
	m_pEffect->SetTechnique( m_hTechnique );
}


//------------------------------------------------------------------------
// 
// [2011/3/23 jjuiddong]
//------------------------------------------------------------------------
void CShader::BeginPass(int pass)
{
	if (!m_pEffect) return;

	m_pEffect->BeginPass(pass);
}


//------------------------------------------------------------------------
// 
// [2011/3/23 jjuiddong]
//------------------------------------------------------------------------
void CShader::EndPass()
{
	if (!m_pEffect) return;

	m_pEffect->EndPass();

}


//------------------------------------------------------------------------
// 
// [2011/3/23 jjuiddong]
//------------------------------------------------------------------------
void CShader::End()
{
	if (!m_pEffect) return;
	m_pEffect->End();

}


//------------------------------------------------------------------------
// 
// [2011/3/23 jjuiddong]
//------------------------------------------------------------------------
void CShader::SetMatrix(char *pKey, const Matrix44 &mat)
{
	if (!m_pEffect) return;
	m_pEffect->SetMatrix( pKey, (D3DXMATRIX*)&mat );
}
void CShader::SetTexture(char *pKey, IDirect3DTexture9 *pTexture)
{
	if (!m_pEffect) return;
	m_pEffect->SetTexture( pKey, pTexture );
}
void CShader::SetTexture(char *pKey, LPDIRECT3DCUBETEXTURE9 pTexture)
{
	if (!m_pEffect) return;
	m_pEffect->SetTexture( pKey, pTexture );
}

void CShader::CommitChanges()
{
	if (!m_pEffect) return;
	m_pEffect->CommitChanges();
}
void CShader::SetFloat(char *pKey, float val)
{
	if (!m_pEffect) return;
	m_pEffect->SetFloat( pKey, val );
}
void CShader::SetVector(char *pKey, const Vector3 &vec )
{
	if (!m_pEffect) return;
	D3DXVECTOR4 temp(vec.x, vec.y, vec.z, 1.f);
	m_pEffect->SetVector( pKey, &temp );
}
void CShader::SetVector(char *pKey, const D3DXVECTOR4 &vec )
{
	if (!m_pEffect) return;
	m_pEffect->SetVector( pKey, &vec );
}