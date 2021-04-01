#include "stdafx.h"

#ifdef OTB_SERVER_AE

#include "ServerAECondition.h"
#include "SrvApp.h"
#include "OTB.h"

//-----------------------------------------------------------------------------
// ServerAECondition
//-----------------------------------------------------------------------------

BOOL ServerAECondition::getAckRequired(IN WORD changeMask)
{
	ServerApp* pApp = ::getServerApp();

	if (!pApp->m_callbackFunctions.m_OTSAcknowledgeCondition)
	{
		return FALSE;
	}

	if ((changeMask & (OPC_CHANGE_ACTIVE_STATE | OPC_CHANGE_SUBCONDITION)))
	{
		return m_ackRequired;
	}

	return FALSE;
}

BOOL ServerAECondition::setConditionName(DWORD categoryId, LPCTSTR condName)
{
	SOCmnPointer<SOAeSEntry> entryAE = getSOAeSEntry();
	SOCmnPointer<SOAeSEventSpaceRoot> esRoot = entryAE->getEventSpaceRoot();
	SOCmnPointer<SOAeSCategory> category;
	SOCmnList<SOAeSCategory> categoryList((SOCmnList<SOAeSCategory> *)esRoot->getBranchList());
	category = categoryList.findKey(categoryId);

	if (!category)
	{
		return FALSE;
	}

	SOCmnPointer<SOAeSConditionClass> condClass;
	SOCmnList<SOAeSConditionClass> condClassList((SOCmnList<SOAeSConditionClass> *)category->getBranchList());
	condClass = condClassList.findKey(condName);

	if (!condClass)
	{
		return FALSE;
	}

	setConditionClass(condClass);

	SOCmnList<SOAeSSubConditionClass> subCondClassList((SOCmnList<SOAeSSubConditionClass> *)condClass->getLeafList());

	if ((subCondClassList.isInit()) && (subCondClassList.getCount() > 0))
	{
		SOCmnListPosition listPosition = subCondClassList.getStartPosition();
		if (listPosition)
		{
			SOAeSSubConditionClass* pSubCondClass = subCondClassList.getNext(listPosition);
			setLastActSubConditionClass(pSubCondClass);
		}
		else
		{
			setLastActSubConditionClass(NULL);
		}
	}
	else
	{
		setLastActSubConditionClass(NULL);
	}

	return TRUE;
}

BOOL ServerAECondition::setSubConditionName(LPCTSTR subCondName)
{
	SOCmnPointer<SOAeSConditionClass> condClass(getConditionClass());

	if (!condClass)
	{
		return FALSE;
	}

	SOCmnPointer<SOAeSSubConditionClass> subCondClass;
	SOCmnList<SOAeSSubConditionClass> subCondClassList((SOCmnList<SOAeSSubConditionClass> *)condClass->getLeafList());

	if (subCondClassList.isInit())
	{
		subCondClass = subCondClassList.findKey(subCondName);

		if (!subCondClass)
		{
			return FALSE;
		}
	}

	setActSubConditionClass(subCondClass);
	setLastActSubConditionClass(subCondClass);
	return TRUE;
}


HRESULT ServerAECondition::handleAcknowledge(
	IN LPCTSTR ackID,
	IN LPCTSTR ackComment,
	IN LPFILETIME /* activeTime */)
{
	ServerApp* pApp = ::getServerApp();
	HRESULT res = E_FAIL;

	if (pApp->m_callbackFunctions.m_OTSAcknowledgeCondition)
	{
		OTSObjectData object;
		object.m_objectHandle = (OTObjectHandle)this->getOpcHandle();
		object.m_userData = getOTSUserData();

		SOCMN_TRY
		{
			res = pApp->m_callbackFunctions.m_OTSAcknowledgeCondition(object, (LPTSTR)ackID, (LPTSTR)ackComment);
		}
		SOCMN_CATCH(...)
		{
			_TRACE(SOCMNTRACE_L_ERR, SOCMNTRACE_G_OPC_SERVER, (_T("ServerAECondition::handleAcknowledge - Exception occurred in vendor application callback: OTSAcknowledgeCondition")));
		}
	}
	else
	{
		res = E_FAIL;
	}

	return res;
} // handleAcknowledge

#endif // OTB_SERVER_AE
