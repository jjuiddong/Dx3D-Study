
#include "stdafx.h"

#include "messagesystem.h"
#include "messagedispatch.h"


//------------------------------------------------------------------------
// 
// [2011/1/14 jjuiddong]
//------------------------------------------------------------------------
void CMessageDispatch::SendMsg(const SMessage &msg)
{
	CMessageSystem::Get()->SendMessage(msg);
}

//------------------------------------------------------------------------
// true를 리턴하면 파생된 클래스에게 까지 전달된다.
// [2011/1/14 jjuiddong]
//------------------------------------------------------------------------
BOOL CMessageDispatch::MsgDipatch(const SMessage &msg)
{
	// 상속받는 클래스에서 구현한다.
	return TRUE;
}
