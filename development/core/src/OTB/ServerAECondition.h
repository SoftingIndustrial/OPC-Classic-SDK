#ifndef _SERVERAECONDITION_H_
#define _SERVERAECONDITION_H_

#include "OTServer.h"

#pragma pack(push, 4)


//-----------------------------------------------------------------------------
// ServerAECondition
//-----------------------------------------------------------------------------

class ServerAECondition : public SOAeSConditionDefault
{
public:
	ServerAECondition(void)
	{
		m_severity = 1;
		m_ackRequired = FALSE;
		m_userDataOTS = 0;
	}

	virtual SOCmnString getSourceName(void)
	{
		return m_sourceName;
	}
	void setSourceName(LPCTSTR sourceName)
	{
		m_sourceName = sourceName;
	}
	virtual DWORD getSeverity(void)
	{
		return m_severity;
	}
	void setSeverity(DWORD severity)
	{
		m_severity = severity;
	}
	virtual SOCmnString getMessage(void)
	{
		return m_message;
	}
	void setMessage(LPCTSTR message)
	{
		m_message = message;
	}

	virtual BOOL getAckRequired(IN WORD changeMask);
	void setAckRequired(BOOL ar)
	{
		m_ackRequired = (BYTE)ar;
	}

	BOOL setConditionName(DWORD categoryId, LPCTSTR condName);
	BOOL setSubConditionName(LPCTSTR subCondName);

	void setOTSUserData(DWORD ud)
	{
		m_userDataOTS = ud;
	}
	DWORD getOTSUserData(void)
	{
		return m_userDataOTS;
	}

protected:
	SOCmnString m_sourceName;
	SOCmnString m_message;
	DWORD m_severity;
	BYTE m_ackRequired;
	DWORD m_userDataOTS;

	virtual ~ServerAECondition(void) {}

	// external handling of client acknowledge
	virtual HRESULT handleAcknowledge(IN LPCTSTR ackID, IN LPCTSTR ackComment,
									  IN LPFILETIME activeTime);
};

#pragma pack(pop)
#endif
