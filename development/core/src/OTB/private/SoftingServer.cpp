#include "stdafx.h"
#include "SOSrvComEnum.h"
#include "ServerDAServer.h"

//-----------------------------------------------------------------------------
// queryInterface
//
// - query interface for own interfaces
//
// returns:
//		E_NOTIMPL     - no query interface implementation for this interface
//		S_OK          - interface delivered
//		E_NOINTERFACE - interface not supported
//
HRESULT ServerDAServer::queryInterface(
	IN REFIID iid,      // interface id
	IN void** object)   // interface pointer
{
#ifdef OTB_BUILD_LIB

	// Softing ActiveX Controls detection of a Softing Server
	// Only include this on a Softing static library build
	if (iid == IID_IEnumString)
	{
		SOSrvComEnumStringObject* strList = NULL;
		SOCMN_ALLOCATE_OBJECT(strList, SOSrvComEnumStringObject)
		strList->m_list.add(_T("{28BF4118-6056-481b-BB04-0330DE5BB7F3}"));
		strList->Reset();
		return strList->QueryInterface(IID_IEnumString, (void**)object);
	}

#endif // OTB_BUILD_LIB
	return E_NOTIMPL;
} // queryInterface

