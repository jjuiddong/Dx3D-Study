
#include "stdafx.h"
#include "mouseinput.h"

//#include "Common.h"

//------------------------
//	Constructor
//------------------------
CMouseInput::CMouseInput()
:	m_nX			( 0 ),
	m_nY			( 0 ),
	m_nWidth		( 0 ),
	m_nHeight		( 0 ),
	m_nMouseZ		( 0 ),
	m_nPrevMouseZ	( 0 ),
	m_dwTicks		( 0 ),
	m_dwClickDelay	( 400 ),
	m_dwDoubleClickDelay ( 400 ),	
	m_bClickL		( FALSE ),
	m_bClickR		( FALSE ),
	m_bDClickL		( FALSE ),
	m_bDClickR		( FALSE ),
	m_bMouseL		( FALSE ),
	m_bMouseR		( FALSE ),
	m_bMouseM		( FALSE ),
	m_bOldMouseL	( FALSE ),
	m_bOldMouseR	( FALSE ),
	m_bOldMouseM	( FALSE )
{
	m_ptMouse.x		= 0;
	m_ptMouse.y		= 0;

	m_ptPrevMouse.x	= 0;
	m_ptPrevMouse.y	= 0;
	
	m_nLClick		= 0;
	m_nRClick		= 0;
} //CMouseInput::CMouseInput

//------------------------
//	Destructor
//------------------------
CMouseInput::~CMouseInput()
{
	//-- NULL --//
} //CMouseInput::~CMouseInput

//------------------------
// Initialize
//------------------------
BOOL CMouseInput::Init( HWND hWnd )
{
	m_hWnd = hWnd;
	return TRUE;
} //CMouseInput::Init

//------------------------
// Set ViewPort
//------------------------
void CMouseInput::SetViewport( int nX, int nY, int nWidth, int nHeight )
{
	// 
	m_nX		= nX;
	m_nY		= nY;
	m_nWidth	= nWidth;
	m_nHeight	= nHeight;

	m_ptMouse.x		= m_nWidth  / 2;
	m_ptMouse.y		= m_nHeight / 2;
	m_ptPrevMouse.x	= m_ptMouse.x;
	m_ptPrevMouse.y	= m_ptMouse.y;	
} //CMouseInput::SetViewport

//------------------------
// Set Mouse Wheel
//------------------------
void CMouseInput::SetMouseWheel( WORD wDelta )
{
	if( wDelta > 32768 )
	{
		m_nPrevMouseZ = -(65536 - wDelta);
	}
	else
	{
		m_nPrevMouseZ = wDelta;
	} //if..else	
} //CMouseInput::SetMouseWheel

//------------------------
// Update
//------------------------
void CMouseInput::Update( DWORD dwTick )
{
	m_dwTicks += dwTick;
	//m_dwClickDelay += dwTick;

	if( m_dwTicks >= m_dwClickDelay )
	{
	//	m_dwTicks = 0;
		m_nLClick = 0;
		m_nRClick = 0;
	} //if

//	m_nEvent = EVENT_NONE;

	// Set Old Mouse State
	m_bOldMouseL = m_bMouseL;
	m_bOldMouseR = m_bMouseR;
	m_bOldMouseM = m_bMouseM;

	m_ptPrevMouse = m_ptMouse;

	GetCursorPos( &m_ptMouse );
	ScreenToClient( m_hWnd, &m_ptMouse );

	m_nMouseZ = m_nPrevMouseZ;
	m_nPrevMouseZ = 0;

	if( !IsViewport() )
	{
		return ;
	} //if

	m_bClickL = FALSE;
	m_bClickR = FALSE;

	// Update L Button
	if( !UpdateL() && !UpdateR() )
	{
		m_nEvent = EVENT_NONE;
	} //if
	
	if( GetAsyncKeyState( 0x04 ) < 0 )	m_bMouseM = TRUE;
	else								m_bMouseM = FALSE;

} //CMouseInput::Update

//------------------------
//
//------------------------
void CMouseInput::PreUpdate()
{
	m_ptPrevMouse = m_ptMouse;

	GetCursorPos( &m_ptMouse );
	ScreenToClient( m_hWnd, &m_ptMouse );
} //CMouseInput::PreUpdate

//------------------------
// Is View Port ( check )
//------------------------
BOOL CMouseInput::IsViewport()
{
	BOOL bFlag = FALSE;

	if( m_ptMouse.x < m_nX )			
	{
		m_ptMouse.x = m_nX;	bFlag = TRUE;
	} //if

	if( m_ptMouse.y < m_nY )	
	{
		m_ptMouse.y = m_nY;	bFlag = TRUE;
	} //if

	if( m_ptMouse.x >= m_nX + m_nWidth )
	{
		m_ptMouse.x = m_nX + m_nWidth  - 1;
		bFlag = TRUE;
	} //of

	if( m_ptMouse.y >= m_nY + m_nHeight )	
	{
		m_ptMouse.y = m_nY + m_nHeight - 1;
		bFlag = TRUE;
	} //if

	return !bFlag;
} //CMouseInput::IsViewport

//------------------------
// Update L Button
//------------------------
BOOL CMouseInput::UpdateL()
{
	// Get 
	if( GetAsyncKeyState( 0x01 ) < 0 )	
	{	// Down
		m_nEvent	= EVENT_MOUSE_L_DOWN;

		if( !m_bMouseL )
		{
			m_bMouseL	= TRUE;
			m_ptClickL	= m_ptMouse;
		} 
		else if( m_ptClickL.x != m_ptMouse.x ||
				 m_ptClickL.y != m_ptMouse.y
				 )
		{
			m_nEvent = EVENT_MOUSE_L_DRAG;			
		} //if..else..

		if( m_nLClick <= 0 )
		{
			m_dwTicks = 0;
		} //if

		return TRUE;
	}
	else
	{	// Up
		if( !m_bMouseL )
		{
			return FALSE;
		} //if

		m_bMouseL = FALSE;

		if( m_nLClick <= 0 )
		{
			m_nLClick = 1;
		} 
		else
		{
			if( m_ptClickL.x == m_ptMouse.x &&
				m_ptClickL.y == m_ptMouse.y
				)
			{
				m_nEvent = EVENT_MOUSE_L_DOUBLECLICK;
			} //if

			m_nLClick = 0;
			return TRUE;
		} //if..else..


		if( m_nEvent == EVENT_MOUSE_L_DRAG )
		{
			m_nEvent = EVENT_MOUSE_L_UP;
		} //if
		else
		{
			m_nEvent = EVENT_MOUSE_CLICK_L;
			m_bClickL = TRUE;
		} //if

		return TRUE;
	} //if

	return FALSE;
} //CMouseInput::UpdateL

//------------------------
// Update R Button
//------------------------
BOOL CMouseInput::UpdateR()
{
	if( GetAsyncKeyState( 0x02 ) < 0 )	
	{
		if( !m_bMouseR )
		{
			m_bMouseR	= TRUE;
			m_ptClickR	= m_ptMouse;
		} //if
	}
	else						
	{
		if( m_ptClickR.x == m_ptMouse.x &&
			m_ptClickR.y == m_ptMouse.y &&
			m_bMouseR	== TRUE
			)
		{
			m_bClickR = TRUE;
		} //if
		
		m_bMouseR = FALSE;
	} //if..else..

	return FALSE;
} //CMouseInput::UpdateR

//------------------------
// Get Mouse 
//------------------------
int CMouseInput::GetMouseX()
{
	return m_ptMouse.x;
} //CMouseInput::GetMouseX

//------------------------
// Get Mouse 
//------------------------
int CMouseInput::GetMouseY()
{
	return m_ptMouse.y;
} //CMouseInput::GetMouseY

//------------------------
// Get Mouse 
//------------------------
int CMouseInput::GetMouseZ()
{
	return m_nMouseZ;
} //CMouseInput::GetMouseZ

//------------------------
// Get Mouse Move
//------------------------
int CMouseInput::GetMouseMoveX()
{
	return m_ptMouse.x - m_ptPrevMouse.x;
} //CMouseInput::GetMouseMoveX

//------------------------
// Get Mouse Move
//------------------------
int CMouseInput::GetMouseMoveY()
{
	return m_ptMouse.y - m_ptPrevMouse.y;
} //CMouseInput::GetMouseMoveY

//------------------------
// Get Mouse 
//------------------------
BOOL CMouseInput::GetMouseL()
{
	if( GetAsyncKeyState( 0x01 ) < 0 )		return TRUE;
	else									return FALSE;
} //CMouseInput::GetMouseL

//------------------------
// Get Mouse 
//------------------------
BOOL CMouseInput::GetMouseR()
{
	if( GetAsyncKeyState( 0x02 ) < 0 )		return TRUE;
	else									return FALSE;
} //CMouseInput::GetMouseR

//------------------------
// Get Mouse 
//------------------------
BOOL CMouseInput::GetMouseM()
{
	if( GetAsyncKeyState( 0x04 ) < 0 )		return TRUE;
	else									return FALSE;
} //CMouseInput::GetMouseM

//------------------------
// Get Mouse L Up
//------------------------
BOOL CMouseInput::GetMouseLUp()
{
	return m_bClickL;
} //CMouseInput::

//------------------------
//  Get Mouse R Up
//------------------------
BOOL CMouseInput::GetMouseRUp()
{
	if( m_bOldMouseR && !m_bMouseR )	return TRUE;
	else								return FALSE;
} //CMouseInput::GetMouseRUp

//------------------------
// Get Mouse Click
//------------------------
BOOL CMouseInput::GetMouseLClick()
{
	return m_bClickL;
} //CMouseInput::GetMouseLClick

//------------------------
// Get Mouse Click
//------------------------
BOOL CMouseInput::GetMouseRClick()
{
	return GetMouseRUp();
} //CMouseInput::GetMouseRClick

//------------------------
// Get Mouse Click
//------------------------
BOOL CMouseInput::GetMouseMClick()
{
	if( m_bOldMouseM && !m_bMouseM )	return TRUE;
	else								return FALSE;
} //CMouseInput::GetMouseMClick

//------------------------
//
//------------------------
int CMouseInput::GetEvent()
{
	return m_nEvent;
} //CMouseInput::GetEvent
