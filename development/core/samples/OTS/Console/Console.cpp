#if defined(WIN32) || defined(WIN64)
#include <Windows.h>
#include <TChar.h>
#else
#include <unistd.h>
#endif
#include "OTServer.h"
#include <stdio.h>
#include <memory.h>
#include <math.h>

#ifndef _T
#define _T(a)          a
#define _tcsncmp    strncmp
#define _tcscmp     strcmp
#define _tcslen     strlen
#define _tcscpy     strcpy
#define _stprintf   sprintf
#endif

#if defined(WIN32) || defined(WIN64)
#define OUTPUT _tprintf
#else
#define OUTPUT printf
#endif

// callback functions
void OTAPI_CALL OnTrace(IN OTChar* traceString, IN unsigned short level, IN unsigned long mask, IN OTChar* objId, IN OTChar* text);
long OTSAPI_CALL Shutdown(void);
long OTSAPI_CALL HandleReadRequests(IN long count, IN OTSRequestData* paRequests);
long OTSAPI_CALL HandleWriteRequests(IN long count, IN OTSRequestData* paRequests, IN OTSValueData* pValues);
long OTSAPI_CALL QueryProperties(IN OTSObjectData* pObjectData, IN OTChar* objItemId, IN long propID, OUT unsigned long* pPropCount, OUT OTSPropertyData** ppPropData);
#if defined(WIN32) || defined(WIN64)
long OTSAPI_CALL QueryConditions(IN OTSObjectData* pObjectData, IN OTChar* sourcePath, OUT unsigned long* pConditionCount, OUT OTChar** *pConditionNames);
long OTSAPI_CALL AcknowledgeCondition(IN OTSObjectData conditionData, IN OTChar* ackId, OTChar* ackComment);
long OTSAPI_CALL QueryConditionDefinition(IN OTSObjectData conditionData, OUT OTSConditionDefinitionData* pConditionDefData);
long OTSAPI_CALL WebHandleTemplate(IN OTChar* templateName, IN unsigned long numArgs, IN OTChar** pArgs, OUT OTChar** pResult);
#endif

// local used functions
void BuildAddressSpace(void);
void BuildEventCategories(void);
void SetString(OTChar** pStrD, OTChar* strS);

// global data
OTSInitData g_initData =
{
	419,                                                  // used OTS version
	OTS_APPTYPE_OUTPROC,                                  // application type of OPC server
	NULL,                                                 // name of service; if no service NULL
	_T("{D6E8EB13-C29F-4210-A739-5321A0D511F4}"),         // class id of DA server
	_T("Softing.OPCToolboxCSmpl.Console.DA"),             // description of OPC DA Server
	_T("Softing.OPCToolboxCSmpl.Console.DA.1"),           // version independent ProgId of OPC DA Server
	_T("{6F8B44DB-B4AB-42e4-910A-27259D825C55}"),         // class id of AE server
	_T("Softing.OPCToolboxCSmpl.Console.AE"),             // description of OPC AE Server
	_T("Softing.OPCToolboxCSmpl.Console.AE.1"),           // version independent ProgId of OPC AE Server
	_T("Softing OPC Toolkit C Console Server Sample"),    // server description
	8081,                                                 // IP port number of HTTP server
	_T("/OPC/DA"),                                        // URL of XML-DA server
	1, 1, 1,                                              // major version number, minor version number and build numberof the server
	_T("Softing Industrial Automation GmbH"),             // version info of the server
	_T('/'),                                              // delimiter sign for AddressSpace levels in the item id
	30000,                                                // period in ms to check the notification connections to the clients
	_T('.'),                                              // delimiter sign for DA properties in the item id
	100,                                                  // minimal update rate for a DA group (ms)
	false,                                                // support to enable or disable conditions
	_T("www"),                                            // root directory for web pages
	NULL,                                                 // root file of the web server; default: index.html
	_T("ad"),                                             // web administrator user password
	_T("op")                                              // web operator user password
};

OTTraceData g_traceSettings =
{
	OT_TRACEMASK_ALL,                      // error level trace mask
	OT_TRACEMASK_ALL,                      // warning level trace mask
	OT_TRACEMASK_USER | OT_TRACEMASK_SOPC, // info level trace mask
	OT_TRACEMASK_NOTHING,                  // error level trace mask
	false,                                 // enable trace to file
	_T("trace1.txt"),                      // first trace file
	_T("trace2.txt"),                      // second trace file
	1000000                                // maximum size of trace files
};

OTSCallbackFunctions g_callbackFunctions =
{
	&OnTrace,
	&Shutdown,
	&HandleReadRequests,
	&HandleWriteRequests,
	&QueryProperties,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
#if defined(WIN32) || defined(WIN64)
	&QueryConditions,
	&AcknowledgeCondition,
	&QueryConditionDefinition,
	NULL,
	&WebHandleTemplate
#else
	NULL,
	NULL,
	NULL,
	NULL,
	NULL
#endif
};

#if defined(WIN32) || defined(WIN64)
HANDLE g_endEvent;
#endif

#define OBJ_COUNT                9
#define OBJ_IDX_TAG_ACCEPT       0
#define OBJ_IDX_NODE_RANDOM      1
#define OBJ_IDX_TAG_RANDOM_100MS 2
#define OBJ_IDX_TAG_RANDOM_1S    3
#define OBJ_IDX_NODE_MATHS       4
#define OBJ_IDX_TAG_MATHS_SIN    5
#define OBJ_IDX_SRC_TIMER        6
#define OBJ_IDX_COND_MULTIPLE    7
#define OBJ_IDX_COND_SINGLE      8

unsigned long g_objHandles[OBJ_COUNT];

#define CAT_ID_TIME_TICK         1
#define CAT_ID_TIME_SLOT         2
#define COND_NAME_BETWEEN_SINGLE    _T("between [10,50]")
#define COND_NAME_BETWEEN_MULTIPLE  _T("between")

unsigned short g_angle;

#if defined(WIN32) || defined(WIN64)

//-----------------------------------------------------------------------------
// controlHandler
//
// - handle console control events
//
// returns:
//		true  - handled event
//		false - event not handled
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

		return true;
	}
	break;
	}

	return false;
} // controlHandler

#endif

#if defined(WIN32) || defined(WIN64)
int _tmain(int argc, TCHAR* argv[])
#else
int main(int argc, char* argv[])
#endif
{
#ifdef OTB_USE_STATIC_LIB
	OTInitLib();
#endif
#if defined(WIN32) || defined(WIN64)
	g_endEvent = CreateEvent(NULL, false, false, NULL);
	SetConsoleCtrlHandler(controlHandler, true);
#endif
	OTActivate(OTS_FEATURE_DA_SERVER, _T("SAMPLE"));
	OTActivate(OTS_FEATURE_XMLDA_SERVER, _T("SAMPLE"));
	OTActivate(OTS_FEATURE_AE_SERVER, _T("SAMPLE"));
	OTSAdvise(&g_callbackFunctions);
	OTEnableTracing(OT_TRACEDATA_ALL, &g_traceSettings);
	OTTrace(OT_TRACELEVEL_INF, OT_TRACEMASK_USER1, _T(""), _T("Start"));

	if (SUCCEEDED(OTSInitialize(&g_initData)))
	{
#if defined(WIN32) || defined(WIN64)

		if (S_OK == OTSProcessCommandLine((OTChar*)GetCommandLine()))
#endif
		{
			if (SUCCEEDED(OTSStart()))
			{
				BuildEventCategories();
				BuildAddressSpace();
				g_objHandles[OBJ_IDX_COND_MULTIPLE] = 0;
				g_objHandles[OBJ_IDX_COND_SINGLE] = 0;
				OTSReady();
				OUTPUT(_T("Press Ctrl-C to exit\n"));
				OTBool end = false;
				unsigned long loopCnt = 0;
				OTSValueData value[3];
				OTVariant var[3];
				unsigned long objectH[3];
				long i;
				double radianAngle;
				static double pi = 3.1415926535;
#if defined(WIN32) || defined(WIN64)
				OTBool firstLoop = true;
				unsigned long waitTime = 0;
				unsigned long waitRet;
				SYSTEMTIME now;
				unsigned long eventCount;
				OTSEventData events[3];
				OTSVariant time[3];
				unsigned short second = 0xFFFF;
				unsigned short minute = 0xFFFF;
				unsigned short hour = 0xFFFF;
				unsigned long condCount;
				OTSConditionData condData[2];
				memset(condData, 0, sizeof(OTSConditionData) * 2);
				memset(events, 0, sizeof(OTSEventData) * 3);
#endif // WIN32
				objectH[0] = g_objHandles[OBJ_IDX_TAG_RANDOM_100MS];
				objectH[1] = g_objHandles[OBJ_IDX_TAG_RANDOM_1S];
				objectH[2] = g_objHandles[OBJ_IDX_TAG_MATHS_SIN];

				for (i = 0; i < 3; i++)
				{
					OTVariantInit(&var[i]);
					value[i].m_pValue = &var[i];
				}

#if defined(WIN32) || defined(WIN64)
				time[0].vt = VT_UI2;
				time[0].uiVal = 0;
				time[1].vt = VT_UI2;
				time[1].uiVal = 0;
				time[2].vt = VT_UI2;
				time[2].uiVal = 0;
				condData[0].m_eventCategory = CAT_ID_TIME_SLOT;
				condData[0].m_name = COND_NAME_BETWEEN_MULTIPLE;
				condData[0].m_sourcePath = _T("computer/clock/timer");
				condData[0].m_quality = OT_QUALITY_GOOD;
				condData[0].m_eventAttrCount = 1;
				condData[0].m_pEventAttrs = &time[2];
				condData[0].m_userData = OBJ_IDX_COND_MULTIPLE;
				condData[1].m_eventCategory = CAT_ID_TIME_SLOT;
				condData[1].m_name = COND_NAME_BETWEEN_SINGLE;
				condData[1].m_sourcePath = _T("computer/clock/timer");
				condData[1].m_quality = OT_QUALITY_GOOD;
				condData[1].m_eventAttrCount = 1;
				condData[1].m_pEventAttrs = &time[2];
				condData[1].m_userData = OBJ_IDX_COND_SINGLE;
#endif // WIN32

				while (!end)
				{
#if defined(WIN32) || defined(WIN64)
					waitRet = WaitForSingleObject(g_endEvent, waitTime);
					waitTime = 100;

					if (waitRet == WAIT_OBJECT_0)
					{
						end = true;
					}
					else
#else
					usleep(100000);

#endif
					{
						loopCnt++;
						// data change simulation
						value[0].m_pValue->vt = VT_I4;
						value[0].m_pValue->lVal = rand();
						value[0].m_quality = OT_QUALITY_GOOD;
#if defined(WIN32) || defined(WIN64)
						value[0].m_timestamp.dwHighDateTime = 0;
						value[0].m_timestamp.dwLowDateTime = 0;
#else
						value[0].m_timestamp.tv_sec = 0;
						value[0].m_timestamp.tv_usec = 0;
#endif

						if (loopCnt >= 10)
						{
							value[1].m_pValue->vt = VT_I4;
							value[1].m_pValue->lVal = rand();
							value[1].m_quality = OT_QUALITY_GOOD;
#if defined(WIN32) || defined(WIN64)
							value[1].m_timestamp.dwHighDateTime = 0;
							value[1].m_timestamp.dwLowDateTime = 0;
#else
							value[1].m_timestamp.tv_sec = 0;
							value[1].m_timestamp.tv_usec = 0;
#endif

							if (g_angle < 359)
							{
								g_angle++;
							}
							else
							{
								g_angle = 0;
							}

							radianAngle = (2 * pi) * ((double)g_angle / 360.0);
							value[2].m_pValue->vt = VT_R8;
							value[2].m_pValue->dblVal = sin(radianAngle);
							value[2].m_quality = OT_QUALITY_GOOD;
#if defined(WIN32) || defined(WIN64)
							value[2].m_timestamp.dwHighDateTime = 0;
							value[2].m_timestamp.dwLowDateTime = 0;
#else
							value[2].m_timestamp.tv_sec = 0;
							value[2].m_timestamp.tv_usec = 0;
#endif
							OTSValuesChanged(3, objectH, value);
							loopCnt = 0;
						}
						else
						{
							OTSValuesChanged(1, objectH, value);
						}

#if defined(WIN32) || defined(WIN64)
						// alarm simulation
						GetLocalTime(&now);
						time[0].uiVal = now.wHour;
						time[1].uiVal = now.wMinute;
						time[2].uiVal = now.wSecond;
						// conditions
						condCount = 0;

						if (!firstLoop)
						{
							if ((second != now.wSecond) && (now.wSecond == 10))
							{
								condData[0].m_severity = 500;
								condData[0].m_message = _T("between [10,30[");
								condData[0].m_ackRequired = true;
								condData[0].m_activeSubConditionName =  _T("[10,30[");
								condData[0].m_changeMask = OT_CONDITIONCHANGE_ACTIVE_STATE | OT_CONDITIONCHANGE_ACK_STATE | OT_CONDITIONCHANGE_SEVERITY | OT_CONDITIONCHANGE_MESSAGE | OT_CONDITIONCHANGE_SUBCONDITION | OT_CONDITIONCHANGE_ATTRIBUTE;
								condData[0].m_stateChange = OT_CONDITIONSTATE_ACTIVE; // active and not acked
								condData[1].m_severity = 510;
								condData[1].m_message = _T("between [10,50]");
								condData[1].m_ackRequired = true;
								condData[1].m_changeMask = OT_CONDITIONCHANGE_ACTIVE_STATE | OT_CONDITIONCHANGE_ACK_STATE | OT_CONDITIONCHANGE_SEVERITY | OT_CONDITIONCHANGE_MESSAGE | OT_CONDITIONCHANGE_ATTRIBUTE;
								condData[1].m_stateChange = OT_CONDITIONSTATE_ACTIVE; // active and not acked
								condCount = 2;
							}

							if ((second != now.wSecond) && (now.wSecond == 30))
							{
								condData[0].m_severity = 540;
								condData[0].m_message = _T("between [30,50]");
								condData[0].m_ackRequired = true;
								condData[0].m_activeSubConditionName =  _T("[30,50]");
								condData[0].m_changeMask = OT_CONDITIONCHANGE_ACTIVE_STATE | OT_CONDITIONCHANGE_ACK_STATE | OT_CONDITIONCHANGE_SEVERITY | OT_CONDITIONCHANGE_MESSAGE | OT_CONDITIONCHANGE_SUBCONDITION | OT_CONDITIONCHANGE_ATTRIBUTE;
								condData[0].m_stateChange = OT_CONDITIONSTATE_ACTIVE; // active and not acked
								condCount = 1;
							}

							if ((second != now.wSecond) && (now.wSecond == 50))
							{
								condData[0].m_severity = 1;
								condData[0].m_message = _T("not between [10,50]");
								condData[0].m_ackRequired = false;
								condData[0].m_activeSubConditionName =  _T("");
								condData[0].m_changeMask = OT_CONDITIONCHANGE_ACTIVE_STATE | OT_CONDITIONCHANGE_SEVERITY | OT_CONDITIONCHANGE_MESSAGE | OT_CONDITIONCHANGE_SUBCONDITION | OT_CONDITIONCHANGE_ATTRIBUTE;
								condData[0].m_stateChange = 0; // not active
								condData[1].m_severity = 1;
								condData[1].m_message = _T("not between [10,50]");
								condData[1].m_ackRequired = false;
								condData[1].m_changeMask = OT_CONDITIONCHANGE_ACTIVE_STATE | OT_CONDITIONCHANGE_SEVERITY | OT_CONDITIONCHANGE_MESSAGE | OT_CONDITIONCHANGE_ATTRIBUTE;
								condData[1].m_stateChange = 0; // not active
								condCount = 2;
							}
						}
						else
						{
							// firstLoop
							firstLoop = false;

							if ((now.wSecond >= 10) && (now.wSecond < 30))
							{
								condData[0].m_severity = 540;
								condData[0].m_message = _T("between [30,50]");
								condData[0].m_ackRequired = true;
								condData[0].m_activeSubConditionName =  _T("[30,50]");
								condData[0].m_changeMask = OT_CONDITIONCHANGE_ACTIVE_STATE | OT_CONDITIONCHANGE_ACK_STATE | OT_CONDITIONCHANGE_SEVERITY | OT_CONDITIONCHANGE_MESSAGE | OT_CONDITIONCHANGE_SUBCONDITION | OT_CONDITIONCHANGE_ATTRIBUTE;
								condData[0].m_stateChange = OT_CONDITIONSTATE_ACTIVE; // active and not acked
								condData[1].m_severity = 510;
								condData[1].m_message = _T("between [10,50]");
								condData[1].m_ackRequired = true;
								condData[1].m_changeMask = OT_CONDITIONCHANGE_ACTIVE_STATE | OT_CONDITIONCHANGE_ACK_STATE | OT_CONDITIONCHANGE_SEVERITY | OT_CONDITIONCHANGE_MESSAGE | OT_CONDITIONCHANGE_ATTRIBUTE;
								condData[1].m_stateChange = OT_CONDITIONSTATE_ACTIVE; // active and not acked
								condCount = 2;
							}

							if ((now.wSecond >= 30) && (now.wSecond < 50))
							{
								condData[0].m_severity = 540;
								condData[0].m_message = _T("between [30,50]");
								condData[0].m_ackRequired = true;
								condData[0].m_activeSubConditionName =  _T("[30,50]");
								condData[0].m_changeMask = OT_CONDITIONCHANGE_ACTIVE_STATE | OT_CONDITIONCHANGE_ACK_STATE | OT_CONDITIONCHANGE_SEVERITY | OT_CONDITIONCHANGE_MESSAGE | OT_CONDITIONCHANGE_SUBCONDITION | OT_CONDITIONCHANGE_ATTRIBUTE;
								condData[0].m_stateChange = OT_CONDITIONSTATE_ACTIVE; // active and not acked
								condData[1].m_severity = 510;
								condData[1].m_message = _T("between [10,50]");
								condData[1].m_ackRequired = true;
								condData[1].m_changeMask = OT_CONDITIONCHANGE_ACTIVE_STATE | OT_CONDITIONCHANGE_ACK_STATE | OT_CONDITIONCHANGE_SEVERITY | OT_CONDITIONCHANGE_MESSAGE | OT_CONDITIONCHANGE_ATTRIBUTE;
								condData[1].m_stateChange = OT_CONDITIONSTATE_ACTIVE; // active and not acked
								condCount = 2;
							}
						}

						if (condCount > 0)
						{
							OTSConditionsChanged(condCount, &g_objHandles[OBJ_IDX_COND_MULTIPLE], condData, &g_objHandles[OBJ_IDX_COND_MULTIPLE]);
						}

						// simple events
						eventCount = 0;

						if (second != now.wSecond)
						{
							second = now.wSecond;
							events[eventCount].m_eventType = OT_EVENTTYPE_SIMPLE;
							events[eventCount].m_eventCategory = CAT_ID_TIME_TICK;
							events[eventCount].m_severity = 10;
							events[eventCount].m_sourcePath = _T("computer/clock/timer");
							events[eventCount].m_message = _T("Second changed");
							memset(&events[eventCount].m_occurenceTime, 0, sizeof(OTSDateTime));
							events[eventCount].m_eventAttrCount = 3;
							events[eventCount].m_pEventAttrs = time;
							eventCount++;
						}

						if (minute != now.wMinute)
						{
							minute = now.wMinute;
							events[eventCount].m_eventType = OT_EVENTTYPE_SIMPLE;
							events[eventCount].m_eventCategory = CAT_ID_TIME_TICK;
							events[eventCount].m_severity = 490;
							events[eventCount].m_sourcePath = _T("computer/clock/timer");
							events[eventCount].m_message = _T("Minute changed");
							memset(&events[eventCount].m_occurenceTime, 0, sizeof(OTSDateTime));
							events[eventCount].m_eventAttrCount = 3;
							events[eventCount].m_pEventAttrs = time;
							eventCount++;
						}

						if (hour != now.wHour)
						{
							hour = now.wHour;
							events[eventCount].m_eventType = OT_EVENTTYPE_SIMPLE;
							events[eventCount].m_eventCategory = CAT_ID_TIME_TICK;
							events[eventCount].m_severity = 990;
							events[eventCount].m_sourcePath = _T("computer/clock/timer");
							events[eventCount].m_message = _T("Hour changed");
							memset(&events[eventCount].m_occurenceTime, 0, sizeof(OTSDateTime));
							events[eventCount].m_eventAttrCount = 3;
							events[eventCount].m_pEventAttrs = time;
							eventCount++;
						}

						OTSFireEvents(eventCount, events);
#endif // WIN32
					}
				}

				OTSStop();
			}
		}

		OTSTerminate();
	}

#if defined(WIN32) || defined(WIN64)
	CloseHandle(g_endEvent);
#endif // WIN32
#ifdef OTB_USE_STATIC_LIB
	OTExitLib();
#endif
	return 0;
}


void BuildAddressSpace(void)
{
	OTSAddressSpaceElementData data;
	data.m_elementType = OT_ADDRESSSPACEELEMENT_TYPE_DA;
	data.m_name = _T("accept");
	data.m_accessRights = OT_ACCESSRIGHTS_READABLE | OT_ACCESSRIGHTS_WRITEABLE;
	data.m_datatype = VT_UI4;
	data.m_ioMode = OTS_IOMODE_POLL;
	data.m_userData = OBJ_IDX_TAG_ACCEPT;
	data.m_hasChildren = false;
	data.m_isBrowsable = true;
	OTSAddAddressSpaceElement(0, &data, &g_objHandles[OBJ_IDX_TAG_ACCEPT]);
	data.m_elementType = OT_ADDRESSSPACEELEMENT_TYPE_DA;
	data.m_name = _T("random");
	data.m_ioMode = OTS_IOMODE_NONE;
	data.m_userData = OBJ_IDX_NODE_RANDOM;
	data.m_hasChildren = true;
	data.m_isBrowsable = true;
	OTSAddAddressSpaceElement(0, &data, &g_objHandles[OBJ_IDX_NODE_RANDOM]);
	data.m_elementType = OT_ADDRESSSPACEELEMENT_TYPE_DA;
	data.m_name = _T("100ms");
	data.m_accessRights = OT_ACCESSRIGHTS_READABLE;
	data.m_datatype = VT_I4;
	data.m_ioMode = OTS_IOMODE_REPORT;
	data.m_userData = OBJ_IDX_TAG_RANDOM_100MS;
	data.m_hasChildren = false;
	data.m_isBrowsable = true;
	OTSAddAddressSpaceElement(g_objHandles[OBJ_IDX_NODE_RANDOM], &data, &g_objHandles[OBJ_IDX_TAG_RANDOM_100MS]);
	data.m_elementType = OT_ADDRESSSPACEELEMENT_TYPE_DA;
	data.m_name = _T("1s");
	data.m_accessRights = OT_ACCESSRIGHTS_READABLE;
	data.m_datatype = VT_I4;
	data.m_ioMode = OTS_IOMODE_REPORT;
	data.m_userData = OBJ_IDX_TAG_RANDOM_1S;
	data.m_hasChildren = false;
	data.m_isBrowsable = true;
	OTSAddAddressSpaceElement(g_objHandles[OBJ_IDX_NODE_RANDOM], &data, &g_objHandles[OBJ_IDX_TAG_RANDOM_1S]);
	data.m_elementType = OT_ADDRESSSPACEELEMENT_TYPE_DA;
	data.m_name = _T("maths");
	data.m_ioMode = OTS_IOMODE_NONE;
	data.m_userData = OBJ_IDX_NODE_MATHS;
	data.m_hasChildren = true;
	data.m_isBrowsable = true;
	OTSAddAddressSpaceElement(0, &data, &g_objHandles[OBJ_IDX_NODE_MATHS]);
	data.m_elementType = OT_ADDRESSSPACEELEMENT_TYPE_DA;
	data.m_name = _T("sin");
	data.m_accessRights = OT_ACCESSRIGHTS_READABLE;
	data.m_datatype = VT_R8;
	data.m_ioMode = OTS_IOMODE_REPORT;
	data.m_userData = OBJ_IDX_TAG_MATHS_SIN;
	data.m_hasChildren = false;
	data.m_isBrowsable = true;
	OTSAddAddressSpaceElement(g_objHandles[OBJ_IDX_NODE_MATHS], &data, &g_objHandles[OBJ_IDX_TAG_MATHS_SIN]);
	OTSObjectHandle hComputer, hClock;
	data.m_elementType = OT_ADDRESSSPACEELEMENT_TYPE_AE;
	data.m_name = _T("computer");
	data.m_hasChildren = true;
	data.m_userData = 0;
	data.m_isBrowsable = true;
	OTSAddAddressSpaceElement(0, &data, &hComputer);
	data.m_elementType = OT_ADDRESSSPACEELEMENT_TYPE_AE;
	data.m_name = _T("clock");
	data.m_hasChildren = true;
	data.m_userData = 0;
	data.m_isBrowsable = true;
	OTSAddAddressSpaceElement(hComputer, &data, &hClock);
	data.m_elementType = OT_ADDRESSSPACEELEMENT_TYPE_AE;
	data.m_name = _T("timer");
	data.m_hasChildren = false;
	data.m_userData = OBJ_IDX_SRC_TIMER;
	data.m_isBrowsable = true;
	OTSAddAddressSpaceElement(hClock, &data, &g_objHandles[OBJ_IDX_SRC_TIMER]);
}

void BuildEventCategories(void)
{
	unsigned long hCategory;
	OTSAddEventCategory(CAT_ID_TIME_TICK, _T("time tick"), OT_EVENTTYPE_SIMPLE, &hCategory);
	OTSAddEventAttribute(hCategory, 1, _T("hour"), VT_UI2);
	OTSAddEventAttribute(hCategory, 2, _T("minute"), VT_UI2);
	OTSAddEventAttribute(hCategory, 3, _T("second"), VT_UI2);
	OTSAddEventCategory(CAT_ID_TIME_SLOT, _T("time slot"), OT_EVENTTYPE_CONDITION, &hCategory);
	OTSAddEventAttribute(hCategory, 1, _T("second"), VT_UI2);
	OTSAddCondition(hCategory, COND_NAME_BETWEEN_SINGLE);         // single state condition
	OTSAddCondition(hCategory, COND_NAME_BETWEEN_MULTIPLE);                 // multi state condition
	OTSAddSubCondition(hCategory, COND_NAME_BETWEEN_MULTIPLE, _T("[10,30["));
	OTSAddSubCondition(hCategory, COND_NAME_BETWEEN_MULTIPLE, _T("[30,50]"));
}

void OTAPI_CALL OnTrace(IN OTChar* traceString, IN unsigned short level, IN unsigned long mask, IN OTChar* objId, IN OTChar* text)
{
	printf(_T("%s"), traceString);
}

long OTSAPI_CALL Shutdown(void)
{
#if defined(WIN32) || defined(WIN64)
	SetEvent(g_endEvent);
#endif
	return S_OK;
}

long OTSAPI_CALL HandleReadRequests(IN long count, IN OTSRequestData* paRequests)
{
	long i;
	OTSValueData value;
	OTVariant v;
	long result = S_OK;
	OTVariantInit(&v);
	value.m_pValue = &v;

	for (i = 0; i < count; i++)
	{
		if (paRequests[i].m_propertyID == 0)
		{
			// process value
			OTSGetCacheValue(paRequests[i].m_object.m_objectHandle, &value);
			OTSCompleteRequests(1, &paRequests[i], &result, &value);
		}
		else
		{
			// property
#if defined(WIN32) || defined(WIN64)
			value.m_timestamp.dwHighDateTime = 0;
			value.m_timestamp.dwLowDateTime = 0;
#else
			value.m_timestamp.tv_sec = 0;
			value.m_timestamp.tv_usec = 0;
#endif
			value.m_quality = OT_QUALITY_GOOD;

			if (paRequests[i].m_propertyID == 101)
			{
				value.m_pValue->vt = VT_BSTR;

				switch (paRequests[i].m_object.m_userData)
				{
				case OBJ_IDX_TAG_RANDOM_100MS:
					value.m_pValue->bstrVal = OTSysAllocString(_T("Random value changing every 100 ms"));
					break;

				case OBJ_IDX_TAG_RANDOM_1S:
					value.m_pValue->bstrVal = OTSysAllocString(_T("Random value changing every second"));
					break;

				case OBJ_IDX_NODE_RANDOM:
					value.m_pValue->bstrVal = OTSysAllocString(_T("Random values"));
					break;
				}

				OTSCompleteRequests(1, &paRequests[i], &result, &value);
			}

			if (paRequests[i].m_propertyID == 6020)
			{
				value.m_pValue->vt = VT_UI2;
				value.m_pValue->iVal = g_angle;
				OTSCompleteRequests(1, &paRequests[i], &result, &value);
			}
		}

		OTVariantClear(&v);
	}

	return S_OK;
}

long OTSAPI_CALL HandleWriteRequests(IN long count, IN OTSRequestData* paRequests, IN OTSValueData* pValues)
{
	long i;
	long result;

	for (i = 0; i < count; i++)
	{
		result = S_OK;

		if (paRequests[i].m_propertyID == 0)
		{
			// process value
			OTSCompleteRequests(1, &paRequests[i], &result, NULL);

			if (SUCCEEDED(result)) // write succeeded -> update cache
			{
				OTSValuesChanged(1, &paRequests[i].m_object.m_objectHandle, &pValues[i]);
			}
		}
		else
		{
			// property
			if (paRequests[i].m_propertyID == 6020)
			{
				if (pValues[i].m_pValue->iVal < 359)
				{
					g_angle = pValues[i].m_pValue->iVal;
					double radianAngle;
					static double pi = 3.1415926535;
					OTSValueData value;
					OTVariant var;
					value.m_pValue = &var;
					radianAngle = (2 * pi) * ((double)g_angle / 360.0);
					value.m_pValue->vt = VT_R8;
					value.m_pValue->dblVal = sin(radianAngle);
					value.m_quality = OT_QUALITY_GOOD;
#if defined(WIN32) || defined(WIN64)
					value.m_timestamp.dwHighDateTime = 0;
					value.m_timestamp.dwLowDateTime = 0;
#else
					value.m_timestamp.tv_sec = 0;
					value.m_timestamp.tv_usec = 0;
#endif
					OTSValuesChanged(1, &g_objHandles[OBJ_IDX_TAG_MATHS_SIN], &value);
				}
				else
				{
					result = OT_E_OPC_RANGE;
				}

				OTSCompleteRequests(1, &paRequests[i], &result, NULL);
			}
		}
	}

	return S_OK;
}

long OTSAPI_CALL QueryProperties(
	IN OTSObjectData* pObjectData,
	IN OTChar* objItemId,
	IN long propID,
	OUT unsigned long* pPropCount,
	OUT OTSPropertyData** ppPropData)
{
	long ret = S_FALSE;
	*ppPropData = NULL;
	*pPropCount = 0;

	if (pObjectData)
	{
		switch (pObjectData->m_userData)
		{
		case OBJ_IDX_TAG_RANDOM_100MS:
		case OBJ_IDX_TAG_RANDOM_1S:
		case OBJ_IDX_NODE_RANDOM:
			if ((propID == 0) || (propID == 101))
			{
				*ppPropData = (OTSPropertyData*)OTAllocateMemory(sizeof(OTSPropertyData) * 1);
				*pPropCount = 1;
				(*ppPropData)[0].m_pid = 101;
				SetString(&((*ppPropData)[0].m_itemID), _T("itemDescription"));
				SetString(&((*ppPropData)[0].m_name), _T("description"));
				SetString(&((*ppPropData)[0].m_descr), _T("Item Description"));
				(*ppPropData)[0].m_datatype = VT_BSTR;
				(*ppPropData)[0].m_accessRights = OT_ACCESSRIGHTS_READABLE;
				ret = S_OK;
			}

			break;

		case OBJ_IDX_TAG_MATHS_SIN:
			if ((propID == 0) || (propID == 6020))
			{
				*ppPropData = (OTSPropertyData*)OTAllocateMemory(sizeof(OTSPropertyData) * 1);
				*pPropCount = 1;
				(*ppPropData)[0].m_pid = 6020;
				SetString(&((*ppPropData)[0].m_itemID), _T("angle"));
				SetString(&((*ppPropData)[0].m_name), _T("angle"));
				SetString(&((*ppPropData)[0].m_descr), _T("Angle"));
				(*ppPropData)[0].m_datatype = VT_UI2;
				(*ppPropData)[0].m_accessRights = OT_ACCESSRIGHTS_READABLE | OT_ACCESSRIGHTS_WRITEABLE;
				ret = S_OK;
			}

			break;
		}
	}

	return ret;
}

#if defined(WIN32) || defined(WIN64)

long OTSAPI_CALL QueryConditions(
	IN OTSObjectData* pObjectData,
	IN OTChar* sourcePath,
	OUT unsigned long* pConditionCount,
	OUT OTChar** *pConditionNames)
{
	long ret = S_FALSE;
	*pConditionNames = NULL;
	*pConditionCount = 0;

	if (pObjectData)
	{
		switch (pObjectData->m_userData)
		{
		case OBJ_IDX_SRC_TIMER:
			*pConditionNames = (OTChar**)OTAllocateMemory(sizeof(OTChar**) * 2);
			*pConditionCount = 2;
			SetString(&((*pConditionNames)[0]), COND_NAME_BETWEEN_SINGLE);
			SetString(&((*pConditionNames)[1]), COND_NAME_BETWEEN_MULTIPLE);
			ret = S_OK;
			break;
		}
	}

	return ret;
}

long OTSAPI_CALL AcknowledgeCondition(IN OTSObjectData conditionData, IN OTChar* ackId, OTChar* ackComment)
{
	OTSConditionData condData;
	condData.m_changeMask = OT_CONDITIONCHANGE_ACK_STATE | OT_CONDITIONCHANGE_ACK_DATA;
	condData.m_stateChange = OT_CONDITIONSTATE_ACKED;
	CoFileTimeNow(&condData.m_occurenceTime);
	condData.m_ackRequired = false;
	condData.m_ackID = ackId;
	condData.m_ackComment = ackComment;
	condData.m_ackTime = condData.m_occurenceTime;
	OTSConditionsChanged(1, &conditionData.m_objectHandle, &condData, &g_objHandles[conditionData.m_userData]);
	return S_OK;
}

long OTSAPI_CALL QueryConditionDefinition(IN OTSObjectData conditionData, OUT OTSConditionDefinitionData* pConditionDefData)
{
	long res = E_FAIL;

	switch (conditionData.m_userData)
	{
	case OBJ_IDX_COND_SINGLE:
		SetString(&pConditionDefData->m_definition, _T("between single"));
		pConditionDefData->m_subConditionCount = 0;
		pConditionDefData->m_subConditionDefinitions = NULL;
		pConditionDefData->m_subConditionDescriptions = NULL;
		pConditionDefData->m_subConditionSeverities = NULL;
		res = S_OK;
		break;

	case OBJ_IDX_COND_MULTIPLE:
		SetString(&pConditionDefData->m_definition, _T("between multiple"));
		pConditionDefData->m_subConditionCount = 2;
		pConditionDefData->m_subConditionDefinitions = (OTChar**)OTAllocateMemory(sizeof(OTChar*) * pConditionDefData->m_subConditionCount);
		pConditionDefData->m_subConditionDescriptions = (OTChar**)OTAllocateMemory(sizeof(OTChar*) * pConditionDefData->m_subConditionCount);
		pConditionDefData->m_subConditionSeverities = (unsigned long*)OTAllocateMemory(sizeof(unsigned long) * pConditionDefData->m_subConditionCount);
		SetString(pConditionDefData->m_subConditionDefinitions + 0, _T("10 <= t < 30"));
		SetString(pConditionDefData->m_subConditionDefinitions + 1, _T("30 <= t < 50"));
		SetString(pConditionDefData->m_subConditionDescriptions + 0, _T("between 10 and 30"));
		SetString(pConditionDefData->m_subConditionDescriptions + 1, _T("between 30 an 50"));
		pConditionDefData->m_subConditionSeverities[0] = 500;
		pConditionDefData->m_subConditionSeverities[1] = 540;
		res = S_OK;
		break;
	}

	return res;
}

long OTSAPI_CALL WebHandleTemplate(IN OTChar* templateName, IN unsigned long numArgs, IN OTChar** pArgs, OUT OTChar** pResult)
{
	if (_tcsncmp(templateName, _T("CURRENTTIME"), 11) == 0)
	{
		OTBool isGerman = false;

		if (numArgs == 1)
		{
			isGerman = (_tcscmp(pArgs[0], _T("GERMAN")) == 0);
		}

		*pResult = (OTChar*)OTAllocateMemory(512);
		SYSTEMTIME st;
		GetLocalTime(&st);

		if (isGerman)
		{
			_stprintf(*pResult, _T("%2.2d.%2.2d.%4.4d %2.2d:%2.2d:%2.2d"), st.wDay, st.wMonth, st.wYear, st.wHour, st.wMinute, st.wSecond);
		}
		else
		{
			unsigned short hour;
			OTChar* pmam;

			if (st.wHour < 13)
			{
				hour = st.wHour;
				pmam = _T("AM");
			}
			else
			{
				hour = st.wHour - 12;
				pmam = _T("PM");
			}

			_stprintf(*pResult, _T("%2.2d/%2.2d/%4.4d %2.2d:%2.2d:%2.2d %s"), st.wMonth, st.wDay, st.wYear, hour, st.wMinute, st.wSecond, pmam);
		}

		return S_OK;
	}

	return E_NOTIMPL;
}

#endif // WIN32

void SetString(OTChar** pStrD, OTChar* strS)
{
	*pStrD = (OTChar*)OTAllocateMemory((unsigned long)(_tcslen(strS) + sizeof(OTChar)));
	_tcscpy(*pStrD, strS);
}
