#include "stdafx.h"
#include "OpcClient.h"
#include "ClientCommon.h"

using namespace std;
using namespace SoftingOPCToolboxClient;

OpcClient* instance = NULL;

OpcClient* getOpcClient(void)
{
	return instance;
}   //  end getOpcClient

void createOpcClient(void)
{
	if (instance == NULL)
	{
		instance = new OpcClient();
	}
}   //  end createOpcClient

void destroyOpcClient(void)
{
	if (instance != NULL)
	{
		delete instance;
		instance = NULL;
	}
}   //  end destroyOpcClient

OpcClient::OpcClient()
{
	m_daSession = NULL;
	m_daSubscription = NULL;
	m_daItem = NULL;
	m_aeSubscription = NULL;
	m_aeSession = NULL;
}   //  end constructor

tstring OpcClient::getStateToString(EnumConditionState state)
{
	tstring stateToString = _T("");

	if ((state & EnumConditionState_ACTIVE) == EnumConditionState_ACTIVE)
	{
		stateToString += _T(" ACT");
	}   //  end if

	if ((state & EnumConditionState_ENABLED) == EnumConditionState_ENABLED)
	{
		stateToString += _T(" ENA");
	}   //  end if

	if ((state & EnumConditionState_ACKED) == EnumConditionState_ACKED)
	{
		stateToString += _T(" ACK");
	}   //  end if

	if (state == 0)
	{
		stateToString = _T("DIS");
	}   //  end if

	return stateToString;
}   //  end getStateToString

Application* OpcClient::getApp()
{
	return getApplication();
}   //  end getApp

long OpcClient::initialize()
{
	long result = S_OK;
	getApp()->setVersionOtb(447);
	getApp()->enableTracing(
		EnumTraceGroup_ALL,
		EnumTraceGroup_ALL,
		EnumTraceGroup_CLIENT,
		EnumTraceGroup_CLIENT,
		_T("Trace.txt"),
		1000000,
		0);

	//  TODO - binary license activation
	//  Fill in your binary license activation keys here
	//
	//  NOTE: you can activate one or all of the features at the same time
	//  firstly activate the COM-DA Client feature
	// result = getApplication()->activate(EnumFeature_DA_CLIENT, _T("XXXX-XXXX-XXXX-XXXX-XXXX"));

	if (!SUCCEEDED(result))
	{
		return result;
	}

	//  activate the XML-DA Client Feature
	// result = getApplication()->activate(EnumFeature_XMLDA_CLIENT, _T("XXXX-XXXX-XXXX-XXXX-XXXX"));

	if (!SUCCEEDED(result))
	{
		return result;
	}

	//  activate the COM-AE Client Feature
	// result = getApplication()->activate(EnumFeature_AE_CLIENT, _T("XXXX-XXXX-XXXX-XXXX-XXXX"));

	if (!SUCCEEDED(result))
	{
		return FALSE;
	}

	//  END TODO - binary license activation
	result = getApp()->initialize();

	return result;
}   //  end initialize

long OpcClient::processCommandLine(tstring command)
{
	return getApp()->processCommandLine(command);
}   //  end processCommandLine

void OpcClient::terminate()
{
	getApp()->terminate();

	if (m_daSession != NULL)
	{
		delete m_daSession;
		m_daSession = NULL;
	}

	if (m_daSubscription != NULL)
	{
		delete m_daSubscription;
		m_daSubscription = NULL;
	}

	if (m_daItem != NULL)
	{
		delete m_daItem;
		m_daItem = NULL;
	}

	if (m_aeSession != NULL)
	{
		delete m_aeSession;
		m_aeSession = NULL;
	}

	if (m_aeSubscription != NULL)
	{
		delete m_aeSubscription;
		m_aeSubscription = NULL;
	}

	releaseApplication();
}   //  end terminate

void OpcClient::setServiceName(tstring serviceName)
{
	getApp()->setServiceName(serviceName);
}   //  end setServiceName

void OpcClient::trace(
	EnumTraceLevel aLevel,
	EnumTraceGroup aMask,
	TCHAR* anObjectID,
	TCHAR* aMessage,
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
}   //  end trace


long OpcClient::initializeAeObjects()
{
	ExecutionOptions executionOptions;
	executionOptions.setExecutionType(EnumExecutionType_SYNCHRONOUS);
	tstring url(_T("opcae:///Softing.OPCToolboxDemo_ServerAE.1/{2E565243-B238-11D3-842D-0008C779D775}"));
	m_aeSession = new MyAeSession(url);
	m_aeSubscription = new MyAeSubscription(m_aeSession);
	unsigned long connectResult = m_aeSession->connect(TRUE, TRUE, &executionOptions);
	m_aeSubscription->refreshAeConditions(&executionOptions);
	return connectResult;
}   //  end initializeAeObjects


tstring OpcClient::getConditionState()
{
	if (m_aeSession == NULL)
	{
		return _T("");
	}   //  end if

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
	unsigned long result = m_aeSession->getAeConditionState(
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
		}// end for
	}
	else
	{
		message.append(_T("Get condition state failed!\n"));
	}//end if...else

	return message;
}   //  end getConditionState


long OpcClient::initializeDaObjects()
{
	ExecutionOptions executionOptions;
	executionOptions.setExecutionType(EnumExecutionType_SYNCHRONOUS);
	executionOptions.setExecutionContext(0);
	tstring url(_T("opcda:///Softing.OPCToolboxDemo_ServerDA.1/{2E565242-B238-11D3-842D-0008C779D775}"));
	m_daSession = new MyDaSession(url);
	m_daSubscription = new MyDaSubscription(1000, m_daSession);
	m_daItem = new MyDaItem(_T("maths.sin"), m_daSubscription);
	unsigned long connectResult = m_daSession->connect(TRUE, FALSE, &executionOptions);
	return connectResult;
}   //  end initializeDaObjects

tstring OpcClient::readItem()
{
	ExecutionOptions executionOptions;
	executionOptions.setExecutionType(EnumExecutionType_SYNCHRONOUS);
	executionOptions.setExecutionContext(0);
	tstring message;
	unsigned long readCount = 1;
	long result;
	std::vector<long> readResults(readCount, E_FAIL);
	std::vector<DaItem*>itemsToRead(readCount, NULL);
	std::vector<ValueQT*> valuesToRead(readCount, NULL);
	std::vector<ValueQT*>::iterator valuesToReadIterator;
	itemsToRead[0] = m_daItem;

	if (SUCCEEDED(result = m_daSubscription->read(
							   0,
							   itemsToRead,
							   valuesToRead,
							   readResults,
							   &executionOptions)))
	{
		message.append(_T(" \nRead item synchronously using subscription \n"));

		for (unsigned int i = 0; i < readCount; i++)
		{
			if (SUCCEEDED(readResults[i]))
			{
				message.append(itemsToRead[i]->getId().c_str());
				message.append(_T(" - "));
				message.append(valuesToRead[i]->toString().c_str());
				message.append(_T("\n"));
			} //end if
			else
			{
				message.append(_T("Read failed"));
			} //end if...else
		} //end for

		for (valuesToReadIterator = valuesToRead.begin(); valuesToReadIterator != valuesToRead.end(); valuesToReadIterator++)
		{
			if (*valuesToReadIterator != NULL)
			{
				delete *valuesToReadIterator;
			}   //  end if
		} //end for
	} //end if
	else
	{
		message.append(_T("Read failed"));
	} //end if...else

	return message;
}   //  end readItem

