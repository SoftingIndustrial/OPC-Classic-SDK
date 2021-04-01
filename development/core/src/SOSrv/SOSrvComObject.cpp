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
//                           OPC Toolkit - SOSrv                              |
//                                                                            |
//  Filename    : SOSrvComObject.cpp                                          |
//  Version     : 4.47.1                                                      |
//  Date        : 27-July-2020                                                |
//                                                                            |
//  Description : COM base classes for the Server Toolkits                    |
//          - SOSrvComObject: Template for IUnknown functionality             |
//          - SOSrvComBase: Base functions for all COM objects                |
//                                                                            |
//-----------------------------------------------------------------------------

#include "stdafx.h"

#ifdef SOSRV

#ifdef SOFEATURE_DCOM

#include "SOSrvComObject.h"
#include "SOSrvEntry.h"
#include "SOCmnHandleManager.h"


//-----------------------------------------------------------------------------
// SOSrvComBase                                                               |
//-----------------------------------------------------------------------------

SOSrvComBase::SOSrvComBase()
{
	m_opcHandle = PROVIDE_OPC_HANDLE(this);
}

//-----------------------------------------------------------------------------
// ~SOSrvComBase                                                               |
//-----------------------------------------------------------------------------

SOSrvComBase::~SOSrvComBase()
{
	RELEASE_OPC_HANDLE(m_opcHandle);
}

//-----------------------------------------------------------------------------
// ~getOpcHandle                                                               |
//-----------------------------------------------------------------------------

DWORD SOSrvComBase::getOpcHandle() const
{
	return m_opcHandle;
}

//-----------------------------------------------------------------------------
// afterConstructorInit
//
// - all initialisation which may needs virtual methods
//
HRESULT SOSrvComBase::afterConstructorInit(void)
{
	return E_NOTIMPL;
} // afterConstructorInit


//-----------------------------------------------------------------------------
// lockMe
//
// - lock release and access from non COM object
//
void SOSrvComBase::lockMe(void)
{
} // lockMe


//-----------------------------------------------------------------------------
// unlockMe
//
// - unlock release and access from non COM object
//
void SOSrvComBase::unlockMe(void)
{
} // unlockMe


//-----------------------------------------------------------------------------
// disuniteMe
//
// - disunite COM from non COM object
//
void SOSrvComBase::disuniteMe(void)
{} // disuniteMe


//-----------------------------------------------------------------------------
// queryInterface
//
// - query interface overwritable method
//
// returns:
//		E_NOTIMPL     - do standard ATL QueryInterface
//		S_OK          - returned interface
//		E_NOINTERFACE - interface not supported
//
HRESULT SOSrvComBase::queryInterface(
	IN REFIID /* iid */,        // interface id
	IN void** /* object */)     // interface
{
	return E_NOTIMPL;
} // queryInterface


//-----------------------------------------------------------------------------
// lastExternalReleaseWithInternalRefs
//
// - released last external reference with pending internal references
//
// returns:
//		TRUE  - object deleted
//		FALSE - object still exists
//
BOOL SOSrvComBase::lastExternalReleaseWithInternalRefs(void)
{
	return FALSE;
} // lastExternalReleaseWithInternalRefs


//-----------------------------------------------------------------------------
// lastCOMObjectActions
//
// - check if released last object of the server
//   -> end OutProc server via event handler
//
void SOSrvComBase::lastCOMObjectActions(void)
{
	// check if released last object
	SOCmnPointer<SOSrvEntry> engine = ::getSOSrvEntry();

	if (engine.isNotNull())
	{
		SOCmnPointer<SOSrvEventHandler> ev = engine->getEventHandler();
		ev->onReleasedOPCObject();

		if (engine->canUnloadNow() == S_OK)
		{
			ev->onReleasedLastOPCObject();
		}
	}
} // lastCOMObjectActions


//-----------------------------------------------------------------------------
// setProxyBlanket
//
// - set the security blanket for callback interfaces
//
// returns:
//    S_OK - set security blanket
//		error results of QueryInterface, QueryBlanket and SetBlanket
//
HRESULT SOSrvComBase::setProxyBlanket(
	IN IUnknown* pCBIf,         // callback interface
	IN DWORD srvAuthnLevel, // authentification level of CoInitSecurity
	IN DWORD authnLevel,        // authentification level
	IN DWORD impLevel)          // impersonation level
{
	IClientSecurity* pcs;
	HRESULT res;

	if (SUCCEEDED(res = pCBIf->QueryInterface(IID_IClientSecurity, (void**)&pcs)))
	{
		DWORD authScv, authzSvc, orgAuthnLevel, orgImpLevel, cababilities;
		OLECHAR* serverPrincName = NULL;
		void* authInfo = NULL;

		if (SUCCEEDED(res = pcs->QueryBlanket(pCBIf, &authScv, &authzSvc, &serverPrincName, &orgAuthnLevel,
											  &orgImpLevel, &authInfo, &cababilities)))
		{
			if ((srvAuthnLevel == orgAuthnLevel) && ((authnLevel != orgAuthnLevel) || (impLevel != orgImpLevel)))
			{
				// don't change the authenification level if it's not the one of the
				// CoInitSecurity call in the blanket
				// For local servers always PKT_PRIVACY is used and should not be changed
				res = pcs->SetBlanket(pCBIf, authScv, authzSvc, serverPrincName, authnLevel,
									  impLevel, authInfo, cababilities);
			}

			if (serverPrincName)
			{
				CoTaskMemFree(serverPrincName);
			}
		}

		pcs->Release();
	}

	return res;
} // setProxyBlanket

#endif // SOFEATURE_DCOM

#endif // SOSRV
