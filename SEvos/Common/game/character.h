//-----------------------------------------------------------------------------//
// 2008-02-12  programer: jaejung ┏(⊙д⊙)┛
// 
//	Model 클래스를 가지며, 몸통, 무기, 갑옷등 하나이상의 모델을 관리한다.
//	키조작에 의한 캐릭터 움직임, 충돌처리를 위해서 CollisionList에 객체를
//  등록하고, Control로 부터 메세지를 받아서 기본적인 기능을 처리하는 
//  기능을 한다.
//-----------------------------------------------------------------------------//

#if !defined(__CHARACTER_H__)
#define __CHARACTER_H__


#include "../model/collision.h"


// CCharacter Class
class CModel;
class CLifeBar;
class CCharacter : public ICollisionable
{
public:
	CCharacter();
	virtual ~CCharacter();

protected:
	enum { BODY=0, WEAPON, MAX_MODEL, };

	STATE m_State;
	CHARACTER_TYPE m_Group;
	int m_ObjId;
	char m_Name[ 32];
	CModel *m_pModel[ MAX_MODEL];
	Sphere m_Sphere;
	Sphere m_MinSphere;
	CLifeBar *m_pLifeBar;
	Matrix44 *m_pWorldTM;	// m_pModel[ BODY] 의 월드행렬 포인터를 가르킨다.
	CTerrain *m_pTerrain;

	float m_AttackLength;	// 공격할수 있는 최대범위 (무기에 따라 달라진다.)
	int m_Life;				// 생명치 0이되면 죽는다.
	int m_Level;			// 레벨

	// 콤보 관련 변수
	SCombo *m_pCombo[ MAX_COMBO];
	int m_ComboCount;
	SActInfo *m_pCurAct;
	int m_KeyDownTime;
	int m_PrevKey;			// 전에 누른키정보를 저장한다.
	char m_ComboTrace[ 64];	// 디버깅용

	// 이동 관련 변수
	float m_Velocity;		// 이동 속도 (millisecond 단위)
	Vector3 m_Dir;			// 이동 방향
	Vector3 m_DestPos;		// 이동 목적지 위치

	// 띄우기 관련 변수
	float m_LaunchVelocity;	// 띄우기 가속도
	int m_LaunchIncTime;
	float m_OffsetY;

	// 공격 포커스
	CCharacter *m_pAttackFocus;
	int m_nFocusTime;

public:
	void SetName( char *pName ) { if( pName ) { strcpy_s(m_Name, sizeof(m_Name), pName); } }
	char* GetName() const { return (char*)m_Name; }
	BOOL LoadBody( char *pFileName );
	BOOL LoadWeapon( char *pFileName );
	BOOL LoadCombo( char *pFileName );
	BOOL LoadAni( char *pFileName );
	void SetAnimation( int nAniIdx, int nAniTime, BOOL bContinue, BOOL bLoop, BOOL bQuick );
	void SetAnimation( char *pAniName, int nAniTime, BOOL bContinue, BOOL bLoop, BOOL bQuick );
	void SetCollision( CHARACTER_TYPE Group, CollisionList *pCollision );
	void SetTerrain( CTerrain *pTerrain );
	void Render();
	void Animate( int nDelta );
	void Command( SMsg Msg );
	void KeyProc( int Key, int KeyTime );
	void Action( int Key, int KeyTime, int ElapseTime, SActInfo *pAct );

	void Move( const Vector3& DestPos, BOOL bDash=FALSE );
	BOOL Attack( const Vector3& vDir, int Key );
	void AttackSuccess();
	void Damage( ATTACK_TYPE AttackType, int Damage, Vector3 *pDir );

	BOOL IsDead() { return 0 >= m_Life; }
	const int& GetHP() const { return m_Life; }
	void SetHP( int hp ) { m_Life = hp; }
	const int& GetLevel() const { return m_Level; }
	void SetLevel( int level ) { m_Level = level; }

	STATE GetState() const { return m_State; }
	CHARACTER_TYPE GetCharacterType() const { return m_Group; }
	void SetPos( Vector3 Pos );
	Vector3 GetPos() const; 
	void SetDirection( const Vector3& dir );		// dir: 라디안 값
	void SetDirection( const Quaternion& q );
	const Vector3& GetDirection() const { return m_Dir; }
	const float& GetAttackLength() const { return m_AttackLength; }	
	const Vector3& GetDestPos() const { return m_DestPos; }
	
	void SetWorldTM( Matrix44 *pTM );
	void MutliplyWorldTM( Matrix44 *pTM );
//	void MutliplyBoneTM( int nBoneId, Matrix44 *pTM );
	void Clear();

protected:
	SActInfo* FindComboAnimation( SActInfo *pCurAct, int Key, int PrevKey, int ElapseTime );
	SCombo* FindCombo( STATE State, int Key, int PrevKey, int ElapseTime );
	BOOL IsCorrectAction( SActInfo *pCurAct, int Key, int PrevKey, int ElapseTime );
	void PrintComboTree( SCombo *pCombo[ MAX_COMBO], int ComboSize );
	void PrintComboActTree( SActInfo *pAct, int Tab );

	BOOL IsAct( SActInfo *pAct, int key );
	void ClearCombo();

public:
	// ICollisionable 인터페이스 구현
	virtual BOOL IsTest( int TestNum );
	virtual int GetObjId() { return m_ObjId; }
	virtual Matrix44* GetWorldTM();
	virtual Sphere* GetSphere() { return &m_Sphere; }
	virtual Sphere* GetMinimumSphere() { return &m_MinSphere; }
	virtual void UpdateCollisionBox();
	virtual void GetCollisionBox( std::list<Box*> *pList );
	virtual void Collision( int TestNum, ICollisionable *pObj );

};

#endif // __CHARACTER_H__
