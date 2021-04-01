#include "stdafx.h"

#include "OpcServer.h"
#include "OTcommon.h"


#include "Mutex.h"

#include "resource.h"


// instance handle of application
HINSTANCE s_instance;

const tstring s_windowClassName = _T("XXOPCsrv");
const tstring s_applicationName = _T("Softing XX OPC Server");

HWND s_windowHandle = 0;

// TODO: remove simulation which starts here >>>

BOOL s_simulationEnd = FALSE;
Mutex s_simulationEndMutex;
//-----------------------------------------------------------------------------
//	simulationThread - thread method used to generate simulation values
//
//-----------------------------------------------------------------------------
DWORD WINAPI simulationThread(void* context)
{
	OpcServer* pServer = getOpcServer();
	s_simulationEndMutex.lock();

	while (!s_simulationEnd)
	{
		//  simulate the AE Server events generation
		pServer->fireSimulationEvents();
		//  simulate the DA Server data changes generation
		pServer->changeSimulationValues();
		::Sleep(1000);
	}   //  end while

	s_simulationEndMutex.unlock();
	return 0;
}   //  end simulationThread


//-----------------------------------------------------------------------------
//	startSimulationThread    - thread startup function
//
//-----------------------------------------------------------------------------
void startSimulationThread(void)
{
	HANDLE hThread = 0;
	DWORD tID = -1;
	hThread = ::CreateThread(NULL, 0, &simulationThread, NULL, 0, &tID);
}   //  end startThread

// TODO: remove simulation which ends here <<<


//-----------------------------------------------------------------------------
//	wndProc  - main window message handler function
//
//-----------------------------------------------------------------------------
LRESULT CALLBACK wndProc(
	IN HWND wnd,
	IN UINT message,
	IN WPARAM wParam,
	IN LPARAM lParam)
{
	switch (message)
	{
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc;
		hdc = BeginPaint(wnd, &ps);
		EndPaint(wnd, &ps);
		break;
	}

	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	default:
		return DefWindowProc(wnd, message, wParam, lParam);
	}

	return 0;
}   //  end wndProc


//-----------------------------------------------------------------------------
//	_tWinMain
//
//-----------------------------------------------------------------------------
int APIENTRY _tWinMain(
	HINSTANCE instance,
	HINSTANCE prevInstance,
	LPTSTR cmdLine,
	int cmdShow)
{
	OTInitLib();
	s_instance = instance;
	long result = 0;
	//  create and initialize the OpcServer instance
	createOpcServer();
	OpcServer* pServer = getOpcServer();
	pServer->initialize();
	WNDCLASSEX wcex;
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = (WNDPROC)wndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = instance;
	wcex.hIcon = LoadIcon(instance, MAKEINTRESOURCE(IDI_ICON));
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = s_windowClassName.c_str();
	wcex.hIconSm = NULL;
	::RegisterClassEx(&wcex);
	s_windowHandle = ::CreateWindow(s_windowClassName.c_str(), s_applicationName.c_str(),
									WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_ICONIC,
									0, 0, 400, 0, s_windowHandle, NULL, s_instance, NULL);
	//  provide the server with the proper custom creator
	MyCreator creator;

	if (!SUCCEEDED(pServer->prepare(&creator)))
	{
		pServer->terminate();
		destroyOpcServer();
		::CoUninitialize();
		OTExitLib();
		return 1;
	}   //  end if

	//  handle the command line arguments (register/unregister, etc)
	tstring commandLine(GetCommandLine());
	result = pServer->processCommandLine(commandLine);

	if (result != S_OK)
	{
		long returnResult = 0;

		if (result == S_FALSE)
		{
			//  registration operation successful
			pServer->trace(EnumTraceLevel_INF, EnumTraceGroup_USER1, _T("_tWinMain"),
						   _T("Registration succeeded"));
			returnResult = 0;
		}
		else
		{
			//  registration operation has failed
			pServer->trace(EnumTraceLevel_INF, EnumTraceGroup_USER1, _T("_tWinMain"),
						   _T("Registration failed"));
			returnResult = 1;
		}

		//  no matter what close the application if processCommandLine returned
		//  something different of S_OK
		pServer->terminate();
		destroyOpcServer();
		::CoUninitialize();
		OTExitLib();
		return returnResult;
	}   //  end if

	//  Start the OPC server's I/O internal mechanism
	if (SUCCEEDED(pServer->start()))
	{
		//  build the namespace
		pServer->buildAddressSpace();
		pServer->buildEventCategories();
		//  declare the namespaces built and the server ready for clients to connect
		pServer->ready();
	}   //  end if

	// TODO: remove simulation which starts here >>>
	//  start the simulation thread
	if (SUCCEEDED(result))
	{
		startSimulationThread();
	}   //  end if

	// TODO: remove simulation which ends here <<<
#ifdef _DEBUG

	//  Display the window
	if (s_windowHandle)
	{
		::ShowWindow(s_windowHandle, SW_NORMAL);
		::UpdateWindow(s_windowHandle);
	}

#endif
	// main message loop
	MSG msg;
	WPARAM ret = 0;

	while (::GetMessage(&msg, NULL, 0, 0))
	{
		::TranslateMessage(&msg);
		::DispatchMessage(&msg);
	}

	ret = msg.wParam;
	//  terminate the simulation
	s_simulationEnd = TRUE;
	s_simulationEndMutex.lock();
	s_simulationEndMutex.unlock();
	pServer->stop();
	pServer->terminate();
	destroyOpcServer();
	::CoUninitialize();
	OTExitLib();
	return 0;
}   //  end Main
