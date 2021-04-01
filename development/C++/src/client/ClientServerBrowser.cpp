#include "OSCompat.h"
#include "ClientServerBrowser.h"
#include "ClientApplication.h"

#include "OTClient.h"

using namespace SoftingOPCToolboxClient;

//--------------------------------------------
//ServerBrowser
//--------------------------------------------
unsigned long ServerBrowser::m_objectIndex = 0;
ServerBrowser::ServerBrowser(tstring ipAddress)
{
	m_index     = ++m_objectIndex;
	m_ipAddress = ipAddress;
	Application::Instance()->m_serverBrowserList.insert(ServerBrowserPair(m_index, this));
} //end constructor

ServerBrowser::~ServerBrowser()
{
} //end destructor

unsigned long ServerBrowser::getUserData()
{
	return m_index;
} // end getKey;

tstring& ServerBrowser::getIpAddress()
{
	return m_ipAddress;
} //end
void ServerBrowser::setIpAddress(tstring anIpAddress)
{
	m_ipAddress = anIpAddress;
} //end

long ServerBrowser::browse(
	EnumOPCSpecification whatOPCSpecification,
	EnumServerBrowserData whatServerData,
	std::vector<ServerBrowserData*>& serverData,
	ExecutionOptions* someExecutionOptions)
{
	long result = E_FAIL;
	OTCExecutionOptions options;
	unsigned long serverDataCount = 0;
	OTCServerData* pServerData;
	unsigned long i;

	if (someExecutionOptions != NULL)
	{
		options.m_executionContext    = someExecutionOptions->getExecutionContext();
		options.m_executionType       = (unsigned char)someExecutionOptions->getExecutionType();
	} // end if
	else
	{
		options.m_executionContext = 0;
		options.m_executionType    = EnumExecutionType_SYNCHRONOUS;
	} //end else

#ifdef TBC_OS_WINDOWS
	OTChar* otcIpAddress = _tcsdup(m_ipAddress.c_str());
#endif
#ifdef TBC_OS_LINUX
	OTChar* otcIpAddress = strdup(m_ipAddress.c_str());
#endif
	result = OTCBrowseServer(
				 otcIpAddress,
				 (unsigned char)whatOPCSpecification,
				 (unsigned char)whatServerData,
				 &serverDataCount,
				 &pServerData,
				 &options);

	if (options.m_executionType == EnumExecutionType_SYNCHRONOUS)
	{
		if (SUCCEEDED(result))
		{
			serverData.resize(serverDataCount);

			for (i = 0; i < serverDataCount; i++)
			{
				tstring clsid, progId, description, vProgId, url;
				EnumOPCSpecification opcSpecifications;
				// check for empty values due to filter usage
				!pServerData[i].m_clsid ? clsid = _T("") : clsid = pServerData[i].m_clsid;
				!pServerData[i].m_progId ? progId = _T("") : progId = pServerData[i].m_progId;
				!pServerData[i].m_description ? description = _T("") : description = pServerData[i].m_description;
				!pServerData[i].m_vProgId ? vProgId = _T("") : vProgId = pServerData[i].m_vProgId;
				!pServerData[i].m_opcSpecifications ? opcSpecifications = EnumOPCSpecification_DEFAULT : opcSpecifications = (EnumOPCSpecification) pServerData[i].m_opcSpecifications;
				!pServerData[i].m_url ? url = _T("") : url = pServerData[i].m_url;
				serverData[i] = new ServerBrowserData(
					clsid,
					progId,
					description,
					vProgId,
					opcSpecifications,
					url);
				OTFreeMemory(pServerData[i].m_clsid);
				OTFreeMemory(pServerData[i].m_progId);
				OTFreeMemory(pServerData[i].m_description);
				OTFreeMemory(pServerData[i].m_vProgId);
				OTFreeMemory(pServerData[i].m_url);
			} //end for

			if (pServerData)
			{
				OTFreeMemory(pServerData);
			} //end if
		} //end if
	} //end if

	free(otcIpAddress);
	return result;
} //end browse

void ServerBrowser::onBrowseServers(
	unsigned long executionContext,
	tstring& ipAddress,
	std::vector<ServerBrowserData*>& serverData,
	long result)
{
	handleBrowseServersCompleted(
		executionContext,
		ipAddress,
		serverData,
		result);
} //end onBrowseServers

void ServerBrowser::handleBrowseServersCompleted(
	unsigned long executionContext,
	tstring& ipAddress,
	std::vector<ServerBrowserData*>& serverData,
	long result)
{
	Application::Instance()->trace(
		EnumTraceLevel_INF,
		EnumTraceGroup_CLIENT,
		_T("ObjectSpaceElement::handleBrowseServers"),
		_T(""));
} //end handleBrowseServers

//--------------------------------------------
//ServerBrowserData
//--------------------------------------------

ServerBrowserData::ServerBrowserData()
{
	m_opcSpecification = EnumOPCSpecification_DEFAULT;
} //end consctuctor

ServerBrowserData::ServerBrowserData(
	tstring clsId,
	tstring progId,
	tstring description,
	tstring progIdVersionIndependent,
	EnumOPCSpecification opcSpecification,
	tstring url)
{
	m_clsId = clsId;
	m_progId = progId;
	m_description = description;
	m_progIdVersionIndependent = progIdVersionIndependent;
	m_opcSpecification = opcSpecification;
	m_url = url;
} //end constructor

ServerBrowserData::~ServerBrowserData()
{
} //end destructor

tstring& ServerBrowserData::getClsId()
{
	return m_clsId;
} //end getClsId

tstring& ServerBrowserData::getDescription()
{
	return m_description;
} //end getDescription;

EnumOPCSpecification ServerBrowserData::getOpcSpecification()
{
	return m_opcSpecification;
} //end getOpcSpecification

tstring& ServerBrowserData::getProgId()
{
	return m_progId;
} //end getProgId

tstring& ServerBrowserData::getProgIdVersionIndependent()
{
	return m_progIdVersionIndependent;
} //end getProgIdVersionIndependent

tstring& ServerBrowserData::getUrl()
{
	return m_url;
} // end getUrl
