//-------------------------------------------------------
//	Math.h
//-------------------------------------------------------

#ifndef __Math_Header__
#define __Math_Header__

// Include
	#include <d3dx9.h>
	#include <windows.h>
	#include <math.h>
	#include <time.h>
// ~Include

// Const
	const float MATH_PI = 3.14159265358979323846F;
	const float MATH_EPSILON = 0.0005F;//1.0e-5F;
// ~Const

// Macro
	#define random( x )		(int)( ( (long)rand() * (x) ) / ( RAND_MAX + 1 ) )
	#define randomize()		srand( (unsigned)time( NULL ) )

	#define ABS( x )	( (x) >= 0 ? (x) : -(x) )

	#define MAX( a, b )	( (a) > (b) ? (a) : (b) )
	#define MIN( a, b )	( (a) < (b) ? (a) : (b) )
	#define GET_MARK( a ) ( (a) < 0 ? 0 : 1 )

	#define ANGLE( x )	( (x) * MATH_PI / 180.0F )	// angle to radian
	#define DEGREE( x ) ( (x) * 180 / MATH_PI )		// radian to angle
	#define ANGLETORAD( x ) (ANGLE( x ))
	#define RADTOANGLE( x ) (DEGREE( x ))

	#define	SQR(x)		( (x) * (x) )

	// limits a value to low and high
	#define LIMIT_RANGE(low, value, high)	{	if (value < low)	value = low;	else if(value > high)	value = high;	}
	#define FLOAT_EQ(x,v)	( ((v) - MATH_EPSILON) < (x) && (x) < ((v) + MATH_EPSILON) )
	__forceinline void SWAP(float &x, float &y) {	float temp;	temp = x;	x = y;	y = temp;	}

// ~Macro

// Inline Function
	int GetRand( int x, int add = 0 );

	// 가까운 2의 승수 구하기.
	unsigned __fastcall CeilPowerOf2(unsigned x);
// ~Inline Function
	
// Structure
	struct Vector2;		// 2 Axis 
	struct Vector3;		// 3 Axis
	struct Matrix44;	// 4 Row
	struct Quaternion;	// 
	struct Triangle;	// 
	struct SBezier;		//
	struct Line2;
	class CFrustum;
// ~Structure

// Include
	#include "Vector2.h"
	#include "Vector3.h"
	#include "Matrix.h"	
	#include "Quaternion.h"
//	#include "Inc/Interpolate.h"
//	#include "Inc/Triangle.h"
//	#include "Inc/Plane.h"
//	#include "Inc/Collision.h"	
//	#include "Inc/Curve.h"
//	#include "Inc/Frustum.h"
//	#include "Inc/Line.h"
// ~Include


#endif //#ifndef __Math_Header__
