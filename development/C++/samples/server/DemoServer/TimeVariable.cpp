#include "stdafx.h"
#include "TimeVariable.h"

TimeVariable::TimeVariable(tstring varName) : DemoDaAddressSpaceElement()
{
	setName(varName);
	setAccessRights(EnumAccessRights_READABLE);
	setIoMode(EnumIoMode_POLL);
}

TimeVariable::TimeVariable(
	IN tstring variableName,    // name
	IN enum TimeType type,      // type
	IN enum TimeZone timeZone)  // time zone
{
	setName(variableName);
	setAccessRights(EnumAccessRights_READABLE);

	switch (type)
	{
	case hour:
	case second:
	case minute:
		setDatatype(VT_UI1);
		break;

	case string:
		setDatatype(VT_BSTR);
		break;

	case array:
		setDatatype((VARENUM)(VT_UI1 | VT_ARRAY));
		break;

	case date:
		setDatatype(VT_DATE);
		break;

	case limitSecond:
		setAccessRights(EnumAccessRights_READWRITEABLE);
		setDatatype(VT_UI1);
		break;
	}

	m_timeType = type;
	m_timeZone = timeZone;
	setIoMode(EnumIoMode_POLL);
}

void TimeVariable::handleReadRequest(DaRequest* pRequest)   // request
{
	if (pRequest)
	{
		Variant value;
		DateTime now;
		now.now();
		unsigned short secs = 0;
		unsigned short mins = 0;
		unsigned short hours = 0;

		switch (m_timeZone)
		{
		case local:
			secs = now.second();
			mins = now.minute();
			hours = now.hour();
			break;

		case GMT:
			secs = now.secondGMT();
			mins = now.minuteGMT();
			hours = now.hourGMT();
			break;

		case none:
			// nothing to do
			break;
		}

		switch (m_timeType)
		{
		case hour:
			value = hours;
			break;

		case second:
			value = secs;
			break;

		case minute:
			value = mins;
			break;

		case date:
			value.vt = VT_DATE;
			DATE timeDATE;
#ifdef TBC_OS_WINDOWS
			now.get(timeDATE);
#endif
#ifdef TBC_OS_LINUX
			now.get(&timeDATE);
#endif
			value.setDATE(timeDATE);
			break;

		case array:
		{
			SAFEARRAYBOUND dim;
			unsigned char* timeArray;
			dim.lLbound = 0;
			dim.cElements = 3;
			value.vt = VT_UI1 | VT_ARRAY;
			value.parray = Variant::safeArrayCreate(VT_UI1, 1, &dim);
			Variant::SafeArrayAccessData(value.parray, (void**)&timeArray);
			timeArray[0] = (unsigned char)hours;
			timeArray[1] = (unsigned char)mins;
			timeArray[2] = (unsigned char)secs;
			Variant::SafeArrayUnaccessData(value.parray);
		}
		break;

		case limitSecond:
		{
			ValueQT cacheValue;
			getCacheValue(cacheValue);
			value.setUI1(cacheValue.getData().bVal);
		}
		break;

		case string:
		{
			tstringstream streamTimeH;
			streamTimeH << hours;
			tstringstream streamTimeM;
			streamTimeM << mins;
			tstringstream streamTimeS;
			streamTimeS << secs;
			tstring stringTime = streamTimeH.str() + _T(":") +
								 streamTimeM.str() + _T(":") +
								 streamTimeS.str();
			streamTimeH.clear();
			streamTimeM.clear();
			streamTimeS.clear();
			value.setBSTR(stringTime.c_str());
		}
		break;
		}

		ValueQT val(value, EnumQuality_GOOD, now);
		this->valueChanged(val);
		pRequest->setValue(val);
		pRequest->setResult(S_OK);
	}
} // handleRequest

void TimeVariable::handleWriteRequest(DaRequest* pRequest)
{
	if (pRequest)
	{
		if (m_timeType == limitSecond)
		{
			long res = S_FALSE;
			DaAddressSpaceElement* pElement = pRequest->getAddressSpaceElement();

			if (pElement)
			{
				pElement->setDatatype(VT_UI1);
				ValueQT* pValue = pRequest->getValue();

				if (pValue->getData().bVal <= 59)
				{
					pElement->valueChanged(*pValue);
					res = S_OK;
				}
				else
				{
					res = S_FALSE;
				}

				pRequest->setResult(res);
			}
			else
			{
				// should never get here
				pRequest->setResult(E_FAIL);
			}
		}
	}
}
