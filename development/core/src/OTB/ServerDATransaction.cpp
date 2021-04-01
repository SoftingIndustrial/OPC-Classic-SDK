#include "stdafx.h"

#ifdef OTB_SERVER_DA

#include "opcerror.h"
#include "ServerDATransaction.h"
#include "ServerDAServer.h"
#include "ServerDANode.h"
#include "ServerDATag.h"
#include "ServerDAProperty.h"
#include "SrvApp.h"


//-----------------------------------------------------------------------------
// ServerDATransaction
//-----------------------------------------------------------------------------

ServerDATransaction::ServerDATransaction(
	IN SODaSGroup* pGroup,
	IN BYTE type,
	IN BYTE connectionType,
	IN DWORD connection)
	: SODaSTransaction(pGroup, type, connectionType, connection)
{
	m_paRequestData = NULL;
	m_sizeRequestData = 0;
}

ServerDATransaction::~ServerDATransaction(void)
{
	if (m_paRequestData)
	{
		free(m_paRequestData);
	}
}


//-----------------------------------------------------------------------------
// handleRequests
//
// - handle all transaction requests of the transaction
//
void ServerDATransaction::handleRequests(
	IN SOCmnList<SODaSRequest> *pReqList)
{
	SOCMN_TRY
	{
		ServerApp* pApp = ::getServerApp();
		SOCmnPointer<ServerDAServer> server;
		SOCmnList<SODaSRequest> reqList(pReqList);
		SOCmnListPosition pos;
		SOCmnPointer<SODaSItemTag> tag;
		SODaSRequest* pRequest;
		LONG i, j;
		OTSValueData* paWriteValues = NULL;
		SOCmnVariant** ppaVal = NULL;

		if (m_sizeRequestData < reqList.getCount())
		{
			m_sizeRequestData = reqList.getCount();
			m_paRequestData = (OTSRequestData*)realloc(m_paRequestData, sizeof(OTSRequestData) * m_sizeRequestData);
			// TODO: out of memory
		}

		if (isWrite())
		{
			paWriteValues = (OTSValueData*)malloc(sizeof(OTSValueData) * m_sizeRequestData);
			memset(paWriteValues, 0, sizeof(OTSValueData) * m_sizeRequestData);
			ppaVal = (SOCmnVariant**)malloc(sizeof(SOCmnVariant*) * m_sizeRequestData);
			memset(ppaVal, 0, sizeof(SOCmnVariant*) * m_sizeRequestData);
			// TODO: out of memory
		}

		pos = reqList.getStartPosition();
		i = 0;

		while (pos)
		{
			pRequest = reqList.getNext(pos);
			if (!pRequest->isCompleted())
			{
				tag = pRequest->getItemTag();

				if (tag.isNotNull())
				{
					if (tag->getDeviceIOMode(pRequest) == SODaSItemTag::transaction)
					{
						if (!tag->is(SOCMNOBJECT_TYPE_PROPERTY))
						{
							// tag or node
							m_paRequestData[i].m_object.m_objectHandle = (DWORD)tag->getOpcHandle();
							m_paRequestData[i].m_propertyID = 0;
							pRequest->addRef();
							m_paRequestData[i].m_requestHandle = (DWORD)pRequest->getOpcHandle();
							server = (ServerDAServer*)pRequest->getServer();
							m_paRequestData[i].m_clientHandle = (DWORD)server->getOpcHandle();

							if (tag->is(SOCMNOBJECT_TYPE_TAG))
							{
								m_paRequestData[i].m_object.m_userData = ((ServerDATag*)(SODaSItemTag*)tag)->getOTSUserData();
							}
							else if (tag->is(SOCMNOBJECT_TYPE_NODE))
							{
								m_paRequestData[i].m_object.m_userData = ((ServerDANode*)(SODaSItemTag*)tag)->getOTSUserData();
							}
						}
						else
						{
							// property
							SOCmnPointer<SODaSItemTag> parent((SODaSItemTag*)tag->getParentBranch());

							if (!parent)
							{
								pRequest->complete(E_FAIL);
							}

							m_paRequestData[i].m_object.m_objectHandle = (DWORD)parent->getOpcHandle();
							m_paRequestData[i].m_propertyID = ((ServerDAProperty*)(SODaSItemTag*)tag)->getPropertyId();
							pRequest->addRef();
							m_paRequestData[i].m_requestHandle = (DWORD)pRequest->getOpcHandle();
							server = (ServerDAServer*)pRequest->getServer();
							m_paRequestData[i].m_clientHandle = (DWORD)server->getOpcHandle();

							if (parent->is(SOCMNOBJECT_TYPE_TAG))
							{
								m_paRequestData[i].m_object.m_userData = ((ServerDATag*)(SODaSItemTag*)parent)->getOTSUserData();
							}
							else if (parent->is(SOCMNOBJECT_TYPE_NODE))
							{
								m_paRequestData[i].m_object.m_userData = ((ServerDANode*)(SODaSItemTag*)parent)->getOTSUserData();
							}
						}

						if (isWrite())
						{
							ppaVal[i] = new SOCmnVariant();
						
							// VALUE
							paWriteValues[i].m_pValue = ppaVal[i];
							pRequest->getWriteValue(ppaVal[i]);
						
							// QUALITY
							if ((pRequest->getValidWriteQT() & SODASREQUEST_FLAG_QUALITY) == SODASREQUEST_FLAG_QUALITY)
							{
								paWriteValues[i].m_quality = pRequest->getWriteQuality();
							}
							else
							{
								paWriteValues[i].m_quality = OPC_QUALITY_GOOD;
							}
						
							// TIMESTAMP
							SOCmnDateTime temp;
							pRequest->getWriteTimeStamp(temp);
							if (((pRequest->getValidWriteQT() & SODASREQUEST_FLAG_TIMESTAMP) == SODASREQUEST_FLAG_TIMESTAMP) &&
								(temp.isSet()))
							{
								temp.get(&paWriteValues[i].m_timestamp);
							}
							else
							{
#ifdef SOOS_WINDOWS
								paWriteValues[i].m_timestamp.dwHighDateTime = 0;
								paWriteValues[i].m_timestamp.dwLowDateTime = 0;
#else
								paWriteValues[i].m_timestamp.tv_sec = 0;
								paWriteValues[i].m_timestamp.tv_usec = 0;
#endif
							}
						}

						i++;
					}
				}
			}
		}

		if (isRead())
		{
			if (pApp->m_callbackFunctions.m_OTSHandleReadRequests)
			{
				SOCMN_TRY
				{
					pApp->m_callbackFunctions.m_OTSHandleReadRequests(i, m_paRequestData);
				}
				SOCMN_CATCH(...)
				{
					_TRACEC(SOCMNTRACE_L_ERR, SOCMNTRACE_G_OPC_SERVER, server->getObjTraceId(), (_T("ServerDATransaction::handleRequests - Exception occurred in vendor application callback: OTSHandleReadRequests")));
				}
			}
			else
			{
				// No Handler whatsoever. TRACE
			}
		}
		else
		{
			if (pApp->m_callbackFunctions.m_OTSHandleWriteRequests)
			{
				SOCMN_TRY
				{
					pApp->m_callbackFunctions.m_OTSHandleWriteRequests(i, m_paRequestData, paWriteValues);
				}
				SOCMN_CATCH(...)
				{
					_TRACEC(SOCMNTRACE_L_ERR, SOCMNTRACE_G_OPC_SERVER, server->getObjTraceId(), (_T("ServerDATransaction::handleRequests - Exception occurred in vendor application callback: OTSHandleWriteRequests")));
				}
			}
			else
			{
				// No Handler whatsoever. TRACE
			}
		}

		if (isWrite())
		{
			for (j = 0; j < i; j++)
			{
				if (ppaVal[j])
				{
					ppaVal[j]->Clear();
					delete ppaVal[j];
				}
			}

			free(ppaVal);
			free(paWriteValues);
		}
	}
	SOCMN_CATCH(...)
	{
		_TRACE(SOCMNTRACE_L_ERR, SOCMNTRACE_G_OPC_SERVER, (_T("ServerDATransaction::handleRequests - Exception occurred")));
	}
} // handleRequests

//-----------------------------------------------------------------------------
// notiofyCompletedInternally
//
// - notify upper layers that request was internally completed
//
void ServerDATransaction::notiofyCompletedInternally(IN SODaSRequest* pRequest)
{
	SOCMN_TRY
	{
		ServerApp* pApp = ::getServerApp();

		if (pApp->m_callbackFunctions.m_OTSRequestCompletedInternally)
		{
			SOCMN_TRY
			{
				pApp->m_callbackFunctions.m_OTSRequestCompletedInternally((DWORD)pRequest->getOpcHandle());
			}
			SOCMN_CATCH(...)
			{
				_TRACE(SOCMNTRACE_L_ERR, SOCMNTRACE_G_OPC_SERVER, (_T("ServerDATransaction::notiofyCompletedInternally - Exception occurred in vendor application callback: OTSRequestCompletedInternally")));
			}
		}
	}
	SOCMN_CATCH(...)
	{
		_TRACE(SOCMNTRACE_L_ERR, SOCMNTRACE_G_OPC_SERVER, (_T("ServerDATransaction::notiofyCompletedInternally - Exception occurred")));
	}
} // notiofyCompletedInternally

#endif // OTB_SERVER_DA

