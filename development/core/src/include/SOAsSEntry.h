#ifndef _SOASSENTRY_H_
#define _SOASSENTRY_H_

#pragma pack(push, 4)


#include <SODaSEntry.h>
#include <SOAeSEntry.h>


//-----------------------------------------------------------------------------
// SOAsSEntry                                                                 |
//-----------------------------------------------------------------------------

class SOASS_EXPORT SOAsSEntry : public SOSrvEntry
{

public:
	SOAsSEntry(void);

	// initialize engine
	virtual BOOL initialize(IN enum serverTypeCOM type, IN REFCLSID clsidDA, IN REFCLSID clsidAE,
							IN HINSTANCE hInst, OPTIONAL IN DWORD coInit = COINIT_APARTMENTTHREADED);
	virtual BOOL terminate(void);
#ifdef SOFEATURE_SOAP
	virtual BOOL initializeHTTP(IN WORD port);
	BOOL addXMLDAReceiver(IN LPCTSTR url);
#endif
#ifdef SOFEATURE_TUNNEL
	BOOL addTPDAReceiver(void);
	BOOL addTPAEReceiver(void);
#endif

	// sub entry objects
	SODaSEntry* getDAEntry(void);
	SOAeSEntry* getAEEntry(void);

	SODaSServer* getSurveyServer(void);

	// start and stop of entry threads
	virtual BOOL start(void);
	virtual BOOL stop(void);

	// main thread of the toolkit
	virtual DWORD mainThread(void);

	// send shutdown request to all connected clients
	virtual void shutdownAllClients(IN LPCTSTR reason);

	// toolkit object factory
	void setCreator(IN SOSrvCreator* creator)
	{
		m_creator = creator;
		m_creator.addRef();
	}
	SOSrvCreator* getCreator(void)
	{
		m_creator.addRef();
		return m_creator;
	}
	virtual SOSrvCreator* getSrvCreator(void);

	// server watch object
	virtual SOSrvWatch* addWatchObject(void);

	// COM helper methods
	virtual HRESULT getClassObject(IN REFCLSID rclsid, IN REFIID riid,
								   OUT LPVOID* ppvObj);
	virtual HRESULT canUnloadNow(void);

	virtual BOOL initializeSecurity(IN DWORD authnLevel, IN DWORD impLevel,
									IN DWORD cbAuthnLevel, IN DWORD cbImpLevel,
									IN DWORD capabilities);

	virtual HRESULT revokeClassObjects(void);
	virtual BOOL addComObjMapEntry(IN const CLSID* pClsid, IN void* pfnUpdateRegistry,
								   IN void* pfnGetClassObject, IN void* pfnCreateInstance, IN void* pfnGetObjectDescription,
								   IN void* pfnGetCategoryMap, IN void* pfnObjectMain);
	virtual BOOL addComObjMapEntry(IN const CLSID* pClsid, IN DWORD cnt, IN DWORD specID);

protected:
	SOCmnPointer<SODaSEntry> m_entryDA;
	SOCmnPointer<SOAeSEntry> m_entryAE;
	SOCmnPointer<SODaSServer> m_surveyServer;
	SOCmnPointer<SOSrvCreator> m_creator;

	virtual ~SOAsSEntry(void);

	// ATL COM helper methods
	virtual void term(void);
	virtual void init(HINSTANCE instanceHandle);
	virtual HRESULT registerClassObjects(void);
	virtual HRESULT regServer(void);
	virtual HRESULT unregServer(void);
	virtual HRESULT updateRegistryWithRGS(IN BOOL doRegister, IN void* replacementArray);
}; // SOAsSEntry

inline SODaSEntry* SOAsSEntry::getDAEntry(void)
{
	m_entryDA.addRef();
	return (SODaSEntry*)m_entryDA;
}

inline SOAeSEntry* SOAsSEntry::getAEEntry(void)
{
	m_entryAE.addRef();
	return (SOAeSEntry*)m_entryAE;
}

inline SODaSServer* SOAsSEntry::getSurveyServer(void)
{
	m_surveyServer.addRef();
	return (SODaSServer*)m_surveyServer;
}




//-----------------------------------------------------------------------------
// Global Functions                                                           |
//-----------------------------------------------------------------------------

// get pointer of the entry object
SOASS_EXPORT SOAsSEntry* getSOAsSEntry(void);

#pragma pack(pop)
#endif
