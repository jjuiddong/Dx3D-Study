//-----------------------------------------------------------------------------//
// 2001-11-12  programer: jaejung ┏(⊙д⊙)┛
// Sound 관리하는 클래스
// IJukeBox를 배열로 가지며 wave, midi, mp3를 관리한다.
// m_pTrackArray 배열은 MAX_TRACK만큼 Sound File을 가질수 있다.
//
// 2007-12-21
//	- code 정리
//-----------------------------------------------------------------------------//

#if !defined(__SOUND_H__)
#define __SOUND_H__


//#define SOUND_VERSION			"1.02"	// 2002-06-20 // direct show 추가
#define SOUND_VERSION			"1.03"	// 2007-12-21 // code 정리

class IJukeBox;
class CSound
{
	enum
	{
		MAX_TRACK = 256,
		MAX_RECORD = 5,
	};
	typedef struct tagSound_Record
	{
		BOOL bUsed;
		BOOL play;			// 출력한다면 TRUE 멈춘다면 FALSE
		BOOL loop;			// Loop
		int sound_id;		// 출력시킬 Sound Index
		int sound_delay;	// Delay 시간

	} SOUND_RECORD, *LPSOUND_RECORD;

public:
	CSound();
	virtual ~CSound();

	BOOL	Init( HWND hwnd );						// CSound 초기화
	BOOL	Load( int iID, char *szFileName );		// Load Sound file
	BOOL	Load_Adpcm( int iID, char *szFileName );// Load adpcm wave file
	void	Proc( int iElaps );						// Sound Processor
	BOOL	Play( int iID, BOOL bLoop = FALSE );	// Play Sound
	BOOL	Stop( int iID );						// Stop Sound
	void	Volume( int iID, int iVol );			// volume Setting
	void	SetSoundOn( int iID, BOOL bOn );		// Sound On,Off
	void	AllStop();								// Stop All Sound
	void	Add_Record( int iSound_ID, int iSound_Delay, BOOL bPlay, BOOL bLoop );		// Record 저장
	void	Clear_Record();								// Record 초기화

protected:
	int				m_iPlayID;						// 현재 play 중인 sound index
	int				m_iRecord_Count;				// Record 갯수
	IJukeBox		*m_pTrackArray[ MAX_TRACK];		// Sound 포인터 배열
	SOUND_RECORD	m_Record[ MAX_RECORD];			// Record

};

// 전역 Instance
extern CSound	g_Sound;

#endif // __SOUND_H__
