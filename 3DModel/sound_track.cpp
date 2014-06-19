
#include "global.h"
#include "sound_track.h"


// volume conver micro...
#define DSVOLUME_TO_DB(volume) ((DWORD)(-30*(100 - volume)))

/*

///////////////////////////////////////////////////////////////////////////////////
//------------------------------
// 생성자 1
//------------------------------
CTrack_Wav::CTrack_Wav()
{
	memset( &m_DSBuffer, 0, sizeof(m_DSBuffer) );
}


//------------------------------
// 생성자 2
//------------------------------
CTrack_Wav::CTrack_Wav( char *filename )
{
	memset( &m_DSBuffer, 0, sizeof(m_DSBuffer) );
	Load( filename );
}


//------------------------------
// 소멸자
//------------------------------
CTrack_Wav::~CTrack_Wav()
{
	// delete wave sound buffer
	Delete_Sound();
}


//------------------------------
// Load Wave File
//------------------------------
BOOL CTrack_Wav::Load( char *filename )
{
	return DSound_Load_WAV( filename, &m_DSBuffer );
}


//---------------------------
// Wav Play
//---------------------------
//BOOL CTrack_Wav::DSound_Play( int flags, int volume, int rate, int pan )
BOOL CTrack_Wav::Play( BOOL bLoop )
{
	if( !m_bTurnOff )
		return TRUE;

	// this function plays a sound, the only parameter that
	// works is the flags which can be 0 to play once or
	// DSBPLAY_LOOPING
	if( m_DSBuffer.dsbuffer )
	{
		// reset position to start
		if( FAILED( m_DSBuffer.dsbuffer->SetCurrentPosition(0) ) )
			return FALSE;

		// play sound
		if( bLoop )
		{
			if( FAILED( m_DSBuffer.dsbuffer->Play(0, 0, DSBPLAY_LOOPING) ) )
				return FALSE;
		}
		else
		{
			if( FAILED( m_DSBuffer.dsbuffer->Play(0, 0, 0) ) )
				return FALSE;
		}

	}	// end if
	// return success
	return TRUE;

} // end DSound_Play


//----------------------------
// Sound Stop
//----------------------------
//BOOL CTrack_Wav::DSound_Stop_Sound()
BOOL CTrack_Wav::Stop()
{
	// this function stops a sound from playing
	if( m_DSBuffer.dsbuffer )
	{
		m_DSBuffer.dsbuffer->Stop();
		m_DSBuffer.dsbuffer->SetCurrentPosition( 0 );
	} // end if
	// return success
	return TRUE;

} // end DSound_Stop_Sound


//-------------------------------
// Sound Set Volume
//-------------------------------
BOOL CTrack_Wav::Volume( int Vol )
{
	// this function sets the volume on a sound 0-100
	if( m_DSBuffer.dsbuffer )
	{
		if( 0 >= Vol )
		{
			if( m_DSBuffer.dsbuffer->SetVolume( -10000 ) != DS_OK )
				return FALSE;
		}
		else
		{
			if( m_DSBuffer.dsbuffer->SetVolume( DSVOLUME_TO_DB(Vol) ) != DS_OK )
				return FALSE;
		}
	}
	// return success
	return TRUE;

} // end DSound_Set_Volume


//---------------------------------
// 현재 Sound의 상태를 얻는다.
//---------------------------------
int CTrack_Wav::Status_Sound()
{
	// this function returns the status of a sound
	if( m_DSBuffer.dsbuffer )
	{
		ULONG status;

		// get the status
		m_DSBuffer.dsbuffer->GetStatus( &status );

		// return the status
		return(status);

	}	// end if
	else // total failure
		return(-1);

} // end DSound_Status_Sound


//----------------------------
// DirectSoundBuffer 해제
//----------------------------
BOOL CTrack_Wav::Delete_Sound()
{
	// this function deletes a single sound and puts it back onto the available list

	// first stop it
	if( !Stop() )
		return FALSE;

	// now delete it
	if( m_DSBuffer.dsbuffer )
	{
		// release the com object
		m_DSBuffer.dsbuffer->Release();
		m_DSBuffer.dsbuffer = NULL;
   
		// return success
		return TRUE;
   } // end if

	// return success
	return TRUE;

} // end DSound_Delete_Sound


///////////////////////////////////////////////////////////////////////////////////

//----------------------------
// 생성자 1
//----------------------------
CTrack_Mid::CTrack_Mid()
{
	memset( &m_DMBuffer, 0, sizeof(m_DMBuffer) );
}


//----------------------------
// 생성자 2
//----------------------------
CTrack_Mid::CTrack_Mid( char *filename )
{
	memset( &m_DMBuffer, 0, sizeof(m_DMBuffer) );
	Load( filename );
}


//----------------------------z
// 소멸자
//----------------------------
CTrack_Mid::~CTrack_Mid()
{
	// delete Midi Sound buffer
	Delete_MIDI();
}


//----------------------------
// Load Midi File
//----------------------------
BOOL CTrack_Mid::Load( char *filename )
{
	return DMusic_Load_MIDI( filename, &m_DMBuffer );
}


//-------------------------------------
// Music play
//-------------------------------------
BOOL CTrack_Mid::Play( BOOL bLoop )
{
	if( !m_bTurnOff )
		return TRUE;

	// play sound based on id
	if( m_DMBuffer.dm_segment && m_DMBuffer.state != MIDI_NULL )
	{

		// if there is an active midi then stop it
		//if( m_iPlayID !=- 1 )
		//	DMusic_Stop( m_iPlayID );

		// play segment and force tracking of state variable
		g_Perf->PlaySegment( m_DMBuffer.dm_segment, 0, 0, &m_DMBuffer.dm_segstate );
		m_DMBuffer.state = MIDI_PLAYING;


		// set the active midi segment
		//m_iPlayID = id;


		return TRUE;
	}  // end if
	else
		return FALSE;

} // end DMusic_Play



//------------------------------------
// Music Stop
//------------------------------------
BOOL CTrack_Mid::Stop()
{
	// stop a midi segment
	if( m_DMBuffer.dm_segment && m_DMBuffer.state != MIDI_NULL )
	{
		// play segment and force tracking of state variable
		g_Perf->Stop( m_DMBuffer.dm_segment, NULL, 0, 0 );
		m_DMBuffer.state = MIDI_STOPPED;

		// reset active id
//		m_iPlayID = -1;

		return TRUE;
   }  // end if
	else
		return FALSE;

} // end DMusic_Stop


//------------------------------------
// Set Volume
//------------------------------------
BOOL CTrack_Mid::Volume( int iVol )
{
	// midi 에서는 Volume 을설정 할수 없음
	return TRUE;
}


//------------------------------------
// 현재 Midi 상태를 반환한다.
//------------------------------------
int CTrack_Mid::Status_MIDI()
{
	// this checks the status of a midi segment

	if( m_DMBuffer.dm_segment && m_DMBuffer.state != MIDI_NULL )
	{
		// get the status and translate to our defines
		if( g_Perf->IsPlaying( m_DMBuffer.dm_segment, NULL) == S_OK )
			m_DMBuffer.state = MIDI_PLAYING;
		else
			m_DMBuffer.state = MIDI_STOPPED;

	   return( m_DMBuffer.state );
	} // end if
	else
		return MIDI_NULL;

} // end DMusic_Status_MIDI


//-------------------------------------
// Midi 파일 삭제
//-------------------------------------
BOOL CTrack_Mid::Delete_MIDI()
{
	// this function deletes one MIDI segment

	// Unload instruments this will cause silence.
	// CloseDown unloads all instruments, so this call is also not
	// strictly necessary.
	if( m_DMBuffer.dm_segment )
	{
		m_DMBuffer.dm_segment->SetParam( GUID_Unload, -1, 0, 0, (void*)g_Perf );

		// Release the segment and set to null
		m_DMBuffer.dm_segment->Release(); 
		m_DMBuffer.dm_segment  = NULL;
		m_DMBuffer.dm_segstate = NULL;
		m_DMBuffer.state       = MIDI_NULL;
	} // end if

	return TRUE;

} // end DMusic_Delete_MIDI


///////////////////////////////////////////////////////////////////////////////////

//-------------------
// 생성자
//-------------------
CTrack_Adpcm::CTrack_Adpcm()
{
	m_pGraphBuilder	= NULL;
	m_pMediaControl	= NULL;
	m_pMediaSeeking	= NULL;
	m_pSourceCurrent= NULL;
	m_media_event   = NULL;
	m_basic_audio   = NULL;
}


//-------------------
// 소멸자
//-------------------
CTrack_Adpcm::~CTrack_Adpcm()
{
	Delete_Sound();
}


//-----------------------------------------------------------------------------//
// Name: 
// Desc: Load Wave File (adpcm)
// Date: (이재정 2002-06-20 1:49)
// Update : 
//-----------------------------------------------------------------------------//
BOOL CTrack_Adpcm::Load( char *filename )
{
	HRESULT hr;

	if (FAILED (hr = CoInitialize(NULL)) )
		return FALSE;

	if (FAILED (hr = CoCreateInstance(CLSID_FilterGraph, NULL,
		CLSCTX_INPROC, IID_IGraphBuilder,
		reinterpret_cast<void **>(&m_pGraphBuilder))) )
		return FALSE;
    
	if (FAILED (m_pGraphBuilder->QueryInterface(IID_IMediaControl,
		reinterpret_cast<void **>(&m_pMediaControl))))
		return FALSE;

	if (FAILED (m_pGraphBuilder->QueryInterface(IID_IMediaSeeking,
		reinterpret_cast<void **>(&m_pMediaSeeking))))
		return FALSE;
	
	if(FAILED(m_pGraphBuilder->QueryInterface (IID_IBasicAudio,
		reinterpret_cast <void **> (&m_basic_audio))))
		return FALSE;

	if(FAILED(m_pGraphBuilder->QueryInterface (IID_IMediaEvent,
		reinterpret_cast <void **> (&m_media_event))))
		return FALSE;

    IPin *pPin = NULL;
    WCHAR wFileName[MAX_PATH];

#ifndef UNICODE
	MultiByteToWideChar(CP_ACP, 0, filename, -1, wFileName, MAX_PATH);
#else
	lstrcpy(wFileName, path);
#endif

	hr = m_pGraphBuilder->AddSourceFilter(wFileName, wFileName, &m_pSourceCurrent);

	if(FAILED(hr = m_pSourceCurrent->FindPin(L"Output", &pPin)))
		return FALSE;

	if(FAILED(hr = m_pMediaControl->Stop()))
		return FALSE;

	if(FAILED(hr = m_pGraphBuilder->Render(pPin)))
		return FALSE;

	SAFE_RELEASE(pPin);
	SAFE_RELEASE(m_pSourceCurrent); 
	return TRUE;

}


//-----------------------------------------------------------------------------//
// Name: 
// Desc: Play Sound 
// Date: (이재정 2002-06-20 2:5)
// Update : 
//-----------------------------------------------------------------------------//
BOOL CTrack_Adpcm::Play( BOOL bLoop )
{
	if( !m_bTurnOff )
		return TRUE;

	if( m_pMediaControl )
		m_pMediaControl->Run();
	return TRUE;
}


//-----------------------------------------------------------------------------//
// Name: 
// Desc: Stop Sound
// Date: (이재정 2002-06-20 2:5)
// Update : 
//-----------------------------------------------------------------------------//
BOOL CTrack_Adpcm::Stop()
{
	LONGLONG llPos = 0;
	if( m_pMediaControl )
		m_pMediaControl->Stop();

	if( m_pMediaSeeking )
		m_pMediaSeeking->SetPositions(&llPos, AM_SEEKING_AbsolutePositioning,&llPos, AM_SEEKING_NoPositioning);
	return TRUE;
}


//-----------------------------------------------------------------------------//
// Name: 
// Desc: Set Volume
// Date: (이재정 2002-06-20 2:5)
// Update : 
//-----------------------------------------------------------------------------//
BOOL CTrack_Adpcm::Volume( int iVol )
{
	if( m_basic_audio )
		m_basic_audio->put_Volume( DSVOLUME_TO_DB(iVol) );
	return TRUE;
}


//-----------------------------------------------------------------------------//
// Name: 
// Desc: Loop 연주
// Date: (이재정 2002-06-20 13:35)
// Update : 
//-----------------------------------------------------------------------------//
void CTrack_Adpcm::Proc()
{
	LONGLONG llPos=0; 
	long lEventCode, lParam1, lParam2;

	if( !m_media_event )
		return;

	if( m_media_event->GetEvent(&lEventCode, &lParam1, &lParam2, 0) == S_OK)
	{
		if(lEventCode == EC_COMPLETE)
		{
			Stop();
			m_pMediaSeeking->SetPositions(&llPos, AM_SEEKING_AbsolutePositioning,&llPos, AM_SEEKING_NoPositioning);
			Play( TRUE );
		}
	}
}


//-----------------------------------------------------------------------------//
// Name: 
// Desc: Delete Buffer
// Date: (이재정 2002-06-20 2:10)
// Update : 
//-----------------------------------------------------------------------------//
BOOL CTrack_Adpcm::Delete_Sound()
{
	if (m_pMediaControl)
        m_pMediaControl->Stop();

    // Release all remaining pointers
    SAFE_RELEASE( m_pSourceCurrent);
    SAFE_RELEASE( m_pMediaSeeking);
    SAFE_RELEASE( m_pMediaControl);
	SAFE_RELEASE( m_media_event);
    SAFE_RELEASE( m_pGraphBuilder);
	SAFE_RELEASE( m_basic_audio);
    // Clean up COM
    CoUninitialize();

	return TRUE;
}
/**/
