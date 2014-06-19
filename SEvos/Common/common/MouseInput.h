
#ifndef __Common_MouseInput_Header__
#define __Common_MouseInput_Header__

enum
{
	EVENT_NONE = 0,
	EVENT_MOUSE_CLICK_L,
	EVENT_MOUSE_CLICK_R,
	EVENT_MOUSE_MOVE,
	EVENT_MOUSE_L_DOWN,
	EVENT_MOUSE_R_DOWN,
	EVENT_MOUSE_L_UP,
	EVENT_MOUSE_R_UP,
	EVENT_MOUSE_L_DRAG,
	EVENT_MOUSE_R_DRAG,
	EVENT_MOUSE_L_DOUBLECLICK,
	EVENT_MOUSE_R_DOUBLECLICK
}; //enum

//------------------------
//	Mouse Input
//------------------------
class CMouseInput
{
public:
	//MEMORY_UNDER_MGR( CMouseInput );

	// Constructor And Destructor
		CMouseInput();
		~CMouseInput();
	// ~Constructor And Destructor

	// Initialize
	BOOL Init( HWND hWnd );

	// Set ViewPort
	void SetViewport( int nX, int nY, int nWidth, int nHeight );

	// Set Mouse Wheel
	void SetMouseWheel( WORD wDelta );

	// Update
	void Update( DWORD dwTick = 0 );

	//
	void PreUpdate();

	// Update L Button
	BOOL UpdateL();

	// Update R Button
	BOOL UpdateR();

	// Get Event
	int GetEvent();
	
	// Get Mouse 
		int GetMouseX();	// X
		int GetMouseY();	// Y
		int GetMouseZ();	// Wheel Value
	// ~Get Mouse

	// Get Mouse Move
		int GetMouseMoveX();
		int GetMouseMoveY();	
	// ~Get Mouse Move

	// Get Mouse 
		BOOL GetMouseL();
		BOOL GetMouseR();
		BOOL GetMouseM();

		BOOL GetMouseLUp();
		BOOL GetMouseRUp();		
	// ~Get Mouse 

	// Get Mouse Click
		BOOL GetMouseLClick();
		BOOL GetMouseRClick();
		BOOL GetMouseMClick();
	// ~Get Mouse Click


	// Is View Port ( check )
	BOOL IsViewport();
private:

	HWND	m_hWnd;				// Main Window Handle
	
	int		m_nX;				// X
	int		m_nY;				// Y
	int		m_nWidth;			// Screen Width
	int		m_nHeight;			// Screen Height

	int		m_nEvent;

	POINT	m_ptMouse;			// Mouse Point
	int		m_nMouseZ;			// Mouse Z ( Wheel Value )
	POINT	m_ptPrevMouse;		// Prev Mouse Point
	int		m_nPrevMouseZ;		// Prev Mouse Z ( Wheel Value )

	POINT	m_ptClickL;
	POINT	m_ptClickR;

	int		m_nLClick;
	int		m_nRClick;

	BOOL	m_bMouseL;			// Mouse L Button
	BOOL	m_bMouseR;			// Mouse R Button
	BOOL	m_bMouseM;			// Mouse M Button ( Middle , Wheel )
	BOOL	m_bOldMouseL;		// Old Mouse L Button
	BOOL	m_bOldMouseR;		// Old Mouse R Button
	BOOL	m_bOldMouseM;		// Old Mouse M Button

	BOOL	m_bClickL;
	BOOL	m_bClickR;

	BOOL	m_bDClickL;
	BOOL	m_bDClickR;

	DWORD	m_dwTicks;
	DWORD	m_dwClickDelay;
	DWORD	m_dwDoubleClickDelay;	
}; //class CMouseInput

#endif //#ifndef __Common_MouseInput_Header__