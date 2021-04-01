#ifndef _CLIENTAESESSION_H_
#define _CLIENTAESESSION_H_

#include "../ClientEnums.h"
#include "../ClientObjectSpaceElement.h"
#include "../../Trace.h"
#include "../../Enums.h"
#include "Mutex.h"

#include <map>

#ifdef WIN64
#pragma pack(push,8)
#else
#pragma pack(push,4)
#endif

namespace SoftingOPCToolboxClient
{

class AeCategory;
class AeSubscription;
class ServerStatus;
class AddressSpaceElementBrowseOptions;
class AddressSpaceElement;
class AeCondition;
class AeConditionState;
class AeAttribute;

class TBC_EXPORT AeSession : public ObjectSpaceElement
{

	friend class OTCGlobals;
	friend class AeSubscription;

private:

	unsigned char onShutdown(const tstring& reason);

	void onGetServerStatus(
		unsigned long executionContext,
		ServerStatus& aServerStatus,
		long result);

	void onBrowse(
		unsigned long executionContext,
		unsigned long addressSpaceElementHandle,
		std::vector<AddressSpaceElement*>& addressSpaceElements,
		long result);

	void onQueryAeCategories(
		unsigned long executionContext,
		std::vector<AeCategory*>& categories,
		long result);

	void onQueryAvailableAeFilters(
		unsigned long executionContext,
		EnumFilterBy availableFilters,
		long result);

	void onEnableAeConditionsBySource(
		unsigned long executionContext,
		BOOL enable,
		tstring& source,
		long result);

	void onEnableAeConditionsByArea(
		unsigned long executionContext,
		BOOL enable,
		tstring& area,
		long result);

	void onAcknowledgeAeConditions(
		unsigned long executionContext,
		tstring& ackId,
		tstring& ackComment,
		std::vector<AeCondition*>& someAeConditions,
		std::vector<long>& results,
		long result);

	void onQueryAeSourceConditions(
		unsigned long executioncontext,
		tstring& sourceName,
		std::vector<tstring>& conditionNames,
		long result);

	void onGetAeConditionState(
		unsigned long executionContext,
		tstring& sourcePath,
		tstring& conditionName,
		std::vector<unsigned long>& attributesIds,
		AeConditionState& conditionState,
		long result);

	void onGetErrorString(
		unsigned long executionContext,
		long errorCode,
		tstring& errorText,
		long result);

	void onLogon(
		unsigned long executionContext,
		tstring& userName,
		tstring& password,
		long result);

	void onLogoff(
		unsigned long executionContext,
		long result);

	std::map<unsigned long, AeCategory*> m_aeCategoryList;
	Mutex m_aeCategoryListJanitor;

	AeSubscription* findSubscription(unsigned long position);

	virtual long addAeSubscription(AeSubscription* aSubscription);

protected:

	std::map<unsigned long, AeSubscription*> m_aeSubscriptionList;
	Mutex m_aeSubscriptionListJanitor;

public:

	AeSession(const tstring& url);
	virtual ~AeSession();

	std::vector<AeSubscription*> getSubscriptionList(void)
	{
		m_aeSubscriptionListJanitor.lock();
		std::vector<AeSubscription*> subscriptionList(m_aeSubscriptionList.size(), NULL);
		std::map<unsigned long, AeSubscription*>::const_iterator aeSubscriptionIterator;
		unsigned long i = 0;

		for (aeSubscriptionIterator = m_aeSubscriptionList.begin(); aeSubscriptionIterator != m_aeSubscriptionList.end(); aeSubscriptionIterator ++)
		{
			if (aeSubscriptionIterator->second != NULL)
			{
				subscriptionList[i] = aeSubscriptionIterator->second;
				i++;
			} //end if
		} //end for

		m_aeSubscriptionListJanitor.unlock();
		return subscriptionList;
	} // end getSubscriptionList

	virtual tstring getUrl();

	virtual tstring getLocaleId();
	virtual void setLocaleId(tstring localeId);

	virtual tstring getClientName();
	virtual void setClientName(tstring clientName);

	virtual EnumClassContext getClassContext();
	virtual void setClassContext(EnumClassContext aClassContext);

	virtual long removeAeSubscription(AeSubscription* aSubscription);

	virtual long getStatus(
		ServerStatus& aServerStatus,
		ExecutionOptions* someExecutionOptions);

	virtual long browse(
		AddressSpaceElement* anAddressSpaceElement,
		AddressSpaceElementBrowseOptions* someBrowseOptions,
		std::vector<AddressSpaceElement*>& addressSpaceElements,
		ExecutionOptions* someExecutionOptions);

	virtual long queryAeCategories(
		std::vector<AeCategory*>& categories,
		ExecutionOptions* someExecutionOptions);

	virtual long queryAvailableAeFilters(
		EnumFilterBy& availableFilters,
		ExecutionOptions* someExecutionOptions);

	virtual long enableAeConditionsByArea(
		std::vector<tstring>& areas,
		BOOL enable,
		ExecutionOptions* someExecutionOptions);

	virtual long enableAeConditionsBySource(
		std::vector<tstring>& sources,
		BOOL enable,
		ExecutionOptions* someExecutionOptions);

	virtual long acknowledgeAeConditions(
		const tstring& ackId,
		const tstring& ackComment,
		std::vector<AeCondition*>& someAeConditions,
		std::vector<long>& results,
		ExecutionOptions* someExecutionOptions);

	virtual long queryAeSourceConditions(
		tstring& sourceName,
		std::vector<tstring>& conditionNames,
		ExecutionOptions* someExecutionOptions);

	virtual long getAeConditionState(
		tstring& sourcePath,
		tstring& conditionName,
		std::vector<AeAttribute*>& attributes,
		AeConditionState& conditionState,
		ExecutionOptions* someExecutionOptions);

	virtual long getErrorString(
		long errorCode,
		tstring& errorText,
		ExecutionOptions* executionOptions);

	virtual long activateConnectionMonitor(
		BOOL activate,
		unsigned long checkCycle,
		unsigned long connect1Attempts,
		unsigned long connect1Cycle,
		unsigned long connect2Cycle);

	virtual long logon(
		tstring& userName,
		tstring& password,
		ExecutionOptions* someExecutionOptions);

	virtual long logoff(ExecutionOptions* executionOptions);

	virtual unsigned char handleShutdownRequest(const tstring& reason);

	virtual void handleGetStatusCompleted(
		unsigned long executionContext,
		ServerStatus& aServerStatus,
		long result);

	virtual void handleBrowseCompleted(
		unsigned long executionContext,
		unsigned long addressSpaceElementHandle,
		std::vector<AddressSpaceElement*>& addressSpaceElements,
		long result);

	virtual void handleQueryAeCategoriesCompleted(
		unsigned long executionContext,
		std::vector<AeCategory*>& categories,
		long result);

	virtual void handleQueryAvailableAeFiltersCompleted(
		unsigned long executionContext,
		EnumFilterBy availableFilters,
		long result);

	virtual void handleEnableAeConditionsByAreaCompleted(
		unsigned long executionContext,
		BOOL enable,
		tstring& area,
		long result);

	virtual void handleEnableAeConditionsBySourceCompleted(
		unsigned long executionContext,
		BOOL enable,
		tstring& source,
		long result);

	virtual void handleAcknowledgeAeConditionsCompleted(
		unsigned long executionContext,
		tstring& ackId,
		tstring& ackComment,
		std::vector<AeCondition*>& someAeConditions,
		std::vector<long>& results,
		long result);

	virtual void handleQueryAeSourceConditionsCompleted(
		unsigned long executioncontext,
		tstring& sourceName,
		std::vector<tstring>& conditionNames,
		long result);

	virtual void handleGetAeConditionStateCompleted(
		unsigned long executionContext,
		tstring& sourcePath,
		tstring& conditionName,
		std::vector<unsigned long>& attributesIds,
		AeConditionState& conditionState,
		long result);

	virtual void handleGetErrorStringCompleted(
		unsigned long executionContext,
		long errorCode,
		tstring& errorText,
		long result);

	virtual void handleLogonCompleted(
		unsigned long executionContext,
		tstring& userName,
		tstring& password,
		long result);

	virtual void handleLogoffCompleted(
		unsigned long executionContext,
		long result);

}; // end class AeSession

typedef std::pair <unsigned long, AeSession*> AeSessionPair;

}; //end namespace OPC_Client


#pragma pack(pop)
#endif
