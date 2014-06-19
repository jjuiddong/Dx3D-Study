
#include "stdafx.h"
#include "block.h"

using namespace std;


CBlock::~CBlock()
{
	Clear();

}


BOOL CBlock::AddWall( Wall2D *pWall ) 
{
	if (pWall == NULL) return FALSE;

	m_WallList.push_back(pWall);
	return TRUE;
}


BOOL CBlock::DelWall( int index )
{
	list<Wall2D*>::iterator itor = GetWallItor(index);
	if (itor == m_WallList.end()) return FALSE;

	Wall2D *p = *itor;
	delete p;

	m_WallList.erase(itor);
	return TRUE;
}


Wall2D* CBlock::GetWall( int index )
{
	list<Wall2D*>::iterator itor = GetWallItor(index);
	if (itor == m_WallList.end()) return NULL;

	return *itor;
}


Wall2D* CBlock::FindWall( Vector2D *pPoint )
{

	return NULL;
}


void CBlock::Clear()
{
	list<Wall2D*>::iterator itor = m_WallList.begin();
	while (itor != m_WallList.end())
	{
		Wall2D *p = *itor++;
		delete p;
	}
	m_WallList.clear();
}


list<Wall2D*>::iterator CBlock::GetWallItor( int index )
{
	int i = 0;
	list<Wall2D*>::iterator itor = m_WallList.begin();
	while (itor != m_WallList.end() && i <= index)
	{
		if (i == index)
		{
			return itor;
		}
		++i;
		++itor;
	}
	return m_WallList.end();
}


void CBlock::Render()
{
	list<Wall2D*>::iterator itor = m_WallList.begin();
	while (itor != m_WallList.end())
	{
		(*itor)->Render(true);
		++itor;
	}
}


void CBlock::Update(float timeDelta)
{

}


Wall2D* CBlock::GetBeginWall()
{
	return GetWall(0);
}


Wall2D* CBlock::GetEndWall()
{
	return GetWall((int)m_WallList.size()-1);
}


// Wall2D의 노말벡터가 블럭의 바깥을 바라보게 한다
void CBlock::CalculateNormal()
{
	// 중점을 구한다
	Vector2D center(0,0);
	list<Wall2D*>::iterator itor = m_WallList.begin();
	while (itor != m_WallList.end())
	{
		center += (*itor)->From();
		++itor;
	}

	center /= (float)m_WallList.size();

	// 중점과 노말벡터가 마주본다면 반전시킨다
	itor = m_WallList.begin();
	while (itor != m_WallList.end())
	{
		Vector2D line = (*itor)->From() + (*itor)->To();
		line /= 2.f;
		const Vector2D centerToLine = line - center;
		const double dot = centerToLine.Dot((*itor)->Normal());

		if (dot <= 0.f)
		{
			Vector2D to = (*itor)->To();
			(*itor)->SetTo((*itor)->From());
			(*itor)->SetFrom(to);
		}
		++itor;
	}

}


// pPoint 가 Block 안에 있다면 true 를 리턴한다.
BOOL CBlock::IsInBlock( const Vector2D *p)
{
	if (!p) return FALSE;

	int counter = 0;

	list<Wall2D*>::iterator itor = m_WallList.begin();
	while (itor != m_WallList.end())
	{
		Vector2D p1 = (*itor)->From();
		Vector2D p2 = (*itor)->To();

		if (p->y > min(p1.y, p2.y))
		{
			if (p->y <= max(p1.y, p2.y))
			{
				if (p->x <= max(p1.x, p2.x))
				{
					if (p1.y != p2.y)
					{
						double xinters = (p->y - p1.y) * (p2.x - p1.x) / (p2.y - p1.y) + p1.x;
						if (p1.x == p2.x || p->x <= xinters)
							counter++;
					}
				}
			}
		}
		++itor;
	}

	if (counter % 2 != 0)
	{
		return TRUE;
	}

	int count = 0;
	int idx = 0;
	double shortestDistance = 1000.f;
	itor = m_WallList.begin();
	while (itor != m_WallList.end())
	{
		const double dist = (*itor)->Distance(*p);
		if (dist < shortestDistance)
		{
			shortestDistance = dist;
			idx = count;
		}

		++itor;
		++count;
	}

	if (shortestDistance < MAP_CELL_COLLISION_LENGTH)
	{
		return TRUE;
	}

	return FALSE;
}


//
//
BOOL CBlock::IsCrossPoint( const Vector2D& A1, const Vector2D& A2 )
{
	list<Wall2D*>::iterator itor = m_WallList.begin();
	while (itor != m_WallList.end())
	{
		if ((*itor++)->IsCross(A1, A2))
		{
			return TRUE;
		}

		/*Vector2D B1 = (*itor)->From();
		Vector2D B2 = (*itor)->To();

		double under = (B2.y-B1.y)*(A2.x-A1.x)-(B2.x-B1.x)*(A2.y-A1.y);
		if(under==0) { ++itor; continue; }

		double _t = (B2.x-B1.x)*(A1.y-B1.y) - (B2.y-B1.y)*(A1.x-B1.x);
		double _s = (A2.x-A1.x)*(A1.y-B1.y) - (A2.y-A1.y)*(A1.x-B1.x);
		double t = _t/under;
		double s = _s/under;
	
		if(t<0.0 || t>1.0 || s<0.0 || s>1.0) { ++itor; continue; }
		if(_t==0 && _s==0) { ++itor; continue; }

		return TRUE;
/**/
	}

	return FALSE;
}


// 파일에 Block 정보를 저장한다.
//
BOOL CBlock::Write(std::ostream &os)
{
	os << endl;
	os << m_WallList.size();
	list<Wall2D*>::iterator itor = m_WallList.begin();
	while (itor != m_WallList.end())
	{
		(*itor)->Write(os);
		++itor;
	}

	return TRUE;
}


// 파일에서 Block 정보를 읽는다.
//
BOOL CBlock::Read(std::ifstream &is)
{
	Clear();

	int size;
	is >> size;

	m_WallList.clear();
	for (int i=0; i < size; ++i)
	{
		//m_WallList
		Wall2D *pwall = new Wall2D();
		pwall->Read(is);
		m_WallList.push_back(pwall);
	}

	return TRUE;
}
