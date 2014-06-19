
#ifndef __MESSAGESYSTEM_H__
#define __MESSAGESYSTEM_H__

#include <list>
#include "message.h"


// 메세지 정보
class CMessageDispatch;
typedef struct SMessage
{
	Message msg;
	CMessageDispatch *preceiver;
	CMessageDispatch *psender;
	int param1;

	SMessage() {}
	SMessage(Message m):msg(m) { }
	SMessage(Message m, CMessageDispatch *receiver, CMessageDispatch *sender, int _param1 ) :
	msg(m), preceiver(receiver), psender(sender), param1(_param1) { }
} SMessage;


// 메세지 시스템
class CMessageSystem
{
protected:
	CMessageSystem() {} // 싱글톤이라서 인스턴스는 외부에서 생성할 수 없다.
public:
	virtual ~CMessageSystem() {}

protected:
	typedef std::list<SMessage> MsgList;
	typedef MsgList::iterator MsgItor;

	static CMessageSystem *m_pInstance;
	MsgList m_MsgList;

public:
	void SendMessage( const SMessage &msg );
	void PostMessage( const SMessage &pmsg );
	SMessage GetNextMessage();
	void Update( const float timeDelta );
	void Clear();

	// 싱글톤 구현
	static CMessageSystem* Get()
	{
		if (!m_pInstance)
		{
			m_pInstance = new CMessageSystem();
		}
		return m_pInstance;
	}

	static void Release()
	{
		if (m_pInstance)
		{
			m_pInstance->Clear();
			delete m_pInstance;
		}
		m_pInstance = NULL;
	}

};

#endif
