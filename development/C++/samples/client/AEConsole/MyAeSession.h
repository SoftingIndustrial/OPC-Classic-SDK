#ifndef _MYAESESSION_H_
#define _MYAESESSION_H_

#include "Ae\ClientAeSession.h"
#include "ClientServerStatus.h"

class MyAeSession :
	public SoftingOPCToolboxClient::AeSession
{

public:

	MyAeSession(const tstring& url) :
		SoftingOPCToolboxClient::AeSession(url) {}

	virtual ~MyAeSession()
	{} // dtor

	BYTE handleShutdownRequest(const tstring& reason)
	{
		_tprintf(_T("Session shutdown - reason: %s\n"), reason.c_str());
		return TRUE; // reconnect automatically
	}

	void handleStateChangeCompleted(
		EnumObjectState state)
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


	void handleGetStatusCompleted(
		unsigned long executionContext,
		ServerStatus& aServerStatus,
		long result)
	{
		if (SUCCEEDED(result))
		{
			unsigned long i;
			_tprintf(_T("\nServer Status\n"));
			_tprintf(_T("   State: %d\n"), aServerStatus.getState());
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
			_tprintf(_T("\nGet Status failed [0x%8.8X]\n"), result);
		} //end else ... if
	} //end handleGetStatusCompleted


}; // end MySession

#endif
