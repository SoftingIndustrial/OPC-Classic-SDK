#include "stdafx.h"

#ifdef SOCLT

#ifdef SOFEATURE_DCOM

#include "SOCltServerBrowser.h"

//-----------------------------------------------------------------------------
// SOCltServerBrowser                                                         |
//-----------------------------------------------------------------------------


SOCltServerBrowser::SOCltServerBrowser()
	: SOCmnObject(SOCMNOBJECT_TYPE_OTHER)
{
	SOCmnString id;
	id.format(_T("[%s|%8.8X| %s]"), (LPCTSTR)getObjTypeString(), this->getOpcHandle(), _T("SERVER BROWSER"));
	setObjTraceId(id);
	m_serverBrowserNode = _T("localhost");
}

SOCltServerBrowser::~SOCltServerBrowser()
{
}


//-----------------------------------------------------------------------------
// includesCLSID
//
// - check if the class id list includes a class id
//
// returns:
//		TRUE  - list includes
//		FALSE - list includes not
//
BOOL SOCltServerBrowser::includesCLSID(
	IN REFCLSID clsid,          // class id to search for
	IN SOCmnList<CLSID> *classIdList,   // class id list
	IN DWORD opcSpec,
	IN SOCmnDWORDList* whatServersList)
{
	SOCmnListPosition pos;
	CLSID* cid;
	DWORD i = 0;
	pos = classIdList->getStartPosition();

	while (pos)
	{
		cid = classIdList->getNext(pos);

		if (memcmp(cid, &clsid, sizeof(CLSID)) == 0)
		{
			if (whatServersList)
			{
				DWORD whatServers;
				whatServers = whatServersList->findKey(i);
				whatServersList->update(whatServers | opcSpec, i);
			}

			return TRUE;
		}

		i++;
	}

	return FALSE;
} // includesCLSID


//-----------------------------------------------------------------------------
// createServerBrowser
//
// - create server browser object
//
// returns:
//		TRUE  - list includes
//		FALSE - list includes not
//
BOOL SOCltServerBrowser::createServerBrowser()
{
	MULTI_QI mqi[2];
	COSERVERINFO sin, *sinptr;
	DWORD clsctx;
	HRESULT res;
	USES_CONVERSION;
	BOOL ret = FALSE;

	if ((m_serverBrowser) || (m_serverBrowser2))
	{
		return TRUE;
	}

	// create server browser object via COM
	if (m_serverBrowserNode.IsEmpty() || m_serverBrowserNode == _T("localhost"))
	{
		sinptr = 0;
		clsctx = CLSCTX_LOCAL_SERVER;
	}
	else
	{
		sinptr = &sin;
		sin.dwReserved1 = 0;
		sin.dwReserved2 = 0;
		sin.pwszName = T2W((LPTSTR)((LPCTSTR)m_serverBrowserNode));
		sin.pAuthInfo = 0;
		clsctx = CLSCTX_REMOTE_SERVER;
	}

	mqi[0].pIID = &IID_IOPCServerList;
	mqi[0].hr = 0;
	mqi[0].pItf = 0;
	mqi[1].pIID = &IID_IOPCServerList2;
	mqi[1].hr = 0;
	mqi[1].pItf = 0;
	res = ::CoCreateInstanceEx(CLSID_OpcServerList, NULL, clsctx, sinptr, 2, mqi);
	_TRACE(SOCMNTRACE_L_INF, SOCMNTRACE_G_OPC_CALL_C, (_T("::CoCreateInstanceEx(CLSID_OpcServerList) [0x%X] "), res));

	if (SUCCEEDED(res))
	{
		if (SUCCEEDED(mqi[0].hr))
		{
			m_serverBrowser.p = (IOPCServerList*)mqi[0].pItf;
			ret = TRUE;
		}

		if (SUCCEEDED(mqi[1].hr))
		{
			m_serverBrowser2.p = (IOPCServerList2*)mqi[1].pItf;
			ret = TRUE;
		}
	}

	setLastError(res, SOCLT_ERR_CREATE_SERVER_BROWSER);
	return ret;
} // createServerBrowser


//-----------------------------------------------------------------------------
// browseServer
//
// - get the list of the available OPC servers
//
// returns:
//		number of available servers
//
BOOL SOCltServerBrowser::browseServer(
	IN DWORD whatServers,       // what server types to browse
	OUT SOCmnList<CLSID> &classIdList,  // result list
	OUT SOCmnDWORDList* pWhatServersList)
{
	BOOL checkDup = TRUE;
	BOOL addToList;
	HRESULT res = S_OK;
	DWORD dwErr = 0;
	BOOL bOkay = TRUE;
	DWORD numServer = 0;

	if (!classIdList.isInit())
	{
		classIdList.create(SOCMNLIST_TYPE_ORG_LIST | SOCMNLIST_TYPE_KEY_NO | SOCMNLIST_TYPE_OBJ_VOID);
	}

	if (pWhatServersList)
	{
		pWhatServersList->destroy();
		pWhatServersList->create(SOCMNLIST_TYPE_ORG_MAP | SOCMNLIST_TYPE_KEY_DWORD | SOCMNLIST_TYPE_OBJ_DWORD);
	}

	if ((whatServers & SOCLT_BROWSE_SERVER_REG_DA10A) == SOCLT_BROWSE_SERVER_REG_DA10A)
	{
		// read server entries form the registry
		checkDup = TRUE; // a servers CLSID could be found twice in the registry

		if (m_serverBrowserNode.IsEmpty() || m_serverBrowserNode == _T("localhost"))
		{
			// only local server browsing over registry
			HKEY hProgID;
			HKEY hKeyOPC;
			TCHAR progId[MAX_PATH + 1];
			TCHAR clsid[MAX_PATH + 1];
			DWORD dwIndex;
			CLSID classId;
			DWORD size = 0;
			DWORD enumsize = MAX_PATH + 1;
			USES_CONVERSION;

			// search HKEY_CLASSES_ROOT for ProgId entry with subkey OPC

			for (dwIndex = 0; ERROR_SUCCESS == (dwErr = ::RegEnumKeyEx(HKEY_CLASSES_ROOT, dwIndex, progId, &enumsize, NULL, NULL, NULL, NULL)); dwIndex++)
			{
				// Ignore standard COM subtrees as well as shell entries starting
				// with a dot
				enumsize = MAX_PATH + 1;

				if (0 == _tcscmp(progId, _T("CLSID")) ||
					0 == _tcscmp(progId, _T("AppID")) ||
					0 == _tcscmp(progId, _T("Interfaces")) ||
					_T('.') == progId[0])
				{
					continue;
				}

				dwErr = ::RegOpenKeyEx(HKEY_CLASSES_ROOT, progId, 0, KEY_READ, &hProgID);

				if (ERROR_SUCCESS == dwErr)
				{
					size = MAX_PATH + 1;

					if (::RegOpenKeyEx(hProgID, _T("OPC"), 0, KEY_READ, &hKeyOPC) == ERROR_SUCCESS)
					{
						RegCloseKey(hKeyOPC);

						if (::RegOpenKeyEx(hProgID, _T("CLSID"), 0, KEY_READ, &hKeyOPC) == ERROR_SUCCESS)
						{
							size = MAX_PATH + 1;

							if (RegQueryValueEx(hKeyOPC, _T(""), NULL, NULL, (LPBYTE)clsid, &size) == ERROR_SUCCESS)
							{
								CLSIDFromString(T2W((LPTSTR)(LPCTSTR)clsid), &classId);
								addToList = TRUE;

								if (checkDup)
								{
									addToList = !includesCLSID(classId, &classIdList, SOCLT_BROWSE_SERVER_REG_DA10A, pWhatServersList);
								}

								if (addToList)
								{
									classIdList.addMemory(&classId, sizeof(CLSID));

									if (pWhatServersList)
									{
										pWhatServersList->add(SOCLT_BROWSE_SERVER_REG_DA10A, numServer);
									}

									numServer++;
								}
							}

							RegCloseKey(hKeyOPC);
						}
					}

					RegCloseKey(hProgID);
				}
			}

			if (dwErr == ERROR_NO_MORE_ITEMS)
			{
				dwErr = ERROR_SUCCESS;
			}
		}
	}

	res = HRESULT_FROM_WIN32(dwErr);
	setLastError(res, SOCLT_ERR_ENUM_SERVERS);
	bOkay &= SUCCEEDED(res);

	if ((whatServers & SOCLT_BROWSE_SERVER_CAT_ALL) != 0)
	{
		// browse servers with opcenum.exe (OPC V2.0)
		bOkay = createServerBrowser();

		if (bOkay)
		{
			IEnumGUID* enumGUID = NULL;
			IOPCEnumGUID* enumGUIDOPC = NULL;
			CLSID catid;
			CLSID classId;
			ULONG c;
			BOOL doWork;
			int i;
			SOCmnString clsidSZ;
			DWORD opcSpec = 0;

			for (i = 0; i < 4; i++)
			{
				doWork = FALSE;

				if ((i == 0) && (whatServers & SOCLT_BROWSE_SERVER_CAT_DA10A))
				{
					// OPC DA V1.0A servers
					catid = CATID_OPCDAServer10;
					doWork = TRUE;
					checkDup = !classIdList.IsEmpty();
					opcSpec = SOCLT_BROWSE_SERVER_CAT_DA10A;
				}

				if ((i == 1) && (whatServers & SOCLT_BROWSE_SERVER_CAT_DA20))
				{
					// OPC DA V2.0 servers
					catid = CATID_OPCDAServer20;
					doWork = TRUE;
					checkDup = !classIdList.IsEmpty();
					opcSpec = SOCLT_BROWSE_SERVER_CAT_DA20;
				}

				if ((i == 2) && (whatServers & SOCLT_BROWSE_SERVER_CAT_DA30))
				{
					// OPC DA 3.0 servers
					catid = CATID_OPCDAServer30;
					doWork = TRUE;
					checkDup = !classIdList.IsEmpty();
					opcSpec = SOCLT_BROWSE_SERVER_CAT_DA30;
				}

				if ((i == 3) && (whatServers & SOCLT_BROWSE_SERVER_CAT_AE10))
				{
					// OPC AE V1.0 servers
					catid = IID_OPCEventServerCATID;
					doWork = TRUE;
					checkDup = !classIdList.IsEmpty();
					opcSpec = SOCLT_BROWSE_SERVER_CAT_AE10;
				}

				if (doWork)
				{
					// get server class ids
					if (m_serverBrowser2)
					{
						res = m_serverBrowser2->EnumClassesOfCategories(1, &catid, 0, NULL, &enumGUIDOPC);
						_TRACE(SOCMNTRACE_L_INF, SOCMNTRACE_G_OPC_CALL_C, (_T("IOPCServerList2::EnumClassesOfCategories [0x%X] "), res));
					}
					else
					{
						res = m_serverBrowser->EnumClassesOfCategories(1, &catid, 0, NULL, &enumGUID);
						_TRACE(SOCMNTRACE_L_INF, SOCMNTRACE_G_OPC_CALL_C, (_T("IOPCServerList::EnumClassesOfCategories [0x%X] "), res));
					}

					if (SUCCEEDED(res))
					{
						while (SUCCEEDED(res))
						{
							if (m_serverBrowser2)
							{
								res = enumGUIDOPC->Next(1, &classId, &c);

								if (res == S_OK)
								{
									_TRACE(SOCMNTRACE_L_DEB, SOCMNTRACE_G_OPC_CALL_C, (_T("IOPCEnumGuid::Next %s [0x%X] "), clsidSZ.format(_T("{%8.8X-%4.4X-%4.4X-%2.2X%2.2X-%2.2X%2.2X%2.2X%2.2X%2.2X%2.2X}"), classId.Data1, classId.Data2, classId.Data3, classId.Data4[0], classId.Data4[1], classId.Data4[2], classId.Data4[3], classId.Data4[4], classId.Data4[5], classId.Data4[6], classId.Data4[7]), res));
								}
								else
								{
									_TRACE(SOCMNTRACE_L_ERR, SOCMNTRACE_G_OPC_CALL_C, (_T("IOPCEnumGuid::Next [0x%X] "), res));
								}
							}
							else
							{
								res = enumGUID->Next(1, &classId, &c);

								if (res == S_OK)
								{
									_TRACE(SOCMNTRACE_L_DEB, SOCMNTRACE_G_OPC_CALL_C, (_T("IEnumGuid::Next %s [0x%X] "), clsidSZ.format(_T("{%8.8X-%4.4X-%4.4X-%2.2X%2.2X-%2.2X%2.2X%2.2X%2.2X%2.2X%2.2X}"), classId.Data1, classId.Data2, classId.Data3, classId.Data4[0], classId.Data4[1], classId.Data4[2], classId.Data4[3], classId.Data4[4], classId.Data4[5], classId.Data4[6], classId.Data4[7]), res));
								}
								else
								{
									_TRACE(SOCMNTRACE_L_ERR, SOCMNTRACE_G_OPC_CALL_C, (_T("IEnumGuid::Next [0x%X] "), res));
								}
							}

							if (FAILED(res))
							{
								break;
							}

							if (c == 0)
							{
								break;
							}

							addToList = TRUE;

							if (checkDup)
							{
								addToList = !includesCLSID(classId, &classIdList, opcSpec, pWhatServersList);
							}

							if (addToList)
							{
								classIdList.addMemory(&classId, sizeof(CLSID));

								if (pWhatServersList)
								{
									pWhatServersList->add(opcSpec, numServer);
								}

								numServer++;
							}
						}

						if (enumGUID)
						{
							enumGUID->Release();
						}

						if (enumGUIDOPC)
						{
							enumGUIDOPC->Release();
						}
					}
				}
			}
		}
	}

	setLastError(res, SOCLT_ERR_ENUM_SERVERS);
	bOkay &= SUCCEEDED(res);
	return bOkay;
} // browseServer


#ifdef SOOS_WINDOWS_CE
HRESULT SORegGetUserType(REFCLSID clsid, LPWSTR* ppszUserType)
{
	CRegKey keyCLSID;
	CRegKey keyClass;
	DWORD ret;
	HRESULT res = S_OK;
	WCHAR* wszClsid;
	USES_CONVERSION;
	ret = keyCLSID.Open(HKEY_CLASSES_ROOT, _T("CLSID"), KEY_READ);

	if (ret == ERROR_SUCCESS)
	{
		StringFromCLSID(clsid, &wszClsid);
		ret = keyClass.Open(keyCLSID, W2T(wszClsid), KEY_READ);

		if (ret == ERROR_SUCCESS)
		{
			ULONG size;
			ret = keyClass.QueryValue(NULL, _T(""), &size);

			if (ret == ERROR_SUCCESS)
			{
				*ppszUserType = (WCHAR*)CoTaskMemAlloc(size * sizeof(WCHAR));
				ret = keyClass.QueryValue(*ppszUserType, _T(""), &size);
			}

			if (ret != ERROR_SUCCESS)
			{
				StringFromCLSID(clsid, ppszUserType);
			}
		}
		else
		{
			res = REGDB_E_CLASSNOTREG;
		}

		CoTaskMemFree(wszClsid);
	}
	else
	{
		res = REGDB_E_READREGDB;
	}

	return res;
}
#endif


//-----------------------------------------------------------------------------
// getServerDescription
//
// - get the description of the OPC server
//
// returns:
//		TRUE  - got server description
//		FALSE - can't get server description
//
BOOL SOCltServerBrowser::getServerDescription(
	IN REFCLSID clsid,      // class id of the server
	OUT SOCmnString& descr)     // description
{
	LPWSTR userType;
	BOOL ret = FALSE;
	HRESULT res = E_FAIL;
	USES_CONVERSION;

	if (m_serverBrowserNode.IsEmpty() || m_serverBrowserNode == _T("localhost"))
	{
		// local: read it from the registry
#ifndef SOOS_WINDOWS_CE
		if (SUCCEEDED(res = ::OleRegGetUserType(clsid, USERCLASSTYPE_FULL, &userType)))
#else
		if (SUCCEEDED(res = ::SORegGetUserType(clsid, &userType)))
#endif
		{
			descr = W2CT(userType);
			CoTaskMemFree(userType);
			ret = TRUE;
		}
	}
	else
	{
		// remote: ask the server browser object
		ret = createServerBrowser();

		if (ret == TRUE)
		{
			LPWSTR pid;
			LPWSTR vipid = NULL;

			if (m_serverBrowser2)
			{
				res = m_serverBrowser2->GetClassDetails(clsid, &pid, &userType, &vipid);
				_TRACE(SOCMNTRACE_L_INF, SOCMNTRACE_G_OPC_CALL_C, (_T("IOPCServerList2::GetClassDetails [0x%X] "), res));
			}
			else
			{
				res = m_serverBrowser->GetClassDetails(clsid, &pid, &userType);
				_TRACE(SOCMNTRACE_L_INF, SOCMNTRACE_G_OPC_CALL_C, (_T("IOPCServerList::GetClassDetails [0x%X] "), res));
			}

			if (SUCCEEDED(res))
			{
				descr = W2CT(userType);
				CoTaskMemFree(pid);
				CoTaskMemFree(userType);

				if (vipid)
				{
					CoTaskMemFree(vipid);
				}

				ret = TRUE;
			}
		}
	}

	setLastError(res, SOCLT_ERR_GET_CLASS_DETAILS);
	return ret;
} // getServerDescription


//-----------------------------------------------------------------------------
// getServerProgId
//
// - get the prog id of the OPC server
//
// returns:
//		TRUE  - got server prog id
//		FALSE - can't get server prog id
//
BOOL SOCltServerBrowser::getServerProgId(
	IN REFCLSID clsid,      // class id of the server
	OUT SOCmnString& progId)    // progID
{
	LPWSTR pid;
	BOOL ret = FALSE;
	HRESULT res = E_FAIL;
	USES_CONVERSION;

	if (m_serverBrowserNode.IsEmpty() || m_serverBrowserNode == _T("localhost"))
	{
		// local: read it from the registry
		if (SUCCEEDED(res = ::ProgIDFromCLSID(clsid, &pid)))
		{
			progId = W2CT(pid);
			CoTaskMemFree(pid);
			ret = TRUE;
		}
	}
	else
	{
		// remote: ask the server browser object
		ret = createServerBrowser();

		if (ret == TRUE)
		{
			LPWSTR userType;
			LPWSTR vipid = NULL;

			if (m_serverBrowser2)
			{
				res = m_serverBrowser2->GetClassDetails(clsid, &pid, &userType, &vipid);
				_TRACE(SOCMNTRACE_L_INF, SOCMNTRACE_G_OPC_CALL_C, (_T("IOPCServerList2::GetClassDetails [0x%X] "), res));
			}
			else
			{
				res = m_serverBrowser->GetClassDetails(clsid, &pid, &userType);
				_TRACE(SOCMNTRACE_L_INF, SOCMNTRACE_G_OPC_CALL_C, (_T("IOPCServerList::GetClassDetails [0x%X] "), res));
			}

			if (SUCCEEDED(res))
			{
				progId = W2CT(pid);
				CoTaskMemFree(pid);
				CoTaskMemFree(userType);

				if (vipid)
				{
					CoTaskMemFree(vipid);
				}

				ret = TRUE;
			}
		}
	}

	setLastError(res, SOCLT_ERR_GET_CLASS_DETAILS);
	return ret;
} // getServerProgId


//-----------------------------------------------------------------------------
// getClassDetail
//
// - get the name, prog id and version independent prog id of the OPC server
//
// returns:
//		TRUE  - got class details
//		FALSE - can't get class details
//
BOOL SOCltServerBrowser::getClassDetail(
	IN REFCLSID clsid,        // class id of the server
	OUT SOCmnString& descr,   // description
	OUT SOCmnString& progId,  // progID
	OUT SOCmnString& vProgId) // version independent progID
{
	BOOL ret = FALSE;
	HRESULT res = E_FAIL;
	USES_CONVERSION;
	ret = createServerBrowser();

	if (ret == TRUE)
	{
		LPWSTR pid;
		LPWSTR userType;
		LPWSTR vipid = NULL;

		if (m_serverBrowser2)
		{
			res = m_serverBrowser2->GetClassDetails(clsid, &pid, &userType, &vipid);
			_TRACE(SOCMNTRACE_L_INF, SOCMNTRACE_G_OPC_CALL_C, (_T("IOPCServerList2::GetClassDetails [0x%X] "), res));
		}
		else
		{
			res = m_serverBrowser->GetClassDetails(clsid, &pid, &userType);
			_TRACE(SOCMNTRACE_L_INF, SOCMNTRACE_G_OPC_CALL_C, (_T("IOPCServerList::GetClassDetails [0x%X] "), res));
		}

		if (SUCCEEDED(res))
		{
			progId = W2CT(pid);
			descr = W2CT(userType);
			CoTaskMemFree(pid);
			CoTaskMemFree(userType);

			if (vipid)
			{
				vProgId = W2CT(vipid);
				CoTaskMemFree(vipid);
			}

			ret = TRUE;
		}
	}

	setLastError(res, SOCLT_ERR_GET_CLASS_DETAILS);
	return ret;
} // getServerProgId


//-----------------------------------------------------------------------------
// getCLSIDFromProgID
//
// - get the CLSID from the ProgID of the server
//
// returns:
//		TRUE  - got clsid
//		FALSE - can't get clsid
//
BOOL SOCltServerBrowser::getCLSIDFromProgID(
	IN LPCTSTR progID,  // progID
	OUT CLSID* clsid)   // class id of the server
{
	BOOL ret = FALSE;
	HRESULT res = E_FAIL;
	USES_CONVERSION;
	ret = createServerBrowser();

	if (ret == TRUE)
	{
		CLSID lclsid;

		if (m_serverBrowser2)
		{
			res = m_serverBrowser2->CLSIDFromProgID(T2CW(progID), &lclsid);
			_TRACE(SOCMNTRACE_L_INF, SOCMNTRACE_G_OPC_CALL_C, (_T("IOPCServerList2::CLSIDFromProgID [0x%X] "), res));
		}
		else
		{
			res = m_serverBrowser->CLSIDFromProgID(T2CW(progID), &lclsid);
			_TRACE(SOCMNTRACE_L_INF, SOCMNTRACE_G_OPC_CALL_C, (_T("IOPCServerList::CLSIDFromProgID [0x%X] "), res));
		}

		if ((SUCCEEDED(res)) && (clsid))
		{
			memcpy(clsid, &lclsid, sizeof(CLSID));
			ret = TRUE;
		}
	}

	setLastError(res, SOCLT_ERR_GET_CLSID);
	return ret;
} // getCLSIDFromProgID


SOCmnObject* SOCltServerBrowser::getObjectCltElementIError(void)
{
	return (SOCmnObject*)this;
}

#endif // SOFEATURE_DCOM

#endif // SOCLT
