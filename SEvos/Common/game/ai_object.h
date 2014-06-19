//-----------------------------------------------------------------------------//
// 2008-02-28  programer: jaejung ┏(⊙д⊙)┛
// 
//	인공지능을 담당하는 클래스다.
//	자신의 캐릭터를 Control에서 얻어와 레퍼런스로 가지며, 상황에 
//	맞게 CCharacter클래스에게 바로 입력을 가하거나, Control클래스를
//	거쳐 명령을 내린다.
//
//	유한상태머신은 스크립트에서 편집되고, CAIObject는 각 상태에
//	따른 세부적인 작업을 맡는다. 
//	예를들어 Wait상태에서는 위치이동 없이 가만히 있고,
//	Walk상태에서는 목표지역까지 일정한 속도로 움직이게 하며,
//	Attack상태에서는 목표물의 방향에 맞춰 콤보공격이나, 공격자세를
//	취하게 한다.
//
//	CAIObject는 주인공의 Character클래스를 가지며, 상황에 따라
//	주인공의 상태, 위치, 체력을 분석할수 있다.
//
//-----------------------------------------------------------------------------//

#if !defined(__AIOBJECT_H__)
#define __AIOBJECT_H__


// CAIObject
class CControl;
class CCharacter;
class CAIObject
{
public:
	CAIObject();
	virtual ~CAIObject() {}

	enum AISTATE
	{ 
		AI_WAIT=1, AI_WATCH, AI_ATTACK, AI_COMBOATTACK, AI_ATTACKWAIT,
		AI_OUTMOVE, AI_INMOVE, AI_CHASE, AI_RUNAWAY, AI_DEAD 
	};

protected:
	CControl *m_pControl;
	CCharacter *m_pHero;	// 주인공 캐릭터
	BOOL m_bScriptLoad;

public:
	AISTATE m_State;			// 인공지능 상태
	AISTATE m_PrevState;		// 전 상태
	CCharacter *m_pCharacter;	// 캐릭터
	int m_IncTime;				// 누적시간 (Animate 처리되면 초기화된다)
	int m_Elapse;				// 상태경과 시간
	float m_Distance;			// 적과의 거리(주인공과의 거리)
	float m_AttackDistance;		// 공격을 하기위한 최소거리

public:
	BOOL Create( CControl *pControl, CCharacter *pCharacter, char *pAIFileName );
	BOOL Animate( int nDelta );
	void ChangeState( int State );
	void Command( SMsg Msg );

protected:
	void Action( int nDelta );

};

#endif //  __AIOBJECT_H__
