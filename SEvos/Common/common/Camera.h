//-------------------------------------------------------
//	Camera.h
//-------------------------------------------------------

#if !defined(__CAMERA_H__)
#define __CAMERA_H__



//-------------------------------
//	Camera
//-------------------------------
class CCamera
{
public:
	CCamera();
	~CCamera();

	// Execute Camera Setting.
	void	Execute();
	void	ExecuteCam();

	// Update
	void	Update( DWORD dwTick );
	void	Animate( int nDelta );

	// Set Camera Data
		void	SetViewPort( int Width, int Height );	// ºäÆ÷Æ® ¼³Á¤
		void	SetFov( const WORD wAngle );			// Fov
		void	SetZoom( const float fDist );			// Zoom
		void	SetWorld( const Matrix44& matWorld );	// World Matrix
		BOOL	SetPosition( const Vector3& vPos );		// Position
		void	SetFarPlane( const float fFarPlane );	// Far Plane
		void	SetNearPlane( const float fNearPlane );	// Near Plane
		void	SetUpVector( const Vector3& vUp );		// Set Up Vector
		void	SetEye( const Vector3& vEye );			// Set Eye Vector
		void	SetLookAt( const Vector3& vat );
		void	SetMaxZoom( const float fMax );			// Set Max Zoom
		void	SetCamValue( const float fPitch,
							 const float fYaw,
							 const float fRoll, 
							 const float fDistance );
		void	SetGoal( const Vector3& vGoal );
		void	SetActionX( const BOOL bAction );
		void	SetActionY( const BOOL bAction );
		void	SetActionZ( const BOOL bAction );
		void	SetActRateX( const float fRate );
		void	SetActRateY( const float fRate );
		void	SetActRateZ( const float fRate );
	// ~Set Camera Data			

	// Rotate And Zoom
		void	Zoom( const float fOffset );				// Zoom
		void	RotateX( const float fAngle );				// X Axis
		void	RotateY( const float fAngle );				// X Axis
		void	RotateZ( const float fAngle );				// Z Axis ( D3D, Y Axis )			
	// ~Rotate And Zoom	

	// Get Camera's Data
		float	GetZoom();			// Zoom Distance

		Vector3 GetFront();			// a Vector in front of Camera
		Vector3 GetRight();			// a Vector right of Camera
		Vector3	GetPosition();		// a Position Of Camera
		Vector3 GetGoal();			// a Goal Position Camera
		BOOL GetActionX();
		BOOL GetActionY();
		BOOL GetActionZ();
		float GetActRateX();
		float GetActRateY();
		float GetActRateZ();

		const Vector3& GetEye();	// a Eye Vector
		const float GetFarPlane();	// a Far Plane			

		Matrix44 GetViewMatrix();		// a View Matrix
		Matrix44 GetTopViewMatrix();	// Top View Matrix

		Vector3& GetLookAt()			{ return m_vAt;	}

		void GetVec3Project( Vector3 *pOut, Vector3 *pV );
		void GetViewProjMatrix( Matrix44 *pOut );
		void GetViewMatrix( Matrix44 *pOut ) { *pOut = m_matView; }
		void GetProjectMatrix( Matrix44 *pOut ) { *pOut = m_matProj; }

		BOOL	IsMoved()
		{ 
			return m_bMove; 
		}
	// ~Get Camera's Data

	// Is Changes
	BOOL IsChange()
	{
		return m_bStateChange;
	} //IsChange

public:
	Matrix44	m_matProj;
	Matrix44	m_matView;
	int			m_Width;
	int			m_Height;
	D3DVIEWPORT9 m_Viewport;

private:
	Vector3		m_vGoal;			//	Goal
	Vector3		m_vEye;				//	Eye	
	Vector3		m_vAt;				//	At
	Vector3		m_vUp;				//	Up
	WORD		m_wFovAngle;		//	Fov Angle

	float		m_fDistance;		//	Distance ( Zoom )
	float		m_fPitch;			//	Pitch
	float		m_fYaw;				//	Yaw
	float		m_fRoll;			//	Roll

	float		m_fPrevDistance;
	float		m_fPrevPitch;
	float		m_fPrevYaw;
	BOOL		m_bMove;			//	Was Move Cam

	float		m_fYawRange;		//	Range Of Yaw
	float		m_fFarPlane;		//	Far Plane
	float		m_fNearPlane;		//	Near Plane
	float		m_fMaxZoom;			//	Max of Zoom
	float		m_fMinPitch;		//	Min of Pitch

	WORD		m_wCamAction;		//	Camera Action
	DWORD		m_dwActionTick;		//	Action Tick

	BOOL		m_bStateChange;

	int			m_nAniIncTime;
	float		m_fMoveVelocity;
	Vector3		m_vMoveDir;


}; //class CCamera
extern CCamera g_Camera;

#endif // __CAMERA_H__