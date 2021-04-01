#include "stdafx.h"
#include "OpcServer.h"
#include "ServerCommon.h"

#include "SOVersion.h"

OpcServer* instance = NULL;

#ifdef TBC_OS_WINDOWS
extern HANDLE g_endEvent;
#endif

#ifdef TBC_OS_LINUX
extern PosixEvents g_events;
#endif

const unsigned char MyDaAddressSpaceElement::TYP_UNKNOWN    = 0x00;
const unsigned char MyDaAddressSpaceElement::TYP_NODE       = 0x80;
const unsigned char MyDaAddressSpaceElement::TYP_TAG        = 0x08;

#ifdef TBC_OS_WINDOWS

tstring COND_NAME_BETWEEN_SINGLE    = _T("between [10,50]");
tstring COND_NAME_BETWEEN_MULTIPLE  = _T("between");

const unsigned long CAT_ID_TIME_TICK    = 1;
const unsigned long CAT_ID_KEY_HIT      = 2;
const unsigned long CAT_ID_TIME_SLOT    = 3;

#endif

Mutex MyTransaction::m_syncTransaction;

long API_CALL handleShutdownRequest(void)
{
#ifdef TBC_OS_WINDOWS
	SetEvent(g_endEvent);
#endif
#ifdef TBC_OS_LINUX
	g_events.signal(0);
#endif
	return S_OK;
}   //  end HandleShutdownRequest

OpcServer* getOpcServer(void)
{
	return instance;
}   //  end getOpcServer


void createOpcServer(void)
{
	if (instance == NULL)
	{
		instance = new OpcServer();
	}   //  end if
}   //  end createOpcServer


void destroyOpcServer(void)
{
	if (instance != NULL)
	{
		delete instance;
		instance = NULL;
	}   //  end if
}   //  end destroyOpcServer


OpcServer::OpcServer(void)
{
#ifdef TBC_OS_WINDOWS
	m_timeTick = NULL;
	m_timeSlot = NULL;
	m_keyHit = NULL;
	m_condition1 = NULL;
	m_condition2 = NULL;
#endif
}   //  end constructor


OpcServer::~OpcServer(void)
{
}   //  end destructor


long OpcServer::initialize(void)
{
	getApp()->setVersionOtb(447);
	getApp()->setAppType(EnumApplicationType_EXECUTABLE);
	getApp()->setClsidDa(_T("{77D78F35-90A2-470c-AFD8-FE2343F78C54}"));
	getApp()->setProgIdDa(_T("Softing.OPCToolbox.C++.Smpl.CTT.DA.1"));
	getApp()->setVerIndProgIdDa(_T("Softing.OPCToolbox.C++.Smpl.CTT.DA"));
#ifdef TBC_OS_WINDOWS
	getApp()->setClsidAe(_T("{1E80512D-1D31-4127-AAE1-63D3FFEE6297}"));
	getApp()->setProgIdAe(_T("Softing.OPCToolbox.C++.Smpl.CTT.AE.1"));
	getApp()->setVerIndProgIdAe(_T("Softing.OPCToolbox.C++.Smpl.CTT.AE"));
#endif
	getApp()->setDescription(_T("Softing OPC Toolkit C++ Console CTT Server Sample"));
	getApp()->setServiceName(tstring());
	getApp()->setIpPortHTTP(8079);
	getApp()->setUrlDa(_T("/OPC/DA"));
	getApp()->setMajorVersion(MAJOR_VERSION_NUMBER);
	getApp()->setMinorVersion(MINOR_VERSION_NUMBER);
	getApp()->setPatchVersion(PATCH_VERSION_NUMBER);
	getApp()->setBuildNumber(BUILD_NUMBER);
	getApp()->setVendorInfo(tstring(_T("Softing Industrial Automation GmbH")));
	getApp()->setMinUpdateRateDa(100);
	getApp()->setClientCheckPeriod(30000);
	getApp()->setAddressSpaceDelimiter(_T('.'));
	getApp()->setPropertyDelimiter(_T('#'));
	getApp()->setSupportDisableConditions(true);
	getApp()->ShutdownRequest = handleShutdownRequest;
	//  proceed with initializing of the Toolkit tracing mechanism
	getApp()->enableTracing(
		EnumTraceGroup_ALL,
		EnumTraceGroup_ALL,
		EnumTraceGroup_ALL,
		EnumTraceGroup_ALL,
		_T("Trace.txt"),
		1000000,
		2);
	return S_OK;
}   //  end initialize


void OpcServer::setServiceName(tstring serviceName)
{
	getApp()->setServiceName(serviceName);
}   //  end setServiceName


long OpcServer::prepare(MyCreator* creator)
{
	long result = S_OK;
#ifdef TBC_OS_WINDOWS
	//  TODO - binary license activation
	//  Fill in your binary license activation keys here
	//
	//  NOTE: you can activate one or all of the features at the same time
	//  firstly activate the COM-DA Server feature
	result = getApp()->activate(EnumFeature_DA_SERVER, _T("0f00-0000-0000-f97a-21d8"));

	if (!SUCCEEDED(result))
	{
		return result;
	}

	//  activate the XML-DA Server Feature
	result = getApp()->activate(EnumFeature_XMLDA_SERVER, _T("0f20-0000-0000-07ea-d600"));

	if (!SUCCEEDED(result))
	{
		return result;
	}

	//  activate the COM-AE Server Feature
	result = getApp()->activate(EnumFeature_AE_SERVER, _T("0f10-0000-0000-5deb-054a"));

	if (!SUCCEEDED(result))
	{
		return result;
	}

#endif //   TBC_OS_WINDOES
#ifdef TBC_OS_LINUX
	//  activate the XML-DA Server Feature
	result = getApp()->activate(EnumFeature_XMLDA_SERVER, _T("0bb0-0000-0000-80d8-419c"));

	if (!SUCCEEDED(result))
	{
		return result;
	}

#endif  //  TBC_OS_LINUX
	//  END TODO - binary license activation
	//  a valid creator must be provided to the initialize
	result = getApp()->initialize(creator);

	if (!SUCCEEDED(result))
	{
		return result;
	}

	return result;
}   //  end prepare


long OpcServer::start(void)
{
	return getApp()->start();
}   //  end start


long OpcServer::ready(void)
{
	return getApp()->ready();
}   //  end ready


long OpcServer::stop(void)
{
	return getApp()->stop();
}   //  end stop


long OpcServer::terminate(void)
{
#ifdef TBC_OS_WINDOWS

	if (m_timeTick)
	{
		delete m_timeTick;
		m_timeTick = NULL;
	}

	if (m_timeSlot)
	{
		delete m_timeSlot;
		m_timeSlot = NULL;
	}

	if (m_keyHit)
	{
		delete m_keyHit;
		m_keyHit = NULL;
	}

	if (m_condition1)
	{
		delete m_condition1;
		m_condition1 = NULL;
	}

	if (m_condition2)
	{
		delete m_condition2;
		m_condition2 = NULL;
	}

#endif
	long result = getApp()->terminate();
	releaseApp();
	return result;
}   //  end terminate


long OpcServer::processCommandLine(tstring command)
{
	return getApp()->processCommandLine(command);
}   //  end processCommandLine


long OpcServer::buildDaAddressSpace(void)
{
	MyDaAddressSpaceElement* tag = NULL;
	MyDaAddressSpaceElement* node = NULL;
	tstring name;
	DateTime time;
	time.now();
	Variant theValue;
	node = new MyDaAddressSpaceElement();
	name = _T("CTT");
	node->setName(name);
	node->setIoMode(EnumIoMode_NONE);
	node->setHasChildren(TRUE);
	node->setType(MyDaAddressSpaceElement::TYP_NODE);
	DaAddressSpaceRoot* daRoot = getApp()->getDaAddressSpaceRoot();

	if (daRoot == NULL)
	{
		return FALSE;
	}

	if (!daRoot->addChild(node))
	{
		return FALSE;
	}

	tag = new MyDaAddressSpaceElement();
	name = _T("I1");
	tag->setName(name);
	tag->setAccessRights(EnumAccessRights_READWRITEABLE);
	tag->setDatatype(VT_I1);
	tag->setIoMode(EnumIoMode_POLL);
	tag->setType(MyDaAddressSpaceElement::TYP_TAG);
	node->addChild(tag);
	theValue.setI1(1);
	ValueQT valueQTI1(theValue, EnumQuality_GOOD, time);
	tag->valueChanged(valueQTI1);
	tag = new MyDaAddressSpaceElement();
	name = _T("UI1");
	tag->setName(name);
	tag->setAccessRights(EnumAccessRights_READWRITEABLE);
	tag->setDatatype(VT_UI1);
	tag->setIoMode(EnumIoMode_POLL);
	tag->setType(MyDaAddressSpaceElement::TYP_TAG);

	if (!node->addChild(tag))
	{
		return FALSE;
	}

	theValue.setUI1(1);
	ValueQT valueQTUI1(theValue, EnumQuality_GOOD, time);
	tag->valueChanged(valueQTUI1);
	tag = new MyDaAddressSpaceElement();
	name = _T("I2");
	tag->setName(name);
	tag->setAccessRights(EnumAccessRights_READWRITEABLE);
	tag->setDatatype(VT_I2);
	tag->setIoMode(EnumIoMode_POLL);
	tag->setType(MyDaAddressSpaceElement::TYP_TAG);
	node->addChild(tag);
	theValue.setI2(1);
	ValueQT valueQTI2(theValue, EnumQuality_GOOD, time);
	tag->valueChanged(valueQTI1);
	tag = new MyDaAddressSpaceElement();
	name = _T("UI2");
	tag->setName(name);
	tag->setAccessRights(EnumAccessRights_READWRITEABLE);
	tag->setDatatype(VT_UI2);
	tag->setIoMode(EnumIoMode_POLL);
	tag->setType(MyDaAddressSpaceElement::TYP_TAG);
	node->addChild(tag);
	theValue.setUI2(1);
	ValueQT valueQTUI2(theValue, EnumQuality_GOOD, time);
	tag->valueChanged(valueQTUI2);
	tag = new MyDaAddressSpaceElement();
	name = _T("I4");
	tag->setName(name);
	tag->setAccessRights(EnumAccessRights_READWRITEABLE);
	tag->setDatatype(VT_I4);
	tag->setIoMode(EnumIoMode_POLL);
	tag->setType(MyDaAddressSpaceElement::TYP_TAG);
	node->addChild(tag);
	theValue.setI4(1);
	ValueQT valueQTI4(theValue, EnumQuality_GOOD, time);
	tag->valueChanged(valueQTI4);
	tag = new MyDaAddressSpaceElement();
	name = _T("UI4");
	tag->setName(name);
	tag->setAccessRights(EnumAccessRights_READWRITEABLE);
	tag->setDatatype(VT_UI4);
	tag->setIoMode(EnumIoMode_POLL);
	tag->setType(MyDaAddressSpaceElement::TYP_TAG);
	node->addChild(tag);
	theValue.setUI4(1);
	ValueQT valueQTUI4(theValue, EnumQuality_GOOD, time);
	tag->valueChanged(valueQTUI4);
	tag = new MyDaAddressSpaceElement();
	name = _T("I8");
	tag->setName(name);
	tag->setAccessRights(EnumAccessRights_READWRITEABLE);
	tag->setDatatype(VT_I8);
	tag->setIoMode(EnumIoMode_POLL);
	tag->setType(MyDaAddressSpaceElement::TYP_TAG);
	node->addChild(tag);
	theValue.setI8(1);
	ValueQT valueQTI8(theValue, EnumQuality_GOOD, time);
	tag->valueChanged(valueQTI8);
	tag = new MyDaAddressSpaceElement();
	name = _T("UI8");
	tag->setName(name);
	tag->setAccessRights(EnumAccessRights_READWRITEABLE);
	tag->setDatatype(VT_UI8);
	tag->setIoMode(EnumIoMode_POLL);
	tag->setType(MyDaAddressSpaceElement::TYP_TAG);
	node->addChild(tag);
	theValue.setUI8(1);
	ValueQT valueQTUI8(theValue, EnumQuality_GOOD, time);
	tag->valueChanged(valueQTUI8);
	tag = new MyDaAddressSpaceElement();
	name = _T("R4");
	tag->setName(name);
	tag->setAccessRights(EnumAccessRights_READWRITEABLE);
	tag->setDatatype(VT_R4);
	tag->setIoMode(EnumIoMode_POLL);
	tag->setType(MyDaAddressSpaceElement::TYP_TAG);
	node->addChild(tag);
	theValue.setR4(1.0);
	ValueQT valueQTR4(theValue, EnumQuality_GOOD, time);
	tag->valueChanged(valueQTR4);
	tag = new MyDaAddressSpaceElement();
	name = _T("R8");
	tag->setName(name);
	tag->setAccessRights(EnumAccessRights_READWRITEABLE);
	tag->setDatatype(VT_R8);
	tag->setIoMode(EnumIoMode_POLL);
	tag->setType(MyDaAddressSpaceElement::TYP_TAG);
	node->addChild(tag);
	theValue.setR8(1.0);
	ValueQT valueQTR8(theValue, EnumQuality_GOOD, time);
	tag->valueChanged(valueQTR8);
	tag = new MyDaAddressSpaceElement();
	name = _T("Bool");
	tag->setName(name);
	tag->setAccessRights(EnumAccessRights_READWRITEABLE);
	tag->setDatatype(VT_BOOL);
	tag->setIoMode(EnumIoMode_POLL);
	tag->setType(MyDaAddressSpaceElement::TYP_TAG);
	node->addChild(tag);
	theValue.setBOOL(TRUE);
	ValueQT valueQTBOOL(theValue, EnumQuality_GOOD, time);
	tag->valueChanged(valueQTBOOL);
	tag = new MyDaAddressSpaceElement();
	name = _T("BSTR");
	tag->setName(name);
	tag->setAccessRights(EnumAccessRights_READWRITEABLE);
	tag->setDatatype(VT_BSTR);
	tag->setIoMode(EnumIoMode_POLL);
	tag->setType(MyDaAddressSpaceElement::TYP_TAG);
	node->addChild(tag);
	theValue.setBSTR(_T("CTT"));
	ValueQT valueQTBSTR(theValue, EnumQuality_GOOD, time);
	tag->valueChanged(valueQTBSTR);
	tag = new MyDaAddressSpaceElement();
	name = _T("Date");
	tag->setName(name);
	tag->setAccessRights(EnumAccessRights_READWRITEABLE);
	tag->setDatatype(VT_DATE);
	tag->setIoMode(EnumIoMode_POLL);
	tag->setType(MyDaAddressSpaceElement::TYP_TAG);
	node->addChild(tag);
#ifdef TBC_OS_WINDOWS
	Variant dateVariant;
	DATE pDate;
	time.get(pDate);
	dateVariant.setDATE(pDate);
	ValueQT valueQTDate(dateVariant, EnumQuality_GOOD, time);
#endif
#ifdef TBC_OS_LINUX
	timeval tv;
	time.get(&tv);
	Variant dateVariant(tv);
	ValueQT valueQTDate(dateVariant, EnumQuality_GOOD, time);
#endif
	tag->valueChanged(valueQTDate);
	//  Arrays
	unsigned long arraySize = 5;
	SAFEARRAYBOUND dim;
	dim.lLbound = 0;
	dim.cElements = arraySize;
	tag = new MyDaAddressSpaceElement();
	name = _T("I1 Array");
	tag->setName(name);
	tag->setAccessRights(EnumAccessRights_READWRITEABLE);
	tag->setDatatype((VARENUM)(VT_ARRAY | VT_I1));
	tag->setIoMode(EnumIoMode_POLL);
	tag->setType(MyDaAddressSpaceElement::TYP_TAG);
	node->addChild(tag);
	{
		void* byteArray;
		Variant byteArrayValue;
		byteArrayValue.vt = (VARENUM)(VT_ARRAY | VT_I1);
		byteArrayValue.parray = Variant::safeArrayCreate(byteArrayValue.vt & ~VT_ARRAY, 1, &dim);
		Variant::SafeArrayAccessData(byteArrayValue.parray, (void**)&byteArray);

		for (unsigned long i = 0; i < arraySize; i++)
		{
			char* pVal = ((char*)byteArray) + i;
			*pVal = (char)i;
		} // end for

		Variant::SafeArrayUnaccessData(byteArrayValue.parray);
		ValueQT arrayI1(byteArrayValue, EnumQuality_GOOD, time);
		tag->valueChanged(arrayI1);
	}
	tag = new MyDaAddressSpaceElement();
	name = _T("UI1 Array");
	tag->setName(name);
	tag->setAccessRights(EnumAccessRights_READWRITEABLE);
	tag->setDatatype((VARENUM)(VT_ARRAY | VT_UI1));
	tag->setIoMode(EnumIoMode_POLL);
	tag->setType(MyDaAddressSpaceElement::TYP_TAG);
	node->addChild(tag);
	{
		void* uByteArray;
		Variant uByteArrayValue;
		uByteArrayValue.vt = (VARENUM)(VT_ARRAY | VT_UI1);
		uByteArrayValue.parray = Variant::safeArrayCreate(uByteArrayValue.vt & ~VT_ARRAY, 1, &dim);
		Variant::SafeArrayAccessData(uByteArrayValue.parray, (void**)&uByteArray);

		for (unsigned long j = 0; j < arraySize; j++)
		{
			unsigned char* pVal = ((unsigned char*)uByteArray) + j;
			*pVal = (unsigned char)j;
		} // end for

		Variant::SafeArrayUnaccessData(uByteArrayValue.parray);
		ValueQT arrayUI1(uByteArrayValue, EnumQuality_GOOD, time);
		tag->valueChanged(arrayUI1);
	}
	tag = new MyDaAddressSpaceElement();
	name = _T("I2 Array");
	tag->setName(name);
	tag->setAccessRights(EnumAccessRights_READWRITEABLE);
	tag->setDatatype((VARENUM)(VT_ARRAY | VT_I2));
	tag->setIoMode(EnumIoMode_POLL);
	tag->setType(MyDaAddressSpaceElement::TYP_TAG);
	node->addChild(tag);
	{
		void* shortArray;
		Variant shortArrayValue;
		shortArrayValue.vt = (VARENUM)(VT_ARRAY | VT_I2);
		shortArrayValue.parray = Variant::safeArrayCreate(shortArrayValue.vt & ~VT_ARRAY, 1, &dim);
		Variant::SafeArrayAccessData(shortArrayValue.parray, (void**)&shortArray);

		for (unsigned long k = 0; k < arraySize; k++)
		{
			short* pVal = ((short*)shortArray) + k;
			*pVal = (short)k;
		} // end for

		Variant::SafeArrayUnaccessData(shortArrayValue.parray);
		ValueQT arrayI2(shortArrayValue, EnumQuality_GOOD, time);
		tag->valueChanged(arrayI2);
	}
	tag = new MyDaAddressSpaceElement();
	name = _T("UI2 Array");
	tag->setName(name);
	tag->setAccessRights(EnumAccessRights_READWRITEABLE);
	tag->setDatatype((VARENUM)(VT_ARRAY | VT_UI2));
	tag->setIoMode(EnumIoMode_POLL);
	tag->setType(MyDaAddressSpaceElement::TYP_TAG);
	node->addChild(tag);
	{
		void* uShortArray;
		Variant uShortArrayValue;
		uShortArrayValue.vt = (VARENUM)(VT_ARRAY | VT_UI2);
		uShortArrayValue.parray = Variant::safeArrayCreate(uShortArrayValue.vt & ~VT_ARRAY, 1, &dim);
		Variant::SafeArrayAccessData(uShortArrayValue.parray, (void**)&uShortArray);

		for (unsigned long l = 0; l < arraySize; l++)
		{
			unsigned short* pVal = ((unsigned short*)uShortArray) + l;
			*pVal = (unsigned short)l;
		} // end for

		Variant::SafeArrayUnaccessData(uShortArrayValue.parray);
		ValueQT arrayUI2(uShortArrayValue, EnumQuality_GOOD, time);
		tag->valueChanged(arrayUI2);
	}
	tag = new MyDaAddressSpaceElement();
	name = _T("I4 Array");
	tag->setName(name);
	tag->setAccessRights(EnumAccessRights_READWRITEABLE);
	tag->setDatatype((VARENUM)(VT_ARRAY | VT_I4));
	tag->setIoMode(EnumIoMode_POLL);
	tag->setType(MyDaAddressSpaceElement::TYP_TAG);
	node->addChild(tag);
	{
		void* longArray;
		Variant longArrayValue;
		longArrayValue.vt = (VARENUM)(VT_ARRAY | VT_I4);
		longArrayValue.parray = Variant::safeArrayCreate(longArrayValue.vt & ~VT_ARRAY, 1, &dim);
		Variant::SafeArrayAccessData(longArrayValue.parray, (void**)&longArray);

		for (unsigned long m = 0; m < arraySize; m++)
		{
			long* pVal = ((long*)longArray) + m;
			*pVal = (long)m;
		} // end for

		Variant::SafeArrayUnaccessData(longArrayValue.parray);
		ValueQT arrayI4(longArrayValue, EnumQuality_GOOD, time);
		tag->valueChanged(arrayI4);
	}
	tag = new MyDaAddressSpaceElement();
	name = _T("UI4 Array");
	tag->setName(name);
	tag->setAccessRights(EnumAccessRights_READWRITEABLE);
	tag->setDatatype((VARENUM)(VT_ARRAY | VT_UI4));
	tag->setIoMode(EnumIoMode_POLL);
	tag->setType(MyDaAddressSpaceElement::TYP_TAG);
	node->addChild(tag);
	{
		void* uLongArray;
		Variant uLongArrayValue;
		uLongArrayValue.vt = (VARENUM)(VT_ARRAY | VT_UI4);
		uLongArrayValue.parray = Variant::safeArrayCreate(uLongArrayValue.vt & ~VT_ARRAY, 1, &dim);
		Variant::SafeArrayAccessData(uLongArrayValue.parray, (void**)&uLongArray);

		for (unsigned long n = 0; n < arraySize; n++)
		{
			unsigned long* pVal = ((unsigned long*)uLongArray) + n;
			*pVal = n;
		} // end for

		Variant::SafeArrayUnaccessData(uLongArrayValue.parray);
		ValueQT arrayUI4(uLongArrayValue, EnumQuality_GOOD, time);
		tag->valueChanged(arrayUI4);
	}
	tag = new MyDaAddressSpaceElement();
	name = _T("R4 Array");
	tag->setName(name);
	tag->setAccessRights(EnumAccessRights_READWRITEABLE);
	tag->setDatatype((VARENUM)(VT_ARRAY | VT_R4));
	tag->setIoMode(EnumIoMode_POLL);
	tag->setType(MyDaAddressSpaceElement::TYP_TAG);
	node->addChild(tag);
	{
		void* floatArray;
		Variant floatArrayValue;
		floatArrayValue.vt = (VARENUM)(VT_ARRAY | VT_R4);
		floatArrayValue.parray = Variant::safeArrayCreate(floatArrayValue.vt & ~VT_ARRAY, 1, &dim);
		Variant::SafeArrayAccessData(floatArrayValue.parray, (void**)&floatArray);

		for (unsigned long o = 0; o < arraySize; o++)
		{
			float* pVal = ((float*)floatArray) + o;
			*pVal = (float)o;
		} // end for

		Variant::SafeArrayUnaccessData(floatArrayValue.parray);
		ValueQT arrayR4(floatArrayValue, EnumQuality_GOOD, time);
		tag->valueChanged(arrayR4);
	}
	tag = new MyDaAddressSpaceElement();
	name = _T("R8 Array");
	tag->setName(name);
	tag->setAccessRights(EnumAccessRights_READWRITEABLE);
	tag->setDatatype((VARENUM)(VT_ARRAY | VT_R8));
	tag->setIoMode(EnumIoMode_POLL);
	tag->setType(MyDaAddressSpaceElement::TYP_TAG);
	node->addChild(tag);
	{
		void* doubleArray;
		Variant doubleArrayValue;
		doubleArrayValue.vt = (VARENUM)(VT_ARRAY | VT_R8);
		doubleArrayValue.parray = Variant::safeArrayCreate(doubleArrayValue.vt & ~VT_ARRAY, 1, &dim);
		Variant::SafeArrayAccessData(doubleArrayValue.parray, (void**)&doubleArray);
		double* pVal = (double*)doubleArray;

		for (unsigned long p = 0; p < arraySize; p++)
		{
			pVal[p] = (double)p;
		} // end for

		Variant::SafeArrayUnaccessData(doubleArrayValue.parray);
		ValueQT arrayR8(doubleArrayValue, EnumQuality_GOOD, time);
		tag->valueChanged(arrayR8);
	}
	tag = new MyDaAddressSpaceElement();
	name = _T("I8 Array");
	tag->setName(name);
	tag->setAccessRights(EnumAccessRights_READWRITEABLE);
	tag->setDatatype((VARENUM)(VT_ARRAY | VT_I8));
	tag->setIoMode(EnumIoMode_POLL);
	tag->setType(MyDaAddressSpaceElement::TYP_TAG);
	node->addChild(tag);
	{
		void* pArray = NULL;
		Variant I8ArrayValue;
		I8ArrayValue.vt = (VARENUM)(VT_ARRAY | VT_I8);
		I8ArrayValue.parray = Variant::safeArrayCreate(VT_I8, 1, &dim);
		Variant::SafeArrayAccessData(I8ArrayValue.parray, (void**)&pArray);
		LONGLONG* pVal = (LONGLONG*)pArray;

		for (unsigned long p = 0; p < arraySize; p++)
		{
			pVal[p] = (LONGLONG)p;
		}   //  end for

		Variant::SafeArrayUnaccessData(I8ArrayValue.parray);
		ValueQT arrayI8(I8ArrayValue, EnumQuality_GOOD, time);
		tag->valueChanged(arrayI8);
	}
	tag = new MyDaAddressSpaceElement();
	name = _T("UI8 Array");
	tag->setName(name);
	tag->setAccessRights(EnumAccessRights_READWRITEABLE);
	tag->setDatatype((VARENUM)(VT_ARRAY | VT_UI8));
	tag->setIoMode(EnumIoMode_POLL);
	tag->setType(MyDaAddressSpaceElement::TYP_TAG);
	node->addChild(tag);
	{
		void* pArray = NULL;
		Variant UI8ArrayValue;
		UI8ArrayValue.vt = (VARENUM)(VT_ARRAY | VT_UI8);
		UI8ArrayValue.parray = Variant::safeArrayCreate(VT_UI8, 1, &dim);
		Variant::SafeArrayAccessData(UI8ArrayValue.parray, (void**)&pArray);
		ULONGLONG* pVal = (ULONGLONG*)pArray;

		for (unsigned long p = 0; p < arraySize; p++)
		{
			pVal[p] = (LONGLONG)p;
		}   //  end for

		Variant::SafeArrayUnaccessData(UI8ArrayValue.parray);
		ValueQT arrayUI8(UI8ArrayValue, EnumQuality_GOOD, time);
		tag->valueChanged(arrayUI8);
	}
	tag = new MyDaAddressSpaceElement();
	name = _T("Bool Array");
	tag->setName(name);
	tag->setAccessRights(EnumAccessRights_READWRITEABLE);
	tag->setDatatype((VARENUM)(VT_ARRAY | VT_BOOL));
	tag->setIoMode(EnumIoMode_POLL);
	tag->setType(MyDaAddressSpaceElement::TYP_TAG);
	node->addChild(tag);
	{
		void* boolArray = NULL;
		Variant boolArrayValue;
		boolArrayValue.vt = (VARENUM)(VT_ARRAY | VT_BOOL);
		boolArrayValue.parray = Variant::safeArrayCreate(boolArrayValue.vt & ~VT_ARRAY, 1, &dim);
		Variant::SafeArrayAccessData(boolArrayValue.parray, (void**)&boolArray);

		for (unsigned long q = 0; q < arraySize; q++)
		{
			bool* pVal = ((bool*)boolArray) + q;

			if (q % 2 == 0)
			{
				*pVal = true;
			}
			else
			{
				*pVal = false;
			} // end if .. else
		} // end for

		Variant::SafeArrayUnaccessData(boolArrayValue.parray);
		ValueQT arrayBool(boolArrayValue, EnumQuality_GOOD, time);
		tag->valueChanged(arrayBool);
	}
	tag = new MyDaAddressSpaceElement();
	name = _T("BSTR Array");
	tag->setName(name);
	tag->setAccessRights(EnumAccessRights_READWRITEABLE);
	tag->setDatatype((VARENUM)(VT_ARRAY | VT_BSTR));
	tag->setIoMode(EnumIoMode_POLL);
	tag->setType(MyDaAddressSpaceElement::TYP_TAG);
	node->addChild(tag);
	{
		void* strArray;
		Variant strArrayValue;
		strArrayValue.vt = (VARENUM)(VT_ARRAY | VT_BSTR);
		strArrayValue.parray = Variant::safeArrayCreate(strArrayValue.vt & ~VT_ARRAY, 1, &dim);
		Variant::SafeArrayAccessData(strArrayValue.parray, (void**)&strArray);

		for (unsigned long r = 0; r < arraySize; r++)
		{
#ifdef TBC_OS_LINUX
			DWORD j = r % 26;
			TCHAR** pVal = ((TCHAR**)strArray) + r;
			TCHAR str[3];
			str[0] = _T('A') + (unsigned short)j;
			str[1] = _T('a') + (unsigned short)(j + 1);
			str[2] = 0;
			*pVal = OTSysAllocString((TCHAR*)str);
#endif
#ifdef TBC_OS_WINDOWS
			DWORD j = r % 26;
			BSTR* pVal = ((BSTR*)strArray) + r;
			unsigned short wstr[3];
			wstr[0] = L'A' + (unsigned short)j;
			wstr[1] = L'a' + (unsigned short)(j + 1);
			wstr[2] = 0;
			*pVal = SysAllocString((WCHAR*)wstr);
#endif
		} // end for

		Variant::SafeArrayUnaccessData(strArrayValue.parray);
		ValueQT arrayBSTR(strArrayValue, EnumQuality_GOOD, time);
		tag->valueChanged(arrayBSTR);
	}
	tag = new MyDaAddressSpaceElement();
	name = _T("Date Array");
	tag->setName(name);
	tag->setAccessRights(EnumAccessRights_READWRITEABLE);
	tag->setDatatype((VARENUM)(VT_ARRAY | VT_DATE));
	tag->setIoMode(EnumIoMode_POLL);
	tag->setType(MyDaAddressSpaceElement::TYP_TAG);
	node->addChild(tag);
	{
		void* dateArray;
		Variant dateArrayValue;
		dateArrayValue.vt = (VARENUM)(VT_ARRAY | VT_DATE);
		dateArrayValue.parray = Variant::safeArrayCreate(dateArrayValue.vt & ~VT_ARRAY, 1, &dim);
		Variant::SafeArrayAccessData(dateArrayValue.parray, (void**)&dateArray);

		for (unsigned long t = 0; t < arraySize; t++)
		{
#ifdef TBC_OS_WINDOWS
			DATE* pVal = ((DATE*)dateArray) + t;
			time.get(*pVal);
#endif
#ifdef TBC_OS_LINUX
			timeval* pVal = ((timeval*)dateArray) + t;
			time.get(pVal);
#endif
		} // end for

		Variant::SafeArrayUnaccessData(dateArrayValue.parray);
		ValueQT arrayDate(dateArrayValue, EnumQuality_GOOD, time);
		tag->valueChanged(arrayDate);
	}
	_tprintf(_T("Build Namespace successful\n"));
	return S_OK;
}   //  end buildDaNameSpace


void OpcServer::trace(
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


#ifdef TBC_OS_WINDOWS

long OpcServer::buildAeAddressSpace(void)
{
	//  AE
	AeAddressSpaceRoot* aeRoot = getApp()->getAeAddressSpaceRoot();
	MyAeAddressSpaceElement* clock = new MyAeAddressSpaceElement();
	tstring aName = tstring(_T("clock"));
	clock->setName(aName);
	clock->setHasChildren(TRUE);
	aeRoot->addChild(clock);
	MyAeAddressSpaceElement* timer = new MyAeAddressSpaceElement();
	aName = tstring(_T("timer"));
	timer->setName(aName);
	timer->setHasChildren(FALSE);
	clock->addChild(timer);
	MyAeAddressSpaceElement* timeSlot1 = new MyAeAddressSpaceElement();
	aName = tstring(_T("time slot 1"));
	timeSlot1->setName(aName);
	timeSlot1->setHasChildren(FALSE);
	clock->addChild(timeSlot1);
	MyAeAddressSpaceElement* timeSlot2 = new MyAeAddressSpaceElement();
	aName = tstring(_T("time slot 2"));
	timeSlot2->setName(aName);
	timeSlot2->setHasChildren(FALSE);
	clock->addChild(timeSlot2);
	MyAeAddressSpaceElement* keyboard = new MyAeAddressSpaceElement();
	aName = tstring(_T("keyboard"));
	keyboard->setName(aName);
	keyboard->setHasChildren(TRUE);
	aeRoot->addChild(keyboard);
	MyAeAddressSpaceElement* key = new MyAeAddressSpaceElement();
	aName = tstring(_T("key"));
	key->setName(aName);
	key->setHasChildren(FALSE);
	keyboard->addChild(key);
	return S_OK;
}


long OpcServer::buildEventCategories(void)
{
	long result = S_OK;
	tstring timeTick(_T("time tick"));
	m_timeTick = new AeCategory(EnumEventType_SIMPLE, CAT_ID_TIME_TICK, timeTick);
	result = getApp()->addAeCategory(m_timeTick);

	if (result == S_OK)
	{
		tstring hour(_T("hour"));
		m_timeTick->addAttribute(1, hour, VT_UI2);
		tstring minute(_T("minute"));
		m_timeTick->addAttribute(2, minute, VT_UI2);
		tstring second(_T("second"));
		m_timeTick->addAttribute(3, second, VT_UI2);
	}   //  end if

	tstring keyHit(_T("key hit"));
	m_keyHit = new AeCategory(EnumEventType_TRACKING, CAT_ID_KEY_HIT, keyHit);
	result = getApp()->addAeCategory(m_keyHit);

	if (result == S_OK)
	{
		tstring key(_T("key"));
		m_keyHit->addAttribute(1, key, VT_UI2);
	}

	tstring timeSlot(_T("time slot"));
	m_timeSlot = new AeCategory(EnumEventType_CONDITION, CAT_ID_TIME_SLOT, timeSlot);
	result = getApp()->addAeCategory(m_timeSlot);

	if (result == S_OK)
	{
		tstring second(_T("second"));
		tstring str;
		m_timeSlot->addAttribute(1, second, VT_UI2);
		m_timeSlot->addConditionDefinition(COND_NAME_BETWEEN_SINGLE);
		m_timeSlot->addConditionDefinition(COND_NAME_BETWEEN_MULTIPLE);
		str = _T("[10,30]");
		m_timeSlot->addSubConditionDefinition(COND_NAME_BETWEEN_MULTIPLE, str);
		AeSubConditionDefinition* subConditionDefinition = m_timeSlot->getSubConditionDefinition(COND_NAME_BETWEEN_MULTIPLE, str);

		if (subConditionDefinition != NULL)
		{
			subConditionDefinition->setDescription(str);
			subConditionDefinition->setSeverity(100);
		}

		str = _T("[30,50]");
		m_timeSlot->addSubConditionDefinition(COND_NAME_BETWEEN_MULTIPLE, str);
		subConditionDefinition = m_timeSlot->getSubConditionDefinition(COND_NAME_BETWEEN_MULTIPLE, str);

		if (subConditionDefinition != NULL)
		{
			subConditionDefinition->setDescription(str);
			subConditionDefinition->setSeverity(200);
		}
	}   //  end if

	return result;
}

void OpcServer::initializeConditions(void)
{
	m_condition1 = new AeCondition();
	m_condition2 = new AeCondition();
	m_condition1->setCategory(m_timeSlot);
	m_condition1->setName(COND_NAME_BETWEEN_SINGLE);
	m_condition1->setDefinition(m_timeSlot->getConditionDefinition(COND_NAME_BETWEEN_SINGLE));
	tstring sourcePath = _T("clock.time slot 1");
	m_condition1->setSourcePath(sourcePath);
	m_condition1->setQuality(EnumQuality_GOOD);
	m_condition1->setUserData(1);
	m_condition2->setCategory(m_timeSlot);
	m_condition2->setDefinition(m_timeSlot->getConditionDefinition(COND_NAME_BETWEEN_MULTIPLE));
	m_condition2->setName(COND_NAME_BETWEEN_MULTIPLE);
	sourcePath = _T("clock.time slot 2");
	m_condition2->setSourcePath(sourcePath);
	m_condition2->setQuality(EnumQuality_GOOD);
	m_condition2->setUserData(2);
}

void OpcServer::updateAECondition(
	unsigned long severity,
	tstring& message,
	BOOL ackReq,
	tstring& subCondName,
	std::vector<Variant>& eventAttributes,
	unsigned short changeMask,
	EnumConditionState conditionState,
	AEConditionIndex conditionIndex)
{
	switch (conditionIndex)
	{
	case FirstCondition:
	{
		m_condition1->setSeverity(severity);
		m_condition1->setMessage(message);
		m_condition1->setAckRequired(ackReq);
		m_condition1->setChangeMask(changeMask);
		m_condition1->setStateChange(conditionState);
		m_condition1->setEventAttributes(eventAttributes);

		if (!subCondName.empty())
		{
			m_condition1->setActiveSubConditionName(subCondName);
		}

		m_conditionList.push_back(m_condition1);
	}
	break;

	case SecondCondition:
	{
		m_condition2->setSeverity(severity);
		m_condition2->setMessage(message);
		m_condition2->setAckRequired(ackReq);
		m_condition2->setChangeMask(changeMask);
		m_condition2->setStateChange(conditionState);
		m_condition2->setEventAttributes(eventAttributes);

		if (!subCondName.empty())
		{
			m_condition2->setActiveSubConditionName(subCondName);
		}

		m_conditionList.push_back(m_condition2);
	}
	break;
	}
}

void OpcServer::clearConditionList()
{
	m_conditionList.clear();
}

void OpcServer::changeConditions()
{
	if (m_conditionList.size() > 0)
	{
		getApp()->changeConditions(m_conditionList);
		clearConditionList();
	}   //  end if
}

void OpcServer::handleSimpleEvents(
	unsigned short& second,
	unsigned short& minute,
	DateTime& now)
{
	tstring anActorID(_T("opcUser"));
	tstring message(_T(""));

	if (second != now.second())
	{
		AeEvent secondEvent(EnumEventType_SIMPLE, tstring(_T("clock.timer")), CAT_ID_TIME_TICK);
		std::vector<Variant> attributeValues(3);
		attributeValues[0] = now.hour();
		attributeValues[1] = now.minute();
		attributeValues[2] = now.second();
		second = now.second();
		secondEvent.setSeverity(50);
		secondEvent.setActorId(anActorID);
		message = tstring(_T("Second Changed"));
		secondEvent.setMessage(message);
		secondEvent.setAttributeValueList(attributeValues);
		secondEvent.fire();
	}   //  end if

	if (minute != now.minute())
	{
		AeEvent minuteEvent(EnumEventType_SIMPLE, tstring(_T("clock.timer")), CAT_ID_TIME_TICK);
		std::vector<Variant> attributeValues(3);
		attributeValues[0] = now.hour();
		attributeValues[1] = now.minute();
		attributeValues[2] = now.second();
		minute = now.minute();
		minuteEvent.setSeverity(900);
		minuteEvent.setActorId(anActorID);
		message = tstring(_T("Minute Changed"));
		minuteEvent.setMessage(message);
		minuteEvent.setAttributeValueList(attributeValues);
		minuteEvent.fire();
	}   //  end if
}

void OpcServer::handleKeyEvent(TCHAR& key)
{
	tstring anActorID(_T("opcUser"));
	std::vector<Variant> attributeValues(1);
	attributeValues[0] = (unsigned short)key;
	AeEvent keyEvent(EnumEventType_TRACKING, tstring(_T("keyboard.key")), CAT_ID_KEY_HIT);
	keyEvent.setSeverity(600);
	keyEvent.setActorId(anActorID);
	tstring message(_T("key hit: "));
	message += key;
	keyEvent.setMessage(message);
	keyEvent.setAttributeValueList(attributeValues);
	keyEvent.fire();
}

#endif
