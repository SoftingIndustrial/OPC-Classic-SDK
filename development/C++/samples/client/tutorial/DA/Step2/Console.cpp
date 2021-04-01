#include "stdafx.h"


#include "OpcClient.h"


using namespace std;

HANDLE g_endEvent = 0;


BOOL WINAPI ControlHandler(IN unsigned long crtlType)   // control type
{
	switch (crtlType)
	{
	case CTRL_BREAK_EVENT:  // use Ctrl+C or Ctrl+Break to end server
	case CTRL_C_EVENT:
	case CTRL_CLOSE_EVENT:
		if (g_endEvent)
		{
			SetEvent(g_endEvent);
		}

		return TRUE;
	}

	return FALSE;
}   //  end controlHandler


int _tmain(int argc, _TCHAR* argv[])
{
	long result = S_OK;
	g_endEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	SetConsoleCtrlHandler(ControlHandler, TRUE);
	//  create the OpcClient instance
	createOpcClient();
	OpcClient* pClient = getOpcClient();

	//  TODO - binary license activation
	//  Fill in your binary license activation keys here
	//
	//  NOTE: you can activate one or all of the features at the same time
	//  firstly activate the COM-DA Client feature
	//result = getApplication()->activate(EnumFeature_DA_CLIENT, _T("XXXX-XXXX-XXXX-XXXX-XXXX"));

	if (!SUCCEEDED(result))
	{
		return result;
	}

	//  activate the XML-DA Client Feature
	//result = getApplication()->activate(EnumFeature_XMLDA_CLIENT, _T("XXXX-XXXX-XXXX-XXXX-XXXX"));

	if (!SUCCEEDED(result))
	{
		return result;
	}

	//  END TODO - binary license activation

	//  initialize the client instance
	if (!SUCCEEDED(pClient->initialize()))
	{
		pClient->terminate();
		destroyOpcClient();
		::CoUninitialize();
		CloseHandle(g_endEvent);
		return 1;
	}

	//  initialize the DA client simulation
	result |= pClient->initializeDaObjects();

	//  start the simulation thread
	if (FAILED(result))
	{
		return 0;
	}   //  end if

	_tprintf(_T("Press Ctrl-C to exit\n"));
	unsigned long waitTime = 100;
	BOOL end = FALSE;
	unsigned long waitRet = 0;

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
			//  TODO: place your cyclic code here
		}   //  end if...else
	}   //  ens while

	pClient->terminate();
	destroyOpcClient();
	CloseHandle(g_endEvent);
	CoUninitialize();
	return 0;
}   //  end main
