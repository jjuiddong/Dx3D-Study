
#include "global.h"
#include "sound.h"
#include "sound_track.h"
//#include "dbg.h"

#pragma comment(lib, "dsound.lib")
#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "strmiids.lib")



// 전역 Instance
CSound	g_Sound;

//------------------------
// 생성자
//------------------------
CSound::CSound() : m_iPlayID( -1 ), m_iRecord_Count( 0 )
{
	memset( m_pTrackArray, 0, sizeof(IJukeBox*)*MAX_TRACK );
	Clear_Record();
}


//------------------------
// 소멸자
//------------------------
CSound::~CSound()
{
	AllStop();	
	// delete sound data
	//for( int i=0; i < MAX_TRACK; ++i )
	//{
	//	if( m_pTrackArray[ i] )
	//		delete m_pTrackArray[ i];
	//}

	// DirectSound, DirectMusic 해제
	//DSound_Shutdown();
	//DMusic_Shutdown();
}


//-------------------------------------------
// CSound 초기화
//-------------------------------------------
BOOL CSound::Init( HWND hwnd )
{
	//if( !DSound_Init( hwnd ) )
	//	return FALSE;

	//if( !DMusic_Init( hwnd ) )
	//	return FALSE;

	return TRUE;
}


//-------------------------------------------
// Load Sound file
// 확장자 비교후 Load 됨, 만약 확장자가 맞지않다면 실패
// 함수가 성공한다면 index를 리턴한다.
//--------------------------------------------
BOOL CSound::Load( int iID, char *szFileName )
{
	//if( MAX_TRACK <= (unsigned)iID ) return FALSE;
	//if( m_pTrackArray[ iID] ) return FALSE; // 이미 설정되었다면 리턴

	//// 확장자 얻음
	//char szExpend[ 4];
	//int len = strlen( szFileName );
	//strcpy( szExpend, &szFileName[ len-3] );

	//// Load
	//if( !strcmp( szExpend, "mid" ) ) // midi
	//	m_pTrackArray[ iID] = new CTrack_Mid();
	//else if( !strcmp( szExpend, "wav" ) ) // wave
	//	m_pTrackArray[ iID] = new CTrack_Wav();
	//else
	//{
	//	return FALSE; // 지원하지 않는 확장자
	//}

	//// load sound file
	//if( !m_pTrackArray[ iID]->Load(szFileName) )
	//	return FALSE;

	return TRUE;
}


//-----------------------------------------------------------------------------//
// Load adpcm wave file 
//-----------------------------------------------------------------------------//
BOOL CSound::Load_Adpcm( int iID, char *szFileName )
{
	//if( MAX_TRACK <= (unsigned)iID ) return FALSE;
	//if( m_pTrackArray[ iID] ) return FALSE; // 이미 설정되었다면 리턴

	//// adpcm wave
	//m_pTrackArray[ iID] = new CTrack_Adpcm();

	//// load sound file
	//if( !m_pTrackArray[ iID]->Load( szFileName ) )
	//	return FALSE;

	return TRUE;
}


//-----------------------------
// Play Sound
//-----------------------------
BOOL CSound::Play( int iID, BOOL bLoop ) // bLoop = FALSE
{
//	if( (unsigned)iID >= MAX_TRACK ) return FALSE;
//
//	if( m_pTrackArray[ iID] )
//	{
////		if( -1 != m_iPlayID )
////			Stop( m_iPlayID );
//
//		// play sound
//		m_pTrackArray[ iID]->Play( bLoop );
//		m_iPlayID = iID;
//		return TRUE;
//	}

	return FALSE;
}


//------------------------------
// Stop Sound
//------------------------------
BOOL CSound::Stop( int iID )
{
	//if( (unsigned)iID >= MAX_TRACK ) return FALSE;

	//if( m_pTrackArray[ iID] )
	//{
	//	// play sound
	//	m_pTrackArray[ iID]->Stop();
	//	m_iPlayID = -1;
	//	return TRUE;
	//}

	return FALSE;
}


//-------------------------------
// Stop All Sound
//-------------------------------
void CSound::AllStop()
{
	//for( int i=0; i < MAX_TRACK; ++i )
	//{
	//	if( m_pTrackArray[ i] )
	//		m_pTrackArray[ i]->Stop();
	//}
}


//------------------------
// volume Setting
//------------------------
void CSound::Volume( int iID, int iVol )
{
	//if( (unsigned)iID >= MAX_TRACK ) return;
	//if( m_pTrackArray[ iID] )
	//	m_pTrackArray[ iID]->Volume( iVol );
}


//------------------------
// Record 저장
//------------------------
void CSound::Add_Record( int iSound_ID, int iSound_Delay, BOOL bPlay, BOOL bLoop  )
{
	//if( MAX_RECORD <= (unsigned)m_iRecord_Count ) return;

	//int find = -1;
	//for( int i=0; i < MAX_RECORD; ++i )
	//{
	//	if( !m_Record[ i].bUsed )
	//	{
	//		find = i;
	//		break;
	//	}
	//}
	//if( 0 > find ) return;

	//// 저장
	//m_Record[ find].bUsed = TRUE;
	//m_Record[ find].play = bPlay;
	//m_Record[ find].loop = bLoop;
	//m_Record[ find].sound_id = iSound_ID;
	//m_Record[ find].sound_delay = iSound_Delay;

	//// 레코드 갯수증가
	//++m_iRecord_Count;
}


//----------------------
// Record 초기화
//----------------------
void CSound::Clear_Record()
{
	m_iRecord_Count = 0;
	memset( m_Record, 0, sizeof(m_Record) );
}


//-------------------------
// Sound Processor
//-------------------------
void CSound::Proc( int iElaps )
{
	//for( int i=0; i < MAX_TRACK; ++i )
	//{
	//	if( m_pTrackArray[ i] )
	//		m_pTrackArray[ i]->Proc();
	//}
	//if( 0 >= m_iRecord_Count )
	//	return;

	//for( i=0; i < MAX_RECORD; ++i )
	//{
	//	if( m_Record[ i].bUsed )
	//	{
	//		// 일정시간이 지나면 사운드를 Play 하거나 Stop 한다.
	//		m_Record[ i].sound_delay -= iElaps;
	//		if( 0 >= m_Record[ i].sound_delay )
	//		{
	//			if( m_Record[ i].play )
	//				Play( m_Record[ i].sound_id, m_Record[ i].loop );
	//			else
	//				Stop( m_Record[ i].sound_id );

	//			m_Record[ i].bUsed = FALSE;
	//			--m_iRecord_Count;
	//		}
	//	}
	//}
}


//-----------------------------------------------------------------------------//
// Sound On,Off
//-----------------------------------------------------------------------------//
void CSound::SetSoundOn( int iID, BOOL bOn )
{
	//if( (unsigned)iID >= MAX_TRACK ) return;
	//Volume( iID, (bOn)? 100 : 0 );
	//if( m_pTrackArray[ iID] )
	//	m_pTrackArray[ iID]->SoundOn( bOn );
}

