// Matrix.h: interface for the Matrix class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MATRIX_H__E2559E10_6977_4DBC_B101_DCBA0C07C86A__INCLUDED_)
#define AFX_MATRIX_H__E2559E10_6977_4DBC_B101_DCBA0C07C86A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//#include <cmath>
#include <stdio.h>
#include <math.h>

struct Vertex
{
	double x;
	double y;
	double z;
	double h;

	Vertex()
	{
		x = 0.0;
		y = 0.0;
		z = 0.0;
		h = 1;
	}
	
	Vertex(double v1, double v2, double v3, double vh = 1.0)
	{
		x = v1;
		y = v2;
		z = v3;
		h = vh;
	}

	Vertex operator+(Vertex vertex)
	{
		return Vertex(x + vertex.x, y + vertex.y, z + vertex.z);
	}

	Vertex operator-(Vertex vertex)
	{
		return Vertex(x - vertex.x, y - vertex.y, z - vertex.z);
	}

	BOOL operator==(Vertex vertex)
	{
		return (x==vertex.x && y==vertex.y && z==vertex.z);
	}
	BOOL operator!=(Vertex vertex)
	{
		return (x!=vertex.x || y!=vertex.y || z!=vertex.z);
	}

	Vertex operator*(int number)
	{
		return Vertex(x * number, y * number, z * number);
	}

	Vertex operator*(double number)
	{
		return Vertex(x * number, y * number, z * number);
	}

	Vertex operator/(double number)
	{
		return Vertex(x / number, y / number, z / number);
	}



	void Normalization()
	{
		double dis = sqrt(pow(x, 2.0) + pow(y, 2.0) + pow(z, 2.0));
		x /= dis;
		y /= dis;
		z /= dis;
	}

	Vertex CrossProduct(Vertex b)
	{
		return Vertex(y*b.z - z*b.y, z*b.x - x*b.z, x*b.y - y*b.x);
	}

	double DotProduct(Vertex b)
	{
		return x*b.x + y*b.y + z*b.z;
	}

	double Length()
	{
		return sqrt(x*x + y*y + z*z);
	}
};

struct Color
{
	int _r,_g,_b;
	Color() {}
	Color( COLORREF color )
	{
		_r = GetRValue(color); 
		_g = GetGValue(color);
		_b = GetBValue(color);
	}
	Color( int r, int g, int b )
	{
		_r = min( r, 255 );
		_g = min( g, 255 );
		_b = min( b, 255 );
		_r = max( _r, 0 );
		_g = max( _g, 0 );
		_b = max( _b, 0 );
	}
	Color operator+(Color color)
	{
		return Color(_r + color._r, _g + color._g, _b + color._b);
	}
	Color operator-(Color color)
	{
		return Color(_r - color._r, _g - color._g, _b - color._b);
	}
	Color operator*(int number)
	{
		return Color(_r * number, _g * number, _b * number);
	}
	Color operator*(double number)
	{
		return Color( (int)((double)_r * number), (int)((double)_g * number), (int)((double)_b * number) );
	}
	void Normalization()
	{
		double dis = sqrt( (double)(_r*_r + _g*_g + _b*_b) );
		_r = (int)((double)_r / dis);
		_g = (int)((double)_g / dis); 
		_b = (int)((double)_b / dis);
	}
	COLORREF GetCOLORREF() 
	{
		return RGB( _r, _g, _b );
	}
	double Length()
	{
		return sqrt((double)(_r*_r + _g*_g + _b*_b));
	}

};


struct Face
{
	int v1;
	int v2;
	int v3;

	BOOL visible;	// 화면에 보이면 TRUE

	Face* f1;	// v1-v2와 연결된 face
	Face* f2;	// v2-v3와 연결된 face
	Face* f3;	// v3-v1와 연결된 face

	// line index
	int l1;
	int l2;
	int l3;

	Face()
	{
	}

	Face(int f1, int f2, int f3)
	{
		v1 = f1;
		v2 = f2;
		v3 = f3;
	}
};

class Matrix  
{
public:
	Matrix();
	virtual ~Matrix();

	double m[16];

	void operator=(Matrix *b);
	Matrix operator+(const Matrix& b);
	Matrix operator-(const Matrix& b);
	Matrix operator*(const Matrix& b);
	Vertex operator*(const Vertex& b);
};


struct Line
{
	int v1,v2;		// vertex array index
	Vertex norm;	// line normal

};



#endif // !defined(AFX_MATRIX_H__E2559E10_6977_4DBC_B101_DCBA0C07C86A__INCLUDED_)
