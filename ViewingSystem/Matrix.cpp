// Matrix.cpp: implementation of the Matrix class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ViewingSystem.h"
#include "Matrix.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Matrix::Matrix()
{
	for (int i = 0; i < 16; i++)
	{
		int j = i / 4;
		int k = i % 4;
		if (j == k)
			m[i] = 1;
		else
			m[i] = 0;
	}
}

Matrix::~Matrix()
{

}

void Matrix::operator=(Matrix *b)
{
	for (int i = 0; i < 16; i++)
		this->m[i] = b->m[i];

	delete[] b;
}

Matrix Matrix::operator+(const Matrix& b)
{
	Matrix r;
	for (int i = 0; i < 16; i++)
		r.m[i] = this->m[i] + b.m[i];
	return r;
}

Matrix Matrix::operator-(const Matrix& b)
{
	Matrix r;
	for (int i = 0; i < 16; i++)
		r.m[i] = this->m[i] - b.m[i];
	return r;
}

Matrix Matrix::operator*(const Matrix& b)
{
	Matrix r;
	for (int i = 0; i < 16; i++)
	{
		int j = i / 4;
		int k = i % 4;

		r.m[i] = this->m[j*4] * b.m[k] +
			 this->m[j*4+1] * b.m[k+4] +
			 this->m[j*4+2] * b.m[k+8] +
			 this->m[j*4+3] * b.m[k+12];
		
	}
	return r;
}

Vertex Matrix::operator*(const Vertex &b)
{
	Vertex r;
	r.x = this->m[0]*b.x + this->m[1]*b.y + this->m[2]*b.z + this->m[3]*1.0;
	r.y = this->m[4]*b.x + this->m[5]*b.y + this->m[6]*b.z + this->m[7]*1.0;
	r.z = this->m[8]*b.x + this->m[9]*b.y + this->m[10]*b.z + this->m[11]*1.0;
	r.h = this->m[12]*b.x + this->m[13]*b.y + this->m[14]*b.z + this->m[15]*1.0;
	return r;
}