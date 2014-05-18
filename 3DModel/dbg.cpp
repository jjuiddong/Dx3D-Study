
//#include "stdafx.h"
#include "dbg.h"
#include "dbgsymengine.h"
#include "dbgpcinfo.h"


using namespace std;

static char *g_DbgKey[] = 
{
	"DEBUGKEY", // 절대 바꾸지 말것
	"1",	// 절대 바꾸지 말것
	"0",
	"1",	// 절대 바꾸지 말것
};


// 함수 argument 처리 매크로
#define VALISTPARSE(recv,src) {\
								va_list marker;\
								va_start(marker, src);\
								vsprintf(recv, src, marker);\
								va_end(marker); }

CDbg g_Dbg;
CDbg::CDbg():
m_nOutputType(DBGLIB_NONE),
m_nLogID(-1),
m_nDbgDrawCursor(0),
m_pDbgDrawAgent(NULL)
{
	// 실행파일을 수정해서 디버깅가능하게 한다.
	if( 0 != atoi(g_DbgKey[2]) )
		m_nOutputType = atoi(g_DbgKey[2]);

	if( DBGLIB_CONSOL & m_nOutputType )
		::AllocConsole();
	else		
	  ::FreeConsole();

	Console( "%s", g_DbgKey[ 2] );
}

CDbg::~CDbg() 
{
	LogFileItor itor = m_LogFileMap.begin();
	while( m_LogFileMap.end() != itor )
	{
		delete (*itor).second;
		itor = m_LogFileMap.erase( itor );
	}

	ClearTrackingValue();
}


//-----------------------------------------------------------------------------//
// Name: 클래스 생성
// Desc: 
// Date: (이재정 2003-11-18 10:55)
// Update: 
//-----------------------------------------------------------------------------//
BOOL CDbg::Create( int nOutputType, int nLogID, IDbgDrawAgent *pDbgDrawAgent ) // nLogID=-1, pDbgDrawAgent=NULL
{
	SetOutputType( nOutputType, nLogID );
	SetDrawAgent( pDbgDrawAgent );

	return TRUE;
}


//-----------------------------------------------------------------------------//
// Name: 
// Desc: 
// Date: (이재정 2003-10-16 18:10)
// Update: 
//-----------------------------------------------------------------------------//
void CDbg::SetOutputType( int nOutputType, int nLogID )
{
	m_nOutputType = nOutputType;
	m_nLogID = nLogID;

	// 실행파일을 수정해서 디버깅가능하게 한다.
	if( 0 != atoi(g_DbgKey[2]) )
		m_nOutputType = atoi(g_DbgKey[2]);

	if( DBGLIB_CONSOL & m_nOutputType )
		::AllocConsole();
	else
		::FreeConsole();
}


//-----------------------------------------------------------------------------//
// Name: 
// Desc: OutputType에 따라 출력한다.
// Date: (이재정 2003-10-16 17:32)
// Update: 
//-----------------------------------------------------------------------------//
void CDbg::Print( char *szMsg, ... )
{
	if( DBGLIB_NONE == m_nOutputType ) return;

	char szbuf[256];
	VALISTPARSE( szbuf, szMsg );
	DbgPrint( m_nOutputType, m_nLogID, szbuf );
}


//-----------------------------------------------------------------------------//
// Name: 
// Desc: Consol 출력
// Date: (이재정 2003-10-16 17:31)
// Update: 
//-----------------------------------------------------------------------------//
void CDbg::Console( char *szMsg, ... )
{
	char szbuf[256];
	VALISTPARSE( szbuf, szMsg );
	m_ConsolOstream << szbuf;
}


//-----------------------------------------------------------------------------//
// Name: 
// Desc: File 출력
// Date: (이재정 2003-10-16 17:31)
// Update: 
//-----------------------------------------------------------------------------//
void CDbg::Log( int nLogID, char *szMsg, ... )
{
	dfileostream *pStream = FindLogFile( nLogID );
	if( !pStream ) return;

	char szbuf[256];
	VALISTPARSE( szbuf, szMsg );
	*pStream << szbuf;
}


//-----------------------------------------------------------------------------//
// Name: 
// Desc: Output창 출력
// Date: (이재정 2003-10-16 17:31)
// Update: 
//-----------------------------------------------------------------------------//
void CDbg::Output( char *szMsg, ... )
{
	char szbuf[256];
	VALISTPARSE( szbuf, szMsg );
	m_OutputOstream << szbuf;
}


//-----------------------------------------------------------------------------//
// Name: 
// Desc: 메세지박스 출력
// Date: (이재정 2003-10-16 17:32)
// Update: 
//-----------------------------------------------------------------------------//
void CDbg::MessageBox( char *szMsg, ... )
{
	char szbuf[256];
	VALISTPARSE( szbuf, szMsg );
	m_MessageOstream << szbuf;	
}


//-----------------------------------------------------------------------------//
// Name: 
// Desc: LogFile 생성
// Date: (이재정 2003-10-16 17:33)
// Update: 
//-----------------------------------------------------------------------------//
BOOL CDbg::CreateLogFile( int nLogID, char *szFileName )
{
	LogFileItor itor = m_LogFileMap.find( nLogID );
	if( m_LogFileMap.end() != itor ) return FALSE;

	dfileostream *ostream = new dfileostream( szFileName );
	m_LogFileMap.insert( LogFileMap::value_type(nLogID, ostream) );
	return TRUE;
}



//-----------------------------------------------------------------------------//
// Name: 출력 interface 설정
// Desc: 
// Date: (이재정 2003-10-28 1:46)
// Update : 
//-----------------------------------------------------------------------------//
BOOL CDbg::SetDrawAgent( IDbgDrawAgent *pDbgDrawAgent )
{
	if( !pDbgDrawAgent ) return FALSE;
	m_pDbgDrawAgent = pDbgDrawAgent;
	return TRUE;
}


//-----------------------------------------------------------------------------//
// Name: LogFile 찾기
// Desc: 
// Date: (이재정 2003-10-16 17:49)
// Update: 
//-----------------------------------------------------------------------------//
dfileostream* CDbg::FindLogFile( int nLogID )
{
	LogFileItor itor = m_LogFileMap.find( nLogID );
	if( m_LogFileMap.end() == itor ) return NULL;
	return (*itor).second;
}


//-----------------------------------------------------------------------------//
// Name: nOutputType에 따라 출력한다.
// Desc: 
// Date: (이재정 2003-11-10 0:27)
// Update : 
//-----------------------------------------------------------------------------//
void CDbg::DbgPrint( int nOutputType, int nLogID, char *szMsg )
{
	if( DBGLIB_CONSOL & nOutputType )
		m_ConsolOstream << szMsg;
	if( DBGLIB_OUTPUT & nOutputType )
		m_OutputOstream << szMsg;
	if( DBGLIB_MESSAGE & nOutputType )
		m_MessageOstream << szMsg;
	if( DBGLIB_LOG & nOutputType )
		if( FindLogFile(nLogID) )
			*FindLogFile(nLogID) << szMsg;
}


//-----------------------------------------------------------------------------//
// Name: 변수 추적
// Desc: 
// Date: (이재정 2003-10-17 17:34)
// Update: 
//-----------------------------------------------------------------------------//
void CDbg::TrackingValue( char *szTitle, int *pValue, int nModify, int nLogID ) // nLogID = -1
{
	m_TrackingValue.push_back( new CDbgInt(szTitle,pValue,nModify) );
}
void CDbg::TrackingValue( char *szTitle, float *pValue, float fModify, int nLogID ) // nLogID = -1
{
	m_TrackingValue.push_back( new CDbgFloat(szTitle,pValue,fModify) );
}
void CDbg::TrackingValue( char *szTitle, double *pValue, double dModify, int nLogID ) // nLogID = -1
{
	m_TrackingValue.push_back( new CDbgDouble(szTitle,pValue,dModify) );
}


//-----------------------------------------------------------------------------//
// Name: 스택탐색
// Desc: 
// Date: (이재정 2003-10-17 17:49)
// Update: 
//-----------------------------------------------------------------------------//
void CDbg::StackWalk( CONTEXT *ContextRecord, int nOutputType, int nLogID )	// nLogID = -1
{
	if( DBGLIB_CONSOL & nOutputType )
		dbgsymengine::stack_trace( m_ConsolOstream, ContextRecord );
	if( DBGLIB_OUTPUT & nOutputType )
		dbgsymengine::stack_trace( m_OutputOstream, ContextRecord );
	if( DBGLIB_MESSAGE & nOutputType )
		dbgsymengine::stack_trace( m_MessageOstream, ContextRecord );
	if( DBGLIB_LOG & nOutputType )
		if( FindLogFile(nLogID) )
			dbgsymengine::stack_trace( *FindLogFile(nLogID), ContextRecord );
}


//-----------------------------------------------------------------------------//
// Name: 
// Desc: 
// Date: (이재정 2003-11-10 0:15)
// Update : 
//-----------------------------------------------------------------------------//
void CDbg::PCInfo( int nOutputType, int nLogID ) // nLogID = -1
{
	KSystemInfo info;
	char szPcInfo[ 512] = {0,}, szTemp[ 128] = {0,};

	info.GetOS( szTemp );
	strcat( szPcInfo, szTemp ); strcat( szPcInfo, "\n" );
	info.GetCPUInfo( szTemp );
	strcat( szPcInfo, szTemp ); strcat( szPcInfo, "\n" );
	info.GetVgaInfo( szTemp );
	strcat( szPcInfo, szTemp ); strcat( szPcInfo, "\n" );
	info.GetHddInfo( szTemp );
	strcat( szPcInfo, szTemp ); strcat( szPcInfo, "\n" );
	info.GetIPAddress( szTemp );
	strcat( szPcInfo, szTemp ); strcat( szPcInfo, "\n" );

	DbgPrint( nOutputType, nLogID, szPcInfo );
}


//-----------------------------------------------------------------------------//
// Name: TrackingValue 출력
// Desc: 
// Date: (이재정 2003-11-09 22:19)
// Update : 
//-----------------------------------------------------------------------------//
void CDbg::Draw()
{
	if( !m_pDbgDrawAgent ) return; // 출력 Interface가 설정되지 않았다면 return

	m_pDbgDrawAgent->DrawBegin();
	const int nSize = m_TrackingValue.size();
	for( int i=0; i < nSize; ++i )
	{
		m_pDbgDrawAgent->Draw( m_pDbgDrawAgent->m_X, 
			m_pDbgDrawAgent->m_Y+(m_pDbgDrawAgent->m_nHeight*i),
			(m_nDbgDrawCursor==i), m_TrackingValue[ i]->ConvertString() );
	}
	m_pDbgDrawAgent->DrawEnd();
}


//-----------------------------------------------------------------------------//
// Name: Keyboard 처리
// Desc: 
// Date: (이재정 2003-11-09 22:34)
// Update : 
//-----------------------------------------------------------------------------//
void CDbg::KeyProc( int nKey )
{
	const int nSize = m_TrackingValue.size();
	if( 0 >= nSize ) return; // TrackingValue에 저장된게 없다면 return

	BOOL bDraw = FALSE;
	switch( nKey )
	{
	case VK_ADD:		++(*m_TrackingValue[ m_nDbgDrawCursor]); bDraw = TRUE; break;
	case VK_SUBTRACT:	--(*m_TrackingValue[ m_nDbgDrawCursor]); bDraw = TRUE; break;
	case VK_DOWN:		if( nSize <= ++m_nDbgDrawCursor ) m_nDbgDrawCursor = nSize-1; bDraw = TRUE; break;
	case VK_UP:			if( 0 > --m_nDbgDrawCursor ) m_nDbgDrawCursor = 0; bDraw = TRUE; break;
	}

	if( bDraw )
		Draw();

}


//-----------------------------------------------------------------------------//
// Name: 
// Desc: Trackingvalue 초기화
// Date: (이재정 2003-11-26 12:12)
// Update: 
//-----------------------------------------------------------------------------//
void CDbg::ClearTrackingValue()
{
	for( int i=0; i < m_TrackingValue.size(); ++i )
		delete m_TrackingValue[ i];
	m_TrackingValue.clear();	
}
