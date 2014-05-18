
#include "global.h"
#include "control.h"
#include "character.h"
#include "ai_object.h"


float g_flag2 = 12.f;


//-----------------------------------------------------------------------------//
// 캐릭터 인공지능 스크립트에서 호출하는 콜백함수다.
// nFuncID : 함수 타입을 나타낸다.
//		LoadVariable:  class pointer, id (import 클래스 멤버변수 일 경우)
//					   id				 (import 전역 변수)
//		StoreVariable: class pointer, value, id (import 클래스 멤버변수 일 경우)
//					   value, id				(import 전역 변수)
// pProcessor : 스크립트 프로세서, 이 객체를 통해서 인자,리턴값을 설정할수 있다.
//-----------------------------------------------------------------------------//
void CharacterAICallBack( int nFuncID, ns_script::CProcessor *pProcessor )
{
	switch( nFuncID )
	{
	case ns_script::LoadPointer:
		{
			CAIObject *pai = (CAIObject*)pProcessor->GetArgumentClass( 0 );
			int id = (int)pProcessor->GetArgumentFloat( 1 );

			switch( id )
			{
			// global flag
			case 1000:
				pProcessor->SetReturnValue( &g_flag2 );
				break;

			case 200:	// AI::state
				pProcessor->SetReturnValue( &pai->m_State );
				break;
			case 201:	// AI::elapse
				pProcessor->SetReturnValue( &pai->m_Elapse );
				break;
			case 202:	// AI::distance
				pProcessor->SetReturnValue( &pai->m_Distance );
				break;
			case 203:	// AI::hp
				pProcessor->SetReturnValue( (DWORD)&pai->m_pCharacter->GetHP() );
				break;
			case 204:	// AI::attack_distance
				pProcessor->SetReturnValue( &pai->m_AttackDistance );
				break;
			case 205:	// AI::level
				pProcessor->SetReturnValue( (DWORD)&pai->m_pCharacter->GetLevel() );
				break;
			}
		}
		break;

	// AI::ChangeAIState( state )
	case 211:
		{
			CAIObject *pai = (CAIObject*)pProcessor->GetArgumentClass( 0 );
			pai->ChangeState( (int)pProcessor->GetArgumentFloat(1) );
		}
		break;

	// AI::IsDead()
	case 212:
		{
			CAIObject *pai = (CAIObject*)pProcessor->GetArgumentClass( 0 );
			pProcessor->SetReturnValue( pai->m_pCharacter->IsDead() );
		}
		break;

	// GetAI()
	case 1021:
		{
			char *pstr = pProcessor->GetArgumentStr( 0 );
			CAIObject *pai = g_Control.GetAI( pstr );
			pProcessor->SetReturnValue( (DWORD)pai );
		}
		break;
	}

}


CAIObject::CAIObject()
{
	ChangeState( WAIT );

}


//-----------------------------------------------------------------------------//
// 인공지능 명령을 내릴 캐릭터 클래스를 설정하고, 인공지능 스크립트를 로드한다. 
// 스크립트가 없다면 Animate() 함수에서 아무처리 없이 바로 리턴된다.
// pControl: 콘트롤 클래스, 캐릭터클래스에게 명령을 보내려면 콘트롤을 통해야한다.
// pCharacter: 인공지능에 의해서 조작될 캐릭터 클래스
// pAIFileName: 인공지능 스크립트 파일명
//-----------------------------------------------------------------------------//
BOOL CAIObject::Create( CControl *pControl, CCharacter *pCharacter, char *pAIFileName )
{
	m_bScriptLoad = (BOOL)pAIFileName;
	m_pCharacter = pCharacter;
	m_pControl = pControl;
	m_pHero = pControl->GetCharacter( "hero" );
	m_IncTime = 0;
	m_AttackDistance = pCharacter->GetAttackLength();
	m_State = AI_WAIT;
	m_PrevState = AI_WAIT;

	if( m_pHero )
	{
		Vector3 v0 = m_pCharacter->GetPos();
		Vector3 v1 = m_pHero->GetPos();
		m_Distance = sqrtf( v0.LengthRoughly( v1 ) );
	}

	if( pAIFileName )
	{
		// 스크립트 실행 인자값으로 캐릭터이름을 전달하기 때문에 
		// 스크립트에서는 해당 캐릭터 포인터에 접근 할 수 있다.
		g_Scr.Execute( pAIFileName, CharacterAICallBack, pCharacter->GetName() );
	}

	return TRUE;
}


//-----------------------------------------------------------------------------//
// 에니메이션
// 주인공의 인공지능은 처리되지 않는다.
//-----------------------------------------------------------------------------//
BOOL CAIObject::Animate( int nDelta )
{
	if( !m_bScriptLoad ) return TRUE;
	int skipstate = JUMP | KO | DAMAGE | DEAD | STATE5 | STATE6;

	m_IncTime += nDelta;
	m_Elapse += nDelta;
	if( m_IncTime < 200 )
		return TRUE;

	m_IncTime = 0;

	// 주인공과의 거리를 계산한다.
	Vector3 v0 = m_pCharacter->GetPos();
	Vector3 v1 = m_pHero->GetPos();
	m_Distance = sqrtf( v0.LengthRoughly( v1 ) );

	switch( m_State )
	{
	case AI_WAIT:
		// 위치이동 없이 제자리에 서있음
		break;

	case AI_WATCH:	// 경계상태
		{
			// 주인공 주위를 돌아다닌다.
		}
		break;

	case AI_ATTACK:
	case AI_COMBOATTACK:
		{
			if( skipstate & m_pCharacter->GetState() )
				break;

			Vector3 dir = v1 - v0;
			dir.y = 0;
			dir.Normalize();
	
			// 공격한후 바로 AttackWait 상태로 바뀐다.			
			if( m_pCharacter->Attack(dir, KEY_A) )
				ChangeState( AI_ATTACKWAIT );
		}
		break;

	case AI_CHASE:
		{
			if( skipstate & m_pCharacter->GetState() )
				break;

			if( 15.f < m_Distance && 35.f > m_Distance )
			{
				Vector3 v = v1 - v0;
				v.y = 0;
				v.Normalize();
				m_pCharacter->SetDirection( v );
			}
			else
			{
				if( m_PrevState == m_State )
				{
					if( m_pCharacter->GetDestPos().EqualEpsilon(3.f,v0) || (m_Elapse > 4000) )
					{
						Vector3 v = v1 - v0;
						v.y = 0;
						v.Normalize();
						v = v0 + (v * (m_Distance - 30.f));
						m_pCharacter->Move( v );
					}
				}
				else
				{
					Vector3 v = v1 - v0;
					v.y = 0;
					v.Normalize();
					v = v0 + (v * (m_Distance - 30.f));
					m_pCharacter->Move( v );
				}
			}
		}
		break;

	case AI_OUTMOVE:
		{
			if( skipstate & m_pCharacter->GetState() )
				break;

			Vector3 v = v1 - v0;
			v.y = 0;
			v.Normalize();
			v = v0 + (v * (m_Distance - 130.f));
			m_pCharacter->Move( v );
		}
		break;
	
	case AI_INMOVE:
		{
			if( skipstate & m_pCharacter->GetState() )
				break;

			if( 57.f < m_Distance && 63.f > m_Distance )
			{
				Vector3 v = v1 - v0;
				v.y = 0;
				v.Normalize();
				m_pCharacter->SetDirection( v );
			}
			else
			{
				Vector3 v = v1 - v0;
				v.y = 0;
				v.Normalize();
				v = v0 + (v * (m_Distance - 80.f));
				m_pCharacter->Move( v );
			}
		}
		break;

	case AI_RUNAWAY:
		break;

	case AI_DEAD:
		return (m_Elapse < 10000);
	}

	m_PrevState = m_State;
	return TRUE;
}


//-----------------------------------------------------------------------------//
// 인공지능 스크립트에서 상태가 바뀌면 이함수를 호출한다.
// State : 변환될 상태값
//-----------------------------------------------------------------------------//
void CAIObject::ChangeState( int State )
{
	m_PrevState = m_State;
	m_State = (AISTATE)State;

	m_Elapse = 0;
}


//-----------------------------------------------------------------------------//
// 인공지능클래스가 담당하는 캐릭터에 명령이 내려질때 마다
// 이 함수가 호출된다. CControl 클래스에서 호출된다.
//-----------------------------------------------------------------------------//
void CAIObject::Command( SMsg Msg )
{
	switch( Msg.type )
	{
	case MSG_KEYDOWN:
//		KeyProc( Msg.lparam, Msg.wparam );
		break;

	case MSG_SETPOS:
		{
			Vector3 *pv = (Vector3*)Msg.lparam;
			if( m_pHero )
			{
				Vector3 v1 = m_pHero->GetPos();
				m_Distance = sqrtf( pv->LengthRoughly( v1 ) );
			}
		}
		break;

	case MSG_MOVPOS:
		{

		}
		break;
	}

}
