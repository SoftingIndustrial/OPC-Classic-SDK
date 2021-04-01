#include "stdafx.h"

#ifdef OTB_SERVER_DA

#include "ServerDACache.h"
#include "ServerDAServer.h"
#include "ServerDANode.h"
#include "ServerDATag.h"
#include "SrvApp.h"


//-----------------------------------------------------------------------------
// ServerDACache                                                                 |
//-----------------------------------------------------------------------------

ServerDACache::ServerDACache(void) {}
ServerDACache::~ServerDACache(void) {}

BOOL ServerDACache::getCache(IN SODaSRequest* req, OTSValueData& value)
{
	ServerApp* pApp = ::getServerApp();
	BOOL ret = FALSE;

	SOCMN_TRY
	{
		if (pApp->m_callbackFunctions.m_OTSQueryCacheValue)
		{
			SOCmnPointer<SODaSItemTag> itemTag = req->getItemTag();

			if (itemTag->is(SOCMNOBJECT_TYPE_PROPERTY))
			{
				itemTag = (SODaSItemTag*)itemTag->getParentBranch();
			}

			SOCmnPointer<ServerDAServer> server = (ServerDAServer*)req->getServer();
			OTSObjectData object;
			object.m_objectHandle = (DWORD)itemTag->getOpcHandle();

			if (itemTag->is(SOCMNOBJECT_TYPE_TAG))
			{
				object.m_userData = ((ServerDATag*)(SODaSItemTag*)itemTag)->getOTSUserData();
			}
			else
			{
				object.m_userData = ((ServerDANode*)(SODaSItemTag*)itemTag)->getOTSUserData();
			}

			SOCMN_TRY
			{
				ret = (pApp->m_callbackFunctions.m_OTSQueryCacheValue((DWORD)server->getOpcHandle(), object, &value) == S_OK);
			}
			SOCMN_CATCH(...)
			{
				_TRACE(SOCMNTRACE_L_ERR, SOCMNTRACE_G_OPC_SERVER, (_T("ServerDACache::getCache - Exception occurred in vendor application callback: OTSQueryCacheValue")));
			}
		}
	}
	SOCMN_CATCH(...)
	{
		_TRACE(SOCMNTRACE_L_ERR, SOCMNTRACE_G_OPC_SERVER, (_T("ServerDACache::getCache - Exception occurred")));
	}

	return ret;
}

BOOL ServerDACache::getTimeStamp(IN SODaSRequest* req, OUT SOCmnDateTime& timeStamp)
{
	OTSValueData value;
	SOCmnVariant v;
	value.m_pValue = v;
	BOOL ret = getCache(req, value);

	if (ret)
	{
		timeStamp.set(&value.m_timestamp);
	}

	return ret;
}

BOOL ServerDACache::getValue(IN SODaSRequest* req, IN VARTYPE typeWanted, IN LCID localeID, OUT SOCmnVariant& value, OPTIONAL OUT HRESULT* pConvRes)
{
	OTSValueData val;
	value.clear();
	val.m_pValue = &value;
	BOOL ret = getCache(req, val);

	if (ret)
	{
		if (((typeWanted != VT_EMPTY) && (typeWanted != (VT_EMPTY | VT_ARRAY)) &&
			 (typeWanted != value.vt)))
		{
			ret = changeDatatype(value, typeWanted, value, localeID, NULL, pConvRes);

			if (ret)
			{
				SOCmnVariant::variantClear(value);
			}
		}
	}
	else
	{
		if (pConvRes)
		{
			*pConvRes = E_FAIL;
		}
	}

	return ret;
}

WORD ServerDACache::getQuality(IN SODaSRequest* req)
{
	OTSValueData value;
	memset(&value, 0, sizeof(OTSValueData));
	SOCmnVariant v(0);
	value.m_pValue = v;
	BOOL ret = getCache(req, value);

	if (ret)
	{
		return value.m_quality;
	}

	return OPC_QUALITY_BAD;
}

#endif // OTB_SERVER_DA
