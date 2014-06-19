
#ifndef __DISPOBJECT_H__
#define __DISPOBJECT_H__


//------------------------------------------------------------------------
// 화면에 출력하는 오브젝트를 추상화한 클래스다.
// [2011/3/15 jjuiddong]
//------------------------------------------------------------------------
class CDispObject
{
public:
	CDispObject() { }
	virtual ~CDispObject() { }

public:
	virtual int GetId() = 0;
	virtual void Render() = 0;
	virtual void RenderDepth() = 0;
	virtual BOOL Update( int elapseTime ) = 0;
	virtual Matrix44* GetWorldTM() = 0;
	virtual void Clear() = 0;

};

#endif
