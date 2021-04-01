#ifndef _CLIENTAPPLICATION_H_
#define _CLIENTAPPLICATION_H_

#include "OSCompat.h"
#include "Enums.h"
#include "ClientEnums.h"
#include "Trace.h"

#include "Da/ClientDaSession.h"
#include "Ae/ClientAeSession.h"

#include <map>

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

namespace SoftingOPCToolboxClient
{

typedef void (API_CALL* TraceEventHandler)(
	IN tstring aTraceString,
	IN unsigned short aLevel,
	IN unsigned long aMask,
	IN tstring anObjId,
	IN tstring aText);

class ServerBrowser;
class AeCondition;

class TBC_EXPORT Application
{

	friend class OTCGlobals;
	friend class ServerBrowser;
	friend class DaSession;
	friend class AeSession;
	friend class DaSubscription;
	friend class AeCondition;

private:

	// Singleton Instance access
	static Application* m_instance;

	Application();
	virtual ~Application();

	BOOL m_isInit;
	//  Init Data
	short m_versionOTB;                 // used OTB version
	tstring m_serviceName;              // name of the windows service; leave empty if not a service
	tstring m_serviceDescription;       // description of the windows service; optional
	DWORD m_authLevel;                  // DCOM call authentication level
	DWORD m_impLevel;                   // DCOM call impersonation level
	DWORD m_xmldaGetStatusInterval;     // interval for calling GetStatus method in order to stay connected on "special" XMLDA servers

	std::map<unsigned long, ServerBrowser*> m_serverBrowserList;
	std::map<unsigned long, tstring> m_localeIdToStringList;
	std::map<unsigned long, ServerBrowser*>& getServerBrowserList()
	{
		return m_serverBrowserList;
	}

	DaSession* findDaSession(unsigned long position);
	AeSession* findAeSession(unsigned long position);
	std::map<unsigned long, DaSession*> m_daSessionList;
	Mutex m_daSessionListJanitor;
	std::map<unsigned long, AeSession*> m_aeSessionList;
	Mutex m_aeSessionListJanitor;
	EnumUpdateAttributesBehavior m_updateAttributesBehavior;

	//Adds a Session object to the Application.
	long addDaSession(
		const tstring& url,
		DaSession* aDaSession);

	//Adds an Alarms and Events Session object to the Application.
	long addAeSession(
		const tstring& url,
		AeSession* anAeSession);

public:

	unsigned short getVersionOtb(void)
	{
		return m_versionOTB;
	}
	void setVersionOtb(unsigned short aValue)
	{
		m_versionOTB = aValue;
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

	// DCOM-related security settings
	// direct call authentication level
	void setDcomAuthLevel(DWORD aValue)
	{
		m_authLevel = aValue;
	}
	DWORD getDcomAuthLevel()
	{
		return m_authLevel;
	}
	// direct call impersonation level
	void setDcomImpLevel(DWORD aValue)
	{
		m_impLevel = aValue;
	}
	DWORD getDcomImpLevel()
	{
		return m_impLevel;
	}

	void setXmldaGetStatusInterval(const DWORD& aValue)
	{
		m_xmldaGetStatusInterval = aValue;
	}

	DWORD getXmldaGetStatusInterval() const
	{
		return m_xmldaGetStatusInterval;
	}

	TraceEventHandler TraceOutput;

	//Gets the list with the OPC DA Servers added to the Application.
	std::vector<DaSession*> getDaSessionList()
	{
		m_daSessionListJanitor.lock();
		std::vector<DaSession*> sessionList(m_daSessionList.size(), NULL);
		std::map<unsigned long, DaSession*>::const_iterator daSessionIterator;
		unsigned long i = 0;

		for (daSessionIterator = m_daSessionList.begin(); daSessionIterator != m_daSessionList.end(); daSessionIterator ++)
		{
			if (daSessionIterator->second != NULL)
			{
				sessionList[i] = daSessionIterator->second;
				i++;
			} //end if
		} //end for

		m_daSessionListJanitor.unlock();
		return sessionList;
	}   //  end getDaSessionList

	//  Gets the list with the OPC AE Servers added to the Application.
	std::vector<AeSession*> getAeSessionList()
	{
		m_aeSessionListJanitor.lock();
		std::vector<AeSession*> sessionList(m_aeSessionList.size(), NULL);
		std::map<unsigned long, AeSession*>::const_iterator aeSessionIterator;
		unsigned long i = 0;

		for (aeSessionIterator = m_aeSessionList.begin(); aeSessionIterator != m_aeSessionList.end(); aeSessionIterator ++)
		{
			if (aeSessionIterator->second != NULL)
			{
				sessionList[i] = aeSessionIterator->second;
				i++;
			} //end if
		} //end for

		m_aeSessionListJanitor.unlock();
		return sessionList;
	}//end getAeSessionList

	//  Gets the modality of updating the attributes of an object.
	EnumUpdateAttributesBehavior getUpdateAttributesBehavior()
	{
		return m_updateAttributesBehavior;
	}//end getUpdateAttributesBehavior

	//  Sets the modality of updating the attributes of an object.
	void setUpdateAttributesBehavior(EnumUpdateAttributesBehavior aValue)
	{
		m_updateAttributesBehavior = aValue;
	} // end setUpdateAttributesBehavior


	static Application* Instance()
	{
		if (m_instance == NULL)
		{
			m_instance = new Application();
		}   //  end if

		return m_instance;
	}   //  end Instance

	static void Release()
	{
		if (m_instance != NULL)
		{
			delete m_instance;
			m_instance = NULL;
		}   //  end if
	}   //  end Release


	//Activates the Application.
	long activate(EnumFeature aProduct, tstring aKey);

	// Force the target application to run as demo
	long forceDemoVersion();

	//  Initializes the Application.
	long initialize();

	//  processes the command line - useful for registration and unregistration of service
	long processCommandLine(tstring& aCommandLine);

	//  Removes a Data Access Session object from the Application.
	long removeDaSession(DaSession* aDaSession);

	//  Removes an Alarms and Events Session object from the Application.
	long removeAeSession(AeSession* anAeSession);

	//  Terminates the Application.
	long terminate();

	//  Traces the Application.
	void trace(
		EnumTraceLevel aLevel,
		EnumTraceGroup aMask,
		const TCHAR* anObjectID,
		const TCHAR* aMessage,
		...);

	//  Enables tracing for the entire Application.
	void enableTracing(
		EnumTraceGroup errorLevelMask,
		EnumTraceGroup warningLevelMask,
		EnumTraceGroup infoLevelMask,
		EnumTraceGroup debugLevelMask,
		tstring fileName,
		unsigned long fileMaxSize,
		unsigned long maxBackups);

}; // end class Application

typedef std::pair<unsigned long, tstring> LcidToStringPair;

}; //end namespace SoftingOPCToolboxClient

#ifdef __cplusplus
extern "C"
{
#endif

	TBC_EXPORT SoftingOPCToolboxClient::Application* API_CALL getApplication();
	TBC_EXPORT void API_CALL releaseApplication();

#ifdef __cplusplus
}
#endif


#pragma pack(pop)
#endif
