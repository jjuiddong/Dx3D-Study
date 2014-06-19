
#include "stdafx.h"
#include "../model/model.h"
#include "control.h"
#include "lifebar.h"
#include "character.h"

static const float RUN_VELOCITY = 0.1f;
static const float WALK_VELOCITY = 0.05f;


CCharacter::CCharacter() : m_ComboCount(0),m_State(WAIT), m_pCurAct(NULL), m_KeyDownTime(0), m_AttackLength(0)
{
	ZeroMemory( m_pModel, sizeof(m_pModel) );
	m_ObjId = d3d::CreateObjectId();
	m_Dir = Vector3(0,0,-1);
	m_Velocity = 0.f;
	m_ComboTrace[ 0] = NULL;
	m_pTerrain = NULL;
	m_pAttackFocus = NULL;
	m_nFocusTime = 0;

}


CCharacter::~CCharacter() 
{
	Clear();
}


//-----------------------------------------------------------------------------//
// 몸통모델 로드
//-----------------------------------------------------------------------------//
BOOL CCharacter::LoadBody( char *pFileName )
{
	if( !m_pModel[ BODY] ) m_pModel[ BODY] = new CModel;
	BOOL reval = m_pModel[ BODY]->LoadModel( pFileName );

	m_pModel[ BODY]->SetPos( Vector3(0,0,0) );
	m_Sphere.SetSphere( 50.f, Vector3(0,0,0) );
	m_MinSphere.SetSphere( 10.f, Vector3(0,0,0) );
	m_Life = 300;
	m_pWorldTM = m_pModel[ BODY]->GetWorldTM();

	return reval;
}


//-----------------------------------------------------------------------------//
// 무기모델 로드
//-----------------------------------------------------------------------------//
BOOL CCharacter::LoadWeapon( char *pFileName )
{
	if( !m_pModel[ BODY] ) return FALSE;

	if( !m_pModel[ WEAPON] ) m_pModel[ WEAPON] = new CModel;
	if( m_pModel[ WEAPON]->LoadModel(pFileName) )
	{
		CBoneNode *pbone1 = m_pModel[ BODY]->GetBoneNode( "dummy_weapon" );
		if( pbone1 )
			m_pModel[ WEAPON]->SetLink( m_pModel[ BODY], pbone1 );

		// 무기의 대략적인 크기를 얻는다.
		list<Box*> lst;
		m_pModel[ WEAPON]->GetCollisionBox( &lst );
		if( !lst.empty() )
		{
			Box *pbox = lst.front();
			m_AttackLength = pbox->GetSize() + 10.f; // 무기 길이 + 팔 길이
		}
	}

	return TRUE;
}


//-----------------------------------------------------------------------------//
// 콤보트리 정보를 읽어온다.
//-----------------------------------------------------------------------------//
BOOL CCharacter::LoadCombo( char *pFileName )
{
	ClearCombo();
	m_ComboCount = CFileLoader::LoadCombo( pFileName, m_pCombo );

//	PrintComboTree( m_pCombo, m_ComboCount );

	return TRUE;
}

//-----------------------------------------------------------------------------//
// 콤보 출력
//-----------------------------------------------------------------------------//
void CCharacter::PrintComboTree( SCombo *pCombo[ MAX_COMBO], int ComboSize )
{
	if( !pCombo ) return;

	g_Dbg.Console( "Combo Script\n" );
	for( int i=0; i < ComboSize; ++i )
	{
		g_Dbg.Console( "Combo[%d] state: %x	\n", pCombo[ i]->id, pCombo[ i]->cur_state );
		PrintComboActTree( &pCombo[ i]->act, 1 );
	}
}
void CCharacter::PrintComboActTree( SActInfo *pAct, int Tab )
{
	if( !pAct ) return;
	for( int t=0; t < Tab; ++t )
		g_Dbg.Console( "\t" );

	g_Dbg.Console( "Act[%d] stime: %3d, etime: %3d, ani: %s\n", pAct->id, pAct->stime, pAct->etime, pAct->ani );
	for( int i=0; i < pAct->nextcount; ++i )
	{
		PrintComboActTree( pAct->next[ i], Tab+1 );
	}

}


//-----------------------------------------------------------------------------//
// 외부에서 온 명령들을 이함수에서 처리한다.
//-----------------------------------------------------------------------------//
void CCharacter::Command( SMsg Msg )
{
	switch( Msg.type )
	{
	case MSG_KEYDOWN:
		KeyProc( Msg.lparam, Msg.wparam );
		break;

	case MSG_SETPOS:
		{
			Matrix44 mat;
			Vector3 *pv = (Vector3*)Msg.lparam;
			mat.SetWorld( *pv );
			SetWorldTM( &mat );
		}
		break;

	case MSG_MOVPOS:
		{

		}
		break;
	}

}


//-----------------------------------------------------------------------------//
// 키입력 처리 
// Key : combo.h 에 정의된 키값을 가지며, OR연산 가능하다.
//	     0x01 이면 아무키도 누르지 않았다는 뜻이다.
//-----------------------------------------------------------------------------//
void CCharacter::KeyProc( int Key, int KeyTime )
{
	static int cnt = 0;
	++cnt;
	static SCombo *pcombo = NULL; // debug용
	int elapse = KeyTime - m_KeyDownTime;
//	g_Dbg.Console( "KeyProc %5d: %4x, %4x, %4d\n", cnt, Key, m_PrevKey, elapse );
	if( (KEY_KEY & Key) && ((50 >= elapse) && ((Key == m_PrevKey) || (KEY_BTN & m_PrevKey))) )
	{
		// Skip
//		g_Dbg.Console( "Skip %5d: %4x, %4x, %4d\n", cnt, Key, m_PrevKey, elapse );
		return;
	}

//	g_Dbg.Console( "KeyProc %5d: %6x, %4x, %4x, %4d\n", cnt, m_State, Key, m_PrevKey, elapse );
	SActInfo *pact = FindComboAnimation( m_pCurAct, Key, m_PrevKey, elapse );
	g_Dbg.Console( "ComboTrace: %s\n", m_ComboTrace );
	if( !pact ) return;

	m_KeyDownTime = KeyTime;
	if( KEY_NONE != Key )
		m_PrevKey = Key;
	Action( Key, KeyTime, elapse, pact );
}


//-----------------------------------------------------------------------------//
// 인자로 넘어온 pAct값대로 행동한다.
// Key : 입력키
// KeyTime : 키를 누른 시간 millisecond단위
// ElapseTime : 전 키입력 상태로 부터의 경과시간 millisecond단위
//-----------------------------------------------------------------------------//
void CCharacter::Action( int Key, int KeyTime, int ElapseTime, SActInfo *pAct )
{
	if( !pAct ) return;
	if( pAct->ani[ 0] != NULL && pAct->ani[ 0] != ' ' )
	{
//		g_Dbg.Console( "Action: %s, %x \n", pAct->ani, pAct->btn[0] );
		BOOL flag[ 4][ 3] =
		{
			{ TRUE, FALSE, FALSE },// CONTINUE
			{ FALSE,TRUE,  FALSE },// REPEAT
			{ FALSE,FALSE, FALSE },// INSTANT
			{ FALSE,FALSE, TRUE  },// QUICK ANI
		};
		// continue는 에니메이션타입에 따라 그리고 전의 에니메이션과 같을때만 TRUE가 된다.
		BOOL _continue = flag[ pAct->ani_type][ 0] && ((m_pCurAct)? !strcmp(m_pCurAct->ani, pAct->ani) : FALSE);
		BOOL loop = flag[ pAct->ani_type][ 1];
		BOOL quick = flag[ pAct->ani_type][ 2];
		SetAnimation( pAct->ani, pAct->anitime, _continue, loop, quick );
	}

	m_State = (STATE)pAct->state;
	m_pCurAct = pAct;

	switch( m_State )
	{
	case WAIT:
		break;

	// 이동 상태
	case WALK:
	case DASH:
	case STATE1:
	case STATE4:
		{
			Vector3 dir = Vector3(0,0,0);
			if( KEY_F & Key ) dir.x +=  1.f;
			if( KEY_B & Key ) dir.x += -1.f;
			if( KEY_U & Key ) dir.z +=  1.f;
			if( KEY_D & Key ) dir.z += -1.f;
			dir.Normalize();
			m_pModel[ BODY]->SetDirection( dir );

			m_Dir = dir;
			m_Velocity = (DASH==m_State)? RUN_VELOCITY : WALK_VELOCITY;
		}
		break;

	// 공격 상태
	case ATTACK:
	case STATE2:
		{
			BOOL focusattack = FALSE;
			Vector3 p0, p1, dir;

			// 포커스된 적이 있다면, 포커스된 적이 죽지않았고, 공격범위 안에 있어야 하며, 
			// 전공격 시간과 1초내의 텀이라면 포커스는 그대로 유지된다.
			if( m_pAttackFocus && (g_CurTime < m_nFocusTime+1000) )
			{
				p0 = Vector3( m_pWorldTM->_41, m_pWorldTM->_42, m_pWorldTM->_43 ); 
				p1 = m_pAttackFocus->GetPos();
				float len = sqrt( p0.LengthRoughly(p1) );
				if( (m_AttackLength >= len) && !(DEAD & m_pAttackFocus->m_State) )
					focusattack = TRUE;
			}

			if( focusattack )
			{
				dir = p1 - p0;
				dir.y = 0;
				dir.Normalize();
			}
			else
			{
				m_pAttackFocus = g_Control.GetAttackTarget( this, &dir );
			}

			if( m_pAttackFocus )
			{
				SetDirection( dir );
				m_nFocusTime = g_CurTime;
			}
		}
		break;
	}
}


//-----------------------------------------------------------------------------//
// 현재상태와 키값으로 콤보시작 트리를 찾는다.
//-----------------------------------------------------------------------------//
SCombo* CCharacter::FindCombo( STATE State, int Key, int PrevKey, int ElapseTime )
{
	for( int i=0; i< m_ComboCount; ++i )
	{
			// etime = 0 이면 무한대를 뜻한다. 즉 elapse가 stime보다 크고 etime이 0이면 
			// 어떤 KeyTime의 입력이 되든 받아들인다.
			// etime이 0이 아니면 elapse는 stime+etime 사이값이어야 한다.
			// btn 이 KEY_PREV 일경우 전에 누른 키값과 같다면 받아들인다는 의미다.
		if( (State & m_pCombo[ i]->cur_state) && IsCorrectAction(&m_pCombo[ i]->act,Key,PrevKey,ElapseTime) )
			return m_pCombo[ i];
	}
	return NULL;
}


//-----------------------------------------------------------------------------//
// DestPos 위치로 이동한다.
// DestPos : 목적 위치
// bDash : TRUE=달려가기, FALSE=걷기
//-----------------------------------------------------------------------------//
void CCharacter::Move( const Vector3 &DestPos, BOOL bDash ) // bDash = FALSE
{
	Vector3 curpos = m_pModel[ BODY]->GetPos();
	Vector3 dir = DestPos - curpos;
	float len = dir.Length();
	float v = (bDash)? 0.10f : 0.05f;
	float t = len / v;
	if( 200.f > t ) return;

	STATE state = (bDash)? DASH : WALK;
	BOOL _continue = (m_State == state);
	SetAnimation( (bDash)? "dash":"walk", (int)t, _continue, FALSE, FALSE );

	dir.y = 0;
	dir.Normalize();
	SetDirection( dir );

	m_State = state;
	m_DestPos = DestPos;
	m_Dir = dir;
	m_Velocity = v;

}


//-----------------------------------------------------------------------------//
// 상대방으로 부터 일정한 거리에 도달하면 공격한다.
// vDir : 공격방향
// Key: 키 버튼 
// return value: 
//				true : 공격함
//				false : 쫓는중
//-----------------------------------------------------------------------------//
BOOL CCharacter::Attack( const Vector3& vDir, int Key )
{
	// 목적지에 도착한후 공격한다.
	Vector3 curpos = m_pModel[ BODY]->GetPos();
	if( curpos.EqualEpsilon(3.f, m_DestPos) )
	{
		KeyProc( Key, g_CurTime );
		return TRUE;
	}
	return FALSE;
}


//-----------------------------------------------------------------------------//
// 공격이 성공했을때 호출되는 함수
// 한번의 공격이 여러번 Collision검사 되는것을 막기위해서 공격이 성공된후 
// Collision검사를 막는 플래그값을 설정한다.
//-----------------------------------------------------------------------------//
void CCharacter::AttackSuccess()
{

}


//-----------------------------------------------------------------------------//
// 공격을 받은후 데미지를 입은 에니메이션을 출력하고, 라이프값을 줄이고, 공격받은
// 반대방향으로 밀려난다.
// AttackType : 공격받은 공격속성
//				ATTACK_TYPE { NONE, NORMAL, POWER, LAUNCH, THRUST, EARTH, MORTAL, };
// Damage : 공격받은 데미지량
// Dir : 공격받은 방향
//-----------------------------------------------------------------------------//
void CCharacter::Damage( ATTACK_TYPE AttackType, int Damage, Vector3 *pDir )
{
	int key[] = { KEY_NONE, KEY_DMG1, KEY_DMG2, KEY_DMG3, };
	float slide_dist[] = { 0.f, 1.f, 3.f, 2.f, };

	m_Life -= Damage;
	m_pLifeBar->SetHp( m_Life );
	m_pModel[ BODY]->MovePos( *pDir*slide_dist[ AttackType] );

	// 띄우기 공격에 당했다면 Animate() 함수에서 시간에 따라 높이 계산을 한다.
	const static int launchstate = STATE5 | STATE6;	// 띄우기 상태
	if( LAUNCH == AttackType && !(launchstate & m_State) )
	{
		m_LaunchIncTime = 0;
		m_LaunchVelocity = 200.f;
		m_OffsetY = m_pWorldTM->_42;
	}
	else if( (NORMAL == AttackType || POWER == AttackType) && (launchstate & m_State) )
	{
		m_LaunchIncTime = 0;
		m_LaunchVelocity = 30.f;
		m_OffsetY = m_pWorldTM->_42;
	}

	// 띄워진 상태에서는 HP가 0이 되더라도 Dead 에니메이션을 보여주지 않는다.
	if( (0 >= m_Life) && !(launchstate & m_State) )
	{
		KeyProc( KEY_DEAD1, g_CurTime );
	}
	else
	{
		KeyProc( key[ AttackType], g_CurTime );
	}

}


//-----------------------------------------------------------------------------//
// 에니메이션 찾기
// pCurAct : 현재 행동 상태
// Key : 눌려진 키
// PrevKey : 전에 누른키
// ElapseTime : 전에 키를 누른 시간이후 경과된 시간
//-----------------------------------------------------------------------------//
SActInfo* CCharacter::FindComboAnimation( SActInfo *pCurAct, int Key, int PrevKey, int ElapseTime )
{
	SCombo *pcombo = NULL; // debug용

	// 행동이 설정되어 있지 않다면 루트에서 검색한다.
	SActInfo *pact = NULL;
	if( !pCurAct || (0 == pCurAct->nextcount) )
	{
		pcombo = FindCombo( m_State, Key, PrevKey, ElapseTime );
		if( !pcombo ) 
		{
			strcpy_s( m_ComboTrace, sizeof(m_ComboTrace), "not found" );
			return NULL;
		}
		sprintf_s( m_ComboTrace, sizeof(m_ComboTrace), "%6x <%d>", m_State, pcombo->id );
		pact = &pcombo->act;
	}
	else
	{
		// 다음 행동 찾기
		for( int i=0; i < pCurAct->nextcount; ++i )
		{
			if( IsCorrectAction(pCurAct->next[ i], Key, m_PrevKey, ElapseTime) )
			{
				pact = pCurAct->next[ i];

				char buf[  16];
				sprintf_s( buf, sizeof(buf), "-%d", pact->id );
				strcat_s( m_ComboTrace, sizeof(m_ComboTrace), buf );
//				g_Dbg.Console( "%d %d %d\n", pCurAct->next[ i]->stime, pCurAct->next[ i]->etime, ElapseTime );
				break;
			}
		}
	}

	if( !pact )
	{
/*
		// 못찾으면 버그다 --;;
		// Debugging Code
		g_Dbg.Console( "bug name: %s, state: %d, Key: %x\n", m_Name, m_State, Key );
		if( pcombo )
		{
			g_Dbg.Console( "root: %x, %x, btn: %x, ani: %s\n", 
						pcombo->cur_state, pcombo->next_state, pcombo->act.btn[0], pcombo->act.ani );
		}
		if( m_pCurAct )
		{
			g_Dbg.Console( "next: %d, cur_ani: %s, cur_btn: %x, cur_elapse: %d \n", 
						pCurAct->nextcount, pCurAct->ani, pCurAct->btn[0], ElapseTime );
		}
		else
		{
			g_Dbg.Console( "CurAct: NULL, %d \n", ElapseTime );
		}
/**/
		return NULL;
	}

	return pact;
}


//-----------------------------------------------------------------------------//
// 액션정보 pCurAct값이 Key, PrevKey, ElapseTime 조건에 부합하면 TRUE를 리턴하고 
// 그렇지 않다면 FALSE를 리턴한다.
//-----------------------------------------------------------------------------//
BOOL CCharacter::IsCorrectAction( SActInfo *pCurAct, int Key, int PrevKey, int ElapseTime )
{
	// etime = 0 이면 무한대를 뜻한다. 즉 elapse가 stime보다 크고 etime이 0이면 
	// 어떤 KeyTime의 입력이 되든 받아들인다.
	// etime이 0이 아니면 elapse는 stime+etime 사이값이어야 한다.
	// btn 이 KEY_PREV 일경우 전에 누른 키값과 같다면 받아들인다는 의미다.
	BOOL reval = (pCurAct->stime <= ElapseTime) && 
				 ( ((!pCurAct->etime) || ((pCurAct->stime + pCurAct->etime) >= ElapseTime)) &&			
				    Combo_CompareBtn(pCurAct->btn,Key,PrevKey) );
	return reval;
}


//-----------------------------------------------------------------------------//
// 몸통 에니메이션 로드 (에니메이션은 몸통만 적용된다. ) 
//-----------------------------------------------------------------------------//
BOOL CCharacter::LoadAni( char *pFileName )
{
	if( m_pModel[ BODY] )
		m_pModel[ BODY]->LoadAni( pFileName, 0, FALSE );
	return TRUE;
}


//-----------------------------------------------------------------------------//
// 에니메이션 설정
//-----------------------------------------------------------------------------//
void CCharacter::SetAnimation( int nAniIdx, int nAniTime, BOOL bContinue, BOOL bLoop, BOOL bQuick )
{
	if( m_pModel[ BODY] )
		m_pModel[ BODY]->SetAnimation( nAniIdx, nAniTime, bContinue, bLoop, bQuick );
}
void CCharacter::SetAnimation( char *pAniName, int nAniTime, BOOL bContinue, BOOL bLoop, BOOL bQuick )
{
	if( m_pModel[ BODY] )
		m_pModel[ BODY]->SetAnimation( pAniName, nAniTime, bContinue, bLoop, bQuick );
}


//-----------------------------------------------------------------------------//
// 충돌박스를 CollisionList에 저장한다.
// Group: Character의 그룹, 0=주인공, 1=적
// pCollision: 등록될 충돌리스트
//-----------------------------------------------------------------------------//
void CCharacter::SetCollision( CHARACTER_TYPE Group, CollisionList *pCollision )
{
	// 주인공이 공격한 후 충돌체크하기 위한 Test 번호는 1 이다.
	// 적이 공격한 후 충돌체크하기 위한 Test 번호는 2 이다.

 	// 주인공 무기와 적의 몸통을 같은 test값을 가지게 하고,
	// 적의 무기와 주인공의 몸통과 같은 test값을 가지게 해야한다.
	m_Group = Group;

	m_pLifeBar = new CLifeBar;
	m_pLifeBar->Create( this, m_Life, m_Life, m_Group );

	pCollision->AddObj( Group, this, 0 );
	pCollision->AddObj( this, m_pModel[ BODY], (Group==CHAR_HERO)? 2 : 1 );
	pCollision->AddObj( this, m_pModel[ WEAPON], (Group==CHAR_HERO)? 1 : 2 );

}


//-----------------------------------------------------------------------------//
// 출력
//-----------------------------------------------------------------------------//
void CCharacter::Render()
{
	for( int i=0; i < MAX_MODEL; ++i )
		if( m_pModel[ i] )
			m_pModel[ i]->Render();

	m_pLifeBar->Render();
}


//-----------------------------------------------------------------------------//
// 에니메이션
//-----------------------------------------------------------------------------//
void CCharacter::Animate( int nDelta )
{
/*	if( m_pModel[ BODY] )
	{
		// 에니메이션이 종료되면 Animate() 함수는 FALSE를 리턴한다.
		if( !m_pModel[ BODY]->Animate(nDelta) )
		{
			// KO 에니메이션이 끝난후 처리
			if( (KO & m_State) && (0 >= m_Life) )
				m_State = DEAD;
			else
				KeyProc( KEY_NONE, g_CurTime );
		}
	}

	if( m_pModel[ WEAPON] )
		m_pModel[ WEAPON]->Animate( nDelta );

	m_pLifeBar->Animate( nDelta );

	const static int movstate = STATE1 | STATE4 | WALK | DASH;	// 이동 상태
	const static int launchstate = STATE5 | STATE6;	// 띄우기 상태
	if( movstate & m_State )
	{
		if( m_pTerrain )
		{
			float height = m_pTerrain->GetHeight( m_pWorldTM->_41, m_pWorldTM->_43 );
			int atrib = m_pTerrain->GetAttribute();
//			if( 0 == atrib )// 움직일수 있는 영역
			{
				m_pModel[ BODY]->MovePos( m_Dir * (m_Velocity * nDelta) );
			}
//			else
			{
				// 움직일수 없는 영역에서는 뒤로 밀려난다.
//				m_pModel[ BODY]->MovePos( m_Dir * (-0.4f * nDelta) );
			}

			SetPos( Vector3(m_pWorldTM->_41, height, m_pWorldTM->_43) );
		}
		else
		{
			m_pModel[ BODY]->MovePos( m_Dir * (m_Velocity * nDelta) );
		}
	}
	else if( launchstate & m_State )
	{
		m_LaunchIncTime += nDelta;
		float t = (float)(m_LaunchIncTime) / 1000.f;
		float h = (m_LaunchVelocity * t) + (0.5f * (-300.f * t * t));
		m_pWorldTM->_42 = m_OffsetY + h;
		m_pModel[ BODY]->SetWorldTM( m_pWorldTM );

		float height = m_pTerrain->GetHeight( m_pWorldTM->_41, m_pWorldTM->_43 );
		if( height > m_pWorldTM->_42 )
		{
			m_pWorldTM->_42 = height;
			KeyProc( KEY_KO2, g_CurTime );
		}
	}
/**/
}


void CCharacter::SetTerrain( CTerrain *pTerrain )
{
	m_pTerrain = pTerrain;
}


//-----------------------------------------------------------------------------//
// 초기화, 메모리 소거
//-----------------------------------------------------------------------------//
void CCharacter::Clear()
{
	for( int i=0; i < MAX_MODEL; ++i )
		SAFE_DELETE( m_pModel[ i] );

	SAFE_DELETE( m_pLifeBar );

	ClearCombo();
	m_Dir = Vector3(0,0,-1);
	m_AttackLength = 0;
}


void CCharacter::SetPos( Vector3 Pos )
{
	if( !m_pModel[ BODY] ) return;
	m_pModel[ BODY]->SetPos( Pos );
}

Vector3 CCharacter::GetPos() const
{
	if( !m_pModel[ BODY] ) return Vector3(0,0,0);
	return m_pModel[ BODY]->GetPos();
}


// dir: 라디안 값
void CCharacter::SetDirection( const Vector3 &dir )
{
	if( !m_pModel[ BODY] ) return;
	m_pModel[ BODY]->SetDirection( dir );
}
void CCharacter::SetDirection( const Quaternion& q )
{
	if( !m_pModel[ BODY] ) return;
	m_pModel[ BODY]->SetDirection( q );
}


//-----------------------------------------------------------------------------//
// SetWorldTM
//-----------------------------------------------------------------------------//
void CCharacter::SetWorldTM( Matrix44 *pTM ) 
{
	if( m_pModel[ BODY] )
		m_pModel[ BODY]->SetWorldTM( pTM );
}
Matrix44* CCharacter::GetWorldTM()
{
	return m_pModel[ BODY]->GetWorldTM();
}


//-----------------------------------------------------------------------------//
// MutliplyWorldTM
//-----------------------------------------------------------------------------//
void CCharacter::MutliplyWorldTM( Matrix44 *pTM ) 
{
	if( m_pModel[ BODY] )
		m_pModel[ BODY]->MutliplyWorldTM( pTM );
}

void CCharacter::ClearCombo()
{
	if( 0 < m_ComboCount )
	{
		for( int i=0; i < m_ComboCount; ++i )
		{
			Release_Combo( m_pCombo[ i] );
			m_pCombo[ i] = NULL;
		}
		m_ComboCount = 0;
	}
	m_State = WAIT;
	m_pCurAct = NULL;
}


//-----------------------------------------------------------------------------//
// 충돌테스트 여부를 결정한다.
// 주인공이나 적이 공격할때 충돌테스트를 한다.
// TestNum : 1 = 주인공이 공격한후 충돌체크
//			 2 = 적이 공격한후 충돌체크
//			 3 = 이동시 충돌체크
//-----------------------------------------------------------------------------//
BOOL CCharacter::IsTest( int TestNum )
{
	if( 3 == TestNum ) return TRUE;
	if( DEAD & m_State ) return FALSE;
	if( (CHAR_HERO == m_Group) && 2 == TestNum ) return TRUE;
	if( (CHAR_ENEMY == m_Group) && 1 == TestNum ) return TRUE;

	if( (1 == TestNum) && (CHAR_HERO == m_Group) || (2 == TestNum) && (CHAR_ENEMY == m_Group) )
	{
		const int attack_type = ATTACK | STATE2;
		if( !(attack_type & m_State) ) return FALSE;
		if( !m_pCurAct )
		{
			g_Dbg.Console( "IsTest() Error, %s\n", m_Name );
			return TRUE;
		}

		float frame = (float)m_pModel[ BODY]->GetCurrentFrame();
		float col_s = (float)m_pCurAct->col_stime;
		float col_e = (float)(m_pCurAct->col_stime + m_pCurAct->col_etime);
		if( frame > col_s && frame < col_e ) return TRUE;
	}

	return FALSE;
}


//-----------------------------------------------------------------------------//
// 구의 좌표를 업데이트한다.
//-----------------------------------------------------------------------------//
void CCharacter::UpdateCollisionBox()
{
	Matrix44 *pmat = m_pModel[ BODY]->GetWorldTM();
	m_Sphere.SetCenter( &pmat->GetPosition() );
	m_MinSphere.SetCenter( &pmat->GetPosition() );

	m_pModel[ BODY]->UpdateCollisionBox();
	m_pModel[ WEAPON]->UpdateCollisionBox();
}


void CCharacter::GetCollisionBox( std::list<Box*> *pList )
{
	// 충돌박스를 몸통, 무기모델에서 추가되고, 캐릭터클래스는
	// 구를 리턴한다. (GetSpere)
}


//-----------------------------------------------------------------------------//
// 충돌이 일어나면 이 함수가 호출된다.
// TestNum : 1 = 주인공이 공격한후 충돌체크
//			 2 = 적이 공격한후 충돌체크
// ICollisionable : 충돌한 객체 포인터
//-----------------------------------------------------------------------------//
void CCharacter::Collision( int TestNum, ICollisionable *pObj )
{
   	if( CHAR_HERO == m_Group )
	{
		switch( TestNum )
		{
		case 1:
			break;
		case 2:
			{
				int damage = 0;
				ATTACK_TYPE type = NONE;
				Vector3 dir(0,0,0);
				CCharacter *p = (CCharacter*)pObj;
				if( p->m_pCurAct )
				{
					damage = p->m_pCurAct->damage;
					type = (ATTACK_TYPE)p->m_pCurAct->attack_type;
					dir = GetPos() - p->GetPos();
					dir.Normalize();
				}
				Damage( type, damage, &dir );
			}
			break;
		}
	}
	else if( CHAR_ENEMY == m_Group )
	{
		switch( TestNum )
		{
		case 1:
			{
				int damage = 0;
				ATTACK_TYPE type = NONE;
				Vector3 dir(0,0,0);
				CCharacter *p = (CCharacter*)pObj;
				if( p->m_pCurAct )
				{
					damage = p->m_pCurAct->damage;
					type = (ATTACK_TYPE)p->m_pCurAct->attack_type;
					dir = GetPos() - p->GetPos();
					dir.Normalize();
				}
				Damage( type, damage, &dir );
			}
			break;
		case 2:
			break;
		}
	}

}

