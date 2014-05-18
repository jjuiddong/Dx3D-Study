
#include "global.h"
#include "camera.h"	
#include "d3dutility.h"


//---------------------------
//	Constructor
//---------------------------
CCamera::CCamera()
:	m_fDistance		( 1.0F ),
	m_vEye			( 0.0F, m_fDistance, 0.0F ),
	m_vAt			( 0.0F, 0.0F, 0.0F ),
	m_vUp			( 0.0F, 1.0F, 0.0F ),
	m_wFovAngle		( 30 ),
	m_fPitch		( 45.0F ),
	m_fYaw			( 0.0F ),
	m_fPrevDistance	( 0.0f ),
	m_fPrevPitch	( 0.0f ),
	m_fPrevYaw		( 0.0f ),
	m_bMove			( FALSE ),
	m_fYawRange		( 90.0F ),
	m_fFarPlane		( 2000.0F ),
	m_fNearPlane	( 1.0F ),
	m_fMaxZoom		( 35.0F ),
	m_fMinPitch		( 25.0F ),
	m_dwActionTick	( 0 ),
	m_bStateChange	( TRUE )
{
//	Assert( g_pDirect3D );

	m_vEye	= Vector3( 0.0F, m_fDistance, 0.0F );
	m_vAt	= Vector3( 0.0F, 0.0F, 0.0F );
	m_vUp	= Vector3( 0.0F, 1.0F, 0.0F );

	m_vEye.RotateX( ANGLE( m_fPitch ) );
	m_vEye.RotateZ( ANGLE( m_fYaw ) );
	m_vEye += m_vAt;

//	GetD3D()->SetCamera( this );

	Execute();
} //CCamera::CCamera

//---------------------------
//	Destructor
//---------------------------
CCamera::~CCamera()
{
	//-- NULL --//
} //CCamera::~CCamera

//---------------------------
//	Apply
//---------------------------
void CCamera::Execute()
{
	//	Camera Execute.
	if( g_pDevice )
	{
		//m_matView
//		g_pDirect3D->SetProjection( ANGLE( m_wFovAngle ), m_fNearPlane, m_fFarPlane );
//		g_pDirect3D->LookAt( m_vEye, m_vAt, m_vUp );
//		g_pDirect3D->GetFrustum().SetPos( GetPosition() );		
//		g_pDirect3D->SetCameraPos( GetPosition() );

		D3DXMatrixPerspectiveFovLH( (D3DXMATRIX*)&m_matProj, D3DX_PI/4.f, (float)m_Viewport.Width / (float)m_Viewport.Height, 1.0f, 1000.0f );
		g_pDevice->SetTransform( D3DTS_PROJECTION, (D3DXMATRIX*)&m_matProj );

		D3DXMatrixLookAtLH( (D3DXMATRIX*)&m_matView, (D3DXVECTOR3*)&m_vEye, (D3DXVECTOR3*)&m_vAt, (D3DXVECTOR3*)&m_vUp );
		g_pDevice->SetTransform( D3DTS_VIEW, (D3DXMATRIX*)&m_matView );
	} //if

	m_bStateChange = FALSE;
} //CCamera::Excute


// 카메라 위치를 업데이트한다.
void CCamera::ExecuteCam()
{
	if( g_pDevice )
	{
		D3DXMatrixLookAtLH( (D3DXMATRIX*)&m_matView, (D3DXVECTOR3*)&m_vEye, (D3DXVECTOR3*)&m_vAt, (D3DXVECTOR3*)&m_vUp );
		g_pDevice->SetTransform( D3DTS_VIEW, (D3DXMATRIX*)&m_matView );
	}
}


//---------------------------
//	Set Fov
//---------------------------
void CCamera::Update( DWORD dwTick )
{
	if( m_fPitch	!= m_fPrevPitch		||
		m_fYaw		!= m_fPrevYaw		||
		m_fDistance != m_fPrevDistance	)		
	{
		m_fPrevPitch	= m_fPitch;
		m_fPrevYaw		= m_fYaw;
		m_fPrevDistance = m_fDistance;

		m_bMove = TRUE;
	}
	else
	{
		m_bMove = FALSE;
	} // if..else..
} // CCamera::Update


// 뷰포트 설정
void CCamera::SetViewPort( int Width, int Height )
{
	m_Width = Width;
	m_Height = Height;
	m_Viewport.X = 0;
	m_Viewport.Y = 0;
	m_Viewport.Width = Width;
	m_Viewport.Height = Height;
	m_Viewport.MinZ = 0;
	m_Viewport.MaxZ = 100;

}


//---------------------------
//	Set Fov
//---------------------------
void CCamera::SetFov( const WORD wAngle )
{
	m_wFovAngle = wAngle;
} //CCamera::SetFov

//---------------------------
//	Set Position
//  return value: 
//					TRUE = 카메라 이동 에니메이션이 된다면 
//					FALSE = 이동 에니메이션 없음
//---------------------------
BOOL CCamera::SetPosition( const Vector3& vPos )
{
	Vector3 vOffset = vPos - GetPosition();

	if( vOffset.x == 0.0F &&
		vOffset.y == 0.0F &&
		vOffset.z == 0.0F
		)
	{
		return TRUE;
	} //if

	// 차이가 크다면 에니메이션 된다.
	float len = vOffset.Length();

//	if( 3 < len )
	{
		m_bMove = TRUE;
		m_nAniIncTime = 0;
		m_fMoveVelocity = len / 500.f;
		vOffset.Normalize();
		m_vMoveDir = vOffset;
		return TRUE;
	}
//	else
	{
//		m_vAt	+= vOffset;
//		m_vEye	+= vOffset;
	}

	m_bStateChange = TRUE;
	return FALSE;
} //CCamera::SetPosition


//-----------------------------------------------------------------------------//
// 카메라 이동 에니메이션
//-----------------------------------------------------------------------------//
void CCamera::Animate( int nDelta )
{
	if( !m_bMove ) return;

	m_nAniIncTime += nDelta;
	if( 1000 < m_nAniIncTime )
	{
		nDelta = m_nAniIncTime - 1000;
		m_bMove = FALSE;
	}

	Vector3 mov = m_vMoveDir * (m_fMoveVelocity * (float)nDelta);
	m_vAt += mov;
	m_vEye += mov;
	ExecuteCam();
}


//---------------------------
//	Set Up Vector
//---------------------------
void CCamera::SetUpVector( const Vector3& vUp )
{
	m_vUp = vUp;
} //CCmera::SetUpVector

//---------------------------
//	Set Eye Vector
//---------------------------
void CCamera::SetEye( const Vector3& vEye )
{
	m_vEye = vEye;
} //CCmera::SetEye

//---------------------------
//	Get Position
//---------------------------
Vector3 CCamera::GetPosition()
{
	return m_vAt - m_vUp;
} //CCamera::GetPosition

//---------------------------
//	Get Eye
//---------------------------
const Vector3& CCamera::GetEye()
{
	return m_vEye;
} //CCamera::GetEye

//---------------------------
//	Get Front 
//---------------------------
Vector3 CCamera::GetFront()
{
	return ( m_vAt - m_vEye ).Normal();
} //CCamera::GetFront

//---------------------------
//	Get Right
//---------------------------
Vector3 CCamera::GetRight()
{
	Vector3 vDir = ( m_vAt - m_vEye ).Normal();
	return m_vUp.CrossProduct( vDir ).Normal();
} //CCamera::GetRight

//---------------------------
//	Get View Matrix
//---------------------------
Matrix44 CCamera::GetViewMatrix()
{
	Matrix44 matView;
	matView.SetView( m_vEye, ( m_vAt - m_vEye ), m_vUp );
	return matView;
} //CCamera::GetViewMatrix

//---------------------------
// Top View Matrix
//---------------------------
Matrix44 CCamera::GetTopViewMatrix()
{
	Matrix44 matView;
	Vector3 vEye = m_vEye;

	vEye.z += 32.0F;
	Vector3 vAt = m_vEye;
	vAt.z += 31.0F;
	vAt += 0.001F;

	//vAt += 0.1F;

	matView.SetView( vEye, ( vAt - vEye ), m_vUp );

	return matView;
} //CCamera::GetTopViewMatrix

//---------------------------
//	Set World Matrix
//---------------------------
void CCamera::SetWorld( const Matrix44& matWorld )
{
	Matrix44	matUp = matWorld;
	matUp._41 = 0.0F;
	matUp._42 = 0.0F;
	matUp._43 = 0.0F;
	m_vEye	= Vector3( 0.0F, m_fDistance, 0.0F ) * matWorld;
	m_vAt	= Vector3( 0.0F, 0.0F, 0.0F ) * matWorld;
	m_vUp	= Vector3( 0.0F, 0.0F, 1.0F ) * matUp;

	m_bStateChange = TRUE;
} //CCamera::SetWorld

//---------------------------
//	Rotate X
//---------------------------
void CCamera::RotateX( float fAngle )
{
	if( fAngle == 0.0F )
		return ;

	m_fPitch -= fAngle;	
/*
	if( m_fPitch < -88.0f)			m_fPitch = -88.0f;
	if( m_fPitch > 88.0F )			m_fPitch = 88.0F;

#ifndef _CAMCONTROL

	m_vEye	= Vector3( 0.0F, m_fDistance, 0.0F );
	m_vEye.RotateX( ANGLE( m_fPitch ) );	

#else

	static float fPitch;
	if( m_fPitch >= 0 )
	{
		m_fDistance = m_fMaxZoom * ( m_fPitch / 88 );
		fPitch = 45 * ( m_fDistance / m_fMaxZoom );
	}
	else
	{
		fPitch = m_fPitch;
	} // if..else..

	m_vEye	= Vector3( 0.0F, m_fDistance, 0.0F );
	m_vEye.RotateX( ANGLE( fPitch ) );

#endif // #ifdef _CAMCONTROL	

	m_vEye.RotateY( ANGLE( m_fRoll ) );
//	m_vEye.RotateZ( ANGLE( m_fYaw ) );
	m_vEye += m_vAt;
/**/

	Vector3 axis = GetRight();
	Quaternion q( axis, ANGLETORAD(fAngle) );
	Matrix44 mat = q.GetMatrix();
	m_vEye *= mat;

	m_bStateChange = TRUE;
} //CCamera::RotateX

//---------------------------
//	Rotate Z
//---------------------------
void CCamera::RotateZ( float fAngle )
{
	if( fAngle == 0.0F )
		return;

	m_fYaw += fAngle;

	while( m_fYaw < 0.0F )
	{
		m_fYaw += 360.0F;
	} //while
	while( m_fYaw > 360.0F )
	{
		m_fYaw -= 360.0F;
	} //while

	m_vEye	= Vector3( 0.0F, m_fDistance, 0.0F );

	m_vEye.RotateX( ANGLE( m_fPitch ) );
	m_vEye.RotateY( ANGLE( m_fRoll ) );
//	m_vEye.RotateZ( ANGLE( m_fYaw ) );
	m_vEye += m_vAt;

	m_bStateChange = TRUE;
} //CCamera::RotateZ


//---------------------------
//	Rotate Y
//---------------------------
void CCamera::RotateY( float fAngle )
{
	if( fAngle == 0.0F )
		return;

	m_fRoll += fAngle;
/*
	while( m_fRoll < 0.0F )
	{
		m_fRoll += 360.0F;
	} //while
	while( m_fRoll > 360.0F )
	{
		m_fRoll -= 360.0F;
	} //while

/*
	m_vEye	= Vector3( 0.0F, m_fDistance, 0.0F );

	m_vEye.RotateX( ANGLE( m_fPitch ) );
	m_vEye.RotateZ( ANGLE( m_fYaw ) );
	m_vEye += m_vAt;
/**/

	Quaternion q( Vector3(0,1,0), ANGLETORAD( fAngle ) );
	Matrix44 mat = q.GetMatrix();
	m_vEye *= mat;

	m_bStateChange = TRUE;
} //CCamera::RotateZ


//---------------------------
//	Zoom
//---------------------------
void CCamera::Zoom( float fOffset )
{
	if( fOffset == 0.0F )
	{
		return;
	} //if

	m_fDistance -= fOffset;	

	if( m_fDistance < 2.0F )		m_fDistance = 2.0F;
	if( m_fDistance > m_fMaxZoom )	m_fDistance = m_fMaxZoom;	

#ifndef _CAMCONTROL

	m_vEye	= Vector3( 0.0F, m_fDistance, 0.0F );

	m_vEye.RotateX( ANGLE( m_fPitch ) );
	m_vEye.RotateZ( ANGLE( m_fYaw ) );
	m_vEye += m_vAt;	

#else

	if( m_fDistance > 0.5f )
	{
		RotateX( fOffset );
	} // if	

#endif // #ifdef _CAMCONTROL

	m_bStateChange = TRUE;
} //CCamera::Zoom

//---------------------------
//	Get Zoom
//---------------------------
float CCamera::GetZoom()
{
	return m_fDistance;
} //CCamera::GetZoom

//---------------------------
//	Set Zoom
//---------------------------
void CCamera::SetZoom( const float fDist )
{
	m_fDistance = fDist;
} //CCamera::SetZoom

//---------------------------
//	Set Max Zoom
//---------------------------
void CCamera::SetMaxZoom( const float fMax )
{
	m_fMaxZoom = fMax;
} //CCamera::SetMaxZoom

//---------------------------
//	Set Far Plane
//---------------------------
void CCamera::SetFarPlane( const float fFarPlane )
{
	m_fFarPlane = fFarPlane;
} //CCamera::SetFarPlane

//---------------------------
//	Set Near Plane
//---------------------------
void CCamera::SetNearPlane( const float fNearPlane )
{
	m_fNearPlane = fNearPlane;
} //CCamera::SetNearPlane

//---------------------------
//	Get Far Plane
//---------------------------
const float CCamera::GetFarPlane()
{
	return m_fFarPlane;
} //CCamera::GetFarPlane

void CCamera::SetCamValue( const float fPitch, const float fYaw, const float fRoll, const float fDistance )
{
	m_fYaw = fYaw;
	m_fPitch = fPitch;
	m_fRoll = fRoll;
	m_fDistance = fDistance;

}

void CCamera::GetVec3Project( Vector3 *pOut, Vector3 *pV )
{
	D3DXVec3Project( (D3DXVECTOR3*)pOut, (D3DXVECTOR3*)pV, &m_Viewport, 
		(D3DXMATRIX*)&m_matProj, (D3DXMATRIX*)&m_matView, (D3DXMATRIX*)&g_matIdentity );
}
