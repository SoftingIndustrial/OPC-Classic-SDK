#include "stdafx.h"

#ifdef OTB_SERVER_AE

#include "ServerAEAddressSpaceElement.h"
#include "ServerAEArea.h"
#include "ServerAESource.h"
#include "SrvApp.h"
#include "OTB.h"

//-----------------------------------------------------------------------------
// ServerAEAddressSpaceElement
//-----------------------------------------------------------------------------

ServerAEAddressSpaceElement::ServerAEAddressSpaceElement(void)
{
	m_userDataOTS = 0;
	m_isBrowsable = TRUE;
}


LONG ServerAEAddressSpaceElement::getAddressSpaceElementData(SOCmnElement* pElement, OTSAddressSpaceElementData* pData)
{
	SOCmnString name;
	name = pElement->getName();
	pData->m_elementType = OT_ADDRESSSPACEELEMENT_TYPE_AE;
	SetOutputStructMemberString(name, &pData->m_name);

	if (pElement->is(SOCMNOBJECT_TYPE_SOURCE))
	{
		pData->m_userData = ((ServerAESource*)pElement)->getOTSUserData();
		pData->m_hasChildren = FALSE;
	}
	else
	{
		pData->m_userData = ((ServerAEArea*)pElement)->getOTSUserData();
		pData->m_hasChildren = TRUE;
	}

	pData->m_isBrowsable = (BYTE)pElement->isBrowsable(NULL);
	pData->m_accessRights = 0;
	pData->m_datatype = VT_EMPTY;
	pData->m_ioMode = OTS_IOMODE_NONE;
	return S_OK;
}


void ServerAEAddressSpaceElement::create(SOCmnElement* pParent, SOCmnElement* pElement)
{
	ServerApp* pApp = ::getServerApp();

	if (pApp->m_callbackFunctions.m_OTSCreateAddressSpaceElement)
	{
		OTSObjectHandle hObject, hParent;
		OTSAddressSpaceElementData data;
		OTSAddressSpaceElementData dataOut;

		if ((pParent) && (pParent->is(SOCMNOBJECT_TYPE_ROOT)))
		{
			pParent = NULL;
		}

		hObject = (DWORD)pElement->getOpcHandle();
		hParent = (DWORD)pParent->getOpcHandle();
		memset(&data, 0, sizeof(OTSAddressSpaceElementData));
		memset(&dataOut, 0, sizeof(OTSAddressSpaceElementData));
		getAddressSpaceElementData(pElement, &data);

		SOCMN_TRY
		{
			pApp->m_callbackFunctions.m_OTSCreateAddressSpaceElement(hParent, hObject, &data, &dataOut);
		}
		SOCMN_CATCH(...)
		{
			_TRACEC(SOCMNTRACE_L_ERR, SOCMNTRACE_G_OPC_SERVER,pElement->getObjTraceId(), (_T("ServerAEAddressSpaceElement::create - Exception occurred in vendor application callback: OTSCreateAddressSpaceElement")));
		}

		if (pElement->is(SOCMNOBJECT_TYPE_SOURCE))
		{
			((ServerAESource*)pElement)->setOTSUserData(dataOut.m_userData);
		}
		else if (pElement->is(SOCMNOBJECT_TYPE_AREA))
		{
			((ServerAEArea*)pElement)->setOTSUserData(dataOut.m_userData);
		}

		FreeOTSAddressSpaceElementData(&data);
		FreeOTSAddressSpaceElementData(&dataOut);
	}
}

void ServerAEAddressSpaceElement::destroy(SOCmnElement* pElement)
{
	ServerApp* pApp = ::getServerApp();

	if (pApp->m_callbackFunctions.m_OTSDestroyAddressSpaceElement)
	{
		OTSObjectData object;
		object.m_objectHandle = (DWORD)pElement->getOpcHandle();

		if (pElement->is(SOCMNOBJECT_TYPE_SOURCE))
		{
			object.m_userData = ((ServerAESource*)pElement)->getOTSUserData();
		}
		else if (pElement->is(SOCMNOBJECT_TYPE_AREA))
		{
			object.m_userData = ((ServerAEArea*)pElement)->getOTSUserData();
		}

		SOCMN_TRY
		{
			pApp->m_callbackFunctions.m_OTSDestroyAddressSpaceElement(object);
		}
		SOCMN_CATCH(...)
		{
			_TRACEC(SOCMNTRACE_L_ERR, SOCMNTRACE_G_OPC_SERVER, pElement->getObjTraceId(), (_T("ServerAEAddressSpaceElement::destroy - Exception occurred in vendor application callback: OTSDestroyAddressSpaceElement")));
		}
	}
}

BOOL ServerAEAddressSpaceElement::isBrowsableI(
	IN SOCmnObject* /* pServer */)
{
	return (BOOL)m_isBrowsable;
}

#endif // OTB_SERVER_AE
