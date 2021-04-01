#ifndef _MYDASESSION_H_
#define _MYDASESSION_H_

#include "ClientServerStatus.h"

#ifdef TBC_OS_WINDOWS
#include "Da\ClientDaSession.h"
#endif

#ifdef TBC_OS_LINUX
#include "ClientDaSession.h"
#endif


class MyDaSession :
	public SoftingOPCToolboxClient::DaSession
{
public:

	MyDaSession(const tstring& url): DaSession(url) {}

	unsigned char handleShutdownRequest(const tstring& reason)
	{
		_tprintf(_T("Session shutdown - reason: %s\n"), reason.c_str());
		return TRUE; // reconnect automatically
	}

	void handleStateChangeCompleted(EnumObjectState state)
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

		_tprintf(_T("\n%s state changed - %s\n"), _T("Session"), stateToString.c_str());
	} //end handleStateChange

	void handlePerformStateTransitionCompleted(
		unsigned long executionContext,
		long result)
	{
		_tprintf(_T("\n"));

		if (SUCCEEDED(result))
		{
			_tprintf(_T("%s performed state transition - context: %lu\n"), _T("Session"), executionContext);
		} //end if
		else
		{
			_tprintf(_T(" performed state transition failed"));
		}
	} //end handlePerformStateTransitionCompleted


	void handleReadCompleted(
		unsigned long executionContext,
		const std::vector<tstring>& itemIds,
		const std::vector<tstring>& itemPaths,
		const std::vector<ValueQT*>& values,
		const std::vector<long>& results,
		long result)
	{
		unsigned long i;
		size_t itemCount = itemIds.size();
		_tprintf(_T("\nSession - Read complete\n"));

		if (SUCCEEDED(result))
		{
			for (i = 0; i < itemCount; i++)
			{
				if (SUCCEEDED(results[i]))
				{
					_tprintf(_T("%-23.23s - %s\n"), itemIds[i].c_str(), values[i]->toString().c_str());
				}
				else
				{
					_tprintf(_T("Read failed - %s [0x%8.8X]\n"), itemIds[i].c_str(), (unsigned int)results[i]);
				}//end if...else
			}//end for
		}
		else
		{
			_tprintf(_T("Read failed [0x%8.8X]\n"), (unsigned int)result);
		}//end if...else
	} //end handleReadCompleted


	void handleWriteCompleted(
		unsigned long executionContext,
		const std::vector<tstring>& itemIds,
		const std::vector<tstring>& itemPaths,
		const std::vector<ValueQT*>& values,
		const std::vector<long>& results,
		long result)
	{
		_tprintf(_T("\nSession write complete\n"));

		if (SUCCEEDED(result))
		{
			unsigned long i;
			size_t itemCount = itemIds.size();

			for (i = 0; i < itemCount; i++)
			{
				if (SUCCEEDED(results[i]))
				{
					_tprintf(_T("%-23.23s - %s\n"), itemIds[i].c_str(), values[i]->toString().c_str());
				}
				else
				{
					_tprintf(_T("Write failed - %s [0x%8.8X]\n"), itemIds[i].c_str(), (unsigned int)results[i]);
				} //end if ... else
			} //end for
		}
		else
		{
			_tprintf(_T("Write failed [0x%8.8X]\n"), (unsigned int)result);
		} //end if... else
	} //end handleWriteCompleted

	void handleLogonCompleted(
		unsigned long executionContext,
		tstring& userName,
		tstring& password,
		long result)
	{
		_tprintf(_T("\nLogon for secure communication\n"));

		if (SUCCEEDED(result))
		{
			_tprintf(_T(" - successfully logon for user: %s.\n"), userName.c_str());
		}
		else
		{
			_tprintf(_T(" - failed logon for user: %s, [0x%8.8X]\n"), userName.c_str(), (unsigned int)result);
		} //end if... else
	} // end handleLogonCompleted

	void handleLogoffCompleted(
		unsigned long executionContext,
		long result)
	{
		_tprintf(_T("\nLogoff for secure communication\n"));

		if (SUCCEEDED(result))
		{
			_tprintf(_T(" - successfully logoff.\n"));
		}
		else
		{
			_tprintf(_T(" - failed logoff. [0x%8.8X]\n"), (unsigned int)result);
		} //end if... else
	} // end handleLogoffCompleted

	void handleGetStatusCompleted(
		unsigned long executionContext,
		ServerStatus& aServerStatus,
		long result)
	{
		if (SUCCEEDED(result))
		{
			unsigned long i;
			_tprintf(_T("\nServer Status\n"));
			_tprintf(_T("   State: %ld\n"), aServerStatus.getState());
			_tprintf(_T("   Vendor info: %s\n"), aServerStatus.getVendorInfo().c_str());
			_tprintf(_T("   Product version: %s\n"), aServerStatus.getProductVersion().c_str());
			_tprintf(_T("   Start time: %s\n"), aServerStatus.getStartTime().toString().c_str());
			_tprintf(_T("   Last update time: %s\n"), aServerStatus.getLastUpdateTime().toString().c_str());
			_tprintf(_T("   Current time: %s\n"), aServerStatus.getCurrentTime().toString().c_str());
			_tprintf(_T("   Group count: %ld\n"), aServerStatus.getGroupCount());
			_tprintf(_T("   Bandwidth: %ld\n"), aServerStatus.getBandwidth());
			std::vector<tstring>supportedLcIds = aServerStatus.getSupportedLcIds();

			for (i = 0; i < supportedLcIds.size(); i++)
			{
				_tprintf(_T("   Supported LCID: %s\n"), supportedLcIds[i].c_str());
			}

			_tprintf(_T("   Status info: %s\n"), aServerStatus.getStatusInfo().c_str());
		}
		else
		{
			_tprintf(_T("\nGet Status failed [0x%8.8X]\n"), (unsigned int)result);
		} //end else ... if
	} //end handleGetStatusCompleted

	virtual ~MyDaSession()
	{} // end dtor

}; // end class MyDaSession

#endif
