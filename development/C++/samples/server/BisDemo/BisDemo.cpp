#include "msxml6.h"

#include "ServerApplication.h"
#include "ServerCommon.h"

#include "DataPointTypeProperty.h"
#include "CommandProperty.h"
#include "BaseEvent.h"
#include "DataEvent.h"
#include "DeviceEvent.h"
#include "BaseAeElement.h"
#include "BaseDaElement.h"
#include "DataPointElement.h"
#include "CommandElement.h"
#include "DeviceElement.h"
#include "BisCreator.h"

#include <vector>
#include <iostream>
#include <cstdlib>
#include <ctime>

#include <crtdbg.h>

using namespace SoftingOPCToolboxServer;


HANDLE g_endEvent;

BaseDaElement* device1Node = NULL;
BaseDaElement* device2Node = NULL;



//-----------------------------------------------------------------------------
// HandleShutdownRequest
//
// - handle shutdown requests
//
long API_CALL handleShutdownRequest(
	void)
{
	SetEvent(g_endEvent);
	return S_OK;
}   //  end handleShutdownRequest



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
}   //  controlHandler



//-----------------------------------------------------------------------------
// initServer
//
// - initialize server
//
void initServer(
	void)
{
	getApp()->setVersionOtb(447);
	getApp()->setAppType(EnumApplicationType_EXECUTABLE);
	getApp()->setClsidDa(_T("{CB389B46-F767-402e-A85E-C63623658BBE}"));
	getApp()->setProgIdDa(_T("Softing.OPCToolbox.C++.BISDemo.DA.1"));
	getApp()->setVerIndProgIdDa(_T("Softing.OPCToolbox.C++.BISDemo.DA"));
	getApp()->setClsidAe(_T("{2B1AAB3E-7ADC-4cc8-8631-5B6DBF7CA93A}"));
	getApp()->setProgIdAe(_T("Softing.OPCToolbox.C++.BISDemo.AE.1"));
	getApp()->setVerIndProgIdAe(_T("Softing.OPCToolbox.C++.BISDemo.AE"));
	getApp()->setDescription(_T("Softing OPC Toolkit C++ BIS Demo Server"));
	getApp()->setServiceName(tstring());
	getApp()->setIpPortHTTP(0);
	getApp()->setUrlDa(_T("/OPC/DA"));
	getApp()->setMajorVersion(4);
	getApp()->setMinorVersion(47);
	getApp()->setPatchVersion(1);
	getApp()->setBuildNumber(0);
	getApp()->setVendorInfo(tstring(_T("Softing Industrial Automation GmbH")));
	getApp()->setMinUpdateRateDa(100);
	getApp()->setClientCheckPeriod(30000);
	getApp()->setAddressSpaceDelimiter(_T('.'));
	getApp()->setPropertyDelimiter(_T('#'));
	getApp()->setSupportDisableConditions(false);
	getApp()->ShutdownRequest = handleShutdownRequest;
	getApp()->enableTracing(
		EnumTraceGroup_ALL,
		EnumTraceGroup_ALL,
		EnumTraceGroup_SERVER,
		EnumTraceGroup_SERVER,
		_T("Trace.txt"),
		1000000,
		2);
}   //  end initServer



//-----------------------------------------------------------------------------
// buildNamespace
//
// - builds the namespace
//
static void buildNamespace(
	void)
{
	DaAddressSpaceRoot* daRoot = getApp()->getDaAddressSpaceRoot();
	device1Node = (BaseDaElement*)getApp()->getCreator()->createDaAddressSpaceElement();
	device1Node->setHasChildren(true);
	device1Node->setName(tstring(_T("BMS1")));
	daRoot->addChild(device1Node);
	DataPointElement* datapoint = new DataPointElement(tstring(_T("Datapoint11")), 5002, tstring(_T("Meeting room 1")));
	device1Node->addChild(datapoint);
	datapoint = new DataPointElement(tstring(_T("Datapoint12")), 5003, tstring(_T("Office 1")));
	device1Node->addChild(datapoint);
	datapoint = new DataPointElement(tstring(_T("Datapoint13")), 5003, tstring(_T("Office 2")));
	device1Node->addChild(datapoint);
	datapoint = new DataPointElement(tstring(_T("Datapoint14")), 5003, tstring(_T("Office 3")));
	device1Node->addChild(datapoint);
	datapoint = new DataPointElement(tstring(_T("Datapoint15")), 5003, tstring(_T("Office 4")));
	device1Node->addChild(datapoint);
	datapoint = new DataPointElement(tstring(_T("Datapoint16")), 5003, tstring(_T("Office 5")));
	device1Node->addChild(datapoint);
	DeviceElement* device = new DeviceElement(tstring(_T("DeviceState")), 5001, tstring(_T("Device connection status datapoint")));
	device1Node->addChild(device);
	device->setConnected(false);
	device2Node = (BaseDaElement*)getApp()->getCreator()->createDaAddressSpaceElement();
	device2Node->setHasChildren(true);
	device2Node->setName(tstring(_T("BMS2")));
	daRoot->addChild(device2Node);
	datapoint = new DataPointElement(tstring(_T("Datapoint21")), 5004, tstring(_T("Meeting room 1")));
	device2Node->addChild(datapoint);
	datapoint = new DataPointElement(tstring(_T("Datapoint22")), 5004, tstring(_T("Office 1")));
	device2Node->addChild(datapoint);
	datapoint = new DataPointElement(tstring(_T("Datapoint23")), 5004, tstring(_T("Office 2")));
	device2Node->addChild(datapoint);
	datapoint = new DataPointElement(tstring(_T("Datapoint24")), 5004, tstring(_T("Office 3")));
	device2Node->addChild(datapoint);
	datapoint = new DataPointElement(tstring(_T("Datapoint25")), 5005, tstring(_T("Office 4")));
	device2Node->addChild(datapoint);
	datapoint = new DataPointElement(tstring(_T("Datapoint26")), 5005, tstring(_T("Office 5")));
	device2Node->addChild(datapoint);
	datapoint = new DataPointElement(tstring(_T("Datapoint27")), 5004, tstring(_T("Office 6")));
	device2Node->addChild(datapoint);
	device = new DeviceElement(tstring(_T("DeviceState")), 5001, tstring(_T("Device connection status datapoint")));
	device2Node->addChild(device);
	device->setConnected(false);
	CommandElement* cmd = new CommandElement();
	CommandProperty* cmdProperty = new CommandProperty(5001, tstring(_T("Device State")));
	cmdProperty->addCommand(tstring(_T("1")), tstring(_T("Initialises Comms")));
	cmdProperty->addCommand(tstring(_T("0")), tstring(_T("Closes Comms")));
	cmd->addProperty(cmdProperty);
	cmdProperty = new CommandProperty(5002, tstring(_T("BMS1 5002")));
	cmdProperty->addCommand(tstring(_T("3")), tstring(_T("Init")));
	cmdProperty->addCommand(tstring(_T("2")), tstring(_T("Close")));
	cmd->addProperty(cmdProperty);
	cmdProperty = new CommandProperty(5003, tstring(_T("BMS1 5003")));
	cmdProperty->addCommand(tstring(_T("5")), tstring(_T("Resets sensor")));
	cmdProperty->addCommand(tstring(_T("4")), tstring(_T("Activates the sensor")));
	cmd->addProperty(cmdProperty);
	cmdProperty = new CommandProperty(5004, tstring(_T("BMS2 5004")));
	cmdProperty->addCommand(tstring(_T("7")), tstring(_T("Activate Zone")));
	cmdProperty->addCommand(tstring(_T("6")), tstring(_T("Deactivate Zone")));
	cmd->addProperty(cmdProperty);
	cmdProperty = new CommandProperty(5005, tstring(_T("BMS2 5005")));
	cmdProperty->addCommand(tstring(_T("9")), tstring(_T("Heating on")));
	cmdProperty->addCommand(tstring(_T("8")), tstring(_T("Heating off")));
	cmd->addProperty(cmdProperty);
	daRoot->addChild(cmd);
}   //  end buildNamespace



//-----------------------------------------------------------------------------
// buildEventCategories
//
// - builds the event categories
//
static void buildEventCategories(
	void)
{
	DataEvent::buildCategory();
	DeviceEvent::buildCategory();
}   //  end buildEventCategories



//-----------------------------------------------------------------------------
// main
//
int _tmain(int argc, _TCHAR* argv[])
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	Application* app = getApp();
	long result = S_OK;
	// set the controlhandler and end event
	g_endEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	SetConsoleCtrlHandler(controlHandler, TRUE);

	//  TODO - binary license activation
	//  Fill in your binary license activation keys here
	//
	//  NOTE: you can activate one or all of the features at the same time
	//  firstly activate the COM-DA Server feature
	//result = getApp()->activate(EnumFeature_DA_SERVER, _T("XXXX-XXXX-XXXX-XXXX-XXXX"));

	if (!SUCCEEDED(result))
	{
		return result;
	}

	//  activate the XML-DA Server Feature
	// result = getApp()->activate(EnumFeature_XMLDA_SERVER, _T("XXXX-XXXX-XXXX-XXXX-XXXX"));

	if (!SUCCEEDED(result))
	{
		return result;
	}

	//  END TODO - binary license activation
	// initialize server
	initServer();
	BisCreator creator;

	if (app && SUCCEEDED(app->initialize(&creator)))
	{
		tstring commandLine(GetCommandLine());
		result = getApp()->processCommandLine(commandLine);

		if (result == S_OK)
		{
			if (SUCCEEDED(getApp()->start()))
			{
				buildEventCategories();
				buildNamespace();
				getApp()->ready();
				_tprintf(_T("Press Ctrl-C to exit\n"));

				if (device1Node == NULL || device2Node == NULL)
				{
					_tprintf(_T("Error: Device node unsuccesfully initialized !\n"));
					return 0;
				} // end if

				//  loop speciffic variables
				unsigned long waitTime = 100;
				BOOL end = FALSE;
				unsigned long waitRet = 0;
				int simulationIndex = 0;

				while (!end)
				{
					waitRet = WaitForSingleObject(g_endEvent, waitTime);
					waitTime = 100;

					if (waitRet == WAIT_OBJECT_0)
					{
						end = TRUE;
					}
					else
					{
						//simulationIndex = rand();
						if (simulationIndex % 10 == 0)
						{
							device1Node->simulate();
						} // end if

						if (simulationIndex % 25 == 0)
						{
							device2Node->simulate();
						} // end if

						simulationIndex++;
					} // end if .. else
				} // end while

				getApp()->stop();
			} // end if
		}
		else if (result == S_FALSE)
		{
			_tprintf(_T("Registration operation %s"), (LPCTSTR)GetCommandLine());
		}
		else
		{
			_tprintf(_T("processCommandLine has failed"));
		}   //  end if ... else
	}   //  end if

	getApp()->terminate();
	CloseHandle(g_endEvent);
	releaseApp();
	return 0;
}   //  end main
