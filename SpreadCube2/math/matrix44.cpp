
#include "math.h"

Matrix44::Matrix44()
{
	SetIdentity();
}

void Matrix44::SetIdentity()
{
	_11 = _22 = _33 = _44 = 1;
	_12 = _13 = _14 = _21 = _23 = _24 = _31 = _32 = _34 = _41 = _42 = _43 = 0;
}


void	Matrix44::SetRotationX( const float angle )
{
	float fCos = cosf( angle );
	float fSin = sinf( angle );
	SetIdentity();
	_22 = fCos;
	_23 = fSin;
	_32 = -fSin;
	_33 = fCos;
}


void	Matrix44::SetRotationY( const float angle )
{
	float fCos = cosf( angle );
	float fSin = sinf( angle );
	SetIdentity();
	_11 = fCos;
	_13 = -fSin;
	_31 = fSin;
	_33 = fCos;
}


void	Matrix44::SetRotationZ( const float angle )
{
	float fCos = cosf( angle );
	float fSin = sinf( angle );
	SetIdentity();
	_11 = fCos;
	_12 = fSin;
	_21 = -fSin;
	_22 = fCos;
}


void	Matrix44::SetTranslate( const Vector3& pos )
{
	SetIdentity();
	_41 = pos.x;
	_42 = pos.y;
	_43 = pos.z;
}


void	Matrix44::SetScale( const Vector3& scale)
{
	SetIdentity();
	_11 = scale.x;
	_22 = scale.y;
	_33 = scale.z;
}


Vector3 Matrix44::GetPosition() const
{
	return Vector3(_41, _42, _43);
}


Matrix44 Matrix44::operator * ( const Matrix44& rhs ) const
{
	Matrix44 matrix;
	ZeroMemory( &matrix, sizeof( matrix ) );

	for( int i = 0 ; i < 4 ; ++i )
	{
		for( int j = 0 ; j < 4 ; ++j )
		{			
			for( int k = 0 ; k < 4 ; ++k )
			{
				matrix.m[i][j] += m[i][k] * rhs.m[k][j];
			}
		}
	}

	return matrix;
}


Matrix44& Matrix44::operator *= ( const Matrix44& rhs )
{
	Matrix44 matrix;
	ZeroMemory( &matrix, sizeof( matrix ) );

	for( int i = 0 ; i < 4 ; ++i )
	{
		for( int j = 0 ; j < 4 ; ++j )
		{			
			for( int k = 0 ; k < 4 ; ++k )
			{
				matrix.m[i][j] += m[i][k] * rhs.m[k][j];
			}
		}
	}

	*this = matrix;
	return *this;
}


void Matrix44::SetView( const Vector3& pos, const Vector3& dir0, const Vector3& up0 )
{
	Vector3 vDir;
	Vector3 vUp;
	Vector3 vCross;

	vDir = dir0.Normal();
	vCross = up0.CrossProduct( vDir );
	vCross.Normalize();
	vUp = vDir.CrossProduct( vCross );

	_11 = vCross.x;
	_12 = vUp.x;
	_13 = vDir.x;
	_14 = 0.0f;
	_21 = vCross.y;
	_22 = vUp.y;
	_23 = vDir.y;
	_24 = 0.0f;
	_31 = vCross.z;
	_32 = vUp.z;
	_33 = vDir.z;
	_34 = 0.0f;
	_41 = -pos.DotProduct( vCross );
	_42 = -pos.DotProduct( vUp );
	_43 = -pos.DotProduct( vDir );
	_44 = 1.0f;
}


void Matrix44::SetProjection(	const float fov, const float aspect, const float nearPlane, const float farPlane )
{
	float fH = cosf(fov / 2.f) / sinf(fov / 2.f);
	float fW = fH / aspect;
	float fQ = farPlane / (farPlane - nearPlane);

	_12 = _13 = _14 = 0.0f;
	_21 = _23 = _24 = 0.0f;
	_31 = _32 = 0.0f;
	_41 = _42 = 0.0f;
	_11 = fW;
	_22 = fH;
	_33 = fQ;
	_34 = 1.0f;
	_43 = -fQ * nearPlane;
	_44 = 1.0f;
	_44 = 0.f;
}


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
	}

	return q;
}
