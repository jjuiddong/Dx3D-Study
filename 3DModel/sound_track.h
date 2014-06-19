//-----------------------------------------------------------------------------//
// 2007-12-21  programer: jaejung ┏(⊙д⊙)┛
// Sound Library 코드 정리
//		- CTrack_Wav, CTrack_Mid, CTrack_Adpcm 클래스를 한곳에 모았다.
//-----------------------------------------------------------------------------//

#if !defined(__SOUND_TRACK_H__)
#define __SOUND_TRACK_H__


#include <dshow.h>
#include "sound_lib.h"
/*

//----------------------------------------------------------
// CTrack_Mid, CTrack_Wav, CTrack_Mp3 의 부모가 된다.
//----------------------------------------------------------
//#include "global.h"

class IJukeBox
{
public:
	IJukeBox() { m_bTurnOff = TRUE; }
	virtual ~IJukeBox() {}
	virtual BOOL Play(BOOL bLoop)=0;
	virtual BOOL Stop()=0;
	virtual BOOL Volume( int iVol )=0;
	virtual BOOL Load( char *filename )=0;
	virtual void Proc() {}
	virtual void SoundOn( BOOL bFlag ) { m_bTurnOff = bFlag; }
protected:
	BOOL	m_bTurnOff;
};



//---------------------------------------------------------
// Play adpcm format
//---------------------------------------------------------
class CTrack_Adpcm : public IJukeBox
{
public:
	CTrack_Adpcm();
	virtual ~CTrack_Adpcm();
	virtual BOOL Play( BOOL bLoop );		// Play Sound 
	virtual BOOL Stop();					// Stop Sound
	virtual BOOL Volume( int iVol );		// Set Volume
	virtual BOOL Load( char *filename );	// Load Wave File
	virtual void Proc();					// Loop 연주
	BOOL	Delete_Sound();					// Delete Buffer
protected:
	IGraphBuilder *m_pGraphBuilder;
	IMediaControl *m_pMediaControl;
	IMediaSeeking *m_pMediaSeeking;
	IMediaEvent   *m_media_event;
	IBaseFilter   *m_pSourceCurrent;
	IBasicAudio   *m_basic_audio;
};



//---------------------------------------------------------
// Midi file을 출력하며
// play 하기전 DirectMusic이 초기화 되어있어야 한다.
//---------------------------------------------------------
class CTrack_Mid : public IJukeBox
{
public:
	CTrack_Mid();
	CTrack_Mid( char *filename );
	virtual ~CTrack_Mid();
	virtual BOOL Play( BOOL bLoop );		// Play Midi Sound
	virtual BOOL Stop();					// Stop Midi Sound
	virtual BOOL Volume( int iVol );		// Set Volume
	virtual BOOL Load( char *filename );	// Load Midi File
	int		Status_MIDI();					// Get State
	BOOL	Delete_MIDI();					// Delete Midi Sound
protected:
	DMUSIC_MIDI			m_DMBuffer;			// DirectMusic Buffer 
};



//---------------------------------------------------------
// Wave file을 출력하며
// play 하기전 DirectSound가 초기화 되어있어야 한다.
//---------------------------------------------------------
class CTrack_Wav : public IJukeBox
{
public:
	CTrack_Wav();
	CTrack_Wav( char *filename );
	virtual ~CTrack_Wav();
	virtual BOOL Play( BOOL bLoop );		// Play Sound 
	virtual BOOL Stop();					// Stop Sound
	virtual BOOL Volume( int iVol );		// Set Volume
	virtual BOOL Load( char *filename );	// Load Wave File
	int		Status_Sound();					// 상태 얻기
	BOOL	Delete_Sound();					// Delete Buffer
protected:
	DSOUND_WAV		m_DSBuffer;				// DirectSound Buffer info struct
};

/**/
#endif // __SOUND_TRACK_H__
