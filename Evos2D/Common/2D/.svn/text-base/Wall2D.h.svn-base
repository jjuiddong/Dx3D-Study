#ifndef WALL_H
#define WALL_H
//------------------------------------------------------------------------
//
//  Name:   Wall2D.h
//
//  Desc:   class to create and render 2D walls. Defined as the two 
//          vectors A - B with a perpendicular normal. 
//          
//
//  Author: Mat Buckland (fup@ai-junkie.com)
//
//------------------------------------------------------------------------
#include "../Common/misc/Cgdi.h"
#include "../Common/2d/Vector2D.h"
#include <fstream>


class Wall2D 
{
protected:

  Vector2D    m_vA,
              m_vB,
              m_vN;

  void CalculateNormal()
  {
    Vector2D temp = Vec2DNormalize(m_vB - m_vA);

    m_vN.x = -temp.y;
    m_vN.y = temp.x;
  }

public:

  Wall2D(){}

  Wall2D(Vector2D A, Vector2D B):m_vA(A), m_vB(B)
  {
    CalculateNormal();
  }

  Wall2D(Vector2D A, Vector2D B, Vector2D N):m_vA(A), m_vB(B), m_vN(N)
  { }

  Wall2D(std::ifstream& in){Read(in);}

  virtual void Render(bool RenderNormals = false)const
  {
    gdi->Line(m_vA, m_vB);

    //render the normals if rqd
    if (RenderNormals)
    {
      int MidX = (int)((m_vA.x+m_vB.x)/2);
      int MidY = (int)((m_vA.y+m_vB.y)/2);

      gdi->Line(MidX, MidY, (int)(MidX+(m_vN.x * 5)), (int)(MidY+(m_vN.y * 5)));
    }
  }

  Vector2D From()const  {return m_vA;}
  void     SetFrom(Vector2D v){m_vA = v; CalculateNormal();}

  Vector2D To()const    {return m_vB;}
  void     SetTo(Vector2D v){m_vB = v; CalculateNormal();}
  
  Vector2D Normal()const{return m_vN;}
  void     SetNormal(Vector2D n){m_vN = n;}
  
  Vector2D Center()const{return (m_vA+m_vB)/2.0;}

  double   Distance( const Vector2D &point ) { return point.DistanceLineToPoint(this->m_vA, this->m_vB, point); }

  bool IsCross(const Vector2D &A1, const Vector2D &A2)
  {
	  Vector2D B1 = From();
	  Vector2D B2 = To();

	  double under = (B2.y-B1.y)*(A2.x-A1.x)-(B2.x-B1.x)*(A2.y-A1.y);
	  if(under==0) 
	  {
		  return FALSE;
	  }

	  double _t = (B2.x-B1.x)*(A1.y-B1.y) - (B2.y-B1.y)*(A1.x-B1.x);
	  double _s = (A2.x-A1.x)*(A1.y-B1.y) - (A2.y-A1.y)*(A1.x-B1.x);
	  double t = _t/under;
	  double s = _s/under;

	  if(t<0.0 || t>1.0 || s<0.0 || s>1.0) 
	  { 
		  return FALSE;
	  }
	  if(_t==0 && _s==0)
	  { 
		  return FALSE;
	  }

	  return TRUE;
  }

  std::ostream& Wall2D::Write(std::ostream& os)const
  {
    os << std::endl;
    os << From() << ",";
    os << To() << ",";
    os << Normal();
    return os;
  }

 void Read(std::ifstream& in)
  {
    double x,y;
	char comma;

    in >> x >> y;
	in >> comma;
    SetFrom(Vector2D(x,y));

	in >> x >> y;
	in >> comma;
    SetTo(Vector2D(x,y));

	in >> x >> y;
    SetNormal(Vector2D(x,y));
  }
  
};

#endif