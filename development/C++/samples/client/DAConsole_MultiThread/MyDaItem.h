#ifndef _MYDAITEM_H_
#define _MYDAITEM_H_

#ifdef TBC_OS_WINDOWS
#include "Da\ClientDaItem.h"
#include "ValueQT.h"
#endif
#include "Mutex.h"

extern unsigned short g_itemDataType;
extern const unsigned short g_minStringLength;
extern const unsigned short g_maxStringLength;
extern bool g_tracesOn;
extern Mutex g_syncDisplay;
extern const unsigned int g_silentDisplayCount;

#ifdef TBC_OS_LINUX
#include "ClientDaItem.h"
#endif

class MyDaItem :
	public SoftingOPCToolboxClient::DaItem
{
private:
	unsigned short m_dataType;
	std::string m_itemId;
	DWORD m_itemNo;
public:

	MyDaItem(DWORD itemNo, tstring itemId,
		SoftingOPCToolboxClient::DaSubscription* parentSubscription,
		unsigned short dataType)
		: DaItem(itemId, parentSubscription)
	{
		m_itemNo = itemNo;
		m_dataType = dataType;
		m_itemId = itemId;
	}

	DWORD getItemNo()
	{
		return m_itemNo;
	}

	VARTYPE getDataType()
	{
		return m_dataType;
	}

	std::string getItemId()
	{
		return m_itemId;
	}

	virtual ~MyDaItem()
	{} // end dtor

	Variant getRandomWriteValue(int rnd)
	{
		Variant data;
		switch (m_dataType)
		{
		case VT_I1:
			data.setI1(rnd);
			break;
		case VT_I2:
			data.setI2(rnd * 13 / 7);
			break;
		case VT_I4:
			data.setI4(rnd * 769 / 7);
			break;
		case VT_I8:
			data.setI8(rnd * 7919 / 7);
			break;
		case VT_UI1:
			data.setUI1(rnd);
			break;
		case VT_UI2:
			data.setUI2(rnd * 13 / 7);
			break;
		case VT_UI4:
			data.setUI4(rnd * 769 / 7);
			break;
		case VT_UI8:
			data.setUI8(rnd * 7919 / 7);
			break;
		case VT_BOOL:
			data.setBOOL((rnd % 2) == 1);
			break;
		case VT_R4:
			data.setR4(((float)(rnd * 3701)) / 7);
			break;
		case VT_R8:
			data.setR8(((double)(rnd * 7187)) / 7);
			break;
		case VT_BSTR:
			{
				TCHAR str[256] = {0};
				int len = g_minStringLength + (rnd % (g_maxStringLength - g_minStringLength));
				for (int i = 0; i < len; ++i)
				{
					str[i] = 32 + (rnd % 95); // ascii #32...#126
					++rnd;
				}
				data.setBSTR(str);
			}
			break;
		case VT_DATE:
			data.setDATE(rnd * 63391 / 7);
			break;
		default:
			data.setI1(rnd);
			if ((g_tracesOn) && (g_syncDisplay.lock(0)))
			{
				_tprintf(_T("%s is of datatype %d, no random value could be calculated. I1 random set.\n"), this->getId().c_str(), m_dataType);
				g_syncDisplay.unlock();
			}
		}
		return data;
	}

	void handleStateChangeCompleted(
		EnumObjectState state)
	{
		if (g_tracesOn == true)
		{
			if (g_syncDisplay.lock(0))
			{
				tstring stateToString;

				switch (state)
				{
				case EnumObjectState_DISCONNECTED:
					stateToString = _T("DISCONNECTED");
					break;

				case EnumObjectState_CONNECTED:
					stateToString = _T("CONNECTED");
					break;

				case EnumObjectState_ACTIVATED:
					stateToString = _T("ACTIVATED");
				} //end switch

				_tprintf(_T("%s state changed - %s\n"), this->getId().c_str(), stateToString.c_str());
				g_syncDisplay.unlock();
			}
		}
		else if (m_itemNo % g_silentDisplayCount == 0)
		{
			if (g_syncDisplay.lock(0))
			{
				_tprintf(_T("itm%dStChCompl "), m_itemNo);
				g_syncDisplay.unlock();
			}
		}
	} //end handleStateChange


	void handlePerformStateTransitionCompleted(
		unsigned long executionContext,
		long result)
	{
		if (SUCCEEDED(result))
		{
			if (g_tracesOn == true)
			{
				if (g_syncDisplay.lock(0))
				{
					_tprintf(_T("\n"));
					_tprintf(_T("%s performed state transition - context: %lu\n"), this->getId().c_str(), executionContext);
					g_syncDisplay.unlock();
				} //end if
			}
			else if ((result != S_OK) || (m_itemNo % g_silentDisplayCount == 0))
			{
				if (g_syncDisplay.lock(0))
				{
					_tprintf(_T("itm%dPerStTrCompl(%d) "), m_itemNo, result);
					g_syncDisplay.unlock();
				}
			}
		}
		else
		{
			if (g_syncDisplay.lock(0))
			{
				if (g_tracesOn == true)
				{
					_tprintf(_T("%s performed state transition failed - [0x%8.8X]\n"), this->getId().c_str(), result);
				}
				else
				{
					_tprintf(_T("itm%dPerStTrCompl(%d) "), m_itemNo, result);
				}
				g_syncDisplay.unlock();
			}
		}
	} //handlePerformStateTransitionCompleted

	void handleGetAttributesFromServerCompleted(unsigned long executionContext, std::vector<EnumObjectAttribute>& whatAttributes, std::vector<long>& results, long result)
	{
		if (SUCCEEDED(result))
		{
			if (g_tracesOn == true)
			{
				if (g_syncDisplay.lock(0))
				{
					_tprintf(_T("\n"));
					_tprintf(_T("%s get server attributes completed - context: %lu, count: %d, properties:\n"), this->getId().c_str(), executionContext, whatAttributes.size());
					std::vector<EnumObjectAttribute>::const_iterator it = whatAttributes.begin();
					for (; it != whatAttributes.end(); ++it)
					{
						_tprintf(_T("%s "), *it, executionContext);
					}
					_tprintf(_T("\n"));
					g_syncDisplay.unlock();
				} //end if
			}
			else if ((result != S_OK) || (m_itemNo % g_silentDisplayCount == 0))
			{
				if (g_syncDisplay.lock(0))
				{
					_tprintf(_T("itm%dGetAtrFrSrvCompl(%d) "), m_itemNo, result);
					g_syncDisplay.unlock();
				}
			}
		}
		else
		{
			if (g_syncDisplay.lock(0))
			{
				if (g_tracesOn == true)
				{
					_tprintf(_T("%s get server attributes failed - [0x%8.8X]\n"), this->getId().c_str(), result);
				}
				else
				{
					_tprintf(_T("itm%dGetAtrFrSrvCompl(%d) "), m_itemNo, result);
				}
				g_syncDisplay.unlock();
			}
		}
	}

};  // end class MyDaItem

#endif
