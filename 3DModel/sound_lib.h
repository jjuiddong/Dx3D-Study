//-----------------------------------------------------------------------------//
// 2007-12-21  programer: jaejung ┏(⊙д⊙)┛
// 
// DirectMusic, DirectSound 초기화와 데이타를 로드하는 함수들 모움
// 예전 lib_sound_mid, lib_sound_wav 파일에 있던 소스를 한곳에 모았다.
// 
//-----------------------------------------------------------------------------//

#if !defined(__SOUND_LIB_H__)
#define __SOUND_LIB_H__


//////////////////////////////////////////////////////////////////////////////////
// DirectMusic

#include <dmksctrl.h>
#include <dmusici.h>
#include <dmusicc.h>
#include <dmusicf.h>

// midi object state defines
#define MIDI_NULL				0   // this midi object is not loaded
#define MIDI_LOADED				1   // this midi object is loaded
#define MIDI_PLAYING			2   // this midi object is loaded and playing
#define MIDI_STOPPED			3   // this midi object is loaded, but stopped


// TYPES

// directmusic MIDI segment
typedef struct DMUSIC_MIDI_TYP
{
	IDirectMusicSegment			*dm_segment;  // the directmusic segment
	IDirectMusicSegmentState	*dm_segstate; // the state of the segment
	int							id;           // the id of this segment
	int							state;        // state of midi song

} DMUSIC_MIDI, *LPDMUSIC_MIDI;

// global function
BOOL DMusic_Init( HWND hwnd );									  // DirectMusic 초기화
BOOL DMusic_Shutdown();											  // DirectMusic 해제
BOOL DMusic_Load_MIDI( char *filename, LPDMUSIC_MIDI pDMBuf );	  // Load Midi file

// global variable
extern IDirectMusicPerformance		*g_Perf;  // the directmusic performance manager
extern IDirectMusicLoader			*g_Loader;// the directmusic loader



//////////////////////////////////////////////////////////////////////////////////
// DiectSound

#include <mmsystem.h>
#include <dsound.h>

// digital sound object state defines
#define SOUND_NULL     0 // " "
#define SOUND_LOADED   1
#define SOUND_PLAYING  2
#define SOUND_STOPPED  3

// this holds a single sound
typedef struct DSOUND_WAV_TYP
{
	LPDIRECTSOUNDBUFFER dsbuffer;   // the ds buffer containing the sound
	int state;                      // state of the sound
	int rate;                       // playback rate
	int size;                       // size of sound
	int id;                         // id number of the sound
} DSOUND_WAV, *LPDSOUND_WAV;

// global function
BOOL DSound_Init( HWND hwnd );
BOOL DSound_Shutdown();
BOOL DSound_Load_WAV( char *filename, LPDSOUND_WAV pDSBuf );

// global variable
extern LPDIRECTSOUND		g_lpDS;	// directsound interface pointer


#endif // __SOUND_LIB_H__
