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
	getApp()->setVersionOtb(447);
	getApp()->enableTracing(
		EnumTraceGroup_ALL,
		EnumTraceGroup_ALL,
		EnumTraceGroup_CLIENT,
		EnumTraceGroup_CLIENT,
		_T("Trace.txt"),
		1000000,
		2);

	long result = getApp()->initialize();

	return result;
}   //  end initialize

long OpcClient::processCommandLine(tstring command)
{
	return getApp()->processCommandLine(command);
}   //  end processCommandLine

void OpcClient::terminate()
{
	getApp()->terminate();

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
	long connectResult = m_aeSession->connect(TRUE, TRUE, &executionOptions);
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
	long result = m_aeSession->getAeConditionState(
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

