
#include "stdafx.h"
#include "control.h"
#include "character.h"
#include "ai_object.h"
#include "../model/collision.h"

using namespace std;

CControl g_Control;	// 전역변수

CControl::CControl() : m_pHero(NULL)
{
	m_pCollision = new CollisionList;
//	m_pTerrain = new CTerrain;
 
}


CControl::~CControl() 
{
	Clear();

}


//-----------------------------------------------------------------------------//
// 에니메이션
//-----------------------------------------------------------------------------//
void CControl::Animate( int nDelta )
{
	CharMap::iterator it = m_ModelList.begin();
	while( m_ModelList.end() != it )
	{
		// 죽은 오브젝트는 DeadList에 삽입된다.
		it->second.pchar->Animate( nDelta );
		if( !it->second.pai->Animate(nDelta) )
		{
			m_DeadList.push_back( it->second );
			it = m_ModelList.erase( it );
		}
		else
		{
			++it;
		}
	}

	m_pCollision->UpdateCollisionBox();
	for( int k=1; k < 3; ++k )
	{
		int collisioncnt = m_pCollision->CollisionTest( k );
		if( 0 < collisioncnt )
		{
			g_Dbg.Console( "Collision\n" );
			for( int i=0; i < collisioncnt; ++i )
			{
				m_pCollision->m_ChkTable[ i][ 0]->Collision( k, m_pCollision->m_ChkTable[ i][ 1] );
				m_pCollision->m_ChkTable[ i][ 1]->Collision( k, m_pCollision->m_ChkTable[ i][ 0] );
			}
		}
	}

	MsgProc();
	AIGroupProc( nDelta );

	// 주인공 위치에 따라 카메라 위치가 이동한다.
	if( m_pHero )
	{
		Vector3 pos = m_pHero->GetPos() + g_DefaultCameraPos;
//		Vector3 cp = g_Camera.GetPosition();
//		if( cp != pos )
		{
			g_Camera.SetPosition( pos );
		}
	}
}


//-----------------------------------------------------------------------------//
// 출력
//-----------------------------------------------------------------------------//
void CControl::Render()
{
	CharMap::iterator it = m_ModelList.begin();
	while( m_ModelList.end() != it )
	{
		it->second.pchar->Render();
		++it;
	}
//	m_pTerrain->Render();

}


//-----------------------------------------------------------------------------//
// 캐릭터 추가
//-----------------------------------------------------------------------------//
CCharacter* CControl::AddCharacter( CHARACTER_TYPE Type, char *pName, char *pModelFile, 
							        char *pAniFile, char *pWeaponFile, char *pComboFile, char *pAIFile )
{
	if( !pModelFile ) return NULL;

	CharMap::iterator it = m_ModelList.find( pName );	// 같은 아이디가 존재하면 리턴된다.
	if( m_ModelList.end() != it ) return it->second.pchar;

	// 캐릭터 클래스 생성
	CCharacter *pchar = new CCharacter;
	pchar->SetName( pName );
	pchar->LoadBody( pModelFile );
	if( pWeaponFile ) pchar->LoadWeapon( pWeaponFile );
	if( pAniFile ) pchar->LoadAni( pAniFile );
	if( pComboFile ) pchar->LoadCombo( pComboFile );
	pchar->SetCollision( Type, m_pCollision );
	pchar->SetTerrain( m_pTerrain );

	// 인공지능 클래스 생성
	CAIObject *pai = new CAIObject;
	pai->Create( this, pchar, pAIFile );

	// 등록
	m_ModelList.insert( CharMap::value_type(pName,SObject(pchar,pai)) );

	if( !strcmp("hero", pName) )
		m_pHero = pchar;

	return pchar;
}




//-----------------------------------------------------------------------------//
// 메모리 소거, 초기화
//-----------------------------------------------------------------------------//
void CControl::Clear()
{
	CharMap::iterator it = m_ModelList.begin();
	while( m_ModelList.end() != it )
	{
		SObject s = it++->second;
		delete s.pchar;
		delete s.pai;
	}
	m_ModelList.clear();

	CharList::iterator i = m_DeadList.begin();
	while( m_DeadList.end() != i )
	{
		SObject s = *i++;
		delete s.pchar;
		delete s.pai;
	}
	m_DeadList.clear();

	SAFE_DELETE( m_pCollision );
//	SAFE_DELETE( m_pTerrain );
}


//-----------------------------------------------------------------------------//
// 메세지를 받아서 메세지큐에 저장한다.
//-----------------------------------------------------------------------------//
void CControl::Command( SMsg &Msg )
{
	m_MsgQ.push( Msg );
}


//-----------------------------------------------------------------------------//
// 메세지를 처리한다.
//-----------------------------------------------------------------------------//
void CControl::MsgProc()
{
	const int MAX_PROC = 10;
	int proccnt = 0;

	while( !m_MsgQ.empty() && proccnt < MAX_PROC )
	{
		const SMsg &msg = m_MsgQ.front();

		// Control에서 처리될 메세지
		if( MSG_CONTROL == msg.type )
		{

		}
		else
		{
			// id가 NULL이면 주인공을 뜻한다.
			char *key = "hero";
			if( msg.id ) key = msg.id;

			CharMap::iterator it = m_ModelList.find( key );
			if( m_ModelList.end() != it ) 
			{
				SObject obj = it->second;
				obj.pchar->Command( msg );
				obj.pai->Command( msg );
			}
		}

		proccnt++;
		m_MsgQ.pop();
	}
}


//-----------------------------------------------------------------------------//
// 캐릭터, 인공지능 클래스 찾기
//-----------------------------------------------------------------------------//
CCharacter* CControl::GetCharacter( char *pCharacterName )
{
	CharMap::iterator it = m_ModelList.find( pCharacterName );
	if( it == m_ModelList.end() ) return NULL;
	return it->second.pchar;
}
CAIObject* CControl::GetAI( char *pCharacterName )
{
	CharMap::iterator it = m_ModelList.find( pCharacterName );
	if( it == m_ModelList.end() ) return NULL;
	return it->second.pai;
}


//-----------------------------------------------------------------------------//
// 공격할 타겟의 위치를 리턴한다. 타겟이 없다면 FALSE를 리턴한다.
// pCharacter : 공격한 캐릭터 
// pReval : 타겟위치 값이 설정된후 리턴될 변수
//-----------------------------------------------------------------------------//
CCharacter* CControl::GetAttackTarget( CCharacter *pCharacter, Vector3 *pReval )
{
	Vector3 p0 = pCharacter->GetPos();
	CHARACTER_TYPE type = pCharacter->GetCharacterType();

	// 주인공이 공격할경우 주인공이 바라보는 영역안의 타겟만 공격하며, 뒤에 있는 
	// 타겟은 공격하지 못한다.
	// 그러나 적의 경우 타겟이 앞에 있든 뒤에 있든 관계없이 타겟을 설정할수 있다.
	// 타겟이 뒤에 있다면 dotproduct() 값은 음수가 되므로 이를 이용해서 구현할수 있다.
	float max_angle = (pCharacter->GetCharacterType()==CHAR_HERO)? 0.f : -2.f;
	float attack_len = pCharacter->GetAttackLength();
	CCharacter *ptarget = NULL;

	CharMap::iterator it = m_ModelList.begin();
	while( m_ModelList.end() != it )
	{
		SObject obj = it++->second;
		if( type == obj.pchar->GetCharacterType() ) continue;

		Vector3 p1 = obj.pchar->GetPos();
		float l = (float)sqrt( p0.LengthRoughly(p1) );
		// 공격 범위 내에 있는지 검사
		if( attack_len > l )
		{
			Vector3 dir = p1 - p0;
			dir.Normalize();
			float angle = ( dir.DotProduct( pCharacter->GetDirection() ) );

			// 공격자의 방향과 적의 위치가 가까울수록 타겟이 된다.
			if( max_angle < angle )
			{
				ptarget = obj.pchar;
				max_angle = angle;
			}
		}
	}

	if( !ptarget ) return NULL;

	*pReval = ptarget->GetPos() - p0;
	pReval->y = 0;
	pReval->Normalize();
	
	return ptarget;
}


//-----------------------------------------------------------------------------//
// 지형파일 로드
//-----------------------------------------------------------------------------//
BOOL CControl::LoadTerrain( char *pFileName )
{
//	if( !m_pTerrain->LoadTerrain(pFileName) )
//		return FALSE;
//	if( !m_pTerrain->SetTerrain() )
//		return FALSE;
	return TRUE;
}


//-----------------------------------------------------------------------------//
// 집단 인공지능에 관련된 처리
//		캐릭터가 이동한 후 도착점에서 서로 위치가 비슷하다면 중복되지 않는 위치로 
//		다시 이동한다.
//-----------------------------------------------------------------------------//
void CControl::AIGroupProc( int nDelta )
{
	m_ElapseTime += nDelta;
	if( 300 > m_ElapseTime ) return;
	m_ElapseTime = 0;

	// 캐릭터 이동충돌 테스트번호는 3번이다.
	int collisioncnt = m_pCollision->CollisionGroupTest( 1, 3 );
	if( 0 < collisioncnt )
	{
		for( int i=0; i < collisioncnt; ++i )
		{
			CCharacter *pc0 = (CCharacter*)m_pCollision->m_ChkTable[ i][ 0];
			CCharacter *pc1 = (CCharacter*)m_pCollision->m_ChkTable[ i][ 1];

			if( CHAR_HERO == pc0->GetCharacterType() || CHAR_HERO == pc1->GetCharacterType() ) continue;

			// pc1, pa1 : 움직일 오브젝트
			Vector3 p0;
			Vector3 p1 = pc1->GetPos();
			if( pc1->GetDestPos().EqualEpsilon(3.f, p1) )
			{
				// exchange
				CCharacter *ptmp1 = pc1;
				pc1 = pc0;
				pc0 = ptmp1;
			}

			p0 = pc0->GetPos();
			p1 = pc1->GetPos();
			Vector3 vt = p1 - p0;
			vt.Normalize();

			Vector3 v = pc0->GetDirection() + vt;
			v.Normalize();
			v *= 20.f;

			if( pc1->GetDestPos().EqualEpsilon(3.f, p1) )
				pc1->Move( p0 + v );
		}
	}
}
