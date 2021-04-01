#ifndef _SERVERAPPLICATION_H_
#define _SERVERAPPLICATION_H_

#include "OSCompat.h"
#include "Enums.h"
#include "ServerEnums.h"
#include "Trace.h"

#include "ServerCreator.h"
#include "Mutex.h"
#include <map>
#include <vector>

#ifdef WIN64
#pragma pack(push,8)
#else
#pragma pack(push,4)
#endif

#ifdef TBC_OS_WINDOWS
#define API_CALL __stdcall
#endif

#ifdef TBC_OS_LINUX
#define API_CALL
#endif


namespace SoftingOPCToolboxServer
{

typedef long(API_CALL* ShutdownRequestHandler)(void);
typedef void (API_CALL* TraceEventHandler)(
	IN tstring aTraceString,
	IN unsigned short aLevel,
	IN unsigned long aMask,
	IN tstring anObjId,
	IN tstring aText);

class AeCondition;
class AeCategory;
class AeEvent;

// The Application class is the servers main part
class TBC_EXPORT Application
{
	friend class OTSGlobals;
private:

	//  Init Data
	short m_versionOTB;                 // used OTB version
	//  server identity
	EnumApplicationType m_appType;      // application type of OPC server
	tstring m_serviceName;              // name of the windows service; leave empty if not a service
	tstring m_serviceDescription;       // description of the windows service; optional
	tstring m_clsidDA;                  // class id of DA server
	tstring m_progIdDA;                 // prog id of DA server
	tstring m_verIndProgIdDA;           // version independent prog id of DA server
	tstring m_clsidAE;                  // class id of DA server
	tstring m_progIdAE;                 // prog id of DA server
	tstring m_verIndProgIdAE;           // version independent prog id of DA server
	tstring m_description;              // description of server
	unsigned short m_ipPortHTTP;        // IP port number of HTTP server
	tstring m_urlDA;                    // URL of XML-DA server
	// version information
	short m_majorVersion;               // major version number of the server
	short m_minorVersion;               // minor version number of the server
	short m_patchVersion;               // patch version number of the server
	short m_buildNumber;                // build number of the server
	tstring m_vendorInfo;               // version info of the server
	// server parameters
	TCHAR m_addressSpaceDelimiter;      // delimiter sign for addressSpace levels in the item id
	long m_clientCheckPeriod;           // period in ms to check the notification connections to the clients
	unsigned long m_stringBasedObjectDataExpiry; // string-based object data expiry timeout
	// DA server parameters
	TCHAR m_propertyDelimiter;          // delimiter sign for DA properties in the item id
	long m_minUpdateRateDA;             // minimal update rate for a group (ms)
	// AE server parameters
	unsigned char m_supportDisableConditions;   // supVerIndProgIdDaport to enable or disable conditions
	// Web server parameters
	tstring m_webRootDirectory;         // root directory for web pages
	tstring m_webRootFile;              // root file of the web server; default: index.html
	tstring m_webAdministratorPassword; // web administrator user password
	tstring m_webOperatorPassword;      // web operator user password

	//  Tunnel protocol properties
	unsigned short m_tpPort;            // TP port number
	long m_tpCredentialsNumber;         // the number of TP credentials
	std::vector<tstring> m_tpUsers;     // the list containing the users
	std::vector<tstring> m_tpPasswords; // the list containing the passwords

	// optimize the internal Toolkit CORE structures for speed in detriment of sorted items
	unsigned char m_optimizeForSpeed;	// whether to optimize for speed or not

	// DCOM-related security settings
	DWORD m_authLevel;                  // DCOM call authentication level
	DWORD m_impLevel;                   // DCOM call impersonation level
	DWORD m_cbAuthLevel;                // DCOM callback authentication level
	DWORD m_cbImpLevel;                 // DCOM callback impersonation level

	// Singleton Instance access
	static Application* m_instance;

	//  private constructor
	Application();

	virtual ~Application();

	Creator* m_creator;

	DaAddressSpaceRoot* m_DaAddressSpaceRoot;
	std::map<unsigned long, DaSession*> m_sessionList;
	Mutex m_sessionListJanitor;

	std::map<unsigned long, DaTransaction*> m_transactionList;
	Mutex m_transactionListJanitor;

	AeAddressSpaceRoot* m_AeAddressSpaceRoot;

	std::map<unsigned long, AeCategory*> m_categoryList;
	std::map<unsigned long, AeCondition*> m_conditionList;
	Mutex m_conditionListJanitor;
	Mutex m_categoryListJanitor;

	WebTemplate* m_webTemplate;

	void addTransaction(unsigned long aTransactionKey, DaTransaction* aTransaction);
	long markRequestCompletedInternally(unsigned long requestHandle);

public:

	TraceEventHandler TraceCompleted;
	ShutdownRequestHandler ShutdownRequest;

	static Application* Instance(void)
	{
		if (m_instance == NULL)
		{
			m_instance = new Application();
		}   //  end if

		return m_instance;
	}   //  end Instance

	static void Release(void)
	{
		if (m_instance != NULL)
		{
			delete m_instance;
			m_instance = NULL;
		}   //  end if
	}   //  end Release

	unsigned short getVersionOtb(void)
	{
		return m_versionOTB;
	}
	void setVersionOtb(unsigned short aValue)
	{
		m_versionOTB = aValue;
	}

	//  Application type of OPC server
	EnumApplicationType getAppType(void)
	{
		return m_appType;
	}
	void setAppType(EnumApplicationType aValue)
	{
		m_appType = aValue;
	}

	//  Service name which is and has to be set when the application runs as service
	tstring& getServiceName(void)
	{
		return m_serviceName;
	}
	void setServiceName(tstring aValue)
	{
		m_serviceName = aValue;
	}

	//  Service description may be set when the application runs as service
	tstring& getServiceDescription(void)
	{
		return m_serviceDescription;
	}
	void setServiceDescription(tstring aValue)
	{
		m_serviceDescription = aValue;
	}

	//  Class id of DA server
	tstring& getClsidDa(void)
	{
		return m_clsidDA;
	}
	void setClsidDa(tstring aValue)
	{
		m_clsidDA = aValue;
	}

	//  Prog ID of DA server
	tstring& getProgIdDa(void)
	{
		return m_progIdDA;
	}
	void setProgIdDa(tstring aValue)
	{
		m_progIdDA = aValue;
	}

	//  Version independent prog ID of DA server
	tstring& getVerIndProgIdDa(void)
	{
		return m_verIndProgIdDA;
	}
	void setVerIndProgIdDa(tstring aValue)
	{
		m_verIndProgIdDA = aValue;
	}

	// Description of the OPC server
	tstring& getDescription(void)
	{
		return m_description;
	}
	void setDescription(tstring aValue)
	{
		m_description = aValue;
	}

	// Class id of AE server
	tstring& getClsidAe(void)
	{
		return m_clsidAE;
	}
	void setClsidAe(tstring aValue)
	{
		m_clsidAE = aValue;
	}

	// Prog id of AE server
	tstring& getProgIdAe(void)
	{
		return m_progIdAE;
	}
	void setProgIdAe(tstring aValue)
	{
		m_progIdAE = aValue;
	}

	// Version independent prog ID of AE server
	tstring& getVerIndProgIdAe(void)
	{
		return m_verIndProgIdAE;
	}
	void setVerIndProgIdAe(tstring aValue)
	{
		m_verIndProgIdAE = aValue;
	}

	// IP port number of HTTP server
	unsigned short getIpPortHTTP(void)
	{
		return m_ipPortHTTP;
	}
	void setIpPortHTTP(unsigned short aValue)
	{
		m_ipPortHTTP = aValue;
	}

	// URL of XML-DA server
	tstring& getUrlDa(void)
	{
		return m_urlDA;
	}
	void setUrlDa(tstring aValue)
	{
		m_urlDA = aValue;
	}

	// Major version number of the server
	short getMajorVersion(void)
	{
		return m_majorVersion;
	}
	void setMajorVersion(short aValue)
	{
		m_majorVersion = aValue;
	}

	// Minor version number of the server
	short getMinorVersion(void)
	{
		return m_minorVersion;
	}
	void setMinorVersion(short aValue)
	{
		m_minorVersion = aValue;
	}

	// Patch version number of the server
	short getPatchVersion(void)
	{
		return m_patchVersion;
	}
	void setPatchVersion(short aValue)
	{
		m_patchVersion = aValue;
	}

	// Build number of the server
	short getBuildNumber(void)
	{
		return m_buildNumber;
	}
	void setBuildNumber(short aValue)
	{
		m_buildNumber = aValue;
	}

	//  Vendor info of the server
	tstring& getVendorInfo(void)
	{
		return m_vendorInfo;
	}
	void setVendorInfo(tstring aValue)
	{
		m_vendorInfo = aValue;
	}

	// Delimiter sign for namespace levels in the item id
	TCHAR getAddressSpaceDelimiter(void)
	{
		return m_addressSpaceDelimiter;
	}
	void setAddressSpaceDelimiter(TCHAR aValue)
	{
		m_addressSpaceDelimiter = aValue;
	}

	//  Period in ms to check the notification connections to the clients
	long getClientCheckPeriod(void)
	{
		return m_clientCheckPeriod;
	}
	void setClientCheckPeriod(long aValue)
	{
		m_clientCheckPeriod = aValue;
	}

	// Delimiter sign for DA properties in the item ID
	TCHAR getPropertyDelimiter(void)
	{
		return m_propertyDelimiter;
	}
	void setPropertyDelimiter(TCHAR aValue)
	{
		m_propertyDelimiter = aValue;
	}

	//  Minimal update rate for a group (ms)
	long getMinUpdateRateDa(void)
	{
		return m_minUpdateRateDA;
	}
	void setMinUpdateRateDa(long aValue)
	{
		m_minUpdateRateDA = aValue;
	}

	// string-based object data expiry timeout (in milliseconds)
	unsigned long getStringBasedObjectDataExpiry()
	{
		return m_stringBasedObjectDataExpiry;
	}
	void setStringBasedObjectDataExpiry(unsigned long expiryTimeoutMilliseconds)
	{
		m_stringBasedObjectDataExpiry = expiryTimeoutMilliseconds;
	}

	// disables AE conditions support
	unsigned char getSupportDisableConditions(void)
	{
		return m_supportDisableConditions;
	}
	void setSupportDisableConditions(unsigned char aValue)
	{
		m_supportDisableConditions = aValue;
	}

	//  Web Root Directory
	tstring& getWebRootDirectory(void)
	{
		return m_webRootDirectory;
	}
	void setWebRootDirectory(tstring aValue)
	{
		m_webRootDirectory = aValue;
	}

	//  Web Root File
	tstring& getWebRootFile(void)
	{
		return m_webRootFile;
	}
	void setWebRootFile(tstring aValue)
	{
		m_webRootFile = aValue;
	}

	//  Web AdministratorPassword
	tstring& getWebAdministratorPassword(void)
	{
		return m_webAdministratorPassword;
	}
	void setWebAdministratorPassword(tstring aValue)
	{
		m_webAdministratorPassword = aValue;
	}

	//  Web OperatorPassword
	tstring& getWebOperatorPassword(void)
	{
		return m_webOperatorPassword;
	}
	void setWebOperatorPassword(tstring aValue)
	{
		m_webOperatorPassword = aValue;
	}

	// IP port number of TP server
	unsigned short getTpPort(void)
	{
		return m_tpPort;
	}
	void setTpPort(unsigned short aValue)
	{
		m_tpPort = aValue;
	}

	// Whether to optimize the internal Toolkit CORE structures for speed in detriment of sorted items
	void setOptimizeForSpeed(BOOL aValue)
	{
		m_optimizeForSpeed = (unsigned char) aValue;
	}
	BOOL getOptimizeForSpeed() const
	{
		return (BOOL)m_optimizeForSpeed;
	}

	// DCOM-related security settings
	// direct call authentication level
	void setDcomAuthLevel(const DWORD aValue)
	{
		m_authLevel = aValue;
	}
	DWORD geDcomAuthLevel() const
	{
		return m_authLevel;
	}
	// direct call impersonation level
	void setDcomImpLevel(DWORD aValue)
	{
		m_impLevel = aValue;
	}
	DWORD getDcomImpLevel() const
	{
		return m_impLevel;
	}
	// callback authentication level
	void setDcomCbAuthLevel(DWORD aValue)
	{
		m_cbAuthLevel = aValue;
	}
	DWORD getDcomCbAuthLevel() const
	{
		return m_cbAuthLevel;
	}
	// callback impersonation level
	void setCbImpLevel(DWORD aValue)
	{
		m_cbImpLevel = aValue;
	}
	DWORD getCbImpLevel() const
	{
		return m_cbImpLevel;
	}

	//  adds the credentials
	long addTpCredentials(
		std::vector<tstring>& users,
		std::vector<tstring>& paswords)
	{
		if (users.size() != paswords.size())
		{
			return EnumResultCode_E_INVALIDARG;
		}   //  end if

		m_tpUsers = users;
		m_tpPasswords = paswords;
		m_tpCredentialsNumber = (long)users.size();
		return S_OK;
	}   //  addTpCredential

	//  Gets the DaAddressSpaceRoot root
	DaAddressSpaceRoot* getDaAddressSpaceRoot(void)
	{
		if (m_DaAddressSpaceRoot == NULL)
		{
			//  attempt a LazyLoad
			m_DaAddressSpaceRoot = m_creator->createDaAddressSpaceRoot();
		}   //  end if

		return m_DaAddressSpaceRoot;
	}   //  end getDaAddressSpaceRoot

	//  Gets the AeAddressSpaceRoot
	AeAddressSpaceRoot* getAeAddressSpaceRoot(void)
	{
		if (m_AeAddressSpaceRoot == NULL)
		{
			//  attempt a LazyLoad
			m_AeAddressSpaceRoot = m_creator->createAeAddressSpaceRoot();
		}   //  end if

		return m_AeAddressSpaceRoot;
	}   //  end getAeAddressSpaceRoot

	//  gets the associated web template
	WebTemplate* getWebTemplate(void)
	{
		return m_webTemplate;
	}   //  end getWebTemplate

	//  The Application`s creator for namespace elements, the namespace root, requests, transactions
	Creator* getCreator(void)
	{
		return m_creator;
	}

	long activate(
		EnumFeature aFeature,
		tstring aKey);

	// force the target application to run as demo
	long forceDemoVersion();

	long initialize(Creator* aCreator);

	long terminate(void);
	long processCommandLine(tstring& aCommandLine);

	long setAeServerState(EnumServerState newState);
	long setDaServerState(EnumServerState newState);

	long start(void);
	long ready(void);
	long stop(tstring reason = _T(""));

	long changeCondition(AeCondition* aCondition);
	long changeConditions(std::vector<AeCondition*>& aConditionList);
	void releaseCondition(unsigned long aConditionHandle);

	long addAeCategory(AeCategory* aCategory);

	long fireEvents(std::vector<AeEvent*>& anEventList);
	std::vector<AeCondition*> getAeConditionList(void);
	std::vector<AeCategory*> getAeCategoryList(void);

	DaSession* getSession(unsigned long aSessionHandle);
	void addSession(DaSession* aSession, unsigned long aSessionHandle);
	void removeSession(unsigned long aSessionHandle);

	DaTransaction* findTransaction(unsigned long aKey);
	void releaseTransaction(unsigned long aKey);

	void trace(EnumTraceLevel aLevel,
			   EnumTraceGroup aMask,
			   const TCHAR* anObjectID,
			   const TCHAR* aMessage,
			   ...);

	void enableTracing(
		EnumTraceGroup errorLevelMask,
		EnumTraceGroup warningLevelMask,
		EnumTraceGroup infoLevelMask,
		EnumTraceGroup debugLevelMask,
		tstring fileName,
		unsigned long fileMaxSize,
		unsigned long maxBackups);

};  //  end class Application

}   //  end namespace SoftingOPCToolboxServer

#ifdef __cplusplus
extern "C"
{
#endif
	TBC_EXPORT SoftingOPCToolboxServer::Application* API_CALL getApp();
	TBC_EXPORT void API_CALL releaseApp();
#ifdef __cplusplus
}
#endif

#pragma pack(pop)
#endif

