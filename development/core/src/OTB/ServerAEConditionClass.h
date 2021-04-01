#ifndef _SERVERAECONDITIONCLASS_H_
#define _SERVERAECONDITIONCLASS_H_

#include "OTServer.h"

#pragma pack(push, 4)


//-----------------------------------------------------------------------------
// ServerAEConditionClass
//-----------------------------------------------------------------------------

class ServerAEConditionClass : public SOAeSConditionClassMultipleStateDefault
{
public:
	ServerAEConditionClass(IN LPCTSTR conditionName);

	BOOL isInit(void)
	{
		return m_isInit;
	}
	void init(void)
	{
		m_isInit = TRUE;
	}

protected:
	BYTE m_isInit;
};

#pragma pack(pop)
#endif
