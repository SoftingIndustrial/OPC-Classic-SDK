#include "stdafx.h"

#ifdef OTB_SERVER_DA

#include "opcerror.h"
#include "ServerDAProperty.h"
#include "ServerDAServer.h"

//-----------------------------------------------------------------------------
// ServerDAProperty
//-----------------------------------------------------------------------------

ServerDAProperty::ServerDAProperty(DWORD propertyID)
	: SODaSProperty(propertyID)
{
	if (!isStandardTagProperty(propertyID))
	{
		setDeviceIOMode(transaction);
	}
}

SOCmnString ServerDAProperty::getItemID(void)
{
	SOCmnPointer<SODaSItemTag> itemTag = (SODaSItemTag*)getParentBranch();
	SOCmnString itemID;

	if (itemTag.isNotNull())
	{
		ServerDAServer::getPropertyItemIDI(itemTag, itemTag->getItemID(), getPropertyId(), itemID);
	}

	if (itemID.isEmpty())
	{
		itemID = SODaSProperty::getItemID();
	}

	return itemID;
} // getPropertyItemID

#endif // OTB_SERVER_DA
