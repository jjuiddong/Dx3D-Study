
#include "global.h"
#include "sound_lib.h"

#include <mmsystem.h>
#include <direct.h>

///////////////////////////////////////////////////////////////////////////////////////
// DirectMusic
/*
// global variable
IDirectMusicPerformance		*g_Perf = NULL;				// the directmusic performance manager
IDirectMusicLoader			*g_Loader = NULL;			// the directmusic loader

// DMusic_Init 에서 사용됨 (init)
extern LPDIRECTSOUND		g_lpDS;	// directsound interface pointer
// initializes a direct draw struct
#define DD_INIT_STRUCT(ddstruct) { memset(&ddstruct,0,sizeof(ddstruct)); ddstruct.dwSize=sizeof(ddstruct); }
// Convert from multibyte format to Unicode using the following macro:
#define MULTI_TO_WIDE( x,y )  MultiByteToWideChar( CP_ACP,MB_PRECOMPOSED, y,-1,x,_MAX_PATH);


//-------------------------------------------------------------------
// DirectMusic 초기화 
// 만약 DirectSound와 같이 사용한다면 DirectSound를 먼저 생성한후
// IDirectMusicPerformance::Init() 함수의 2번째 인자에 객체를 넘겨주면된다.
//-------------------------------------------------------------------
BOOL DMusic_Init( HWND hwnd )
{
	// set up directmusic
	// initialize COM
	if( FAILED(CoInitialize(NULL)) )
	{
		// Terminate the application.
		return FALSE;
	}	// end if

	// create the performance
	if( FAILED( CoCreateInstance(CLSID_DirectMusicPerformance, NULL, CLSCTX_INPROC, IID_IDirectMusicPerformance, (void**)&g_Perf) ) )
	{
		// return null
		return FALSE;
	}	// end if

	// initialize the performance, check if directsound is on-line if so, use the
	// directsound object, otherwise create a new one
	if( FAILED( g_Perf->Init(NULL, g_lpDS, hwnd) ) )
	{
		return FALSE;// Failure -- performance not initialized
	}	// end if

	// add the port to the performance
	if( FAILED( g_Perf->AddPort(NULL) ) )
	{    
		return FALSE;// Failure -- port not initialized
	} // end if

	// create the loader to load object(s) such as midi file
	if( FAILED( CoCreateInstance(CLSID_DirectMusicLoader, NULL, CLSCTX_INPROC, IID_IDirectMusicLoader, (void**)&g_Loader) ) )
	{
		// error
		return FALSE;
	}	// end if

	// all good baby
	return TRUE;

} // end DMusic_Init


//--------------------------------------
// DirectMusic 해제
//--------------------------------------
BOOL DMusic_Shutdown()
{
	// If there is any music playing, stop it. This is 
	// not really necessary, because the music will stop when
	// the instruments are unloaded or the performance is    
	// closed down.
	if( g_Perf )
		g_Perf->Stop( NULL, NULL, 0, 0 );

	// CloseDown and Release the performance object.
	if( g_Perf )
	{
		g_Perf->CloseDown();
		g_Perf->Release();     
	} // end if

	// Release the loader object.
	if( g_Loader )
		g_Loader->Release();     

	// Release COM
	CoUninitialize(); 

	// return success
	return TRUE;

} // end DMusic_Shutdown


//-------------------------------------------------
// Midi 파일 읽음
// 함수가 성공하면 저장된 index가 return 된다.
//-------------------------------------------------
BOOL DMusic_Load_MIDI( char *filename, LPDMUSIC_MIDI pDMBuf )
{
	// this function loads a midi segment

	if( !g_Loader ) return FALSE;
	if( !g_Perf ) return FALSE;

	DMUS_OBJECTDESC ObjDesc; 
	HRESULT hr;
	IDirectMusicSegment* pSegment = NULL;

	// get current working directory
	char szDir[ _MAX_PATH];
	WCHAR wszDir[ _MAX_PATH]; 

	if( _getcwd( szDir, _MAX_PATH ) == NULL )
	{
		return FALSE;
	} // end if

	MULTI_TO_WIDE(wszDir, szDir);

	// tell the loader were to look for files
	hr = g_Loader->SetSearchDirectory( GUID_DirectMusicAllTypes, wszDir, FALSE );

	if( FAILED(hr) ) 
	{
		return FALSE;
	} // end if

	// convert filename to wide string
	WCHAR wfilename[ _MAX_PATH]; 
	MULTI_TO_WIDE( wfilename, filename );
 
	// setup object description
	DD_INIT_STRUCT( ObjDesc );
	ObjDesc.guidClass = CLSID_DirectMusicSegment;
	wcscpy( ObjDesc.wszFileName, wfilename );
	ObjDesc.dwValidData = DMUS_OBJ_CLASS | DMUS_OBJ_FILENAME;
 
	// load the object and query it for the IDirectMusicSegment interface
	// This is done in a single call to IDirectMusicLoader::GetObject
	// note that loading the object also initializes the tracks and does 
	// everything else necessary to get the MIDI data ready for playback.

	hr = g_Loader->GetObject( &ObjDesc, IID_IDirectMusicSegment, (void**)&pSegment );

	if( FAILED(hr) )
	return FALSE;
 
	// ensure that the segment plays as a standard MIDI file
	// you now need to set a parameter on the band track
	// Use the IDirectMusicSegment::SetParam method and let 
	// DirectMusic find the trackby passing -1 (or 0xFFFFFFFF) in the dwGroupBits method parameter.

	hr = pSegment->SetParam( GUID_StandardMIDIFile, -1, 0, 0, (void*)g_Perf );

	if( FAILED(hr) )
		return FALSE;

	// This step is necessary because DirectMusic handles program changes and 
	// bank selects differently for standard MIDI files than it does for MIDI 
	// content authored specifically for DirectMusic. 
	// The GUID_StandardMIDIFile parameter must be set before the instruments are downloaded. 

	// The next step is to download the instruments. 
	// This is necessary even for playing a simple MIDI file 
	// because the default software synthesizer needs the DLS data 
	// for the General MIDI instrument set
	// If you skip this step, the MIDI file will play silently.
	// Again, you call SetParam on the segment, this time specifying the GUID_Download parameter:

	hr = pSegment->SetParam( GUID_Download, -1, 0, 0, (void*)g_Perf );

	if( FAILED(hr) )
		return FALSE;

	// at this point we have MIDI loaded and a valid object

	pDMBuf->dm_segment  = pSegment;
	pDMBuf->dm_segstate = NULL;
	pDMBuf->state       = MIDI_LOADED;

	// return id
	return TRUE;
 
} // end DMusic_Load_MIDI




////////////////////////////////////////////////////////////////////////////////////////
// DirectSound

// global variable
LPDIRECTSOUND		g_lpDS = NULL;			// directsound interface pointer

//---------------------------
// DirectSound 초기화
//---------------------------
BOOL DSound_Init( HWND hwnd )
{
	// this function initializes the sound system
	static int first_time = 1; // used to track the first time the function
		                       // is entered

	// test for very first time
	if( first_time )
	{		
		// clear everything out
//		memset( m_Sound, 0, sizeof(m_Sound) );
	
		// reset first time
		first_time = 0;

		// create a directsound object
		if( FAILED( DirectSoundCreate(NULL, &g_lpDS, NULL) ) )
			return FALSE;

		// set cooperation level
		if( FAILED( g_lpDS->SetCooperativeLevel(hwnd, DSSCL_NORMAL) ) )
			return FALSE;

	} // end if

	// return sucess
	return TRUE;

} // end DSound_Init



//--------------------------
// DirectSound 해제
//--------------------------
BOOL DSound_Shutdown()
{
	// now release the directsound interface itself
	if( g_lpDS )
		g_lpDS->Release();

	// return success
	return TRUE;

} // end DSound_Shutdown


//---------------------------
// Wave Load
//---------------------------
BOOL DSound_Load_WAV( char *filename, LPDSOUND_WAV pDSBuf )
{
	// this function loads a .wav file, sets up the directsound 
	// buffer and loads the data into memory, the function returns 
	// the id number of the sound

	HMMIO 			hwav;    // handle to wave file
	MMCKINFO		parent,  // parent chunk
					child;   // child chunk
	WAVEFORMATEX    wfmtx;   // wave format structure
	PCMWAVEFORMAT	pcmwf;	 // generic waveformat structure

	DSBUFFERDESC	dsbd;    // directsound description

	UCHAR	*snd_buffer,       // temporary sound buffer to hold voc data
			*audio_ptr_1=NULL, // data ptr to first write buffer 
			*audio_ptr_2=NULL; // data ptr to second write buffer

	DWORD	audio_length_1=0,  // length of first write buffer
			audio_length_2=0;  // length of second write buffer

	// set up chunk info structure
	parent.ckid 	    = (FOURCC)0;
	parent.cksize 	    = 0;
	parent.fccType	    = (FOURCC)0;
	parent.dwDataOffset = 0;
	parent.dwFlags		= 0;

	// copy data
	child = parent;

	// open the WAV file
	if( (hwav = mmioOpen(filename, NULL, MMIO_READ | MMIO_ALLOCBUF)) == NULL )
		return FALSE;

	// descend into the RIFF 
	parent.fccType = mmioFOURCC('W', 'A', 'V', 'E');

	if( mmioDescend( hwav, &parent, NULL, MMIO_FINDRIFF ) )
    {
		// close the file
		mmioClose( hwav, 0 );

		// return error, no wave section
		return FALSE; 	
	}	// end if

	// descend to the WAVEfmt 
	child.ckid = mmioFOURCC('f', 'm', 't', ' ');

	if( mmioDescend( hwav, &child, &parent, 0 ) )
    {
		// close the file
		mmioClose( hwav, 0 );

		// return error, no format section
		return FALSE;
	}	// end if

	// now read the wave format information from file
	if( mmioRead( hwav, (char *)&wfmtx, sizeof(wfmtx) ) != sizeof(wfmtx) )
    {
		// close file
		mmioClose(hwav, 0);

		// return error, no wave format data
		return FALSE;
	}	// end if

	// make sure that the data format is PCM
	if( wfmtx.wFormatTag != WAVE_FORMAT_PCM )
	{
		// close the file
		mmioClose( hwav, 0 );

		// return error, not the right data format
		return FALSE;
	}	// end if

	// now ascend up one level, so we can access data chunk
	if( mmioAscend( hwav, &child, 0 ) )
	{
		// close file
		mmioClose( hwav, 0 );

		// return error, couldn't ascend
		return FALSE;
	} // end if

	// descend to the data chunk 
	child.ckid = mmioFOURCC('d', 'a', 't', 'a');

	if( mmioDescend( hwav, &child, &parent, MMIO_FINDCHUNK ) )
	{
		// close file
		mmioClose( hwav, 0 );

		// return error, no data
		return FALSE;
	} // end if

	// finally!!!! now all we have to do is read the data in and
	// set up the directsound buffer

	// allocate the memory to load sound data
	snd_buffer = (UCHAR *)malloc(child.cksize);

	// read the wave data 
	mmioRead( hwav, (char *)snd_buffer, child.cksize );

	// close the file
	mmioClose( hwav, 0 );

	// set rate and size in data structure
	pDSBuf->rate  = wfmtx.nSamplesPerSec;
	pDSBuf->size  = child.cksize;
	pDSBuf->state = SOUND_LOADED;

	// set up the format data structure
	memset( &pcmwf, 0, sizeof(WAVEFORMATEX) );

	pcmwf.wf.wFormatTag		= WAVE_FORMAT_PCM;
	pcmwf.wf.nChannels		= wfmtx.nChannels;
	pcmwf.wf.nSamplesPerSec	= wfmtx.nSamplesPerSec;
	pcmwf.wf.nBlockAlign	= wfmtx.nBlockAlign;
	pcmwf.wf.nAvgBytesPerSec= wfmtx.nAvgBytesPerSec;
	pcmwf.wBitsPerSample	= wfmtx.wBitsPerSample;	

	// prepare to create sounds buffer
	DD_INIT_STRUCT( dsbd );
	dsbd.dwFlags		= DSBCAPS_CTRLVOLUME | DSBCAPS_STATIC | DSBCAPS_LOCSOFTWARE;
	dsbd.dwBufferBytes	= child.cksize;
	dsbd.lpwfxFormat	= (LPWAVEFORMATEX)&pcmwf;

	// create the sound buffer
	if( FAILED( g_lpDS->CreateSoundBuffer( &dsbd, &pDSBuf->dsbuffer, NULL ) ) )
	{
		// release memory
		free( snd_buffer );

		// return error
		return FALSE;
	} // end if

	// copy data into sound buffer
	if( FAILED( pDSBuf->dsbuffer->Lock( 0, child.cksize,	(void **) &audio_ptr_1, &audio_length_1, 
												(void **)&audio_ptr_2, &audio_length_2, DSBLOCK_FROMWRITECURSOR ) ) )
	{
		return FALSE;
	}

	// copy first section of circular buffer
	memcpy( audio_ptr_1, snd_buffer, audio_length_1 );

	// copy last section of circular buffer
	memcpy( audio_ptr_2, (snd_buffer + audio_length_1), audio_length_2 );

	// unlock the buffer
	if( FAILED( pDSBuf->dsbuffer->Unlock( audio_ptr_1, audio_length_1, audio_ptr_2, audio_length_2 ) ) )
	{
		return FALSE;
	}

	// release the temp buffer
	free( snd_buffer );

	// return id
	return TRUE;

} // end DSound_Load_WAV

/**/