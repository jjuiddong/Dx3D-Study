//-----------------------------------------------------------------------------//
// 2008-01-21  programer: jaejung ┏(⊙д⊙)┛
// 
// CCollisionList 클래스는 충돌테스트를 일괄적으로 처리한다.
// CCollisionList 클래스는 충돌테스트될 오브젝트를 2개의 그룹으로 나누고, 각 그룹은
// 트리 자료구조 형태다. 즉 부모끼리 충돌테스트가 성공하면 자식으로 내려가 충돌테스트
// 하고, 재귀적으로 충돌테스트된 후 최종 노드까지 충돌테스트가 성공하면 충돌되었다고 알려준다.
// 각 오브젝트는 nTestNum을 설정해 다른 그룹끼리 충돌테스트시 같은 nTestNum끼리만 충돌테스트한다.
// 충돌테스트할 오브젝트는 ICollisionable 인터페이스를 상속받아야 한다.
//-----------------------------------------------------------------------------//

#if !defined(__COLLISION_H__)
#define __COLLISION_H__

#pragma warning (disable: 4786)
#include <list>


///////////////////////////////////////////////////////////////////////////////////////
// interface ICollisionable
// 충돌테스트할 오브젝트는 ICollisionable 인터페이스를 상속받아 구현해야 한다.
struct ICollisionable
{
	// Collision Test할지 여부
	virtual BOOL IsTest( int TestNum )=0;
	virtual int GetObjId()=0;

	//-----------------------------------------------------------------------------//
	// 모델의 월드행렬을 리턴한다.
	// 만약 모델이 다른 모델의 자식으로 연결되어 있다면 부모의 행렬을 적용해서 리턴된다.
	//-----------------------------------------------------------------------------//
	virtual Matrix44* GetWorldTM()=0;

	// 충돌구 리턴
	virtual Sphere* GetSphere()=0;
	// 최소 충돌구 리턴 (이동시 사용됨)
	virtual Sphere* GetMinimumSphere()=0;

	//-----------------------------------------------------------------------------//
	// 충돌테스트 하기전에 충돌박스, 충돌구의 좌표를 제위치로 옮기기 위해서 
	// 월드행렬을 곱한다. Bone의 경우 내부적으로 월드행렬을 가지고 있다.
	// UpdateCollisionBox() 함수는 충돌테스트하기 전에만 호출되며 
	// 이는 매 루프마다 충돌박스 위치를 업데이트 해줘야하는 오버헤드를 줄이기위해서다.
	//-----------------------------------------------------------------------------//
	virtual void UpdateCollisionBox()=0;

	//-----------------------------------------------------------------------------//
	// 모델이 가진 충돌박스를 리스트에 저장한후 리턴한다. 
	// 충돌박스는 Mesh, Bone의 BoneNode에서 가지고 있다.
	//-----------------------------------------------------------------------------//
	virtual void GetCollisionBox( std::list<Box*> *pList )=0;

	//-----------------------------------------------------------------------------//
	// 충돌된후 충돌을 일으킨 객체에게 호출됨
	// TestNum: 테스트 번호
	// pObj : 충돌된 객체 포인터
	//-----------------------------------------------------------------------------//
	virtual void Collision( int TestNum, ICollisionable *pObj )=0;
};


///////////////////////////////////////////////////////////////////////////////////////
// CollisionList

#include "3dnode.h"

// 충돌테스트할 오브젝트를 일괄적으로 처리한다.
class CollisionList
{
public:
	CollisionList();
	virtual ~CollisionList();
	enum { MAX_COLLISION_TABLE = 64 };
	ICollisionable* m_ChkTable[ MAX_COLLISION_TABLE][ 2];		// [table size][ src/dst]

protected:
	// 충돌테스트할 오브젝트를 저장하는 트리
	typedef struct _tagObjTree : C3dNode
	{
		int testnum;			// 테스트번호 (0번을 제외한, 같은 테스트번호끼리 비교한다.)
		ICollisionable *pobj;
		Sphere *psphere;
		Sphere *pminsphere;		// 캐릭터 이동충돌체크를 위해서 만들어짐
		std::list< Box* > boxlist;
		_tagObjTree( int id ) : C3dNode(id) {}

	} SObjTree;
	SObjTree *m_pGrp1;
	SObjTree *m_pGrp2;

public:
	void AddObj( int nGroup, ICollisionable *pObj, int nTestNum=0 );			// 충돌테스트할 오브젝트 추가
	void AddObj( ICollisionable *pParent, ICollisionable *pObj, int nTestNum=0 ); // 충돌테스트할 오브젝트 추가
	void DelObj( ICollisionable *pObj );						// 리스트에서 오브젝트 추가
	void UpdateCollisionBox();									// 충돌박스의 좌표를 갱신한다.
	int CollisionTest( int nTestNum );							// 충돌테스트
	int CollisionGroupTest( int nGroup, int nTestNum );			// 그룹내 충돌테스트

protected:
	void AddObj( SObjTree *pParent, ICollisionable *pObj, int nTestNum );
	BOOL CollisionTest_SrcRec( SObjTree *pSrc, SObjTree *pTarget, int nTestNum );
	BOOL CollisionTest_TargetRec( SObjTree *pSrc, SObjTree *pTarget, int nTestNum );
	BOOL CollisionTest_Obj( SObjTree *pSrc, SObjTree *pTarget, int nTestNum );
	int _CollisionGroupTest( SObjTree *pGrp, int nTestNum );
};

#endif // __COLLISION_H__
