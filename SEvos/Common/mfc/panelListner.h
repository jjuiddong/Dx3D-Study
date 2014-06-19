
#ifndef __PANELLISTNER_H__
#define __PANELLISTNER_H__

#include <list>
#include <string>

enum PANEL_TYPE
{
	VIEW,
	MODEL,
	TERRAIN,
	TILE,
};

class IPanelObserver;
class IPanelListner
{
protected:
	IPanelListner() {}
	virtual ~IPanelListner() {}

public:
	virtual void Update(IPanelObserver *pObserver)=0;
};


class IPanelObserver
{
protected:
	IPanelObserver(PANEL_TYPE type) : m_Type(type) {}
	virtual ~IPanelObserver() {}

protected:
	PANEL_TYPE m_Type;
	std::list<IPanelListner*> m_listnerList;

public:
	PANEL_TYPE GetPanelType() { return m_Type; }
	void Add(IPanelListner *pListner) 
	{
		m_listnerList.push_back(pListner);
	}
	void Update()
	{
		std::list<IPanelListner*>::iterator itor = m_listnerList.begin();
		while (m_listnerList.end() != itor)
		{
			(*itor++)->Update(this);
		}
	}
};

#endif
