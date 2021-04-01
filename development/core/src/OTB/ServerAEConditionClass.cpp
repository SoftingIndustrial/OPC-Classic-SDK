#include "stdafx.h"

#ifdef OTB_SERVER_AE

#include "ServerAEConditionClass.h"
#include "SrvApp.h"
#include "OTB.h"

//-----------------------------------------------------------------------------
// ServerAEConditionClass
//-----------------------------------------------------------------------------

ServerAEConditionClass::ServerAEConditionClass(IN LPCTSTR conditionName)
	: SOAeSConditionClassMultipleStateDefault()
{
	setName(conditionName);
	m_isInit = FALSE;
}

#endif // OTB_SERVER_AE
