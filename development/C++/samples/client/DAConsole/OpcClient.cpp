#include "stdafx.h"

#include "OpcClient.h"

#include "ClientCommon.h"

using namespace std;
using namespace SoftingOPCToolboxClient;

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
	session = NULL;
	subscription = NULL;
	item1 = NULL;
	item2 = NULL;
	item3 = NULL;
	item4 = NULL;
	item5 = NULL;
	item6 = NULL;
} // end constructor

void OpcClient::setCredentials(IN const tstring& usr, IN const tstring& pwd)
{
	m_usr = usr;
	m_pwd = pwd;
}


Application* OpcClient::getApp()
{
	return getApplication();
} // end getApp

long OpcClient::initialize()
{
	long result = EnumResultCode_S_OK;
	getApp()->setVersionOtb(447);
	getApp()->enableTracing(
		EnumTraceGroup_ALL,
		EnumTraceGroup_ALL,
		EnumTraceGroup_CLIENT,
		EnumTraceGroup_CLIENT,
		_T("Trace.txt"),
		1000000,
		0);

	// TODO - binary license activation
	// Fill in your binary license activation keys here
	//
	// NOTE: you can activate one or all of the features at the same time

	//  activate the COM-DA Client feature
	// result = getApplication()->activate(EnumFeature_DA_CLIENT, _T("XXXX-XXXX-XXXX-XXXX-XXXX"));
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
	if ((session != NULL) && (session->getCurrentState() != EnumObjectState_DISCONNECTED))
	{
		session->disconnect(NULL);
	}

	if (subscription != NULL)
	{

		if (subscription->getCurrentState() != EnumObjectState_DISCONNECTED)
		{
			subscription->disconnect(NULL);
		}

		subscription->removeDaItem(item1);
		subscription->removeDaItem(item2);
		subscription->removeDaItem(item3);
		subscription->removeDaItem(item4);
		subscription->removeDaItem(item5);
		subscription->removeDaItem(item6);

		if (session != NULL)
		{
			session->removeDaSubscription(subscription);
		}
	}
	
	if (session != NULL)
	{
		getApp()->removeDaSession(session);
	}

	getApp()->terminate();

	if (session != NULL)
	{
		delete session;
		session = NULL;
	}

	if (subscription != NULL)
	{
		delete subscription;
		subscription = NULL;
	}

	if (item1 != NULL)
	{
		delete item1;
		item1 = NULL;
	}

	if (item2 != NULL)
	{
		delete item2;
		item2 = NULL;
	}

	if (item3 != NULL)
	{
		delete item3;
		item3 = NULL;
	}

	if (item4 != NULL)
	{
		delete item4;
		item4 = NULL;
	}

	if (item5 != NULL)
	{
		delete item5;
		item5 = NULL;
	}

	if (item6 != NULL)
	{
		delete item6;
		item6 = NULL;
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
	const unsigned long LENGTH = 400;
	TCHAR buffer[LENGTH + 1] = { 0 };
	va_list arglist;
	va_start(arglist, aMessage);
#ifdef TBC_OS_WINDOWS
	_vsntprintf(buffer, LENGTH, aMessage, arglist);
#endif
#ifdef TBC_OS_LINUX
	vsnprintf(buffer, LENGTH, aMessage, arglist);
#endif
	getApp()->trace(aLevel, aMask, anObjectID, buffer);
} // end trace


long OpcClient::initializeDaObjects()
{
	readCount = 5;
	writeCount = 4;
	updateCount = 2;
	itemsToRead.assign(readCount, NULL);
	itemsToWrite.assign(writeCount, NULL);
	valuesToRead.assign(readCount, NULL);
	valuesToWrite.assign(writeCount, NULL);
	readResults.assign(readCount, E_FAIL);
	writeResults.assign(writeCount, E_FAIL);
	updateResults.assign(updateCount, E_FAIL);
	executionOptions.setExecutionType(EnumExecutionType_ASYNCHRONOUS);
	executionOptions.setExecutionContext(0);
	executionContext = executionOptions.getExecutionContext();
	aValue = NULL;
#ifdef TBC_OS_WINDOWS
	itemIdsForRead.push_back(_T("increment.UI1"));
	itemIdsForRead.push_back(_T("maths.sin"));
	itemIdsForRead.push_back(_T("time.local.second"));
	itemIdsForRead.push_back(_T("increment.BSTR"));
	itemIdsForRead.push_back(_T("secure.I2"));
	itemPathsForRead.push_back(_T(""));
	itemPathsForRead.push_back(_T(""));
	itemPathsForRead.push_back(_T(""));
	itemPathsForRead.push_back(_T(""));
	itemPathsForRead.push_back(_T(""));
	itemIdsForWrite.push_back(_T("increment.UI1"));
	itemIdsForWrite.push_back(_T("increment.BSTR"));
	itemIdsForWrite.push_back(_T("secure.I2"));
	itemIdsForWrite.push_back(_T("IOP.static.I4 array"));
	itemPathsForWrite.push_back(_T(""));
	itemPathsForWrite.push_back(_T(""));
	itemPathsForWrite.push_back(_T(""));
	itemPathsForWrite.push_back(_T(""));
#endif
#ifdef TBC_OS_LINUX
	itemIdsForRead.push_back(_T("all data types.dynamic.UI1"));
	itemIdsForRead.push_back(_T("all data types.dynamic.R4"));
	itemIdsForRead.push_back(_T("all data types.dynamic.UI2"));
	itemIdsForRead.push_back(_T("all data types.dynamic.BSTR"));
	itemIdsForRead.push_back(_T("all data types.secure.I2"));
	itemPathsForRead.push_back(_T(""));
	itemPathsForRead.push_back(_T(""));
	itemPathsForRead.push_back(_T(""));
	itemPathsForRead.push_back(_T(""));
	itemPathsForRead.push_back(_T(""));
	itemIdsForWrite.push_back(_T("all data types.dynamic.UI1"));
	itemIdsForWrite.push_back(_T("all data types.dynamic.BSTR"));
	itemIdsForWrite.push_back(_T("all data types.secure.I2"));
	itemIdsForWrite.push_back(_T("all data types.dynamic.I4 array"));
	itemPathsForWrite.push_back(_T(""));
	itemPathsForWrite.push_back(_T(""));
	itemPathsForWrite.push_back(_T(""));
	itemPathsForWrite.push_back(_T(""));
#endif
	session = NULL;
#ifdef TBC_OS_WINDOWS
	// TODO add your server URL here
	// this is the server url for Softing OPCToolbox Demo Server
	// COM-DA url
	session = new MyDaSession(_T("opcda:///Softing.OPCToolboxDemo_ServerDA.1/{2E565242-B238-11D3-842D-0008C779D775}"));	
	
	//	session = new MyDaSession("http://localhost:8079/OPC/DA");
#endif
#ifdef TBC_OS_LINUX
	//  TODO set the COM-DA url here; Note the url must be changed to work properly
	session = new MyDaSession("http://localhost:8079/OPC/DA");
#endif
#ifdef TBC_OS_WINDOWS
	subscription = new MyDaSubscription(1000, session);
	item1 = new MyDaItem(_T("increment.UI1"), subscription);
	item2 = new MyDaItem(_T("maths.sin"), subscription);
	item3 = new MyDaItem(_T("time.local.second"), subscription);
	item4 = new MyDaItem(_T("increment.BSTR"), subscription);
	item5 = new MyDaItem(_T("secure.I2"), subscription);
	item6 = new MyDaItem(_T("IOP.static.I1 array"), subscription);
#endif
#ifdef TBC_OS_LINUX
	//  It is working against DemoServer OPC server running on localhost
	subscription = new MyDaSubscription(1000, session);
	item1 = new MyDaItem(_T("all data types.dynamic.UI1"), subscription);
	item2 = new MyDaItem(_T("all data types.dynamic.R4"), subscription);
	item3 = new MyDaItem(_T("all data types.dynamic.UI2"), subscription);
	item4 = new MyDaItem(_T("all data types.dynamic.BSTR"), subscription);
	item5 = new MyDaItem(_T("all data types.secure.I2"), subscription);
	item6 = new MyDaItem(_T("all data types.dynamic.I1 array"), subscription);
#endif
	itemsToRead[0] = item1;
	itemsToRead[1] = item2;
	itemsToRead[2] = item3;
	itemsToRead[3] = item4;
	itemsToRead[4] = item5;
	itemsToWrite[0] = item1;
	itemsToWrite[1] = item4;
	itemsToWrite[2] = item5;
	itemsToWrite[3] = item6;
	long connectResult = session->connect(TRUE, FALSE, NULL);
	return connectResult;
} // end initializeDaObjects

void OpcClient::readSyncItem()
{
	ValueQT itemValue;
	long itemResult;
	item1->read(100, itemValue, itemResult, NULL);

	_tprintf(_T(" \nDirect item read:\n"));

	if (SUCCEEDED(itemResult))
	{
		_tprintf(_T("%-23.23s - %s\n"), item1->getId().c_str(), itemValue.toString().c_str());
	} //end if
	else
	{
		_tprintf(_T("Read failed - %s [0x%8.8X]\n"), item1->getId().c_str() , (unsigned int)itemResult);
	} //end else
} // end readItem

void OpcClient::activateAsyncSubscription()
{
	subscription->connect(TRUE, TRUE, &executionOptions);
	executionOptions.setExecutionContext(++executionContext);
} // end activateAsyncSubscription

void OpcClient::activateSyncSubscription()
{
	subscription->connect(TRUE, TRUE, NULL);
} // end activateSyncSubscription

void OpcClient::connectAsyncSubscription()
{
	subscription->connect(TRUE, FALSE, &executionOptions);
	executionOptions.setExecutionContext(++executionContext);
} // end connectAsyncSubscription

void OpcClient::connectSyncSubscription()
{
	subscription->connect(TRUE, FALSE, NULL);
} // end connectSyncSubscription

void OpcClient::disconnectAsyncSubscription()
{
	subscription->disconnect(&executionOptions);
	executionOptions.setExecutionContext(++executionContext);
} // end disconnectAsyncSubscription

void OpcClient::disconnectSyncSubscription()
{
	subscription->disconnect(NULL);
} // end disconnectSyncSubscription

void OpcClient::readAsyncSubscription()
{
	long result;

	if (FAILED(result = subscription->read(0,
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
} // end readAsyncSubscription

void OpcClient::readSyncSubscription()
{
	std::vector<ValueQT*>::iterator valuesToReadIterator;
	long result;

	if (SUCCEEDED(result = subscription->read(0,
											  itemsToRead,
											  valuesToRead,
											  readResults,
											  NULL)))
	{
		_tprintf(_T(" \nRead items using subscription \n"));

		for (unsigned int i = 0; i < readCount; i++)
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
	}// end if
	else
	{
		_tprintf(_T("Read failed [0x%8.8X]\n"), (unsigned int)result);
	}// end else
} // end readSyncSubscription

void OpcClient::refreshSubscription()
{
	long result;

	if (FAILED(result = subscription->refresh(0, &executionOptions)))
	{
		_tprintf(_T("\nRefresh failed [0x%8.8X]\n"), (unsigned int)result);
	}//end if
	else
	{
		executionOptions.setExecutionContext(++executionContext);
	}// end else
}// end refreshSubscription

void OpcClient::readAsyncSession()
{
	long result;

	if (FAILED(result = session->read(0,
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
} // end readAsyncSession

void OpcClient::readSyncSession()
{
	std::vector<ValueQT*>::iterator valuesToReadIterator;
	long result;

	if (SUCCEEDED(result = session->read(0,
										 itemIdsForRead,
										 itemPathsForRead,
										 valuesToRead,
										 readResults,
										 NULL)))
	{
		_tprintf(_T("\nRead items using session\n"));

		for (unsigned int i = 0; i < readCount; i++)
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
	} // end else
} // end readSyncSession

void OpcClient::writeSyncSubscription()
{
	long result;
	DateTime writeDateTime;
	
	data.setUI1(1);
	aValue = new ValueQT(data, EnumQuality_QUALITY_NOT_SET, writeDateTime);
	valuesToWrite[0] = aValue;

	data.setBSTR(_T("Softing"));
	aValue = new ValueQT(data, EnumQuality_QUALITY_NOT_SET, writeDateTime);
	valuesToWrite[1] = aValue;

	data.setI2(2);
	aValue = new ValueQT(data, EnumQuality_QUALITY_NOT_SET, writeDateTime);
	valuesToWrite[2] = aValue;

	//Define array bounds
	int lowerBound = 1;
	int boundLength = 2;

	//Create basic array
	//This will be the actual information passed to the Item
	int* basicArray = new int[boundLength];
	basicArray[0] = 5;
	basicArray[1] = 6;

	//Define cursor to access the safe array
	int* pArrayVar;

	//Array variant of type I4
	data.vt = VT_I4 | VT_ARRAY;

	//Create safe array vector with lower bound / bound length
	data.parray  = SafeArrayCreateVector(VT_I4, lowerBound, boundLength);
	data.SafeArrayAccessData(data.parray, (void**) &pArrayVar);
	memcpy(pArrayVar, basicArray, boundLength * sizeof(int));
	SafeArrayUnaccessData(data.parray);

	//Prepare array with lower bound for write.

	aValue = new ValueQT(data, EnumQuality_QUALITY_NOT_SET, writeDateTime);
	valuesToWrite[3] = aValue;

	if (SUCCEEDED(result = subscription->write(itemsToWrite,
											   valuesToWrite,
											   writeResults,
											   NULL)))
	{
		_tprintf(_T(" \nWrite items using subscription \n"));

		for (unsigned int i = 0; i < writeCount; i++)
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
}// end writeSyncItem

void OpcClient::writeAsyncSubscription()
{
	long result;
	DateTime writeDateTime;
	
	data.setUI1(3);
	aValue = new ValueQT(data, EnumQuality_QUALITY_NOT_SET, writeDateTime);
	valuesToWrite[0] = aValue;
	
	data.setBSTR(_T("Softing"));
	aValue = new ValueQT(data, EnumQuality_QUALITY_NOT_SET, writeDateTime);
	valuesToWrite[1] = aValue;

	data.setI2(4);
	aValue = new ValueQT(data, EnumQuality_QUALITY_NOT_SET, writeDateTime);
	valuesToWrite[2] = aValue;

	//Define array bounds
	int lowerBound = 1;
	int boundLength = 2;

	//Create basic array
	//This will be the actual information passed to the Item
	int* basicArray = new int[boundLength];
	basicArray[0] = 7;
	basicArray[1] = 8;

	//Define cursor to access the safe array
	int* pArrayVar;

	//Array variant of type I4
	data.vt = VT_I4 | VT_ARRAY;

	//Create safe array vector with lower bound / bound length
	data.parray  = SafeArrayCreateVector(VT_I4, lowerBound, boundLength);
	data.SafeArrayAccessData(data.parray, (void**) &pArrayVar);
	memcpy(pArrayVar, basicArray, boundLength * sizeof(int));
	SafeArrayUnaccessData(data.parray);

	//Prepare array with lower bound for write.

	aValue = new ValueQT(data, EnumQuality_QUALITY_NOT_SET, writeDateTime);
	valuesToWrite[3] = aValue;

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
}// end writeAsyncItem

void OpcClient::writeAsyncSession()
{
	long result;
	DateTime writeDateTime;

	data.setUI1(5);
	aValue = new ValueQT(data, EnumQuality_QUALITY_NOT_SET, writeDateTime);
	valuesToWrite[0] = aValue;

	data.setBSTR(_T("Softing"));
	aValue = new ValueQT(data, EnumQuality_QUALITY_NOT_SET, writeDateTime);
	valuesToWrite[1] = aValue;

	data.setI2(6);
	aValue = new ValueQT(data, EnumQuality_QUALITY_NOT_SET, writeDateTime);
	valuesToWrite[2] = aValue;
	
	//Define array bounds
	int lowerBound = 1;
	int boundLength = 2;

	//Create basic array
	//This will be the actual information passed to the Item
	int* basicArray = new int[boundLength];
	basicArray[0] = 1;
	basicArray[1] = 2;

	//Define cursor to access the safe array
	int* pArrayVar;

	//Array variant of type I4
	data.vt = VT_I4 | VT_ARRAY;

	//Create safe array vector with lower bound / bound length
	data.parray  = SafeArrayCreateVector(VT_I4, lowerBound, boundLength);
	data.SafeArrayAccessData(data.parray, (void**) &pArrayVar);
	memcpy(pArrayVar, basicArray, boundLength * sizeof(int));
	SafeArrayUnaccessData(data.parray);

	//Prepare array with lower bound for write.

	aValue = new ValueQT(data, EnumQuality_QUALITY_NOT_SET, writeDateTime);
	valuesToWrite[3] = aValue;
	
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
}// end writeAsyncSession

void OpcClient::writeSyncSession()
{
	long result;
	DateTime writeDateTime;

	data.setUI1(7);
	aValue = new ValueQT(data, EnumQuality_QUALITY_NOT_SET, writeDateTime);
	valuesToWrite[0] = aValue;

	data.setBSTR(_T("Softing"));
	aValue = new ValueQT(data, EnumQuality_QUALITY_NOT_SET, writeDateTime);
	valuesToWrite[1] = aValue;

	data.setI2(8);
	aValue = new ValueQT(data, EnumQuality_QUALITY_NOT_SET, writeDateTime);
	valuesToWrite[2] = aValue;

	//Define array bounds
	int lowerBound = 1;
	int boundLength = 2;

	//Create basic array
	//This will be the actual information passed to the Item
	int* basicArray = new int[boundLength];
	basicArray[0] = 3;
	basicArray[1] = 4;

	//Define cursor to access the safe array
	int* pArrayVar;

	//Array variant of type I4
	data.vt = VT_I4 | VT_ARRAY;

	//Create safe array vector with lower bound / bound length
	data.parray  = SafeArrayCreateVector(VT_I4, lowerBound, boundLength);
	data.SafeArrayAccessData(data.parray, (void**) &pArrayVar);
	memcpy(pArrayVar, basicArray, boundLength * sizeof(int));
	SafeArrayUnaccessData(data.parray);

	//Prepare array with lower bound for write.

	aValue = new ValueQT(data, EnumQuality_QUALITY_NOT_SET, writeDateTime);
	valuesToWrite[3] = aValue;

	if (SUCCEEDED(result = session->write(itemIdsForWrite,
										  itemPathsForWrite,
										  valuesToWrite,
										  writeResults,
										  NULL)))
	{
		_tprintf(_T(" \nWrite items using session \n"));

		for (unsigned int i = 0; i < writeCount; i++)
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
}// end writeSyncSession

void OpcClient::getAsyncServerStatus()
{
	long result;

	if (FAILED(result = session->getStatus(serverStatus, &executionOptions)))
	{
		_tprintf(_T("Get server status failed"));
	} //end if
	else
	{
		executionOptions.setExecutionContext(++executionContext);
	} //end else
}// end getAsyncServerStatus

void OpcClient::getSyncServerStatus()
{
	long result;

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

		for (unsigned int i = 0; i < lcIds.size(); i++)
		{
			_tprintf(_T("   Supported LCID: %s\n"), lcIds[i].c_str());
		}

		_tprintf(_T("   Status info: %s\n"), serverStatus.getStatusInfo().c_str());
	}// end if
	else
	{
		_tprintf(_T("\nGet Status failed [0x%8.8X]\n"), (unsigned int)result);
	}// end else
}// end getSyncServerStatus

void OpcClient::changeDataRetrievalMode()
{
	EnumDataRetrievalMode currendMode = subscription->getDataRetrievalMode();
	EnumDataRetrievalMode nextMode = EnumDataRetrievalMode_ASYNCADVISE;

	_tprintf(_T("\nPrevious data retrieval mode was: "));

	switch (currendMode)
	{
	case EnumDataRetrievalMode_ASYNCADVISE:
		_tprintf(_T("Asynchronous Advises.\nNew data retrieval mode was set: Polled Refresh.\nPlease reconnect the subscription.\n"));
		nextMode = EnumDataRetrievalMode_POLLEDREFRESH;
		break;
	case EnumDataRetrievalMode_POLLEDREFRESH:
		_tprintf(_T("Polled Refresh.\nNew data retrieval mode was set: Synchronous reads.\nPlease reconnect the subscription.\n"));
		nextMode = EnumDataRetrievalMode_SYNCREAD;
		break;
	case EnumDataRetrievalMode_SYNCREAD:
		_tprintf(_T("Synchronous reads.\nNew data retrieval mode was set: Asynchronous Advises.\nPlease reconnect the subscription.\n"));
		nextMode = EnumDataRetrievalMode_ASYNCADVISE;
		break;
	default: 
		_tprintf(_T("Not Set.\nNew data retrieval mode was set: Asynchronous Advises.\nPlease reconnect the subscription.\n"));
		nextMode = EnumDataRetrievalMode_ASYNCADVISE;
		break;
	}

	subscription->setDataRetrievalMode(nextMode);
}	// end changeDataRetrievalMode

void OpcClient::activateConnectionMonitor()
{
	long result;

	if (SUCCEEDED(result = session->activateConnectionMonitor(true, 5000, 10, 10000, 300000)))
	{
		_tprintf(_T("Activated connection monitor\n"));
	}
	else
	{
		_tprintf(_T("Activate connection monitor failed [0x%8.8X]\n"), (unsigned int)result);
	}
}// end activateConnectionMonitor

void OpcClient::deactivateConnectionMonitor()
{
	long result;

	if (SUCCEEDED(result = session->activateConnectionMonitor(false, 5000, 10, 10000, 300000)))
	{
		_tprintf(_T("Deactivated connection monitor\n"));
	}
	else
	{
		_tprintf(_T("Deactivate connection monitor failed [0x%8.8X]\n"), (unsigned int)result);
	}
}// end deactivateConnectionMonitor


void OpcClient::logOnAsyncForSecureCommunication()
{
	long result;
	_tprintf(_T("Logging on asynchronously: usr=%s, pwd=%s\n"), m_usr.c_str(), m_pwd.c_str());

	EnumObjectState sessionState = session->getCurrentState();
	if (sessionState != EnumObjectState_DISCONNECTED)
	{
		if (SUCCEEDED(result = session->logon(m_usr, m_pwd, &executionOptions)))
		{
			executionOptions.setExecutionContext(++executionContext);
		}
		else
		{
			_tprintf(_T("\nLogon for secure connection failed [0x%8.8X]\n"), (unsigned int)result);
		}
	}
	else
	{
		_tprintf(_T("\nLogon for secure connection failed - session is not connected.\n"));
	}
} // end logOnAsyncForSecureCommunication

void OpcClient::logOnSyncForSecureCommunication()
{
	long result;
	_tprintf(_T("Logging on synchronously: usr=%s, pwd=%s\n"), m_usr.c_str(), m_pwd.c_str());

	EnumObjectState sessionState = session->getCurrentState();
	if (sessionState != EnumObjectState_DISCONNECTED)
	{
		if (SUCCEEDED(result = session->logon(m_usr, m_pwd, NULL)))
		{
			_tprintf(_T("\nLogon for secure connection - done.\n"));
		}
		else
		{
			_tprintf(_T("\nLogon for secure connection failed [0x%8.8X]\n"), (unsigned int)result);
		}
	}
	else
	{
		_tprintf(_T("\nLogon for secure connection failed - session is not connected.\n"));
	}
} // end logOnSyncForSecureCommunication

void OpcClient::logOffAsyncForSecureCommunication()
{
	long result;

	EnumObjectState sessionState = session->getCurrentState();
	if (sessionState != EnumObjectState_DISCONNECTED)
	{
		if (SUCCEEDED(result = session->logoff(&executionOptions)))
		{
			executionOptions.setExecutionContext(++executionContext);
		}
		else
		{
			_tprintf(_T("\nLogoff for secure connection failed [0x%8.8X]\n"), (unsigned int)result);
		}
	}
	else
	{
		_tprintf(_T("\nLogoff for secure connection failed - session is not connected.\n"));
	}
} // end logOffAsyncForSecureCommunication

void OpcClient::logOffSyncForSecureCommunication()
{
	long result;

	EnumObjectState sessionState = session->getCurrentState();
	if (sessionState != EnumObjectState_DISCONNECTED)
	{
		if (SUCCEEDED(result = session->logoff(NULL)))
		{
			_tprintf(_T("\nLogoff for secure connection - done.\n"));
		}
		else
		{
			_tprintf(_T("\nLogoff for secure connection failed [0x%8.8X]\n"), (unsigned int)result);
		}
	}
	else
	{
		_tprintf(_T("\nLogoff for secure connection failed - session is not connected.\n"));
	}
} // end logOffSyncForSecureCommunication

void OpcClient::setCredentials(void)
{
	TCHAR str[100];
	tstring usr;
	tstring pwd;
	_tprintf(_T("\tUser name: "));
#ifdef TBC_OS_WINDOWS
	_tscanf(_T("%s"), str);
#endif
#ifdef TBC_OS_LINUX
	scanf(_T("%s"), str);
#endif
	m_usr = str;
	_tprintf(_T("\tPassword: "));
#ifdef TBC_OS_WINDOWS
	_tscanf(_T("%s"), str);
#endif
#ifdef TBC_OS_LINUX
	scanf(_T("%s"), str);
#endif
	m_pwd = str;
	_tprintf(_T("Provided server authentication credentials were set: \n\tuser=%s\n\tpassword=%s\n\n"), m_usr.c_str(), m_pwd.c_str());
} // end setCredentials(void)
