#ifndef _OPCSERVER_H_
#define _OPCSERVER_H_

#include "ServerApplication.h"

#include "MyDaAddressSpaceElement.h"
#include "MyRequest.h"
#include "MyTransaction.h"
#include "MyDaAddressSpaceRoot.h"

#include "MyAeAddressSpaceElement.h"

#include "MyCreator.h"
#include "IOManager.h"

using namespace SoftingOPCToolboxServer;


#define TraceGroup_IO_CALL          EnumTraceGroup_USER1
#define TraceGroup_IO_ADVISE        EnumTraceGroup_USER2
#define TraceGroup_CONFIGURATION    EnumTraceGroup_USER4
#define TraceGroup_LICENSE          EnumTraceGroup_USER8

class OpcServer
{
public:

	OpcServer(void);
	virtual ~OpcServer(void);

	long initialize(void);
	long start(void);
	long stop(void);
	long ready(void);
	long terminate(void);

	long prepare(IN MyCreator* pCreator);
	long processCommandLine(IN tstring commandLine);

	long buildAddressSpace(void);

	long buildEventCategories(void);
	void fireSimulationEvents(void);

	void changeSimulationValues(void);

	void setServiceName(tstring serviceName);

	void trace(
		EnumTraceLevel aLevel,
		EnumTraceGroup aMask,
		TCHAR* anObjectID,
		TCHAR* aMessage,
		...);

	bool checkLicense(void);
	bool isLicensed(void)
	{
		return m_license;
	}
	tstring& getLicenseInfo(bool english);
	void updateLicense(void);
	static void CALLBACK updateLicenseTimerProc(HWND hParent, UINT uMsg, UINT uEventID, DWORD dwTimer);

	IOManager* getIOManager(void)
	{
		return m_pIOManager;
	}

private:
	ShutdownRequestHandler m_ShutdownRequest;
	MyDaAddressSpaceElement* m_pDaSimulationElement;
	IOManager* m_pIOManager;

	HANDLE m_serverRunningEvent;

	// licensing
	bool m_license;
	tstring m_licInfoE;
	tstring m_licInfoG;
	DateTime m_startTimeDemoMode;
	UINT_PTR m_licenseUpdateTimer;

	// demo mode
	bool m_shownDemoElapsed;
	HANDLE m_serverDemoEvent;
};  //  end class OpcServer

//	Public OpcServer's handlers
OpcServer* getOpcServer(void);
void createOpcServer(void);
void destroyOpcServer(void);



// NotQuiteNullDacl.h: interface for the NotQuiteNullDacl class.
//
// This class allows the user to simply create a Not-Quite-Null
// Discretionary Access Control List (Dacl).  This Dacl has
// many of the attributes of the Null Dacl that is often used
// to open up access to file and kernel objects but denies the
// user access to the objects security attributes.  Therefore
// it is less subject to abuse than the simple Null Dacl.
//
// This class is based heavily on the concepts and sample code
// found in Chapter 10 of Programming Server-Side Applications
// for Microsoft Windows 2000 by Richter and Clark.  It also
// used ideas and code from the MSDN article "Ask Dr. Gui #49"
#include <aclapi.h>
class NotQuiteNullDacl
{
public:
	// constructor/destructor
	NotQuiteNullDacl();
	virtual ~NotQuiteNullDacl();

	// accessors
	PACL  GetPDACL()
	{
		return m_pDACL;
	};
	DWORD GetErr()
	{
		return m_dwErr;
	}

	// the real work gets done here
	bool Create();

private:
	bool CreateEveryoneSid();
	ULONG CalculateAclSize();
	bool AddNotQuiteNullAces();

private:
	PACL                    m_pDACL;
	PSID                    m_pEveryoneSid;
	DWORD                   m_dwErr;
};

#endif //   _OPCSERVER_H_

