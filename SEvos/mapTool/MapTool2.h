// MapTool2.h : main header file for the MAPTOOL2 application
//

#if !defined(AFX_MAPTOOL2_H__22C788C7_1C84_46CF_88FA_0A8CCA682BA2__INCLUDED_)
#define AFX_MAPTOOL2_H__22C788C7_1C84_46CF_88FA_0A8CCA682BA2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CMapTool2App:
// See MapTool2.cpp for the implementation of this class
//

class CMapTool2App : public CWinApp
{
public:
	CMapTool2App();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMapTool2App)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CMapTool2App)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MAPTOOL2_H__22C788C7_1C84_46CF_88FA_0A8CCA682BA2__INCLUDED_)
