//-----------------------------------------------------------------------------//
// 2009-08-23  programer: jaejung ┏(⊙д⊙)┛
// 
//-----------------------------------------------------------------------------//

#if !defined(__FRUSTUM_H__)
#define __FRUSTUM_H__


#include "global.h"


class CFrustum
{
public:
	CFrustum() {}
	virtual ~CFrustum() {}

protected:
	Vector3 m_Vtx[ 8];		// frustum 정점 좌표
	Vector3 m_vPos;			// 카메라 위치
	Plane	m_Plane[ 6];	// frustum의 6개 평면

public:
	// 카메라(view) * 프로젝션(projection)행렬을 입력받아 6개의 평면을 만든다.
	BOOL Make( Matrix44* pmatViewProj );
	// 한점 v가 프러스텀안에 있으면 TRUE를 반환, 아니면 FALSE를 반환한다.
	BOOL IsIn( Vector3* pv );
	// 중심(v)와 반지름(radius)를 갖는 경계구(bounding sphere)가 프러스텀안에 있으면
	// TRUE를 반환, 아니면 FALSE를 반환한다.
	BOOL IsInSphere( Vector3* pv, float radius );
	void Render();
	Vector3* GetPos() { return &m_vPos; }

};

#endif // __FRUSTUM_H__
