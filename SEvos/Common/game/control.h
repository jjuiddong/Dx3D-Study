//-----------------------------------------------------------------------------//
// 2008-02-13  programer: jaejung ┏(⊙д⊙)┛
// 
//-----------------------------------------------------------------------------//

#if !defined(__CONTROL_H__)
#define __CONTROL_H__


// 게임에서 사용되는 메세지를 정의한다.
enum MSG_TYPE
{
	MSG_CONTROL,	// Control에서 처리될 메세지, lparam=Msg Type
	MSG_KEYDOWN,	// lparam = key, wparam = 키를 누른시간
	MSG_SETPOS,		// lparam = vector3* (pointer를 인자로 하기때문에 지역변수를 할당해서는 안된다.)
	MSG_MOVPOS,		// lparam = vector3* (pointer를 인자로 하기때문에 지역변수를 할당해서는 안된다.)
					// wparam = 1 = 걷기
					//			2 = 뛰기

};

enum CHARACTER_TYPE
{
	CHAR_HERO, 
	CHAR_ENEMY,
};

typedef struct _tagMsg
{
	MSG_TYPE type;
	char *id;			// 수신자의 아이디, NULL=hero, name=character
	int lparam;
	int wparam;

	_tagMsg() {}
	_tagMsg( MSG_TYPE t, char *i, int l, int w ):type(t), id(i), lparam(l), wparam(w) {}

} SMsg;


// CControl Class
class CCharacter;
class CAIObject;
class CollisionList;
class CTerrain;
class CControl
{
public:
	CControl();
	virtual ~CControl();

protected:
	// 게임내에서 출력되는 캐릭터, 물체를 표현하기위한 구조체
	typedef struct _tagSObject
	{
		CCharacter *pchar;
		CAIObject *pai;
		_tagSObject() {}
		_tagSObject( CCharacter *pc, CAIObject *pa ):pchar(pc), pai(pa) {}
	} SObject;

	typedef std::list< SObject > CharList;
	typedef std::map< std::string, SObject > CharMap;
	typedef std::queue< SMsg > MsgQ;
	CharMap m_ModelList;	// 화면에 출력되는 오브젝트를 저장한다.
	CharList m_DeadList;	// 제거된 오브젝트를 저장한다.
	CTerrain *m_pTerrain;	// 지형 클래스
	CCharacter *m_pHero;	// 주인공 클래스

	MsgQ m_MsgQ;
	CollisionList *m_pCollision;
	int m_ElapseTime;

public:
	CCharacter* AddCharacter( CHARACTER_TYPE Type, char *pName, char *pModelFile, 
				  		      char *pAniFile, char *pWeaponFile, char *pComboFile, char *pAIFile=NULL );
	CCharacter* GetCharacter( char *pCharacterName );
	CAIObject* GetAI( char *pCharacterName );
	CTerrain* GetTerrain() const { return m_pTerrain; }
	BOOL LoadTerrain( char *pFileName );
	void Command( SMsg &Msg );
	void Animate( int nDelta );
	void Render();
	void Clear();

	CCharacter* GetAttackTarget( CCharacter *pCharacter, Vector3 *pReval );

protected:
	void MsgProc();

	// 집단 인공지능에 관련된 처리
	void AIGroupProc( int nDelta );

};
extern CControl g_Control;

#endif // __CONTROL_H__
