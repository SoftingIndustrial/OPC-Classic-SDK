#include "stdafx.h"
#include "md5.c"
#include "SoftingProd.h"

#ifdef TB_OEM_I4T

#ifdef _DEBUG
#pragma comment(lib, "SLMbaseMDd.lib")
#pragma comment(lib, "liblmxclient_mdd.lib")
#else
#pragma comment(lib, "SLMbaseMD.lib")
#pragma comment(lib, "liblmxclient_md.lib")
#endif

#pragma comment(lib, "libhasp_windows.lib")

#include "SLMbase.h"

/* License version numbers
 * ----------------------------------------------------------------------------
 */
#define OSF_LICENSE_VER_MAJOR           2
#define OSF_LICENSE_VER_MINOR           0

#define OSF_LIC_LICENSENAME_BASE                    "IFM_OSF_Base"
#define OSF_LIC_LICENSENAME_SERVER_CONNECTIONS      "IFM_OSF_ServerConnections"

#define OSF_LIC_OPTION_SERVER_CONNECTIONS   "SRV"

BOOL s_SLMInitialized = FALSE;
LONG s_licensedConnections = 0;

//	an attempt to obfuscate the sempahore
LPCTSTR s_licenseSemaphoreName = _T("Global\\{0C2A4D0A-09B5-44d9-A759-6AE770EA93F1}");
HANDLE s_licensedConnectionsSemaphore = NULL;

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

#endif // TB_OEM_I4T

#define LIC_HW_INDEPENDENT_INST_KEY    "nohwsofting:b3f9-8b0b"
#define LIC_KEY_BYTES   10
#define LIC_KEY_CHARS   24

#define LIC_OK                  0
#define LIC_ERR_KEY_FORMAT      13
#define LIC_ERR_BAD_PRODNUM     5
#define LIC_ERR_KEY_INVALID     14

#define OT_LICENSEBIT_DT         0x8000
#define OT_LICENSEBIT_RT_SW      0x0800
#define OT_LICENSEBIT_RT_HW      0x0400
#define OT_LICENSEBIT_RECHECK    0x0001
#define OT_LICENSEMASK_LICENSED  0xFFF0

#ifdef OTB_FORCE_NO_LICENSE
// Only to be used in exceptional cases
static WORD s_licenseInfoServerDA = OT_LICENSEBIT_DT;
static WORD s_licenseInfoClientDA = OT_LICENSEBIT_DT;
static WORD s_licenseInfoServerTP = OT_LICENSEBIT_DT;
static WORD s_licenseInfoClientTP = OT_LICENSEBIT_DT;
static WORD s_licenseInfoServerXMLDA = OT_LICENSEBIT_DT;
static WORD s_licenseInfoClientXMLDA = OT_LICENSEBIT_DT;
static WORD s_licenseInfoServerAE = OT_LICENSEBIT_DT;
static WORD s_licenseInfoClientAE = OT_LICENSEBIT_DT;
#else
static WORD s_licenseInfoServerDA = 0;
static WORD s_licenseInfoClientDA = 0;
static WORD s_licenseInfoServerTP = 0;
static WORD s_licenseInfoClientTP = 0;
static WORD s_licenseInfoServerXMLDA = 0;
static WORD s_licenseInfoClientXMLDA = 0;
static WORD s_licenseInfoServerAE = 0;
static WORD s_licenseInfoClientAE = 0;
#endif
static BYTE s_activateInfoServerDA = FALSE;
static BYTE s_activateInfoClientDA = FALSE;
static BYTE s_activateInfoServerXMLDA = FALSE;
static BYTE s_activateInfoClientXMLDA = FALSE;
static BYTE s_activateInfoServerAE = FALSE;
static BYTE s_activateInfoClientAE = FALSE;
static BYTE s_activateInfoServerTP = FALSE;
static BYTE s_activateInfoClientTP = FALSE;
extern DWORD g_demoStamp;

static BOOL s_forceDemoMode = FALSE;

typedef long(_stdcall _SLIC_GetLicenseForProduct)(long prodNum, long* pFeatureBits, long* pHoursLeft, long* pLicMedia);

static DWORD s_recheckCount = GetTickCount();

BOOL isDemoMode(void)
{
	return FALSE; // no demo version
}

BOOL isLicensed(BYTE feature, LONG reserved)
{
	return true;
}

#ifdef TB_OEM_I4T

void destroy(void)
{
	if (s_SLMInitialized)
	{
		SLMbaseExit();
	}

	if (s_licensedConnectionsSemaphore != NULL)
	{
		CloseHandle(s_licensedConnectionsSemaphore);
		s_licensedConnectionsSemaphore = NULL;
	}
}

/* getLicenseOption
 * ----------------------------------------------------------------------------
 */
DWORD getLicenseOption(LPCTSTR option, LPCTSTR base)
{
	TCHAR temp[100];
	_stprintf(temp, _T("%s="), base);

	if (_tcsncmp(option, temp, _tcslen(temp)) != 0)
	{
		return 0;
	}

	return _ttoi(option + _tcslen(temp));
}

#endif  //  TB_OEM_I4T


void updateLicenseInfo(BYTE feature)
{
#ifndef SOOS_WINDOWS_CE
	WORD curLic = 0;
	LONG prodNr = 0;
	SOCmnString prodSZ;
	BOOL usesSLM = TRUE;

	switch (feature)
	{
	case OTS_FEATURE_DA_SERVER:
		curLic = s_licenseInfoServerDA;
		prodNr = SPROD_OTS_DA_RT_V40;
		prodSZ = _T("OTS - OPC DA");
		break;

	case OTS_FEATURE_XMLDA_SERVER:
		curLic = s_licenseInfoServerXMLDA;
		prodNr = SPROD_OTS_XMLDA_RT_V40;
		prodSZ = _T("OTS - XML-DA");
		break;

	case OTC_FEATURE_DA_CLIENT:
		curLic = s_licenseInfoClientDA;
		prodNr = SPROD_OTC_DA_RT_V40;
		prodSZ = _T("OTC - OPC DA");
		break;

	case OTC_FEATURE_XMLDA_CLIENT:
		curLic = s_licenseInfoClientXMLDA;
		prodNr = SPROD_OTC_XMLDA_RT_V40;
		prodSZ = _T("OTC - XML-DA");
		break;

	case OTS_FEATURE_AE_SERVER:
		curLic = s_licenseInfoServerAE;
		prodNr = SPROD_OPC_AE_SERVER_RT_V40X;
		prodSZ = _T("OTS - OPC AE");
		break;

	case OTC_FEATURE_AE_CLIENT:
		curLic = s_licenseInfoClientAE;
		prodNr = SPROD_OPC_AE_CLIENT_RT_V40X;
		prodSZ = _T("OTC - OPC AE");
		break;
#ifdef TB_OEM_I4T

	case OTC_FEATURE_TP_CLIENT:
		usesSLM = FALSE;
		break;
#endif

	default:
		return;
		break;
	}

	// check for binary license
	if ((curLic & OT_LICENSEBIT_DT) == OT_LICENSEBIT_DT)
	{
		return;
	}

#ifdef SOOS_WINDOWS
	// check for runtime license
	// check license
	WORD newLic = 0;

	if (usesSLM)
	{
		HMODULE licDLLInstance = LoadLibrary(_T("fc_slbase20.dll"));

		if (licDLLInstance != NULL)
		{
			_SLIC_GetLicenseForProduct* SLIC_GetLicenseForProduct;
			SLIC_GetLicenseForProduct = (_SLIC_GetLicenseForProduct*)GetProcAddress(licDLLInstance, "SLIC_GetLicenseForProduct");

			if (SLIC_GetLicenseForProduct != NULL)
			{
				LONG lic, featureBits, hoursLeft, licMedia = 0;
				lic = SLIC_GetLicenseForProduct(prodNr, &featureBits, &hoursLeft, &licMedia);

				if (lic == 0)
				{
					// we have a valid temporary license or a full license
					// #define SLIC_OK                  0
					if (hoursLeft == -1)
					{
						// full license
						// SLIC_MEDIA_SOFT = 1
						// SLIC_MEDIA_DONGLE= 2
						if (licMedia == 1)
						{
							newLic = OT_LICENSEBIT_RT_SW;
							_TRACEC(SOCMNTRACE_L_INF, OT_TRACEMASK_OTBLICENSE, _T("LICENSE"), (_T("%s - Full license - software"), (LPCTSTR)prodSZ));
						}
						else
						{
							newLic = OT_LICENSEBIT_RT_HW | OT_LICENSEBIT_RECHECK;
							_TRACEC(SOCMNTRACE_L_INF, OT_TRACEMASK_OTBLICENSE, _T("LICENSE"), (_T("%s - Full license - dongle"), (LPCTSTR)prodSZ));
						}
					}
					else
					{
						newLic = OT_LICENSEBIT_RT_SW | OT_LICENSEBIT_RECHECK;
						_TRACEC(SOCMNTRACE_L_INF, OT_TRACEMASK_OTBLICENSE, _T("LICENSE"), (_T("%s - Temporary license - %ld hours left"), (LPCTSTR)prodSZ, hoursLeft));
					}
				}
				else
				{
					switch (lic)
					{
					case 20:
						_TRACEC(SOCMNTRACE_L_ERR, OT_TRACEMASK_OTBLICENSE, _T("LICENSE"), (_T("%s - Component not found"), (LPCTSTR)prodSZ));
						break;

					case 21:
						_TRACEC(SOCMNTRACE_L_WRN, OT_TRACEMASK_OTBLICENSE, _T("LICENSE"), (_T("%s - No license found"), (LPCTSTR)prodSZ));
						break;

					case 22:
						_TRACEC(SOCMNTRACE_L_WRN, OT_TRACEMASK_OTBLICENSE, _T("LICENSE"), (_T("%s - Temporary license expired"), (LPCTSTR)prodSZ));
						break;
					}
				}
			}
		}
	}
	else
	{
#ifdef TB_OEM_I4T

		if (feature == OTC_FEATURE_TP_CLIENT)
		{
			int majorVersion = 2;
			int minorVersion = 0;
			int trialPeriod = 0;
			char options[SLM_MAX_FIELD_LENGTH + 1];
			newLic = 0;
			SLM_STATUS res = -1;

			if (!s_SLMInitialized)
			{
				//  no notifications required
				SLM_STATUS res = SLMbaseInit(0, NULL, NULL);
				atexit(destroy);

				if (res != SLM_STATUS_SUCCESS)
				{
					_TRACEC(SOCMNTRACE_L_ERR, SOCMNTRACE_G_OPC_CALL_C, _T("LICENSE"), (_T("Can't initialize SLM library [%d]"), res));
				}
				else
				{
					s_SLMInitialized = TRUE;
					_TRACEC(SOCMNTRACE_L_INF, SOCMNTRACE_G_OPC_CALL_C, _T("LICENSE"), (_T("Initialize SLM library success")));
				}
			}

			s_licensedConnections = 0;
			res = SLMbaseCheckout(OSF_LIC_LICENSENAME_BASE, majorVersion, minorVersion, trialPeriod, options);
			USES_CONVERSION;

			if (res == SLM_STATUS_SUCCESS)
			{
				newLic = OT_LICENSEBIT_RT_SW | OT_LICENSEBIT_RECHECK;
				s_licensedConnections = 1;
				_TRACEC(SOCMNTRACE_L_INF, SOCMNTRACE_G_OPC_CALL_C, _T("LICENSE"), (_T("SLM License for %s found"), A2T(OSF_LIC_LICENSENAME_BASE)));
				res = SLMbaseCheckout(OSF_LIC_LICENSENAME_SERVER_CONNECTIONS, majorVersion, minorVersion, trialPeriod, options);

				if (res == SLM_STATUS_SUCCESS)
				{
					s_licensedConnections = getLicenseOption(A2T(options), _T(OSF_LIC_OPTION_SERVER_CONNECTIONS));
					_TRACEC(SOCMNTRACE_L_INF, SOCMNTRACE_G_OPC_CALL_C, _T("LICENSE"), (_T("SLM License for %s found"), A2T(OSF_LIC_LICENSENAME_SERVER_CONNECTIONS)));
				}
				else
				{
					_TRACEC(SOCMNTRACE_L_ERR, SOCMNTRACE_G_OPC_CALL_C, _T("LICENSE"), (_T("SLM License checkout for %s failed with [%s]"),
																					   A2T(OSF_LIC_LICENSENAME_SERVER_CONNECTIONS),
																					   A2T(SLMbaseGetStatusString(res))));
				}

				SLMbaseCheckin(OSF_LIC_LICENSENAME_SERVER_CONNECTIONS);
			}
			else
			{
				_TRACEC(SOCMNTRACE_L_ERR, SOCMNTRACE_G_OPC_CALL_C, _T("LICENSE"), (_T("SLM License checkout for %s failed with [%s]"),
																				   A2T(OSF_LIC_LICENSENAME_BASE),
																				   A2T(SLMbaseGetStatusString(res))));
			}

			SLMbaseCheckin(OSF_LIC_LICENSENAME_BASE);
		}

#endif // TB_OEM_I4T
	}

	switch (feature)
	{
	case OTS_FEATURE_DA_SERVER:
		s_licenseInfoServerDA = newLic;
		break;

	case OTS_FEATURE_XMLDA_SERVER:
		s_licenseInfoServerXMLDA = newLic;
		break;

	case OTC_FEATURE_DA_CLIENT:
		s_licenseInfoClientDA = newLic;
		break;

	case OTC_FEATURE_XMLDA_CLIENT:
		s_licenseInfoClientXMLDA = newLic;
		break;

	case OTS_FEATURE_AE_SERVER:
		s_licenseInfoServerAE = newLic;
		break;

	case OTC_FEATURE_AE_CLIENT:
		s_licenseInfoClientAE = newLic;
		break;
#ifdef TB_OEM_I4T

	case OTC_FEATURE_TP_CLIENT:
		s_licenseInfoClientTP = newLic;
		break;
#endif

	default:
		break;
	}

#endif // SOOS_WINDOWS
#endif // SOOS_WINDOWS_CE
}

BOOL demoElapsed(BYTE feature, LONG reserved)
{
	return false;
}


BOOL __inline HexToByte(LPCTSTR psz, BYTE* pby)
{
	TCHAR hh[3];
#ifdef SOOS_WINDOWS

	if (!(_istxdigit(psz[0]) && _istxdigit(psz[1])))
	{
		return FALSE;
	}

#endif
	hh[0] = psz[0];
	hh[1] = psz[1];
	hh[2] = 0;
	*pby = (BYTE)(_tcstoul(hh, NULL, 16) & 0xff);
	return TRUE;
}


static LONG RegKeyStringToBin(LPCTSTR pszRegKey, BYTE binKey[LIC_KEY_BYTES])
{
	int nBytes;
	int nChars;
	nBytes = nChars = 0;

	for (;;)
	{
		if (nBytes >= LIC_KEY_BYTES)
		{
			break;
		}

		if (!HexToByte(pszRegKey + nChars, &binKey[nBytes]) || !HexToByte(pszRegKey + nChars + 2, &binKey[nBytes + 1]))
		{
			return LIC_ERR_KEY_FORMAT;
		}

		nChars += 4;
		nBytes += 2;

		if (!pszRegKey[nChars])
		{
			break;
		}

		if (pszRegKey[nChars] != '-')
		{
			return LIC_ERR_KEY_FORMAT;
		}
		else
		{
			nChars++;
		}
	}

	if (nBytes != LIC_KEY_BYTES)
	{
		return LIC_ERR_KEY_FORMAT;
	}

	return LIC_OK;
}


LONG checkLicense(LONG prodNum, LPCTSTR licKey)
{
	return LIC_OK;
}


LONG OTCAPI_CALL OTActivate(IN BYTE feature, IN LPTSTR key, IN BYTE forceDemo)
{
	SOCMN_TRY
	{
		LONG prodNr = 0;
		SOCmnString prodSZ;
		LONG lLicense;
		SOCmnTrace *pTrace = ::getSOCmnTrace();

		if (forceDemo)
		{
			return S_OK;
		}

		switch (feature)
		{
		case OTS_FEATURE_DA_SERVER:
			prodNr = SPROD_OTS_DA_DT_V447;
			prodSZ = _T("OPC Toolkit Server V4.47 - DA");

			if (s_activateInfoServerDA)
			{
				return OT_E_INVALIDLICENSE;
			}

			s_activateInfoServerDA = TRUE;
			break;

		case OTS_FEATURE_XMLDA_SERVER:
#ifdef SOOS_WINDOWS
			prodNr = SPROD_OTS_XMLDA_DT_WINDOWS_V447;
			prodSZ = _T("OPC Toolkit Server V4.47 - XML-DA Windows");

			if (s_activateInfoServerXMLDA)
			{
				return OT_E_INVALIDLICENSE;
			}

			s_activateInfoServerXMLDA = TRUE;
#endif
#ifdef SOOS_LINUX
			prodNr = SPROD_OTS_XMLDA_DT_LINUX_V430;
			prodSZ = _T("OPC Toolkit Server V4.3x - XML-DA Linux");

			if (s_activateInfoServerXMLDA)
			{
				return OT_E_INVALIDLICENSE;
			}

			s_activateInfoServerXMLDA = TRUE;
#endif
			break;

		case OTC_FEATURE_DA_CLIENT:
			prodNr = SPROD_OTC_DA_DT_V447;
			prodSZ = _T("OPC Toolkit Client V4.47 - DA");

			if (s_activateInfoClientDA)
			{
				return OT_E_INVALIDLICENSE;
			}

			s_activateInfoClientDA = TRUE;
			break;

		case OTC_FEATURE_XMLDA_CLIENT:
#ifdef SOOS_WINDOWS
			prodNr = SPROD_OTC_XMLDA_DT_WINDOWS_V447;
			prodSZ = _T("OPC Toolkit Client V4.47 - XML-DA Windows");

			if (s_activateInfoClientXMLDA)
			{
				return OT_E_INVALIDLICENSE;
			}

			s_activateInfoClientXMLDA = TRUE;
#endif
#ifdef SOOS_LINUX
			prodNr = SPROD_OTC_XMLDA_DT_LINUX_V430;
			prodSZ = _T("OPC Toolkit Client V4.3x - XML-DA Linux");

			if (s_activateInfoClientXMLDA)
			{
				return OT_E_INVALIDLICENSE;
			}

			s_activateInfoClientXMLDA = TRUE;
#endif
			break;

		case OTS_FEATURE_AE_SERVER:
			prodNr = SPROD_OTS_AE_DT_V447;
			prodSZ = _T("OPC Toolkit Server V4.47 - AE");

			if (s_activateInfoServerAE)
			{
				return OT_E_INVALIDLICENSE;
			}

			s_activateInfoServerAE = TRUE;
			break;

		case OTC_FEATURE_AE_CLIENT:
			prodNr = SPROD_OTC_AE_DT_V447;
			prodSZ = _T("OPC Toolkit Client V4.47 - AE");

			if (s_activateInfoClientAE)
			{
				return OT_E_INVALIDLICENSE;
			}

			s_activateInfoClientAE = TRUE;
			break;
#ifdef TB_OEM_ABB

		case OTC_FEATURE_TP_CLIENT:
			prodNr = SPROD_OTC_TP_OBJECT; // 191 = SPROD_OPC_TP_CLIENT; 192 = SPROD_OPC_TP_SERVER
			prodSZ = _T("OPC Toolkit Client TP");

			if (s_activateInfoClientTP)
			{
				return OT_E_INVALIDLICENSE;
			}

			s_activateInfoClientTP = TRUE;
			break;
#endif
#ifdef TB_OEM_SOFTING

		case OTC_FEATURE_TP_CLIENT:
			prodNr = SPROD_OTC_TP_OBJECT; // 191 = SPROD_OPC_TP_CLIENT; 192 = SPROD_OPC_TP_SERVER
			prodSZ = _T("OPC Toolkit Client TP");

			if (s_activateInfoClientTP)
			{
				return OT_E_INVALIDLICENSE;
			}

			s_activateInfoClientTP = TRUE;
			break;

		case OTS_FEATURE_TP_SERVER:
			prodNr = SPROD_OTS_TP_OBJECT; // 191 = SPROD_OPC_TP_CLIENT; 192 = SPROD_OPC_TP_SERVER
			prodSZ = _T("OPC Toolkit Server TP");

			if (s_activateInfoServerTP)
			{
				return OT_E_INVALIDLICENSE;
			}

			s_activateInfoServerTP = TRUE;
			break;
#endif

		default:
			return OT_E_INVALIDFEATURE;
			break;
		}

		lLicense = checkLicense(prodNr, key);

		if (lLicense == 0)
		{
			_TRACEC(SOCMNTRACE_L_INF, OT_TRACEMASK_OTBLICENSE, _T("LICENSE"), (_T("%s - Binary license"), (LPCTSTR)prodSZ));

			switch (feature)
			{
			case OTS_FEATURE_DA_SERVER:
				s_licenseInfoServerDA = OT_LICENSEBIT_DT;
				pTrace->addLicenseInfo(_T("OPC DA Server"));
				break;

			case OTS_FEATURE_XMLDA_SERVER:
				s_licenseInfoServerXMLDA = OT_LICENSEBIT_DT;
				pTrace->addLicenseInfo(_T("XMLDA Server"));
				break;

			case OTC_FEATURE_DA_CLIENT:
				pTrace->addLicenseInfo(_T("OPC DA Client"));
				s_licenseInfoClientDA = OT_LICENSEBIT_DT;
				break;

			case OTC_FEATURE_XMLDA_CLIENT:
				pTrace->addLicenseInfo(_T("XMLDA Client"));
				s_licenseInfoClientXMLDA = OT_LICENSEBIT_DT;
				break;

			case OTS_FEATURE_AE_SERVER:
				pTrace->addLicenseInfo(_T("OPC AE Server"));
				s_licenseInfoServerAE = OT_LICENSEBIT_DT;
				break;

			case OTC_FEATURE_AE_CLIENT:
				pTrace->addLicenseInfo(_T("OPC AE Client"));
				s_licenseInfoClientAE = OT_LICENSEBIT_DT;
				break;

			case OTC_FEATURE_TP_CLIENT:
				pTrace->addLicenseInfo(_T("Tunnel DA Client"));
				s_licenseInfoClientTP = OT_LICENSEBIT_DT;
				break;

			case OTS_FEATURE_TP_SERVER:
				pTrace->addLicenseInfo(_T("Tunnel DA Server"));
				s_licenseInfoServerTP = OT_LICENSEBIT_DT;
				break;

			default:
				break;
			}   //  end switch

			return S_OK;
		}

		_TRACEC(SOCMNTRACE_L_ERR, OT_TRACEMASK_OTBLICENSE, _T("LICENSE"), (_T("%s - Invalid binary license : %s"), (LPCTSTR)prodSZ, (LPCTSTR)key));
		return OT_E_INVALIDLICENSE;
	}
	SOCMN_CATCH(...)
	{
		return OT_E_EXCEPTION;
	}
}


#ifdef TB_OEM_I4T

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
*/
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


#endif  //  TB_OEM_I4T