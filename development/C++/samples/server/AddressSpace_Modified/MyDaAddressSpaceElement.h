#ifndef _MYDAADDRESSSPACEELEMENT_H_
#define _MYDAADDRESSSPACEELEMENT_H_

#include "Da/ServerDaAddressSpaceElement.h"
#include "Da/ServerDaProperty.h"
#include "OpcServer.h"

using namespace SoftingOPCToolboxServer;

extern BOOL g_holdSignalChange;
extern Mutex g_syncDisplay;
extern const unsigned short g_minStringLength;
extern const unsigned short g_maxStringLength;
extern const unsigned int g_qualities[17];

#ifdef TBC_OS_WINDOWS
extern HANDLE g_changedEvent;
#endif

#ifdef TBC_OS_LINUX
extern PosixEvents g_events;
#endif

//-----------------------------------------------------------------------------
// MyDaAddressSpaceElement
//-----------------------------------------------------------------------------
class MyDaAddressSpaceElement : public DaAddressSpaceElement
{

public:
	MyDaAddressSpaceElement(
		tstring& anItemID,
		tstring& aName,
		unsigned long anUserData,
		unsigned long anObjectHandle,
		unsigned long aParentHandle) : DaAddressSpaceElement(anItemID, aName, anUserData, anObjectHandle, aParentHandle)
	{
	}   //  end ctr

	MyDaAddressSpaceElement(void)
	{
	}   //  end ctr

	virtual ~MyDaAddressSpaceElement()
	{
	}   //  end destructor

	virtual void addedToAddressSpace(void)
	{
	} // end addedToAddressSpace


	virtual void removedFromAddressSpace(void)
	{
	} // end removedFromAddressSpace

	virtual void setRandomDatatype(const unsigned int& rnd)
	{
		switch (rnd % 13)
		{
		case 0:
			setDatatype(VT_I1);
			break;
		case 1:
			setDatatype(VT_I2);
			break;
		case 2:
			setDatatype(VT_I4);
			break;
		case 3:
			setDatatype(VT_I8);
			break;
		case 4:
			setDatatype(VT_UI1);
			break;
		case 5:
			setDatatype(VT_UI2);
			break;
		case 6:
			setDatatype(VT_UI4);
			break;
		case 7:
			setDatatype(VT_UI8);
			break;
		case 8:
			setDatatype(VT_BOOL);
			break;
		case 9:
			setDatatype(VT_R4);
			break;
		case 10:
			setDatatype(VT_R8);
			break;
		case 11:
			setDatatype(VT_BSTR);
			break;
		case 12:
			setDatatype(VT_DATE);
			break;
		default:
			setDatatype(VT_EMPTY);
		}
	}

	void emptyValue(DateTime &writeDateTime)
	{
		Variant writeData;
		EnumQuality writeQuality;
		switch (getDatatype())
		{
		case VT_I1:
			writeData.setI1(0);
			break;
		case VT_I2:
			writeData.setI2(0);
			break;
		case VT_I4:
			writeData.setI4(0);
			break;
		case VT_I8:
			writeData.setI8(0);
			break;
		case VT_UI1:
			writeData.setUI1(0);
			break;
		case VT_UI2:
			writeData.setUI2(0);
			break;
		case VT_UI4:
			writeData.setUI4(0);
			break;
		case VT_UI8:
			writeData.setUI8(0);
			break;
		case VT_BOOL:
			writeData.setBOOL(false);
			break;
		case VT_R4:
			writeData.setR4(0);
			break;
		case VT_R8:
			writeData.setR8(0);
			break;
		case VT_BSTR:
			{
				TCHAR str[1] = {0};
				writeData.setBSTR(str);
			}
			break;
		case VT_DATE:
			DATE aDate;
			writeDateTime.get(aDate);
			writeData.setDATE(aDate);
			break;
		default:
			writeData.setI1(0);
			if (g_syncDisplay.lock(0) == TRUE)
			{
				_tprintf(_T("%s is of datatype %d, no random value could be calculated. I1 random set.\n"), this->getName().c_str(), getDatatype());
				g_syncDisplay.unlock();
			}
		}
		writeQuality = EnumQuality_GOOD;
		m_cacheValue = ValueQT(writeData, writeQuality, writeDateTime);
		valueChanged(m_cacheValue);
	}

	void randomValue(DateTime &writeDateTime, int &rnd)
	{
		Variant writeData;
		EnumQuality writeQuality;
		if (rnd % 2 == 1)
		{
			writeDateTime.now();
		}
		else
		{
			writeDateTime.clear();
		}
		switch (getDatatype())
		{
		case VT_I1:
			writeData.setI1(rnd);
			break;
		case VT_I2:
			writeData.setI2(rnd * 13 / 7);
			break;
		case VT_I4:
			writeData.setI4(rnd * 769 / 7);
			break;
		case VT_I8:
			writeData.setI8(rnd * 7919 / 7);
			break;
		case VT_UI1:
			writeData.setUI1(rnd);
			break;
		case VT_UI2:
			writeData.setUI2(rnd * 13 / 7);
			break;
		case VT_UI4:
			writeData.setUI4(rnd * 769 / 7);
			break;
		case VT_UI8:
			writeData.setUI8(rnd * 7919 / 7);
			break;
		case VT_BOOL:
			writeData.setBOOL((rnd % 2) == 1);
			break;
		case VT_R4:
			writeData.setR4(((float)(rnd * 3701)) / 7);
			break;
		case VT_R8:
			writeData.setR8(((double)(rnd * 7187)) / 7);
			break;
		case VT_BSTR:
			{
				TCHAR str[256] = {0};
				int len = g_minStringLength + (rnd % (g_maxStringLength - g_minStringLength));
				for (int i = 0; i < len; ++i)
				{
					++rnd;
					str[i] = 32 + (rnd % 95); // ascii #32...#126
				}
				writeData.setBSTR(str);
			}
			break;
		case VT_DATE:
			{
				DateTime x;
				x.now();
				x.addMicroseconds(rnd * 54763391 * 1000);
				DATE y;
				x.get(y);
				writeData.setDATE(y);
			}
			break;
		default:
			writeData.setI1(rnd);
			if (g_syncDisplay.lock(0) == TRUE)
			{
				_tprintf(_T("%s is of datatype %d, no random value could be calculated. I1 random set.\n"), this->getName().c_str(), getDatatype());
				g_syncDisplay.unlock();
			}
		}
		if (rnd % 6 > 3)
		{
			writeQuality = EnumQuality_GOOD;
		}
		else
		{
			writeQuality = (EnumQuality)g_qualities[rnd % 17];
		}
		m_cacheValue = ValueQT(writeData, writeQuality, writeDateTime);
		valueChanged(m_cacheValue);
	}

	Variant getRandomValue(int &rnd)
	{
		Variant randomValue;
		EnumQuality writeQuality;
		switch (getDatatype())
		{
		case VT_I1:
			randomValue.setI1(rnd);
			break;
		case VT_I2:
			randomValue.setI2(rnd * 13 / 7);
			break;
		case VT_I4:
			randomValue.setI4(rnd * 769 / 7);
			break;
		case VT_I8:
			randomValue.setI8(rnd * 7919 / 7);
			break;
		case VT_UI1:
			randomValue.setUI1(rnd);
			break;
		case VT_UI2:
			randomValue.setUI2(rnd * 13 / 7);
			break;
		case VT_UI4:
			randomValue.setUI4(rnd * 769 / 7);
			break;
		case VT_UI8:
			randomValue.setUI8(rnd * 7919 / 7);
			break;
		case VT_BOOL:
			randomValue.setBOOL((rnd % 2) == 1);
			break;
		case VT_R4:
			randomValue.setR4(((float)(rnd * 3701)) / 7);
			break;
		case VT_R8:
			randomValue.setR8(((double)(rnd * 7187)) / 7);
			break;
		case VT_BSTR:
			{
				TCHAR str[256] = {0};
				int len = g_minStringLength + (rnd % (g_maxStringLength - g_minStringLength));
				for (int i = 0; i < len; ++i)
				{
					++rnd;
					str[i] = 32 + (rnd % 95); // ascii #32...#126
				}
				randomValue.setBSTR(str);
			}
			break;
		case VT_DATE:
			{
				DateTime x;
				x.now();
				x.addMicroseconds(rnd * 54763391 * 1000);
				DATE y;
				x.get(y);
				randomValue.setDATE(y);
			}
			break;
		default:
			randomValue.setI1(rnd);
			if (g_syncDisplay.lock(0) == TRUE)
			{
				_tprintf(_T("%s is of datatype %d, no random value could be calculated. I1 random set.\n"), this->getName().c_str(), getDatatype());
				g_syncDisplay.unlock();
			}
		}
		return randomValue;
	}

	long queryProperties(std::vector<DaProperty*>& aPropertyList)
	{
		return EnumResultCode_S_OK;
	}   //  end QueryProperties

	virtual long queryCacheValue(
		DaSession* aSession,
		ValueQT& aCacheValue)
	{
//		::Sleep(5000);

		_tprintf(_T("queryCacheValue for %s: %s\n"), this->getName().c_str(), m_cacheValue.toString().c_str());
		if (getIoMode() == EnumIoMode_POLL_OWNCACHE)
		{
			aCacheValue = m_cacheValue;
		}
		else
		{
			ValueQT pValue;
			DaAddressSpaceElement::getCacheValue(pValue);
			aCacheValue = pValue;
		} // end if .. else

		return EnumResultCode_S_OK;
	} // end queryCacheValue

	void setCachedValue(ValueQT cacheValue)
	{
		m_cacheValue = cacheValue;
	}

	ValueQT getCachedValue()
	{
		return m_cacheValue;
	}

private:
	ValueQT m_cacheValue;
};  //  end class MyAddressSpaceElement

#endif
