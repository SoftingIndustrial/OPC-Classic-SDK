#ifndef _CLIENTDASUBSCRIPTION_H_
#define _CLIENTDASUBSCRIPTION_H_

#include "../../Enums.h"
#include "../ClientEnums.h"
#include "../../ValueQT.h"

#include "../ClientObjectSpaceElement.h"
#include "../../Trace.h"
#include "Mutex.h"

#include <map>

#ifdef WIN64
#pragma pack(push,8)
#else
#pragma pack(push,4)
#endif

namespace SoftingOPCToolboxClient
{

class DaSession;
class DaItem;

class TBC_EXPORT DaSubscription : virtual public ObjectSpaceElement
{

	friend class OTCGlobals;
	friend class DaItem;

private:

	void onDataChange(
		unsigned long executionContext,
		const std::vector<DaItem*>& items,
		const std::vector<ValueQT*>& values,
		const std::vector<long>& results);

	void onReadComplete(
		unsigned long executionContext,
		const std::vector<DaItem*>& items,
		const std::vector<ValueQT*>& values,
		const std::vector<long>& results,
		long result);

	void onWriteComplete(
		unsigned long executionContext,
		const std::vector<DaItem*>& items,
		const std::vector<ValueQT*>& values,
		const std::vector<long>& results,
		long result);

	void onValidateDaItems(
		unsigned long executionContext,
		std::vector<DaItem*>& items,
		std::vector<long>& results);

	void onGetDaItemAttributesFromServer(
		unsigned long executionContext,
		std::vector<DaItem*>& items,
		std::vector<EnumObjectAttribute>& whatAttributes,
		std::vector<long>& results,
		long result);

	void onSetDaItemAttributesToServer(
		unsigned long executionContext,
		std::vector<DaItem*>& items,
		std::vector<EnumObjectAttribute>& whatAttributes,
		std::vector<long>& results,
		long result);

	virtual long addDaItem(
		tstring itemId,
		DaItem* anItem);

	virtual long addDaItem(
		DaItem* anItem);

	DaItem* findItem(unsigned long position);

protected:

	std::map<unsigned long, DaItem*> m_itemList;
	Mutex m_itemListJanitor;
	DaSession* m_session;

public:

	DaSubscription(
		unsigned long updateRate,
		DaSession* parentSession);

	virtual ~DaSubscription();

	DaSession* getDaSession();
	void setDaSession(DaSession* aSession);

	std::vector<DaItem*> getItemList(void)
	{
		m_itemListJanitor.lock();
		std::vector<DaItem*> itemList(m_itemList.size(), NULL);
		std::map<unsigned long, DaItem*>::const_iterator daItemIterator;
		unsigned long i = 0;

		for (daItemIterator = m_itemList.begin(); daItemIterator != m_itemList.end(); daItemIterator++)
		{
			if (daItemIterator->second != NULL)
			{
				itemList[i] = daItemIterator->second;
				i++;
			} //end if
		} //end for

		m_itemListJanitor.unlock();
		return itemList;
	} // end getItemList

	unsigned long getRequestedUpdateRate();
	void setRequestedUpdateRate(unsigned long requestedUpdateRate);

	unsigned long getRevisedUpdateRate();

	unsigned long getRequestedKeepAliveTime();
	void setRequestedKeepAliveTime(unsigned long requestedKeepAliveTime);

	unsigned long getRevisedKeepAliveTime();

	EnumDataRetrievalMode getDataRetrievalMode();
	void setDataRetrievalMode(EnumDataRetrievalMode dataRetrievalMode);

	tstring getName();
	void setName(tstring name);

	float getDeadband();
	void setDeadband(float deadband);

	long getTimeBias();
	void setTimeBias(long timeBias);

	tstring getLocaleId();
	void setLocaleId(tstring localeId);

	virtual long removeDaItem(DaItem* anItem);

	virtual long read(
		unsigned long maxAge,
		std::vector<DaItem*> items,
		std::vector<ValueQT*>& values,
		std::vector<long>& results,
		ExecutionOptions* someExecutionOptions);

	virtual long write(
		std::vector<DaItem*> items,
		std::vector<ValueQT*> values,
		std::vector<long>& results,
		ExecutionOptions* someExecutionOptions);

	virtual long validateDaItems(
		std::vector<DaItem*> items,
		std::vector<long>& results,
		ExecutionOptions* someExecutionOptions);

	virtual long getDaItemAttributesFromServer(
		std::vector<DaItem*> items,
		std::vector<EnumObjectAttribute> whatAttributes,
		std::vector<long>& results,
		ExecutionOptions* someExecutionOptions);

	virtual long setDaItemAttributesToServer(
		std::vector<DaItem*> items,
		std::vector<EnumObjectAttribute> whatAttributes,
		std::vector<long>& results,
		ExecutionOptions* someExecutionOptions);

	virtual long refresh(
		unsigned long maxAge,
		ExecutionOptions* someExecutionOptions);

	virtual void handleDataChanged(
		const std::vector<DaItem*>& items,
		const std::vector<ValueQT*>& values,
		const std::vector<long>& results);

	virtual void handleDataChangedEx(
		unsigned long executionContext,
		const std::vector<DaItem*>& items,
		const std::vector<ValueQT*>& values,
		const std::vector<long>& results);

	virtual void handleReadCompleted(
		unsigned long executionContext,
		const std::vector<DaItem*>& items,
		const std::vector<ValueQT*>& values,
		const std::vector<long>& results,
		long result);

	virtual void handleWriteCompleted(
		unsigned long executionContext,
		const std::vector<DaItem*>& items,
		const std::vector<ValueQT*>& values,
		const std::vector<long>& results,
		long result);

	virtual void handleValidateDaItemsCompleted(
		unsigned long executionContext,
		std::vector<DaItem*>& items,
		std::vector<long>& results);

	virtual void handleGetDaItemAttributesFromServerCompleted(
		unsigned long executionContext,
		std::vector<DaItem*>& items,
		std::vector<EnumObjectAttribute>& whatAttributes,
		std::vector<long>& results,
		long result);

	virtual void handleSetDaItemAttributesToServerCompleted(
		unsigned long executionContext,
		std::vector<DaItem*>& items,
		std::vector<EnumObjectAttribute>& whatAttributes,
		std::vector<long>& results,
		long result);

}; // end class DaSubscription

typedef std::pair <unsigned long, DaSubscription*> DaSubscriptionPair;

}; //end namespace OPC_Client


#pragma pack(pop)
#endif
