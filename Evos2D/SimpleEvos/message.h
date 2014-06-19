
#ifndef __MESSAGE_H__
#define __MESSAGE_H__


// 메세지 타입
enum Message
{
	MSG_NONE,

	MSG_KEYDOWN,
		// param1 = virtual key value

	MSG_NOPATHAVAILABLE,

	MSG_PATHREADY,

};


#endif
