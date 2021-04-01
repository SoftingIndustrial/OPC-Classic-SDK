#ifndef _CLIENTDASESSION_H_
#define _CLIENTDASESSION_H_

#include "../../Enums.h"
#include "../ClientEnums.h"
#include "../../ValueQT.h"
#include "../ClientObjectSpaceElement.h"
#include "ClientDaAddressSpaceElement.h"
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

class DaSubscription;
class DaAddressSpaceElementBrowseOptions;
class DaProperty;
class DaGetPropertiesOptions;
class ServerStatus;

class TBC_EXPORT DaSession : virtual public ObjectSpaceElement
{

	friend class OTCGlobals;
	friend class DaSubscription;
	friend class DaAddressSpaceElement;

private:

	unsigned char onShutdown(const tstring& reason);

	void onReadComplete(
		unsigned long executionContext,
		const std::vector<tstring>& itemIds,
		const std::vector<tstring>& itemPaths,
		const std::vector<ValueQT*>& values,
		const std::vector<long>& results,
		long result);

	void onWriteComplete(
		unsigned long executionContext,
		const std::vector<tstring>& itemIds,
		const std::vector<tstring>& itemPaths,
		const std::vector<ValueQT*>& values,
		const std::vector<long>& results,
		long result);

	void onBrowse(
		unsigned long executionContext,
		unsigned long addressSpaceElementHandle,
		tstring& addressSpaceElementId,
		tstring& addressSpaceElementPath,
		std::vector<DaAddressSpaceElement*>& addressSpaceElements,
		long result);

	void onGetDaProperties(
		unsigned long executionContext,
		unsigned long addressSpaceElementHandle,
		tstring& addressSpaceElementId,
		tstring& addressSpaceElementPath,
		std::vector<DaProperty*>& someDaProperties,
		long result);

	void onGetServerStatus(
		unsigned long executionContext,
		ServerStatus& aServerStatus,
		long result);

	void onGetErrorString(
		unsigned long executionContext,
		long errorCode,
		tstring& errorString,
		long result);

	void onLogon(
		unsigned long executionContext,
		tstring& userName,
		tstring& password,
		long result);

	void onLogoff(
		unsigned long executionContext,
		long result);

	virtual long addDaSubscription(
		unsigned long updateRate,
		DaSubscription* aSubscription);

	virtual long getDaProperties(
		DaAddressSpaceElement* addressSpaceElement,
		tstring anAddressSpaceElementId,
		tstring anAddressSpaceElementPath,
		DaGetPropertiesOptions* aGetPropertyOptions,
		std::vector<DaProperty*>& someDaProperties,
		ExecutionOptions* someExecutionOptions);

	DaSubscription* findSubscription(unsigned long position);

protected:

	std::map<unsigned long, DaSubscription*> m_daSubscriptionList;
	Mutex m_daSubscriptionListJanitor;

public:

	DaSession(const tstring& url);
	virtual ~DaSession();

	std::vector<DaSubscription*> getSubscriptionList()
	{
		m_daSubscriptionListJanitor.lock();
		std::vector<DaSubscription*> subscriptionList(m_daSubscriptionList.size(), NULL);
		std::map<unsigned long, DaSubscription*>::const_iterator daSubscriptionIterator;
		unsigned long i = 0;

		for (daSubscriptionIterator = m_daSubscriptionList.begin(); daSubscriptionIterator != m_daSubscriptionList.end(); daSubscriptionIterator ++)
		{
			if (daSubscriptionIterator->second != NULL)
			{
				subscriptionList[i] = daSubscriptionIterator->second;
				i++;
			} //end if
		} //end for

		m_daSubscriptionListJanitor.unlock();
		return subscriptionList;
	} // end getSubscriptionList

	virtual tstring getUrl();

	virtual tstring getLocaleId();
	virtual void setLocaleId(tstring localeId);

	virtual tstring getClientName();
	virtual void setClientName(tstring clientName);

	virtual unsigned long getRequestTimeout();
	virtual void setRequestTimeout(unsigned long requestTimeout);

	virtual EnumOPCSpecification getSupportedOpcSpecification();

	virtual BOOL getPerformInitialRead();
	virtual void setPerformInitialRead(BOOL performInitialRead);

	virtual EnumOPCSpecification getForcedOpcSpecification();
	virtual void setForcedOpcSpecification(EnumOPCSpecification anOpcSpecification);

	virtual EnumClassContext getClassContext();
	virtual void setClassContext(EnumClassContext aClassContext);

	virtual long removeDaSubscription(DaSubscription* aSubscription);

	virtual long getErrorString(
		long errorCode,
		tstring& errorText,
		ExecutionOptions* executionOptions);

	virtual long read(
		unsigned long maxAge,
		std::vector<tstring> itemIds,
		std::vector<tstring> itemPaths,
		std::vector<ValueQT*>& values,
		std::vector<long>& results,
		ExecutionOptions* someExecutionOptios);

	virtual long write(
		std::vector<tstring> itemIds,
		std::vector<tstring> itemPaths,
		std::vector<ValueQT*>& values,
		std::vector<long>& results,
		ExecutionOptions* someExecutionOptions);

	virtual long browse(
		tstring rootId,
		tstring rootPath,
		DaAddressSpaceElementBrowseOptions* someBrowseOptions,
		std::vector<DaAddressSpaceElement*>& addressSpaceElements,
		ExecutionOptions* someExecutionOptions);

	virtual long getDaProperties(
		tstring anAddressSpaceElementId,
		tstring anAddressSpaceElementPath,
		DaGetPropertiesOptions* aGetPropertyOptions,
		std::vector<DaProperty*>& someDaProperties,
		ExecutionOptions* someExecutionOptions);

	virtual long getStatus(
		ServerStatus& aServerStatus,
		ExecutionOptions* someExecutionOptions);

	virtual long activateConnectionMonitor(
		BOOL activate,
		unsigned long checkCycle,
		unsigned long connect1Attempts,
		unsigned long connect1Cycle,
		unsigned long connect2Cycle);

	virtual long logon(
		tstring userName,
		tstring password,
		ExecutionOptions* someExecutionOptions);

	virtual long logoff(ExecutionOptions* executionOptions);

	virtual unsigned char handleShutdownRequest(const tstring& reason);

	virtual void handleReadCompleted(
		unsigned long executionContext,
		const std::vector<tstring>& itemIds,
		const std::vector<tstring>& itemPaths,
		const std::vector<ValueQT*>& values,
		const std::vector<long>& results,
		long result);

	virtual void handleWriteCompleted(
		unsigned long executionContext,
		const std::vector<tstring>& itemIds,
		const std::vector<tstring>& itemPaths,
		const std::vector<ValueQT*>& values,
		const std::vector<long>& results,
		long result);

	virtual void handleBrowseCompleted(
		unsigned long executionContext,
		unsigned long addressSpaceElementHandle,
		tstring& addressSpaceElementId,
		tstring& addressSpaceElementPath,
		std::vector<DaAddressSpaceElement*>& addressSpaceElements,
		long result);

	virtual void handleGetDaPropertiesCompleted(
		unsigned long executionContext,
		unsigned long addressSpaceElementHandle,
		tstring& addressSpaceElementId,
		tstring& addressSpaceElementPath,
		std::vector<DaProperty*>& someDaProperties,
		long result);

	virtual void handleGetStatusCompleted(
		unsigned long executionContext,
		ServerStatus& aServerStatus,
		long result);

	virtual void handleGetErrorStringCompleted(
		unsigned long executionContext,
		long errorCode,
		tstring& errorString,
		long result);

	virtual void handleLogonCompleted(
		unsigned long executionContext,
		tstring& userName,
		tstring& password,
		long result);

	virtual void handleLogoffCompleted(
		unsigned long executionContext,
		long result);

}; // end class DaSession

typedef std::pair <unsigned long, DaSession*> DaSessionPair;

}; //end namespace OPC_Client


#pragma pack(pop)
#endif
