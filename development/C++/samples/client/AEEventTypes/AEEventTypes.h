#if !defined(AFX_AEEVENTTYPES_H__A492A327_1113_11D4_9200_009027A26C86__INCLUDED_)
#define AFX_AEEVENTTYPES_H__A492A327_1113_11D4_9200_009027A26C86__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CAEEventTypesApp:
// See AEEventTypes.cpp for the implementation of this class
//

class CAEEventTypesApp : public CWinApp
{
public:
	CAEEventTypesApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAEEventTypesApp)
public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CAEEventTypesApp)
	// NOTE - the ClassWizard will add and remove member functions here.
	//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_AEEVENTTYPES_H__A492A327_1113_11D4_9200_009027A26C86__INCLUDED_)
