//-----------------------------------------------------------------------------//
// 2008-02-09  programer: jaejung ┏(⊙д⊙)┛
// 
// 콤보트리를 저장하고 읽는 기능을 가진다.
//-----------------------------------------------------------------------------//

#if !defined(__COMBO_H__)
#define __COMBO_H__

enum KEY_VALUE
{
	KEY_NONE = 0x01, KEY_A = 0x02, KEY_J = 0x04, KEY_S = 0x08, 
	KEY_U = 0x100, KEY_D = 0x200, KEY_F = 0x400, KEY_B = 0x800,
	KEY_UF = 0x100 | 0x400, KEY_DF = 0x200 | 0x400, KEY_UB = 0x100 | 0x800, KEY_DB = 0x200 | 0x800,

	KEY_PREV = 0x1000, KEY_NOT = 0x2000,
	KEY_DMG1 = 0x4000, KEY_DMG2 = 0x8000, KEY_DMG3 = 0x10000, KEY_KO2 = 0x20000,
	KEY_DEAD1 = 0x100000,

	KEY_DIR = KEY_UF | KEY_DF | KEY_UB | KEY_DB,	// 방향키 버튼
	KEY_BTN = KEY_A | KEY_J | KEY_S,				// 스킬버튼
	KEY_KEY = KEY_DIR | KEY_BTN,					// 방향키 + 스킬 
	KEY_EXT = ~(KEY_DIR | KEY_BTN),	
};

enum STATE 
{ 
	WAIT=0x01, WALK=0x02, DASH=0x04, ATTACK=0x08, JUMP=0x10, KO=0x20, DAMAGE=0x40, DEAD=0x80, 

	// 사용자가 임의로 추가할수 있는 상태값
	STATE1=0x010000, STATE2=0x020000, STATE3=0x040000, STATE4=0x080000, 
	STATE5=0x100000, STATE6=0x200000, STATE7=0x400000, STATE8=0x800000,
};

enum ANIMATION_TYPE { CONTINUE=0, REPEAT=1, INSTANT=2 };
enum ATTACK_TYPE { NONE, NORMAL, POWER, LAUNCH, THRUST, EARTH, MORTAL, };

enum
{
	MAX_COMBO = 64,
	MAX_COMBOACT = 16,
	MAX_BTN = 16,
};

typedef struct _tagActInfo
{
	int id;
	int btn[ MAX_BTN];
	int state;				// STATE 값을 가진다.
	int last_state;			// SActInfo트리에서 가장 말단노드에서만 값이 설정된다.
	int attack_type;		// ATTACK_TYPE 값을 가진다.
	int ani_type;			// ANIMATION_TYPE 값을 가진다.
	int anitime;
	int stime;
	int etime;
	int col_stime;			// 충돌테스트 시작시간
	int col_etime;			// 충돌테스트 간격
	int damage;
	int cancel;
	char ani[ 64];
	int nextcount;
	_tagActInfo *next[ MAX_COMBOACT];

} SActInfo;

typedef struct _tagCombo
{
	int id;
	int cur_state;
	int next_state;
	SActInfo act;

} SCombo;

BOOL Write_ComboData( char *pFileName, int ComboSize, SCombo *pCombo[] );
int Read_ComboData( char *pFileName, SCombo *pCombo[] );
void Release_Combo( SCombo *pCombo );
BOOL Combo_CompareBtn( int SrcBtn[ MAX_BTN], int Btn, int PrevBtn );

#endif // __COMBO_H__
