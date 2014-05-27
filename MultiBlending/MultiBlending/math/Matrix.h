//-------------------------------------------------------
//	Matrix44.h
//-------------------------------------------------------

#ifndef __Math_Matrix44_Header__
#define __Math_Matrix44_Header__

//-----------------------------
// 4 x 4 Matrix
//-----------------------------
struct Matrix44
{
	// union
		union
		{
			struct
			{
				float	_11, _12, _13, _14;
				float	_21, _22, _23, _24;
				float	_31, _32, _33, _34;
				float	_41, _42, _43, _44;
			}; //struct
			float	m[4][4];
		}; //union
	// ~union

	// Constructor
		Matrix44();
	// ~Constructor

	// operator
		Matrix44		operator * ( const Matrix44& m ) const;
		Matrix44		operator * ( const float f );
		Matrix44&	operator *= ( const Matrix44& m );
		const float& operator () ( const int nRow, const int nColumn );
	// ~operator

	// Function
		Matrix44	Inverse() const;
		Quaternion GetQuaternion() const;

		void		SetIdentity();
		void		SetRotationX( const float fAngle );
		void		SetRotationY( const float fAngle );
		void		SetRotationZ( const float fAngle );
		void		SetRotation( const Vector3& vINAxis, const float fAngle );

		void		SetView( const Vector3& vPos, const Vector3& vINDir, const Vector3& vINUp );
		void		SetProjection( const float fFOV, const float fAspect, const float fNearPlane, const float fFarPlane );

		// Set World
			void		SetWorld( const Vector3& vPos );
			void		SetWorld( const Vector3& vPos, const Quaternion& qRot );
			void		SetWorld( const Vector3& vPos, const Vector3& vDir, const Vector3& vUp );
		// ~Set World

		//void		SetPosition( const Vector3& vPos );
		void		Translate( const Vector3& vPos );

		Vector3		GetPosition();

		Matrix44		GetBillboard() const;
		Matrix44		GetBillboardX() const;
		Matrix44		GetBillboardZ() const;
	// ~Function
}; //struct Matrix44

// Multiply
void Matrix44Multiply( Matrix44& matA, Matrix44& matB, Matrix44& matOut );



///////////////////////////////////////////////////////////////////////////////////////
// Sphere

class Sphere
{
public:
	Sphere() {}
	Sphere( float r, Vector3 p ) : m_R(r), m_vCenter(p) {}

public:
	float m_R;			// 반지름
	Vector3 m_vCenter;	// 위치

public:
	void SetSphere( float r, Vector3 center );
	void SetRadius( float r ) { m_R = r; }
	BOOL Collision( Sphere *pSphere );
	void SetCenter( Vector3 *pPos ) { m_vCenter = *pPos; }
	void Render();

};


///////////////////////////////////////////////////////////////////////////////////////
// Box

// Box 클래스
// d3d::CreateBox() 함수로 Bounding Box를 생성한다.
class Box
{
public:
	Box() {}

public:
	Vector3 m_Min, m_Max;			// Box의 최대 최소값
	Vector3 m_Box[ 8];
	Matrix44 m_matWorld;

public:
	void SetBox( Vector3 *pMin, Vector3 *pMax );
	void GetMinMax( Vector3 *pMin, Vector3 *pMax, BOOL bOwn=TRUE, BOOL bUpdate=FALSE );
	float GetSize();	// length( m_Max - m_Min )
	void Update();
	BOOL Collision( Box *pBox );
	void SetWorldTM( Matrix44 *mat );
	void MultiplyWorldTM( Matrix44 *mat );
	void Render();
	void Render_Hierarchy();
};



#endif //#ifndef __Math_Matrix44_Header__
