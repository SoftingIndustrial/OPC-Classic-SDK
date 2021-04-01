#ifndef _SRVAPP_H_
#define _SRVAPP_H_

#pragma pack(push, 4)


#ifdef OTB_SERVER

#include "OTServer.h"

//-----------------------------------------------------------------------------
// ServerApp
//-----------------------------------------------------------------------------

class CWebServer;
class SOSrvWebTrace;
class ServerApp
{
public:
	ServerApp();

	// server identity
#ifdef SOFEATURE_DCOM
	CLSID m_clsidDA;                               // class id of DA server
	SOCmnString m_progIdDA;                        // prog id of DA server
	SOCmnString m_verIndProgIdDA;                  // version independent prog id of DA server
	CLSID m_clsidAE;                               // class id of AE server
	SOCmnString m_progIdAE;                        // prog id of AE server
	SOCmnString m_verIndProgIdAE;                  // version independent prog id of AE server
#endif // SOFEATURE_DCOM
	SOCmnString m_serverName;                      // name of DA server
	WORD m_ipPortHTTP;                             // IP port number of HTTP server
	SOCmnString m_urlDA;                           // URL of XML-DA server
	SOCmnString m_serviceName;                     // service name
	SOCmnString m_serviceDescription;              // service description
	BYTE m_appType;                                // application type of OPC server
	// server version
	WORD m_majorVersionNumber;                     // major version number
	WORD m_minorVersionNumber;                     // minor version number
	WORD m_patchVersionNumber;                     // patch version number
	WORD m_buildNumber;                            // build number
	SOCmnString m_vendorInfo;                      // vendor information
	// server parameters
	TCHAR m_addressSpaceDelimiter;                 // delimiter sign for addressSpace levels in the item id
	BYTE m_addressSpaceType;                       // object and/or string base addressSpace
	LONG m_clientCheckPeriod;                      // period in ms to check the notification connections to the clients
	DWORD m_stringBasedObjectDataExpiry;           // string-based address space object data expiry timeout
	// DA server parameters
	TCHAR m_propertyDelimiter;                     // delimiter sign for DA properties in the item id
	DWORD m_minUpdateRateDA;                       // minimal update rate for a group (ms)
	// AE server parameters
	BYTE m_supportDisableConditions;               // support to enable or disable conditions
	// DA and AE server parameters
	BYTE m_optimizeForSpeed;                       // optimize the Toolkit CORE for speed in detriment of sorted items
	// Web server parameters
	SOCmnString m_webRootDirectory;                // root directory for web pages
	SOCmnString m_webAdministratorPassword;        // web administrator user password
	SOCmnString m_webOperatorPassword;             // web operator user password
#ifdef SOFEATURE_DCOM
	// DCOM-related security settings
	DWORD m_authLevel;                             // DCOM call authentication level
	DWORD m_impLevel;                              // DCOM call impersonation level
	DWORD m_cbAuthLevel;                           // DCOM callback authentication level
	DWORD m_cbImpLevel;                            // DCOM callback impersonation level
#endif

#ifdef SOOS_WINDOWS
#ifndef SOOS_WINDOWS_CE
	SERVICE_STATUS m_serviceStatus;                // current status of the service
	SERVICE_STATUS_HANDLE m_serviceStatusHandle;   // status handle of the service
#endif
#endif // SOOS_WINDOWS

#ifdef SOFEATURE_TUNNEL
	WORD m_tpPort;                                 // tunnel IP port number
	SOCmnStringList m_tpUserList;                  // tunnel user list
	SOCmnStringList m_tpPwdList;                   // tunnel password list
#endif

	// runtime data
	BOOL m_startedByCOM;                           // started the server via COM?
	SOCmnEvent* m_pCanCloseEvent;                  // can close application during shutdown event
#ifdef SOOS_WINDOWS
	HINSTANCE m_instance;                          // instance handle of application
#endif // SOOS_WINDOWS

	SOCmnEvent* m_startupEvent;                    // handle of the startup event

#ifdef OTB_SERVER_DA
	SOCmnList<SODaSItemTag> m_itemTagChangedList;
#endif // OTB_SERVER_DA

#ifdef OTB_SERVER_AE
	SOCmnList<SOAeSCondition> *getAlarmList(void)
	{
		return &m_alarmList;
	}
	SOCmnList<SOAeSCondition> m_alarmList;         // list of active or unacknowledged alarms
#endif // OTB_SERVER_AE

#ifdef OTB_XMLDA_SERVER
	SOCmnPointer<CWebServer> m_webServer;
	SOSrvWebTrace* getWebTrace(void);
	SOCmnPointer<SOSrvWebTrace> m_webTrace;
#endif // SOFEATURE_SOAP

	// initialization/termination
	BOOL initialize(void);                         // initialize the OPC server application
	BOOL prepare(void);                            // perpare the OPC toolkit
	BOOL start(void);                              // start the OPC server I/O functionality
	void stop(IN SOCmnString reason);              // stop the OPC server I/O functionality
	void unload(void);                             // unload the OPC server configuration
	void finalize(void);                           // finalize the OPC toolkit
	void terminate(void);                          // terminate the OPC server application
	BOOL processCommandLine(IN LPCTSTR cmdLine);   // process the command line options

#ifdef SOOS_WINDOWS
#ifndef SOOS_WINDOWS_CE
	BOOL unregisterService(void);
	void stopService(IN SC_HANDLE hService);
	BOOL registerService(void);
#endif
#endif // SOOS_WINDOWS

	OTSCallbackFunctions m_callbackFunctions;

#ifdef SOFEATURE_TUNNEL
	void buildPermissibleCredentialsList(SOCmnList<SOCmnTPCredentials> &credentialsList);
#endif // SOFEATURE_TUNNEL

}; // ServerApp




//-----------------------------------------------------------------------------
// Global functions
//-----------------------------------------------------------------------------

// server application access function
extern ServerApp* getServerApp();
extern void createServerApp();
extern void destroyServerApp();



//-----------------------------------------------------------------------------
// CEventHandler
//-----------------------------------------------------------------------------

class CEventHandler : public SOSrvEventHandler
{
public:
	// released last COM object of the server
	virtual void onReleasedLastOPCObject(void);
}; // CEventHandler

#endif // OTB_SERVER


extern BOOL demoElapsed(void);

#pragma pack(pop)
#endif
