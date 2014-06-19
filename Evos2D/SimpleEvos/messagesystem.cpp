
#include "stdafx.h"
#include "../Common/global.h"
#include "messagedispatch.h"

#include "messagesystem.h"

CMessageSystem *CMessageSystem::m_pInstance = NULL;

//
// 메세지를 리스트에 저장한다.
void CMessageSystem::SendMessage( const SMessage &msg )
{
	m_MsgList.push_back(msg);
}


//
//
void CMessageSystem::PostMessage( const SMessage &msg )
{
	m_MsgList.push_back(msg);
}


//------------------------------------------------------------------------
// 
// [2011/1/14 jjuiddong]
//------------------------------------------------------------------------
SMessage CMessageSystem::GetNextMessage()
{
	if (m_MsgList.empty()) 
	{
		return SMessage(MSG_NONE);
	}
	SMessage msg = m_MsgList.front();
	m_MsgList.pop_front();
	return msg;
}


//------------------------------------------------------------------------
// 저장된 메세지를 모두 처리한다.
// [2011/1/14 jjuiddong]
//------------------------------------------------------------------------
void CMessageSystem::Update( const float timeDelta )
{
	while (!m_MsgList.empty())
	{
		if (m_MsgList.front().preceiver)
		{
			m_MsgList.front().preceiver->MsgDipatch(m_MsgList.front());
		}
		m_MsgList.pop_front();
	}

}


//------------------------------------------------------------------------
// 메세지 시스템 초기화
// [2011/1/14 jjuiddong]
//------------------------------------------------------------------------
void CMessageSystem::Clear()
{
	m_MsgList.clear();

}

