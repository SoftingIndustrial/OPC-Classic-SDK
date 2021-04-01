#ifndef _MYDASESSION_H_
#define _MYDASESSION_H_

#include "ClientApplication.h"

#ifdef TBC_OS_WINDOWS
#include "Da\ClientDaGetPropertiesOptions.h"
#include "Da\ClientDaProperty.h"
#include "Da\ClientDaSession.h"
#endif

#ifdef TBC_OS_LINUX
#include "ClientDaGetPropertiesOptions.h"
#include "ClientDaProperty.h"
#include "ClientDaSession.h"
#endif



using namespace SoftingOPCToolboxClient;


class MyDaSession :
	public SoftingOPCToolboxClient::DaSession
{
public:

	MyDaSession(const tstring& url) : DaSession(url) {}

	unsigned char handleShutdownRequest(const tstring& reason)
	{
		_tprintf(_T("Session shutdown - reason: %s\n"), reason.c_str());
		return TRUE; // reconnect automatically
	}

	void handleGetDaPropertiesCompleted(
		unsigned long executionContext,
		unsigned long addressSpaceElementHandle,
		tstring& addressSpaceElementId,
		tstring& addressSpaceElementPath,
		std::vector<DaProperty*>& someDaProperties,
		long result)
	{
		unsigned long i;

		if (SUCCEEDED(result))
		{
			size_t count = someDaProperties.size();

			for (i = 0; i < count; i++)
			{
				_tprintf(_T("%s%-30.30s\n"), _T("Property Name: "), someDaProperties[i]->getName().c_str());
				_tprintf(_T("%s%5.5d\n"), _T("Property Id: "), (int)someDaProperties[i]->getId());
				_tprintf(_T("%s%-30.30s\n"), _T("Property Item Id: "), someDaProperties[i]->getItemId().c_str());
				_tprintf(_T("%s%5.5d\n"), _T("Property DataType: "), someDaProperties[i]->getDataType());
				_tprintf(_T("%s%-30.30s\n"), _T("Property Description: "), someDaProperties[i]->getDescription().c_str());
				_tprintf(_T("%s%s\n"), _T("Property Value: "), someDaProperties[i]->getValueQT().toString().c_str());
				_tprintf(_T("\n"));
			} //end for
		} //end if
		else
		{
			_tprintf(_T(" Failed to asynchronously get properties of address space element: %s - 0x%08X\n"), addressSpaceElementId.c_str(), result);
		}//end if ... else
	} //end handleGetDaPropertiesCompleted

	virtual ~MyDaSession()
	{} // end dtor

}; // end class MyDaSession

#endif
