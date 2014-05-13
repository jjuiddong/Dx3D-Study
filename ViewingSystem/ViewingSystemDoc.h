// ViewingSystemDoc.h : interface of the CViewingSystemDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_VIEWINGSYSTEMDOC_H__C9D36D04_6DB8_4D69_8C1A_1DD2C175A75B__INCLUDED_)
#define AFX_VIEWINGSYSTEMDOC_H__C9D36D04_6DB8_4D69_8C1A_1DD2C175A75B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#pragma warning(disable: 4786)
#include "Matrix.h"
#include <map>

#define		PARALLEL	0
#define		PERSPECTIVE	1

#define		WIREFRAME	10
#define		FLAT		11
#define		GOURAUD		12
#define		PHONG		13
#define		CARTOON		20
#define		CARTOON2	21

class CViewingSystemDoc : public CDocument
{
protected: // create from serialization only
	CViewingSystemDoc();
	DECLARE_DYNCREATE(CViewingSystemDoc)

// Attributes
public:
	// Viewing System
	Vertex *m_vertices;
	Vertex *m_normals;			// vertex normal (vertex 개수만큼 존재한다.)
	Vertex *m_normalface[ 3];	// vertex normal (faces*3 개수만큼 존재한다. m_normalface[ 0,1,2][ faceidx])
	Face *m_faces;
	Line *m_lines;				// vertex끼리 연결된 라인을 나타낸다.
	int m_linecnt;				// line수를 저장한다.

	std::map<int,int> m_LineMap;	// key = v1+v2 index, 
									// data = line array index
	typedef std::map<int,int>::iterator LineItor;

	int m_numberOfVertices;
	int m_numberOfFaces;

	Vertex m_eye;
	Vertex m_lookAt;
	Vertex m_up;

	CRect win;
	CDC memdc;

	int m_viewingMode;

	// Shading
	int m_shadingMode;
	//int *zBuffer;

	Vertex m_light;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CViewingSystemDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	//}}AFX_VIRTUAL

// Implementation
public:
	void CartoonRendering(Matrix view);
	void PhongShading(Matrix view);
	void GouraudShading(Matrix view);
	void FlatShading(Matrix view);
	void WireframeShading(Matrix view);
	int CrossProduct(CPoint a, CPoint b);
	bool TriangleInsideTest(CPoint triangle[], CPoint testPoint);
	Vertex CrossProduct(Vertex a, Vertex b);
	double DotProduct(Vertex a, Vertex b);
	Vertex Normalization(Vertex vertex);
	void Rendering();
	void FileOpen(CString filename);

	void LineToGouraud( int x1, int y1, COLORREF color1, int x2, int y2, COLORREF color2 );
	void LineToPhong( Vertex *pViewVector, int x1, int y1, Vertex *pvs, Vertex *pnorm1, int x2, int y2, Vertex *pvt, Vertex *pnorm2 );
	void LineToCartoon( Vertex *pViewVector, int x1, int y1, Vertex *pvs, Vertex *pnorm1, int x2, int y2, Vertex *pvt, Vertex *pnorm2 );
	
	virtual ~CViewingSystemDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CViewingSystemDoc)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VIEWINGSYSTEMDOC_H__C9D36D04_6DB8_4D69_8C1A_1DD2C175A75B__INCLUDED_)
