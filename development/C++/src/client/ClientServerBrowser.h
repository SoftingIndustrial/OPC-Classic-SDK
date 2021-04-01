#ifndef _CLIENTSERVERBROWSER_H_
#define _CLIENTSERVERBROWSER_H_

#include "../Enums.h"
#include "ClientEnums.h"
#include "../ValueQT.h"
#include <vector>

#ifdef WIN64
#pragma pack(push,8)
#else
#pragma pack(push,4)
#endif

namespace SoftingOPCToolboxClient
{

class ServerBrowserData;

class TBC_EXPORT ServerBrowser
{

	//friend class Application;
	friend class OTCGlobals;

private:

	unsigned long m_index;
	static unsigned long m_objectIndex;

	void onBrowseServers(
		unsigned long executionContext,
		tstring& ipAddress,
		std::vector<ServerBrowserData*>& serverData,
		long result);

protected:

	tstring m_ipAddress;

public:

	ServerBrowser(tstring ipAddress);

	virtual ~ServerBrowser();

	virtual unsigned long getUserData(void);

	tstring& getIpAddress();
	void setIpAddress(tstring anIpAddress);

	virtual long browse(
		EnumOPCSpecification whatOPCSpecification,
		EnumServerBrowserData whatServerData,
		std::vector<ServerBrowserData*>& serverData,
		ExecutionOptions* someExecutionOptions);

	virtual void handleBrowseServersCompleted(
		unsigned long executionContext,
		tstring& ipAddress,
		std::vector<ServerBrowserData*>& serverData,
		long result);

}; // end ServerBrowser

typedef std::pair<unsigned long, ServerBrowser*> ServerBrowserPair;

class TBC_EXPORT ServerBrowserData
{

	friend class ServerBrowser;
	//friend class Application;
	friend class OTCGlobals;

private:

	tstring m_clsId;
	tstring m_progId;
	tstring m_description;
	tstring m_progIdVersionIndependent;
	EnumOPCSpecification m_opcSpecification;
	tstring m_url;

	ServerBrowserData(
		tstring clsId,
		tstring progId,
		tstring description,
		tstring progIdVersionIndependent,
		EnumOPCSpecification opcSpecification,
		tstring url);

protected:

public:

	ServerBrowserData();

	virtual ~ServerBrowserData();

	tstring& getClsId();

	tstring& getDescription();

	EnumOPCSpecification getOpcSpecification();

	tstring& getProgId();

	tstring& getProgIdVersionIndependent();

	tstring& getUrl();

}; // end ServerBrowser
};// end SoftingOPCToolboxClient

#pragma pack(pop)
#endif
