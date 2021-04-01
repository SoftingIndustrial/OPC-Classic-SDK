#include "stdafx.h"
#include "Mutex.h"
#include "OSCompat.h"

#include "OpcClient.h"

#include <iostream>
#ifdef TBC_OS_WINDOWS
#include <conio.h>
#include <time.h>
#endif

#include <stdio.h>

#ifdef TBC_OS_LINUX
#include <pthread.h>
#include <semaphore.h>
#include <errno.h>
#endif

/**
 * TODO: update these settings as required
 */
char g_initialThreadCount = 13;
const BOOL g_initialThreadCountRandom = true;
const DWORD g_minInitialThreadCount = 3;
const DWORD g_maxInitialThreadCount = 15;
bool g_tracesOn = false;
bool g_threadsStarted = true;
const unsigned long g_minItemsPerSubscription = 5; // >0
const unsigned long g_maxItemsPerSubscription = 50; // >0
const unsigned int g_minSubscriptionUpdateRate = 31;
const unsigned int g_maxSubscriptionUpdateRate = 1777;
const unsigned short g_minStringLength = 150;
const unsigned short g_maxStringLength = 250;
const unsigned int g_silentDisplayCount = 10;
const BOOL g_connectItemsFirst = FALSE;
bool g_allowItemRemoval = true;
bool g_allowsubscriptionDisconnection = true;
DWORD g_initialUpdateRate = 654;
DWORD g_hardcodedOperationInterval = 321;
BOOL g_dynamicOperationInterval = true;
const DWORD g_minOperationInterval = 10;
const DWORD g_maxOperationInterval = 500;
const BOOL g_writeQuality = FALSE;
const BOOL g_useHardcodedDataType = FALSE;
const VARENUM g_hardcodedDataType = VT_I8;

const unsigned int g_itemsTotalCount = 400;
const TCHAR *g_itemNames[] = { _T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager15;s=TV_00000007"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager10;s=TV_00000006"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager21;s=TV_00000035"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager15;s=TV_00000008"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager10;s=TV_00000007"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager21;s=TV_00000036"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager15;s=TV_00000009"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager10;s=TV_00000008"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager21;s=TV_00000037"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager15;s=TV_00000010"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager10;s=TV_00000009"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager22;s=TV_00000001"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager15;s=TV_00000011"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager10;s=TV_00000010"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager22;s=TV_00000002"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager15;s=TV_00000012"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager10;s=TV_00000011"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager22;s=TV_00000003"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager15;s=TV_00000013"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager10;s=TV_00000012"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager22;s=TV_00000004"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager16;s=TV_00000001"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager10;s=TV_00000013"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager22;s=TV_00000005"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager16;s=TV_00000002"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager10;s=TV_00000014"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager22;s=TV_00000006"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager16;s=TV_00000003"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager10;s=TV_00000015"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager22;s=TV_00000007"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager16;s=TV_00000004"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager10;s=TV_00000016"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager22;s=TV_00000008"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager16;s=TV_00000005"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager10;s=TV_00000017"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager22;s=TV_00000009"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager16;s=TV_00000006"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager10;s=TV_00000018"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager22;s=TV_00000010"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager16;s=TV_00000007"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager10;s=TV_00000019"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager22;s=TV_00000011"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager16;s=TV_00000008"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager10;s=TV_00000020"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager22;s=TV_00000012"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager16;s=TV_00000009"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager10;s=TV_00000021"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager22;s=TV_00000013"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager16;s=TV_00000010"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager10;s=TV_00000022"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager22;s=TV_00000014"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager16;s=TV_00000011"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager10;s=TV_00000023"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager22;s=TV_00000015"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager16;s=TV_00000012"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager11;s=TV_00000001"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager22;s=TV_00000016"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager16;s=TV_00000013"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager11;s=TV_00000002"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager22;s=TV_00000017"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager16;s=TV_00000014"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager11;s=TV_00000003"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager22;s=TV_00000018"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager16;s=TV_00000015"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager11;s=TV_00000004"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager22;s=TV_00000019"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager16;s=TV_00000016"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager11;s=TV_00000005"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager22;s=TV_00000020"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager16;s=TV_00000017"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager11;s=TV_00000006"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager22;s=TV_00000021"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager16;s=TV_00000018"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager11;s=TV_00000007"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager22;s=TV_00000022"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager16;s=TV_00000019"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager11;s=TV_00000008"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager22;s=TV_00000023"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager16;s=TV_00000020"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager11;s=TV_00000009"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager22;s=TV_00000024"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager16;s=TV_00000021"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager11;s=TV_00000010"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager22;s=TV_00000025"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager16;s=TV_00000022"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager11;s=TV_00000011"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager22;s=TV_00000026"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager16;s=TV_00000023"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager11;s=TV_00000012"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager22;s=TV_00000027"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager16;s=TV_00000024"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager11;s=TV_00000013"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager22;s=TV_00000028"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager16;s=TV_00000025"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager11;s=TV_00000014"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager22;s=TV_00000029"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager16;s=TV_00000026"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager11;s=TV_00000015"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager22;s=TV_00000030"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager16;s=TV_00000027"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager11;s=TV_00000016"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager22;s=TV_00000031"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager16;s=TV_00000028"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager11;s=TV_00000017"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager22;s=TV_00000032"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager16;s=TV_00000029"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager11;s=TV_00000018"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager22;s=TV_00000033"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager16;s=TV_00000030"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager11;s=TV_00000019"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager22;s=TV_00000034"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager11;s=TV_00000020"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager16;s=TV_00000031"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager22;s=TV_00000035"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager11;s=TV_00000021"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager17;s=TV_00000001"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager22;s=TV_00000036"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager11;s=TV_00000022"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager17;s=TV_00000002"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager22;s=TV_00000037"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager11;s=TV_00000023"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager17;s=TV_00000003"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager22;s=TV_00000038"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager11;s=TV_00000024"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager17;s=TV_00000004"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager22;s=TV_00000039"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager11;s=TV_00000025"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager17;s=TV_00000005"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager23;s=TV_00000001"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager11;s=TV_00000026"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager17;s=TV_00000006"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager23;s=TV_00000002"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager11;s=TV_00000027"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager17;s=TV_00000007"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager23;s=TV_00000003"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager17;s=TV_00000008"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager11;s=TV_00000028"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager23;s=TV_00000004"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager17;s=TV_00000009"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager11;s=TV_00000029"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager23;s=TV_00000005"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager17;s=TV_00000010"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager11;s=TV_00000030"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager23;s=TV_00000006"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager17;s=TV_00000011"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager11;s=TV_00000031"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager23;s=TV_00000007"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager17;s=TV_00000012"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager11;s=TV_00000032"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager23;s=TV_00000008"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager17;s=TV_00000013"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager11;s=TV_00000033"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager23;s=TV_00000009"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager17;s=TV_00000014"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager11;s=TV_00000034"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager23;s=TV_00000010"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager17;s=TV_00000015"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager12;s=TV_00000001"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager23;s=TV_00000011"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager17;s=TV_00000016"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager12;s=TV_00000002"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager23;s=TV_00000012"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager17;s=TV_00000017"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager12;s=TV_00000003"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager23;s=TV_00000013"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager17;s=TV_00000018"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager12;s=TV_00000004"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager23;s=TV_00000014"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager17;s=TV_00000019"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager12;s=TV_00000005"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager23;s=TV_00000015"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager18;s=TV_00000001"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager12;s=TV_00000006"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager23;s=TV_00000016"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager18;s=TV_00000002"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager12;s=TV_00000007"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager23;s=TV_00000017"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager18;s=TV_00000003"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager12;s=TV_00000008"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager23;s=TV_00000018"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager18;s=TV_00000004"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager12;s=TV_00000009"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager23;s=TV_00000019"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager18;s=TV_00000005"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager12;s=TV_00000010"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager23;s=TV_00000020"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager18;s=TV_00000006"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager12;s=TV_00000011"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager23;s=TV_00000021"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager18;s=TV_00000007"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager12;s=TV_00000012"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager23;s=TV_00000022"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager18;s=TV_00000008"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager12;s=TV_00000013"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager23;s=TV_00000023"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager18;s=TV_00000009"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager12;s=TV_00000014"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager23;s=TV_00000024"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager18;s=TV_00000010"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager12;s=TV_00000015"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager23;s=TV_00000025"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager18;s=TV_00000011"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager12;s=TV_00000016"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager23;s=TV_00000026"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager18;s=TV_00000012"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager12;s=TV_00000017"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager23;s=TV_00000027"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager18;s=TV_00000013"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager12;s=TV_00000018"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager23;s=TV_00000028"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager18;s=TV_00000014"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager12;s=TV_00000019"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager23;s=TV_00000029"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager18;s=TV_00000015"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager12;s=TV_00000020"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager23;s=TV_00000030"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager18;s=TV_00000016"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager12;s=TV_00000021"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager23;s=TV_00000031"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager18;s=TV_00000017"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager12;s=TV_00000022"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager23;s=TV_00000032"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager18;s=TV_00000018"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager12;s=TV_00000023"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager23;s=TV_00000033"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager18;s=TV_00000019"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager12;s=TV_00000024"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager23;s=TV_00000034"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager18;s=TV_00000020"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager12;s=TV_00000025"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager23;s=TV_00000035"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager18;s=TV_00000021"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager12;s=TV_00000026"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager23;s=TV_00000036"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager18;s=TV_00000022"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager12;s=TV_00000027"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager23;s=TV_00000037"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager18;s=TV_00000023"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager13;s=TV_00000001"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager23;s=TV_00000038"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager18;s=TV_00000024"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager13;s=TV_00000002"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager23;s=TV_00000039"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager18;s=TV_00000025"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager13;s=TV_00000003"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager23;s=TV_00000040"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager18;s=TV_00000026"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager13;s=TV_00000004"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager23;s=TV_00000041"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager18;s=TV_00000027"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager13;s=TV_00000005"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager23;s=TV_00000042"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager19;s=TV_00000001"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager13;s=TV_00000006"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager23;s=TV_00000043"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager19;s=TV_00000002"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager13;s=TV_00000007"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager23;s=TV_00000044"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager19;s=TV_00000003"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager13;s=TV_00000008"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager23;s=TV_00000045"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager19;s=TV_00000004"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager13;s=TV_00000009"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager23;s=TV_00000046"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager19;s=TV_00000005"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager13;s=TV_00000010"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager23;s=TV_00000047"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager19;s=TV_00000006"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager13;s=TV_00000011"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager23;s=TV_00000048"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager13;s=TV_00000012"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager19;s=TV_00000007"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager23;s=TV_00000049"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager13;s=TV_00000013"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager19;s=TV_00000008"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager23;s=TV_00000050"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager13;s=TV_00000014"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager19;s=TV_00000009"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager23;s=TV_00000051"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager13;s=TV_00000015"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager19;s=TV_00000010"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager13;s=TV_00000016"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager19;s=TV_00000011"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager13;s=TV_00000017"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager19;s=TV_00000012"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager13;s=TV_00000018"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager19;s=TV_00000013"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager13;s=TV_00000019"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager19;s=TV_00000014"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager19;s=TV_00000015"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager13;s=TV_00000020"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager13;s=TV_00000021"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager19;s=TV_00000016"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager13;s=TV_00000022"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager20;s=TV_00000001"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager13;s=TV_00000023"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager20;s=TV_00000002"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager13;s=TV_00000024"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager20;s=TV_00000003"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager13;s=TV_00000025"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager20;s=TV_00000004"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager13;s=TV_00000026"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager20;s=TV_00000005"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager13;s=TV_00000027"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager20;s=TV_00000006"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager13;s=TV_00000028"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager20;s=TV_00000007"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager13;s=TV_00000029"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager20;s=TV_00000008"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager13;s=TV_00000030"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager20;s=TV_00000009"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager13;s=TV_00000031"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager20;s=TV_00000010"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager13;s=TV_00000032"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager20;s=TV_00000011"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager13;s=TV_00000033"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager20;s=TV_00000012"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager14;s=TV_00000001"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager20;s=TV_00000013"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager14;s=TV_00000002"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager20;s=TV_00000014"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager14;s=TV_00000003"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager20;s=TV_00000015"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager14;s=TV_00000004"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager20;s=TV_00000016"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager14;s=TV_00000005"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager20;s=TV_00000017"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager14;s=TV_00000006"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager21;s=TV_00000001"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager14;s=TV_00000007"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager21;s=TV_00000002"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager14;s=TV_00000008"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager21;s=TV_00000003"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager14;s=TV_00000009"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager21;s=TV_00000004"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager14;s=TV_00000010"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager21;s=TV_00000005"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager14;s=TV_00000011"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager21;s=TV_00000006"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager14;s=TV_00000012"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager21;s=TV_00000007"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager14;s=TV_00000013"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager21;s=TV_00000008"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager14;s=TV_00000014"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager21;s=TV_00000009"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager14;s=TV_00000015"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager21;s=TV_00000010"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager14;s=TV_00000016"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager21;s=TV_00000011"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager14;s=TV_00000017"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager21;s=TV_00000012"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager14;s=TV_00000018"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager21;s=TV_00000013"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager14;s=TV_00000019"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager21;s=TV_00000014"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager14;s=TV_00000020"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager21;s=TV_00000015"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager14;s=TV_00000021"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager21;s=TV_00000016"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager14;s=TV_00000022"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager21;s=TV_00000017"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager14;s=TV_00000023"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager21;s=TV_00000018"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager14;s=TV_00000024"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager21;s=TV_00000019"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager14;s=TV_00000025"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager21;s=TV_00000020"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager14;s=TV_00000026"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager21;s=TV_00000021"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager14;s=TV_00000027"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager21;s=TV_00000022"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager14;s=TV_00000028"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager21;s=TV_00000023"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager14;s=TV_00000029"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager21;s=TV_00000024"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager14;s=TV_00000030"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager21;s=TV_00000025"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager14;s=TV_00000031"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager21;s=TV_00000026"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager14;s=TV_00000032"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager21;s=TV_00000027"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager14;s=TV_00000033"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager21;s=TV_00000028"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager15;s=TV_00000001"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager21;s=TV_00000029"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager15;s=TV_00000002"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager10;s=TV_00000001"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager21;s=TV_00000030"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager15;s=TV_00000003"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager10;s=TV_00000002"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager21;s=TV_00000031"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager15;s=TV_00000004"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager10;s=TV_00000003"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager21;s=TV_00000032"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager15;s=TV_00000005"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager10;s=TV_00000004"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager21;s=TV_00000033"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager15;s=TV_00000006"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager10;s=TV_00000005"),
	_T("OPC_UA_Server_2.nsuri=http://somecompany.com/TestServer/SimulationManager21;s=TV_00000034") };

/**
 * END TODO
 */

unsigned long g_dataChangeSetsReceived = 0;
unsigned long g_totalGoodDataChanges = 0;
unsigned long g_totalBadDataChanges = 0;
unsigned long g_legitGoodDataChanges = 0;
unsigned long g_legitBadDataChanges = 0;
bool g_subscriptionsActive = false;
HANDLE g_threadNewLine;
MyMutex g_syncDisplay;
using namespace std;
using namespace SoftingOPCToolboxClient;
#ifdef TBC_OS_WINDOWS
HANDLE g_endEvent = 0;
HANDLE g_threadsStopEvent = 0;
HANDLE g_threads[50] = {0};
#endif
#ifdef TBC_OS_LINUX
sem_t g_endSemaphor;
#endif

#ifdef TBC_OS_WINDOWS

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
} // end controlHandler
#endif

void usage(void)
{
	g_syncDisplay.aLock();
	_tprintf(_T("\nUsage:\n"));
	_tprintf(_T("Use lower case letters for synchronous execution\n"));
	_tprintf(_T("Use upper case letters for asynchronous execution\n"));
	_tprintf(_T("Press \'a\' or \'A\' to activate the objects\n"));
	_tprintf(_T("Press \'c\' or \'C\' to connect the objects\n"));
	_tprintf(_T("Press \'d\' or \'D\' to disconnect the objects\n"));
	_tprintf(_T("Press \',\' or \'>\' to connect session (deep)\n"));
	_tprintf(_T("Press \'.\' or \'<\' to disconnect session (deep)\n"));
	_tprintf(_T("Press \'r\' or \'R\' to read the items\n"));
	_tprintf(_T("Press \'w\' or \'W\' to write the items\n"));
	_tprintf(_T("Press \'f\' or \'F\' to refresh the items\n"));
	_tprintf(_T("Press \'z\' or \'Z\' to read using server object\n"));
	_tprintf(_T("Press \'t\' or \'T\' to write using the server object\n"));
	_tprintf(_T("Press \'s\' or \'S\' to get the server status\n"));
	_tprintf(_T("Press \'m\' to activate the connection monitor\n"));
	_tprintf(_T("Press \'n\' to deactivate the connection monitor\n"));
	_tprintf(_T("Press \'l\' or \'L\' to logon for secure communication\n"));
	_tprintf(_T("Press \'o\' or \'O\' to logoff for secure communication\n"));
	_tprintf(_T("\nPress \'&\' to remove random items\n"));
	_tprintf(_T("\nPress \'(\' to re-add random items\n"));
	_tprintf(_T("\nPress \'1\' to start the threads\n"));
	_tprintf(_T("Press \'0\' to stop the threads\n\t - threads are %s -\n"), g_threadsStarted == true ? _T("started") : _T("stopped"));
	_tprintf(_T("\nPress \'2\' to allow subscription disconnection/connection\n"));
	_tprintf(_T("Press \'3\' to disallow subscription disconnection/connection\n\t - subscription disconnection/connection is %s -\n"), g_allowsubscriptionDisconnection == true ? _T("allowed") : _T("disallowed"));
	_tprintf(_T("\nPress \'7\' to allow item removal/addition\n"));
	_tprintf(_T("Press \'9\' to disallow item removal/addition\n\t - item removal/addition is %s -\n"), g_allowItemRemoval == true ? _T("allowed") : _T("disallowed"));
	_tprintf(_T("\nPress \'+\' to increase the thread count (max 50 threads)\n"));
	_tprintf(_T("Press \'-\' to decrease the thread count (min 1 thread)\n\t - number of threads is %d -\n"), g_initialThreadCount);
	_tprintf(_T("\nPress \'\\\' to manually specify the update rate\n"));
	_tprintf(_T("Press \'[\' to decrease the update rate\n"));
	_tprintf(_T("Press \']\' to increase the update rate\n\t - current update rate is %d -\n\t   (starting threads will modify it randomly on a per-subscription basis between %d and %d)\n"), g_initialUpdateRate, g_minSubscriptionUpdateRate, g_maxSubscriptionUpdateRate);
	_tprintf(_T("\nPress \'5\' to manually specify the thread operation interval (0 switches to dynamic)\n"));
	_tprintf(_T("Press \'4\' to decrease the thread operation interval\n"));
	_tprintf(_T("Press \'6\' to increase the thread operation interval\n"));
	if (g_dynamicOperationInterval)
	{
		_tprintf(_T("\t - current thread operation interval is dynamic: %dms(%.2f op / sec) ... %dms(%.2f op / sec) - \n"),
			g_minSubscriptionUpdateRate, ((float)1000) / g_minSubscriptionUpdateRate,
			g_maxSubscriptionUpdateRate, ((float)1000) / g_maxSubscriptionUpdateRate);
	}
	else
	{
		_tprintf(_T("\t - current thread operation interval is hardcoded: %dms(%.2f op / sec) - \n"), g_hardcodedOperationInterval, ((float)1000)/g_hardcodedOperationInterval);
	}
	_tprintf(_T("\nPress \'/\' to disable output\n"));
	_tprintf(_T("Press \'*\' to enable output\n\t - output is %s -\n"), g_tracesOn == true ? _T("enabled") : _T("disabled"));
	_tprintf(_T("\nPress \'?\' or \'u\' to display this usage information\n"));
	_tprintf(_T("Press \'e\' or \'q\' or Ctrl-C to exit\n"));
	g_syncDisplay.aUnlock();
}

DWORD NewLineThreadEntryPoint(LPVOID param)
{
	DWORD currentWaitTime = 0;
	while (true)
	{
		if (g_subscriptionsActive == true)
		{
			if (currentWaitTime >= g_initialUpdateRate)
			{
				currentWaitTime = 0;
				if (g_syncDisplay.aLock(0))
				{
					char buffer[256];
					_stprintf_s(buffer, _T("-------- s:%d, t:%d, g:%d b:%d (tl: %d, lg:%d, lb:%d) --------"), g_dataChangeSetsReceived, g_totalGoodDataChanges + g_totalBadDataChanges,
						g_totalGoodDataChanges, g_totalBadDataChanges, g_legitGoodDataChanges + g_legitBadDataChanges, g_legitGoodDataChanges, g_legitBadDataChanges);
					_tprintf(_T("\n%s\n\n"), buffer);
					getApplication()->trace(EnumTraceLevel_INF, EnumTraceGroup_USER1, "NewLineThreadEntryPoint", buffer);
					g_dataChangeSetsReceived = 0;
					g_totalGoodDataChanges = 0;
					g_totalBadDataChanges = 0;
					g_legitGoodDataChanges= 0;
					g_legitBadDataChanges= 0;
					g_syncDisplay.aUnlock();
				}
			}
			else
			{
				++currentWaitTime;
				::Sleep(1);
			}
		}
		else
		{
			::Sleep(500);
		}
		if (WaitForSingleObject(g_threadsStopEvent, 0) == WAIT_OBJECT_0)
		{
			break;
		}
	}
	return 0;
}

DWORD WorkerThreadEntryPoint(LPVOID param)
{
	unsigned int i;
	int threadNo = g_initialThreadCount;
	g_syncDisplay.aLock();
	{
		i = time(NULL) * GetTickCount64();
		srand(i);
		i = GetTickCount64() * GetCurrentThreadId() * rand() / RAND_MAX;
		srand(i);
		_tprintf(_T("\tThread #%-2d #%-6d (0x%016X) seed: %u\n"), threadNo, GetCurrentThreadId(), GetCurrentThreadId(), i);
		g_syncDisplay.aUnlock();
	}
	DWORD currentWaitTime = 0;
	OpcClient* pClient = getOpcClient();
	bool done = false;
	bool locked = false;
	int random = 0;
	DWORD operationInterval = g_hardcodedOperationInterval;

	while (true)
	{
		if (g_threadsStarted == true)
		{
			if (currentWaitTime >= operationInterval)
			{
				if (g_dynamicOperationInterval)
				{
					operationInterval = g_minOperationInterval + (g_maxOperationInterval - g_minOperationInterval) * rand() / RAND_MAX;
				}
				else
				{
					operationInterval = g_hardcodedOperationInterval;
				}

				currentWaitTime = 0;
				done = false;

				locked = g_syncDisplay.aLock(0);

				if (locked)
				{
					_tprintf(_T("\n%d:"), threadNo);
				}

				while (!done)
				{
					switch (rand() % 20)
					{
					case 1:
						pClient->readAsyncSubscription();
						done = true;
						break;
					case 2:
						pClient->readSyncItem();
						done = true;
						break;
					case 3:
						pClient->readSyncSubscription();
						done = true;
						break;
					case 4:
						pClient->refreshSubscription();
						done = true;
						break;
					case 5:
						//pClient->readAsyncSession();
						//done = true;
						break;
					case 6:
						//pClient->readSyncSession();
						//done = true;
						break;
					case 7:
						pClient->writeAsyncSubscription();
						done = true;
						break;
					case 8:
						pClient->writeSyncSubscription();
						done = true;
						break;
					case 9:
						//pClient->writeAsyncSession();
						//done = true;
						break;
					case 10:
						//pClient->writeSyncSession();
						//done = true;
						break;
					case 11:
						pClient->getAsyncServerStatus();
						done = true;
						break;
					case 12:
						if (g_allowItemRemoval)
						{
							pClient->removeRandomItems();
							done = true;
						}
						break;
					case 13:
						if (g_allowItemRemoval)
						{
							pClient->addRandomItems();
							done = true;
						}
						break;
					case 14:
						if (g_allowsubscriptionDisconnection)
						{
							if (rand() % 2)
							{
								pClient->disconnectSyncSubscription();
							}
							else
							{
								pClient->disconnectAsyncSubscription();
							}
							done = true;
						}
						break;
					case 15:
						if (g_allowsubscriptionDisconnection)
						{
							if (rand() % 2)
							{
								pClient->connectSyncSubscription();
							}
							else
							{
								pClient->connectAsyncSubscription();
							}
							done = true;
						}
						break;
					case 16:
						if (g_allowsubscriptionDisconnection)
						{
							if (rand() % 2)
							{
								pClient->activateSyncSubscription();
							}
							else
							{
								pClient->activateAsyncSubscription();
							}
							done = true;
						}
						break;
					default:
						if (g_allowItemRemoval)
						{
							pClient->addRandomItems();
							done = true;
						}
						break;
					}

					if (locked)
					{
						g_syncDisplay.aUnlock();
					}
				}
			}
			else
			{
				++currentWaitTime;
				::Sleep(1);
			}
		}
		else
		{
			::Sleep(500);
		}
		if (WaitForSingleObject(g_threadsStopEvent, 0) == WAIT_OBJECT_0)
		{
			break;
		}
	}
	return 0;
}

int _tmain(int argc, _TCHAR* argv[])
{
	srand((unsigned int)time(NULL) * GetTickCount());
	long result = S_OK;
#ifdef TBC_OS_WINDOWS
	SetConsoleCtrlHandler(ControlHandler, TRUE);
#endif
	// create the OpcClient instance
	createOpcClient();
	OpcClient* pClient = getOpcClient();
#ifdef TBC_OS_WINDOWS
	unsigned long waitRet;
	g_endEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	g_threadsStopEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
#endif

	// initialize the client instance
	if (!SUCCEEDED(pClient->initialize()))
	{
		pClient->terminate();
		destroyOpcClient();
#ifdef TBC_OS_WINDOWS
		CloseHandle(g_endEvent);
		CloseHandle(g_threadsStopEvent);
#endif
		return 1;
	}

	if (g_initialThreadCountRandom)
	{
		g_initialThreadCount = g_minInitialThreadCount + DWORD(((float)(g_maxInitialThreadCount - g_minInitialThreadCount)) * rand() / RAND_MAX);
	}

	// initialize the DA client simulation
	result |= pClient->initializeDaObjects();
	usage();
#ifdef TBC_OS_WINDOWS
	BOOL end = FALSE;

	DWORD threadId;
	int n = g_initialThreadCount;
	g_initialThreadCount = 0;

	_tprintf(_T("\nPlease wait, creating threads with high entropy/seed...\n"));

	for (int i = 0; i < n; ++i)
	{
		g_threads[i] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)&WorkerThreadEntryPoint, NULL, 0, &threadId);
		::Sleep(500 * rand() / RAND_MAX);
		++g_initialThreadCount;
	}

	g_threadNewLine = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)&NewLineThreadEntryPoint, NULL, 0, &threadId);
	_tprintf(_T("\n...threads created successfully.\n\nYou may hit the keyboard with your head now %c %c\n"), 1, 2);

	while (!end)
	{
		waitRet = WaitForSingleObject(g_endEvent, 100);

		if (waitRet == WAIT_OBJECT_0)
		{
			end = TRUE;
			CloseHandle(g_endEvent);
		}
		else
		{
#endif
#ifdef TBC_OS_LINUX
	unsigned long timeout = 0;

	while (timeout < waitTime)
	{
		timespec delay;
		delay.tv_sec = 0;
		delay.tv_nsec = 100000000;  /* 100 milli sec */
		// Wait for the event be signaled
		int retCode = sem_trywait(&g_endSemaphor); // event semaphore handle

		if (!retCode)
		{
			/* Event is signaled */
			break;

		}
		else
		{
			/* check whether somebody else has the mutex */
			if (retCode == EPERM)
			{
				/* sleep for delay time */
				nanosleep(&delay, NULL);
				timeout++ ;
			}

#endif
#ifdef TBC_OS_WINDOWS

			if (_kbhit())
			{
				int input;
				input = _getch();
#endif
#ifdef TBC_OS_LINUX
			char input;

			if (1)
			{
				std::cin.get(input);
#endif

				switch (input)
				{
				case _T('A'):
					pClient->activateAsyncSubscription();
					g_subscriptionsActive = true;
					break;

				case _T('a'):
					pClient->activateSyncSubscription();
					g_subscriptionsActive = true;
					break;

				case _T('C'):
					pClient->connectAsyncSubscription();
					g_subscriptionsActive = false;
					break;

				case _T('c'):
					pClient->connectSyncSubscription();
					g_subscriptionsActive = false;
					break;

				case _T('D'):
					pClient->disconnectAsyncSubscription();
					g_subscriptionsActive = false;
					break;

				case _T('d'):
					pClient->disconnectSyncSubscription();
					g_subscriptionsActive = false;
					break;

				case _T('R'):
					pClient->readAsyncSubscription();
					break;

				case _T('r'):
					//pClient->readSyncItem();
					pClient->readSyncSubscription();
					break;

				case _T('F'):
				case _T('f'):
					//the refresh can be done only asynchronously!
					pClient->refreshSubscription();
					break;

				case _T('Z'):
					pClient->readAsyncSession();
					break;

				case _T('z'):
					pClient->readSyncSession();
					break;

				case _T('W'):
					pClient->writeAsyncSubscription();
					break;

				case _T('w'):
					pClient->writeSyncSubscription();
					break;

				case _T('T'):
					pClient->writeAsyncSession();
					break;

				case _T('t'):
					pClient->writeSyncSession();
					break;

				case _T('S'):
					pClient->getAsyncServerStatus();
					break;

				case _T('s'):
					pClient->getSyncServerStatus();
					break;

				case _T('M'):
				case _T('m'):
					pClient->activateConnectionMonitor();
					break;

				case _T('N'):
				case _T('n'):
					pClient->deactivateConnectionMonitor();
					break;

				case _T('l'):
					pClient->setCredentials();
					pClient->logOnSyncForSecureCommunication();
					break;
				case _T('.'):
				case _T('>'):
					pClient->connectSession();
					g_subscriptionsActive = false;
					break;
				case _T(','):
				case _T('<'):
					pClient->disconnectSession();
					g_subscriptionsActive = false;
					break;

				/*
				case _T('L'):
					pClient->setCredentials();
					pClient->logOnAsyncForSecureCommunication();
					break;

				case _T('o'):
					pClient->setCredentials(_T("Guest"), _T("nopass"));
					pClient->logOffSyncForSecureCommunication();
					break;

				case _T('O'):
					pClient->logOffAsyncForSecureCommunication();
					pClient->setCredentials(_T("Guest"), _T("nopass"));
					break;
				*/

				case _T('?'):
				case _T('U'):
				case _T('u'):
					usage();
					break;

				case _T('+'):
					g_syncDisplay.aLock();
					if (g_initialThreadCount < 50)
					{
						g_threads[g_initialThreadCount] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)&WorkerThreadEntryPoint, NULL, 0, &threadId);
						++g_initialThreadCount;
						_tprintf(_T("\nNumber of threads increased to: %d\n"), g_initialThreadCount);
					}
					else
					{
						_tprintf(_T("\nWARNING: Number of threads is already %d, cannot increase anymore!!!\n"), g_initialThreadCount);
					}
					g_syncDisplay.aUnlock();
					break;
				case _T(']'):
					g_syncDisplay.aLock();
					if (g_initialUpdateRate < 0xfffffff5)
					{
						g_initialUpdateRate += 10;
						pClient->modifyUpdateRate(g_initialUpdateRate);
						_tprintf(_T("\nUpdate rate increased to: %dms\n"), g_initialUpdateRate);
					}
					else
					{
						_tprintf(_T("\nWARNING: Update rate is already %dms, cannot increase anymore!!!\n"), g_initialUpdateRate);
					}
					g_syncDisplay.aUnlock();
					break;
				case _T('['):
					g_syncDisplay.aLock();
					if (g_initialUpdateRate > 11)
					{
						g_initialUpdateRate -= 10;
						pClient->modifyUpdateRate(g_initialUpdateRate);
						_tprintf(_T("\nUpdate rate decreased to: %dms\n"), g_initialUpdateRate);
					}
					else
					{
						_tprintf(_T("\nWARNING: Update rate is already %dms, cannot decrease anymore!!!\n"), g_initialUpdateRate);
					}
					g_syncDisplay.aUnlock();
					break;
				case _T('5'):
					{
						g_syncDisplay.aLock();
						char intStr[256];
						long long intDw = 0;
						_tprintf(_T("\nNew thread operation interval: [0 ... 4294967295; 0 = dynamic): "));
						scanf(_T("%s"), intStr);
						intDw = atoi(intStr);
						if (intDw > 0)
						{
							if (intDw < MAXDWORD)
							{
								g_dynamicOperationInterval = false;
								g_hardcodedOperationInterval = (DWORD)intDw;
								_tprintf(_T("\nNew hardcoded thread operation interval (%dms, %.2f op/sec) successfuly set.\n"), g_hardcodedOperationInterval, ((float)1000)/g_hardcodedOperationInterval);
							}
							else
							{
								_tprintf(_T("\nWARNING: Provided thread operation interval (%d) is higher than 4294967295!!!\n"), intDw);
							}
						}
						else
						{
							if (g_dynamicOperationInterval)
							{
								_tprintf(_T("\nWARNING: Thread operation interval is ALREADY set to dynamic [%d ... %d].\n"), g_minOperationInterval, g_maxOperationInterval);
							}
							
							_tprintf(_T("\nWARNING: Thread operation interval successfully set to dynamic [%d ... %d].\n"), g_minOperationInterval, g_maxOperationInterval);
						}
						g_syncDisplay.aUnlock();
						break;
					}
				case _T('4'):
					g_syncDisplay.aLock();
					if (g_hardcodedOperationInterval < 0xfffffff5)
					{
						g_hardcodedOperationInterval += 10;
						_tprintf(_T("\nThread operation interval increased to: %dms (%.2f op/sec)\n"), g_hardcodedOperationInterval, ((float)1000)/g_hardcodedOperationInterval);
					}
					else
					{
						_tprintf(_T("\nWARNING: Thread operation interval is already %dms (%.2f op/sec), cannot increase anymore!!!\n"), g_hardcodedOperationInterval, ((float)1000)/g_hardcodedOperationInterval);
					}
					g_syncDisplay.aUnlock();
					break;
				case _T('6'):
					g_syncDisplay.aLock();
					if (g_hardcodedOperationInterval > 10)
					{
						g_hardcodedOperationInterval -= 10;
						_tprintf(_T("\nThread operation interval decreased to: %dms (%.2f op/sec)\n"), g_hardcodedOperationInterval, ((float)g_hardcodedOperationInterval)/1000);
					}
					else
					{
						_tprintf(_T("\nWARNING: Thread operation interval is already %dms (%.2f op/sec), cannot decrease anymore!!!\n"), g_hardcodedOperationInterval, ((float)1000)/g_hardcodedOperationInterval);
					}
					g_syncDisplay.aUnlock();
					break;
				case _T('\\'):
					{
						g_syncDisplay.aLock();
						char intStr[256];
						long long intDw = 0;
						_tprintf(_T("\nNew update rate: [1 ... 4294967295): "));
						scanf(_T("%s"), intStr);
						intDw = atoi(intStr);
						if (intDw > 0)
						{
							if (intDw < MAXDWORD)
							{
								g_initialUpdateRate = (DWORD)intDw;
								pClient->modifyUpdateRate();
								_tprintf(_T("\nNew update rate (%dms) successfuly set.\n"), g_initialUpdateRate);
							}
							else
							{
								_tprintf(_T("\nWARNING: Provided update rate (%d) is higher than 4294967295!!!\n"), intDw);
							}
						}
						else
						{
							_tprintf(_T("\nWARNING: Provided update rate (%d) is lower than 1!!!\n"), intDw);
						}
						g_syncDisplay.aUnlock();
						break;
					}
				case _T('-'):
					g_syncDisplay.aLock();
					if (g_initialThreadCount > 1)
					{
						--g_initialThreadCount;
						TerminateThread(g_threads[g_initialThreadCount], 12345678);
						CloseHandle(g_threads[g_initialThreadCount]);
						g_threads[g_initialThreadCount] = 0;
						_tprintf(_T("\nNumber of threads decreased to: %d\n"), g_initialThreadCount);
					}
					else
					{
						_tprintf(_T("\nWARNING: Number of threads is already %d, cannot decrease anymore!!!\n"), g_initialThreadCount);
					}
					g_syncDisplay.aUnlock();
					break;
				case _T('1'):
					if (g_threadsStarted == false)
					{
						g_threadsStarted = true;
						g_syncDisplay.aLock();
						_tprintf(_T("\nThreads started, R/W operations in progress\n"));
						g_syncDisplay.aUnlock();
					}
					else
					{
						g_syncDisplay.aLock();
						_tprintf(_T("\nWARNING: Threads already started, cannot start again!!!\n"));
						g_syncDisplay.aUnlock();
					}
					break;
				case _T('0'):
					if (g_threadsStarted == true)
					{
						g_threadsStarted = false;
						g_syncDisplay.aLock();
						_tprintf(_T("\nThreads stopped, R/W operations suspended\n"));
						g_syncDisplay.aUnlock();
					}
					else
					{
						g_syncDisplay.aLock();
						_tprintf(_T("\nWARNING: Threads already stopped, cannot stop again!!!\n"));
						g_syncDisplay.aUnlock();
					}
					g_syncDisplay.aUnlock();
					break;
				case _T('7'):
					g_syncDisplay.aLock();
					if (g_allowItemRemoval == false)
					{
						g_allowItemRemoval = true;
						_tprintf(_T("\nItem removal/addition is now allowed.\n"));
					}
					else
					{
						_tprintf(_T("\nWARNING: Item removal/addition already allowed, cannot allow again!!!\n"));
					}
					g_syncDisplay.aUnlock();
					break;
				case _T('9'):
					g_syncDisplay.aLock();
					if (g_allowItemRemoval == true)
					{
						g_allowItemRemoval = false;
						_tprintf(_T("\nItem removal/addition is now DISallowed\n"));
					}
					else
					{
						_tprintf(_T("\nWARNING: Item removal/addition already DISallowed, cannot DISallow again!!!\n"));
					}
					g_syncDisplay.aUnlock();
					break;
				case _T('2'):
					g_syncDisplay.aLock();
					if (g_allowsubscriptionDisconnection == false)
					{
						g_allowsubscriptionDisconnection = true;
						_tprintf(_T("\nSubscription disconnection/connectionis now allowed.\n"));
					}
					else
					{
						_tprintf(_T("\nWARNING: Subscription disconnection/connection already allowed, cannot allow again!!!\n"));
					}
					g_syncDisplay.aUnlock();
					break;
				case _T('3'):
					g_syncDisplay.aLock();
					if (g_allowsubscriptionDisconnection == true)
					{
						g_allowsubscriptionDisconnection = false;
						_tprintf(_T("\nSubscription disconnection/connection is now DISallowed\n"));
					}
					else
					{
						_tprintf(_T("\nWARNING: Subscription disconnection/connection already DISallowed, cannot DISallow again!!!\n"));
					}
					g_syncDisplay.aUnlock();
					break;
				case _T('&'):
					pClient->removeRandomItems();
					break;
				case _T('('):
					pClient->addRandomItems();
					break;
				case _T('/'):
					g_syncDisplay.aLock();
					if (g_tracesOn == true)
					{
						g_tracesOn = false;
						_tprintf(_T("\nTraces disabled\n"));
					}
					else
					{
						_tprintf(_T("\nWARNING: Traces already disabled, cannot mute again!!!\n"));
					}
					g_syncDisplay.aUnlock();
					break;
				case _T('*'):
					g_syncDisplay.aLock();
					if (g_tracesOn == false)
					{
						g_tracesOn = true;
						_tprintf(_T("\nTraces enabled\n"));
					}
					else
					{
						_tprintf(_T("\nWARNING: Traces already enabled, cannot unmute again!!!\n"));
					}
					g_syncDisplay.aUnlock();
					break;
				case _T('E'):
				case _T('e'):
				case _T('Q'):
				case _T('q'):
#ifdef TBC_OS_WINDOWS
					end = TRUE;
					CloseHandle(g_endEvent);
#endif
#ifdef TBC_OS_LINUX
					sem_post(&g_endSemaphor);
#endif
					break;
				} // end switch
			} // end if
		} // end else
	} // end while

	if (g_threadsStarted == true)
	{
		g_threadsStarted = false;
	}
	SetEvent(g_threadsStopEvent);
	if (g_threads[0] != 0)
	{
		WaitForMultipleObjects(g_initialThreadCount, g_threads, TRUE, INFINITE);
		for (int i = 0; i < g_initialThreadCount; ++i)
		{
			CloseHandle(g_threads[i]);
		}
	}

	WaitForSingleObject(g_threadNewLine, INFINITE);

	CloseHandle(g_threadsStopEvent);

	pClient->terminate();
	destroyOpcClient();
	return 0;
} // end main
