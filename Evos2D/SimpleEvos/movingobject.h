
#ifndef __MOVINGOBJECT_H__
#define __MOVINGOBJECT_H__

#include "../Common/2D/Vector2D.h"
#include "messagedispatch.h"

struct lua_State;
class CMovingObject : public CMessageDispatch
{
public:
	CMovingObject();
	virtual ~CMovingObject();

protected:
	Vector2D m_Position;
	Vector2D m_Direction;
	float m_Scale;
	float m_Velocity;
	float m_Acceleration;

public:
	void SetPosition( const Vector2D &pos ) { m_Position = pos; }
	void SetDirection( const Vector2D &dir ) { m_Direction = dir; }
	void SetVelocity( const float velocity ) { m_Velocity = velocity; }
	void SetAcceleration( const float accel ) { m_Acceleration = accel; }
	void SetScale( const float scale ) { m_Scale = scale; }
	float GetVelocity() { return m_Velocity; }
	float GetAcceleration() { return m_Acceleration; }
	float GetScale() { return m_Scale; }
	const Vector2D& GetPosition() { return m_Position; }
	const Vector2D& GetDirection() { return m_Direction; }

	virtual void Update( const float timeDelta );
	virtual void Render() {}

	static void RegisteLuaBind(lua_State* pLua);

};

#endif
