#ifndef _SERVERDAADDRESSSPACEELEMENT_H_
#define _SERVERDAADDRESSSPACEELEMENT_H_

#include "OTServer.h"

#pragma pack(push, 4)


//-----------------------------------------------------------------------------
// ServerDAAddressSpaceElement
//-----------------------------------------------------------------------------

class ServerDAAddressSpaceElement
{
public:
	ServerDAAddressSpaceElement(void);

	void setOTSUserData(DWORD ud)
	{
		m_userDataOTS = ud;
	}
	DWORD getOTSUserData(void)
	{
		return m_userDataOTS;
	}

	BOOL hasPropertyI(IN SODaSItemTag* pItemTag, IN DWORD propId,  SODaSPropertyData* pPropData);
	void getPropertiesI(IN SODaSItemTag* pItemTag, IN BOOL isSOAP, OUT SOCmnList<SODaSPropertyData> *pListPD);
	BOOL handlePropertyRequestI(IN SODaSItemTag* pItemTag, IN SODaSProperty* prop,
								IN SODaSRequest* request);
	BOOL initPropertyI(IN SODaSItemTag* pItemTag, IN SODaSProperty* property,  SODaSPropertyData* pPropData);

	BOOL isBrowsableI(IN SOCmnObject* pServer);
	void setIsBrowsable(BYTE browsable)
	{
		m_isBrowsable = browsable;
	}

	void setOwnCache(BOOL ownCache)
	{
		m_ownCache = (BYTE)ownCache;
	}
	BOOL getOwnCache(void)
	{
		return (BOOL) m_ownCache;
	}

	void setReportCyclic(BOOL reportCyclic)
	{
		m_reportCyclic = (BYTE)reportCyclic;
	}
	BOOL getReportCyclic(void)
	{
		return (BOOL) m_reportCyclic;
	}

	void connectedObjectChanged(SODaSItemTag* pItemTag);

	LONG getAddressSpaceElementData(SODaSItemTag* pItemTag, OTSAddressSpaceElementData* pData);
	BOOL getPropertyData(IN SODaSItemTag* pItemTag, IN DWORD propId, SODaSPropertyData* pPropData);

	void create(SOCmnElement* pParent, SODaSItemTag* pItemTag);
	void destroy(SODaSItemTag* tag);

	BYTE getDeviceIOModeI(SODaSItemTag* pItemTag, BYTE deviceIOAndUpdateMode, SODaSRequest* req);

	static void clonePropertyData(IN SODaSItemTag* pItemTag, IN OTSPropertyData* pPropDataO, OUT SODaSPropertyData* pPropData);

protected:
	DWORD m_userDataOTS;
	BYTE m_ownCache;
	BYTE m_isBrowsable;
	BYTE m_reportCyclic;
}; // ServerDAAddressSpaceElement

#pragma pack(pop)
#endif
