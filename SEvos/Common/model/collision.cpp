
#include "stdafx.h"
//#include "global.h"
#include "collision.h"

using namespace std;


///////////////////////////////////////////////////////////////////////////////////////
// Collision
int g_CompareId;

int FindCollision( ICollisionable *pObj )
{
	if( pObj->GetObjId() == g_CompareId )
		return 1;
	return 0;
}


CollisionList::CollisionList()
{
	m_pGrp1 = new SObjTree( 0 );
	m_pGrp2 = new SObjTree( 0 );

}

CollisionList::~CollisionList()
{
	ReleaseTree( m_pGrp1 );
	ReleaseTree( m_pGrp2 );

}


//-----------------------------------------------------------------------------//
// nGroup: 0, 1 만 지원한다.
// 충돌테스트는 다른 그룹끼리만 가능하다.
// pObj : 충돌테스트할 오브젝트
// nTestNum : 충돌테스트 번호이며, 다른 그룹에서 같은 충돌테스트번호 끼리 충돌테스트한다.
//-----------------------------------------------------------------------------//
void CollisionList::AddObj( int nGroup, ICollisionable *pObj, int nTestNum )
{
	SObjTree *grp;
	if( 0 == nGroup ) grp = m_pGrp1;
	else if( 1 == nGroup ) grp = m_pGrp2;
	else return;
	AddObj( grp, pObj, nTestNum );
}


//-----------------------------------------------------------------------------//
// 충돌테스트할 오브젝트 추가
// pParent : 부모 충돌 오브젝트
// pParent 오브젝트가 충돌된후 자식으로 pObj 오브젝트가 충돌테스트 검사된다.
//-----------------------------------------------------------------------------//
void CollisionList::AddObj( ICollisionable *pParent, ICollisionable *pObj, int nTestNum )
{
	// 두그룹에서 부모 노드를 찾는다.
	SObjTree *parent;
	parent = (SObjTree*)FindTree( m_pGrp1, pParent->GetObjId() );
	if( !parent ) parent = (SObjTree*)FindTree( m_pGrp2, pParent->GetObjId() );
	if( !parent ) return;
	AddObj( parent, pObj, nTestNum );
}


//-----------------------------------------------------------------------------//
// 충돌테스트할 오브젝트 추가
// nTestNum 이 3는 오브젝트가 이동할때 충돌을 체크하는 번호이다.
//-----------------------------------------------------------------------------//
void CollisionList::AddObj( SObjTree *pParent, ICollisionable *pObj, int nTestNum )
{
	SObjTree *pnew = new SObjTree( pObj->GetObjId() );
	pnew->testnum = nTestNum;
	pnew->pobj = pObj;
	pnew->psphere = pObj->GetSphere();
	pnew->pminsphere = pObj->GetMinimumSphere();

	if( !pnew->psphere )
		pObj->GetCollisionBox( &pnew->boxlist );

	InsertChildTree( pParent, pnew );
}


//-----------------------------------------------------------------------------//
// 리스트에서 오브젝트 추가
//-----------------------------------------------------------------------------//
void CollisionList::DelObj( ICollisionable *pObj )
{
	SObjTree *p1 = (SObjTree*)FindTree( m_pGrp1, pObj->GetObjId() );
	SObjTree *p2 = (SObjTree*)FindTree( m_pGrp2, pObj->GetObjId() );
	DelTree( m_pGrp1, pObj->GetObjId() );
	DelTree( m_pGrp2, pObj->GetObjId() );
	ReleaseTree( p1 );
	ReleaseTree( p2 );
}


//-----------------------------------------------------------------------------//
// 충돌테스트하기 전에 CollisionBox 좌표를 업데이트한다.
// 모델이 이동되었다면 CollisionTest() 함수 호출 전에 이 함수를 먼저 호출해야 한다.
//-----------------------------------------------------------------------------//
void CollisionList::UpdateCollisionBox()
{
	SObjTree *pGrp1 = (SObjTree*)m_pGrp1->m_pChild;
	SObjTree *pGrp2 = (SObjTree*)m_pGrp2->m_pChild;
	while( pGrp1 )
	{
		pGrp1->pobj->UpdateCollisionBox();
		pGrp1 = (SObjTree*)pGrp1->m_pNext;
	}
	while( pGrp2 )
	{
		pGrp2->pobj->UpdateCollisionBox();
		pGrp2 = (SObjTree*)pGrp2->m_pNext;
	}
}


//-----------------------------------------------------------------------------//
// nTestNum 값으로 설정된 CollisionBox들만 충돌테스트한다.
// 같은그룹끼리는 충돌테스트 하지 않는다.
// 충돌된 오브젝트 갯수를 리턴한다.
//-----------------------------------------------------------------------------//
int CollisionList::CollisionTest( int nTestNum )
{
	SObjTree *pGrp1 = (SObjTree*)m_pGrp1->m_pChild;
	int cnt = 0;
	while( pGrp1 )
	{
		if( !pGrp1->pobj->IsTest(nTestNum) )
		{
			pGrp1 = (SObjTree*)pGrp1->m_pNext;
			continue;
		}

		SObjTree *pGrp2 = (SObjTree*)m_pGrp2->m_pChild;
		while( pGrp2 )
		{
			if( !pGrp2->pobj->IsTest(nTestNum) )
			{
				pGrp2 = (SObjTree*)pGrp2->m_pNext;
				continue;
			}

			// Root그룹 충돌 테스트
			if( CollisionTest_Obj(pGrp1, pGrp2, nTestNum) )
			{
				// 자식이 없다면 충돌테스트 끝
				// nTestNum이 3이라면 루트만 충돌테스트 한다.
				if( 3 == nTestNum || (!pGrp1->m_pChild && !pGrp2->m_pChild) )
				{
					m_ChkTable[ cnt][ 0] = pGrp1->pobj;
					m_ChkTable[ cnt][ 1] = pGrp2->pobj;
					++cnt;
				}
				else
				{
					// 자식이 있다면 자식까지 충돌테스트 성공해야 최종적으로 충돌된 상태가 된다.
					SObjTree *p1 = (SObjTree*)((pGrp1->m_pChild)? pGrp1->m_pChild : pGrp1);
					SObjTree *p2 = (SObjTree*)((pGrp2->m_pChild)? pGrp2->m_pChild : pGrp2);
					if( CollisionTest_SrcRec(p1, p2, nTestNum) )
					{
						m_ChkTable[ cnt][ 0] = pGrp1->pobj;
						m_ChkTable[ cnt][ 1] = pGrp2->pobj;
						++cnt;
					}
				}
			}
			pGrp2 = (SObjTree*)pGrp2->m_pNext;
		}
		pGrp1 = (SObjTree*)pGrp1->m_pNext;
	}
	return cnt;
}


//-----------------------------------------------------------------------------//
// 그룹내 충돌테스트
// nGroup : 테스트할 그룹
// nTestNum :충돌테스트 번호
//-----------------------------------------------------------------------------//
int CollisionList::CollisionGroupTest( int nGroup, int nTestNum )
{
	SObjTree *grp;
	if( 0 == nGroup ) grp = m_pGrp1;
	else if( 1 == nGroup ) grp = m_pGrp2;
	else return 0;
	return _CollisionGroupTest( grp, nTestNum );
}
int CollisionList::_CollisionGroupTest( SObjTree *pGrp, int nTestNum )
{
	SObjTree *pGrp1 = (SObjTree*)pGrp->m_pChild;
	
	int cnt = 0;
	while( pGrp1 )
	{
		if( !pGrp1->pobj->IsTest(nTestNum) )
		{
			pGrp1 = (SObjTree*)pGrp1->m_pNext;
			continue;
		}

		SObjTree *pGrp2 = (SObjTree*)pGrp1->m_pNext;
		while( pGrp2 )
		{
			if( !pGrp2->pobj->IsTest(nTestNum) )
			{
				pGrp2 = (SObjTree*)pGrp2->m_pNext;
				continue;
			}

			// Root그룹 충돌 테스트
			if( CollisionTest_Obj(pGrp1, pGrp2, nTestNum) )
			{
				// 자식이 없다면 충돌테스트 끝
				// nTestNum이 3이라면 루트만 충돌테스트 한다.
				if( 3 == nTestNum || (!pGrp1->m_pChild && !pGrp2->m_pChild) )
				{
					m_ChkTable[ cnt][ 0] = pGrp1->pobj;
					m_ChkTable[ cnt][ 1] = pGrp2->pobj;
					++cnt;
				}
				else
				{
					// 자식이 있다면 자식까지 충돌테스트 성공해야 최종적으로 충돌된 상태가 된다.
					SObjTree *p1 = (SObjTree*)((pGrp1->m_pChild)? pGrp1->m_pChild : pGrp1);
					SObjTree *p2 = (SObjTree*)((pGrp2->m_pChild)? pGrp2->m_pChild : pGrp2);
					if( CollisionTest_SrcRec(p1, p2, nTestNum) )
					{
						m_ChkTable[ cnt][ 0] = pGrp1->pobj;
						m_ChkTable[ cnt][ 1] = pGrp2->pobj;
						++cnt;
					}
				}
			}
			pGrp2 = (SObjTree*)pGrp2->m_pNext;
		}
		pGrp1 = (SObjTree*)pGrp1->m_pNext;
	}
	return cnt;
}


//-----------------------------------------------------------------------------//
// 두개의 SObjTree 그룹을 충돌테스트 한다.
// SObjTree가 트리자료 구조로 되어있기 때문에 트리를 순회하기 위해서는 
// 두개의 State가 필요하다. (CollisionTest_SrcRec, CollisionTest_TargetRec)
//-----------------------------------------------------------------------------//
BOOL CollisionList::CollisionTest_SrcRec( SObjTree *pSrc, SObjTree *pTarget, int nTestNum )
{
	if( 0 == nTestNum ) return FALSE; // error
	if( !pSrc || !pTarget ) return FALSE;

	if( CollisionTest_TargetRec(pSrc, pTarget, nTestNum) ) return TRUE;
	return CollisionTest_SrcRec( (SObjTree*)pSrc->m_pNext, pTarget, nTestNum );
}
BOOL CollisionList::CollisionTest_TargetRec( SObjTree *pSrc, SObjTree *pTarget, int nTestNum )
{
	if( 0 == nTestNum ) return FALSE; // error
	if( !pSrc || !pTarget ) return FALSE;

	if( CollisionTest_Obj(pSrc, pTarget, nTestNum) )
	{
		if( !pSrc->m_pChild && !pTarget->m_pChild ) return TRUE;
		// 자식이 있을때는 자식까지 충돌테스트 한다.
		SObjTree *p1 = (SObjTree*)((pSrc->m_pChild)? pSrc->m_pChild : pSrc);
		SObjTree *p2 = (SObjTree*)((pTarget->m_pChild)? pTarget->m_pChild : pTarget);
		return CollisionTest_SrcRec( p1, p2, nTestNum );
	}
	return CollisionTest_TargetRec( (SObjTree*)pSrc, (SObjTree*)pTarget->m_pNext, nTestNum );
}


//-----------------------------------------------------------------------------//
// pSrc, pTarget 노드가 같은 nTestNum 테스트번호를 가졌을때만 충돌테스트 한다.
//-----------------------------------------------------------------------------//
BOOL CollisionList::CollisionTest_Obj( SObjTree *pSrc, SObjTree *pTarget, int nTestNum )
{
//	if( 0 == nTestNum ) return FALSE; // error
	if( !pSrc || !pTarget ) return FALSE;

	// Src TestNum값이 0일때는 Target의 TestNum상관없이 충돌테스트를 한다.
	// Src TestNum값이 설정되어 있을때는 인자로 넘어온 nTestNum값과 Target의 TestNum이 같을때만 충돌테스트한다.
	if( ((0 != pSrc->testnum) && (nTestNum != pSrc->testnum)) ||
		(((0 != pSrc->testnum) && (0 != pTarget->testnum)) && (pSrc->testnum != pTarget->testnum)) )
		return FALSE;

	// Sphere vs Sphere 충돌테스트
	if( 3 == nTestNum )
	{
		if( pSrc->pminsphere && pTarget->pminsphere )
			return pSrc->pminsphere->Collision( pTarget->pminsphere );
	}
	else
	{
		if( pSrc->psphere && pTarget->psphere )
			return pSrc->psphere->Collision( pTarget->psphere );
	}

	// 다른오브젝트끼리 충돌테스트 불가
	if( pSrc->psphere || pTarget->psphere )
		return FALSE;

	// Box vs Box 충돌테스트
	if( pSrc->boxlist.size() <= 0 || pTarget->boxlist.size() <= 0 )
		return FALSE;

	// 충돌 체크
	list<Box*>::iterator it = pSrc->boxlist.begin();
	while( pSrc->boxlist.end() != it )
	{
		list<Box*>::iterator kt = pTarget->boxlist.begin();
		while( pTarget->boxlist.end() != kt )
		{
			if( (*it)->Collision(*kt) )
			{
				return TRUE;
			}
			++kt;
		}
		++it;
	}

	return FALSE;
}

