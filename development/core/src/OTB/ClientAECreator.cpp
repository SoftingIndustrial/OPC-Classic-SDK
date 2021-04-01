#include "stdafx.h"

#ifdef OTB_CLIENT_AE

#include "ClientAECreator.h"
#include "ClientAESession.h"
#include "ClientAESubscription.h"

//-----------------------------------------------------------------------------
// ClientAECreator
//-----------------------------------------------------------------------------

SOAeCServer* ClientAECreator::createServer(
	IN BYTE protocolType,
	IN SOAeCEntry* /* parent */)
{
	SOAeCServer* p = NULL;

	switch (protocolType)
	{
#ifdef SOFEATURE_DCOM

	case SOCLT_PROTOCOL_DCOM:
		SOCMN_ALLOCATE_OBJECT_C(p, ClientAESessionCom(), (SOAeCServer*));
		break;
#endif
#ifdef SOFEATURE_TUNNEL

	case SOCLT_PROTOCOL_TUNNEL:
		SOCMN_ALLOCATE_OBJECT_C(p, ClientAESessionTp(), (SOAeCServer*));
		break;
#endif

	default:
		return NULL;
		break;
	}

	return p;
}

SOAeCSubscription* ClientAECreator::createSubscription(
	IN BYTE protocolType,
	IN SOAeCServer* /* parent */)
{
	SOAeCSubscription* p = NULL;

	switch (protocolType)
	{
#ifdef SOFEATURE_DCOM

	case SOCLT_PROTOCOL_DCOM:
		SOCMN_ALLOCATE_OBJECT_C(p, ClientAESubscriptionCom(), (SOAeCSubscription*));
		break;
#endif
#ifdef SOFEATURE_TUNNEL

	case SOCLT_PROTOCOL_TUNNEL:
		SOCMN_ALLOCATE_OBJECT_C(p, ClientAESubscriptionTp(), (SOAeCSubscription*));
		break;
#endif

	default:
		return NULL;
		break;
	}

	return p;
}

#endif // OTB_CLIENT_AE
