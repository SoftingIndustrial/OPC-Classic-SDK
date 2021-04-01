#include "stdafx.h"

#include "OpcClient.h"

#include "ClientCommon.h"

const unsigned int g_maxSubscriptionCount = ((unsigned int)(g_itemsTotalCount / g_minItemsPerSubscription) + 1);

using namespace std;
using namespace SoftingOPCToolboxClient;
extern bool g_tracesOn;

OpcClient* instance = NULL;

OpcClient* getOpcClient(void)
{
	return instance;
} // end getOpcClient

void createOpcClient(void)
{
	if (instance == NULL)
	{
		instance = new OpcClient();
	}
} // end createOpcClient

void destroyOpcClient(void)
{
	if (instance != NULL)
	{
		delete instance;
		instance = NULL;
	}
} // end destroyOpcClient

OpcClient::OpcClient()
{
	m_session = NULL;

	m_subscriptions = (MyDaSubscription**)malloc(((unsigned int)(g_itemsTotalCount / g_minItemsPerSubscription) + 1) * sizeof(MyDaSubscription*));
	for (unsigned int i = 0; i < g_maxSubscriptionCount; ++i)
	{
		m_subscriptions[i] = NULL;
	}

	m_executionOptions.setExecutionType(EnumExecutionType_ASYNCHRONOUS);

} // end constructor

OpcClient::~OpcClient()
{
	free(m_subscriptions);
} // end constructor

/*void OpcClient::setCredentials(IN const tstring& usr, IN const tstring& pwd)
{
	m_usr = usr;
	m_pwd = pwd;
}*/


Application* OpcClient::getApp()
{
	return getApplication();
} // end getApp

long OpcClient::initialize()
{
	long result = EnumResultCode_S_OK;
	getApp()->setVersionOtb(447);
#ifdef EC_CLIENT
	getApp()->enableTracing(
			EnumTraceGroup_ALL,
			EnumTraceGroup_ALL,
			EnumTraceGroup_ALL,
			EnumTraceGroup_NOTHING,
			_T("DaConsole - OPCDA EC 10k items.txt"),
			500000000,
			5);
#else
	getApp()->enableTracing(
		EnumTraceGroup_NOTHING,
		EnumTraceGroup_NOTHING,
		EnumTraceGroup_NOTHING,
		EnumTraceGroup_NOTHING,
		_T("DaConsole - XMLDA AddressSpace8012 10k items.txt"),
		500000000,
		5);
#endif

	// TODO - binary license activation
	// Fill in your binary license activation keys here
	//
	// NOTE: you can activate one or all of the features at the same time

	//  activate the COM-DA Client feature
	result = getApplication()->activate(EnumFeature_DA_CLIENT, _T("10d0-042c-1d82-da0c-85da"));
	if (!SUCCEEDED(result))
	{
		return result;
	}

	// END TODO - binary license activation
	result = getApp()->initialize();

	return result;
} // end initialize

long OpcClient::processCommandLine(tstring command)
{
	return getApp()->processCommandLine(command);
} // end processCommandLine

void OpcClient::terminate()
{
	unsigned int n = 0;

	if ((m_session != NULL) && (m_session->getCurrentState() != EnumObjectState_DISCONNECTED))
	{
		m_session->disconnect(NULL);
	}

	std::vector<DaItem*> subscriptionItems;
	unsigned int currItemNo, currSubNo;
	unsigned long itemNo, subNo;
	subNo = 0;

	for (unsigned int i = 0; i < g_maxSubscriptionCount; ++i)
	{
		if (m_subscriptions[i] != NULL)
		{
			currSubNo = m_subscriptions[i]->getSubNo();
			subscriptionItems = m_subscriptions[i]->getItemList();

			if (m_subscriptions[i]->getCurrentState() != EnumObjectState_DISCONNECTED)
			{
				m_subscriptions[i]->disconnect(NULL);
			}

			itemNo = 0;

			for (std::vector<DaItem*>::iterator it = subscriptionItems.begin(); it != subscriptionItems.end(); ++it)
			{
				currItemNo = ((MyDaItem*)(*it))->getItemNo();

				if (itemNo % g_silentDisplayCount == 0)
				{
					if (g_syncDisplay.aLock(0))
					{
						_tprintf(_T("sub%d:rmItm%d "), currSubNo, currItemNo);
						g_syncDisplay.aUnlock();
					}
				}

				m_subscriptions[i]->removeDaItem(*it);
				delete *it;
				++itemNo;
			}

			subscriptionItems.clear();
		}

		if (m_session != NULL)
		{
			if (subNo % g_silentDisplayCount == 0)
			{
				if (g_syncDisplay.aLock(0))
				{
					_tprintf(_T("ses:rmSub%d "), currSubNo);
					g_syncDisplay.aUnlock();
				}
			}

			m_session->removeDaSubscription(m_subscriptions[i]);
			delete m_subscriptions[i];
		}
		++subNo;
	}

	if (m_session != NULL)
	{
		getApp()->removeDaSession(m_session);
	}

	getApp()->terminate();

	if (m_session != NULL)
	{
		delete m_session;
		m_session = NULL;
	}
	releaseApplication();
} // end terminate

void OpcClient::setServiceName(tstring serviceName)
{
	getApp()->setServiceName(serviceName);
} // end setServiceName

void OpcClient::trace(
	EnumTraceLevel aLevel,
	EnumTraceGroup aMask,
	const TCHAR* anObjectID,
	const TCHAR* aMessage,
	...)
{
	getApp()->trace(aLevel, aMask, anObjectID, aMessage);
} // end trace


long OpcClient::initializeDaObjects()
{
	if (g_syncDisplay.aLock(0))
	{
		if (g_tracesOn)
		{
			_tprintf(_T("OPC client create items\n"));
		}
		else
		{
			_tprintf(_T("initDaObj "));
		}
		g_syncDisplay.aUnlock();
	}

	//m_session = new MyDaSession(_T("opcda:///Softing.OPCToolbox.C++.Sample.AddressSpace.Multithread.DA.1/{ED06F266-4FD0-4DA3-BB1A-8F906D2B85B2}"));
	m_session = new MyDaSession(_T("opcda:///Softing.OPC.DF.Configuration1.DA.1/{DA771DCC-3C2C-4687-81F2-FF79C0355D20}"));
	//m_session = new MyDaSession(_T("http://localhost:8079/OPC/DA"));
	//m_session = new MyDaSession(_T("tpda://localhost:8079"));


	//m_session = new MyDaSession(_T("opcda:///Softing.OPC.EC.TunnelClient.DA.1/{F0437DE7-81B4-4BE4-A455-147FD0816E68}"));
	//m_session = new MyDaSession(_T("opcda:///OpcTestLab.OpcAnalyzer.1/{B8AADBA5-382F-45DA-AA0A-D7BB21E4AE97}"));
	//m_session = new MyDaSession(_T("opcda:///Softing.OPC.EC.Configuration1.DA.1/{A76F65FF-A34F-43DD-9595-C3351B490F37}"));
#ifdef EC_CLIENT
	//m_session = new MyDaSession(_T("opcda:///Softing.OPC.EC.XMLDA_Client.DA.1/{E92AD0B9-2133-4E0E-B20B-E9C864B118A9}"));
#else
	//m_session = new MyDaSession(_T("http://localhost:8012/OPC/DA"));
#endif
	trace(EnumTraceLevel_INF, EnumTraceGroup_USER1, "session", m_session->getUrl().c_str());

	m_session->setClientName(_T("!@#!@#!@#!@#!@#!@#"));
	long connectResult = m_session->connect(TRUE, FALSE, NULL);

	TCHAR itemName[256];
	MyDaSubscription *aSubscription = NULL;
	MyDaItem *anItem = NULL;
	unsigned int n = 0;
	unsigned int m = 0;
	unsigned int k = 0;
	long result = S_OK;
	unsigned int itemsCountPerSubscription = 0;
	unsigned int step = 0;

	for (unsigned int i = 0; i < g_maxSubscriptionCount; ++i)
	{
		m_subscriptions[i] = new MyDaSubscription(i, rand() * (g_maxSubscriptionUpdateRate - g_minSubscriptionUpdateRate) / RAND_MAX, m_session);
		aSubscription = m_subscriptions[i];

		if (g_connectItemsFirst)
		{
			result = aSubscription->connect(TRUE, FALSE, NULL);
		}

		if ((result != S_OK) || (i % g_silentDisplayCount == 0))
		{
			if (g_syncDisplay.aLock(0))
			{
				_tprintf(_T("ses:addSub%d "), i);
				g_syncDisplay.aUnlock();
			}
		}

		itemsCountPerSubscription = rand() * (g_maxItemsPerSubscription - g_minItemsPerSubscription) / RAND_MAX;

		for (unsigned int j = 0; (j < itemsCountPerSubscription) && (n < g_itemsTotalCount); ++j)
		{
			m_itemPaths.push_back(_T(""));
			_tcscpy_s(itemName, g_itemNames[n]);
			m_itemIds.push_back(itemName);
			anItem = new MyDaItem(n + k, itemName, aSubscription, g_useHardcodedDataType ? g_hardcodedDataType : VT_EMPTY);

			if (g_connectItemsFirst)
			{
				result = anItem->connect(FALSE, FALSE, NULL);
			}
			if ((result != S_OK) || (j % g_silentDisplayCount == 0))
			{
				if (g_syncDisplay.aLock(0))
				{
					_tprintf(_T("sub%d-addI%d "), i, j);
					g_syncDisplay.aUnlock();
				}
			}

			++n;
		}

		// again, random step this time

		itemsCountPerSubscription = rand() * (g_maxItemsPerSubscription - g_minItemsPerSubscription) / RAND_MAX;
		step = n + rand() * (g_itemsTotalCount / 13) / RAND_MAX;
		m = n;

		for (unsigned int j = 0; (j < itemsCountPerSubscription) && (n < g_itemsTotalCount); ++j)
		{
			m += step;
			m %= g_itemsTotalCount;

			m_itemPaths.push_back(_T(""));
			_tcscpy_s(itemName, g_itemNames[m]);
			anItem = new MyDaItem(n + k, itemName, aSubscription, g_useHardcodedDataType ? g_hardcodedDataType : VT_EMPTY);

			if (g_connectItemsFirst)
			{
				result = anItem->connect(FALSE, FALSE, NULL);
			}
			if ((result != S_OK) || (j % g_silentDisplayCount == 0))
			{
				if (g_syncDisplay.aLock(0))
				{
					_tprintf(_T("sub%d-addItm%d "), i, j);
					g_syncDisplay.aUnlock();
				}
			}

			++k;
		}

	}
	if ((g_tracesOn) && (g_syncDisplay.aLock(0)))
	{
		_tprintf(_T("OPC client create items completed\nOPC client connect\n"));
		g_syncDisplay.aUnlock();
	}
	if (!g_connectItemsFirst)
	{
		m_session->connect(TRUE, FALSE, NULL);
	}
	if ((g_tracesOn) && (g_syncDisplay.aLock(0)))
	{
		_tprintf(_T("OPC client connect completed: [0x%8.8X\n"), (unsigned int)connectResult);
		g_syncDisplay.aUnlock();
	}
	return connectResult;
} // end initializeDaObjects

void OpcClient::readSyncItem()
{
	m_removedItemsJanitor.unlock();

	if (m_removedItems.size() > g_itemsTotalCount)
	{
		m_removedItemsJanitor.unlock();
		return;
	}

	m_removedItemsJanitor.unlock();

	if (g_syncDisplay.aLock(0))
	{
		if (g_tracesOn)
		{
			_tprintf(_T("Sync item read:\n"));
		}
		else
		{
			_tprintf(_T("sItmRd "));
		}
		g_syncDisplay.aUnlock();
	}
	ValueQT itemValue;
	long itemResult;
	std::vector<DaItem*> subscriptionItems;
	std::vector<DaItem*>::iterator it;
	unsigned int count = 0;
	long result;
	for (int i = 0, n = g_maxSubscriptionCount; i < n; ++i)
	{
		if ((rand() % 2) || (m_subscriptions[i]->getMyDaSubscriptionItemsCount() == 0))
		{
			continue;
		}

		m_subscriptions[i]->lock();

		subscriptionItems = m_subscriptions[i]->getItemList();
		count = 0;

		for (it = subscriptionItems.begin(); it != subscriptionItems.end(); ++it)
		{
			result = (*it)->read(rand() % 2345, itemValue, itemResult, NULL);

			if (g_tracesOn)
			{
				if (g_syncDisplay.aLock(0))
				{
					if (SUCCEEDED(itemResult))
					{
						_tprintf(_T(" - SyncItemRead %d - %s - %s\n"), i, (*it)->getId().c_str(), itemValue.toString().c_str());
					} //end if
					else
					{
						_tprintf(_T(" - SyncItemRead %d failed - %s [0x%8.8X]\n"), i, (*it)->getId().c_str(), (unsigned int)itemResult);
					} //end else
					g_syncDisplay.aUnlock();
				}
			}
			else if ((result != S_OK) || (count % g_silentDisplayCount == 0))
			{
				if (g_syncDisplay.aLock(0))
				{
					_tprintf(_T("sItm%dRd(%d) "), count, itemResult);
					g_syncDisplay.aUnlock();
				}
			}
			++count;
		}

		m_subscriptions[i]->unlock();
	}
} // end readItem

void OpcClient::activateAsyncSubscription()
{
	if (g_syncDisplay.aLock(0))
	{
		if (g_tracesOn)
		{
			_tprintf(_T("Async subscription activate:\n"));
		}
		else
		{
			_tprintf(_T("aSubAct "));
		}
		g_syncDisplay.aUnlock();
	}
	long result;
	for (unsigned int i = 0; i < g_maxSubscriptionCount; ++i)
	{
		if (rand() % 2)
		{
			continue;
		}

		m_subscriptions[i]->lock();
		result = m_subscriptions[i]->connect(TRUE, TRUE, &m_executionOptions);
		m_executionOptions.setExecutionContext(InterlockedIncrement(&m_executionContext));
		if (g_tracesOn)
		{
			if (g_syncDisplay.aLock(0))
			{
				_tprintf(_T(" - Async subscription #%d activate: [0x%8.8X]\n"), i, result);
				g_syncDisplay.aUnlock();
			}
		}
		else if ((result != S_OK) || (i % g_silentDisplayCount))
		{
			if (g_syncDisplay.aLock(0))
			{
				_tprintf(_T("aSub%dAct(%d) "), i, result);
				g_syncDisplay.aUnlock();
			}
		}
		m_subscriptions[i]->unlock();
	}
} // end activateAsyncSubscription

void OpcClient::activateSyncSubscription()
{
	if (g_syncDisplay.aLock(0))
	{
		if (g_tracesOn)
		{
			_tprintf(_T("Sync subscription activate:\n"));
		}
		else
		{
			_tprintf(_T("sSubAct "));
		}
		g_syncDisplay.aUnlock();
	}
	long result;
	for (unsigned int i = 0; i < g_maxSubscriptionCount; ++i)
	{
		if (rand() % 2)
		{
			continue;
		}
		m_subscriptions[i]->lock();
		result = m_subscriptions[i]->connect(TRUE, TRUE, NULL);
		if (g_tracesOn)
		{
			if (g_syncDisplay.aLock(0))
			{
				_tprintf(_T(" - Sync subscription #%d activate: [0x%8.8X]\n"), i, result);
				g_syncDisplay.aUnlock();
			}
		}
		else if ((result != S_OK) || (i % g_silentDisplayCount == 0))
		{
			if (g_syncDisplay.aLock(0))
			{
				_tprintf(_T("sSub%dAct(%d) "), i, result);
				g_syncDisplay.aUnlock();
			}
		}
		m_subscriptions[i]->unlock();
	}
} // end activateSyncSubscription

void OpcClient::connectAsyncSubscription()
{
	if (g_syncDisplay.aLock(0))
	{
		if (g_tracesOn)
		{
			_tprintf(_T("Async subscription connect:\n"));
		}
		else
		{
			_tprintf(_T("aSubCon "));
		}
		g_syncDisplay.aUnlock();
	}
	long result;
	for (unsigned int i = 0; i < g_maxSubscriptionCount; ++i)
	{
		if (rand() % 3)
		{
			continue;
		}
		m_subscriptions[i]->lock();
		result = m_subscriptions[i]->connect(TRUE, FALSE, &m_executionOptions);
		m_executionOptions.setExecutionContext(InterlockedIncrement(&m_executionContext));
		if (g_tracesOn)
		{
			if (g_syncDisplay.aLock(0))
			{
				_tprintf(_T(" - Async subscription #%d connect: [0x%8.8X]\n"), i, result);
				g_syncDisplay.aUnlock();
			}
		}
		else if ((result != S_OK) || (i % g_silentDisplayCount == 0))
		{
			if (g_syncDisplay.aLock(0))
			{
				_tprintf(_T("aSub%dCon(%d) "), i, result);
				g_syncDisplay.aUnlock();
			}
		}
		m_subscriptions[i]->unlock();
	}
} // end connectAsyncSubscription

void OpcClient::connectSyncSubscription()
{
	if (g_syncDisplay.aLock(0))
	{
		if (g_tracesOn)
		{
			_tprintf(_T("Sync subscription connect:\n"));
		}
		else
		{
			_tprintf(_T("sSubCon "));
		}
		g_syncDisplay.aUnlock();
	}
	long result;
	for (unsigned int i = 0; i < g_maxSubscriptionCount; ++i)
	{
		if (rand() % 4)
		{
			continue;
		}
		m_subscriptions[i]->lock();
		result = m_subscriptions[i]->connect(TRUE, FALSE, NULL);
		if (g_tracesOn)
		{
			if (g_syncDisplay.aLock(0))
			{
				_tprintf(_T(" - Sync subscription #%d connect: [0x%8.8X]\n"), i, result);
				g_syncDisplay.aUnlock();
			}
		}
		else if ((result != S_OK) || (i % g_silentDisplayCount == 0))
		{
			if (g_syncDisplay.aLock(0))
			{
				_tprintf(_T("sSub%dCon(%d) "), i, result);
				g_syncDisplay.aUnlock();
			}
		}
		m_subscriptions[i]->unlock();
	}
} // end connectSyncSubscription

void OpcClient::disconnectAsyncSubscription()
{
	if (g_syncDisplay.aLock(0))
	{
		if (g_tracesOn)
		{
			_tprintf(_T("Async subscription disconnect:\n"));
		}
		else
		{
			_tprintf(_T("aSubDis "));
		}
		g_syncDisplay.aUnlock();
	}
	long result;
	for (unsigned int i = 0; i < g_maxSubscriptionCount; ++i)
	{
		if (rand() % 5)
		{
			continue;
		}
		m_subscriptions[i]->lock();
		result = m_subscriptions[i]->disconnect(&m_executionOptions);
		m_executionOptions.setExecutionContext(InterlockedIncrement(&m_executionContext));
		if (g_tracesOn)
		{
			if (g_syncDisplay.aLock(0))
			{
				_tprintf(_T(" - Async subscription #%d disconnect: [0x%8.8X]\n"), i, result);
				g_syncDisplay.aUnlock();
			}
		}
		else if ((result != S_OK) || (i % g_silentDisplayCount == 0))
		{
			if (g_syncDisplay.aLock(0))
			{
				_tprintf(_T("aSub%dDis(%d) "), i, result);
				g_syncDisplay.aUnlock();
			}
		}
		m_subscriptions[i]->unlock();
	}
} // end disconnectAsyncSubscription

void OpcClient::disconnectSyncSubscription()
{
	if (g_syncDisplay.aLock(0))
	{
		if (g_tracesOn)
		{
			_tprintf(_T("Sync subscription disconnect:\n"));
		}
		else
		{
			_tprintf(_T("sSubDis "));
		}
		g_syncDisplay.aUnlock();
	}
	long result;
	for (unsigned int i = 0; i < g_maxSubscriptionCount; ++i)
	{
		if (rand() % 6)
		{
			continue;
		}
		m_subscriptions[i]->lock();
		result = m_subscriptions[i]->disconnect(NULL);
		if (g_tracesOn)
		{
			if (g_syncDisplay.aLock(0))
			{
				_tprintf(_T(" - Sync subscription #%d disconnect: [0x%8.8X]\n"), i, result);
				g_syncDisplay.aUnlock();
			}
		}
		else if (((result != S_OK)) || (i % g_silentDisplayCount == 0))
		{
			if (g_syncDisplay.aLock(0))
			{
				_tprintf(_T("sSub%dDis(%d) "), i, result);
				g_syncDisplay.aUnlock();
			}
		}
		m_subscriptions[i]->unlock();
	}
} // end disconnectSyncSubscription

void OpcClient::readAsyncSubscription()
{
	m_removedItemsJanitor.unlock();

	if (m_removedItems.size() > g_itemsTotalCount)
	{
		m_removedItemsJanitor.unlock();
		return;
	}

	m_removedItemsJanitor.unlock();

	if (g_syncDisplay.aLock(0))
	{
		if (g_tracesOn)
		{
			_tprintf(_T("Async subscription read:\n"));
		}
		else
		{
			_tprintf(_T("aSubRead "));
		}
		g_syncDisplay.aUnlock();
	}

	std::vector<DaItem*> itemsToRead;
	std::vector<ValueQT*> valuesToRead;
	std::vector<long> readResults;
	long result;
	for (unsigned int i = 0; i < g_maxSubscriptionCount; ++i)
	{
		if ((rand() % 2) || (m_subscriptions[i]->getMyDaSubscriptionItemsCount() == 0))
		{
			continue;
		}

		m_subscriptions[i]->lock();
		itemsToRead = m_subscriptions[i]->getItemList();

		valuesToRead.assign(itemsToRead.size(), NULL);
		readResults.assign(itemsToRead.size(), E_FAIL);

		if (FAILED(result = m_subscriptions[i]->read(
			0,
			itemsToRead,
			valuesToRead,
			readResults,
			&m_executionOptions)))
		{
			if (g_syncDisplay.aLock(0))
			{
				if (g_tracesOn)
				{
					_tprintf(_T(" - Async subscription #%d read failed [0x%8.8X]\n"), i, (unsigned int)result);
				}
				else
				{
					_tprintf(_T("aSub%dRead(%d) "), i, result);
				}
				g_syncDisplay.aUnlock();
			}
		} //end if
		else
		{
			if (g_tracesOn)
			{
				if (g_syncDisplay.aLock(0))
				{
					_tprintf(_T(" - Async subscription #%d read completed\n"), i);
					g_syncDisplay.aUnlock();
				}
			}
			else if ((result != S_OK) || (i % g_silentDisplayCount == 0))
			{
				if (g_syncDisplay.aLock(0))
				{
					_tprintf(_T("aSub%dRd(%d) "), i, result);
					g_syncDisplay.aUnlock();
				}
			}
			m_executionOptions.setExecutionContext(InterlockedIncrement(&m_executionContext));
		} //end if ... else
		m_subscriptions[i]->unlock();
	}
} // end readAsyncSubscription

void OpcClient::readSyncSubscription()
{
	m_removedItemsJanitor.unlock();

	if (m_removedItems.size() > g_itemsTotalCount)
	{
		m_removedItemsJanitor.unlock();
		return;
	}

	m_removedItemsJanitor.unlock();

	if (g_syncDisplay.aLock(0))
	{
		if (g_tracesOn)
		{
			_tprintf(_T("Sync subscription read:\n"));
		}
		else
		{
			_tprintf(_T("sSubRd "));
		}
		g_syncDisplay.aUnlock();
	}

	std::vector<ValueQT*> valuesToRead;
	std::vector<long> readResults;
	std::vector<DaItem*> itemsToRead;
	std::vector<ValueQT*>::iterator valuesToReadIterator;
	long result;
	for (unsigned int i = 0; i < g_maxSubscriptionCount; ++i)
	{
		if ((rand() % 2) || (m_subscriptions[i]->getMyDaSubscriptionItemsCount() == 0))
		{
			continue;
		}

		m_subscriptions[i]->lock();
		itemsToRead = m_subscriptions[i]->getItemList();
		valuesToRead.assign(itemsToRead.size(), NULL);
		readResults.assign(itemsToRead.size(), E_FAIL);

		if (SUCCEEDED(result = m_subscriptions[i]->read(
			10000,
			itemsToRead,
			valuesToRead,
			readResults,
			NULL)))
		{
			if (g_tracesOn)
			{
				if (g_syncDisplay.aLock(0))
				{
					_tprintf(_T(" - Sync subscription #%d read completed:\n"), i);
					for (unsigned int j = 0, n = m_subscriptions[i]->getMyDaSubscriptionItemsCount(); j < n; ++j)
					{
						if (SUCCEEDED(readResults[j]))
						{
							_tprintf(_T("     - SyncSubRead #%d.%d - %s - %s\n"), i, j, itemsToRead[j]->getId().c_str(), (valuesToRead[j])->toString().c_str());
						} //end if
						else
						{
							_tprintf(_T("     - SyncSubRead #%d.%d failed - %s [0x%8.8X]\n"), i, j, itemsToRead[j]->getId().c_str() , (unsigned int)readResults[j]);
						} //end else
					} //end for
					g_syncDisplay.aUnlock();
				}
			}
			else if ((result != S_OK) || (i % g_silentDisplayCount == 0))
			{
				if (g_syncDisplay.aLock(0))
				{
					_tprintf(_T("sSub%dRd(%d) "), i, result);
					g_syncDisplay.aUnlock();
				}
			}

			for (valuesToReadIterator = valuesToRead.begin(); valuesToReadIterator != valuesToRead.end(); valuesToReadIterator++)
			{
				if (*valuesToReadIterator != NULL)
				{
					delete *valuesToReadIterator;
				}
			} //end for
		}// end if
		else
		{
			if (g_syncDisplay.aLock(0))
			{
				if (g_tracesOn)
				{
					_tprintf(_T(" - Sync subscription #%d read failed [0x%8.8X]\n"), i, (unsigned int)result);
				}
				else
				{
					_tprintf(_T("synSub%dRd(%d) "), i, result);
				}
				g_syncDisplay.aUnlock();
			}
		}// end else
		m_subscriptions[i]->unlock();
	}

} // end readSyncSubscription

void OpcClient::refreshSubscription()
{
	m_removedItemsJanitor.unlock();

	if (m_removedItems.size() > g_itemsTotalCount)
	{
		m_removedItemsJanitor.unlock();
		return;
	}

	m_removedItemsJanitor.unlock();

	if (g_syncDisplay.aLock(0))
	{
		if (g_tracesOn)
		{
			_tprintf(_T("Async subscription refresh:\n"));
		}
		else
		{
			_tprintf(_T("aSubRef "));
		}
		g_syncDisplay.aUnlock();
	}
	long result;
	for (unsigned int i = 0; i < g_maxSubscriptionCount; ++i)
	{
		if ((rand() % 2) || (m_subscriptions[i]->getMyDaSubscriptionItemsCount() == 0))
		{
			continue;
		}

		m_subscriptions[i]->lock();

		if (FAILED(result = m_subscriptions[i]->refresh(0, &m_executionOptions)))
		{
			if (g_syncDisplay.aLock(0))
			{
				if (g_tracesOn)
				{
					_tprintf(_T(" - Async subscription #%d refresh failed [0x%8.8X]\n"), i, (unsigned int)result);
				}
				else
				{
					_tprintf(_T("aSub%dRef(%d) "), i, result);
				}
				g_syncDisplay.aUnlock();
			}
		}//end if
		else
		{
			if (g_tracesOn)
			{
				if (g_syncDisplay.aLock(0))
				{
					_tprintf(_T(" - Async subscription #%d refresh completed\n"), i);
					g_syncDisplay.aUnlock();
				}
			}
			else if ((result != S_OK) || (i % g_silentDisplayCount == 0))
			{
				if (g_syncDisplay.aLock(0))
				{
					_tprintf(_T("aSub%dRef(%d) "), i, result);
					g_syncDisplay.aUnlock();
				}
			}
			m_executionOptions.setExecutionContext(InterlockedIncrement(&m_executionContext));
		}// end else
		m_subscriptions[i]->unlock();
	}
}// end refreshSubscription

void OpcClient::readAsyncSession()
{
	if (g_syncDisplay.aLock(0))
	{
		if (g_tracesOn)
		{
			_tprintf(_T("Async session read:\n"));
		}
		else
		{
			_tprintf(_T("aSesRd "));
		}
		g_syncDisplay.aUnlock();
	}
	std::vector<ValueQT*> valuesToRead;
	valuesToRead.assign(g_itemsTotalCount, NULL);
	std::vector<long> readResults;
	readResults.assign(g_itemsTotalCount, E_FAIL);
	long result;

	if (FAILED(result = m_session->read(
		0,
		m_itemIds,
		m_itemPaths,
		valuesToRead,
		readResults,
		&m_executionOptions)))
	{
		if (g_syncDisplay.aLock(0))
		{
			if (g_tracesOn)
			{
				_tprintf(_T(" - Async session read failed [0x%8.8X]\n"), (unsigned int)result);
			}
			else
			{
				_tprintf(_T("aSesRd(%d) "), result);
			}
			g_syncDisplay.aUnlock();
		}
	}//end if
	else
	{
		if (g_tracesOn)
		{
			if (g_syncDisplay.aLock(0))
			{
				_tprintf(_T(" - Async session read completed\n"));
				g_syncDisplay.aUnlock();
			}
		}
		else if (result != S_OK)
		{
			if (g_syncDisplay.aLock(0))
			{
				_tprintf(_T("aSesRd(%d) "), result);
				g_syncDisplay.aUnlock();
			}
		}
		m_executionOptions.setExecutionContext(InterlockedIncrement(&m_executionContext));
	} //end if ... else
} // end readAsyncSession

void OpcClient::readSyncSession()
{
	if (g_syncDisplay.aLock(0))
	{
		if (g_tracesOn)
		{
			_tprintf(_T("Sync session read:\n"));
		}
		else
		{
			_tprintf(_T("sSesRd "));
		}
		g_syncDisplay.aUnlock();
	}
	std::vector<ValueQT*> valuesToRead;
	std::vector<ValueQT*>::iterator valuesToReadIterator;
	valuesToRead.assign(g_itemsTotalCount, NULL);

	std::vector<long> readResults;
	readResults.assign(g_itemsTotalCount, E_FAIL);

	long result;

	if (SUCCEEDED(result = m_session->read(
		0,
		m_itemIds,
		m_itemPaths,
		valuesToRead,
		readResults,
		NULL)))
	{
		if (g_tracesOn)
		{
			if (g_syncDisplay.aLock(0))
			{
				_tprintf(_T(" - Sync session read completed:\n"));

				for (unsigned int i = 0; i < g_itemsTotalCount; i++)
				{
					if (SUCCEEDED(readResults[i]))
					{
						_tprintf(_T("     - SyncSesRead #%d - %s - %s\n"), i, m_itemIds[i].c_str(), (valuesToRead[i])->toString().c_str());
					} //end if
					else
					{
						_tprintf(_T("     - SyncSesRead #%d failed - %s [0x%8.8X]\n"), i, m_itemIds[i].c_str() , (unsigned int)readResults[i]);
					} //end else
				} //end for
				g_syncDisplay.aUnlock();
			}
		}
		else if (result != S_OK)
		{
			if (g_syncDisplay.aLock(0))
			{
				_tprintf(_T("sSesRd(%d) "), result);
				g_syncDisplay.aUnlock();
			}
		}

		for (valuesToReadIterator = valuesToRead.begin(); valuesToReadIterator != valuesToRead.end(); valuesToReadIterator++)
		{
			if (*valuesToReadIterator != NULL)
			{
				delete *valuesToReadIterator;
			}
		} //end for
	}//end if
	else
	{
		if (g_syncDisplay.aLock(0))
		{
			if (g_tracesOn)
			{
				_tprintf(_T("Sync session read failed [0x%8.8X]\n"), (unsigned int)result);
			}
			else
			{
				_tprintf(_T("sSesRd(%d) "), result);
			}
			g_syncDisplay.aUnlock();
		}
	} //end if ... else
} // end readSyncSession

void OpcClient::writeSyncSubscription()
{
	m_removedItemsJanitor.unlock();

	if (m_removedItems.size() > g_itemsTotalCount)
	{
		m_removedItemsJanitor.unlock();
		return;
	}

	m_removedItemsJanitor.unlock();

	if (g_syncDisplay.aLock(0))
	{
		if (g_tracesOn)
		{
			_tprintf(_T("Sync subscription write:\n"));
		}
		else
		{
			_tprintf(_T("sSubWr "));
		}
		g_syncDisplay.aUnlock();
	}
	std::vector<ValueQT*> valuesToWrite;
	std::vector<ValueQT*>::iterator valuesToWriteIterator;
	std::vector<long> writeResults;
	std::vector<DaItem*> itemsToWrite;

	long result;
	unsigned int j, n = 0;

	DateTime writeDateTime;
	Variant writeData;
	EnumQuality writeQuality;

	for (unsigned int i = 0; i < g_maxSubscriptionCount; ++i)
	{
		if ((rand() % 2) || (m_subscriptions[i]->getMyDaSubscriptionItemsCount() == 0))
		{
			continue;
		}

		m_subscriptions[i]->lock();

		itemsToWrite = m_subscriptions[i]->getItemList();
		n = itemsToWrite.size();
		valuesToWrite.assign(n, NULL);
		writeResults.assign(n, E_FAIL);
		for (j = 0; j < n; ++j)
		{
			if (rand() % 2)
			{
				writeDateTime.now();
			}
			else
			{
				writeDateTime.clear();
			}
			/*if (rand() % 3 == 2)
			{
				writeData = m_items[n + j]->getValue();
			}
			else*/
			{
				writeData = ((MyDaItem*)itemsToWrite[j])->getRandomWriteValue(rand());
			}
			if ((g_writeQuality) && (rand() % 4 == 3))
			{
				writeQuality = (EnumQuality)g_qualities[rand() % 18];
			}
			else
			{
				writeQuality = EnumQuality_QUALITY_NOT_SET;
			}
			valuesToWrite[j] = new ValueQT(writeData, writeQuality, writeDateTime);
		}

		if (SUCCEEDED(result = m_subscriptions[i]->write(
			itemsToWrite,
			valuesToWrite,
			writeResults,
			NULL)))
		{
			if (g_tracesOn)
			{
				if (g_syncDisplay.aLock(0))
				{
					_tprintf(_T(" - Sync subscription #%d write completed:\n"), i);
					for (j = 0; j < n; ++j)
					{
						if (SUCCEEDED(writeResults[j]))
						{
							_tprintf(_T("     - SyncSubWrite %d.%d - %s - %s\n"), i, j, itemsToWrite[j]->getId().c_str(), (valuesToWrite[j])->toString().c_str());
						} //end if
						else
						{
							_tprintf(_T("     - SyncSubWrite %d.%d failed - %s [0x%8.8X]\n"), i, j, itemsToWrite[j]->getId().c_str(), (unsigned int)writeResults[j]);
						} //end else
					} //end for
					g_syncDisplay.aUnlock();
				}
			}
			else if ((result != S_OK) || (i % g_silentDisplayCount == 0))
			{
				if (g_syncDisplay.aLock(0))
				{
					_tprintf(_T("sSub%dWr(%d) "), i, result);
					g_syncDisplay.aUnlock();
				}
			}
		} //end if
		else
		{
			if (g_syncDisplay.aLock(0))
			{
				if (g_tracesOn)
				{
					_tprintf(_T(" - Sync subscription #%d write failed [0x%8.8X]\n"), i, (unsigned int)result);
				}
				else
				{
					_tprintf(_T("sSub%dWr(%d) "), i, result);
				}
				g_syncDisplay.aUnlock();
			}
		} //end else

		for (valuesToWriteIterator = valuesToWrite.begin(); valuesToWriteIterator != valuesToWrite.end(); ++valuesToWriteIterator)
		{
			delete *valuesToWriteIterator;
		}
		m_subscriptions[i]->unlock();
	}
}// end writeSyncItem

void OpcClient::writeAsyncSubscription()
{
	m_removedItemsJanitor.unlock();

	if (m_removedItems.size() > g_itemsTotalCount)
	{
		m_removedItemsJanitor.unlock();
		return;
	}

	m_removedItemsJanitor.unlock();

	if (g_syncDisplay.aLock(0))
	{
		if (g_tracesOn)
		{
			_tprintf(_T("Async subscription write:\n"));
		}
		else
		{
			_tprintf(_T("aSubWr "));
		}
		g_syncDisplay.aUnlock();
	}
	std::vector<ValueQT*> valuesToWrite;
	std::vector<ValueQT*>::iterator valuesToWriteIterator;
	std::vector<long> writeResults;
	std::vector<DaItem*> itemsToWrite;

	long result;
	int j, n = 0;

	DateTime writeDateTime;
	Variant writeData;
	EnumQuality writeQuality;

	for (unsigned int i = 0; i < g_maxSubscriptionCount; ++i)
	{
		if ((rand() % 2) || (m_subscriptions[i]->getMyDaSubscriptionItemsCount() == 0))
		{
			continue;
		}

		m_subscriptions[i]->lock();

		itemsToWrite = m_subscriptions[i]->getItemList();
		n = itemsToWrite.size();
		valuesToWrite.assign(n, NULL);
		writeResults.assign(n, E_FAIL);

		for (j = 0; j < n; ++j)
		{
			if (rand() % 2 == 1)
			{
				writeDateTime.now();
			}
			else
			{
				writeDateTime.clear();
			}
			/*if (rand() % 3 == 2)
			{
				writeData = m_items[n + j]->getValue();
			}
			else*/
			{
				writeData = ((MyDaItem*)itemsToWrite[j])->getRandomWriteValue(rand());
			}
			if ((g_writeQuality) && (rand() % 4 == 3))
			{
				writeQuality = (EnumQuality)g_qualities[rand() % 17];
			}
			else
			{
				writeQuality = EnumQuality_QUALITY_NOT_SET;
			}
			valuesToWrite[j] = new ValueQT(writeData, writeQuality, writeDateTime);
		}

		if (FAILED(result = m_subscriptions[i]->write(
			itemsToWrite,
			valuesToWrite,
			writeResults,
			NULL)))
		{
			if (g_syncDisplay.aLock(0))
			{
				if (g_tracesOn)
				{
					_tprintf(_T(" - Async subscription #%d write failed [0x%8.8X]\n"), i, (unsigned int)result);
				}
				else
				{
					_tprintf(_T("aSub%dWr(%d) "), i, result);
				}
				g_syncDisplay.aUnlock();
			}
		}
		else
		{
			if (g_tracesOn)
			{
				if (g_syncDisplay.aLock(0))
				{
					_tprintf(_T(" - Async subscription #%d write completed\n"), i);
					g_syncDisplay.aUnlock();
				}
			}
			else if ((result != S_OK) || (i % g_silentDisplayCount == 0))
			{
				if (g_syncDisplay.aLock(0))
				{
					_tprintf(_T("aSub%dWr(%d) "), i, result);
					g_syncDisplay.aUnlock();
				}
			}
			m_executionOptions.setExecutionContext(InterlockedIncrement(&m_executionContext));
		} //end else

		for (valuesToWriteIterator = valuesToWrite.begin(); valuesToWriteIterator != valuesToWrite.end(); ++valuesToWriteIterator)
		{
			delete *valuesToWriteIterator;
		}
		m_subscriptions[i]->unlock();
	}
}// end writeAsyncItem

void OpcClient::writeAsyncSession()
{
	if (g_syncDisplay.aLock(0))
	{
		if (g_tracesOn)
		{
			_tprintf(_T("Async session write:\n"));
		}
		else
		{
			_tprintf(_T("aSesWr "));
		}
		g_syncDisplay.aUnlock();
	}
	std::vector<ValueQT*> valuesToWrite;
	std::vector<ValueQT*>::iterator valuesToWriteIterator;
	std::vector<long> writeResults;

	valuesToWrite.assign(g_itemsTotalCount, NULL);
	writeResults.assign(g_itemsTotalCount, E_FAIL);

	long result;

	DateTime writeDateTime;
	Variant writeData;
	EnumQuality writeQuality;

	for (unsigned int i = 0; i < g_itemsTotalCount; ++i)
	{
		if (rand() % 2 == 1)
		{
			writeDateTime.now();
		}
		else
		{
			writeDateTime.clear();
		}
		/*if (rand() % 3 == 2)
		{
			writeData = m_items[i]->getValue();
		}
		else*/
		{
			writeData = m_items[i]->getRandomWriteValue(rand());
		}
		if ((g_writeQuality) && (rand() % 4 == 3))
		{
			writeQuality = (EnumQuality)g_qualities[rand() % 18];
		}
		else
		{
			writeQuality = EnumQuality_QUALITY_NOT_SET;
		}
		valuesToWrite[i] = new ValueQT(writeData, writeQuality, writeDateTime);
	}

	if (FAILED(result = m_session->write(
		m_itemIds,
		m_itemPaths,
		valuesToWrite,
		writeResults,
		&m_executionOptions)))
	{
		if (g_syncDisplay.aLock(0))
		{
			if (g_tracesOn)
			{
				_tprintf(_T(" - Async session write failed [0x%8.8X]\n"), (unsigned int)result);
			}
			else
			{
				_tprintf(_T("aSesWr(%d) "), result);
			}
			g_syncDisplay.aUnlock();
		}
	}
	else
	{
		if (g_tracesOn)
		{
			if (g_syncDisplay.aLock(0))
			{
				_tprintf(_T(" - Async session write completed\n"));
				g_syncDisplay.aUnlock();
			}
		}
		else if (result != S_OK)
		{
			_tprintf(_T("aSesWr(%d) "), result);
		}
		m_executionOptions.setExecutionContext(InterlockedIncrement(&m_executionContext));
	} //end if ... else

	for (valuesToWriteIterator = valuesToWrite.begin(); valuesToWriteIterator != valuesToWrite.end(); ++valuesToWriteIterator)
	{
		delete *valuesToWriteIterator;
	}
}// end writeAsyncSession

void OpcClient::writeSyncSession()
{
	m_removedItemsJanitor.unlock();

	if (m_removedItems.size() > g_itemsTotalCount)
	{
		m_removedItemsJanitor.unlock();
		return;
	}

	m_removedItemsJanitor.unlock();

	if (g_syncDisplay.aLock(0))
	{
		if (g_tracesOn)
		{
			_tprintf(_T("Sync session write:\n"));
		}
		else
		{
			_tprintf(_T("sSesWr "));
		}
		g_syncDisplay.aUnlock();
	}
	std::vector<ValueQT*> valuesToWrite;
	std::vector<ValueQT*>::iterator valuesToWriteIterator;
	std::vector<long> writeResults;

	valuesToWrite.assign(g_itemsTotalCount, NULL);
	writeResults.assign(g_itemsTotalCount, E_FAIL);

	long result;

	DateTime writeDateTime;
	Variant writeData;
	EnumQuality writeQuality;

	for (unsigned int i = 0; i < g_itemsTotalCount; ++i)
	{
		if (rand() % 2 == 1)
		{
			writeDateTime.now();
		}
		else
		{
			writeDateTime.clear();
		}
		/*if (rand() % 3 == 2)
		{
			writeData = m_items[i]->getValue();
		}
		else*/
		{
			writeData = m_items[i]->getRandomWriteValue(rand());
		}
		if ((g_writeQuality) && (rand() % 4 == 3))
		{
			writeQuality = (EnumQuality)g_qualities[rand() % 18];
		}
		else
		{
			writeQuality = EnumQuality_QUALITY_NOT_SET;
		}
		valuesToWrite[i] = new ValueQT(writeData, writeQuality, writeDateTime);
	}

	if (SUCCEEDED(result = m_session->write(
		m_itemIds,
		m_itemPaths,
		valuesToWrite,
		writeResults,
		NULL)))
	{
		if (g_tracesOn)
		{
			if (g_syncDisplay.aLock(0))
			{
				_tprintf(_T(" - Sync session write completed:\n"));
				for (unsigned int i = 0; i < g_itemsTotalCount; i++)
				{
					if (SUCCEEDED(writeResults[i]))
					{
						_tprintf(_T("     - SyncSesWrite %d - %s - %s\n"), i, m_itemIds[i].c_str(), (valuesToWrite[i])->toString().c_str());
					} //end if
					else
					{
						_tprintf(_T("     - SyncSesWrite %d failed - %s [0x%8.8X]\n"), i, m_itemIds[i].c_str(), (unsigned int)writeResults[i]);
					} //end else
				} //end for
				g_syncDisplay.aUnlock();
			}
			else if (result != S_OK)
			{
				if (g_syncDisplay.aLock(0))
				{
					_tprintf(_T("sSesWr(%d) "), result);
					g_syncDisplay.aUnlock();
				}
			}
		}
	}
	else
	{
		if (g_syncDisplay.aLock(0))
		{
			if (g_tracesOn)
			{
				_tprintf(_T(" - Sync session write failed [0x%8.8X]\n"), (unsigned int)result);
			}
			else
			{
				_tprintf(_T("sSesWr(%d) "), result);
			}
			g_syncDisplay.aUnlock();
		}
	} //end if ... else

	for (valuesToWriteIterator = valuesToWrite.begin(); valuesToWriteIterator != valuesToWrite.end(); ++valuesToWriteIterator)
	{
		delete *valuesToWriteIterator;
	}
}// end writeSyncSession

void OpcClient::getAsyncServerStatus()
{
	if (g_syncDisplay.aLock(0))
	{
		if (g_tracesOn)
		{
			_tprintf(_T("Async get server status:\n"));
		}
		else
		{
			_tprintf(_T("aGetSta "));
		}
		g_syncDisplay.aUnlock();
	}
	ServerStatus serverStatus;
	long result;

	if (FAILED(result = m_session->getStatus(serverStatus, &m_executionOptions)))
	{
		if (g_syncDisplay.aLock(0))
		{
			if (g_tracesOn)
			{
				_tprintf(_T(" - Async get server status failed [0x%8.8X]\n"), (unsigned int)result);
			}
			else
			{
				_tprintf(_T("aGetSta(%d) "), result);
			}
			g_syncDisplay.aUnlock();
		}
	} //end if
	else
	{
		if (g_tracesOn)
		{
			if (g_syncDisplay.aLock(0))
			{
				_tprintf(_T(" - Async get server status completed\n"));
				g_syncDisplay.aUnlock();
			}
			else if (result != S_OK)
			{
				if (g_syncDisplay.aLock(0))
				{
					_tprintf(_T("aGetSta(%d) "), result);
					g_syncDisplay.aUnlock();
				}
			}
		}
		m_executionOptions.setExecutionContext(InterlockedIncrement(&m_executionContext));
	} //end else
}// end getAsyncServerStatus

void OpcClient::getSyncServerStatus()
{
	if (g_syncDisplay.aLock(0))
	{
		if (g_tracesOn)
		{
			_tprintf(_T("Sync get server status:\n"));
		}
		else
		{
			_tprintf(_T("sGetSta "));
		}
		g_syncDisplay.aUnlock();
	}
	ServerStatus serverStatus;
	long result;

	if (SUCCEEDED(result = m_session->getStatus(serverStatus, NULL)))
	{
		if (g_tracesOn)
		{
			if (g_syncDisplay.aLock(0))
			{
				_tprintf(_T(" - Sync get server status completed:\n"));
				_tprintf(_T("     - State: %ld\n"), serverStatus.getState());
				_tprintf(_T("     - Vendor info: %s\n"), serverStatus.getVendorInfo().c_str());
				_tprintf(_T("     - Product version: %s\n"), serverStatus.getProductVersion().c_str());
				_tprintf(_T("     - Start time: %s\n"), serverStatus.getStartTime().toString().c_str());
				_tprintf(_T("     - Last update time: %s\n"), serverStatus.getLastUpdateTime().toString().c_str());
				_tprintf(_T("     - Current time: %s\n"), serverStatus.getCurrentTime().toString().c_str());
				_tprintf(_T("     - Group count: %ld\n"), serverStatus.getGroupCount());
				_tprintf(_T("     - Bandwidth: %ld\n"), serverStatus.getBandwidth());
				std::vector<tstring>lcIds = serverStatus.getSupportedLcIds();

				for (unsigned int i = 0; i < lcIds.size(); i++)
				{
					_tprintf(_T("     - Supported LCID: %s\n"), lcIds[i].c_str());
				}

				_tprintf(_T("     - Status info: %s\n"), serverStatus.getStatusInfo().c_str());
				g_syncDisplay.aUnlock();
			}
		}
		else if (result != S_OK)
		{
			if (g_syncDisplay.aLock(0))
			{
				_tprintf(_T("sGetSta(%d) "), result);
				g_syncDisplay.aUnlock();
			}
		}
	}// end if
	else
	{
		if (g_syncDisplay.aLock(0))
		{
			if (g_tracesOn)
			{
				_tprintf(_T(" - Sync get server status failed [0x%8.8X]\n"), (unsigned int)result);
			}
			else
			{
				_tprintf(_T("sGetSta(%d) "), result);
			}
			g_syncDisplay.aUnlock();
		}
	}// end else
}// end getSyncServerStatus

void OpcClient::activateConnectionMonitor()
{
	if (g_syncDisplay.aLock(0))
	{
		if (g_tracesOn)
		{
			_tprintf(_T("Activate connection monitor\n"));
		}
		else
		{
			_tprintf(_T("+ConMon "));
		}
		g_syncDisplay.aUnlock();
	}
	long result;

	if (SUCCEEDED(result = m_session->activateConnectionMonitor(true, 15000, 10, 10000, 300000)))
	{
		if (g_tracesOn)
		{
			if (g_syncDisplay.aLock(0))
			{
				_tprintf(_T(" - Activated connection monitor completed\n"));
				g_syncDisplay.aUnlock();
			}
		}
		else if (result != S_OK)
		{
			if (g_syncDisplay.aLock(0))
			{
				_tprintf(_T("+ConMon(%d) "), result);
				g_syncDisplay.aUnlock();
			}
		}
	}
	else
	{
		if (g_syncDisplay.aLock(0))
		{
			if (g_tracesOn)
			{
				_tprintf(_T(" - Activate connection monitor failed [0x%8.8X]\n"), (unsigned int)result);
			}
			else
			{
				_tprintf(_T("+ConMon(%d) "), result);
			}
			g_syncDisplay.aUnlock();
		}
	}
}// end activateConnectionMonitor

void OpcClient::deactivateConnectionMonitor()
{
	if (g_syncDisplay.aLock(0))
	{
		if (g_tracesOn)
		{
			_tprintf(_T("Deactivate connection monitor\n"));
		}
		else
		{
			_tprintf(_T("-ConMon"));
		}
		g_syncDisplay.aUnlock();
	}
	long result;

	if (SUCCEEDED(result = m_session->activateConnectionMonitor(false, 5000, 10, 10000, 300000)))
	{
		if (g_tracesOn)
		{
			if (g_syncDisplay.aLock(0))
			{
				_tprintf(_T(" - Deactivated connection monitor completed\n"));
				g_syncDisplay.aUnlock();
			}
		}
		else
		{
			if (g_syncDisplay.aLock(0))
			{
				_tprintf(_T("-ConMon(%d) "), result);
				g_syncDisplay.aUnlock();
			}
		}
	}
	else
	{
		if (g_syncDisplay.aLock(0))
		{
			if (g_tracesOn)
			{
				_tprintf(_T(" - Deactivate connection monitor failed [0x%8.8X]\n"), (unsigned int)result);
			}
			else
			{
				_tprintf(_T("-ConMon(%d) "), result);
			}
			g_syncDisplay.aUnlock();
		}
	}
}// end deactivateConnectionMonitor


void OpcClient::logOnAsyncForSecureCommunication()
{
	// removed
} // end logOnAsyncForSecureCommunication

void OpcClient::logOnSyncForSecureCommunication()
{
	// removed
} // end logOnSyncForSecureCommunication

void OpcClient::logOffAsyncForSecureCommunication()
{
	// removed
} // end logOffAsyncForSecureCommunication

void OpcClient::logOffSyncForSecureCommunication()
{
	// removed
} // end logOffSyncForSecureCommunication

void OpcClient::setCredentials()
{
	// removed
} // end setCredentials()

void OpcClient::modifyUpdateRate(const DWORD updateRate)
{
	for (unsigned int i = 0; i < g_maxSubscriptionCount; ++i)
	{
		if (updateRate == INFINITE)
		{
			if ((rand() % 3) && (m_subscriptions[i]->getMyDaSubscriptionItemsCount() == 0))
			{
				m_subscriptions[i]->setRequestedUpdateRate(rand() * (g_maxSubscriptionUpdateRate - g_minSubscriptionUpdateRate) / RAND_MAX);
			}
		}
		else
		{
			m_subscriptions[i]->setRequestedUpdateRate(updateRate);
		}
	}
} // end modifyUpdateRate()

void OpcClient::connectSession()
{
	long result = m_session->connect(TRUE, FALSE, NULL);
	
	if (SUCCEEDED(result))
	{
		if (g_tracesOn)
		{
			if (g_syncDisplay.aLock(0))
			{
				_tprintf(_T("SyncSessionConnect (deep)\n"));
				g_syncDisplay.aUnlock();
			}
		} //end if
		else if (result != S_OK)
		{
			if (g_syncDisplay.aLock(0))
			{
				_tprintf(_T("sSesCon(%d) "), result);
				g_syncDisplay.aUnlock();
			}
		}
	}
	else
	{
		if (g_syncDisplay.aLock(0))
		{
			if (g_tracesOn)
			{
				_tprintf(_T("SyncSessionConnect (deep) failed - [0x%8.8X]\n"), result);
			}
			else
			{
				_tprintf(_T("sSesCon(%d) "), result);
			}
			g_syncDisplay.aUnlock();
		}
	}
} // end connectSession()

void OpcClient::disconnectSession()
{
	long result = m_session->disconnect(NULL);

	if (SUCCEEDED(result))
	{
		if (g_tracesOn)
		{
			if (g_syncDisplay.aLock(0))
			{
				_tprintf(_T("SyncSessionDisconnect (deep)\n"));
				g_syncDisplay.aUnlock();
			}
		} //end if
		else if (result != S_OK)
		{
			if (g_syncDisplay.aLock(0))
			{
				_tprintf(_T("sSesDisc(%d) "), result);
				g_syncDisplay.aUnlock();
			}
		}
	}
	else
	{
		if (g_syncDisplay.aLock(0))
		{
			if (g_tracesOn)
			{
				_tprintf(_T("SyncSessionDisconnect (deep) failed - [0x%8.8X]\n"), result);
			}
			else
			{
				_tprintf(_T("sSesDisc(%d) "), result);
			}
			g_syncDisplay.aUnlock();
		}
	}
} // end disconnectSession()

void OpcClient::removeRandomItems()
{
	if (g_syncDisplay.aLock(0))
	{
		if (g_tracesOn)
		{
			_tprintf(_T("Remove DA Items:\n"));
		}
		else
		{
			_tprintf(_T("rmDaItms "));
		}
		g_syncDisplay.aUnlock();
	}

	unsigned int totalItemsCount = m_removedItems.size();

	for (unsigned int i = 0; i < g_maxSubscriptionCount; ++i)
	{
		totalItemsCount += m_subscriptions[i]->getMyDaSubscriptionItemsCount();

		if ((rand() % 2) || (m_subscriptions[i]->getMyDaSubscriptionItemsCount() == 0))
		{
			continue;
		}

		unsigned int noOfItems = rand() % g_maxItemsPerSubscription;

		m_subscriptions[i]->lock();
		m_removedItemsJanitor.lock();
		noOfItems = m_subscriptions[i]->removeDaItems(noOfItems, m_removedItems);
		m_removedItemsJanitor.unlock();

		if (g_tracesOn == true)
		{
			if (g_syncDisplay.aLock(0))
			{
				_tprintf(_T("Subscription %d (before: %d items), removed %d DA Items:\n"), i, m_subscriptions[i]->getMyDaSubscriptionItemsCount(), noOfItems);
				g_syncDisplay.aUnlock();
			}
		}
		else
		{
			if (g_syncDisplay.aLock(0))
			{
				_tprintf(_T("sub%dRm%dDaItms(%dnow) "), i, noOfItems, m_subscriptions[i]->getMyDaSubscriptionItemsCount());
				g_syncDisplay.aUnlock();
			}
		}

		m_subscriptions[i]->unlock();
	}

} // end removeRandomItems()

void OpcClient::addRandomItems()
{
	if (g_syncDisplay.aLock(0))
	{
		if (g_tracesOn)
		{
			_tprintf(_T("Re-add DA Items:\n"));
		}
		else
		{
			_tprintf(_T("addDaItms "));
		}
		g_syncDisplay.aUnlock();
	}

	unsigned int totalItemsCount = m_removedItems.size();

	for (unsigned int i = 0; i < g_maxSubscriptionCount; ++i)
	{
		totalItemsCount += m_subscriptions[i]->getMyDaSubscriptionItemsCount();

		if ((rand() % 2) && (m_removedItems.size() > 0))
		{
			continue;
		}

		m_subscriptions[i]->lock();
		int numAddedItems = rand() % g_maxItemsPerSubscription;
		int numItemsBefore = m_subscriptions[i]->getMyDaSubscriptionItemsCount();
		m_removedItemsJanitor.lock();
		numAddedItems = m_subscriptions[i]->addDaItems(numAddedItems, m_removedItems);
		m_removedItemsJanitor.unlock();

		if (g_tracesOn == true)
		{
			if (g_syncDisplay.aLock(0))
			{
				_tprintf(_T("Subscription %d (before: %d items), re-added %d DA Items:\n"), i, numItemsBefore, numAddedItems);
				g_syncDisplay.aUnlock();
			}
		}
		else if (i % g_silentDisplayCount == 0)
		{
			if (g_syncDisplay.aLock(0))
			{
				_tprintf(_T("sub%dAdd%dDaItms "), i, numAddedItems);
				g_syncDisplay.aUnlock();
			}
		}

		if ((m_subscriptions[i]->getCurrentState() != EnumObjectState_DISCONNECTED) && (m_session->getCurrentState() != EnumObjectState_DISCONNECTED))
		{
			m_subscriptions[i]->connect(!g_connectItemsFirst, m_subscriptions[i]->getCurrentState() == EnumObjectState_ACTIVATED, NULL);
		}

		m_subscriptions[i]->unlock();
	}

} // end void OpcClient::addRandomItems()

