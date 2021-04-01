//-----------------------------------------------------------------------------
//                                                                            |
//                   Softing Industrial Automation GmbH                       |
//                        Richard-Reitzner-Allee 6                            |
//                           85540 Haar, Germany                              |
//                                                                            |
//                 This is a part of the Softing OPC Toolkit                  |
//        Copyright (c) 2005-2020 Softing Industrial Automation GmbH          |
//                           All Rights Reserved                              |
//                                                                            |
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//                           OPC Toolkit - Demo                               |
//                                                                            |
//  Filename    : DlgAbout.cpp                                                 |
//  Version     : 4.47.1                                                      |
//  Date        : 27-July-2020                                                |
//                                                                            |
//  Description : HTML about dialog                                           |
//                                                                            |
//  This code and information is provided "as is" without warranty of         |
//  any kind, either expressed or implied, including but not limited          |
//  to the implied warranties of merchantability and/or fitness for a         |
//  particular purpose.                                                       |
//                                                                            |
//-----------------------------------------------------------------------------

#include "stdafx.h"
#include "DlgAbout.h"
#include "SOVersion.h"


// CDlgAbout dialog

IMPLEMENT_DYNCREATE(CDlgAbout, CDHtmlDialog)

CDlgAbout::CDlgAbout(CWnd* pParent /*=NULL*/)
	: CDHtmlDialog(CDlgAbout::IDD, CDlgAbout::IDH, pParent)
{
	m_version = _T(VERSION_TEXT);
	m_build = _T(BUILD_NUMBER_STR);
	m_softing = _T("Copyright ©2020 Softing Industrial Automation GmbH");
	m_website = _T("http://data-intelligence.softing.com");
	m_updatesRef = _T("http://industrial.softing.com");
}

CDlgAbout::~CDlgAbout()
{
}

void CDlgAbout::DoDataExchange(CDataExchange* pDX)
{
	CDHtmlDialog::DoDataExchange(pDX);
	DDX_DHtml_ElementInnerText(pDX, _T("idVersion"), m_version);
	DDX_DHtml_ElementInnerText(pDX, _T("idBuild"), m_build);
	DDX_DHtml_ElementInnerText(pDX, _T("idSO"), m_softing);
	DDX_DHtml_Anchor_Href(pDX, _T("idUpdatesRef"), m_updatesRef);
	DDX_DHtml_ElementInnerText(pDX, _T("idUpdatesRef"), m_website);
}

BOOL CDlgAbout::OnInitDialog()
{
	CDHtmlDialog::OnInitDialog();
	HICON hIcon;
	hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	this->SetIcon(hIcon, TRUE);
	return TRUE;  // return TRUE  unless you set the focus to a control
}

BEGIN_MESSAGE_MAP(CDlgAbout, CDHtmlDialog)
END_MESSAGE_MAP()

BEGIN_DHTML_EVENT_MAP(CDlgAbout)
DHTML_EVENT_ONCLICK(_T("ButtonOK"), OnButtonOK)
END_DHTML_EVENT_MAP()



// CDlgAbout message handlers

HRESULT CDlgAbout::OnButtonOK(IHTMLElement* /*pElement*/)
{
	OnOK();
	return S_OK;  // return TRUE  unless you set the focus to a control
}
