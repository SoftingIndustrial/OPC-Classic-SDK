#include "stdafx.h"


#include "OpcClient.h"

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

	//  activate the COM-AE Client Feature
	//result = getApplication()->activate(EnumFeature_AE_CLIENT, _T("XXXX-XXXX-XXXX-XXXX-XXXX"));

	if (!SUCCEEDED(result))
	{
		return FALSE;
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

	//  initialize the AE client simulation
	result |= pClient->initializeAeObjects();
	_tprintf(_T("Press Ctrl-C to exit\n"));
	unsigned long waitTime = 100;
	BOOL end = FALSE;
	unsigned long waitRet = 0;

	while (!end)
	{
		waitRet = WaitForSingleObject(g_endEvent, waitTime);
		waitTime = 1000;

		if (waitRet == WAIT_OBJECT_0)
		{
			end = TRUE;
		}
		else
		{
			tstring conditionState = pClient->getConditionState();
			_tprintf(_T("%s"), conditionState.c_str());
		}   //  end if...else
	}   //  ens while

	pClient->terminate();
	destroyOpcClient();
	CloseHandle(g_endEvent);
	CoUninitialize();
	return 0;
}   //  end main
