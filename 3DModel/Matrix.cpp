
#include "global.h"
#include "Math.h"

using namespace std;

//---------------------------
//	Matrix4x4_Inverse
//---------------------------
static void Matrix4x4_Inverse( const float b[][4], float a[][4] );

//---------------------------
//	Constructor
//---------------------------
Matrix44::Matrix44()
{
	//-- NULL --//
} //Matrix44::Matrix44

//---------------------------
//	operator *
//---------------------------
Matrix44 Matrix44::operator * ( const Matrix44& m_ ) const
{
	Matrix44 matrix;
	ZeroMemory( &matrix, sizeof( matrix ) );

	for( int i = 0 ; i < 4 ; ++i )
	{
		for( int j = 0 ; j < 4 ; ++j )
		{			
			for( int k = 0 ; k < 4 ; ++k )
			{
				matrix.m[i][j] += m[i][k] * m_.m[k][j];
			} //for
		} //for 
	} //for

	return matrix;
} //operator *

Matrix44 Matrix44::operator * ( const float f )
{
	Matrix44 matrix;
	for( int i = 0 ; i < 4 ; ++i )
		for( int j = 0 ; j < 4 ; ++j )
			matrix.m[i][j] = m[ i][ j] * f;
	return matrix;
}


//---------------------------
//	operator *=
//---------------------------
Matrix44& Matrix44::operator *= ( const Matrix44& m_ )
{
	Matrix44 matrix;
	ZeroMemory( &matrix, sizeof( matrix ) );

	for( int i = 0 ; i < 4 ; ++i )
	{
		for( int j = 0 ; j < 4 ; ++j )
		{			
			for( int k = 0 ; k < 4 ; ++k )
			{
				matrix.m[i][j] += m[i][k] * m_.m[k][j];
			} //for
		} //for 
	} //for

	*this = matrix;
	return *this;
} //operator *=

//---------------------------
//
//---------------------------
const float& Matrix44::operator () ( const int nRow, const int nColumn )
{
	return m[nRow][nColumn];
} //operator ()

//---------------------------
//
//---------------------------
void Matrix44::SetIdentity()
{
	_12 = _13 = _14 = 0.0F;
	_21 = _23 = _24 = 0.0F;
	_31 = _32 = _34 = 0.0F;
	_41 = _42 = _43 = 0.0F;
	_11 = _22 = _33 = _44 = 1.0F;
} //SetIdentity

//---------------------------
//
//---------------------------
Matrix44 Matrix44::Inverse() const
{
	Matrix44		matInverse;

	if( fabs( _44 - 1.0F ) > 0.001F )
	{
		Matrix4x4_Inverse( this->m, matInverse.m );
		return matInverse;
	} //if

	if( fabs( _14 ) > 0.001F || fabs( _24 ) > 0.001F || fabs( _34 ) > 0.001F )
	{
		Matrix4x4_Inverse( this->m, matInverse.m );
		return matInverse;
	} //if

	float det =   _11 * ( _22 * _33 - _23 * _32 )
				- _12 * ( _21 * _33 - _23 * _31 )
				+ _13 * ( _21 * _32 - _22 * _31 );

	if( ABS( det ) < MATH_EPSILON )
	{
		Matrix4x4_Inverse( this->m, matInverse.m );
		return matInverse;
	} //if

	det = 1.0F / det;

	matInverse._11 =  det * ( _22 * _33 - _23 * _32 );
	matInverse._12 = -det * ( _12 * _33 - _13 * _32 );
	matInverse._13 =  det * ( _12 * _23 - _13 * _22 );
	matInverse._14 = 0.0F;

	matInverse._21 = -det * ( _21 * _33 - _23 * _31 );
	matInverse._22 =  det * ( _11 * _33 - _13 * _31 );
	matInverse._23 = -det * ( _11 * _23 - _13 * _21 );
	matInverse._24 = 0.0F;

	matInverse._31 =  det * ( _21 * _32 - _22 * _31 );
	matInverse._32 = -det * ( _11 * _32 - _12 * _31 );
	matInverse._33 =  det * ( _11 * _22 - _12 * _21 );
	matInverse._34 = 0.0F;

	matInverse._41 = -( _41 * matInverse._11 + _42 * matInverse._21 + _43 * matInverse._31 );
	matInverse._42 = -( _41 * matInverse._12 + _42 * matInverse._22 + _43 * matInverse._32 );
	matInverse._43 = -( _41 * matInverse._13 + _42 * matInverse._23 + _43 * matInverse._33 );
	matInverse._44 = 1.0F;

	return matInverse;
} //Matrix44::Inverse

//---------------------------
//
//---------------------------
Quaternion Matrix44::GetQuaternion() const
{
	Quaternion q;

	float fTr = _11 + _22 + _33 + _44;

	if( fTr >= 1.0F )	// w >= 0.5
	{
		float s = sqrtf( fTr );
		q.x = ( _32 - _23 ) / ( 2.0F * s );
		q.y = ( _13 - _31 ) / ( 2.0F * s );
		q.z = ( _21 - _12 ) / ( 2.0F * s );
		q.w = 0.5F * s;
	}
	else
	{
		float v[3];
		int i, j, k;

		if( _11 > _22 )		i = 0;
		else				i = 1;
		if( _33 > m[i][i] )	i = 2;

		j = ( i + 1 ) % 3;
		k = ( j + 1 ) % 3;

		float s = sqrtf( m[i][i] - m[j][j] - m[k][k] + 1.0F );
		v[i] = 0.5F * s;
		v[j] = ( m[j][i] + m[i][j] ) / ( 2.0F * s );
		v[k] = ( m[k][i] + m[i][k] ) / ( 2.0F * s );

		q.x = v[0];
		q.y = v[1];
		q.z = v[2];
		q.w = ( m[k][j] - m[j][k] ) / ( 2.0F * s );
	} //if..else..

	return q;
} //Matrix44::GetQuaternion

//---------------------------
//
//---------------------------
void Matrix44::SetRotationX( const float fAngle )
{
	float fCos = cosf( fAngle );
	float fSin = sinf( fAngle );
	SetIdentity();
	_22 = fCos;
	_23 = fSin;
	_32 = -fSin;
	_33 = fCos;
} //Matrix44::SetRotationX

//---------------------------
//
//---------------------------
void Matrix44::SetRotationY( const float fAngle )
{
	float fCos = cosf( fAngle );
	float fSin = sinf( fAngle );
	SetIdentity();
	_11 = fCos;
	_13 = -fSin;
	_31 = fSin;
	_33 = fCos;
} //Matrix44::SetRotationY

//---------------------------
//
//---------------------------
void Matrix44::SetRotationZ( const float fAngle )
{
	float fCos = cosf( fAngle );
	float fSin = sinf( fAngle );
	SetIdentity();
	_11 = fCos;
	_12 = fSin;
	_21 = -fSin;
	_22 = fCos;
} //Matrix44::SetRotationZ

//---------------------------
//
//---------------------------
void Matrix44::SetRotation( const Vector3& vINAxis, const float fAngle )
{
	Vector3	vAxis = vINAxis.Normal();
	float	fCos = cosf( fAngle );
	float	fSin = sinf( fAngle );

	_11 = ( vAxis.x * vAxis.x ) * ( 1.0F - fCos ) + fCos;
	_12 = ( vAxis.x * vAxis.y ) * ( 1.0F - fCos ) - ( vAxis.z * fSin );
	_13 = ( vAxis.x * vAxis.z ) * ( 1.0F - fCos ) + ( vAxis.y * fSin );

	_21 = ( vAxis.y * vAxis.x ) * ( 1.0F - fCos ) + ( vAxis.z * fSin );
	_22 = ( vAxis.y * vAxis.y ) * ( 1.0F - fCos ) + fCos;
	_23 = ( vAxis.y * vAxis.z ) * ( 1.0F - fCos ) - ( vAxis.x * fSin );

	_31 = ( vAxis.z * vAxis.x ) * ( 1.0F - fCos ) - ( vAxis.y * fSin );
	_32 = ( vAxis.z * vAxis.y ) * ( 1.0F - fCos ) + ( vAxis.x * fSin );
	_33 = ( vAxis.z * vAxis.z ) * ( 1.0F - fCos ) + fCos;

	_14 = _24 = _34 = 0.0F;
	_41 = _42 = _43 = 0.0F;
	_44 = 1.0F;
} //Matrix44::SetRotation

//---------------------------
//
//---------------------------
void Matrix44::SetView( const Vector3& vPos, const Vector3& vDir_, const Vector3& vUp_ )
{
	Vector3 vDir;
	Vector3 vUp;
	Vector3 vCross;

	vDir = vDir_.Normal();
	vCross = vUp_.CrossProduct( vDir );
	vCross.Normalize();
	vUp = vDir.CrossProduct( vCross );

	_11 = vCross.x;
	_12 = vUp.x;
	_13 = vDir.x;
	_14 = 0.0F;
	_21 = vCross.y;
	_22 = vUp.y;
	_23 = vDir.y;
	_24 = 0.0F;
	_31 = vCross.z;
	_32 = vUp.z;
	_33 = vDir.z;
	_34 = 0.0F;
	_41 = -vPos.DotProduct( vCross );
	_42 = -vPos.DotProduct( vUp );
	_43 = -vPos.DotProduct( vDir );
	_44 = 1.0F;
} //Matrix44::SetView

//---------------------------
//
//---------------------------
void Matrix44::SetProjection(	const float fFOV,			///<
								const float fAspect,		///<
								const float fNearPlane,		///<
								const float fFarPlane		///<
								)
{
	float fW = fAspect * cosf( fFOV / 2 ) / sinf( fFOV / 2 );
	float fH = 1.0F    * cosf( fFOV / 2 ) / sinf( fFOV / 2 );
	float fQ = fFarPlane / ( fFarPlane - fNearPlane );

	_12 = _13 = _14 = 0.0F;
	_21 = _23 = _24 = 0.0F;
	_31 = _32 = 0.0F;
	_41 = _42 = 0.0F;
	_11 = fW;
	_22 = fH;
	_33 = fQ;
	_34 = 1.0F;					// Perspective..
	_43 = -fQ * fNearPlane;
	_44 = 1.0F;
} //Matrix44::SetProjection



Vector3 Matrix44::GetPosition()
{
	return Vector3(_41, _42, _43);
}


//---------------------------
//
//---------------------------
void Matrix44::SetWorld( const Vector3& vPos )
{
	SetIdentity();

	_41 = vPos.x;
	_42 = vPos.y;
	_43 = vPos.z;
} //Matrix44::SetWorld

//---------------------------
//
//---------------------------
void Matrix44::SetWorld( const Vector3& vPos, const Quaternion& qRot )
{
	*this = qRot.GetMatrix();

	_41 = vPos.x;
	_42 = vPos.y;
	_43 = vPos.z;
} //Matrix44::SetWorld

//---------------------------
//
//---------------------------
void Matrix44::SetWorld( const Vector3& vPos, const Vector3& vINDir, const Vector3& vINUp )
{
	Vector3		vDir	= vINDir.Normal();
	Vector3		vCross	= vINUp.CrossProduct( vDir ).Normal();
	Vector3		vUp		= vDir.CrossProduct( vCross );

	_11 = vCross.x;
	_12 = vCross.y;
	_13 = vCross.z;
	_14 = 0.0F;
	_21 = vUp.x;
	_22 = vUp.y;
	_23 = vUp.z;
	_24 = 0.0F;
	_31 = vDir.x;
	_32 = vDir.y;
	_33 = vDir.z;
	_34 = 0.0F;
	_41 = vPos.x;
	_42 = vPos.y;
	_43 = vPos.z;
	_44 = 1.0F;
} //Matrix44::SetWorld

//---------------------------
//
//---------------------------
void Matrix44::Translate( const Vector3& vPos )
{
	_41 += ( vPos.x * _11 ) + ( vPos.y * _21 ) + ( vPos.z * _31 );
	_42 += ( vPos.x * _12 ) + ( vPos.y * _22 ) + ( vPos.z * _32 );
	_43 += ( vPos.x * _13 ) + ( vPos.y * _23 ) + ( vPos.z * _33 );
} //Matrix44::Translate

//---------------------------
//
//---------------------------
Matrix44 Matrix44::GetBillboard() const
{
	Matrix44 matrix;

	matrix._11 = _11;
	matrix._12 = _21;
	matrix._13 = _31;
	matrix._14 = 0.0F;
	matrix._21 = _12;
	matrix._22 = _22;
	matrix._23 = _32;
	matrix._24 = 0.0F;
	matrix._31 = _13;
	matrix._32 = _23;
	matrix._33 = _33;
	matrix._34 = 0.0F;
	matrix._41 = 0.0F;
	matrix._42 = 0.0F;
	matrix._43 = 0.0F;
	matrix._44 = 1.0F;

	return matrix;
} //Matrix44::GetBillboard

//---------------------------
//
//---------------------------
Matrix44 Matrix44::GetBillboardX() const
{
	static Matrix44	matWorld;
	static Vector3	vDir;

	vDir.x = _13;
	vDir.y = _23;
	vDir.z = _33;

	if( vDir.x == 0.0F )
	{
		matWorld.SetIdentity();
	}
	else	if( vDir.x > 0.0F )
	{
		matWorld.SetRotationY( -atanf( vDir.z / vDir.x ) + MATH_PI / 2 );
	}
	else
	{
		matWorld.SetRotationY( -atanf( vDir.z / vDir.x ) - MATH_PI / 2 );
	} //if..else if..else..

	return matWorld;
} //Matrix44::GetBillboardX

//---------------------------
//
//---------------------------
Matrix44 Matrix44::GetBillboardZ() const
{
	static Matrix44	matWorld;
	static Vector3	vDir;

	vDir.x = _13;
	vDir.y = _23;
	vDir.z = _33;

	if( vDir.x > 0.0F )
	{
		matWorld.SetRotationZ( atanf( vDir.y / vDir.x ) + MATH_PI / 2 );
	}
	else
	{
		matWorld.SetRotationZ( atanf( vDir.y / vDir.x ) - MATH_PI / 2 );
	} //if..else if..else..

	return matWorld;
} //Matrix44::GetBillboardZ

//---------------------------
// Matrix44x4_Inverse
//---------------------------
static void Matrix4x4_Inverse( const float b[][4], float a[][4] )
{
	long	indxc[4], indxr[4], ipiv[4];
	long	i = 0, icol = 0, irow = 0, j = 0, ir = 0, ic = 0;
	float	big, dum, pivinv, temp, bb;

	ipiv[0] = -1;
	ipiv[1] = -1;
	ipiv[2] = -1;
	ipiv[3] = -1;

	a[0][0] = b[0][0];
	a[1][0] = b[1][0];
	a[2][0] = b[2][0];
	a[3][0] = b[3][0];

	a[0][1] = b[0][1];
	a[1][1] = b[1][1];
	a[2][1] = b[2][1];
	a[3][1] = b[3][1];

	a[0][2] = b[0][2];
	a[1][2] = b[1][2];
	a[2][2] = b[2][2];
	a[3][2] = b[3][2];

	a[0][3] = b[0][3];
	a[1][3] = b[1][3];
	a[2][3] = b[2][3];
	a[3][3] = b[3][3];

	for (i = 0; i < 4; i++) {
		big = 0.0f;
		for (j = 0; j < 4; j++) {
			if (ipiv[j] != 0) {
				if (ipiv[0] == -1) {
					if ((bb = ( float) fabs(a[j][0])) > big) {
						big = bb;
						irow = j;
						icol = 0;
					} //if
				} else if (ipiv[0] > 0) {
					return;
				} //if..else if..
				if (ipiv[1] == -1) {
					if ((bb = ( float) fabs(( float) a[j][1])) > big) {
						big = bb;
						irow = j;
						icol = 1;
					} //if
				} else if (ipiv[1] > 0) {
					return;
				} //if..else if..
				if (ipiv[2] == -1) {
					if ((bb = ( float) fabs(( float) a[j][2])) > big) {
						big = bb;
						irow = j;
						icol = 2;
					} //if
				} else if (ipiv[2] > 0) {
					return;
				} //if..else if..
				if (ipiv[3] == -1) {
					if ((bb = ( float) fabs(( float) a[j][3])) > big) {
						big = bb;
						irow = j;
						icol = 3;
					} //if
				} else if (ipiv[3] > 0) {
					return;
				} //if..else if..
			} //if
		} //for

		++(ipiv[icol]);

		if (irow != icol) {
			
			temp = a[irow][0];
			a[irow][0] = a[icol][0];
			a[icol][0] = temp;
			
			temp = a[irow][1];
			a[irow][1] = a[icol][1];
			a[icol][1] = temp;
			
			temp = a[irow][2];
			a[irow][2] = a[icol][2];
			a[icol][2] = temp;
			
			temp = a[irow][3];
			a[irow][3] = a[icol][3];
			a[icol][3] = temp;
		} //if

		indxr[i] = irow;
		indxc[i] = icol;

		if (a[icol][icol] == 0.0) {
			return;
		} //if

		pivinv = 1.0f / a[icol][icol];
		a[icol][icol] = 1.0f;
		a[icol][0] *= pivinv;
		a[icol][1] *= pivinv;
		a[icol][2] *= pivinv;
		a[icol][3] *= pivinv;

		if (icol != 0) {
			dum = a[0][icol];
			a[0][icol] = 0.0f;
			a[0][0] -= a[icol][0] * dum;
			a[0][1] -= a[icol][1] * dum;
			a[0][2] -= a[icol][2] * dum;
			a[0][3] -= a[icol][3] * dum;
		} //if
		if (icol != 1) {
			dum = a[1][icol];
			a[1][icol] = 0.0f;
			a[1][0] -= a[icol][0] * dum;
			a[1][1] -= a[icol][1] * dum;
			a[1][2] -= a[icol][2] * dum;
			a[1][3] -= a[icol][3] * dum;
		} //if
		if (icol != 2) {
			dum = a[2][icol];
			a[2][icol] = 0.0f;
			a[2][0] -= a[icol][0] * dum;
			a[2][1] -= a[icol][1] * dum;
			a[2][2] -= a[icol][2] * dum;
			a[2][3] -= a[icol][3] * dum;
		} //if
		if (icol != 3) {
			dum = a[3][icol];
			a[3][icol] = 0.0f;
			a[3][0] -= a[icol][0] * dum;
			a[3][1] -= a[icol][1] * dum;
			a[3][2] -= a[icol][2] * dum;
			a[3][3] -= a[icol][3] * dum;
		} //if
	} //for
	if (indxr[3] != indxc[3]) {
		ir = indxr[3];
		ic = indxc[3];
		
		temp = a[0][ir];
		a[0][ir] = a[0][ic];
		a[0][ic] = temp;
		
		temp = a[1][ir];
		a[1][ir] = a[1][ic];
		a[1][ic] = temp;
		
		temp = a[2][ir];
		a[2][ir] = a[2][ic];
		a[2][ic] = temp;
		
		temp = a[3][ir];
		a[3][ir] = a[3][ic];
		a[3][ic] = temp;
	} //if
	if (indxr[2] != indxc[2]) {
		ir = indxr[2];
		ic = indxc[2];
		
		temp = a[0][ir];
		a[0][ir] = a[0][ic];
		a[0][ic] = temp;
		
		temp = a[1][ir];
		a[1][ir] = a[1][ic];
		a[1][ic] = temp;
		
		temp = a[2][ir];
		a[2][ir] = a[2][ic];
		a[2][ic] = temp;
		
		temp = a[3][ir];
		a[3][ir] = a[3][ic];
		a[3][ic] = temp;
	} //if
	if (indxr[1] != indxc[1]) {
		ir = indxr[1];
		ic = indxc[1];
		
		temp = a[0][ir];
		a[0][ir] = a[0][ic];
		a[0][ic] = temp;
		
		temp = a[1][ir];
		a[1][ir] = a[1][ic];
		a[1][ic] = temp;
		
		temp = a[2][ir];
		a[2][ir] = a[2][ic];
		a[2][ic] = temp;
		
		temp = a[3][ir];
		a[3][ir] = a[3][ic];
		a[3][ic] = temp;
	} //if
	if (indxr[0] != indxc[0]) {
		ir = indxr[0];
		ic = indxc[0];
		
		temp = a[0][ir];
		a[0][ir] = a[0][ic];
		a[0][ic] = temp;
		
		temp = a[1][ir];
		a[1][ir] = a[1][ic];
		a[1][ic] = temp;
		
		temp = a[2][ir];
		a[2][ir] = a[2][ic];
		a[2][ic] = temp;
		
		temp = a[3][ir];
		a[3][ir] = a[3][ic];
		a[3][ic] = temp;
	} //if
} //Matrix44x4_Invert





///////////////////////////////////////////////////////////////////////////////////////
// Sphere

//-----------------------------------------------------------------------------//
// Sphere 초기화
//-----------------------------------------------------------------------------//
void Sphere::SetSphere( float r, Vector3 center )
{
	m_R = r;
	m_vCenter = center;
}


//-----------------------------------------------------------------------------//
// 충돌 테스트
//-----------------------------------------------------------------------------//
BOOL Sphere::Collision( Sphere *pSphere )
{
	float len = sqrt( m_vCenter.LengthRoughly(pSphere->m_vCenter) );
	return (len < m_R + pSphere->m_R);
}

//-----------------------------------------------------------------------------//
// 출력
//-----------------------------------------------------------------------------//
void Sphere::Render()
{

}



///////////////////////////////////////////////////////////////////////////////////////
// Box

//-----------------------------------------------------------------------------//
// Box 설정
//-----------------------------------------------------------------------------//
void Box::SetBox( Vector3 *pMin, Vector3 *pMax )
{
	//       2-----3
	//      /|    /|
	//     0-----1 |
	//	   | 6 - | 7
	//	   |/    |/
	//	   4-----5
	m_Box[ 0] = Vector3( pMax->x, pMax->y, pMin->z );
	m_Box[ 1] = Vector3( pMin->x, pMax->y, pMin->z );
	m_Box[ 2] = Vector3( pMax->x, pMax->y, pMax->z );
	m_Box[ 3] = Vector3( pMin->x, pMax->y, pMax->z );
	m_Box[ 4] = Vector3( pMax->x, pMin->y, pMin->z );
	m_Box[ 5] = Vector3( pMin->x, pMin->y, pMin->z );
	m_Box[ 6] = Vector3( pMax->x, pMin->y, pMax->z );
	m_Box[ 7] = Vector3( pMin->x, pMin->y, pMax->z );
	m_Min = *pMin;
	m_Max = *pMax;
	m_matWorld.SetIdentity();
}


//-----------------------------------------------------------------------------//
// 월드행렬을 멤버변수 min, max에 적용한다.
//-----------------------------------------------------------------------------//
void Box::Update()
{
	Vector3 box[ 8];
	for( int i=0; i < 8; ++i )
		box[ i] = m_Box[ i] * m_matWorld;

	Vector3 _min = box[ 0];
	Vector3 _max = box[ 1];
	for( int i=0; i < 8; i++ )
	{
		Vector3 *v = &box[ i];
		if( _min.x > v->x )
			_min.x = v->x;
		else if( _max.x < v->x )
			_max.x = v->x;
		if( _min.y > v->y )
			_min.y = v->y;
		else if( _max.y < v->y )
			_max.y = v->y;
		if( _min.z > v->z )
			_min.z = v->z;
		else if( _max.z < v->z )
			_max.z = v->z;
	}
	m_Min = _min;
	m_Max = _max;
}


//-----------------------------------------------------------------------------//
// bOwn = TRUE : 멤버변수 m_Min, m_Max 를 리턴한다.
// bOwn = FALSE: m_Box의 최대최소를 구한후 리턴된다.
// bUpdate = TRUE : 구해진 min, max값을 멤버변수에 업데이트 한다.
//-----------------------------------------------------------------------------//
void Box::GetMinMax( Vector3 *pMin, Vector3 *pMax, BOOL bOwn, BOOL bUpdate ) // bOwn = TRUE, bUpdate = FALSE
{
	if( bOwn )
	{
		*pMin = m_Min;
		*pMax = m_Max;
	}
	else
	{
		// world 행렬 적용
		Vector3 _min = m_Min * m_matWorld;
		Vector3 _max = m_Max * m_matWorld;

		*pMin = _min;
		*pMax = _max;

		if( bUpdate )
		{
			m_Min = _min;
			m_Max = _max;
		}
	}
}


//-----------------------------------------------------------------------------//
// 충돌테스트 충돌되었다면 리턴 TRUE
// AABB 충돌테스트
//-----------------------------------------------------------------------------//
BOOL Box::Collision( Box *pBox )
{
	return ((m_Min.x < pBox->m_Max.x) && (m_Min.y < pBox->m_Max.y) && (m_Min.z < pBox->m_Max.z))
			&& ((pBox->m_Min.x < m_Max.x) && (pBox->m_Min.y < m_Max.y) && (pBox->m_Min.z < m_Max.z));
}

void Box::SetWorldTM( Matrix44 *mat ) 
{ 
	m_matWorld = *mat; 
}

void Box::MultiplyWorldTM( Matrix44 *mat )
{
	m_matWorld *= *mat;
}


// length( m_Max - m_Min )
float Box::GetSize()
{
	return sqrt( m_Max.LengthRoughly(m_Min) );
}


//-----------------------------------------------------------------------------//
// 출력
//-----------------------------------------------------------------------------//
void Box::Render()
{
	g_pDevice->SetRenderState( D3DRS_LIGHTING, FALSE );
	g_pDevice->SetFVF( D3DFVF_XYZ );
	g_pDevice->SetTransform( D3DTS_WORLD, (D3DXMATRIX*)&m_matWorld );
	g_pDevice->DrawPrimitiveUP( D3DPT_LINESTRIP, 7, m_Box, sizeof(Vector3) );	
	g_pDevice->SetRenderState( D3DRS_LIGHTING, TRUE );
}
//-----------------------------------------------------------------------------//
// 계층구조출력시 사용된다.
//-----------------------------------------------------------------------------//
void Box::Render_Hierarchy()
{
	g_pDevice->SetRenderState( D3DRS_LIGHTING, FALSE );
	g_pDevice->SetFVF( D3DFVF_XYZ );
	g_pDevice->MultiplyTransform( D3DTS_WORLD, (D3DXMATRIX*)&m_matWorld );
	g_pDevice->DrawPrimitiveUP( D3DPT_LINESTRIP, 7, m_Box, sizeof(Vector3) );	
	g_pDevice->SetRenderState( D3DRS_LIGHTING, TRUE );
}


