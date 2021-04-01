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
//                           OPC Toolkit - OTC                                |
//                                                                            |
//  Filename    : ClientDAItem.cpp                                            |
//  Version     : 4.47.1                                                      |
//  Date        : 27-July-2020                                                |
//                                                                            |
//  Description : OPC Item class                                              |
//                                                                            |
//-----------------------------------------------------------------------------

#include "stdafx.h"

#ifdef OTB_CLIENT_DA

#include "ClientDAItem.h"
#include "ClientJob.h"
#include "OTC.h"

ClientDAItem::ClientDAItem()
{
	m_userData = 0;
	m_storedTargedState = disconnected;
}

void ClientDAItem::onSetObjectState(void)
{
	if (g_callbackFunctions.m_OTCOnStateChange)
	{
		OTCObjectContext objectContext;
		getOTCObjectContext((OTCObjectHandle)this->getOpcHandle(), &objectContext);

		SOCMN_TRY
		{
			g_callbackFunctions.m_OTCOnStateChange(objectContext, getObjectState());
		}
		SOCMN_CATCH(...)
		{
			_TRACE(SOCMNTRACE_L_ERR, SOCMNTRACE_G_OPC_CLIENT, (_T("ClientDAItem::onSetObjectState - Exception occurred in vendor application callback: OTCOnStateChange")));
		}
	}
}

#endif // OTB_CLIENT_DA
