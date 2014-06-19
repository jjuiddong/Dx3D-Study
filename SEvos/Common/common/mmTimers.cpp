
#include <stdafx.h>
#include <windows.h>
#include "mmTimers.h"

using namespace std;


void CALLBACK CMMTimer_TimerProc(UINT uTimerID, UINT uMsg, DWORD dwUser, DWORD dw1, DWORD dw2)
{
	CMMTimer *pTimer = (CMMTimer*)dwUser;

	int nId = -1;
	map<int,int>::iterator itor = pTimer->m_TimerMap.begin();
	while( pTimer->m_TimerMap.end() != itor )
	{
		if( uTimerID == itor->second )
		{
			nId = itor->first;
			break;
		}
		++itor;
	}
	if( -1 == nId )
		return;

	// 함수 호출
	(*pTimer->m_lpTimeProc)( nId, pTimer->m_dwParam );
}


//-----------------------------------------------------------------------------//
// 생성자
//-----------------------------------------------------------------------------//
CMMTimer::CMMTimer():
m_nTimerRes(0), m_lpTimeProc(NULL), m_dwParam(0)
{
	TIMECAPS tc;
	if( TIMERR_NOERROR == timeGetDevCaps(&tc,sizeof(TIMECAPS)) )
	{
		m_nTimerRes = min( max(tc.wPeriodMin,1), tc.wPeriodMax );
		timeBeginPeriod( m_nTimerRes );
	}
}

CMMTimer::CMMTimer( LPMMTIMECALLBACK lpTimeProc, DWORD dwParam ) : m_nTimerRes(0)
{
	Create( lpTimeProc, dwParam );

	TIMECAPS tc;
	if( TIMERR_NOERROR == timeGetDevCaps(&tc,sizeof(TIMECAPS)) )
	{
		m_nTimerRes = min( max(tc.wPeriodMin,1), tc.wPeriodMax );
		timeBeginPeriod( m_nTimerRes );
	}
}


//-----------------------------------------------------------------------------//
// 
//-----------------------------------------------------------------------------//
CMMTimer::~CMMTimer()
{
	StopAllTimer();
	if( 0 != m_nTimerRes )
	{
		timeEndPeriod( m_nTimerRes );
		m_nTimerRes = 0;
	}
}


//-----------------------------------------------------------------------------//
// 
//-----------------------------------------------------------------------------//
void CMMTimer::Create( LPMMTIMECALLBACK lpTimeProc, DWORD dwParam )
{
	m_lpTimeProc = lpTimeProc;
	m_dwParam = dwParam;

}


//-----------------------------------------------------------------------------//
// Timer 시작
//-----------------------------------------------------------------------------//
BOOL CMMTimer::SetTimer( int nTimerId, UINT uElapse, BOOL bOneShot ) // bOneShot = FALSE
{
	map<int,int>::iterator itor = m_TimerMap.find( nTimerId );
	if( m_TimerMap.end() != itor )
		KillTimer( nTimerId );

	bool res = FALSE;
	MMRESULT result = timeSetEvent( uElapse, m_nTimerRes, CMMTimer_TimerProc, (DWORD)this, bOneShot? TIME_ONESHOT : TIME_PERIODIC );
	if( NULL != result )
	{
		m_TimerMap.insert( map<int,int>::value_type(nTimerId, result) );
		res = TRUE;
	}

	return res;
}


//-----------------------------------------------------------------------------//
// Timer 멈추기
//-----------------------------------------------------------------------------//
BOOL CMMTimer::KillTimer( int nTimerId )
{
	map<int,int>::iterator itor = m_TimerMap.find( nTimerId );
	if( m_TimerMap.end() == itor ) return FALSE;
	int nId = itor->second;

	MMRESULT result = timeKillEvent( nId );
	m_TimerMap.erase( itor );

	return (TIMERR_NOERROR == result);
}


//-----------------------------------------------------------------------------//
// Timer 모두 초기화
//-----------------------------------------------------------------------------//
void CMMTimer::StopAllTimer()
{
	map<int,int>::iterator itor = m_TimerMap.begin();
	while( m_TimerMap.end() != itor )
	{
		timeKillEvent( itor->second );
		++itor;
	}
	m_TimerMap.clear();
}
