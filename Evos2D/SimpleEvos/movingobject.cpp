
#include "stdafx.h"
#include "global.h"
#include "movingobject.h"


//------------------------------------------------------------------------
// 
// [2011/1/13 jjuiddong]
//------------------------------------------------------------------------
CMovingObject::CMovingObject()
{
	m_Position = Vector2D(0, 0);
	m_Direction = Vector2D(0, 1);
	m_Scale = 5.f;
	m_Acceleration = 0.f;

}


//------------------------------------------------------------------------
// 
// [2011/1/13 jjuiddong]
//------------------------------------------------------------------------
CMovingObject::~CMovingObject()
{

}


//------------------------------------------------------------------------
// 
// [2011/1/13 jjuiddong]
//------------------------------------------------------------------------
void CMovingObject::Update( const float timeDelta )
{
	m_Velocity += m_Acceleration * timeDelta;
	const float moveLength = m_Velocity * timeDelta;
	m_Position += m_Direction * moveLength;
}


//------------------------------------------------------------------------
// 
// [2011/2/2 jjuiddong]
//------------------------------------------------------------------------
void CMovingObject::RegisteLuaBind(lua_State* pLua)
{
/*
	using namespace luabind;
	module(pLua)
		[
			class_<CMovingObject>("MovingObject")
			.def(constructor<>())
			.def("GetVelocity", &CMovingObject::GetVelocity)
			.def("GetAcceleration", &CMovingObject::GetAcceleration)
			.def("GetScale", &CMovingObject::GetScale)
			.def("GetPosition", &CMovingObject::GetPosition)
		];
/**/
}

