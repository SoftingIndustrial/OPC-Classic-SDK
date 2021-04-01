#if !defined(AFX_STDAFX_H__66C7D18B_11DE_11D4_9201_009027A26C86__INCLUDED_)
#define AFX_STDAFX_H__66C7D18B_11DE_11D4_9201_009027A26C86__INCLUDED_

#if _MSC_VER >= 1800
#define WINVER 0x0501
#elif _MSC_VER >= 1500
#define WINVER 0x0500
#elif _MSC_VER > 1300
#define WINVER 0x0400
#endif

#define VC_EXTRALEAN        // Exclude rarely-used stuff from Windows headers

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#if _MSC_VER > 1100
#include <afxdtctl.h>       // MFC support for Internet Explorer 4 Common Controls
#endif // _MSC_VER > 1100
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>         // MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__66C7D18B_11DE_11D4_9201_009027A26C86__INCLUDED_)
