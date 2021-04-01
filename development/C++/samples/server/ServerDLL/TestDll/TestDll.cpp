#include "stdafx.h"
#include <conio.h>

typedef int (__cdecl* MYPROC)(void);

int _tmain(int argc, _TCHAR* argv[])
{
	HINSTANCE hLibraryInstance = 0;
	MYPROC ProcStart = NULL;
	MYPROC ProcStop = NULL ;
	BOOL fFreeResult = FALSE;
	//TODO before running the application, copy the OpcProject.dll file in the folder where the TestDll.exe is placed
	hLibraryInstance = LoadLibrary(_T("OpcProject.dll"));

	if (hLibraryInstance != NULL)
	{
		// Get the address of the "Start" exported function in the DLL
		ProcStart = (MYPROC) GetProcAddress(hLibraryInstance, "Start");
		//Get the address of the "Stop" exported function in the DLL
		ProcStop  = (MYPROC) GetProcAddress(hLibraryInstance, "Stop");
	}
	else
	{
		return 1;
	}   //  end if

	if (NULL != ProcStart)
	{
		//Call the "Start" method from the DLL using the corresponding function pointer
		long result = ProcStart();

		switch (result)
		{
		case S_FALSE:
			// registration/unregistration succeeded and application must stop
			fFreeResult = FreeLibrary(hLibraryInstance);
			return 0;
			break;

		case S_OK:
			// everything is successfully
			break;

		default:
			//problems appeared during the starting process
			fFreeResult = FreeLibrary(hLibraryInstance);
			return 1;
		}   //  end switch
	}   //  end if

	_tprintf(_T("Press \'e\' or \'q\' to exit\n\n"));
	bool end = FALSE;

	//Stay in loop until an exit key is pressed
	while (! end)
	{
		if (_kbhit())
		{
			int input;
			input = _getch();

			switch (input)
			{
			case _T('E'):
			case _T('e'):
			case _T('Q'):
			case _T('q'):
				end = TRUE;
			}
		}

		Sleep(100);
	}

	if (NULL != ProcStop)
	{
		//Call the "Stop" method from the DLL using the corresponding function pointer
		(ProcStop)();
	}   //  end if

	// Free the DLL module.
	fFreeResult = FreeLibrary(hLibraryInstance);
	return 0;
}

