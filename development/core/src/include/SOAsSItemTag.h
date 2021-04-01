#ifndef _SOASSITEMTAG_H_
#define _SOASSITEMTAG_H_

#pragma pack(push, 4)


#include "SODaSItem.h"
#include "SOAeSCondition.h"

//-----------------------------------------------------------------------------
// SOAsSItemTagConnectObjectStore                                             |
//-----------------------------------------------------------------------------

class SODAS_EXPORT SOAsSItemTagConnectObjectStore : virtual public SODaSItemTagIConnect
{
public:
	// connected objects (e.g items connected to the item tag)
	virtual void getAllConnectedObjects(IN DWORD objType,
										OUT SOCmnList<SOCmnObject> &objList);
	virtual DWORD notifyAllConnectedObjects(IN OPTIONAL SOCmnVariant* value = NULL,
											IN OPTIONAL WORD quality = OPC_QUALITY_GOOD, IN OPTIONAL SOCmnDateTime* timeStamp = NULL,
											IN OPTIONAL HRESULT result = S_OK);
	virtual void addConnectedObject(IN SOCmnObject* obj, IN BOOL goingToActivate);
	virtual void removeConnectedObject(IN SOCmnObject* obj);

protected:
	SOCmnList<SODaSItem> m_itemList;
	SOCmnList<SOAeSCondition> m_conditionList;

	SOAsSItemTagConnectObjectStore();
	virtual ~SOAsSItemTagConnectObjectStore();

	DWORD checkConditions(IN SOCmnList<SOAeSCondition> *conditionList);
}; // SOAsSItemTagConnectObjectStore

#pragma pack(pop)
#endif
