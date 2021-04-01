#include "stdafx.h"

#ifdef OTB_XMLDA_SERVER

#ifdef OTB_SERVER

#include "SOSrvHttpEx.h"
#include "WebServer.h"
#include "SrvApp.h"
#include "SOVersion.h"

CWebServer::CWebServer(void)
{
}

SOSrvWebExpandData* CWebServer::createExpandData(void)
{
	CWebServerExpandData* exData;
	SOCMN_ALLOCATE_OBJECT_C(exData, CWebServerExpandData(this), (CWebServerExpandData*));
	return exData;
}


BOOL CWebServer::handleTemplate(
	LPCTSTR templ,
	SOCmnStringList* pArgsList,
	SOSrvWebExpandData* /* pExData */,
	SOCmnStringEx& result)
{
	BOOL ret = TRUE;

	if (_tcsncmp(templ, _T("GETVERSION"), 10) == 0)
	{
#ifdef SOOS_WINDOWS
		result.format(_T("%u.%2.2u.%u.%4.4u"), MAJOR_VERSION_NUMBER, MINOR_VERSION_NUMBER, PATCH_VERSION_NUMBER, BUILD_NUMBER);
#else
		result.format(_T("%u.%2.2u.%u.%4.4u"), MAJOR_VERSION_NUMBER, MINOR_VERSION_NUMBER, PATCH_VERSION_NUMBER, BUILD_NUMBER);
#endif
	}
	else if (_tcsncmp(templ, _T("CLIENTCONNECTIONS##"), 19) == 0)
	{
		SOCmnString type = pArgsList->pop();
		SOCmnString language = pArgsList->pop();
		getClientConnections(type, language, result);
	}
	else
	{
		ServerApp* pApp = ::getServerApp();

		if (pApp->m_callbackFunctions.m_OTSWebHandleTemplate)
		{
			DWORD numArgs = pArgsList->getCount();
			LPTSTR* pArgs = NULL;
			LPTSTR pResult = NULL;
			SOCmnListPosition posArgs;
			DWORD i = 0;
			SOCmnString arg;

			if (numArgs > 0)
			{
				pArgs = (LPTSTR*)OTAllocateMemory(sizeof(LPTSTR) * numArgs);
			}

			posArgs = pArgsList->getStartPosition();

			while (posArgs)
			{
				arg = pArgsList->getNext(posArgs);
				pArgs[i] = (LPTSTR)OTAllocateMemory(sizeof(TCHAR) * (DWORD)(_tcslen(arg) + 1));
				_tcscpy(pArgs[i], arg);
				i++;
			}

			SOCMN_TRY
			{
				ret = SUCCEEDED(pApp->m_callbackFunctions.m_OTSWebHandleTemplate((LPTSTR)(LPCTSTR)templ, numArgs, pArgs, &pResult));
			}
			SOCMN_CATCH(...)
			{
				_TRACE(SOCMNTRACE_L_ERR, SOCMNTRACE_G_OPC_SERVER, (_T("CWebServer::handleTemplate - Exception occurred in vendor application callback: OTSWebHandleTemplate")));
			}

			if (ret)
			{
				result = pResult;
				OTFreeMemory(pResult);
			}

			for (i = 0; i < numArgs; i++)
			{
				OTFreeMemory(pArgs[i]);
			}

			if (numArgs > 0)
			{
				OTFreeMemory(pArgs);
			}
		}
		else
		{
			ret = FALSE;
		}
	}

	return ret;
}

void CWebServer::getClientConnections(
	LPCTSTR /* type */,
	LPCTSTR /* language */,
	SOCmnStringEx& result)
{
#ifdef OTB_SERVER_DA
	{
		SOCmnPointer<SODaSEntry> entry = getSODaSEntry();
		SOCmnPointer<SOSrvObjectRoot> objectTree = entry->getObjectRoot();
		SOCmnList<SOCmnElement> serverList(objectTree->getBranchList());
		SOCmnPointer<SODaSServer> server;
		SOCmnListPosition serverPos;
		SOCmnList<SOCmnElement> grpList;
		SOCmnPointer<SODaSGroup> grp;
		SOCmnListPosition grpPos;
		DWORD grpCnt;
		DWORD itemCnt;
		SOCmnString serverName;
		SOCmnStringEx line;
		SOCmnStringEx soap;
		SOCmnStringEx protocol;

		serverList.dontAutoLock();
		serverList.lock();
		serverList.setDirty(FALSE);
		serverPos = serverList.getStartPosition();
		serverList.unlock();

		while (serverPos)
		{
			serverList.lock();

			if (serverList.isDirty())
			{
				// check current position!
				SOCmnListPosition checkPos;
				BOOL found = FALSE;
				checkPos = serverList.getStartPosition();

				while (checkPos)
				{
					if (checkPos == serverPos)
					{
						found = TRUE;
						break;
					}

					serverList.getNext(checkPos);
				}

				if (!found)
				{
					serverPos = serverList.getStartPosition();
					result.empty();
				}

				serverList.setDirty(FALSE);

				if (serverPos == NULL)
				{
					serverList.unlock();
					break;
				}
			}

			server = (SODaSServer*)serverList.getNext(serverPos);
			server.addRef();
			serverList.unlock();

			if ((server->getFlags() & (SODASSERVER_FLAG_DCOM | SODASSERVER_FLAG_SOAP | SODASSERVER_FLAG_TUNNEL)) == 0)
			{
				continue;
			}

			serverName = server->getClientName();
			grpCnt = 0;
			itemCnt = 0;
			grpList = server->getBranchList();
			grpList.dontAutoLock();
			grpList.lock();
			grpList.setDirty(FALSE);
			grpPos = grpList.getStartPosition();
			grpList.unlock();

			while (grpPos)
			{
				grpList.lock();

				if (grpList.isDirty())
				{
					// check current position!
					SOCmnListPosition checkPos;
					BOOL found = FALSE;
					checkPos = grpList.getStartPosition();

					while (checkPos)
					{
						if (checkPos == grpPos)
						{
							found = TRUE;
							break;
						}

						grpList.getNext(checkPos);
					}

					if (!found)
					{
						grpPos = grpList.getStartPosition();
						grpCnt = 0;
						itemCnt = 0;
					}

					grpList.setDirty(FALSE);

					if (grpPos == NULL)
					{
						grpList.unlock();
						break;
					}
				}

				grp = (SODaSGroup*)grpList.getNext(grpPos);
				grp.addRef();
				grpList.unlock();
				grpCnt++;
				itemCnt += grp->getLeafList()->getCount();
			}

			if (server->getFlags() & (SODASSERVER_FLAG_DCOM))
			{
				protocol = _T("opcda");
			}
			else if (server->getFlags() & (SODASSERVER_FLAG_SOAP))
			{
				protocol = _T("http");
			}
			else if (server->getFlags() & (SODASSERVER_FLAG_TUNNEL))
			{
				protocol = _T("tpda");
			}

			if ((server->getFlags() & SODASSERVER_FLAG_HTTP_RECEIVER) == 0)
			{
				line.format(_T("<tr><td>%s</td><td>%s</td><td>%lu</td><td>%lu</td></tr>"), (LPCTSTR)protocol, (LPCTSTR)serverName, grpCnt, itemCnt);

				if (server->getFlags() & SODASSERVER_FLAG_SOAP)
				{
					soap += line;
				}
				else
				{
					result += line;
				}
			}
			else
			{
				line.format(_T("<tr><td>%s</td><td>%s</td></tr>"), (LPCTSTR)protocol, (LPCTSTR)serverName);
				soap += line;
			}
		}

		result += soap;
	}
#endif // OTB_SERVER_DA
#ifdef OTB_SERVER_AE
	{
		SOCmnPointer<SOAeSEntry> entry = getSOAeSEntry();
		SOCmnPointer<SOSrvObjectRoot> objectTree = entry->getObjectRoot();
		SOCmnList<SOCmnElement> serverList(objectTree->getBranchList());
		SOCmnPointer<SOAeSServer> server;
		SOCmnListPosition serverPos;
		SOCmnList<SOCmnElement> subList;
		SOCmnPointer<SOAeSSubscription> sub;
		SOCmnListPosition subPos;
		DWORD subCnt;
		SOCmnString serverName;
		SOCmnStringEx line;
		SOCmnStringEx protocol;

		serverList.dontAutoLock();
		serverList.lock();
		serverList.setDirty(FALSE);
		serverPos = serverList.getStartPosition();
		serverList.unlock();

		while (serverPos)
		{
			serverList.lock();

			if (serverList.isDirty())
			{
				// check current position!
				SOCmnListPosition checkPos;
				BOOL found = FALSE;
				checkPos = serverList.getStartPosition();

				while (checkPos)
				{
					if (checkPos == serverPos)
					{
						found = TRUE;
						break;
					}

					serverList.getNext(checkPos);
				}

				if (!found)
				{
					serverPos = serverList.getStartPosition();
					result.empty();
				}

				serverList.setDirty(FALSE);

				if (serverPos == NULL)
				{
					serverList.unlock();
					break;
				}
			}

			server = (SOAeSServer*)serverList.getNext(serverPos);
			server.addRef();
			serverList.unlock();
			serverName = server->getClientName();
			subCnt = 0;
			subList = server->getLeafList();
			subList.dontAutoLock();
			subList.lock();
			subList.setDirty(FALSE);
			subPos = subList.getStartPosition();
			subList.unlock();

			while (subPos)
			{
				subList.lock();

				if (subList.isDirty())
				{
					// check current position!
					SOCmnListPosition checkPos;
					BOOL found = FALSE;
					checkPos = subList.getStartPosition();

					while (checkPos)
					{
						if (checkPos == subPos)
						{
							found = TRUE;
							break;
						}

						subList.getNext(checkPos);
					}

					if (!found)
					{
						subPos = subList.getStartPosition();
						subCnt = 0;
					}

					subList.setDirty(FALSE);

					if (subPos == NULL)
					{
						subList.unlock();
						break;
					}
				}

				sub = (SOAeSSubscription*)subList.getNext(subPos);
				sub.addRef();
				subList.unlock();
				subCnt++;
			}

			if (server->getFlags() & (SOAESSERVER_FLAG_DCOM))
			{
				protocol = _T("opcae");
			}
			else if (server->getFlags() & (SOAESSERVER_FLAG_TUNNEL))
			{
				protocol = _T("tpae");
			}

			line.format(_T("<tr><td>%s</td><td>%s</td><td>%lu</td><td>-</td></tr>"), (LPCTSTR)protocol, (LPCTSTR)serverName, subCnt);
			result += line;
		}
	}
#endif // OTB_SERVER_AE
}



SOSrvWebTrace* CWebServer::getWebTrace(void)
{
	return getServerApp()->getWebTrace();
}

LPCTSTR CWebServer::getTraceLevelDescription(IN DWORD traceLevel, IN LPCTSTR format, OUT SOCmnStringEx& descr)
{
	return getTraceLevelDescriptionS(traceLevel, format, descr);
}

LPCTSTR CWebServer::getTraceLevelDescriptionS(IN DWORD traceLevel, IN LPCTSTR format, OUT SOCmnStringEx& descr)
{
	switch (traceLevel)
	{
	case SOCMNTRACE_G_ALL:
		if (_tcscmp(format, _T("GERMAN")) == 0)
		{
			descr = _T("ALLES");
		}
		else
		{
			descr = _T("ALL");
		}

		break;

	case SOCMNTRACE_G_NOTHING:
		if (_tcscmp(format, _T("GERMAN")) == 0)
		{
			descr = _T("NICHTS");
		}
		else
		{
			descr = _T("NOTHING");
		}

		break;

	default:
		descr.empty();

		if ((traceLevel & SOCMNTRACE_G_USER) == SOCMNTRACE_G_USER)
		{
			descr += _T("USER");
		}

		if ((traceLevel & SOCMNTRACE_G_OPC) == SOCMNTRACE_G_OPC)
		{
			if (!descr.isEmpty())
			{
				descr += _T(" + ");
			}

			descr += _T("OPC");
		}
		else
		{
			if ((traceLevel & SOCMNTRACE_G_OPC_CLIENT) == SOCMNTRACE_G_OPC_CLIENT)
			{
				if (!descr.isEmpty())
				{
					descr += _T(" + ");
				}

				descr += _T("CLIENT");
			}

			if ((traceLevel & SOCMNTRACE_G_OPC_SERVER) == SOCMNTRACE_G_OPC_SERVER)
			{
				if (!descr.isEmpty())
				{
					descr += _T(" + ");
				}

				descr += _T("SERVER");
			}
		}

		if (!descr.isEmpty())
		{
			DWORD tlDescr = getTraceLevelFromDescriptionS(descr);

			if (tlDescr != traceLevel)
			{
				descr.empty();
			}
		}

		if (descr.isEmpty())
		{
			descr.format(_T("0x%8.8lX"), traceLevel);
		}

		break;
	}

	return descr;
}

DWORD CWebServer::getTraceLevelFromDescription(IN LPCTSTR descr)
{
	return getTraceLevelFromDescriptionS(descr);
}

DWORD CWebServer::getTraceLevelFromDescriptionS(IN LPCTSTR descr)
{
	SOCmnString d(descr);
	d.makeUpper();

	// description names
	if (_tcsncmp(d, _T("0X"), 2) != 0)
	{
		DWORD tl = 0;

		if (d.find(_T("ALL")) != -1)
		{
			return SOCMNTRACE_G_ALL;
		}

		if (d.find(_T("SERVER")) != -1)
		{
			tl |= SOCMNTRACE_G_OPC_SERVER;
		}

		if (d.find(_T("CLIENT")) != -1)
		{
			tl |= SOCMNTRACE_G_OPC_CLIENT;
		}

		if (d.find(_T("OPC")) != -1)
		{
			tl |= SOCMNTRACE_G_OPC;
		}

		if (d.find(_T("USER")) != -1)
		{
			tl |= SOCMNTRACE_G_USER;
		}

		return tl;
	}

	return _tcstoul(descr, NULL, 0);
}

WORD CWebServer::checkAccess(
	IN SOSrvHTTPRequest* pReq,
	IN SOSrvHTTPHandlerData* /* pHTTPData */,
	IN LPCTSTR /* filePath */,
	OUT SOCmnString* httpAttributes)
{
	SOCmnStringList attributeList(pReq->getAttributeList());
	SOCmnString url(pReq->getURL());
	SOCmnString cookie;
	attributeList.findKey(_T("cookie"), &cookie);

	if (url.find(_T("logout")) != -1)
	{
		if (url.find(_T("German")) != -1)
		{
			*httpAttributes += _T("Location: /indexGerman.html\r\nSet-Cookie: userLevel=none; path=/\r\n");
		}
		else
		{
			*httpAttributes += _T("Location: /index.html\r\nSet-Cookie: userLevel=none; path=/\r\n");
		}

		return 303;
	}

	if ((url.find(_T("cfg_")) != -1) || (url.find(_T("diag_")) != -1))
	{
		if (url.find(_T("cfg_")) != -1)
		{
			if (cookie.find(_T("userLevel=administrator")) != -1)
			{
				return SOCMNHTTP_CODE_OK;
			}
			else
			{
				return SOCMNHTTP_CODE_FORBIDDEN;
			}
		}
		else if (url.find(_T("diag_")) != -1)
		{
			if ((cookie.find(_T("userLevel=administrator")) != -1) || (cookie.find(_T("userLevel=operator")) != -1))
			{
				return SOCMNHTTP_CODE_OK;
			}
			else
			{
				return SOCMNHTTP_CODE_FORBIDDEN;
			}
		}
	}

	return SOCMNHTTP_CODE_OK;
}


BOOL CWebServer::handleLOGIN(
	LPCTSTR command,
	IN SOSrvHTTPRequest* pReq,
	IN SOSrvHTTPHandlerData* pHTTPData,
	SOCmnStringList* pArgsList,
	SOSrvWebExpandData* pExData,
	SOCmnStringEx& /* result */,
	OUT SOCmnString* /* type */)
{
	if (_tcsncmp(command, _T("LOGIN"), 5) == 0)
	{
		SOCmnString language;
		BOOL isGerman = FALSE;

		if (_tcsncmp(command, _T("LOGIN##GERMAN"), 13) == 0)
		{
			language = _T("G");
			isGerman = TRUE;
		}

		ServerApp* pApp = getServerApp();
		SOCmnString user;
		SOCmnString pwd;
		SOCmnDateTime now;
		SOCmnStringEx szDefResAttrs;
		SOCmnString loginError;
		pArgsList->findKey(_T("USER"), &user);
		pArgsList->findKey(_T("PASSWORD"), &pwd);
		now.now();
		getResponseAttributes(pHTTPData->m_flags, szDefResAttrs);

		if (_tcsicmp(user, _T("administrator")) == 0)
		{
			if (pwd == pApp->m_webAdministratorPassword)
			{
				pExData->m_httpResponse.format(_T("HTTP/1.1 303 See Other\r\nLocation: /cfg_index%s.html\r\nServer: %s\r\nDate: %s\r\n%sContent-Length: 13\r\nContent-Type: text/plain\r\nSet-Cookie: userLevel=administrator; path=/\r\n\r\n303 See Other"), (LPCTSTR)language, (LPCTSTR)m_httpServerDescription, (LPCTSTR)now.getHTTPDate(), (LPCTSTR)szDefResAttrs);
				return TRUE;
			}

			if (isGerman)
			{
				loginError = _T("FEHLER: Ungueltiges Passwort");
			}
			else
			{
				loginError = _T("ERROR: Invalid password");
			}
		}
		else if (_tcsicmp(user, _T("operator")) == 0)
		{
			if (pwd == pApp->m_webOperatorPassword)
			{
				pExData->m_httpResponse.format(_T("HTTP/1.1 303 See Other\r\nLocation: /diag_index%s.html\r\nServer: %s\r\nDate: %s\r\n%sContent-Length: 13\r\nContent-Type: text/plain\r\nSet-Cookie: userLevel=operator; path=/\r\n\r\n303 See Other"), (LPCTSTR)language, (LPCTSTR)m_httpServerDescription, (LPCTSTR)now.getHTTPDate(), (LPCTSTR)szDefResAttrs);
				return TRUE;
			}

			if (isGerman)
			{
				loginError = _T("FEHLER: Ungueltiges Passwort");
			}
			else
			{
				loginError = _T("ERROR: Invalid password");
			}
		}
		else
		{
			if (isGerman)
			{
				loginError = _T("FEHLER: Ungueltiger Benutzer");
			}
			else
			{
				loginError = _T("ERROR: Invalid user");
			}
		}

		SOCmnStringList attributeList(pReq->getAttributeList());
		SOCmnString cookie;
		attributeList.findKey(_T("cookie"), &cookie);

		if (cookie.find(_T("userLevel=administrator")) != -1)
		{
			pExData->m_httpResponse.format(_T("HTTP/1.1 303 See Other\r\nLocation: /cfg_index_login%s.html\r\nServer: %s\r\nDate: %s\r\n%sContent-Length: 13\r\nContent-Type: text/plain\r\nSet-Cookie: message=%s; path=/\r\n\r\n303 See Other"), (LPCTSTR)language, (LPCTSTR)m_httpServerDescription, (LPCTSTR)now.getHTTPDate(), (LPCTSTR)szDefResAttrs, (LPCTSTR)loginError);
		}
		else if (cookie.find(_T("userLevel=operator")) != -1)
		{
			pExData->m_httpResponse.format(_T("HTTP/1.1 303 See Other\r\nLocation: /diag_index_login%s.html\r\nServer: %s\r\nDate: %s\r\n%sContent-Length: 13\r\nContent-Type: text/plain\r\nSet-Cookie: message=%s; path=/\r\n\r\n303 See Other"), (LPCTSTR)language, (LPCTSTR)m_httpServerDescription, (LPCTSTR)now.getHTTPDate(), (LPCTSTR)szDefResAttrs, (LPCTSTR)loginError);
		}
		else
		{
			pExData->m_httpResponse.format(_T("HTTP/1.1 303 See Other\r\nLocation: /index_login%s.html\r\nServer: %s\r\nDate: %s\r\n%sContent-Length: 13\r\nContent-Type: text/plain\r\nSet-Cookie: message=%s; path=/\r\n\r\n303 See Other"), (LPCTSTR)language, (LPCTSTR)m_httpServerDescription, (LPCTSTR)now.getHTTPDate(), (LPCTSTR)szDefResAttrs, (LPCTSTR)loginError);
		}

		return TRUE;
	}

	return FALSE;
}


CWebServerExpandData::CWebServerExpandData(SOSrvWeb* pServer)
	: SOSrvWebExpandData(pServer)
{
}

void CWebServerExpandData::init(
	IN SOSrvHTTPRequest* pReq,
	IN SOSrvHTTPHandlerData* /* pHTTPData */,
	IN SOCmnString* httpAttributes)
{
	SOCmnString url(pReq->getURL());

	if (url != _T("/login.html"))
	{
		return;
	}

	SOCmnStringList attributeList(pReq->getAttributeList());
	SOCmnString cookie;

	if (attributeList.findKey(_T("cookie"), &cookie))
	{
		int msgIdx = cookie.find(_T("message="));

		if (msgIdx != -1)
		{
			int msgEndIdx;
			msgEndIdx = cookie.find(_T(';'), msgIdx + 8);

			if (msgEndIdx == -1)
			{
				m_message = cookie.right(cookie.getLength() - (msgIdx + 8));
			}
			else
			{
				m_message = cookie.mid(msgIdx + 8, msgEndIdx - (msgIdx + 8));
			}

			*httpAttributes += _T("Set-Cookie: message=delete; expires=Tuesday, 01-Jan-80 00:00:00 GMT; path=/\r\n");
		}
	}
}

void CWebServerExpandData::getQualityFormaterDefault(LPCTSTR formater, SOCmnStringEx& result)
{
	if (_tcsicmp(formater, _T("color")) == 0)
	{
		result = _T("#777777");
	}
}

void CWebServerExpandData::getQualityFormater(WORD quality, LPCTSTR formater, SOCmnStringEx& result)
{
	if (_tcsicmp(formater, _T("color")) == 0)
	{
		if ((quality & OPC_STATUS_MASK) >= OPC_QUALITY_GOOD)
		{
			result = _T("#000000");
		}
		else
		{
			result = _T("#999999");
		}
	}
}

void CWebServerExpandData::getQualityString(WORD quality, SOCmnStringEx& result)
{
	switch (quality & OPC_QUALITY_MASK)
	{
	case OPC_QUALITY_BAD:
		result = _T("BAD ");

		switch (quality & OPC_STATUS_MASK)
		{
		case OPC_QUALITY_CONFIG_ERROR:
			result += _T("CE");
			break;

		case OPC_QUALITY_NOT_CONNECTED:
			result += _T("NC");
			break;

		case OPC_QUALITY_DEVICE_FAILURE:
			result += _T("DF");
			break;

		case OPC_QUALITY_SENSOR_FAILURE:
			result += _T("SF");
			break;

		case OPC_QUALITY_LAST_KNOWN:
			result += _T("LK");
			break;

		case OPC_QUALITY_COMM_FAILURE:
			result += _T("CF");
			break;

		case OPC_QUALITY_OUT_OF_SERVICE:
			result += _T("OOS");
			break;
		}

		break;

	case OPC_QUALITY_UNCERTAIN:
		result = _T("UNCERTAIN ");

		switch (quality & OPC_STATUS_MASK)
		{
		case OPC_QUALITY_LAST_USABLE:
			result += _T("LU");
			break;

		case OPC_QUALITY_SENSOR_CAL:
			result += _T("SC");
			break;

		case OPC_QUALITY_EGU_EXCEEDED:
			result += _T("EE)");
			break;

		case OPC_QUALITY_SUB_NORMAL:
			result += _T("SN");
			break;
		}

		break;

	case OPC_QUALITY_GOOD:
		result = _T("GOOD ");

		switch (quality & OPC_STATUS_MASK)
		{
		case OPC_QUALITY_LOCAL_OVERRIDE:
			result += _T("LO");
			break;
		}

		break;

	default:
		result.format(_T("0x%4.4X"), quality);
		break;
	}
}

#endif // OTB_SERVER

#endif // SOFEATURE_SOAP
