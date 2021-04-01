#include "stdafx.h"
#include "OpcServer.h"
#include "ServerCommon.h"
// version info
#include "OPCVerProd.h"
#include "BuildNr.h"

OpcServer* instance = NULL;
SoftingOPCToolboxServer::Application* SoftingOPCToolboxServer::Application::m_instance = NULL;

#define RUNNING_EVENT_NAME  _T("Global\\Softing_XX_RunEvent")
#define DEMO_EVENT_NAME     _T("Global\\Softing_XX_DemoEvent")

const unsigned long CATEGORY_ID_SIMULATION  = 1;
const unsigned long ATTRIBUTE_ID_SIMULATION = 1;

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


long API_CALL HandleShutdownRequest(void)
{
	return S_OK;
}   //end HandleShutdownRequest


OpcServer::OpcServer(void)
{
	m_pIOManager = new IOManager();
	m_pDaSimulationElement = NULL;
	m_license = false;
	m_shownDemoElapsed = false;
	m_serverRunningEvent = NULL;
	m_serverDemoEvent = NULL;
	m_licenseUpdateTimer = 0;
}   //  end constructor


OpcServer::~OpcServer(void)
{
	if (m_pIOManager)
	{
		delete m_pIOManager;
	}
}   //  end destructor


long OpcServer::initialize(void)
{
	getApp()->setVersionOtb(447);
	getApp()->setAppType(EnumApplicationType_EXECUTABLE);
	// TODO: change DA CLSID for each new Softing server
	getApp()->setClsidDa(_T("{D3F17B86-8312-4446-A61E-19518789DF2B}"));
	// TODO: replace XXX with the server name (e.g. S7, PROFIBUS, FF)
	getApp()->setProgIdDa(_T("Softing.OPC.XX.DA"));
	getApp()->setVerIndProgIdDa(_T("Softing.OPC.XX.DA.1"));
	// TODO: change AE CLSID for each new Softing server
	getApp()->setClsidAe(_T("{9DF71F76-9F07-4928-805E-561041AE5EA8}"));
	// TODO: replace XXX with the server name (e.g. S7, PB, FF)
	getApp()->setProgIdAe(_T("Softing.OPC.XX.AE"));
	getApp()->setVerIndProgIdAe(_T("Softing.OPC.XX.AE.1"));
	// TODO: replace XXX with the server name (e.g. S7, PROFIBUS, FF)
	getApp()->setDescription(_T("Softing XX OPC Server"));
	getApp()->setMajorVersion(PROD_VER_MAJOR);
	getApp()->setMinorVersion(PROD_VER_MINOR);
	getApp()->setPatchVersion(PROD_VER_SERVP);
	getApp()->setBuildNumber(PRODUCT_BUILD);
	getApp()->setVendorInfo(_T("Softing Industrial Automation GmbH"));
	// TODO: check for the correct minimal update rate
	getApp()->setMinUpdateRateDa(50);
	getApp()->setClientCheckPeriod(30000);
	getApp()->setAddressSpaceDelimiter(_T('.'));
	getApp()->setPropertyDelimiter(_T('|'));
	// TODO: change the HTTP IP port to the one assigned for the new server
	getApp()->setIpPortHTTP(8099);
	getApp()->setUrlDa(_T("/OPC/DA"));
	// TODO: change the TP IP port 0 which is the default for all Softing servers
	getApp()->setTpPort(55678);
	// TODO: get the trace settings from the configuration
	getApp()->enableTracing(
		EnumTraceGroup_ALL,
		EnumTraceGroup_ALL,
		EnumTraceGroup_NOTHING,
		EnumTraceGroup_NOTHING,
		_T("Trace.txt"),
		1000000,
		2);
	// TODO: get TP credentials from the configuration
	/*
	    std::vector<tstring> tpUsers;
	    std::vector<tstring> tpPaswords;
	    tpUsers.push_back(_T("Softing"));
	    tpPasswords.push_back(_T("Softing"));
	    getApp()->addTpCredentials(tpUsers, tpPasswords);
	*/
	getApp()->setSupportDisableConditions(true);
	return S_OK;
}   //  end initialize


void OpcServer::setServiceName(tstring serviceName)
{
	getApp()->setServiceName(serviceName);
}   //  end setServiceName


long OpcServer::prepare(MyCreator* creator)
{
	long result = getApp()->initialize(creator);

	checkLicense();

	if (!isLicensed())
	{
		// no license -> 90 minutes demo version
		m_startTimeDemoMode.now();
		getApp()->forceDemoVersion();
	}

	m_licenseUpdateTimer = SetTimer(NULL, 0, 900010, OpcServer::updateLicenseTimerProc); // update license every 15 minutes
	return result;
}   //  end prepare

long OpcServer::start(void)
{
	long ret;
	ret = getApp()->start();

	if (SUCCEEDED(ret))
	{
		// allow everyone to access the event
		SECURITY_DESCRIPTOR sd;
		InitializeSecurityDescriptor(&sd, SECURITY_DESCRIPTOR_REVISION);
		NotQuiteNullDacl Dacl;

		if (Dacl.Create())
		{
			SetSecurityDescriptorDacl(&sd, TRUE, Dacl.GetPDACL(), FALSE);
		}
		else
		{
			SetSecurityDescriptorDacl(&sd, TRUE, 0, FALSE);
		}

		SECURITY_ATTRIBUTES sa = {sizeof sa, &sd, FALSE};
		// create running event
		m_serverRunningEvent = CreateEvent(&sa, FALSE, FALSE, RUNNING_EVENT_NAME);
	}

	return ret;
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
	long result = getApp()->terminate();
	releaseApp();

	if (m_serverRunningEvent)
	{
		CloseHandle(m_serverRunningEvent);
		m_serverRunningEvent = NULL;
	}

	if (m_serverDemoEvent)
	{
		CloseHandle(m_serverDemoEvent);
		m_serverDemoEvent = NULL;
	}

	if (m_licenseUpdateTimer)
	{
		KillTimer(NULL, m_licenseUpdateTimer);
	}

	return result;
}   //  end terminate

long OpcServer::processCommandLine(tstring command)
{
	return getApp()->processCommandLine(command);
}   //  end processCommandLine


long OpcServer::buildAddressSpace(void)
{
	// TODO: replace address space creation
	MyCreator* creator = (MyCreator*)getApp()->getCreator();
	tstring aName;
	//  DA
	DaAddressSpaceRoot* daRoot = getApp()->getDaAddressSpaceRoot();
	m_pDaSimulationElement = (MyDaAddressSpaceElement*)creator->createMyDaAddressSpaceElement();
	aName = tstring(_T("Simulation"));
	m_pDaSimulationElement->setName(aName);
	m_pDaSimulationElement->setAccessRights(EnumAccessRights_READWRITEABLE);
	m_pDaSimulationElement->setDatatype(VT_I4);
	m_pDaSimulationElement->setIoMode(EnumIoMode_REPORT_CYCLIC);
	daRoot->addChild(m_pDaSimulationElement);
	DateTime now;
	Variant aVariant(rand());
	ValueQT value(aVariant, EnumQuality_GOOD, now);
	m_pDaSimulationElement->valueChanged(value);
	//  AE
	AeAddressSpaceRoot* aeRoot = getApp()->getAeAddressSpaceRoot();
	AeAddressSpaceElement* aeElement = creator->createAeAddressSpaceElement();
	aName = tstring(_T("SimulationElement"));
	aeElement->setName(aName);
	aeElement->setHasChildren(FALSE);
	aeRoot->addChild(aeElement);
	return S_OK;
}   //  end buildAddressSpace

long OpcServer::buildEventCategories(void)
{
	// TODO: replace event categories creation
	tstring categoryName(_T("SimulationCategory"));
	AeCategory* simulationCategory = new AeCategory(EnumEventType_SIMPLE, CATEGORY_ID_SIMULATION, categoryName);
	getApp()->addAeCategory(simulationCategory);
	tstring attributeName(_T("SimulationAttribute"));
	simulationCategory->addAttribute(ATTRIBUTE_ID_SIMULATION, attributeName, VT_UI2);
	return S_OK;
}   //  end buildEventCategories


// TODO: remove simulation which starts here >>>

void OpcServer::fireSimulationEvents(void)
{
	// simple events
	std::vector<AeEvent*> events;
	std::vector<Variant> attributeValues;
	tstring message;
	tstring anActorID;
	AeEvent simulationEvent(EnumEventType_SIMPLE, tstring(_T("SimulationElement")), CATEGORY_ID_SIMULATION);
	simulationEvent.setSeverity(10);
	anActorID = tstring(_T("OpcUser"));
	simulationEvent.setActorId(anActorID);
	message = tstring(_T("Simulation event generated"));
	simulationEvent.setMessage(message);
	Variant randomValue(::rand());
	attributeValues.push_back(randomValue);
	simulationEvent.setAttributeValueList(attributeValues);
	events.push_back(&simulationEvent);
	getApp()->fireEvents(events);
}   //  end fireEvents


void OpcServer::changeSimulationValues(void)
{
	if (m_pDaSimulationElement != NULL)
	{
		DateTime now;
		Variant aVariant(::rand());
		ValueQT value(aVariant, EnumQuality_GOOD, now);
		m_pDaSimulationElement->valueChanged(value);
	}
}   //  end changeSimulationValues

// TODO: remove simulation which ends here <<<

void OpcServer::trace(
	EnumTraceLevel aLevel,
	EnumTraceGroup aMask,
	TCHAR* anObjectID,
	TCHAR* aMessage,
	...)
{
	getApp()->trace(aLevel, aMask, anObjectID, aMessage);
}   //  end trace


#define SLIC_OK     0
#define SPROD_XX_OPC_SERVER 0
typedef long(_stdcall _SLIC_GetLicenseForProduct)(long prodNum, long* pFeatureBits, long* pHoursLeft, long* pLicMedia);

bool OpcServer::checkLicense(void)
{
	// check license
	static HMODULE licDLLInstance;

	if (licDLLInstance == NULL)
	{
		licDLLInstance = LoadLibrary(_T("fc_slbase20.dll"));
	}

	if (licDLLInstance == NULL)
	{
		m_license = false;
		m_licInfoE = _T("ERROR: Unable to load license DLL.");
		m_licInfoG = _T("ERROR: Lizenz-DLL konnte nicht geladen werden.");
		trace(EnumTraceLevel_ERR, TraceGroup_LICENSE, _T("LICENSE"), (_T("Unable to load license DLL.")));
		return m_license;
	}

	static _SLIC_GetLicenseForProduct* SLIC_GetLicenseForProduct;

	if (SLIC_GetLicenseForProduct == NULL)
	{
		SLIC_GetLicenseForProduct = (_SLIC_GetLicenseForProduct*)GetProcAddress(licDLLInstance, "SLIC_GetLicenseForProduct");
	}

	if (SLIC_GetLicenseForProduct == NULL)
	{
		m_license = false;
		m_licInfoE = _T("ERROR: Unable to locate entry point in license DLL.");
		m_licInfoG = _T("ERROR: Einsprungspunkt in Lizenz-DLL konnte nicht gefunden werden.");
		trace(EnumTraceLevel_ERR, TraceGroup_LICENSE, _T("LICENSE"), (_T("Unable to locate entry point in license DLL.")));
		return m_license;
	}

	LONG featureBits = 0;
	LONG hoursLeft = 0;
	LONG licMedia = 0;
	LONG lic = SLIC_GetLicenseForProduct(SPROD_XX_OPC_SERVER, &featureBits, &hoursLeft, &licMedia);

	if (lic == SLIC_OK)
	{
		// we have a valid temporary license or a full license
		m_license = true;

		if (hoursLeft == -1)
		{
			// full license
			// SLIC_MEDIA_SOFT = 1
			// SLIC_MEDIA_DONGLE= 2
			if (licMedia == 1)
			{
				m_licInfoE = _T("Full License - Softkey");
				m_licInfoG = _T("Volllizenz");
				trace(EnumTraceLevel_INF, TraceGroup_LICENSE, _T("LICENSE"), (_T("Full License - Softkey")));
			}
			else if (licMedia == 2)
			{
				m_licInfoE = _T("Full License - Dongle");
				m_licInfoG = _T("Volllizenz - Dongle");
				trace(EnumTraceLevel_INF, TraceGroup_LICENSE, _T("LICENSE"), (_T("Full license - Dongle")));
			}
			else
			{
				m_licInfoE = _T("Full license - Interface");
				m_licInfoG = _T("Volllizenz - Interface");
				trace(EnumTraceLevel_INF, TraceGroup_LICENSE, _T("LICENSE"), (_T("Full license - Interface")));
			}
		}
		else
		{
			tstringstream tss;
			tss << _T("Temporary License - ") << hoursLeft / 24 << _T("day(s) and ") << hoursLeft % 24 << _T("hour(s) left");
			m_licInfoE = tss.str();
			tss << _T("Zeitbeschränkte Lizenz - ") << hoursLeft / 24 << _T("Tag(e) und ") << hoursLeft % 24 << _T("Stunde(n) übrig");
			m_licInfoG = tss.str();
			tss << _T("Temporary license - ") << hoursLeft << _T("hours left");
			trace(EnumTraceLevel_INF, TraceGroup_LICENSE, _T("LICENSE"), (TCHAR*)tss.str().c_str());
		}

		return m_license;
	} // if (lic == SLIC_OK)

	m_license = false;
	m_licInfoE = _T("Demo License");
	m_licInfoG = _T("Demolizenz");

	//"normal" errors:
	// #define SLIC_ERR_NO_COMP_FOUND   20
	// #define SLIC_ERR_NO_LIC_FOUND    21
	// #define SLIC_ERR_TMPLIC_EXPIRED  22
	switch (lic)
	{
	case 20:
		trace(EnumTraceLevel_ERR, TraceGroup_LICENSE, _T("LICENSE"), (_T("Component not found")));
		break;

	case 21:
		trace(EnumTraceLevel_WRN, TraceGroup_LICENSE, _T("LICENSE"), (_T("No license found")));
		break;

	case 22:
		m_licInfoE = _T("Demo License - Temporary License Expired");
		m_licInfoG = _T("Demolizenz - zeitbeschränkte Lizenz abgelaufen");
		trace(EnumTraceLevel_WRN, TraceGroup_LICENSE, _T("LICENSE"), (_T("Demo License - Temporary License Expired")));
		break;

	default:
	{
		tstringstream tss;
		tss << _T("Fatal Error (%d)") << lic;
		trace(EnumTraceLevel_ERR, TraceGroup_LICENSE, _T("LICENSE"), (TCHAR*)tss.str().c_str());
		break;
	}
	}

	return m_license;
}

tstring& OpcServer::getLicenseInfo(bool english)
{
	if (english)
	{
		return m_licInfoE;
	}
	else
	{
		return m_licInfoG;
	}
}


void OpcServer::updateLicenseTimerProc(HWND hParent, UINT uMsg, UINT uEventID, DWORD dwTimer)
{
	getOpcServer()->updateLicense();
}

void OpcServer::updateLicense(void)
{
	if (isLicensed())
	{
		checkLicense();

		if (isLicensed())
		{
			// server licensed
			return;
		}
		else if ((!isLicensed()))
		{
			// license was removed -> force demo mode
			m_startTimeDemoMode.now();
			getApp()->forceDemoVersion();
		}
	}

	if (!isLicensed())
	{
		// no license -> demo mode
		DateTime now;
		now.now();
		unsigned long long timeElapsed = now.getMicroseconds() - m_startTimeDemoMode.getMicroseconds();
		bool elapsed;
		elapsed = (timeElapsed > 5400000000); // 90 minutes
		tstring serviceName = getApp()->getServiceName();

		if ((!m_shownDemoElapsed) && (elapsed) && (serviceName.size() == 0))
		{
			// show elapsed message
			m_shownDemoElapsed = true;
			MessageBox(NULL, _T("The XX OPC Server was running under the demo license\nThe demo runtime of 90 minutes is elapsed."), _T("S7/S5 OPC Server: Demo license"), MB_ICONSTOP | MB_OK  | MB_TOPMOST | MB_SETFOREGROUND);
		}

		if ((elapsed) && (m_serverDemoEvent == 0))
		{
			// allow everyone to access the event
			SECURITY_DESCRIPTOR sd;
			InitializeSecurityDescriptor(&sd, SECURITY_DESCRIPTOR_REVISION);
			NotQuiteNullDacl Dacl;

			if (Dacl.Create())
			{
				SetSecurityDescriptorDacl(&sd, TRUE, Dacl.GetPDACL(), FALSE);
			}
			else
			{
				SetSecurityDescriptorDacl(&sd, TRUE, 0, FALSE);
			}

			SECURITY_ATTRIBUTES sa = {sizeof sa, &sd, FALSE};
			m_serverDemoEvent = CreateEvent(&sa, TRUE, TRUE, DEMO_EVENT_NAME);
		}
	}

	return;
}




//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
NotQuiteNullDacl::NotQuiteNullDacl()
{
	m_pEveryoneSid = NULL;
	m_pDACL = NULL;
	m_dwErr = 0;
}

NotQuiteNullDacl::~NotQuiteNullDacl()
{
	if (m_pDACL != NULL)
	{
		::HeapFree(GetProcessHeap(), 0, m_pDACL);
		m_pDACL = NULL;
	}

	if (m_pEveryoneSid != NULL)
	{
		::FreeSid(m_pEveryoneSid);
		m_pEveryoneSid = NULL;
	}
}


//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
// Public methods
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

//********************************************************************
// Create -- All the real work of creating the DACL gets done here
// rather than in the constructor, so we can return a
// success/failure status
//
bool NotQuiteNullDacl::Create()
{
	bool bCreateOk = false;
	m_dwErr = 0;
	// need a SID for the Everyone group
	bool bEveryoneSID = CreateEveryoneSid();

	if (bEveryoneSID)
	{
		// calculate a size for the DACL we're going to use
		ULONG lACLSize = CalculateAclSize();

		if (lACLSize > 0)
		{
			// Allocate memory for the ACL
			m_pDACL = (PACL)::HeapAlloc(GetProcessHeap(), 0, lACLSize);

			if (m_pDACL != NULL)
			{
				// initialize the actual ACL
				BOOL bInitACL = ::InitializeAcl(m_pDACL, lACLSize, ACL_REVISION);

				if (bInitACL)
				{
					// add the ACEs that deny/grant the accesses we're after
					bCreateOk = AddNotQuiteNullAces();
				}
				else
				{
					m_dwErr = ::GetLastError();
				}
			}
			else
			{
				m_dwErr = ::GetLastError();
			}
		}
	}

	return bCreateOk;
}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
// Private methods
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

//********************************************************************
// CreateEveryoneSid -- Create a SID for the built-in "Everyone" group
//
bool NotQuiteNullDacl::CreateEveryoneSid()
{
	bool bSidOk = false;
	m_dwErr = 0;
	SID_IDENTIFIER_AUTHORITY SidAuth = SECURITY_WORLD_SID_AUTHORITY;
	BOOL bAllocAndInitOk = ::AllocateAndInitializeSid(&SidAuth,             // authority
													  1,                   // count of subauthorities used
													  SECURITY_WORLD_RID,  // subauthority we're using
													  0, 0, 0, 0, 0, 0, 0, // seven unused subauthorities
													  &m_pEveryoneSid);    // pointer to receive the new SID

	if (bAllocAndInitOk)
	{
		bSidOk = true;
	}
	else
	{
		m_dwErr = ::GetLastError();
	}

	return bSidOk;
}


//********************************************************************
// CalculateAclSize -- This method calculates the size of the ACL we
// are about to create.
// NB:  This is just a special-purpose version based on
// "Ask Dr. Gui #49".  A more general method is
// outlined in Richter/Clark, Chapter 10, page 455.  This is
// sufficient for the purpose of creating our NotQuiteNullDacl but
// not for creating more general ACLs.
//
ULONG NotQuiteNullDacl::CalculateAclSize()
{
	ULONG lACLSize = 0;
	m_dwErr = 0;
	// validate that SID we created, just to be sure
	BOOL bSIDOk = ::IsValidSid(m_pEveryoneSid);

	if (bSIDOk)
	{
		// we're using that SID twice
		lACLSize += (::GetLengthSid(m_pEveryoneSid) * 2);
		// we're using one access allowed ace and one access denied
		// we substract the length of the sidstart member which
		// is already counted in the sid lengths above
		// (See the Initialize ACL documentation for more info.)
		lACLSize += (sizeof(ACCESS_ALLOWED_ACE) - sizeof(((ACCESS_ALLOWED_ACE*)0)->SidStart));
		lACLSize += (sizeof(ACCESS_DENIED_ACE) - sizeof(((ACCESS_DENIED_ACE*)0)->SidStart));
		// and we need to add in the size of the acl itself
		lACLSize += sizeof(ACL);
	}

	// else -- there's no extended error info to get
	return lACLSize;
}


//********************************************************************
// AddNotQuiteNullAces -- Add the two ACEs to the DACL
// NB:  Note that the order of execution is critical here.
//
bool NotQuiteNullDacl::AddNotQuiteNullAces()
{
	bool bAddedOk = false;
	m_dwErr = 0;
	BOOL bAddAce;
	// Add an ace that denies WRITE_OWNER and WRITE_DAC to 'everyone'
	// which denies everyone but the owner of the associated object
	// access to the object's security.
	bAddAce = ::AddAccessDeniedAce(m_pDACL,                    // the acl to add the ace to
								   ACL_REVISION,              // revision, must be ACL_REVISION
								   WRITE_OWNER | WRITE_DAC,   // accesses to deny
								   m_pEveryoneSid);           // SID to be denied (everyone)

	if (bAddAce)
	{
		// Add an ace that gives 'everyone' all accesses to the object.
		// By itself, the bit of code below would be the moral equivalent
		// of a NULL DACL -- it gives all rights to everyone.  But, because
		// accesses are evaluating in order of their placement in the DACL
		// the DeniedACE above acts as a filter, before this can be evaluated.
		bAddAce = ::AddAccessAllowedAce(m_pDACL,                    // the acl to add the ace to
										ACL_REVISION,              // revision, must be ACL_REVISION
										GENERIC_ALL,               // accesses to allow
										m_pEveryoneSid);           // SID to be allowed

		if (bAddAce)
		{
			bAddedOk = true;
		}
	}

	if (!bAddedOk)
	{
		m_dwErr = ::GetLastError();
	}

	return bAddedOk;
}
