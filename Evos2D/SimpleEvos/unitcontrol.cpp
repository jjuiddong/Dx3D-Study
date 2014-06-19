
#include "stdafx.h"

#include <algorithm>
#include "unit.h"
#include "unitcontrol.h"

using namespace std;


class isUnit: public unary_function<CUnit*, bool>
{
public:
	isUnit(CUnit *punit):m_pUnit(punit) {}
	CUnit *m_pUnit;
	bool operator ()(CUnit *t) const
	{
		if (t == m_pUnit)
			return true;
		return false;
	}
};


CUnitControl::CUnitControl()
{

}


CUnitControl::~CUnitControl()
{
	Clear();

}


//------------------------------------------------------------------------
// 
// [2011/1/23 jjuiddong]
//------------------------------------------------------------------------
void CUnitControl::AddUnit(CUnit *punit)
{
	m_UnitList.push_back(punit);

}


//------------------------------------------------------------------------
// 
// [2011/1/23 jjuiddong]
//------------------------------------------------------------------------
void CUnitControl::DelUnit(CUnit *punit)
{
//	find_if(m_UnitList.begin(), m_UnitList.end(), 
	UnitItor itor = find(m_UnitList.begin(), m_UnitList.end(), punit);
	if (m_UnitList.end() == itor)
		return;

	CUnit *pdelUnit = *itor;
	delete pdelUnit;
	m_UnitList.erase(itor);
}


//------------------------------------------------------------------------
// 
// [2011/1/23 jjuiddong]
//------------------------------------------------------------------------
void CUnitControl::Update(float timeDelta)
{
	UnitItor itor = m_UnitList.begin();
	while (m_UnitList.end() != itor)
	{
		(*itor++)->Update(timeDelta);
	}
}


//------------------------------------------------------------------------
// 
// [2011/1/23 jjuiddong]
//------------------------------------------------------------------------
void CUnitControl::Render()
{
	UnitItor itor = m_UnitList.begin();
	while (m_UnitList.end() != itor)
	{
		(*itor++)->Render();
	}
}


//------------------------------------------------------------------------
// 
// [2011/1/23 jjuiddong]
//------------------------------------------------------------------------
void CUnitControl::Clear()
{
	UnitItor itor = m_UnitList.begin();
	while (m_UnitList.end() != itor)
	{
		CUnit *punit = *itor++;
		delete punit;
	}
	m_UnitList.clear();
}


//------------------------------------------------------------------------
// index 에 해당하는 유닛을 리턴한다.
// [2011/1/23 jjuiddong]
//------------------------------------------------------------------------
CUnit* CUnitControl::GetUnit(int index)
{
	int count = 0;
	UnitItor itor = m_UnitList.begin();
	while (m_UnitList.end() != itor)
	{
		CUnit *punit = *itor++;
		if (index == count)
		{
			return punit;
		}
		++count;
	}
	return NULL;
}


//------------------------------------------------------------------------
// punit과 가장 가까운 유닛을 리턴한다. punit 자신은 리턴되지 않는다.
// [2011/1/23 jjuiddong]
//------------------------------------------------------------------------
CUnit* CUnitControl::GetClosetUnit( CUnit *punit )
{
	double closetLength = 1000000.f;
	CUnit *pclosetUnit = NULL;

	UnitItor itor = m_UnitList.begin();
	while (m_UnitList.end() != itor)
	{
		CUnit *p = *itor++;
		if (p == punit) continue;

		const double len = p->GetPosition().DistanceSq(punit->GetPosition());
		if (len < closetLength)
		{
			pclosetUnit = p;
			closetLength = len;
		}
	}

	return pclosetUnit;
}

