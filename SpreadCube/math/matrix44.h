#pragma once

	struct Vector3;
	struct Quaternion;

	struct Matrix44
	{
		union
		{
			struct
			{
				float	_11, _12, _13, _14;
				float	_21, _22, _23, _24;
				float	_31, _32, _33, _34;
				float	_41, _42, _43, _44;
			};
			float	m[4][4];
		};

		Matrix44();
		inline void SetIdentity();
		void	SetRotationX( const float angle );
		void	SetRotationY( const float angle );
		void	SetRotationZ( const float angle );
		void	SetTranslate( const Vector3& pos );
		void	SetScale( const Vector3& scale);
		Vector3 GetPosition() const;
		void SetView( const Vector3& pos, const Vector3& dir0, const Vector3& up0);
		void	SetProjection( const float fov, const float aspect, const float nearPlane, const float farPlane );
		Quaternion GetQuaternion() const;

		Matrix44 operator * ( const Matrix44& rhs ) const;
		Matrix44& operator *= ( const Matrix44& rhs );
	};


