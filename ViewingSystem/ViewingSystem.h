// ViewingSystem.h : main header file for the VIEWINGSYSTEM application
//

#if !defined(AFX_VIEWINGSYSTEM_H__06AA132C_4F18_41E6_BDBA_1787D2BD32C1__INCLUDED_)
#define AFX_VIEWINGSYSTEM_H__06AA132C_4F18_41E6_BDBA_1787D2BD32C1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CViewingSystemApp:
// See ViewingSystem.cpp for the implementation of this class
//

class CViewingSystemApp : public CWinApp
{
public:
	CViewingSystemApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CViewingSystemApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation
	//{{AFX_MSG(CViewingSystemApp)
	afx_msg void OnAppAbout();
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VIEWINGSYSTEM_H__06AA132C_4F18_41E6_BDBA_1787D2BD32C1__INCLUDED_)
