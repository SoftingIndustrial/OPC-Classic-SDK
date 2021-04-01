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
//  Filename    : SOSrvXML.cpp                                                |
//  Version     : 4.47.1                                                      |
//  Date        : 27-July-2020                                                |
//                                                                            |
//  Description : XML server related classes                                  |
//                - SOSrvXML: XML server object                               |
//                                                                            |
//-----------------------------------------------------------------------------

#include "stdafx.h"

#ifdef SOSRV

#include "SOSrvXML.h"

#ifdef SOFEATURE_XML_METHOD

//-----------------------------------------------------------------------------
// SOSrvXmlMethod                                                             |
//-----------------------------------------------------------------------------

SOSrvXmlMethod::SOSrvXmlMethod(void)
	: SOCmnXmlMethod()
{
}

#ifdef SOFEATURE_SOAP

SOSrvXmlMethod::SOSrvXmlMethod(
	IN BYTE xmlMethodType,
	IN SOSrvHTTPHandler* pHandler,
	IN SOSrvHTTPHandlerData* pHTTPData,
	IN DWORD lenBody,
	IN BYTE* pBody)
{
	m_xmlMethod = xmlMethodType;
	m_httpHandler.setWithAddRef(pHandler);
	m_httpHandlerData.setWithAddRef(pHTTPData);
	m_rcvTime = pHTTPData->m_rcvTime;
	m_flags = 0;
	m_lenData = lenBody;
	m_flags = pHTTPData->m_flags;
	SOCMN_ALLOCATE_MEMORY_C(m_pData, m_lenData + 1, (BYTE*))

	if (m_pData)
	{
		memcpy(m_pData, pBody, m_lenData + 1);
	}

	setObjectState(SOCMNXMLMETHOD_STATE_INIT);
	create(NULL, _T('|'));
	setHandler(SCMNXML_HANDLER_ELEMENT | SCMNXML_HANDLER_CHARACTERDATA | SCMNXML_HANDLER_NAMESPACEDECL);
	m_elementStack.create();
	m_namespaceList.create(SOCMNLIST_TYPE_ORG_MAP | SOCMNLIST_TYPE_KEY_STRING | SOCMNLIST_TYPE_OBJ_STRING | SOCMNLIST_TYPE_BLOCKED);
}

#endif

SOSrvXmlMethod::~SOSrvXmlMethod(void)
{
	SOCMN_FREE_MEMORY(m_pData)
}

#endif //SOFEATURE_XML_METHOD

#endif // SOSRV

