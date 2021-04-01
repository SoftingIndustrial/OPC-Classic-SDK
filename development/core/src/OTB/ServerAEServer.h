#ifndef _SERVERAESERVER_H_
#define _SERVERAESERVER_H_

#include "OTServer.h"

#pragma pack(push, 4)


//-----------------------------------------------------------------------------
// ServerAEServer
//-----------------------------------------------------------------------------

class ServerAEServer : public SOAeSServer
{
public:
	ServerAEServer(IN OPTIONAL BYTE serverInstance = 0);

	virtual BOOL supportEnableCondition(IN BOOL enable, IN BOOL areaOrSource);
	virtual BOOL supportTranslateToItemIDs(void);

protected:
	virtual ~ServerAEServer();

	virtual HRESULT setFlags(BYTE flags);
	virtual void onSetClientName(IN LPCTSTR clientName);

	// string based area space
	virtual BOOL isAreaName(IN LPCTSTR areaName);
	virtual BOOL isSourceName(IN LPCTSTR sourceName);
	virtual void getChildNamesOfAreaName(IN LPCTSTR areaName, BOOL areaOrSourceChilds,
										 LPCTSTR stringFilter, OUT SOCmnStringList& nameList);
	virtual BOOL browseOnlyStringBased(void);
	virtual void getConditionNamesOfSourceName(IN LPCTSTR sourceName,
											   OUT SOCmnStringList& condNameList);

	// string based (not object based) condition handling
	virtual BOOL enableConditionByAreaName(IN BOOL enable, IN LPCTSTR areaName);
	virtual BOOL enableConditionBySourceName(IN BOOL enable, IN LPCTSTR sourceName);
	virtual void refreshConditions(IN SOAeSRefresh* refresh);
	virtual HRESULT createConditionByName(IN LPCTSTR sourceName, IN LPCTSTR conditionName,
										  OUT SOAeSCondition** condition);
	virtual HRESULT ackConditionByName(IN LPCTSTR sourceName, IN LPCTSTR conditionName,
									   IN LPCTSTR ackID, IN LPCTSTR ackComment, LPFILETIME activeTime, DWORD cookie);

	// security
	virtual BOOL isPrivateSecurityAvailable(void);
	virtual BOOL logon(LPCTSTR userName, LPCTSTR password);
	virtual BOOL logoff(void);
	virtual HRESULT checkAuthorizationDCOM(IN LPCTSTR ifFunction);

	virtual HRESULT waitForAreaSpaceStartup(void);
	virtual HRESULT waitForEventSpaceStartup(void);
	virtual HRESULT waitForIOStartup(void);
};

#pragma pack(pop)
#endif
