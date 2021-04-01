#if !defined(AFX_OPCENUMERATE_H__66C7D187_11DE_11D4_9201_009027A26C86__INCLUDED_)
#define AFX_OPCENUMERATE_H__66C7D187_11DE_11D4_9201_009027A26C86__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// COpcEnumerateApp:
// See OpcEnumerate.cpp for the implementation of this class
//

class COpcEnumerateApp : public CWinApp
{
public:
	COpcEnumerateApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COpcEnumerateApp)
public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(COpcEnumerateApp)
	// NOTE - the ClassWizard will add and remove member functions here.
	//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OPCENUMERATE_H__66C7D187_11DE_11D4_9201_009027A26C86__INCLUDED_)
