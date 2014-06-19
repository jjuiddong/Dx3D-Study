
#include "stdafx.h"
#include "light.h"


CLight::CLight()
{
	ZeroMemory( &m_light, sizeof(D3DLIGHT9) );

	SetDirectionalLight(d3d::YELLOW);

	m_light.Direction = D3DXVECTOR3(1,0,0);
	m_light.Range = 1000.0f;
	m_light.Phi = 1.0f;
	m_light.Theta = 0.5f;
	m_light.Falloff = 1.0f;
	m_light.Attenuation0 = 1.0f;
	m_light.Attenuation1 = 0.0f;
	m_light.Attenuation2 = 0.0f;

	m_LookAt = Vector3(0,0,0);
}


CLight::~CLight()
{

}


//------------------------------------------------------------------------
// 
// [2011/2/15 jjuiddong]
//------------------------------------------------------------------------
void CLight::LightOn()
{
	g_pDevice->SetLight(0, &m_light);
	g_pDevice->LightEnable(0, TRUE);

}


//------------------------------------------------------------------------
// 
// [2011/2/15 jjuiddong]
//------------------------------------------------------------------------
void CLight::LightOff()
{
	g_pDevice->LightEnable(0, FALSE);
}


//------------------------------------------------------------------------
// 
// [2011/2/15 jjuiddong]
//------------------------------------------------------------------------
void CLight::SetPointLight(D3DXCOLOR c)
{
//	m_light.Type = D3DLIGHT_POINT;
	m_light = d3d::InitPointLight((D3DXVECTOR3*)&m_dir, &c);

}


//------------------------------------------------------------------------
// 
// [2011/2/15 jjuiddong]
//------------------------------------------------------------------------
void CLight::SetSpotLight(D3DXCOLOR c)
{
//	m_light.Type = D3DLIGHT_SPOT;
	m_light = d3d::InitSpotLight((D3DXVECTOR3*)&m_pos, (D3DXVECTOR3*)&m_dir, &c);
}


//------------------------------------------------------------------------
// 
// [2011/2/15 jjuiddong]
//------------------------------------------------------------------------
void CLight::SetDirectionalLight(D3DXCOLOR c)
{
	m_light = d3d::InitDirectionalLight((D3DXVECTOR3*)&m_dir, &c);

}


//------------------------------------------------------------------------
// color : D3DXCOLOR(r,g,b,a) 형식
// [2011/2/15 jjuiddong]
//------------------------------------------------------------------------
void CLight::SetDiffuse( DWORD color)
{
	m_light.Diffuse.a = ( ( color >> 24 ) & 0xff ) / 255.0f;
	m_light.Diffuse.r = ( ( color >> 16 ) & 0xff ) / 255.0f;
	m_light.Diffuse.g = ( ( color >> 8  ) & 0xff ) / 255.0f;
	m_light.Diffuse.b = (   color         & 0xff ) / 255.0f;
}


//------------------------------------------------------------------------
// 
// [2011/2/15 jjuiddong]
//------------------------------------------------------------------------
void CLight::SetAmbient( DWORD color )
{
	m_light.Ambient.a = ( ( color >> 24 ) & 0xff ) / 255.0f;
	m_light.Ambient.r = ( ( color >> 16 ) & 0xff ) / 255.0f;
	m_light.Ambient.g = ( ( color >> 8  ) & 0xff ) / 255.0f;
	m_light.Ambient.b = (   color         & 0xff ) / 255.0f;
}


//------------------------------------------------------------------------
// 
// [2011/2/15 jjuiddong]
//------------------------------------------------------------------------
void CLight::SetSpecular( DWORD color )
{
	m_light.Specular.a = ( ( color >> 24 ) & 0xff ) / 255.0f;
	m_light.Specular.r = ( ( color >> 16 ) & 0xff ) / 255.0f;
	m_light.Specular.g = ( ( color >> 8  ) & 0xff ) / 255.0f;
	m_light.Specular.b = (   color         & 0xff ) / 255.0f;
}


//------------------------------------------------------------------------
// 
// [2011/2/15 jjuiddong]
//------------------------------------------------------------------------
DWORD CLight::GetAmbient()
{
	const float a = m_light.Ambient.a * 255.0f;
	const float r = m_light.Ambient.r * 255.0f;
	const float g = m_light.Ambient.g * 255.0f;
	const float b = m_light.Ambient.b * 255.0f;
	return D3DXCOLOR(r, g, b, a);;
}


//------------------------------------------------------------------------
// 
// [2011/2/15 jjuiddong]
//------------------------------------------------------------------------
DWORD CLight::GetDiffuse()
{
	const float a = m_light.Diffuse.a * 255.0f;
	const float r = m_light.Diffuse.r * 255.0f;
	const float g = m_light.Diffuse.g * 255.0f;
	const float b = m_light.Diffuse.b * 255.0f;
	return D3DXCOLOR(r, g, b, a);;
}


//------------------------------------------------------------------------
// 
// [2011/2/15 jjuiddong]
//------------------------------------------------------------------------
void CLight::SetPosition( const Vector3& pos )
{
	m_pos = pos;
	m_light.Position = *(D3DVECTOR*)&pos;

	m_dir = m_LookAt - *(Vector3*)&pos;
	m_dir.Normalize();

	SetDirection(m_dir);
}


//------------------------------------------------------------------------
// 
// [2011/2/15 jjuiddong]
//------------------------------------------------------------------------
void CLight::SetDirection( const Vector3& dir )
{
	m_dir = dir;
	m_light.Direction = *(D3DVECTOR*)&dir;
}


void CLight::SetLookAt( const Vector3& lookAt )
{
	m_LookAt = lookAt;

	m_dir = lookAt - m_pos;
	m_dir.Normalize();

	SetDirection(m_dir);
}


//------------------------------------------------------------------------
// 
// [2011/2/15 jjuiddong]
//------------------------------------------------------------------------
void CLight::SetRange( float range )
{
	m_light.Range = range;
}


//------------------------------------------------------------------------
// 
// [2011/3/14 jjuiddong]
//------------------------------------------------------------------------
void CLight::GetViewMatrix( Matrix44 *pOut )
{
	D3DXVECTOR3 vUp       = D3DXVECTOR3( 0.0f, 1.0f, 0.0f );
	D3DXMatrixLookAtLH( (D3DXMATRIX*)pOut, (D3DXVECTOR3*)&m_pos, (D3DXVECTOR3*)&m_LookAt, &vUp );
}


void CLight::GetViewProjectMatrix( Matrix44 *pOut )
{
	Matrix44 mView, mProj;
	D3DXMatrixPerspectiveFovLH( (D3DXMATRIX*)&mProj
		, 0.25f*D3DX_PI		// 시야각
		, 1.0f				// 종횡비
		, 1.0f, 1000.0f );	// near far

	D3DXVECTOR3 vUp = D3DXVECTOR3( 0.0f, 1.0f, 0.0f );
	D3DXMatrixLookAtLH( (D3DXMATRIX*)&mView, (D3DXVECTOR3*)&m_pos, (D3DXVECTOR3*)&m_LookAt, &vUp );

	*pOut = mView * mProj;
}
