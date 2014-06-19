
#ifndef __UNITCONTROL_H__
#define __UNITCONTROL_H__

#include <list>


class CUnit;
class CUnitControl
{
public:
	CUnitControl();
	virtual ~CUnitControl();

	typedef std::list<CUnit*> UnitList;
	typedef UnitList::iterator UnitItor;

protected:
	UnitList m_UnitList;

public:
	void AddUnit( CUnit *punit );
	void DelUnit( CUnit *punit );
	CUnit* GetUnit( int idx );
	CUnit* GetClosetUnit( CUnit *punit );
	void Update(float timeDelta);
	void Render();
	void Clear();

};

#endif
