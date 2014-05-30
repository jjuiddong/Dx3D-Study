#pragma once


struct Matrix44;
struct Vector3
{
	float x,y,z;

	Vector3() : x(0), y(0), z(0) {}
	Vector3(float x0, float y0, float z0) : x(x0), y(y0), z(z0) {}

	bool IsEmpty() const;
	float Length() const;
	Vector3 Normal() const;
	void Normalize();
	float	DotProduct( const Vector3& v ) const;
	Vector3 CrossProduct( const Vector3& v ) const;
	
	Vector3 operator + () const;
	Vector3 operator - () const;
	Vector3 operator + ( const Vector3& rhs ) const;
	Vector3 operator - ( const Vector3& rhs ) const;
	Vector3& operator += ( const Vector3& rhs );
	Vector3& operator -= ( const Vector3& rhs );
	Vector3& operator *= ( const Vector3& rhs );
	Vector3& operator /= ( const Vector3& rhs );

	Vector3 operator * ( const Matrix44& rhs ) const;
	Vector3& operator *= ( Matrix44& rhs );

	template <class T>
	Vector3 operator * ( T t ) const {
		return Vector3(x*t, y*t, z*t);
	}

	template <class T>
	Vector3 operator / ( T t ) const {
		return Vector3(x/t, y/t, z/t);
	}

	template <class T>
	Vector3& operator *= ( T t ) {
		*this = Vector3(x*t, y*t, z*t);
		return *this;
	}

	template <class T>
	Vector3& operator /= ( T t ) {
		*this = Vector3(x/t, y/t, z/t);
		return *this;
	}

	bool operator==( const Vector3 &rhs ) const;

};
