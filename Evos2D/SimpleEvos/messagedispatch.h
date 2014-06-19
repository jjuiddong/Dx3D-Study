
#ifndef __MESSAGEDISPATCH_H__
#define __MESSAGEDISPATCH_H__


struct SMessage;
class CMessageDispatch
{
public:
	CMessageDispatch() {}
	virtual ~CMessageDispatch() {}

protected:

public:
	virtual void SendMsg(const SMessage &msg);
	virtual BOOL MsgDipatch(const SMessage &msg);

};

#endif
