// ViewingSystemDoc.cpp : implementation of the CViewingSystemDoc class
//

#include "stdafx.h"
#include "ViewingSystem.h"

#include <fstream>
//#include <fstream.h>
//#include <cmath>
#include <math.h>

#include "ViewingSystemDoc.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CViewingSystemDoc

IMPLEMENT_DYNCREATE(CViewingSystemDoc, CDocument)

BEGIN_MESSAGE_MAP(CViewingSystemDoc, CDocument)
	//{{AFX_MSG_MAP(CViewingSystemDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CViewingSystemDoc construction/destruction

CViewingSystemDoc::CViewingSystemDoc()
{
	// TODO: add one-time construction code here
	m_vertices = 0;
	m_faces = 0;

	m_numberOfVertices = 0;
	m_numberOfFaces = 0;

	m_viewingMode = PERSPECTIVE;

	m_eye = Vertex(0.0, 0.0, 500.0);
	m_lookAt = Vertex(0.0, 0.0, 0.0);
	m_up = Vertex(0.0, 1.0, 0.0);

	m_shadingMode = WIREFRAME;

	m_light = Vertex(1000.0, 1000.0, 1000.0);

	m_normalface[ 0] = NULL;
	m_normalface[ 1] = NULL;
	m_normalface[ 2] = NULL;

	m_linecnt = 0;
}

CViewingSystemDoc::~CViewingSystemDoc()
{
}

BOOL CViewingSystemDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// CViewingSystemDoc serialization

void CViewingSystemDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}

/////////////////////////////////////////////////////////////////////////////
// CViewingSystemDoc diagnostics

#ifdef _DEBUG
void CViewingSystemDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CViewingSystemDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CViewingSystemDoc commands

void CViewingSystemDoc::FileOpen(CString filename)
{
	using namespace std;
	ifstream fin(filename);						// Visual studio 6.0
	//ifstream fin(filename.AllocSysString());	// Visual studio 2005
	m_linecnt = 0;
	m_LineMap.clear();

	CString check, dump;
	int numberOfVertices, numberOfFaces;

	fin >> check.GetBuffer(100) >> dump.GetBuffer(100) >> numberOfVertices;

	if (check == "VERTEX")
	{
		m_numberOfVertices = numberOfVertices;

		if (m_vertices == 0)
		{
			m_vertices = new Vertex[numberOfVertices];
			m_normals = new Vertex[numberOfVertices];
		}
		else 
		{
			delete[] m_vertices;
			m_vertices = new Vertex[numberOfVertices];
			delete[] m_normals;
			m_normals = new Vertex[numberOfVertices];
		}

		double num1, num2, num3;
		for (int i = 0; i < numberOfVertices; i++)
		{
			fin >> num1 >> num2 >> num3;

			m_vertices[i] = Vertex(num1, num2, num3);
			m_normals[ i] = Vertex( 0, 0, 0 );
		}

		fin >> check.GetBuffer(100) >> dump.GetBuffer(100) >> numberOfFaces;

		if (check == "FACE")
		{
			m_numberOfFaces = numberOfFaces;

			if (m_faces == 0)
			{
				m_faces = new Face[numberOfFaces];
				m_normalface[0] = new Vertex[numberOfFaces];
				m_normalface[1] = new Vertex[numberOfFaces];
				m_normalface[2] = new Vertex[numberOfFaces];
				m_lines = new Line[ numberOfFaces*3];
			}
			else 
			{
				delete[] m_faces;
				m_faces = new Face[numberOfFaces];

				delete[] m_normalface[0];
				delete[] m_normalface[1];
				delete[] m_normalface[2];
				delete[] m_lines;
				m_normalface[0] = new Vertex[numberOfFaces];
				m_normalface[1] = new Vertex[numberOfFaces];
				m_normalface[2] = new Vertex[numberOfFaces];
				m_lines = new Line[ numberOfFaces*3];

			}

			int num4, num5, num6;
			for (int i = 0; i < numberOfFaces; i++)
			{
				fin >> num4 >> num5 >> num6;

				m_faces[i] = Face(num4, num5, num6);
			}

			// Vertex Normal을 구한다.
			for (int i = 0; i < numberOfFaces; i++)
			{
				Vertex v1 = m_vertices[ m_faces[i].v1];
				Vertex v2 = m_vertices[ m_faces[i].v2];
				Vertex v3 = m_vertices[ m_faces[i].v3];

				Vertex a = v3 - v1;
				Vertex b = v2 - v1;
				Vertex norm = b.CrossProduct( a );
				norm.Normalization();

				m_normalface[ 0][ i] = norm;
				m_normalface[ 1][ i] = norm;
				m_normalface[ 2][ i] = norm;

				Vertex norm1 = norm;
				Vertex norm2 = norm;
				Vertex norm3 = norm;

				// 법선벡터가 하나 이상이라면 평균을 저장한다.
				if( m_normals[ m_faces[i].v1] != Vertex(0,0,0) )
				{
					norm1 = m_normals[ m_faces[i].v1] + norm;
					norm1.Normalization();
				}
				m_normals[ m_faces[i].v1] = norm1;

				if( m_normals[ m_faces[i].v2] != Vertex(0,0,0) )
				{
					norm2 = m_normals[ m_faces[i].v2] + norm;
					norm2.Normalization();
				}
				m_normals[ m_faces[i].v2] = norm2;

				if( m_normals[ m_faces[i].v3] != Vertex(0,0,0) )
				{
					norm3 = m_normals[ m_faces[i].v3] + norm;
					norm3.Normalization();
				}
				m_normals[ m_faces[i].v3] = norm3;

				// Face에 라인을 설정한다.
				// v1-v2 line
				int key1 = MAKELONG( m_faces[i].v1, m_faces[i].v2 );
				LineItor it1 = m_LineMap.find( std::map<int,int>::key_type(key1) );
				if( m_LineMap.end() == it1 )
				{
					// 라인의 노말벡터를 구한다.
					Vertex lnorm = norm1 + norm2;
					lnorm.Normalization();

					m_lines[ m_linecnt].v1 = m_faces[i].v1;
					m_lines[ m_linecnt].v2 = m_faces[i].v2;
					m_lines[ m_linecnt].norm = lnorm;
					m_LineMap.insert( std::map<int,int>::value_type(key1,m_linecnt) );
					m_faces[i].l1 = m_linecnt;
					++m_linecnt;
				}
				else
				{
					// 라인의 노말벡터를 업데이트
					Vertex lnorm = norm1 + norm2;
					lnorm.Normalization();
					int idx = it1->second;
					m_faces[i].l1 = idx;
					m_lines[ idx].norm = lnorm;
				}
				
				// v2-v3 line
				int key2 = MAKELONG( m_faces[i].v2, m_faces[i].v3 );
				LineItor it2 = m_LineMap.find( std::map<int,int>::key_type(key2) );
				if( m_LineMap.end() == it2 )
				{
					// 라인의 노말벡터를 구한다.
					Vertex lnorm = norm2 + norm3;
					lnorm.Normalization();

					m_lines[ m_linecnt].v1 = m_faces[i].v2;
					m_lines[ m_linecnt].v2 = m_faces[i].v3;
					m_lines[ m_linecnt].norm = lnorm;
					m_LineMap.insert( std::map<int,int>::value_type(key2,m_linecnt) );
					m_faces[i].l2 = m_linecnt;
					++m_linecnt;
				}
				else
				{
					// 라인의 노말벡터를 업데이트
					Vertex lnorm = norm2 + norm3;
					lnorm.Normalization();
					int idx = it2->second;
					m_faces[i].l2 = idx;
					m_lines[ idx].norm = lnorm;
				}
				
				// v3-v1 line
				int key3 = MAKELONG( m_faces[i].v3, m_faces[i].v1 );
				LineItor it3 = m_LineMap.find( std::map<int,int>::key_type(key3) );
				if( m_LineMap.end() == it3 )
				{
					// 라인의 노말벡터를 구한다.
					Vertex lnorm = norm3 + norm1;
					lnorm.Normalization();

					m_lines[ m_linecnt].v1 = m_faces[i].v3;
					m_lines[ m_linecnt].v2 = m_faces[i].v1;
					m_lines[ m_linecnt].norm = lnorm;
					m_LineMap.insert( std::map<int,int>::value_type(key3,m_linecnt) );
					m_faces[i].l3 = m_linecnt;				
					++m_linecnt;
				}
				else
				{
					// 라인의 노말벡터를 업데이트
					Vertex lnorm = norm3 + norm1;
					lnorm.Normalization();
					int idx = it3->second;
					m_faces[i].l3 = idx;
					m_lines[ idx].norm = lnorm;
				}

			}

			for( int i=0; i < numberOfFaces; ++i )
			{
				m_faces[i].f1 = NULL;
				m_faces[i].f2 = NULL;
				m_faces[i].f3 = NULL;
				m_faces[i].visible = TRUE;
			}

			for( int i=0; i < numberOfFaces; ++i )
			{
				// Face와 중첩된 vertex를 가진 Face를 찾는다.
				BOOL f1=TRUE,f2=TRUE,f3=TRUE;
				for( int k=0; k < numberOfFaces; ++k )
				{
					if( k != i )
					{
						if( f1 &&
							((m_faces[i].v1 == m_faces[k].v1) || (m_faces[i].v1 == m_faces[k].v2) || (m_faces[i].v1 == m_faces[k].v3)) &&
							((m_faces[i].v2 == m_faces[k].v1) || (m_faces[i].v2 == m_faces[k].v2) || (m_faces[i].v2 == m_faces[k].v3))   )
						{
							// v1-v2 와 중첩
							m_faces[i].f1 = &m_faces[k];
							f1 = FALSE;
						}

						if( f2 &&
							((m_faces[i].v2 == m_faces[k].v1) || (m_faces[i].v2 == m_faces[k].v2) || (m_faces[i].v2 == m_faces[k].v3)) &&
							((m_faces[i].v3 == m_faces[k].v1) || (m_faces[i].v3 == m_faces[k].v2) || (m_faces[i].v3 == m_faces[k].v3))   )
						{
							// v2-v3 와 중첩
							m_faces[i].f2 = &m_faces[k];
							f2 = FALSE;
						}

						if( f3 &&
							((m_faces[i].v3 == m_faces[k].v1) || (m_faces[i].v3 == m_faces[k].v2) || (m_faces[i].v3 == m_faces[k].v3)) &&
							((m_faces[i].v1 == m_faces[k].v1) || (m_faces[i].v1 == m_faces[k].v2) || (m_faces[i].v1 == m_faces[k].v3))   )
						{
							// v3-v1 와 중첩
							m_faces[i].f3 = &m_faces[k];
							f3 = FALSE;
						}
						if( !f1 && !f2 && !f3 ) 
							break;
					}
				}
			}


			Rendering();
		}
	}
}

void CViewingSystemDoc::Rendering()
{
	// 배경을 하얀색으로 칠한다
	memdc.FillRect(win, &CBrush(RGB(255, 255, 255)));

	// MFC는 왼쪽 위가 (0, 0)점이므로, 화면 중앙을 (0, 0)으로 만들려면 가운데 점이 필요하다
//	CPoint center = CPoint(win.Width()/2, win.Height()/2);

	// U, V, N 계산
	Vertex U, V, N, L;
	
	N = m_lookAt - m_eye;
	N = Normalization(N);
	V = m_up - (N * DotProduct(m_up, N));
	V = Normalization(V);
	U = CrossProduct(N, V);
	U = Normalization(U);

	Matrix T, R, P, View;
	T.m[3] = -m_eye.x;
	T.m[7] = -m_eye.y;
	T.m[11] = -m_eye.z;

	R.m[0] = U.x;	R.m[1] = U.y;	R.m[2] = U.z;
	R.m[4] = V.x;	R.m[5] = V.y;	R.m[6] = V.z;
	R.m[8] = N.x;	R.m[9] = N.y;	R.m[10] = N.z;

	// Projection Matrix
	if (m_viewingMode == PARALLEL)
	{
		View = R * T;
	}
	else if (m_viewingMode == PERSPECTIVE)
	{
		double zprp = 0.0;
		double zvp = 400.0;
		double dp = zprp - zvp;

		P.m[10] = -zvp/dp;
		P.m[11] = zvp*zvp/dp;
		P.m[14] = -1/dp;
		P.m[15] = zvp/dp;

		View = P * R * T;
	}

	if (m_shadingMode == WIREFRAME)
	{
		WireframeShading( View );
	}
	else if (m_shadingMode == FLAT)
	{
		FlatShading(View);
	}
	else if (m_shadingMode == GOURAUD)
	{
		GouraudShading(View);		
	}
	else if (m_shadingMode == PHONG)
	{
		PhongShading(View);
	}
	else if  (m_shadingMode == CARTOON || m_shadingMode == CARTOON2 )
	{
		CartoonRendering(View);
//		WireframeShading( View );
	}

	CString str1, str2, str3;
	str1.Format("Eye x : %d", int(m_eye.x));
	str2.Format("Eye y : %d", int(m_eye.y));
	str3.Format("Eye z : %d", int(m_eye.z));

	memdc.TextOut(10, 10, str1);
	memdc.TextOut(10, 30, str2);
	memdc.TextOut(10, 50, str3);

}


void CViewingSystemDoc::WireframeShading(Matrix View)
{
	CPoint center = CPoint(win.Width()/2, win.Height()/2);
	Vertex vertex1, vertex2, vertex3;
	Vertex a, b, NormalVector, ViewNormalVector, CullingNormalVector, faceCenterPoint;
	CPoint DrawPoint[4];

	for (int i = 0; i < m_numberOfFaces; i++)
	{
		vertex1 = View * m_vertices[m_faces[i].v1];
		vertex2 = View * m_vertices[m_faces[i].v2];
		vertex3 = View * m_vertices[m_faces[i].v3];

		// 2D 좌표 계산
		vertex1.x /= vertex1.h;		vertex1.y /= vertex1.h;		vertex1.z /= vertex1.h;
		vertex2.x /= vertex2.h;		vertex2.y /= vertex2.h;		vertex2.z /= vertex2.h;
		vertex3.x /= vertex3.h;		vertex3.y /= vertex3.h;		vertex3.z /= vertex3.h;

		//////////////////////////////////////////////////////////////////////////
		// Back face Culling
		a = vertex3 - vertex1;
		b = vertex2 - vertex1;

		CullingNormalVector = b.CrossProduct(a);
		CullingNormalVector.Normalization();

		faceCenterPoint = (m_vertices[m_faces[i].v1] + m_vertices[m_faces[i].v2] + m_vertices[m_faces[i].v3])/3;
		Vertex ViewVector = m_eye - faceCenterPoint;
		ViewVector.Normalization();
		ViewNormalVector = Vertex(0,0,500) - faceCenterPoint;
		ViewNormalVector.Normalization();

		if (CullingNormalVector.DotProduct(ViewNormalVector) < 0)
			continue;

		//////////////////////////////////////////////////////////////////////////
		DrawPoint[0] = CPoint(int(vertex1.x + center.x), int(-vertex1.y + center.y));
		DrawPoint[1] = CPoint(int(vertex2.x + center.x), int(-vertex2.y + center.y));
		DrawPoint[2] = CPoint(int(vertex3.x + center.x), int(-vertex3.y + center.y));
		DrawPoint[3] = CPoint(int(vertex1.x + center.x), int(-vertex1.y + center.y));

		memdc.Polyline(DrawPoint, 4);
	}

}


void CViewingSystemDoc::FlatShading(Matrix view)
{
	// 계산에 필요한 변수 및 상수들
	CPoint center = CPoint(win.Width()/2, win.Height()/2);

	Vertex a, b, c, d, HalfwayVector, ViewNormalVector, CullingNormalVector;
	double Ambient, Diffuse, Specular;
	double Ka = 0.1f;
	double Kd = 0.6f;
	double Ks = 0.4f;
	double square = 50;
	Vertex vertex1, vertex2, vertex3;

	//////////////////////////////////////////////////////////////////////////
	for (int i = 0; i < m_numberOfFaces; i++)
	{
		vertex1 = view * m_vertices[m_faces[i].v1];
		vertex2 = view * m_vertices[m_faces[i].v2];
		vertex3 = view * m_vertices[m_faces[i].v3];

		// 2D 좌표 계산
		vertex1.x /= vertex1.h;		vertex1.y /= vertex1.h;		vertex1.z /= vertex1.h;
		vertex2.x /= vertex2.h;		vertex2.y /= vertex2.h;		vertex2.z /= vertex2.h;
		vertex3.x /= vertex3.h;		vertex3.y /= vertex3.h;		vertex3.z /= vertex3.h;

		//////////////////////////////////////////////////////////////////////////
		// Back face Culling
		a = vertex3 - vertex1;
		b = vertex2 - vertex1;

		CullingNormalVector = b.CrossProduct(a);
		CullingNormalVector.Normalization();

		Vertex faceCenterPoint = (m_vertices[m_faces[i].v1] + m_vertices[m_faces[i].v2] + m_vertices[m_faces[i].v3])/3;
		Vertex ViewVector = m_eye - faceCenterPoint;
		ViewVector.Normalization();
		ViewNormalVector = Vertex(0,0,500) - faceCenterPoint; // m_eye - faceCenterPoint;
		ViewNormalVector.Normalization();
		if( CullingNormalVector.DotProduct(ViewNormalVector) <= 0 )
			continue;

		//////////////////////////////////////////////////////////////////////////
		// 트라이앵글 색 주사 방식
		//		 v3
		//		 -
		//		---
		//	  ------
		//	v1------v2	

		Vertex v1 = Vertex( (vertex1.x + center.x), (-vertex1.y + center.y), 0 );
		Vertex v2 = Vertex( (vertex2.x + center.x), (-vertex2.y + center.y), 0 );
		Vertex v3 = Vertex( (vertex3.x + center.x), (-vertex3.y + center.y), 0 );

		Vertex a1 = v3 - v1;
		Vertex a2 = v3 - v2;
		double len1 = a1.Length();
		double len2 = a2.Length();
		double add = 1 / (0.5f*(len1+len2)+10.f);
		if( 0 >= len1 || 0 >= len2 ) continue;
		a1.Normalization();
		a2.Normalization();

		// normal vector
		Vertex NormalVector = m_normalface[ 0][ i];
		Vertex LightVector = m_light - faceCenterPoint;
		LightVector.Normalization();
		Vertex ReflectVector = (NormalVector*2) - LightVector;
		ReflectVector.Normalization();

		Diffuse = Kd * NormalVector.DotProduct(LightVector) * 255.f;
		double sp = ReflectVector.DotProduct( ViewVector );
		Specular = Ks * pow(max(sp,0), square) * 255.f;
		Ambient = Ka * 255.f;
		int Illumin = (int)(Ambient + max(Diffuse,0) + max(Specular,0)); 
		COLORREF color = RGB( min(255,Illumin), 0, 0 );

		CPen pen;
		pen.CreatePen( PS_SOLID, 2, color );
		CPen *poldpen = memdc.SelectObject( &pen );
		for( double u=0; u <= 1.f; u += add )
		{
			Vertex p1 = v1 + (a1 * (len1*u));
			Vertex p2 = v2 + (a2 * (len2*u));
			memdc.MoveTo( (int)p1.x, (int)p1.y );
			memdc.LineTo( (int)p2.x, (int)p2.y );
		}
		memdc.SelectObject( poldpen );
		pen.DeleteObject();
	}
}


void CViewingSystemDoc::GouraudShading(Matrix view)
{
	// 계산에 필요한 변수 및 상수들
	CPoint center = CPoint(win.Width()/2, win.Height()/2);

	Vertex a, b, c, d, HalfwayVector, ViewNormalVector, CullingNormalVector;
	double Ambient, Diffuse, Specular;
	double Ka = 0.1f;
	double Kd = 0.6f;
	double Ks = 0.4f;
	double square = 50;
	Vertex vertex1, vertex2, vertex3;

	//////////////////////////////////////////////////////////////////////////
	for (int i = 0; i < m_numberOfFaces; i++)
	{
		vertex1 = view * m_vertices[m_faces[i].v1];
		vertex2 = view * m_vertices[m_faces[i].v2];
		vertex3 = view * m_vertices[m_faces[i].v3];

		// 2D 좌표 계산
		vertex1.x /= vertex1.h;		vertex1.y /= vertex1.h;		vertex1.z /= vertex1.h;
		vertex2.x /= vertex2.h;		vertex2.y /= vertex2.h;		vertex2.z /= vertex2.h;
		vertex3.x /= vertex3.h;		vertex3.y /= vertex3.h;		vertex3.z /= vertex3.h;

		//////////////////////////////////////////////////////////////////////////
		// Back face Culling
		a = vertex3 - vertex1;
		b = vertex2 - vertex1;

		CullingNormalVector = b.CrossProduct(a);
		CullingNormalVector.Normalization();

		Vertex faceCenterPoint = (m_vertices[m_faces[i].v1] + m_vertices[m_faces[i].v2] + m_vertices[m_faces[i].v3])/3;
		Vertex ViewVector = m_eye - faceCenterPoint;
		ViewVector.Normalization();
		ViewNormalVector = Vertex(0,0,500) - faceCenterPoint; // m_eye - faceCenterPoint;
		ViewNormalVector.Normalization();
		if( CullingNormalVector.DotProduct(ViewNormalVector) <= 0 )
			continue;

		//////////////////////////////////////////////////////////////////////////
		// 트라이앵글 색 주사 방식
		//		 v3
		//		 -
		//		---
		//	  ------
		//	v1------v2	

		Vertex v1 = Vertex( (vertex1.x + center.x), (-vertex1.y + center.y), 0 );
		Vertex v2 = Vertex( (vertex2.x + center.x), (-vertex2.y + center.y), 0 );
		Vertex v3 = Vertex( (vertex3.x + center.x), (-vertex3.y + center.y), 0 );

		Vertex a1 = v3 - v1;
		Vertex a2 = v3 - v2;
		double len1 = a1.Length();
		double len2 = a2.Length();
		double add = 1 / (0.5f*(len1+len2)+10.f);
		if( 0 >= len1 || 0 >= len2 ) continue;
//		a1.Normalization();
//		a2.Normalization();

		COLORREF color1, color2, color3;

		// v1 vertex color
		{
			Vertex NormalVector = m_normals[ m_faces[i].v1];
			Vertex LightVector = m_light - m_vertices[ m_faces[i].v1];
			LightVector.Normalization();
			Vertex ReflectVector = (NormalVector*2) - LightVector;
			ReflectVector.Normalization();

			Diffuse = Kd * NormalVector.DotProduct(LightVector) * 255.f;
			double sp = ReflectVector.DotProduct( ViewVector );
			Specular = Ks * pow(max(sp,0), square) * 255.f;
			Ambient = Ka * 255.f;
			int Illumin = (int)(Ambient + max(Diffuse,0) + max(Specular,0)); 
			color1 = RGB( min(255,Illumin), 0, 0 );
		}

		// v2 vertex color
		{
			Vertex NormalVector = m_normals[ m_faces[i].v2];
			Vertex LightVector = m_light - m_vertices[ m_faces[i].v2];
			LightVector.Normalization();
			Vertex ReflectVector = (NormalVector*2) - LightVector;
			ReflectVector.Normalization();

			Diffuse = Kd * NormalVector.DotProduct(LightVector) * 255.f;
			double sp = ReflectVector.DotProduct( ViewVector );
			Specular = Ks * pow(max(sp,0), square) * 255.f;
			Ambient = Ka * 255.f;
			int Illumin = (int)(Ambient + max(Diffuse,0) + max(Specular,0)); 
			color2 = RGB( min(255,Illumin), 0, 0 );
		}

		// v3 vertex color
		{
			Vertex NormalVector = m_normals[ m_faces[i].v3];
			Vertex LightVector = m_light - m_vertices[ m_faces[i].v3];
			LightVector.Normalization();
			Vertex ReflectVector = (NormalVector*2) - LightVector;
			ReflectVector.Normalization();

			Diffuse = Kd * NormalVector.DotProduct(LightVector) * 255.f;
			double sp = ReflectVector.DotProduct( ViewVector );
			Specular = Ks * pow(max(sp,0), square) * 255.f;
			Ambient = Ka * 255.f;
			int Illumin = (int)(Ambient + max(Diffuse,0) + max(Specular,0)); 
			color3 = RGB( min(255,Illumin), 0, 0 );
		}

		Color c1( color1 );
		Color c2( color2 );
		Color c3( color3 );
		int oldx1=-1, oldy1=-1, oldx2=-1, oldy2=-1;
		double u = 0.f;
		while( 1 )
		{
			Vertex p1 = v1*(1.f-u) + v3*u;
			Vertex p2 = v2*(1.f-u) + v3*u;
			Color cc1 = c1*(1.f-u) + c3*u;
			Color cc2 = c2*(1.f-u) + c3*u;

			int x1 = (int)p1.x;
			int y1 = (int)p1.y;
			int x2 = (int)p2.x;
			int y2 = (int)p2.y;
			if( oldx1 != x1 || oldy1 != y1 || oldx2 != x2 || oldy2 != y2 ) 
			{
				LineToGouraud( x1, y1, cc1.GetCOLORREF(), x2, y2, cc2.GetCOLORREF() );
			}

			oldx1 = x1;
			oldy1 = y1;
			oldx2 = x2;
			oldy2 = y2;

			if( 1.f <= u ) break;
			u += add;
			if( 1.f < u )
				u = 1.f;
		}
	}

}

void CViewingSystemDoc::PhongShading(Matrix view)
{
	// 계산에 필요한 변수 및 상수들
	CPoint center = CPoint(win.Width()/2, win.Height()/2);

	Vertex a, b, c, d, HalfwayVector, ViewNormalVector, CullingNormalVector;
	double Ka = 0.1f;
	double Kd = 0.6f;
	double Ks = 0.4f;
	double square = 50;
	Vertex vertex1, vertex2, vertex3;

	//////////////////////////////////////////////////////////////////////////
	for (int i = 0; i < m_numberOfFaces; i++)
	{
		vertex1 = view * m_vertices[m_faces[i].v1];
		vertex2 = view * m_vertices[m_faces[i].v2];
		vertex3 = view * m_vertices[m_faces[i].v3];

		// 2D 좌표 계산
		vertex1.x /= vertex1.h;		vertex1.y /= vertex1.h;		vertex1.z /= vertex1.h;
		vertex2.x /= vertex2.h;		vertex2.y /= vertex2.h;		vertex2.z /= vertex2.h;
		vertex3.x /= vertex3.h;		vertex3.y /= vertex3.h;		vertex3.z /= vertex3.h;

		//////////////////////////////////////////////////////////////////////////
		// Back face Culling
		a = vertex3 - vertex1;
		b = vertex2 - vertex1;

		CullingNormalVector = b.CrossProduct(a);
		CullingNormalVector.Normalization();

		Vertex faceCenterPoint = (m_vertices[m_faces[i].v1] + m_vertices[m_faces[i].v2] + m_vertices[m_faces[i].v3])/3;
		Vertex ViewVector = m_eye - faceCenterPoint;
		ViewVector.Normalization();
		ViewNormalVector = Vertex(0,0,500) - faceCenterPoint;
		ViewNormalVector.Normalization();
		if( CullingNormalVector.DotProduct(ViewNormalVector) <= 0 )
			continue;

		//////////////////////////////////////////////////////////////////////////
		// 트라이앵글 색 주사 방식
		//		 v3
		//		 -
		//		---
		//	  ------
		//	v1------v2	

		Vertex v1 = Vertex( (vertex1.x + center.x), (-vertex1.y + center.y), 0 );
		Vertex v2 = Vertex( (vertex2.x + center.x), (-vertex2.y + center.y), 0 );
		Vertex v3 = Vertex( (vertex3.x + center.x), (-vertex3.y + center.y), 0 );

		Vertex a1 = v3 - v1;
		Vertex a2 = v3 - v2;
		double len1 = a1.Length();
		double len2 = a2.Length();
		double add = 1 / (0.5f*(len1+len2)+10.f);
		if( 0 >= len1 || 0 >= len2 ) continue;
		Vertex n1 = m_normals[ m_faces[i].v1];
		Vertex n2 = m_normals[ m_faces[i].v2];
		Vertex n3 = m_normals[ m_faces[i].v3];

		int oldx1=-1, oldy1=-1, oldx2=-1, oldy2=-1;
		double u = 0.f;
		while( 1 )
		{
			Vertex p1 = v1*(1.f-u) + v3*u;
			Vertex p2 = v2*(1.f-u) + v3*u;
			Vertex pp1 = m_vertices[ m_faces[i].v1]*(1.f-u) + m_vertices[ m_faces[i].v3]*u;
			Vertex pp2 = m_vertices[ m_faces[i].v2]*(1.f-u) + m_vertices[ m_faces[i].v3]*u;
			Vertex nn1 = n1*(1.f-u) + n3*u;
			Vertex nn2 = n2*(1.f-u) + n3*u;

			int x1 = (int)p1.x;
			int y1 = (int)p1.y;
			int x2 = (int)p2.x;
			int y2 = (int)p2.y;
			if( oldx1 != x1 || oldy1 != y1 || oldx2 != x2 || oldy2 != y2 ) 
			{
				LineToPhong( &ViewVector, x1, y1, &pp1, &nn1, x2, y2, &pp2, &nn2 );
			}

			oldx1 = x1;
			oldy1 = y1;
			oldx2 = x2;
			oldy2 = y2;

			if( 1.f <= u ) break;
			u += add;
			if( 1.f < u )
				u = 1.f;
		}
	}
  
}


void CViewingSystemDoc::CartoonRendering(Matrix view)
{
	// 계산에 필요한 변수 및 상수들
	CPoint center = CPoint(win.Width()/2, win.Height()/2);

	Vertex a, b, c, d, HalfwayVector, ViewNormalVector, CullingNormalVector;
	double Ka = 0.1f;
	double Kd = 0.6f;
	double Ks = 0.4f;
	double square = 50;
	Vertex vertex1, vertex2, vertex3;
	int i;
	int w = win.Width();
	int h = win.Height();

	//////////////////////////////////////////////////////////////////////////
	for(i = 0; i < m_numberOfFaces; i++)
	{
		vertex1 = view * m_vertices[m_faces[i].v1];
		vertex2 = view * m_vertices[m_faces[i].v2];
		vertex3 = view * m_vertices[m_faces[i].v3];

		// 2D 좌표 계산
		vertex1.x /= vertex1.h;		vertex1.y /= vertex1.h;		vertex1.z /= vertex1.h;
		vertex2.x /= vertex2.h;		vertex2.y /= vertex2.h;		vertex2.z /= vertex2.h;
		vertex3.x /= vertex3.h;		vertex3.y /= vertex3.h;		vertex3.z /= vertex3.h;

		//////////////////////////////////////////////////////////////////////////
		// Back face Culling
		a = vertex3 - vertex1;
		b = vertex2 - vertex1;

		CullingNormalVector = b.CrossProduct(a);
		CullingNormalVector.Normalization();

		Vertex faceCenterPoint = (m_vertices[m_faces[i].v1] + m_vertices[m_faces[i].v2] + m_vertices[m_faces[i].v3])/3;
		Vertex ViewVector = m_eye - faceCenterPoint;
		ViewVector.Normalization();
		ViewNormalVector = Vertex(0,0,500) - faceCenterPoint;
		ViewNormalVector.Normalization();
		if( CullingNormalVector.DotProduct(ViewNormalVector) <= 0 )
		{
			m_faces[ i].visible = FALSE;
			continue;
		}
		else
		{
			m_faces[ i].visible = TRUE;
		}

		//////////////////////////////////////////////////////////////////////////
		//		 v3
		//		 -
		//		---
		//	  ------
		//	v1------v2	
		Vertex v1 = Vertex( (vertex1.x + center.x), (-vertex1.y + center.y), 0 );
		Vertex v2 = Vertex( (vertex2.x + center.x), (-vertex2.y + center.y), 0 );
		Vertex v3 = Vertex( (vertex3.x + center.x), (-vertex3.y + center.y), 0 );

		Vertex a1 = v3 - v1;
		Vertex a2 = v3 - v2;
		double len1 = a1.Length();
		double len2 = a2.Length();
		double add = 1 / (0.5f*(len1+len2)+10.f);
		if( 0 >= len1 || 0 >= len2 ) continue;
		Vertex n1 = m_normals[ m_faces[i].v1];
		Vertex n2 = m_normals[ m_faces[i].v2];
		Vertex n3 = m_normals[ m_faces[i].v3];

		int oldx1=-1, oldy1=-1, oldx2=-1, oldy2=-1;
		double u = 0.f;
		while( 1 )
		{
			Vertex p1 = v1*(1.f-u) + v3*u;
			Vertex p2 = v2*(1.f-u) + v3*u;
			Vertex pp1 = m_vertices[ m_faces[i].v1]*(1.f-u) + m_vertices[ m_faces[i].v3]*u;
			Vertex pp2 = m_vertices[ m_faces[i].v2]*(1.f-u) + m_vertices[ m_faces[i].v3]*u;
			Vertex nn1 = n1*(1.f-u) + n3*u;
			Vertex nn2 = n2*(1.f-u) + n3*u;

			int x1 = (int)p1.x;
			int y1 = (int)p1.y;
			int x2 = (int)p2.x;
			int y2 = (int)p2.y;
/*
			// cohen sutherland line clipping
			int cs_state1 = 0;
			int cs_state2 = 0;
			if( 0 > x1 ) cs_state1 |= 0x01;
			if( w < x1 ) cs_state1 |= 0x02;
			if( 0 > y1 ) cs_state1 |= 0x04;
			if( h < y1 ) cs_state1 |= 0x08;
			
			if( 0 > x2 ) cs_state2 |= 0x01;
			if( w < x2 ) cs_state2 |= 0x02;
			if( 0 > y2 ) cs_state2 |= 0x04;
			if( h < y2 ) cs_state2 |= 0x08;
/**/
			if( (oldx1 != x1 || oldy1 != y1 || oldx2 != x2 || oldy2 != y2) ) 
			{
				LineToCartoon( &ViewVector, x1, y1, &pp1, &nn1, x2, y2, &pp2, &nn2 );
			}

			oldx1 = x1;
			oldy1 = y1;
			oldx2 = x2;
			oldy2 = y2;

			if( 1.f <= u ) break;
			u += add;
			if( 1.f < u )
				u = 1.f;
		}

		// 외곽선 그리기
		if( m_shadingMode == CARTOON )
		{
			CPen pen;
			pen.CreatePen( PS_SOLID, 2, RGB(0,0,0) );
			CPen *poldpen = memdc.SelectObject( &pen );
			const double LIMIT = 0.1f;

			int l1 = m_faces[ i].l1;
			Vertex ViewVector1 = m_eye - (m_vertices[ m_lines[ l1].v1] + m_vertices[ m_lines[ l1].v2]) * .5f;
			ViewVector1.Normalization();
			double scalar1 = ViewVector1.DotProduct( m_lines[ l1].norm );
			if( LIMIT > scalar1 && -LIMIT < scalar1 )
			{
				memdc.MoveTo( (int)v1.x, (int)v1.y );
				memdc.LineTo( (int)v2.x, (int)v2.y );
			}

			int l2 = m_faces[ i].l2;
			Vertex ViewVector2 = m_eye - (m_vertices[ m_lines[ l2].v1] + m_vertices[ m_lines[ l2].v2]) * .5f;
			ViewVector2.Normalization();
			double scalar2 = ViewVector2.DotProduct( m_lines[ l2].norm );
			if( LIMIT > scalar2 && -LIMIT < scalar2 )
			{
				memdc.MoveTo( (int)v2.x, (int)v2.y );
				memdc.LineTo( (int)v3.x, (int)v3.y );
			}

			int l3 = m_faces[ i].l3;
			Vertex ViewVector3 = m_eye - (m_vertices[ m_lines[ l3].v1] + m_vertices[ m_lines[ l3].v2]) * .5f;
			ViewVector3.Normalization();
			double scalar3 = ViewVector3.DotProduct( m_lines[ l3].norm );
			if( LIMIT > scalar3 && -LIMIT < scalar3 )
			{
				memdc.MoveTo( (int)v3.x, (int)v3.y );
				memdc.LineTo( (int)v1.x, (int)v1.y );
			}

			memdc.SelectObject( poldpen );
			pen.DeleteObject();
		}
	}

	if( m_shadingMode == CARTOON)
	{
		for(i = 0; i < m_numberOfFaces; i++)
		{
			if( !m_faces[ i].visible ) continue;

			if( !(m_faces[ i].f1 && !m_faces[ i].f1->visible) &&
				!(m_faces[ i].f2 && !m_faces[ i].f2->visible) &&
				!(m_faces[ i].f3 && !m_faces[ i].f3->visible) )
				continue;

			vertex1 = view * m_vertices[m_faces[i].v1];
			vertex2 = view * m_vertices[m_faces[i].v2];
			vertex3 = view * m_vertices[m_faces[i].v3];

			// 2D 좌표 계산
			vertex1.x /= vertex1.h;		vertex1.y /= vertex1.h;		vertex1.z /= vertex1.h;
			vertex2.x /= vertex2.h;		vertex2.y /= vertex2.h;		vertex2.z /= vertex2.h;
			vertex3.x /= vertex3.h;		vertex3.y /= vertex3.h;		vertex3.z /= vertex3.h;

			Vertex v1 = Vertex( (vertex1.x + center.x), (-vertex1.y + center.y), 0 );
			Vertex v2 = Vertex( (vertex2.x + center.x), (-vertex2.y + center.y), 0 );
			Vertex v3 = Vertex( (vertex3.x + center.x), (-vertex3.y + center.y), 0 );

			// 외곽선 그리기
			CPen pen;
			pen.CreatePen( PS_SOLID, 3, RGB(0,0,0) );
			CPen *poldpen = memdc.SelectObject( &pen );

			if( m_faces[ i].f1 )
			{
				if( !m_faces[ i].f1->visible )
				{
					memdc.MoveTo( (int)v1.x, (int)v1.y );
					memdc.LineTo( (int)v2.x, (int)v2.y );
				}
			}
			if( m_faces[ i].f2 )
			{
				if( !m_faces[ i].f2->visible )
				{
					memdc.MoveTo( (int)v2.x, (int)v2.y );
					memdc.LineTo( (int)v3.x, (int)v3.y );
				}
			}
			if( m_faces[ i].f3 )
			{
				if( !m_faces[ i].f3->visible )
				{
					memdc.MoveTo( (int)v3.x, (int)v3.y );
					memdc.LineTo( (int)v1.x, (int)v1.y );
				}
			}
			memdc.SelectObject( poldpen );
			pen.DeleteObject();
		}
	}
	else
	{
		int x,y;
		BOOL toggle = TRUE;
		COLORREF bgcolor = RGB(255,255,255);
		COLORREF edgecolor = RGB(0,0,0); 
		for( x=0; x < w; x++ )
		{
			for( y=0; y < h; ++y )
			{
				COLORREF color = memdc.GetPixel( x, y );
				if( (toggle && (color != bgcolor)) || (!toggle && (color == bgcolor)) )
				{
					memdc.SetPixel( x, y,   edgecolor );
					memdc.SetPixel( x, y-1, edgecolor );
					memdc.SetPixel( x, y-2, edgecolor );
					toggle = !toggle;
				}
			}
		}

		for( y=0; y < h; ++y )
		{
			for( x=0; x < w; ++x )
			{
				COLORREF color = memdc.GetPixel( x, y );
				if( (toggle && (color != bgcolor)) || (!toggle && (color == bgcolor)) )
				{
					memdc.SetPixel( x,   y, edgecolor );
					memdc.SetPixel( x-1, y, edgecolor );
					memdc.SetPixel( x-2, y, edgecolor );
					toggle = !toggle;
				}
			}
		}
	}
}


Vertex CViewingSystemDoc::Normalization(Vertex vertex)
{
	double dis = sqrt(pow(vertex.x, 2.0) + pow(vertex.y, 2.0) + pow(vertex.z, 2.0));

	return Vertex(vertex.x/dis, vertex.y/dis, vertex.z/dis);
}

double CViewingSystemDoc::DotProduct(Vertex a, Vertex b)
{
	return a.x * b.x + a.y * b.y + a.z * b.z;
}

Vertex CViewingSystemDoc::CrossProduct(Vertex a, Vertex b)
{
	return Vertex(a.y*b.z - a.z*b.y, a.z*b.x - a.x*b.z, a.x*b.y - a.y*b.x);
}

// testPoint가 삼각형 내부에 존재하는지 테스트 하는 함수
bool CViewingSystemDoc::TriangleInsideTest(CPoint triangle[], CPoint testPoint)
{
	if ( ( (CrossProduct(triangle[1]-triangle[0], testPoint - triangle[0]) >= 0)
		&& (CrossProduct(triangle[2]-triangle[1], testPoint - triangle[1]) >= 0)
		&& (CrossProduct(triangle[0]-triangle[2], testPoint  - triangle[2]) >= 0)) ||
		((CrossProduct(triangle[1]-triangle[0], testPoint - triangle[0]) <= 0)
		&& (CrossProduct(triangle[2]-triangle[1], testPoint - triangle[1]) <= 0)
		&& (CrossProduct(triangle[0]-triangle[2], testPoint  - triangle[2]) <= 0) ) )
		return true;
	else
		return false;
}

int CViewingSystemDoc::CrossProduct(CPoint a, CPoint b)
{
	return a.x * b.y - a.y * b.x;
}


//-----------------------------------------------------------------------------//
// x1,y1에서 x2,y2까지 color1에서color2로 보간해가며 선을 긋는다.
//-----------------------------------------------------------------------------//
void CViewingSystemDoc::LineToGouraud( int x1, int y1, COLORREF color1, int x2, int y2, COLORREF color2 )
{
	Vertex v1(x1,y1,0);
	Vertex v2(x2,y2,0);
	Vertex v = v2 - v1;
	double len = v.Length();
	v.Normalization();
	double add = 1 / (len+10.f);
	if( 1 >= len ) return;

	Color c1( color1 );
	Color c2( color2 );

	int oldx = -1, oldy = -1;
	double u = 0.f;
	while( 1 )
	{
		Vertex pos  = v1*(1.f-u) + v2*u;
		Color color = c1*(1.f-u) + c2*u;
		
		int x = (int)pos.x;
		int y = (int)pos.y;
		COLORREF cc = color.GetCOLORREF();

		if( oldx != x || oldy != y )
		{
			memdc.SetPixel( x-1, y, cc );
			memdc.SetPixel( x,   y, cc );
			memdc.SetPixel( x+1, y, cc );
		}

		oldx = x;
		oldy = y;

		if( 1.f <= u ) break;
		u += add;
		if( 1.f < u )
			u = 1.f;
	}
}


//-----------------------------------------------------------------------------//
// x1,y1에서 x2,y2까지 normal1에서 normal2로 보간해가며 선을 긋는다.
//-----------------------------------------------------------------------------//
void CViewingSystemDoc::LineToPhong( Vertex *pViewVector, int x1, int y1, Vertex *pvs, Vertex *pnorm1, int x2, int y2, Vertex *pvt, Vertex *pnorm2 )
{
	double Ambient, Diffuse, Specular;
	double Ka = 0.1f;
	double Kd = 0.6f;
	double Ks = 0.4f;
	double square = 50;

	Vertex v1(x1,y1,0);
	Vertex v2(x2,y2,0);
	Vertex v = v2 - v1;
	double len = v.Length();
	v.Normalization();
	double add = 1 / (len+10.f);
	if( 1 >= len ) return;

	int oldx = -1, oldy = -1;
	double u = 0.f;
	while( 1 )
	{
		Vertex pos  = v1 + (v * (len*u));
		int x = (int)pos.x;
		int y = (int)pos.y;
		Vertex n = (*pnorm1)*(1.f-u) + (*pnorm2)*u;
		Vertex p = (*pvs)*(1.f-u) + (*pvt)*u;

		Vertex LightVector = m_light - p;
		LightVector.Normalization();
		Vertex ReflectVector = (n*2) - LightVector;
		ReflectVector.Normalization();

		Diffuse = Kd * n.DotProduct(LightVector) * 255.f;
		double sp = ReflectVector.DotProduct( *pViewVector );
		Specular = Ks * pow(max(sp,0), square) * 255.f;
		Ambient = Ka * 255.f;
		int Illumin = (int)(Ambient + max(Diffuse,0) + max(Specular,0)); 
		COLORREF color = RGB( min(255,Illumin), 0, 0 );

		if( oldx != x || oldy != y )
		{
			memdc.SetPixel( x-1, y, color );
			memdc.SetPixel( x,   y, color );
			memdc.SetPixel( x+1, y, color );
		}

		oldx = x;
		oldy = y;

		if( 1.f <= u ) break;
		u += add;
		if( 1.f < u )
			u = 1.f;
	}
}


void CViewingSystemDoc::LineToCartoon( Vertex *pViewVector, int x1, int y1, Vertex *pvs, Vertex *pnorm1, int x2, int y2, Vertex *pvt, Vertex *pnorm2 )
{
	double Ambient, Diffuse, Specular;
	double Ka = 0.1f;
	double Kd = 0.6f;
	double Ks = 0.4f;
	double square = 50;

	Vertex v1(x1,y1,0);
	Vertex v2(x2,y2,0);
	Vertex v = v2 - v1;
	double len = v.Length();
	v.Normalization();
	double add = 1 / (len+10.f);
	if( 1 >= len ) return;
	int w = win.Width();
	int h = win.Height();

	int oldx = -1, oldy = -1;
	double u = 0.f;
	while( 1 )
	{
		Vertex pos  = v1 + (v * (len*u));
		int x = (int)pos.x;
		int y = (int)pos.y;
		Vertex n = (*pnorm1)*(1.f-u) + (*pnorm2)*u;
		Vertex p = (*pvs)*(1.f-u) + (*pvt)*u;

		if( (y >= 0 && y < h) && (x >= 0 && x < w) )
		{
			Vertex LightVector = m_light - p;
			LightVector.Normalization();
			Vertex ReflectVector = (n*2) - LightVector;
			ReflectVector.Normalization();

			Diffuse = Kd * n.DotProduct(LightVector) * 255.f;
			double sp = ReflectVector.DotProduct( *pViewVector );
			Specular = Ks * pow(max(sp,0), square) * 255.f;
			Ambient = Ka * 255.f;
			int Illumin = (int)(Ambient + max(Diffuse,0) + max(Specular,0)); 
			COLORREF color;
			int r = min(255,Illumin);

			if( m_shadingMode == CARTOON)
			{
				if( r < 100 )		color = RGB(100+rand()%30,rand()%30,rand()%30);
				else if( r < 150 )	color = RGB(150+rand()%30,rand()%30,rand()%30);
				else if( r < 200 )	color = RGB(200+rand()%30,rand()%30,rand()%30);
				else				color = RGB(255,0,0);
			}
			else
			{
				if( r < 100 )		color = RGB(100,0,0);
				else if( r < 150 )	color = RGB(150,0,0);
				else if( r < 200 )	color = RGB(200,0,0);
				else				color = RGB(255,0,0);
			}

			if( (oldx != x || oldy != y) )
			{
				memdc.SetPixel( x-1, y, color );
				memdc.SetPixel( x,   y, color );
				memdc.SetPixel( x+1, y, color );
				memdc.SetPixel( x-1, y-1, color );
				memdc.SetPixel( x+1, y+1, color );
			}

			oldx = x;
			oldy = y;
		}

		if( 1.f <= u ) break;
		u += add;
		if( 1.f < u )
			u = 1.f;
	}
}