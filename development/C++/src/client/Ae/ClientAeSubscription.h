#ifndef _CLIENTAESUBSCRIPTION_H_
#define _CLIENTAESUBSCRIPTION_H_

#include "../ClientEnums.h"
#include "../ClientObjectSpaceElement.h"
#include "../../Trace.h"
#include "../../Enums.h"
#include "Mutex.h"

#ifdef WIN64
#pragma pack(push,8)
#else
#pragma pack(push,4)
#endif

namespace SoftingOPCToolboxClient
{

class AeSession;
class AeReturnedAttributes;
class AeCondition;
class AeEvent;

class TBC_EXPORT AeSubscription : public ObjectSpaceElement
{

	//friend class Application;
	friend class OTCGlobals;
	friend class AeCondition;

private:

	void onAeEventsReceived(
		BOOL isRefresh,
		BOOL lastRefresh,
		const std::vector<AeEvent*>& events);

	void onAeConditionsChanged(const std::vector<AeCondition*>& someAeConditions);

	void onRefreshAeConditions(
		unsigned long executionContext,
		long result);

	void onCancelRefreshAeConditions(
		unsigned long executionContext,
		long result);


protected:

	AeSession* m_session;
	std::vector<AeCondition*> m_conditionList;
	Mutex m_conditionListJanitor;
	void setAeConditionList(std::vector<AeCondition*> someAeConditions);

public:

	AeSubscription(AeSession* parentSession);
	virtual ~AeSubscription();

	AeSession* getAeSession();
	void setAeSession(AeSession* aSession);

	unsigned long getRequestedBufferTime();
	void setRequestedBufferTime(unsigned long requestedBufferTime);

	unsigned long getRevisedBufferTime();

	unsigned long getRequestedMaxSize();
	void setRequestedMaxSize(unsigned long maxSize);

	unsigned long getRevisedMaxSize();

	unsigned long getFilterSeverityLow();
	void setFilterSeverityLow(unsigned long filterSeverityLow);

	unsigned long getFilterSeverityHigh();
	void setFilterSeverityHigh(unsigned long filterSeverityHigh);

	unsigned long getFilterEventTypes();
	void setFilterEventTypes(unsigned long filterEventTypes);

	std::vector<unsigned long> getFilterCategories();
	void setFilterCategories(std::vector<unsigned long> categories);

	std::vector<tstring> getFilterAreas();
	void setFilterAreas(std::vector<tstring> filterAreas);

	std::vector<tstring> getFilterSources();
	void setFilterSources(std::vector<tstring> filterSources);

	std::vector<AeReturnedAttributes*> getReturnedAttributes();
	void setReturnedAttributes(std::vector<AeReturnedAttributes*> someAeReturnedAttributes);

	std::vector<AeCondition*>& getAeConditionList();

	virtual long refreshAeConditions(ExecutionOptions* someExecutionOptions);

	virtual long cancelRefreshAeConditions(ExecutionOptions* someExecutionOptions);

	virtual void handleAeEventsReceived(
		BOOL isRefresh,
		BOOL lastRefresh,
		const std::vector<AeEvent*>& events);

	virtual void handleAeConditionsChanged(const std::vector<AeCondition*>& someAeConditions);

	virtual void handleRefreshAeConditionsCompleted(
		unsigned long executionContext,
		long result);

	virtual void handleCancelRefreshAeConditionsCompleted(
		unsigned long executionContext,
		long result);

}; // end class AeSubscription

typedef std::pair <unsigned long, AeSubscription*> AeSubscriptionPair;

class TBC_EXPORT AeReturnedAttributes
{

protected:

	unsigned long m_categoryId;
	std::vector<unsigned long>m_attributesIds;

public:

	AeReturnedAttributes()
	{
		m_categoryId = 0;
		m_attributesIds.push_back(0);
	} //end constructor

	virtual ~AeReturnedAttributes();

	unsigned long getCategoryId()
	{
		return m_categoryId;
	} //end getCategoryId

	void setCategoryId(unsigned long categoryId)
	{
		m_categoryId = categoryId;
	}  //end setCategoryId

	const std::vector<unsigned long>& getAttributesIds()
	{
		return m_attributesIds;
	} // end get
	void setAttributesIds(std::vector<unsigned long> attributesIds)
	{
		m_attributesIds = attributesIds;
	} //end set

}; //end AeReturnedAttributes

}; //end namespace OPC_Client


#pragma pack(pop)
#endif
