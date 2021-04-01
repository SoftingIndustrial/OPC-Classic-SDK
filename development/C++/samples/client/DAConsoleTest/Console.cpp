#include "OSCompat.h"
#include "Enums.h"

#include "ClientApplication.h"
#include "ClientCommon.h"

#include "ClientDaSubscription.h"
#include "ClientDaItem.h"
#include "ClientServerStatus.h"

#include <ValueQT.h>
#include<iostream>

#ifdef TBC_OS_WINDOWS
#include <conio.h>
#endif

#include <stdio.h>

#ifdef TBC_OS_LINUX
#include <pthread.h>
#include <semaphore.h>
#include <errno.h>
#endif

using namespace std;
using namespace SoftingOPCToolboxClient;

#ifdef TBC_OS_WINDOWS
HANDLE g_endEvent = 0;
#endif
#ifdef TBC_OS_LINUX
sem_t g_endSemaphor;
#endif

#ifdef TBC_OS_WINDOWS
//-----------------------------------------------------------------------------
// controlHandler
//
// - handle console control events
//
// returns:
//		TRUE  - handled event
//		FALSE - event not handled
//
BOOL WINAPI controlHandler(
	IN unsigned long crtlType)  // control type
{
	switch (crtlType)
	{
	case CTRL_BREAK_EVENT:  // use Ctrl+C or Ctrl+Break to end server
	case CTRL_C_EVENT:
	{
		if (g_endEvent)
		{
			SetEvent(g_endEvent);
		}

		return TRUE;
	}
	break;
	}

	return FALSE;
} // controlHandler
#endif


void usage(void)
{
	_tprintf(_T("\nUsage:\n"));
	_tprintf(_T("Use lower case letters for synchronous execution\n"));
	_tprintf(_T("Use upper case letters for asynchronous execution\n"));
	_tprintf(_T("Press \'a\' or \'A\' to activate the objects\n"));
	_tprintf(_T("Press \'c\' or \'C\' to connect the objects\n"));
	_tprintf(_T("Press \'d\' or \'D\' to disconnect the objects\n"));
	_tprintf(_T("Press \'r\' or \'R\' to read the items\n"));
	_tprintf(_T("Press \'w\' or \'W\' to write the items\n"));
	_tprintf(_T("Press \'f\' or \'F\' to refresh the items\n"));
	_tprintf(_T("Press \'z\' or \'Z\' to read using server object\n"));
	_tprintf(_T("Press \'t\' or \'T\' to write using the server object\n"));
	_tprintf(_T("Press \'s\' or \'S\' to get the server status\n"));
	_tprintf(_T("Press \'m\' to activate the connection monitor\n"));
	_tprintf(_T("Press \'n\' to deactivate the connection monitor\n"));
	_tprintf(_T("Press \'?\' or \'u\' to display this usage information\n"));
	_tprintf(_T("Press \'e\' or \'q\' or Ctrl-C to exit\n\n"));
}

class MyDaSession : public DaSession
{

public:

	MyDaSession(const tstring& url): DaSession(url) {}
	unsigned char handleShutdownRequest(const tstring& reason)
	{
		_tprintf(_T("Session shutdown - reason: %s\n"), reason.c_str());
		return TRUE; // reconnect automatically
	}
	void handleStateChangeCompleted(
		EnumObjectState state)
	{
		tstring stateToString;

		switch (state)
		{
		case EnumObjectState_DISCONNECTED:
			stateToString = _T("DISCONNECTED");
			break;

		case EnumObjectState_CONNECTED:
			stateToString = _T("CONNECTED");
			break;

		case EnumObjectState_ACTIVATED:
			stateToString = _T("ACTIVATED");
		} //end switch

		_tprintf(_T("\n%s state changed - %s\n"), _T("Session"), stateToString.c_str());
	} //end handleStateChange

	void handlePerformStateTransitionCompleted(
		unsigned long executionContext,
		long result)
	{
		_tprintf(_T("\n"));

		if (SUCCEEDED(result))
		{
			_tprintf(_T("%s performed state transition - context: %lu\n"), _T("Session"), executionContext);
		} //end if
		else
		{
			_tprintf(_T(" performed state transition failed"));
		}
	} //end handlePerformStateTransitionCompleted


	void handleReadCompleted(
		unsigned long executionContext,
		const std::vector<tstring>& itemIds,
		const std::vector<tstring>& itemPaths,
		const std::vector<ValueQT*>& values,
		const std::vector<long>& results,
		long result)
	{
		unsigned long i;
		size_t itemCount = itemIds.size();
		_tprintf(_T("\n Session - Read complete\n"));

		if (SUCCEEDED(result))
		{
			for (i = 0; i < itemCount; i++)
			{
				if (SUCCEEDED(results[i]))
				{
					_tprintf(_T("%-23.23s - %s\n"), itemIds[i].c_str(), values[i]->toString().c_str());
				}
				else
				{
					_tprintf(_T("Read failed - %s [0x%8.8X]\n"), itemIds[i].c_str(), (unsigned int)results[i]);
				}//end if...else
			}//end for
		}
		else
		{
			_tprintf(_T("Read failed [0x%8.8X]\n"), (unsigned int)result);
		}//end if...else
	} //end handleReadCompleted


	void handleWriteCompleted(
		unsigned long executionContext,
		const std::vector<tstring>& itemIds,
		const std::vector<tstring>& itemPaths,
		const std::vector<ValueQT*>& values,
		const std::vector<long>& results,
		long result)
	{
		_tprintf(_T("\nSession write complete\n"));

		if (SUCCEEDED(result))
		{
			unsigned long i;
			size_t itemCount = itemIds.size();

			for (i = 0; i < itemCount; i++)
			{
				if (SUCCEEDED(results[i]))
				{
					_tprintf(_T("%-23.23s - %s\n"), itemIds[i].c_str(), values[i]->toString().c_str());
				}
				else
				{
					_tprintf(_T("Write failed - %s [0x%8.8X]\n"), itemIds[i].c_str(), (unsigned int)results[i]);
				} //end if ... else
			} //end for
		}
		else
		{
			_tprintf(_T("Write failed [0x%8.8X]\n"), (unsigned int)result);
		} //end if... else
	} //end handleWriteCompleted

	void handleGetStatusCompleted(
		unsigned long executionContext,
		ServerStatus& aServerStatus,
		long result)
	{
		if (SUCCEEDED(result))
		{
			unsigned long i;
			_tprintf(_T("\nServer Status\n"));
			_tprintf(_T("   State: %ld\n"), aServerStatus.getState());
			_tprintf(_T("   Vendor info: %s\n"), aServerStatus.getVendorInfo().c_str());
			_tprintf(_T("   Product version: %s\n"), aServerStatus.getProductVersion().c_str());
			_tprintf(_T("   Start time: %s\n"), aServerStatus.getStartTime().toString().c_str());
			_tprintf(_T("   Last update time: %s\n"), aServerStatus.getLastUpdateTime().toString().c_str());
			_tprintf(_T("   Current time: %s\n"), aServerStatus.getCurrentTime().toString().c_str());
			_tprintf(_T("   Group count: %ld\n"), aServerStatus.getGroupCount());
			_tprintf(_T("   Bandwidth: %ld\n"), aServerStatus.getBandwidth());
			std::vector<tstring>supportedLcIds = aServerStatus.getSupportedLcIds();

			for (i = 0; i < supportedLcIds.size(); i++)
			{
				_tprintf(_T("   Supported LCID: %s\n"), supportedLcIds[i].c_str());
			}

			_tprintf(_T("   Status info: %s\n"), aServerStatus.getStatusInfo().c_str());
		}
		else
		{
			_tprintf(_T("\nGet Status failed [0x%8.8X]\n"), (unsigned int)result);
		} //end else ... if
	} //end handleGetStatusCompleted

}; //end MySession

class MyDaSubscription : public DaSubscription
{

public:

	MyDaSubscription(
		unsigned long updateRate,
		MyDaSession* parentSession)
		: DaSubscription(
			updateRate,
			parentSession) {}

	void handleStateChangeCompleted(
		EnumObjectState state)
	{
		tstring stateToString;

		switch (state)
		{
		case EnumObjectState_DISCONNECTED:
			stateToString = _T("DISCONNECTED");
			break;

		case EnumObjectState_CONNECTED:
			stateToString = _T("CONNECTED");
			break;

		case EnumObjectState_ACTIVATED:
			stateToString = _T("ACTIVATED");
		} //end switch

		_tprintf(_T("%s state changed - %s\n"), _T("Subscription"), stateToString.c_str());
	} //end handleStateChangeCompleted


	void handlePerformStateTransitionCompleted(
		unsigned long executionContext,
		long result)
	{
		_tprintf(_T("\n"));

		if (SUCCEEDED(result))
		{
			_tprintf(_T("%s performed state transition - context: %lu\n"), _T("Subscription"), executionContext);
		} //end if
		else
		{
			_tprintf(_T(" performed state transition failed"));
		}
	} //handlePerformStateTransitionCompleted


	void handleDataChanged(
		const std::vector<DaItem*>& items,
		const std::vector<ValueQT*>& values,
		const std::vector<long>& results)
	{
		unsigned long i;
		size_t count = items.size();
		_tprintf(_T("\n%s - Data changed\n"), _T("Subscription"));

		for (i = 0; i < count; i++)
		{
			_tprintf(_T("%-23.23s - %s\n"), items[i]->getId().c_str(), values[i]->toString().c_str());
		} //end for
	}

	void handleReadCompleted(
		unsigned long executionContext,
		const std::vector<DaItem*>& items,
		const std::vector<ValueQT*>& values,
		const std::vector<long>& results,
		long result)
	{
		unsigned long i;
		size_t itemCount = items.size();
		_tprintf(_T("\n Subscripion - Read complete\n"));

		if (SUCCEEDED(result))
		{
			for (i = 0; i < itemCount; i++)
			{
				tstring id = items[i]->getId();

				if (SUCCEEDED(results[i]))
				{
					_tprintf(_T("%-23.23s - %s\n"), id.c_str(), values[i]->toString().c_str());
				}
				else
				{
					_tprintf(_T("Read failed - %s [0x%8.8X]\n"), id.c_str(), (unsigned int)results[i]);
				}//end if...else
			}//end for
		}
		else
		{
			_tprintf(_T("Read failed [0x%8.8X]\n"), (unsigned int)result);
		}//end if...else
	} //end handleReadCompleted

	void  handleWriteCompleted(
		unsigned long executionContext,
		const std::vector<DaItem*>& items,
		const std::vector<ValueQT*>& values,
		const std::vector<long>& results,
		long result)
	{
		_tprintf(_T("\nSubscription write complete "));

		if (SUCCEEDED(result))
		{
			unsigned long i;
			size_t itemCount = items.size();

			for (i = 0; i < itemCount; i++)
			{
				if (SUCCEEDED(results[i]))
				{
					_tprintf(_T("%-23.23s \n"), items[i]->getId().c_str());
				}
				else
				{
					_tprintf(_T("Write failed - %s [0x%8.8X]\n"), items[i]->getId().c_str(), (unsigned int)results[i]);
				}
			}
		}
		else
		{
			_tprintf(_T("Write failed [0x%8.8X]\n"), (unsigned int)result);
		}
	} //end handleWriteCompleted
};


class MyDaItem : public DaItem
{

public:

	MyDaItem(
		tstring itemId,
		MyDaSubscription* parentSubscription)
		: DaItem(
			itemId,
			parentSubscription) {}

	void handleStateChangeCompleted(
		EnumObjectState state)
	{
		tstring stateToString;

		switch (state)
		{
		case EnumObjectState_DISCONNECTED:
			stateToString = _T("DISCONNECTED");
			break;

		case EnumObjectState_CONNECTED:
			stateToString = _T("CONNECTED");
			break;

		case EnumObjectState_ACTIVATED:
			stateToString = _T("ACTIVATED");
		} //end switch

		_tprintf(_T("%s state changed - %s\n"), this->getId().c_str(), stateToString.c_str());
	} //end handleStateChange


	void handlePerformStateTransitionCompleted(
		unsigned long executionContext,
		long result)
	{
		_tprintf(_T("\n"));

		if (SUCCEEDED(result))
		{
			_tprintf(_T("%s performed state transition - context: %lu\n"), this->getId().c_str() , executionContext);
		} //end if
		else
		{
			_tprintf(_T(" performed state transition failed"));
		}
	} //handlePerformStateTransitionCompleted


};

int _tmain(int argc, _TCHAR* argv[])
{
	ExecutionOptions executionOptions;
	unsigned long executionContext;
	unsigned long readCount = 4;
	unsigned long writeCount = 1;
	unsigned long i;
	unsigned long updateCount = 2;
	long result = S_OK;
	ValueQT itemValue;
	long itemResult;
	tstring errorText;
	tstring supportedSpecificationToString;
	tstring forcedSpecificationToString;
	std::vector<tstring> itemIdsForRead;
	std::vector<tstring> itemPathsForRead;
	std::vector<tstring> itemIdsForWrite;
	std::vector<tstring> itemPathsForWrite;
	std::vector<ValueQT*> valuesToRead(readCount, NULL);
	std::vector<ValueQT*>::iterator valuesToReadIterator;
	std::vector<DaItem*>itemsToRead(readCount, NULL);
	std::vector<DaItem*> itemsToWrite(writeCount, NULL);
	std::vector<ValueQT*> valuesToWrite(writeCount, NULL);
	std::vector<long> readResults(readCount, E_FAIL);
	std::vector<long> writeResults(writeCount, E_FAIL);
	std::vector<EnumObjectAttribute> whatAttributes;
	std::vector<long>updateResults(updateCount, E_FAIL);
	whatAttributes.push_back(EnumObjectAttribute_DASESSION_CLIENTNAME);
	whatAttributes.push_back(EnumObjectAttribute_DASESSION_LCID);
	ServerStatus serverStatus;
	Variant data;
	DateTime writeDateTime;
	ValueQT* aValue = NULL;
	executionOptions.setExecutionType(EnumExecutionType_ASYNCHRONOUS);
	executionOptions.setExecutionContext(0);
	executionContext = executionOptions.getExecutionContext();
#ifdef TBC_OS_WINDOWS
	//itemIdsForRead.push_back(_T("increment.UI1"));
	//itemIdsForRead.push_back(_T("maths.sin"));
	//itemIdsForRead.push_back(_T("time.local.second"));
	//itemIdsForRead.push_back(_T("increment.BSTR"));
	//itemIdsForWrite.push_back(_T("increment.BSTR"));
	//itemPathsForWrite.push_back(_T("increment.BSTR"));
	// For AddressSpace server
	//itemIdsForRead.push_back(_T("objectBased.T1"));
	//itemIdsForRead.push_back(_T("objectBased.T2"));
	//itemIdsForRead.push_back(_T("stringBased.N1.T1"));
	//itemIdsForRead.push_back(_T("stringBased.N1"));
	//itemIdsForWrite.push_back(_T("stringBased.N1"));
	//itemPathsForWrite.push_back(_T("stringBased.N1"));
	// For Console server
	//itemIdsForRead.push_back(_T("maths/sin"));
	//itemIdsForRead.push_back(_T("random/100ms"));
	//itemIdsForRead.push_back(_T("random/1s"));
	//itemIdsForRead.push_back(_T("accept"));
	//itemIdsForWrite.push_back(_T("accept"));
	//itemPathsForWrite.push_back(_T("accept"));
	// For ctt server
	//itemIdsForRead.push_back(_T("CTT.R8"));
	//itemIdsForRead.push_back(_T("CTT.UI1"));
	//itemIdsForRead.push_back(_T("CTT.UI2"));
	//itemIdsForRead.push_back(_T("CTT.BSTR"));
	//itemIdsForWrite.push_back(_T("CTT.BSTR"));
	//itemPathsForWrite.push_back(_T("CTT.BSTR"));
	// For DemoServer server
	//itemIdsForRead.push_back(_T("all data types.dynamic.UI1"));
	//itemIdsForRead.push_back(_T("all data types.dynamic.R4"));
	//itemIdsForRead.push_back(_T("all data types.dynamic.UI2"));
	//itemIdsForRead.push_back(_T("all data types.dynamic.BSTR"));
	//itemIdsForWrite.push_back(_T("all data types.dynamic.BSTR"));
	//itemPathsForWrite.push_back(_T("all data types.dynamic.BSTR"));
	// For Session server
	//itemIdsForRead.push_back(_T("secured write"));
	//itemIdsForRead.push_back(_T("secured write"));
	//itemIdsForRead.push_back(_T("secured write"));
	//itemIdsForRead.push_back(_T("client specific"));
	//itemIdsForWrite.push_back(_T("client specific"));
	//itemPathsForWrite.push_back(_T("client specific"));
	// For SerialIO server
	//temIdsForRead.push_back(_T("Simulation1"));
	//itemIdsForRead.push_back(_T("Simulation1"));
	//itemIdsForRead.push_back(_T("Simulation1"));
	//itemIdsForRead.push_back(_T("Simulation2"));
	//itemIdsForWrite.push_back(_T("Simulation2"));
	//itemPathsForWrite.push_back(_T("Simulation2"));
	// For UpdateMode server
	itemIdsForRead.push_back(_T("clock.poll.minute"));
	itemIdsForRead.push_back(_T("clock.poll.second"));
	itemIdsForRead.push_back(_T("clock.report.minute"));
	itemIdsForRead.push_back(_T("clock.report.second"));
	itemIdsForWrite.push_back(_T("clock.report.second"));
	itemPathsForWrite.push_back(_T("clock.report.second"));
#endif
#ifdef TBC_OS_LINUX
	// For DemoServer server
	//itemIdsForRead.push_back(_T("all data types.dynamic.UI1"));
	//itemIdsForRead.push_back(_T("all data types.dynamic.R4"));
	//itemIdsForRead.push_back(_T("all data types.dynamic.UI2"));
	//itemIdsForRead.push_back(_T("all data types.dynamic.BSTR"));
	//itemIdsForWrite.push_back(_T("all data types.dynamic.BSTR"));
	//itemPathsForWrite.push_back(_T("all data types.dynamic.BSTR"));
	// For AddressSpace server
	//itemIdsForRead.push_back(_T("objectBased.T1"));
	//itemIdsForRead.push_back(_T("objectBased.T2"));
	//itemIdsForRead.push_back(_T("stringBased.N1.T1"));
	//itemIdsForRead.push_back(_T("stringBased.N1"));
	//itemIdsForWrite.push_back(_T("stringBased.N1"));
	//itemPathsForWrite.push_back(_T("stringBased.N1"));
	// For Console server
	//itemIdsForRead.push_back(_T("maths/sin"));
	//itemIdsForRead.push_back(_T("random/100ms"));
	//itemIdsForRead.push_back(_T("random/1s"));
	//itemIdsForRead.push_back(_T("accept"));
	//itemIdsForWrite.push_back(_T("accept"));
	//itemPathsForWrite.push_back(_T("accept"));
	// For ctt server
	//itemIdsForRead.push_back(_T("CTT.R8"));
	//itemIdsForRead.push_back(_T("CTT.UI1"));
	//itemIdsForRead.push_back(_T("CTT.UI2"));
	//itemIdsForRead.push_back(_T("CTT.BSTR"));
	//itemIdsForWrite.push_back(_T("CTT.BSTR"));
	//itemPathsForWrite.push_back(_T("CTT.BSTR"));
	// For Session server
	//itemIdsForRead.push_back(_T("secured write"));
	//itemIdsForRead.push_back(_T("secured write"));
	//itemIdsForRead.push_back(_T("secured write"));
	//itemIdsForRead.push_back(_T("client specific"));
	//itemIdsForWrite.push_back(_T("client specific"));
	//itemPathsForWrite.push_back(_T("client specific"));
	// For SerialIO server
	itemIdsForRead.push_back(_T("Simulation1"));
	itemIdsForRead.push_back(_T("Simulation1"));
	itemIdsForRead.push_back(_T("Simulation1"));
	itemIdsForRead.push_back(_T("Simulation2"));
	itemIdsForWrite.push_back(_T("Simulation2"));
	itemPathsForWrite.push_back(_T("Simulation2"));
	// For UpdateMode server
	//itemIdsForRead.push_back(_T("clock.poll.minute"));
	//itemIdsForRead.push_back(_T("clock.poll.second"));
	//itemIdsForRead.push_back(_T("clock.report.minute"));
	//itemIdsForRead.push_back(_T("clock.report.second"));
	//itemIdsForWrite.push_back(_T("clock.report.second"));
	//itemPathsForWrite.push_back(_T("clock.report.second"));
#endif
#ifdef TBC_OS_WINDOWS
	unsigned long waitRet;
	g_endEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
#endif
	SoftingOPCToolboxClient::Application* application = getApplication();
	application->setVersionOtb(447);
	application->enableTracing(
		EnumTraceGroup_ALL,
		EnumTraceGroup_ALL,
		EnumTraceGroup_CLIENT,
		EnumTraceGroup_CLIENT,
		_T("Trace.txt"),
		1000000,
		2);

	//  TODO - binary license activation
	//  Fill in your binary license activation keys here
	//
	//  NOTE: you can activate one or all of the features at the same time
	//  firstly activate the COM-DA Client feature
	// result = getApplication()->activate(EnumFeature_DA_CLIENT, "XXXX-XXXX-XXXX-XXXX-XXXX");

	if (!SUCCEEDED(result))
	{
		return result;
	}

	//  activate the XML-DA Client Feature
	// result = getApplication()->activate(EnumFeature_XMLDA_CLIENT, "XXXX-XXXX-XXXX-XXXX-XXXX");

	if (!SUCCEEDED(result))
	{
		return result;
	}

	//  END TODO - binary license activation

	if (SUCCEEDED(application->initialize()))
	{
		MyDaSession* session = NULL;
#ifdef TBC_OS_WINDOWS
		session = new MyDaSession(_T("opcda:///Softing.OPCToolboxDemo_ServerDA.1/{2E565242-B238-11D3-842D-0008C779D775}"));
		//session = new MyDaSession(_T("http://localhost:8079/OPC/DA"));
#endif
#ifdef TBC_OS_LINUX
		//  TODO set the COM-DA url here; Note the url must be changed to work properly
		session = new MyDaSession(_T("http://localhost:8079/OPC/DA"));
#endif
#ifdef TBC_OS_WINDOWS
		MyDaSubscription* subscription = new MyDaSubscription(1000, session);
		//MyDaItem* item1 = new MyDaItem( _T("maths.sin"), subscription);
		//MyDaItem* item2 = new MyDaItem( _T("time.local.second"), subscription);
		//MyDaItem* item3 = new MyDaItem( _T("increment.UI1"), subscription);
		//MyDaItem* item4 = new MyDaItem( _T("increment.BSTR"), subscription);
		//For AddressSpace server
		//MyDaItem* item1 = new MyDaItem( _T("objectBased.T1"), subscription);
		//MyDaItem* item2 = new MyDaItem( _T("objectBased.T2"), subscription);
		//MyDaItem* item3 = new MyDaItem( _T("stringBased.N1.T1"), subscription);
		//MyDaItem* item4 = new MyDaItem( _T("stringBased.N1"), subscription);
		//For Console server
		//MyDaItem* item1 = new MyDaItem( _T("maths/sin"), subscription);
		//MyDaItem* item2 = new MyDaItem( _T("random/100ms"), subscription);
		//MyDaItem* item3 = new MyDaItem( _T("random/1s"), subscription);
		//MyDaItem* item4 = new MyDaItem( _T("accept"), subscription);
		//For CTT server
		//MyDaItem* item1 = new MyDaItem( _T("CTT.R8"), subscription);
		//MyDaItem* item2 = new MyDaItem( _T("CTT.UI1"), subscription);
		//MyDaItem* item3 = new MyDaItem( _T("CTT.UI2"), subscription);
		//MyDaItem* item4 = new MyDaItem( _T("CTT.BSTR"), subscription);
		//For DemoServer server
		//MyDaItem* item1 = new MyDaItem( _T("all data types.dynamic.R4"), subscription);
		//MyDaItem* item2 = new MyDaItem( _T("all data types.dynamic.UI1"), subscription);
		//MyDaItem* item3 = new MyDaItem( _T("all data types.dynamic.UI2"), subscription);
		//MyDaItem* item4 = new MyDaItem( _T("all data types.dynamic.BSTR"), subscription);
		//For Session server
		//MyDaItem* item1 = new MyDaItem( _T("secured write"), subscription);
		//MyDaItem* item2 = new MyDaItem( _T("secured write"), subscription);
		//MyDaItem* item3 = new MyDaItem( _T("secured write"), subscription);
		//MyDaItem* item4 = new MyDaItem( _T("client specific"), subscription);
		//For SerialIO server
		//MyDaItem* item1 = new MyDaItem( _T("Simulation1"), subscription);
		//MyDaItem* item2 = new MyDaItem( _T("Simulation1"), subscription);
		//MyDaItem* item3 = new MyDaItem( _T("Simulation1"), subscription);
		//MyDaItem* item4 = new MyDaItem( _T("Simulation2"), subscription);
		//For UpdateMode server
		MyDaItem* item1 = new MyDaItem(_T("clock.poll.minute"), subscription);
		MyDaItem* item2 = new MyDaItem(_T("clock.poll.second"), subscription);
		MyDaItem* item3 = new MyDaItem(_T("clock.report.minute"), subscription);
		MyDaItem* item4 = new MyDaItem(_T("clock.report.second"), subscription);
#endif
#ifdef TBC_OS_LINUX
		//  It is working against DemoServer OPC server running on localhost
		MyDaSubscription* subscription = new MyDaSubscription(1000, session);
		//For DemoServer server
		//MyDaItem* item1 = new MyDaItem( _T("all data types.dynamic.R4"), subscription);
		//MyDaItem* item2 = new MyDaItem( _T("all data types.dynamic.UI1"), subscription);
		//MyDaItem* item3 = new MyDaItem( _T("all data types.dynamic.UI2"), subscription);
		//MyDaItem* item4 = new MyDaItem( _T("all data types.dynamic.BSTR"), subscription);
		//For AddressSpace server
		//MyDaItem* item1 = new MyDaItem( _T("objectBased.T1"), subscription);
		//MyDaItem* item2 = new MyDaItem( _T("objectBased.T2"), subscription);
		//MyDaItem* item3 = new MyDaItem( _T("stringBased.N1.T1"), subscription);
		//MyDaItem* item4 = new MyDaItem( _T("stringBased.N1"), subscription);
		//For Console server
		//MyDaItem* item1 = new MyDaItem( _T("maths/sin"), subscription);
		//MyDaItem* item2 = new MyDaItem( _T("random/100ms"), subscription);
		//MyDaItem* item3 = new MyDaItem( _T("random/1s"), subscription);
		//MyDaItem* item4 = new MyDaItem( _T("accept"), subscription);
		//For CTT server
		//MyDaItem* item1 = new MyDaItem( _T("CTT.R8"), subscription);
		//MyDaItem* item2 = new MyDaItem( _T("CTT.UI1"), subscription);
		//MyDaItem* item3 = new MyDaItem( _T("CTT.UI2"), subscription);
		//MyDaItem* item4 = new MyDaItem( _T("CTT.BSTR"), subscription);
		//For Session server
		//MyDaItem* item1 = new MyDaItem( _T("secured write"), subscription);
		//MyDaItem* item2 = new MyDaItem( _T("secured write"), subscription);
		//MyDaItem* item3 = new MyDaItem( _T("secured write"), subscription);
		//MyDaItem* item4 = new MyDaItem( _T("client specific"), subscription);
		//For SerialIO server
		MyDaItem* item1 = new MyDaItem(_T("Simulation1"), subscription);
		MyDaItem* item2 = new MyDaItem(_T("Simulation1"), subscription);
		MyDaItem* item3 = new MyDaItem(_T("Simulation1"), subscription);
		MyDaItem* item4 = new MyDaItem(_T("Simulation2"), subscription);
		//For UpdateMode server
		//MyDaItem* item1 = new MyDaItem( _T("clock.poll.minute"), subscription);
		//MyDaItem* item2 = new MyDaItem( _T("clock.poll.second"), subscription);
		//MyDaItem* item3 = new MyDaItem( _T("clock.report.minute"), subscription);
		//MyDaItem* item4 = new MyDaItem( _T("clock.report.second"), subscription);
#endif
		itemsToRead[0] = item1;
		itemsToRead[1] = item2;
		itemsToRead[2] = item3;
		itemsToRead[3] = item4;
		itemsToWrite[0] = item4;
		session->connect(TRUE, FALSE, NULL);
		usage();
		unsigned long waitTime = 100;
#ifdef TBC_OS_WINDOWS
		bool end = FALSE;

		while (!end)
		{
			waitRet = WaitForSingleObject(g_endEvent, waitTime);

			if (waitRet == WAIT_OBJECT_0)
			{
				end = TRUE;
				CloseHandle(g_endEvent);
			}
			else
			{
#endif
#ifdef TBC_OS_LINUX
		unsigned long timeout = 0;

		while (timeout < waitTime)
		{
			timespec delay;
			delay.tv_sec = 0;
			delay.tv_nsec = 100000000;  /* 100 milli sec */
			// Wait for the event be signaled
			int retCode = sem_trywait(&g_endSemaphor); // event semaphore handle

			if (!retCode)
			{
				/* Event is signaled */
				break;
			}
			else
			{
				/* check whether somebody else has the mutex */
				if (retCode == EPERM)
				{
					/* sleep for delay time */
					nanosleep(&delay, NULL);
					timeout++ ;
				}

#endif
#ifdef TBC_OS_WINDOWS

				if (_kbhit())
				{
					int input;
					input = _getch();
#endif
#ifdef TBC_OS_LINUX
					char input;

				if (1)
				{
					std::cin.get(input);
#endif

					switch (input)
					{
					case _T('A'):
						session->connect(TRUE, TRUE, &executionOptions);
						executionOptions.setExecutionContext(++executionContext);
						break;

					case _T('a'):
						session->connect(TRUE, TRUE, NULL);
						break;

					case _T('C'):
						session->connect(TRUE, FALSE, &executionOptions);
						executionOptions.setExecutionContext(++executionContext);
						break;

					case _T('c'):
						session->connect(TRUE, FALSE, NULL);
						break;

					case _T('D'):
						session->disconnect(&executionOptions);
						executionOptions.setExecutionContext(++executionContext);
						break;

					case _T('d'):
						session->disconnect(NULL);
						break;

					case _T('R'):
						if (FAILED(result = subscription->read(
												0,
												itemsToRead,
												valuesToRead,
												readResults,
												&executionOptions)))
						{
							_tprintf(_T(" Subscription read failed "));
						} //end if
						else
						{
							executionOptions.setExecutionContext(++executionContext);
						} //end if ... else

						break;

					case _T('r'):
						item1->read(100,
									itemValue,
									itemResult,
									NULL);

						if (SUCCEEDED(result = subscription->read(
												   0,
												   itemsToRead,
												   valuesToRead,
												   readResults,
												   NULL)))
						{
							_tprintf(_T(" \nRead items using subscription \n"));

							for (i = 0; i < readCount; i++)
							{
								if (SUCCEEDED(readResults[i]))
								{
									_tprintf(_T("%-23.23s - %s\n"), itemsToRead[i]->getId().c_str(), (valuesToRead[i])->toString().c_str());
								} //end if
								else
								{
									_tprintf(_T("Read failed - %s [0x%8.8X]\n"), itemsToRead[i]->getId().c_str() , (unsigned int)readResults[i]);
								} //end else
							} //end for

							for (valuesToReadIterator = valuesToRead.begin(); valuesToReadIterator != valuesToRead.end(); valuesToReadIterator++)
							{
								if (*valuesToReadIterator != NULL)
								{
									delete *valuesToReadIterator;
								}
							} //end for
						} //end if
						else
						{
							_tprintf(_T("Read failed [0x%8.8X]\n"), (unsigned int)result);
						} //end else

						break;

					case _T('F'):
					case _T('f'):

						//the refresh can be done only asynchronously!
						if (FAILED(result = subscription->refresh(0, &executionOptions)))
						{
							_tprintf(_T("\nRefresh failed [0x%8.8X]\n"), (unsigned int)result);
						}//end if
						else
						{
							executionOptions.setExecutionContext(++executionContext);
						}

						break;

					case _T('Z'):
						if (FAILED(result = session->read(
												0,
												itemIdsForRead,
												itemPathsForRead,
												valuesToRead,
												readResults,
												&executionOptions)))
						{
							_tprintf(_T(" Session read failed "));
						}//end if
						else
						{
							executionOptions.setExecutionContext(++executionContext);
						} //end if ... else

						break;

					case _T('z'):
						if (SUCCEEDED(result = session->read(
												   0,
												   itemIdsForRead,
												   itemPathsForRead,
												   valuesToRead,
												   readResults,
												   NULL)))
						{
							_tprintf(_T("\nRead items using session\n "));

							for (i = 0; i < readCount; i++)
							{
								if (SUCCEEDED(readResults[i]))
								{
									_tprintf(_T("%-23.23s - %s\n"), itemIdsForRead[i].c_str(), (valuesToRead[i])->toString().c_str());
								} //end if
								else
								{
									_tprintf(_T("Read failed - %s [0x%8.8X]\n"), itemIdsForRead[i].c_str() , (unsigned int)readResults[i]);
								} //end else
							} //end for

							for (valuesToReadIterator = valuesToRead.begin(); valuesToReadIterator != valuesToRead.end(); valuesToReadIterator++)
							{
								if (*valuesToReadIterator != NULL)
								{
									delete *valuesToReadIterator;
								}
							} //end for
						} //end if
						else
						{
							_tprintf(_T("Read failed [0x%8.8X]\n"), (unsigned int)result);
						} //end else

						break;

					case _T('W'):
						data.setBSTR(_T("Softing"));
						aValue = new ValueQT(data, EnumQuality_QUALITY_NOT_SET, writeDateTime);
						valuesToWrite[0] = aValue;

						if (FAILED(result = subscription->write(
												itemsToWrite,
												valuesToWrite,
												writeResults,
												&executionOptions)))
						{
							_tprintf(_T(" Subscription write failed "));
						}
						else
						{
							executionOptions.setExecutionContext(++executionContext);
						} //end else ... if

						delete aValue;
						break;

					case _T('w'):
						data.setBSTR(_T("Softing"));
						aValue = new ValueQT(data, EnumQuality_QUALITY_NOT_SET, writeDateTime);
						valuesToWrite[0] = aValue;
						result = subscription->write(
									 itemsToWrite,
									 valuesToWrite,
									 writeResults,
									 NULL);

						if (SUCCEEDED(result))
						{
							_tprintf(_T(" \nWrite items using subscription \n"));

							for (i = 0; i < writeCount; i++)
							{
								if (SUCCEEDED(writeResults[i]))
								{
									_tprintf(_T("%-23.23s - %s\n"), itemsToWrite[i]->getId().c_str(), (valuesToWrite[i])->toString().c_str());
								} //end if
								else
								{
									_tprintf(_T("Write failed - %s [0x%8.8X]\n"), itemsToWrite[i]->getId().c_str(), (unsigned int)writeResults[i]);
								} //end else
							} //end for
						} //end if
						else
						{
							_tprintf(_T("\nWrite failed [0x%8.8X]\n"), (unsigned int)result);
						} //end else

						delete aValue;
						break;

					case _T('T'):
						data.setBSTR(_T("Softing"));
						aValue = new ValueQT(data, EnumQuality_QUALITY_NOT_SET, writeDateTime);
						valuesToWrite[0] = aValue;

						if (FAILED(result = session->write(
												itemIdsForWrite,
												itemPathsForWrite,
												valuesToWrite,
												writeResults,
												&executionOptions)))
						{
							_tprintf(_T(" \n Session write failed \n"));
						}
						else
						{
							executionOptions.setExecutionContext(++executionContext);
						} //end if ... else

						delete aValue;
						break;

					case _T('t'):
						data.setBSTR(_T("Softing"));
						aValue = new ValueQT(data, EnumQuality_QUALITY_NOT_SET, writeDateTime);
						valuesToWrite[0] = aValue;
						result = session->write(
									 itemIdsForWrite,
									 itemPathsForWrite,
									 valuesToWrite,
									 writeResults,
									 NULL);

						if (SUCCEEDED(result))
						{
							_tprintf(_T(" \nWrite items using session \n"));

							for (i = 0; i < writeCount; i++)
							{
								if (SUCCEEDED(writeResults[i]))
								{
									_tprintf(_T("%s - %s\n"), itemIdsForWrite[i].c_str(), (valuesToWrite[i])->toString().c_str());
								} //end if
								else
								{
									_tprintf(_T("Write failed - %s [0x%8.8X]\n"), itemIdsForWrite[i].c_str(), (unsigned int)writeResults[i]);
								} //end else
							} //end for
						} //end if
						else
						{
							_tprintf(_T("\nWrite failed [0x%8.8X]\n"), (unsigned int)result);
						} //end else

						delete aValue;
						break;

					case _T('S'):
						if (FAILED(result = session->getStatus(serverStatus, &executionOptions)))
						{
							_tprintf(_T("Get server status failed"));
						} //end if
						else
						{
							executionOptions.setExecutionContext(++executionContext);
						} //end else

						break;

					case _T('s'):
						if (SUCCEEDED(result = session->getStatus(serverStatus, NULL)))
						{
							_tprintf(_T("\nServer Status\n"));
							_tprintf(_T("   State: %ld\n"), serverStatus.getState());
							_tprintf(_T("   Vendor info: %s\n"), serverStatus.getVendorInfo().c_str());
							_tprintf(_T("   Product version: %s\n"), serverStatus.getProductVersion().c_str());
							_tprintf(_T("   Start time: %s\n"), serverStatus.getStartTime().toString().c_str());
							_tprintf(_T("   Last update time: %s\n"), serverStatus.getLastUpdateTime().toString().c_str());
							_tprintf(_T("   Current time: %s\n"), serverStatus.getCurrentTime().toString().c_str());
							_tprintf(_T("   Group count: %ld\n"), serverStatus.getGroupCount());
							_tprintf(_T("   Bandwidth: %ld\n"), serverStatus.getBandwidth());
							std::vector<tstring>lcIds = serverStatus.getSupportedLcIds();

							for (i = 0; i < lcIds.size(); i++)
							{
								_tprintf(_T("   Supported LCID: %s\n"), lcIds[i].c_str());
							}

							_tprintf(_T("   Status info: %s\n"), serverStatus.getStatusInfo().c_str());
						}
						else
						{
							_tprintf(_T("\nGet Status failed [0x%8.8X]\n"), (unsigned int)result);
						}

						break;

					case _T('M'):
					case _T('m'):
						if (SUCCEEDED(result = session->activateConnectionMonitor(true, 5000, 10, 10000, 300000)))
						{
							_tprintf(_T("Activated connection monitor\n"));
						}
						else
						{
							_tprintf(_T("Activate connection monitor failed [0x%8.8X]\n"), (unsigned int)result);
						}

						break;

					case _T('N'):
					case _T('n'):
						if (SUCCEEDED(result = session->activateConnectionMonitor(false, 5000, 10, 10000, 300000)))
						{
							_tprintf(_T("Deactivated connection monitor\n"));
						}
						else
						{
							_tprintf(_T("Activate connection monitor failed [0x%8.8X]\n"), (unsigned int)result);
						}

						break;

					case _T('?'):
					case _T('U'):
					case _T('u'):
						usage();
						break;

					case _T('E'):
					case _T('e'):
					case _T('Q'):
					case _T('q'):
#ifdef TBC_OS_WINDOWS
						end = TRUE;
						CloseHandle(g_endEvent);
#endif
#ifdef TBC_OS_LINUX
						sem_post(&g_endSemaphor);
#endif
						break;
					} // end switch
				} // end if
			} // end else

#ifdef TBC_OS_WINDOWS
			Sleep(1000);
#endif
		} // end while

		if (session->getCurrentState() != EnumObjectState_DISCONNECTED)
		{
			session->disconnect(NULL);
		}

		if (subscription->getCurrentState() != EnumObjectState_DISCONNECTED)
		{
			subscription->disconnect(NULL);
		}

		subscription->removeDaItem(item1);
		subscription->removeDaItem(item2);
		subscription->removeDaItem(item3);
		subscription->removeDaItem(item4);
		session->removeDaSubscription(subscription);
		application->removeDaSession(session);
		application->terminate();
		releaseApplication();
		delete session;
		delete subscription;
		delete item1;
		delete item2;
		delete item3;
		delete item4;
	} // end if initialize succedeed

	return 0;
}
