#if !defined(AFX_AEBROWSE_H__A492A327_1113_11D4_9200_009027A26C86__INCLUDED_)
#define AFX_AEBROWSE_H__A492A327_1113_11D4_9200_009027A26C86__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CAEBrowseApp:
// See AEBrowse.cpp for the implementation of this class
//

class CAEBrowseApp : public CWinApp
{
public:
	CAEBrowseApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAEBrowseApp)
public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CAEBrowseApp)
	// NOTE - the ClassWizard will add and remove member functions here.
	//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}

#endif // !defined(AFX_AEBROWSE_H__A492A327_1113_11D4_9200_009027A26C86__INCLUDED_)
