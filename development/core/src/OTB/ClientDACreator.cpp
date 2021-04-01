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
//  Filename    : ClientDACreator.cpp                                         |
//  Version     : 4.47.1                                                      |
//  Date        : 27-July-2020                                                |
//                                                                            |
//  Description : DA Object creator class                                     |
//                                                                            |
//-----------------------------------------------------------------------------

#include "stdafx.h"

#ifdef OTB_CLIENT_DA

#include "ClientDACreator.h"
#include "ClientDASession.h"
#include "ClientDASubscription.h"
#include "ClientDAItem.h"

//-----------------------------------------------------------------------------
// ClientDACreator
//-----------------------------------------------------------------------------

SODaCServer* ClientDACreator::createServer(
	IN BYTE protocolType,
	IN SODaCEntry* /* parent */)
{
	SODaCServer* p = NULL;

	switch (protocolType)
	{
#ifdef SOFEATURE_DCOM

	case SOCLT_PROTOCOL_DCOM:
		SOCMN_ALLOCATE_OBJECT_C(p, ClientDASessionCom(), (SODaCServer*));
		break;
#endif
#ifdef SOFEATURE_SOAP

	case SOCLT_PROTOCOL_SOAP:
		SOCMN_ALLOCATE_OBJECT_C(p, ClientDASessionXml(), (SODaCServer*));
		if (m_xmlGetStatusInterval > 0)
		{
			SOCmnPointer<SOCltTaskXmlGetStatus> getStatusCyclicTask = createTaskXmlGetStatus(p, m_xmlGetStatusInterval);
			p->setXmlStatusTask(getStatusCyclicTask);
		}
		break;
#endif
#ifdef SOFEATURE_TUNNEL

	case SOCLT_PROTOCOL_TUNNEL:
		SOCMN_ALLOCATE_OBJECT_C(p, ClientDASessionTp(), (SODaCServer*));
		break;
#endif

	default:
		return NULL;
		break;
	}

	return p;
}

SODaCGroup* ClientDACreator::createGroup(
	IN BYTE protocolType,
	IN SODaCServer* /* parent */)
{
	SODaCGroup* p = NULL;

	switch (protocolType)
	{
#ifdef SOFEATURE_DCOM

	case SOCLT_PROTOCOL_DCOM:
		SOCMN_ALLOCATE_OBJECT_C(p, ClientDASubscriptionCom(), (SODaCGroup*));
		break;
#endif
#ifdef SOFEATURE_SOAP

	case SOCLT_PROTOCOL_SOAP:
		SOCMN_ALLOCATE_OBJECT_C(p, ClientDASubscriptionXml(), (SODaCGroup*));
		break;
#endif
#ifdef SOFEATURE_TUNNEL

	case SOCLT_PROTOCOL_TUNNEL:
		SOCMN_ALLOCATE_OBJECT_C(p, ClientDASubscriptionTp(), (SODaCGroup*));
		break;
#endif

	default:
		return NULL;
		break;
	}

	return p;
}


SODaCItem* ClientDACreator::createItem(IN SODaCGroup* /* parent */)
{
	return new ClientDAItem();
}

#endif // OTB_CLIENT_DA

