#ifndef _SERVERAEADDRESSSPACEELEMENT_H_
#define _SERVERAEADDRESSSPACEELEMENT_H_

#include "OTServer.h"

#pragma pack(push, 4)


//-----------------------------------------------------------------------------
// ServerAEAddressSpaceElement
//-----------------------------------------------------------------------------

class ServerAEAddressSpaceElement
{
public:
	ServerAEAddressSpaceElement(void);

	BOOL isBrowsableI(IN SOCmnObject* pServer);
	void setIsBrowsable(BYTE browsable)
	{
		m_isBrowsable = browsable;
	}

	void setOTSUserData(DWORD ud)
	{
		m_userDataOTS = ud;
	}
	DWORD getOTSUserData(void)
	{
		return m_userDataOTS;
	}

	LONG getAddressSpaceElementData(SOCmnElement* pElemenet, OTSAddressSpaceElementData* pData);

	void create(SOCmnElement* pParent, SOCmnElement* pElemenet);
	void destroy(SOCmnElement* pElemenet);

protected:
	DWORD m_userDataOTS;
	BYTE m_isBrowsable;
}; // ServerAEAddressSpaceElement



#pragma pack(pop)
#endif
