
#include "stdafx.h"
#include "Vector2D.h"
#include <fstream>


std::ostream& operator<<(std::ostream& os, const Vector2D& rhs)
{
  os << " " << rhs.x << " " << rhs.y;

  return os;
}

  
std::ifstream& operator>>(std::ifstream& is, Vector2D& lhs)
{
  is >> lhs.x >> lhs.y;

  return is;
}


Vector2D  Vector2D::Interpolation(const Vector2D &v2, const double val) const
{
	return Interpolation(*this, v2, val);
}

Vector2D  Vector2D::Interpolation(const Vector2D &v1, const Vector2D &v2, const double val) const
{
	Vector2D reval;
	reval.x = v2.x * val + v1.x * (1.f - val);
	reval.y = v2.y * val + v1.y * (1.f - val);
	return reval;
}


// 점과 직선사이 거리
double Vector2D::DistanceLineToPoint(const Vector2D &v1, const Vector2D &v2, const Vector2D &point) const
{
	Vector2D v12 = v2 - v1;
	Vector2D v13 = point - v1;
	Vector2D v23 = point - v2;

	const double len13 = v1.Distance(point);
	v12.Normalize();
	v13.Normalize();
	v23.Normalize();
	const double dot13 = v12.Dot(v13);
	const double dot23 = v13.Dot(v23);

	if (dot13 < 0.f || dot23 > 0.f)
	{
		return min(v1.Distance(point), v2.Distance(point));
	}

	Vector2D shortPos = v1 + (v12 * (dot13 * len13));
	return shortPos.Distance(point);	
}

