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
	subscription = NULL;
	session = NULL;
} // end constructor

tstring OpcClient::getStateToString(EnumConditionState state)
{
	tstring stateToString = _T("");

	if ((state & EnumConditionState_ACTIVE) == EnumConditionState_ACTIVE)
	{
		stateToString += _T(" ACT");
	} // end if

	if ((state & EnumConditionState_ENABLED) == EnumConditionState_ENABLED)
	{
		stateToString += _T(" ENA");
	} // end if

	if ((state & EnumConditionState_ACKED) == EnumConditionState_ACKED)
	{
		stateToString += _T(" ACK");
	} // end if

	if (state == 0)
	{
		stateToString = _T("DIS");
	} // end if

	return stateToString;
} // end getStateToString

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

	// activate the COM-AE Client Feature
	// result = getApplication()->activate(EnumFeature_AE_CLIENT, _T("XXXX-XXXX-XXXX-XXXX-XXXX"));
	if (!SUCCEEDED(result))
	{
		return FALSE;
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
	if (session->getCurrentState() != EnumObjectState_DISCONNECTED)
	{
		session->disconnect(NULL);
	}

	if (subscription->getCurrentState() != EnumObjectState_DISCONNECTED)
	{
		subscription->disconnect(NULL);
	}

	delete aeReturnedAttributes[0];
	session->removeAeSubscription(subscription);
	getApp()->removeAeSession(session);
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


long OpcClient::initializeAeObjects()
{
	count = 3;
	executionOptions.setExecutionType(EnumExecutionType_ASYNCHRONOUS);
	executionOptions.setExecutionContext(0);
	executionContext = executionOptions.getExecutionContext();
	session = new MyAeSession(_T("opcae:///Softing.OPCToolboxDemo_ServerAE.1/{2E565243-B238-11D3-842D-0008C779D775}"));
	subscription = new MyAeSubscription(session);
	long connectResult = session->connect(TRUE, FALSE, NULL);
	//define the event areas that will be used to filter events
	//TODO replace the array below with your own areas
	// NOTE: Areas excluded from this filter will not send events/conditions to the AE client.
	//       If no filter is set, all areas will fire events.
	areas.push_back(_T("computer.mouse"));
	areas.push_back(_T("computer.clock"));
	//set the previously defined areas for filtering
	subscription->setFilterAreas(areas);

	//define the event sources that will be used to filter events
	//TODO replace the array below with your own sources
	// NOTE: Sources excluded from this filter will not send events/conditions to the AE client.
	//       If no filter is set, all sources will fire events.
	sources.push_back(_T("computer.mouse.right button"));
	sources.push_back(_T("computer.mouse.middle button"));
	sources.push_back(_T("computer.clock.timer"));
	sources.push_back(_T("computer.clock.time slot 1"));
	sources.push_back(_T("computer.clock.time slot 2"));
	//set the previously defined sources for filtering
	subscription->setFilterSources(sources);

	//define the categories that will be used to filter events ("time tick" category is used)
	//TODO replace the array below with your own category ids
	// NOTE: Category IDs excluded from this filter will not send events/conditions to the AE client.
	//       If no filter is set, all categories will fire events.
	categoryIds.push_back(1);
	categoryIds.push_back(2);
	categoryIds.push_back(3);
	//set the previously defines categoryIds for filtering
	subscription->setFilterCategories(categoryIds);

	// NOTE: Event/conditions property IDs not included in this filter will not be displayed.
	//       If no property filter is set, NO PROPERTIES WILL BE SENT WITH EACH EVENT/CONDITION (except for standard AE event/condition properties).
	aeReturnedAttributes.assign(3, NULL);

	// category ID: 1 (time tick)
	// attribute IDs: 1 (second)
	attributesIds.assign(1, 0);
	attributesIds[0] = 1;
	aeReturnedAttributes[0] = new AeReturnedAttributes();
	aeReturnedAttributes[0]->setAttributesIds(attributesIds);
	aeReturnedAttributes[0]->setCategoryId(1);

	// category ID: 2 (time slot)
	// attribute IDs: 2 (second), 3 (start second), 4 (end second)
	attributesIds.assign(3, 0);
	attributesIds[0] = 2;
	attributesIds[1] = 3;
	attributesIds[2] = 4;
	aeReturnedAttributes[1] = new AeReturnedAttributes();
	aeReturnedAttributes[1]->setAttributesIds(attributesIds);
	aeReturnedAttributes[1]->setCategoryId(2);

	// category ID: 3 (mouse click)
	// attribute IDs: 5 (x position), 6 (y position)
	attributesIds.assign(2, 0);
	attributesIds[0] = 5;
	attributesIds[1] = 6;
	aeReturnedAttributes[2] = new AeReturnedAttributes();
	aeReturnedAttributes[2]->setAttributesIds(attributesIds);
	aeReturnedAttributes[2]->setCategoryId(3);

	subscription->setReturnedAttributes(aeReturnedAttributes);
	return connectResult;
} // end initializeAeObjects


tstring OpcClient::getConditionState()
{
	if (session == NULL)
	{
		return _T("");
	} // end if

	ExecutionOptions executionOptions;
	executionOptions.setExecutionType(EnumExecutionType_SYNCHRONOUS);
	tstring message;
	AeConditionState conditionState;
	unsigned long count = 0;
	std::vector<AeAttribute*> attributes(count, NULL);
	tstring sourcePath = _T("computer.clock.time slot 1");
	tstring conditionName = _T("between");
	_TCHAR buffer[200];
	memset(buffer, 0, 200);
	long result = session->getAeConditionState(
					  sourcePath,
					  conditionName,
					  attributes,
					  conditionState,
					  &executionOptions);

	if (SUCCEEDED(result))
	{
		message.append(_T("The condition state is: \n"));
		message.append(_T(" Source Path: computer.clock.time slot 1 \n"));
		message.append(_T(" Condition Name: between \n"));
		tstring state = this->getStateToString(conditionState.getState());
		message.append(_T(" State: "));
		message.append(state);
		message.append(_T("\n"));
		_stprintf(buffer, _T(" Quality: %d"), conditionState.getQuality());
		message.append(buffer);
		message.append(_T("\n"));
		message.append(_T(" ActiveTime: "));
		message.append((conditionState.getConditionActiveTime()).toString());
		message.append(_T("\n"));
		message.append(_T(" InactiveTime: "));
		message.append((conditionState.getConditionInactiveTime()).toString());
		message.append(_T("\n"));
		message.append(_T(" AckTime: "));
		message.append((conditionState.getConditionAckTime()).toString());
		message.append(_T("\n"));
		_stprintf(buffer, _T(" Ack id: %s"), conditionState.getAcknowledgerId().c_str());
		message.append(buffer);
		message.append(_T("\n"));
		message.append(_T(" Ack comment: "));
		message.append(conditionState.getAcknowledgerComment().c_str());
		message.append(_T("\n"));
		tstring activeSubconditionTime = (conditionState.getSubConditionActiveTime()).toString();
		message.append(_T(" Active subcondition time: "));
		message.append(activeSubconditionTime.c_str());
		message.append(_T("\n"));
		message.append(_T(" Active subcondition name: "));
		message.append(conditionState.getActiveSubConditionName().c_str());
		message.append(_T("\n"));
		message.append(_T(" Active subcondition description: "));
		message.append(conditionState.getActiveSubConditionDescription().c_str());
		message.append(_T("\n"));
		message.append(_T(" Active subcondition definition: "));
		message.append(conditionState.getActiveSubConditionDefinition().c_str());
		message.append(_T("\n"));
		_stprintf(buffer, _T(" Active subcondition severity: %d"), conditionState.getActiveSubConditionSeverity());
		message.append(buffer);
		message.append(_T("\n"));
		std::vector<tstring> subConditionNames = conditionState.getSubConditionsNames();
		std::vector<tstring> subConditionsDefinitions = conditionState.getSubConditionsDefinitions();
		std::vector<tstring> subConditionsDescriptions = conditionState.getSubConditionsDescriptions();
		std::vector<unsigned long> subConditionsSeverities = conditionState.getSubConditionsSeverities();
		_stprintf(buffer, _T(" Number of subconditions: %d\n"), subConditionNames.size());
		message.append(buffer);

		for (unsigned int i = 0; i < subConditionNames.size(); i++)
		{
			message.append(_T("	Subcondition name: "));
			message.append(subConditionNames[i].c_str());
			message.append(_T("\n"));
			message.append(_T("	Subcondition definition: "));
			message.append(subConditionsDefinitions[i].c_str());
			message.append(_T("\n"));
			message.append(_T("	Active subcondition description: "));
			message.append(subConditionsDescriptions[i].c_str());
			message.append(_T("\n"));
			_stprintf(buffer, _T("	Subcondition severity: %d"), subConditionsSeverities[i]);
			message.append(buffer);
			message.append(_T("\n"));
		} // end for
	}
	else
	{
		message.append(_T("Get condition state failed!\n"));
	} // end if...else

	return message;
} // end getConditionState

void OpcClient::activateSyncObjects()
{
	session->connect(TRUE, TRUE, NULL);
}// end activateSyncObjects

void OpcClient::activateAsyncObjects()
{
	session->connect(TRUE, TRUE, &executionOptions);
	executionOptions.setExecutionContext(++executionContext);
}// end activateAsyncObjects

void OpcClient::connectSyncObjects()
{
	session->connect(TRUE, FALSE, NULL);
}// end connectSyncObjects

void OpcClient::connectAsyncObjects()
{
	session->connect(TRUE, FALSE, &executionOptions);
	executionOptions.setExecutionContext(++executionContext);
}// end connectAsyncObjects

void OpcClient::disconnectSyncObjects()
{
	session->disconnect(NULL);
}// end disconnectSyncObjects

void OpcClient::disconnectAsyncObjects()
{
	session->disconnect(&executionOptions);
	executionOptions.setExecutionContext(++executionContext);
}// end disconnectAsyncObjects

void OpcClient::getAsyncServerStatus()
{
	unsigned int result;

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
	unsigned int result;

	if (SUCCEEDED(result = session->getStatus(serverStatus, NULL)))
	{
		_tprintf(_T("Server Status\n"));
		_tprintf(_T("   State: %d\n"), serverStatus.getState());
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
	}
	else
	{
		_tprintf(_T("Get Status failed [0x%8.8X]\n"), result);
	}
} // end getSyncServerStatus

void OpcClient::activateConnectionMonitor()
{
	unsigned int result;

	if (SUCCEEDED(result = session->activateConnectionMonitor(true, 5000, 10, 10000, 300000)))
	{
		_tprintf(_T("Activated connection monitor\n"));
	}
	else
	{
		_tprintf(_T("Activate connection monitor failed [0x%8.8X]\n"), result);
	}
} // end activateConnectionMonitor

void OpcClient::deactivateConnectionMonitor()
{
	unsigned int result;

	if (SUCCEEDED(result = session->activateConnectionMonitor(false, 5000, 10, 10000, 300000)))
	{
		_tprintf(_T("Deactivated connection monitor\n"));
	}
	else
	{
		_tprintf(_T("Deactivate connection monitor failed [0x%8.8X]\n"), result);
	}
}// end deactivateConnectionMonitor
