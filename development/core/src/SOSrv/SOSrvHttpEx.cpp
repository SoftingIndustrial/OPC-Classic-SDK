//-----------------------------------------------------------------------------
//                                                                            |
//                   Softing Industrial Automation GmbH                       |
//                        Richard-Reitzner-Allee 6                            |
//                           85540 Haar, Germany                              |
//                                                                            |
//                 This is a part of the Softing OPC Toolkit                  |
//        Copyright (c) 2005-2020 Softing Industrial Automation GmbH          |
//                           All Rights Reserved                              |
//                                                                            |
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//                           OPC Toolkit - SOSrv                              |
//                                                                            |
//  Filename    : SOSrvHttpEx.cpp                                             |
//  Version     : 4.47.1                                                      |
//  Date        : 27-July-2020                                                |
//                                                                            |
//  Description : HTTP server related classes                                 |
//                - SOSrvHTTP: HTTP server object                             |
//                                                                            |
//-----------------------------------------------------------------------------

#include "stdafx.h"

#ifdef SOSRV

#ifdef SOFEATURE_SOAP

#ifdef SOOS_LINUX
#include <sys/wait.h>
#endif
#include "opcerror.h"
#include "SOSrvHttpEx.h"
#include "SOSrvTraceEx.h"
#include "SOVersion.h"



SOSrvWeb::SOSrvWeb(void)
{
	m_httpReqList.create(SOCMNLIST_TYPE_ORG_LIST | SOCMNLIST_TYPE_KEY_NO | SOCMNLIST_TYPE_OBJ_OBJECT | SOCMNLIST_TYPE_BLOCKED | SOCMNLIST_TYPE_SYNCED);
	setHTTPMethod(SOCMNHTTP_METHOD_GET | SOCMNHTTP_METHOD_POST);
	SOCmnPointer<SODaSEntry> entry = ::getSODaSEntry();
	SOCmnPointer<SOSrvObjectRoot> root;
	SOCmnPointer<SODaSCreator> creator;
	creator = entry->getCreator();
	m_server = creator->createServer(FALSE, 0);
	m_server->stopWorkThread();
	m_server->setFlags(SODASSERVER_FLAG_HTTP_RECEIVER);
	m_server->setClientName(_T("Custom HTTP Receiver"));
	root = entry->getObjectRoot();
	root->addBranch(m_server);
	m_httpServerDescription = _T("Softing OPC Toolkit Web Server");
	m_rootFile = _T("/index.html");
	m_lcid = MAKELCID(MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US), SORT_DEFAULT);
	m_server->addGroup(_T("Session"), FALSE, 0xFFFFFFFF, 0, NULL, NULL, 0, &m_group, NULL, 1);
	m_workEventId = m_workThread.createEvent();
	m_workThread.start(workThreadMain, (void*)this);
}

SOSrvWeb::~SOSrvWeb(void)
{
	SOCmnPointer<SODaSEntry> entry = ::getSODaSEntry();
	SOCmnPointer<SOSrvObjectRoot> root;
	root = entry->getObjectRoot();
	m_workThread.stop();
	m_server->removeGroup(m_group->getServerHandle());
	m_group.release();
	root->removeBranch(m_server);
	m_httpReqList.destroy();
	m_server.release();
}


BOOL SOSrvWeb::setRootDirectories(IN LPCTSTR root1, IN LPCTSTR root2)
{
	m_rootDirectory1 = root1;
	m_rootDirectory2 = root2;
	return TRUE;
}

WORD SOSrvWeb::handleHTTPRequest(
	IN SOSrvHTTPHandler* pHandler,
	IN SOSrvHTTPHandlerData* pHTTPData,
	IN DWORD lenBody,
	IN BYTE* pBody)
{
	WORD ret;
	SOCmnString url = pHandler->getURL();

	if (url.find(_T("..")) != -1)
	{
		pHandler->sndHTTPErrorResponse(pHTTPData, pHTTPData->m_flags, SOCMNHTTP_CODE_NOT_FOUND);
		return SOCMNHTTP_CODE_NOT_FOUND;
	}

	switch (pHandler->getHTTPMethod())
	{
	case SOCMNHTTP_METHOD_GET:
	case SOCMNHTTP_METHOD_POST:
	{
		// queue request and signal work thread
		ret = SOCMNHTTP_CODE_OK;

		if (!queueHTTPRequest(pHandler, pHTTPData, lenBody, pBody))
		{
			ret = SOCMNHTTP_CODE_INTERNAL_SERVER_ERROR;
		}
	}
	break;

	default:
		ret = SOCMNHTTP_CODEI_NOT_HANDLED;
		break;
	}

	return ret;
}


BOOL SOSrvWeb::queueHTTPRequest(
	IN SOSrvHTTPHandler* pHandler,
	IN SOSrvHTTPHandlerData* pHTTPData,
	IN DWORD lenBody,
	IN BYTE* pBody)
{
	SOCmnPointer<SOSrvHTTPRequest> req;
	SOCMN_ALLOCATE_OBJECT_C(req, SOSrvHTTPRequest(pHandler, pHTTPData, lenBody, pBody), (SOSrvHTTPRequest*));
	SOCmnList<SOSrvHTTPRequest> reqList(m_httpReqList);

	if (reqList.add(req))
	{
		m_workThread.getEvents()->signal(m_workEventId);
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

DWORD SOSrvWeb::workThread(void)
{
	BOOL end = FALSE;
	DWORD ret;

	while (!end)
	{
		ret = m_workThread.waitForEvents(INFINITE);

		if (ret == m_workThread.getStopEventId())
		{
			// end
			end = TRUE;
		}
		else
		{
			SOCmnList<SOSrvHTTPRequest> reqList(m_httpReqList);
			SOCmnPointer<SOSrvHTTPRequest> req;

			do
			{
				req = reqList.pop();
				reqList.autoLock();

				if (req.isNotNull())
				{
					switch (req->getHTTPMethod())
					{
					case SOCMNHTTP_METHOD_GET:
						get(req);
						break;

					case SOCMNHTTP_METHOD_POST:
						post(req);
						break;
					}
				}
			}
			while (req.isNotNull());
		}
	}

	return 0;
}


DWORD SOSrvWeb::workThreadMain(void* context)
{
	return ((SOSrvWeb*)context)->workThread();
}


WORD SOSrvWeb::get(IN SOSrvHTTPRequest* pReq)
{
	SOSrvHTTPHandlerData* pHTTPData = pReq->m_httpData;
	WORD ret = SOCMNHTTP_CODEI_NOT_HANDLED;
	DWORD bodyLength = 0;
	BYTE* pBodyData = NULL;
	SOCmnStringEx resHeader;
	SOCmnDateTime now;
	SOCmnStringEx szDefResAttrs;
	SOCmnString bodyType;
	SOCmnString filePath;
	SOCmnString httpAttributes;
	now.now();

	// check URL and load file
	// the requested file must be located in or below the static or dynamic
	// directory of the web server
	if ((ret = getFile(pReq, pHTTPData, NULL, &bodyLength, &pBodyData, &bodyType, &filePath, &httpAttributes)) == SOCMNHTTP_CODE_OK)
	{
		if (_tcsncmp((LPCTSTR)bodyType, _T("text"), 4) == 0)
		{
			// text file
			SOCmnPointer<SOSrvWebExpandData> exData;
			SOCmnStringEx resBody;
			SOCmnString cacheControl;
			DWORD templCnt;
			exData = createExpandData();
			exData->init(pReq, pHTTPData, &httpAttributes);
#ifndef UNICODE
			resBody.attachTo((LPTSTR)pBodyData, bodyLength);
#else
			resBody.setCHAR((LPCSTR)pBodyData, bodyLength);
#endif
			templCnt = expandTemplates(resBody, exData);
			DWORD sizeB = 0;
			BYTE* pByteBody = resBody.cloneToUTF8(&sizeB);

			if (pByteBody != NULL)
			{
				free(pByteBody);
			}

			_TRACE(SOCMNTRACE_L_INF, SOCMNTRACE_G_WEB_SRV, (_T("GET %s - %s ; length: %lu"), (LPCTSTR)pReq->getURL(), (LPCTSTR)bodyType, resBody.getLength()));
			getResponseAttributes(pReq->m_flags, szDefResAttrs);

			if (templCnt > 0)
			{
				cacheControl = _T("Cache-Control: must-revalidate\r\n");
			}

			resHeader.format(_T("HTTP/1.1 200 OK\r\nServer: %s\r\nDate: %s\r\n%s%s%sContent-Type: %s; charset=utf-8\r\nContent-Length: %lu\r\n\r\n"), (LPCTSTR)m_httpServerDescription, (LPCTSTR)now.getHTTPDate(), (LPCTSTR)szDefResAttrs, (LPCTSTR)httpAttributes, (LPCTSTR)cacheControl, (LPCTSTR)bodyType, sizeB);
			pReq->m_httpHandlerForResponse->queueSndData(pHTTPData, pReq->m_flags, resHeader, resBody);
#ifdef UNICODE
			free(pBodyData);
#endif
		}
		else
		{
			// image file
			_TRACE(SOCMNTRACE_L_INF, SOCMNTRACE_G_WEB_SRV, (_T("GET %s - %s ; length: %lu"), (LPCTSTR)pReq->getURL(), (LPCTSTR)bodyType, bodyLength));
			getResponseAttributes(pReq->m_flags, szDefResAttrs);
			resHeader.format(_T("HTTP/1.1 200 OK\r\nServer: %s\r\nDate: %s\r\n%s%sContent-Type: %s; charset=utf-8\r\nContent-Length: %lu\r\n\r\n"), (LPCTSTR)m_httpServerDescription, (LPCTSTR)now.getHTTPDate(), (LPCTSTR)szDefResAttrs, (LPCTSTR)httpAttributes, (LPCTSTR)bodyType, bodyLength);
			pReq->m_httpHandlerForResponse->queueSndData(pHTTPData, pReq->m_flags, resHeader, bodyLength, pBodyData);
			free(pBodyData);
		}
	}
	else
	{
		if (ret != 0)
		{
			pReq->m_httpHandlerForResponse->sndHTTPErrorResponse(pHTTPData, pReq->m_flags, ret, httpAttributes);
		}
	}

	return ret;
}


WORD SOSrvWeb::getFile(
	IN SOSrvHTTPRequest* pReq,
	IN SOSrvHTTPHandlerData* pHTTPData,
	IN LPCTSTR absFilePath,
	OUT DWORD* length,
	OUT BYTE** ppData,
	OUT SOCmnString* type,
	OUT SOCmnString* filePath,
	OUT SOCmnString* httpAttributes)
{
	SOCmnString url = pReq->getURL();
	int queryStartIdx;

	if ((queryStartIdx = url.find(_T('?'))) != -1)
	{
		url = url.left(queryStartIdx);
	}

#ifdef SOOS_LINUX
	struct stat fileStat;

	if (absFilePath == NULL)
	{
		if (url == _T("/"))
		{
			url = m_rootFile;
		}

		// check if file exist in root directory 1
		*filePath->format(_T("%s%s"), (LPCTSTR)m_rootDirectory1, (LPCTSTR)url);

		if (0 != stat(*filePath, &fileStat))
		{
			// check if file exist in root directory 2
			*filePath->format(_T("%s%s"), (LPCTSTR)m_rootDirectory2, (LPCTSTR)url);

			if (0 != stat(*filePath, &fileStat))
			{
				filePath->empty();
				_TRACE(SOCMNTRACE_L_INF, SOCMNTRACE_G_WEB_SRV, (_T("GET %s - invalid URL"), (LPCTSTR)url));
				return SOCMNHTTP_CODE_NOT_FOUND;
			}
		}
	}
	else
	{
		*filePath = absFilePath;

		if (0 != stat(*filePath, &fileStat))
		{
			filePath->empty();
			_TRACE(SOCMNTRACE_L_INF, SOCMNTRACE_G_WEB_SRV, (_T("GET %s - invalid file path"), (LPCTSTR)absFilePath));
			return SOCMNHTTP_CODE_NOT_FOUND;
		}
	}

	// check access rigths
	WORD accRightsRet;

	if ((accRightsRet = checkAccess(pReq, pHTTPData, *filePath, httpAttributes)) != SOCMNHTTP_CODE_OK)
	{
		return accRightsRet;
	}

	// get type depending on the extention of the file
	LPTSTR fileWithoutExt, fileExt;
	SOCmnElement::reducePath(url, fileWithoutExt, fileExt, FALSE, _T("."));

	if (fileWithoutExt == NULL) // no file extension
	{
		*type = _T("CGI");
		return SOCMNHTTP_CODE_OK; // don't have to read the CGI file to execute
	}
	else if ((_tcsicmp(fileExt, _T("html")) == 0) || (_tcsicmp(fileExt, _T("htm")) == 0))
	{
		*type = _T("text/html");
	}
	else if ((_tcsicmp(fileExt, _T("jpg")) == 0) || (_tcsicmp(fileExt, _T("jpeg")) == 0))
	{
		*type = _T("image/jpeg");
	}
	else if (_tcsicmp(fileExt, _T("gif")) == 0)
	{
		*type = _T("image/gif");
	}
	else if (_tcsicmp(fileExt, _T("css")) == 0)
	{
		*type = _T("text/css");
	}
	else if (_tcsicmp(fileExt, _T("txt")) == 0)
	{
		*type = _T("text/plain");
	}
	else if (_tcsicmp(fileExt, _T("xml")) == 0)
	{
		*type = _T("text/xml");
	}
	else
	{
		*type = _T("application/softing");
	}

	if ((ppData == NULL) || (length == NULL))
	{
		return SOCMNHTTP_CODE_OK;
	}

	// open file
	FILE* pFile;
	pFile = fopen(*filePath, "r");

	if (!pFile)
	{
		_TRACE(SOCMNTRACE_L_ERR, SOCMNTRACE_G_WEB_SRV, (_T("GET %s - can't open file %s"), (LPCTSTR)url, (LPCTSTR)filePath));
		return SOCMNHTTP_CODE_NOT_FOUND;
	}

	// read contents of file to buffer
	*ppData = (BYTE*)malloc(fileStat.st_size + 1);  // one byte more for later attach to SOCmnStringEx object

	if (*ppData == NULL)
	{
		_TRACE(SOCMNTRACE_L_ERR, SOCMNTRACE_G_WEB_SRV, (_T("GET %s - can't allocate memory"), (LPCTSTR)url));
		filePath->empty();
		fclose(pFile);
		return SOCMNHTTP_CODE_INTERNAL_SERVER_ERROR;
	}

	(*ppData)[fileStat.st_size] = _T('\0');
	*length = fread((void*)*ppData, 1, fileStat.st_size, pFile);
	fclose(pFile);

	if (*length != (DWORD)fileStat.st_size)
	{
		free(*ppData);
		*ppData = NULL;
		*length = 0;
		filePath->empty();
		_TRACE(SOCMNTRACE_L_ERR, SOCMNTRACE_G_WEB_SRV, (_T("GET %s - can't read complete file - size read %lu ; needed %lu"), (LPCTSTR)url, *length, (DWORD)fileStat.st_size));
		return SOCMNHTTP_CODE_INTERNAL_SERVER_ERROR;
	}

	return SOCMNHTTP_CODE_OK;
#endif
#ifdef SOOS_WINDOWS
	DWORD fileAttributes;

	if (absFilePath == NULL)
	{
		if (url == _T("/"))
		{
			url = m_rootFile;
		}

		// check if file exist in root directory 1
		*filePath->format(_T("%s%s"), (LPCTSTR)m_rootDirectory1, (LPCTSTR)url);
		fileAttributes = GetFileAttributes(*filePath);

		if ((fileAttributes == ((DWORD) - 1)) || ((fileAttributes & (FILE_ATTRIBUTE_DIRECTORY)) != 0))
		{
			// check if file exist in root directory 2
			*filePath->format(_T("%s%s"), (LPCTSTR)m_rootDirectory2, (LPCTSTR)url);
			fileAttributes = GetFileAttributes(*filePath);

			if ((fileAttributes == ((DWORD) - 1)) || ((fileAttributes & (FILE_ATTRIBUTE_DIRECTORY)) != 0))
			{
				filePath->empty();
				_TRACE(SOCMNTRACE_L_INF, SOCMNTRACE_G_WEB_SRV, (_T("GET %s - invalid URL"), (LPCTSTR)url));
				return SOCMNHTTP_CODE_NOT_FOUND;
			}
		}
	}
	else
	{
		*filePath = absFilePath;
		fileAttributes = GetFileAttributes(*filePath);

		if ((fileAttributes == ((DWORD) - 1)) || ((fileAttributes & (FILE_ATTRIBUTE_DIRECTORY)) != 0))
		{
			filePath->empty();
			_TRACE(SOCMNTRACE_L_INF, SOCMNTRACE_G_WEB_SRV, (_T("GET %s - invalid file path"), (LPCTSTR)absFilePath));
			return SOCMNHTTP_CODE_NOT_FOUND;
		}
	}

	// check access rigths
	WORD accRightsRet;

	if ((accRightsRet = checkAccess(pReq, pHTTPData, *filePath, httpAttributes)) != SOCMNHTTP_CODE_OK)
	{
		return accRightsRet;
	}

	// get type depending on the extention of the file
	LPTSTR fileWithoutExt, fileExt;
	SOCmnElement::reducePath(url, fileWithoutExt, fileExt, FALSE, _T("."));

	if (fileWithoutExt == NULL) // no file extension
	{
		*type = _T("CGI");
		return SOCMNHTTP_CODE_OK; // don't have to read the CGI file to execute
	}
	else if ((_tcsicmp(fileExt, _T("html")) == 0) || (_tcsicmp(fileExt, _T("htm")) == 0))
	{
		*type = _T("text/html");
	}
	else if ((_tcsicmp(fileExt, _T("jpg")) == 0) || (_tcsicmp(fileExt, _T("jpeg")) == 0))
	{
		*type = _T("image/jpeg");
	}
	else if (_tcsicmp(fileExt, _T("gif")) == 0)
	{
		*type = _T("image/gif");
	}
	else if (_tcsicmp(fileExt, _T("css")) == 0)
	{
		*type = _T("text/css");
	}
	else if (_tcsicmp(fileExt, _T("txt")) == 0)
	{
		*type = _T("text/plain");
	}
	else if (_tcsicmp(fileExt, _T("xml")) == 0)
	{
		*type = _T("text/xml");
	}
	else
	{
		*type = _T("application/softing");
	}

	if ((ppData == NULL) || (length == NULL))
	{
		return SOCMNHTTP_CODE_OK;
	}

	// open file
	HANDLE hFile;
	hFile = CreateFile(*filePath, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	if (hFile == INVALID_HANDLE_VALUE)
	{
		_TRACE(SOCMNTRACE_L_ERR, SOCMNTRACE_G_WEB_SRV, (_T("GET %s - can't open file %s"), (LPCTSTR)url, (LPCTSTR)filePath));
		return SOCMNHTTP_CODE_NOT_FOUND;
	}

	DWORD fileSize;
	fileSize = GetFileSize(hFile, NULL);
	// read contents of file to buffer
	*ppData = (BYTE*)malloc(fileSize + 1);  // one byte more for later attach to SOCmnStringEx object

	if (*ppData == NULL)
	{
		_TRACE(SOCMNTRACE_L_ERR, SOCMNTRACE_G_WEB_SRV, (_T("GET %s - can't allocate memory"), (LPCTSTR)url));
		filePath->empty();
		CloseHandle(hFile);
		return SOCMNHTTP_CODE_INTERNAL_SERVER_ERROR;
	}

	(*ppData)[fileSize] = _T('\0');
	ReadFile(hFile, (void*)*ppData, fileSize, length, NULL);
	CloseHandle(hFile);

	if (*length != (DWORD)fileSize)
	{
		free(*ppData);
		*ppData = NULL;
		*length = 0;
		filePath->empty();
		_TRACE(SOCMNTRACE_L_ERR, SOCMNTRACE_G_WEB_SRV, (_T("GET %s - can't read complete file - size read %lu ; needed %lu"), (LPCTSTR)url, *length, (DWORD)fileSize));
		return SOCMNHTTP_CODE_INTERNAL_SERVER_ERROR;
	}

	return SOCMNHTTP_CODE_OK;
#endif
}

SOSrvWebExpandData* SOSrvWeb::createExpandData(void)
{
	SOSrvWebExpandData* exData;
	SOCMN_ALLOCATE_OBJECT_C(exData, SOSrvWebExpandData(this), (SOSrvWebExpandData*));
	return exData;
}

DWORD SOSrvWeb::expandTemplates(SOCmnStringEx& data, SOSrvWebExpandData* exData)
{
	// Start of template token: <%#
	// End of template token:   #%>
	// Parameter token:         ##
	LONG startIdxT;
	LONG endIdxT;
	DWORD tCnt;
	BOOL needSecondPass;
	BOOL expanded;

	do
	{
		exData->m_pass++;
		startIdxT = 0;
		endIdxT = -1;
		tCnt = 0;
		needSecondPass = FALSE;

		do
		{
			startIdxT = data.find(_T("<%#"), startIdxT);

			if (startIdxT != -1)
			{
				endIdxT = data.find(_T("#%>"), startIdxT + 3);

				if (endIdxT != -1)
				{
					SOCmnStringEx templ = data.mid(startIdxT + 3, (endIdxT) - (startIdxT + 3));
					SOCmnStringEx result;
					expanded = expandTemplate(templ, exData, result);

					if (expanded)
					{
						data.replace(startIdxT, endIdxT + 3, result, result.getLength());
						startIdxT = startIdxT + result.getLength();
					}
					else
					{
						startIdxT = endIdxT + 3;
					}

					needSecondPass |= (!expanded);
					tCnt++;
				}
			}
		}
		while ((startIdxT != -1) && (endIdxT != -1));

		if ((exData->m_pass == 1) && (needSecondPass))
		{
			exData->execute();
		}
	}
	while ((exData->m_pass < 2) && (needSecondPass));

	return tCnt;
}

BOOL SOSrvWeb::expandTemplate(SOCmnStringEx& templ, SOSrvWebExpandData* pExData, SOCmnStringEx& result)
{
	SOCmnStringList argsList;
	BOOL expanded = TRUE;
	result.empty();
	argsList.create();
	// store template arguements in args list
	int argStartIdx;
	int argEndIdx;
	SOCmnStringEx arg;
	argStartIdx = templ.find(_T("##"));

	while (argStartIdx != -1)
	{
		argStartIdx += 2;
		argEndIdx = templ.find(_T("##"), argStartIdx);

		if (argEndIdx == -1)
		{
			arg = templ.right(templ.getLength() - argStartIdx);
			argStartIdx = -1;
		}
		else
		{
			arg = templ.mid(argStartIdx, argEndIdx - argStartIdx);
			argStartIdx = argEndIdx;
		}

		argsList.add(arg);
	}

	// handle template
	if (_tcsncmp((LPCTSTR)templ, _T("ITEMVALUE##"), 11) == 0)
	{
		SOCmnString itemID = argsList.pop();

		if (pExData->m_pass == 1)
		{
			// add read request to the expand data request list
			pExData->addReadRequest(itemID);
			expanded = FALSE;
		}
		else
		{
			// set the item value
			pExData->getReadValue(itemID, &argsList, result);
		}
	}
	else if (_tcsncmp((LPCTSTR)templ, _T("ITEMQUALITY##"), 13) == 0)
	{
		SOCmnString itemID = argsList.pop();

		if (pExData->m_pass == 1)
		{
			// add read request to the expand data request list
			pExData->addReadRequest(itemID);
			expanded = FALSE;
		}
		else
		{
			// set the item value
			pExData->getReadQuality(itemID, &argsList, result);
		}
	}
	else if (_tcsncmp((LPCTSTR)templ, _T("MESSAGE##"), 9) == 0)
	{
		result = pExData->m_message;
	}
	else if (_tcsncmp((LPCTSTR)templ, _T("TRACE##"), 7) == 0)
	{
		SOCmnString cmd = argsList.pop();
		handleTRACETemplate(cmd, &argsList, pExData, result);
	}
	else if (_tcsncmp((LPCTSTR)templ, _T("ITEMLISTVALUES##"), 16) == 0)
	{
		handleITEMLISTVALUESTemplate(&argsList, pExData, result);
	}
	else if (_tcsncmp((LPCTSTR)templ, _T("POSTARG##"), 9) == 0)
	{
		SOCmnListPosition pos;
		SOCmnString par;
		SOCmnString def;
		SOCmnString key;
		SOCmnString keyPart;
		SOCmnString val;
		pos = argsList.getStartPosition();

		while (pos)
		{
			par = argsList.getNext(pos);

			if (par != _T("DEFAULT"))
			{
				if (!key.isEmpty())
				{
					keyPart.format(_T("##%s"), (LPCTSTR)par);
				}
				else
				{
					keyPart.format(_T("%s"), (LPCTSTR)par);
				}

				key += keyPart;
			}
			else
			{
				if (pos)
				{
					def = argsList.getNext(pos);
				}

				break;
			}
		}

		if (pExData->m_postArgsList.findKey(key, &val))
		{
			SOCmnString::doXMLReplacements(val);
			result = val;
		}
		else
		{
			result = def;
		}
	}
	else
	{
		expanded = handleTemplate((LPCTSTR)templ, &argsList, pExData, result);
	}

	return expanded;
}
//

void SOSrvWeb::handleITEMLISTVALUESTemplate(SOCmnStringList* pArgsList, SOSrvWebExpandData* pExData, SOCmnStringEx& result)
{
	SOCmnString type = pArgsList->pop();
	BOOL read = FALSE;
	BOOL write = FALSE;
	BOOL checkBox = FALSE;

	if (_tcsncmp((LPCTSTR)type, _T("ITEMSREAD"), 9) == 0)
	{
		read = TRUE;
	}

	if (_tcsncmp((LPCTSTR)type, _T("ITEMSREADCHANGE"), 15) == 0)
	{
		read = TRUE;
		checkBox = TRUE;
	}

	if (_tcsncmp((LPCTSTR)type, _T("ITEMSWRITE"), 9) == 0)
	{
		write = TRUE;
		checkBox = TRUE;
	}

	if ((read) || (write))
	{
		SOCmnPointer<SODaSGroup> grp;
		grp = getGroup();

		if (grp.isNotNull())
		{
			SOCmnList<SODaSItem> itemList((SOCmnList<SODaSItem> *)grp->getLeafList());

			if (!itemList.isEmpty())
			{
				SOCmnPointer<SODaSEntry> engine = ::getSODaSEntry();
				SOCmnPointer<SODaSCreator> creator = engine->getCreator();
				SOCmnPointer<SODaSTransaction> readTransaction;
				SOCmnListPosition posI;
				SODaSItem* pItem;
				SOCmnList<SODaSRequest> reqList;
				SOCmnPointer<SODaSRequest> req;
				DWORD i;
				SOCmnVariant** ppValues;
				WORD* pQualities;
				SOCmnDateTime** ppTimeStamps;
				HRESULT* pErrors;
				DWORD readCnt = itemList.getCount();
				SOCmnEvents events;
				DWORD reqEventId = events.create();
				ppValues = (SOCmnVariant**)malloc(readCnt * sizeof(SOCmnVariant*));
				pQualities = (WORD*)malloc(readCnt * sizeof(WORD));
				ppTimeStamps = (SOCmnDateTime**)malloc(readCnt * sizeof(SOCmnDateTime*));
				pErrors = (HRESULT*)malloc(readCnt * sizeof(HRESULT));;

				for (i = 0; i < readCnt; i++)
				{
					SOCMN_ALLOCATE_OBJECT(ppValues[i], SOCmnVariant());
					SOCMN_ALLOCATE_OBJECT(ppTimeStamps[i], SOCmnDateTime());
				}

				// create read transaction
				readTransaction = creator->createTransaction(NULL, SODaSTransaction::serverRead, SODaSTransaction::synchronous, 0);
				reqList = readTransaction->getRequestList();
				posI = itemList.getStartPosition();
				i = 0;

				while (posI)
				{
					pItem = itemList.getNext(posI);
					req = creator->createRequest(m_server, pItem, NULL, SODaSRequest::read, SODaSRequest::syncMaxAge);

					if (req.isNotNull())
					{
						req->setFlag(SODASREQUEST_FLAG_SOAP);
						req->setCompletedEvent(SOCmnEventPointer(&events, reqEventId));
						req->setItem(pItem);
						req->setMaxAge(0);
						req->setResultAddress((pErrors) + i);
						req->setResult(S_OK);
						req->setReadValueAddress(ppValues[i]);
						req->setReadQualityAddress((pQualities) + i);
						req->setReadTimeStampAddress(ppTimeStamps[i]);
						reqList.add(req);
						i++;
					}
				}

				// execute transaction
				readTransaction->execute(m_server, &events, reqEventId, m_server->getDuetimeRead());
				readTransaction->setCompletedEvent(NULL);
				readTransaction->emptyRequestsCompletedEvent();
				// fill result string
				SOCmnStringEx line;
				SOCmnStringEx szQuality, valField;
				SOCmnString szVal, szTimestamp, szCheckBox;
				posI = itemList.getStartPosition();
				i = 0;

				while (posI)
				{
					pItem = itemList.getNext(posI);

					SOCmnString itemIdStr = pItem->getItemID();
					SOCmnString::doXMLReplacements(itemIdStr);

					if (checkBox)
					{
						if (read)
						{
							szCheckBox.format(_T("<input type=\"checkbox\" name=\"CHECK##0x%8.8lX\" ID=\"CHECK##0x%8.8lX\" value=\"1\">"), (DWORD)pItem->getOpcHandle(), (DWORD)pItem->getOpcHandle());
						}
						else
						{
							szCheckBox.format(_T("<input type=\"checkbox\" name=\"CHECK##%s\" value=\"1\">"), (LPCTSTR)itemIdStr);
						}
					}

					if (SUCCEEDED(pErrors[i]))
					{
						variant2string(&szVal, ppValues[i], getLCID());

						// Apply encoding of special chars to avoid XSS attacks
						SOCmnString::doXMLReplacements(szVal);

						if (read)
						{
							valField = szVal;
						}
						else
						{
							valField.format(_T("<input class=\"editField\" type=\"text\" name=\"ITEMVALUE##%s\" size=\"20\" value=\"%s\">"), (LPCTSTR)itemIdStr, (LPCTSTR)szVal);
						}

						pExData->getQualityString(pQualities[i], szQuality);
						

						line.format(_T("<tr><td height=\"24\">%s</td><td>%s</td><td>%s</td><td>%s</td><td>%s</td></tr>"), (LPCTSTR)szCheckBox, (LPCTSTR)itemIdStr, (LPCTSTR)valField, (LPCTSTR)szQuality, (LPCTSTR)ppTimeStamps[i]->toString(szTimestamp));
					}
					else
					{
						if (read)
						{
							valField.empty();
						}
						else
						{
							valField.format(_T("<input class=\"editField\" type=\"text\" name=\"ITEMLISTVALUE##0x%8.8lX\" size=\"20\" value=\"\">"), (DWORD)pItem->getOpcHandle());
						}

						line.format(_T("<tr><td height=\"24\">%s</td><td>%s</td><td>%s</td><td></td><td></td></tr>"), (LPCTSTR)szCheckBox, (LPCTSTR)itemIdStr, (LPCTSTR)valField);
					}

					result += line;
					i++;
				}

				if (ppValues)
				{
					for (i = 0; i < readCnt; i++)
					{
						if (ppValues[i])
						{
							delete ppValues[i];
						}
					}

					free(ppValues);
				}

				if (ppTimeStamps)
				{
					for (i = 0; i < readCnt; i++)
					{
						if (ppTimeStamps[i])
						{
							delete ppTimeStamps[i];
						}
					}

					free(ppTimeStamps);
				}

				if (pQualities)
				{
					free(pQualities);
				}

				if (pErrors)
				{
					free(pErrors);
				}
			}
		}
	}
}

BOOL SOSrvWeb::handleTemplate(
	LPCTSTR /* templ */,
	SOCmnStringList* /* pArgsList */,
	SOSrvWebExpandData* /* pExData */,
	OUT SOCmnStringEx& result)
{
	result = _T("");
	return TRUE;
}

LPCTSTR SOSrvWeb::getTraceLevelDescription(IN DWORD traceLevel, IN LPCTSTR format, OUT SOCmnStringEx& descr)
{
	return getTraceLevelDescriptionS(traceLevel, format, descr);
}

LPCTSTR SOSrvWeb::getTraceLevelDescriptionS(IN DWORD traceLevel, IN LPCTSTR format, OUT SOCmnStringEx& descr)
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

	case SOCMNTRACE_G_OPC:
		descr = _T("OPC");
		break;

	default:
		descr.format(_T("0x%8.8lX"), traceLevel);
		break;
	}

	return descr;
}

void SOSrvWeb::handleTRACETemplate(
	LPCTSTR cmd,
	SOCmnStringList* pArgsList,
	SOSrvWebExpandData* /* pExData */,
	SOCmnStringEx& result)
{
	if (_tcscmp(cmd, _T("VIEW")) == 0)
	{
		SOCmnPointer<SOSrvWebTrace> webTrace = getWebTrace();
		webTrace->getTrace(result);
	}
	else if (_tcscmp(cmd, _T("INFO")) == 0)
	{
		SOCmnTrace* pTrace = getSOCmnTrace();
		SOCmnString type = pArgsList->pop();
		SOCmnString format;

		if (!pArgsList->isEmpty())
		{
			format = pArgsList->pop();
		}

		if (_tcscmp(_T("TLERR"), type) == 0)
		{
			getTraceLevelDescription(pTrace->m_traceLevel[TL_ERR], (LPCTSTR)format, result);
		}
		else if (_tcscmp(_T("TLWRN"), type) == 0)
		{
			getTraceLevelDescription(pTrace->m_traceLevel[TL_WRN], (LPCTSTR)format, result);
		}
		else if (_tcscmp(_T("TLINF"), type) == 0)
		{
			getTraceLevelDescription(pTrace->m_traceLevel[TL_INF], (LPCTSTR)format, result);
		}
		else if (_tcscmp(_T("TLDEB"), type) == 0)
		{
			getTraceLevelDescription(pTrace->m_traceLevel[TL_DEB], (LPCTSTR)format, result);
		}
		else if (_tcscmp(_T("TFNAME1"), type) == 0)
		{
			result.format(_T("%s"), (LPCTSTR)pTrace->getFileName());
		}
		else if (_tcscmp(_T("TFNAME2"), type) == 0)
		{
			result.format(_T("%s"), (LPCTSTR)pTrace->getSecondFileName());
		}
		else if (_tcscmp(_T("TFNAMEACTIVE"), type) == 0)
		{
			result.format(_T("%s"), (LPCTSTR)pTrace->getActiveTraceFileName());
		}
		else if (_tcscmp(_T("TFNAMEBASE"), type) == 0)
		{
			result.format(_T("%s"), (LPCTSTR)pTrace->getFileNameBase());
		}
		else if (_tcscmp(_T("TFMAXSIZE"), type) == 0)
		{
			result.format(_T("%lu"), pTrace->getFileSize());
		}
		else if (_tcscmp(_T("TFMAXBACKUPS"), type) == 0)
		{
			result.format(_T("%lu"), pTrace->getMaxBackups());
		}
		else if (_tcscmp(_T("TFENABLE"), type) == 0)
		{
			BOOL ttf = pTrace->getTraceToFile();

			if (_tcscmp(format, _T("01")) == 0)
			{
				result = (!ttf) ? _T("0") : _T("1") ;
			}
			else if (_tcscmp(format, _T("NY")) == 0)
			{
				result = (!ttf) ? _T("no") : _T("yes");
			}
			else if (_tcscmp(format, _T("NJ")) == 0)
			{
				result = (!ttf) ? _T("Nein") : _T("Ja");
			}
			else if (_tcscmp(format, _T("CHECKED")) == 0)
			{
				result = (!ttf) ? _T("") : _T("checked");
			}
			else
			{
				result = (!ttf) ? _T("false") : _T("true");
			}
		}
	}
}

WORD SOSrvWeb::post(IN SOSrvHTTPRequest* pReq)
{
	SOSrvHTTPHandlerData* pHTTPData = pReq->m_httpData;
	DWORD lenBody = pReq->m_lenBody;
	BYTE* pBody = pReq->m_pBody;
	WORD ret = SOCMNHTTP_CODEI_NOT_HANDLED;
	DWORD bodyLength = 0;
	BYTE* pBodyData = NULL;
	SOCmnStringEx resHeader;
	SOCmnDateTime now;
	SOCmnStringEx szDefResAttrs;
	SOCmnString bodyType;
	SOCmnString filePath;
	SOCmnStringList attributeList(pReq->getAttributeList());
	SOCmnString httpAttributes;
	now.now();

	// check URL and load file
	// the requested file must be located in or below the static or dynamic
	// directory of the web server
	if ((ret = getFile(pReq, pHTTPData, NULL, &bodyLength, &pBodyData, &bodyType, &filePath, &httpAttributes)) == SOCMNHTTP_CODE_OK)
	{
		if (_tcsncmp((LPCTSTR)bodyType, _T("text"), 4) == 0)
		{
			// text file
			SOCmnPointer<SOSrvWebExpandData> exData;
			SOCmnStringEx body;
			SOCmnStringEx resBody;
			SOCmnString cacheControl;
			SOCmnString postCommand;
			DWORD templCnt;
			SOCmnString contentType;
			exData = createExpandData();
			exData->init(pReq, pHTTPData, &httpAttributes);

			if (attributeList.findKey(_T("content-type"), &contentType))
			{
				if (_tcscmp(contentType, _T("application/x-www-form-urlencoded")) == 0)
				{
					body.setCHAR((CHAR*)pBody, lenBody);

					if (!body.isEmpty())
					{
						// parse POST arguments format: id1=val1&id2=val2&...
						int argStartIdx;
						int argEndIdx;
						SOCmnStringEx arg;
						LPTSTR name, val;
						int perRepIdx;
						SOCmnString repChars;
						TCHAR repSign;
						body.replace(_T("+"), _T(" "));

						while ((perRepIdx = body.find(_T('%'))) != -1)
						{
							repChars = body.mid(perRepIdx, 3);
							repSign = (TCHAR)_tcstoul((LPCTSTR)repChars + 1, NULL, 16);
							body.replace(perRepIdx, perRepIdx + 3, (TCHAR*)&repSign, 1);
							/*
							                            body.replace(_T("%23"), _T("#"));
							                            body.replace(_T("%2F"), _T("/"));
							                            body.replace(_T("%20"), _T(" "));
							                            body.replace(_T("%3A"), _T(":"));
							                            body.replace(_T("%5C"), _T("\\"));
							                            body.replace(_T("%2B"), _T("+"));
							*/
						}

						argStartIdx = 0;

						while (argStartIdx != -1)
						{
							argEndIdx = body.find(_T("&"), argStartIdx);

							if (argEndIdx == -1)
							{
								arg = body.right(body.getLength() - argStartIdx);
								argStartIdx = -1;
							}
							else
							{
								arg = body.mid(argStartIdx, argEndIdx - argStartIdx);
								argStartIdx = argEndIdx + 1;
							}

							if (_tcsncmp(arg, _T("CMD##"), 5) != 0)
							{
								SOCmnElement::reducePath(arg, name, val, TRUE, _T("="));

								if (val == NULL)
								{
									val = _T("");
								}

								exData->m_postArgsList.add(val, name);
							}
							else
							{
								if (postCommand.isEmpty())
								{
									postCommand = arg.right(arg.getLength() - 5);
								}
							}
						}
					}
				}
			}

			_TRACE(SOCMNTRACE_L_DEB, SOCMNTRACE_G_WEB_SRV, (_T("POST %s - %s ; %s"), (LPCTSTR)pReq->getURL(), (LPCTSTR)bodyType, (LPCTSTR)body));

			if (!postCommand.isEmpty())
			{
				handlePOSTCommand(postCommand, pReq, pHTTPData, &exData->m_postArgsList, exData, resBody, &bodyType);
			}

			if (resBody.isEmpty())
			{
				// no result data from handlePOSTArguments
#ifndef UNICODE
				resBody.attachTo((LPTSTR)pBodyData, bodyLength);
#else
				resBody.setCHAR((LPCSTR)pBodyData, bodyLength);
#endif
			}
			else
			{
				// received results -> free read file data
				if (pBodyData)
				{
					free(pBodyData);
					pBodyData = NULL;
				}
			}

			templCnt = expandTemplates(resBody, exData);
			_TRACE(SOCMNTRACE_L_INF, SOCMNTRACE_G_WEB_SRV, (_T("POST %s - %s ; length: %lu"), (LPCTSTR)pReq->getURL(), (LPCTSTR)bodyType, resBody.getLength()));

			if (exData->m_httpResponse.isEmpty())
			{
				DWORD sizeB = 0;
				BYTE* pByteBody = resBody.cloneToUTF8(&sizeB);

				if (pByteBody != NULL)
				{
					free(pByteBody);
				}

				getResponseAttributes(pReq->m_flags, szDefResAttrs);

				if (templCnt > 0)
				{
					cacheControl = _T("Cache-Control: must-revalidate\r\n");
				}

				resHeader.format(_T("HTTP/1.1 200 OK\r\nServer: %s\r\nDate: %s\r\n%s%s%sContent-Type: %s; charset=utf-8\r\nContent-Length: %lu\r\n\r\n"), (LPCTSTR)m_httpServerDescription, (LPCTSTR)now.getHTTPDate(), (LPCTSTR)szDefResAttrs, (LPCTSTR)httpAttributes, (LPCTSTR)cacheControl, (LPCTSTR)bodyType, sizeB);
				// send result file
				pReq->m_httpHandlerForResponse->queueSndData(pHTTPData, pReq->m_flags, resHeader, resBody);
			}
			else
			{
				pReq->m_httpHandlerForResponse->queueSndData(pHTTPData, pReq->m_flags, exData->m_httpResponse, 0, NULL);
			}

#ifdef UNICODE

			if (pBodyData)
			{
				free(pBodyData);
			}

#endif
		}
	}
	else
	{
		if (ret != 0)
		{
			pReq->m_httpHandlerForResponse->sndHTTPErrorResponse(pHTTPData, pReq->m_flags, ret, httpAttributes);
		}
	}

	return ret;
}

DWORD SOSrvWeb::getTraceLevelFromDescription(IN LPCTSTR descr)
{
	return getTraceLevelFromDescriptionS(descr);
}

DWORD SOSrvWeb::getTraceLevelFromDescriptionS(IN LPCTSTR descr)
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

		if (d.find(_T("OPC")) != -1)
		{
			tl |= SOCMNTRACE_G_OPC;
		}

		return tl;
	}

	return _tcstoul(descr, NULL, 0);
}

BOOL SOSrvWeb::handlePOSTCommand(
	LPCTSTR command,
	IN SOSrvHTTPRequest* pReq,
	IN SOSrvHTTPHandlerData* pHTTPData,
	SOCmnStringList* pArgsList,
	SOSrvWebExpandData* pExData,
	SOCmnStringEx& result,
	OUT SOCmnString* type)
{
	if (handleLOGIN(command, pReq, pHTTPData, pArgsList, pExData, result, type))
	{
		return TRUE;
	}

	if (_tcsncmp(command, _T("WRITEITEM"), 9) == 0)
	{
		pExData->addWriteRequests(command, pArgsList);
		pExData->execute();
	}
	else if (_tcsncmp(command, _T("TRACE"), 5) == 0)
	{
		SOCmnTrace* pTrace = getSOCmnTrace();
		SOCmnString hlp;
		DWORD tlErr, tlWrn, tlInf, tlDeb;
		tlErr = pTrace->m_traceLevel[TL_ERR];
		tlWrn = pTrace->m_traceLevel[TL_WRN];
		tlInf = pTrace->m_traceLevel[TL_INF];
		tlDeb = pTrace->m_traceLevel[TL_DEB];

		if (pArgsList->findKey(_T("TRACE##TLERR"), &hlp))
		{
			tlErr = getTraceLevelFromDescription(hlp);
		}

		if (pArgsList->findKey(_T("TRACE##TLWRN"), &hlp))
		{
			tlWrn = getTraceLevelFromDescription(hlp);
		}

		if (pArgsList->findKey(_T("TRACE##TLINF"), &hlp))
		{
			tlInf = getTraceLevelFromDescription(hlp);
		}

		if (pArgsList->findKey(_T("TRACE##TLDEB"), &hlp))
		{
			tlDeb = getTraceLevelFromDescription(hlp);
		}

		pTrace->setLevels(tlErr, tlWrn, tlInf, tlDeb);
		BOOL enable = FALSE;
		SOCmnString file1;
		SOCmnString file2;
		DWORD maxSize = 100000;
		DWORD maxBackups = 0;

		if (pArgsList->findKey(_T("TRACE##TFENABLE"), &hlp))
		{
			enable = TRUE;
		}

		if (pArgsList->findKey(_T("TRACE##TFMAXSIZE"), &hlp))
		{
			maxSize = _tcstoul(hlp, NULL, 0);
		}

		if (pArgsList->findKey(_T("TRACE##TFMAXBACKUPS"), &hlp))
		{
			maxBackups = _tcstoul(hlp, NULL, 0);
		}

		if (pArgsList->findKey(_T("TRACE##TFNAMEBASE"), &file1))
		{
			//new style web pages:
			pTrace->updateTraceFileSettings2(&enable, file1, &maxSize, &maxBackups);
		}
		else
		{
			//old style web pages:
			pArgsList->findKey(_T("TRACE##TFNAME1"), &file1);
			pArgsList->findKey(_T("TRACE##TFNAME2"), &file2);
			pTrace->updateTraceFileSettings(&enable, file1, file2, &maxSize, &maxBackups);
		}

		changedConfiguration(command);
	}
	else if (_tcsncmp(command, _T("ITEMLISTVALUES"), 14) == 0)
	{
		handleItemListValuesCommand(command, pArgsList, pExData, result, type);
	}
	else if (_tcsncmp(command, _T("SETPASSWORD"), 11) == 0)
	{
		SOCmnString user;
		SOCmnString pwd;
		SOCmnString pwdO;
		pArgsList->findKey(_T("USER"), &user);
		pArgsList->findKey(_T("PASSWORD"), &pwd);
		pArgsList->findKey(_T("OLDPASSWORD"), &pwdO);
		handleSetPassword(command, user, pwd, pwdO, pExData);
	}
	else
	{
		return handleCommand(command, pArgsList, pExData, result, type);
	}

	return TRUE;
}



BOOL SOSrvWeb::handleLOGIN(
	LPCTSTR command,
	IN SOSrvHTTPRequest* /* pReq */,
	IN SOSrvHTTPHandlerData* /* pHTTPData */,
	SOCmnStringList* pArgsList,
	SOSrvWebExpandData* pExData,
	SOCmnStringEx& result,
	OUT SOCmnString* type)
{
	SOCmnString user;
	SOCmnString pwd;
	pArgsList->findKey(_T("USER"), &user);
	pArgsList->findKey(_T("PASSWORD"), &pwd);

	if (!checkUser(user, pwd, command, pArgsList, pExData, result, type))
	{
		return TRUE;
	}

	return FALSE;
}

void SOSrvWeb::handleSetPassword(
	LPCTSTR /* command */,
	LPCTSTR /* user */,
	LPCTSTR /* pwd */,
	LPCTSTR /* pwdO */,
	SOSrvWebExpandData* pExData)
{
	pExData->setMessage(_T("Can't set password"));
}

SODaSGroup* SOSrvWeb::getGroup()
{
	m_group.addRef();
	return m_group;
}


void SOSrvWeb::handleItemListValuesCommand(
	LPCTSTR command,
	SOCmnStringList* pArgsList,
	SOSrvWebExpandData* pExData,
	SOCmnStringEx& /* result */,
	OUT SOCmnString* /* type */)
{
	SOCmnString errorMsg;
	SOCmnPointer<SODaSGroup> grp;
	grp = getGroup();

	if (grp.isNotNull())
	{
		if (_tcsncmp(command, _T("ITEMLISTVALUES##ADD"), 19) == 0)
		{
			// add item
			_TRACE(SOCMNTRACE_L_DEB, SOCMNTRACE_G_WEB_SRV, (_T("ITEMLISTVALUES##ADD")));
			SOCmnString szItemID;
			BOOL ok = TRUE;
			ok &= pArgsList->findKey(_T("ITEMLISTVALUES##ITEMID"), &szItemID);

			if (ok)
			{
				SOCmnPointer<SODaSItem> item;
				grp->addItem(szItemID, NULL, FALSE, 0, VT_EMPTY, &item, NULL, NULL);
				grp->onAddItems();
			}
		}
		else if (_tcsncmp(command, _T("ITEMLISTVALUES##DELETE"), 22) == 0)
		{
			// delete item
			SOCmnString argV, argN;
			SOCmnList<SODaSItem> itemList((SOCmnList<SODaSItem> *)grp->getLeafList());
			SOCmnListPosition pos;
			DWORD itemAdr;
			pos = pArgsList->getStartPosition();

			while (pos)
			{
				argV = pArgsList->getNext(pos, &argN);

				if (_tcsncmp(argN, _T("CHECK##"), 7) == 0)
				{
					itemAdr = _tcstoul(((LPCTSTR)argN) + 7, NULL, 0);
					grp->removeItem(itemAdr);
				}
			}

			grp->onRemoveItems();
		}
	}

	pExData->setMessage(errorMsg);
}


void SOSrvWeb::changedConfiguration(LPCTSTR /* command */)
{
}


BOOL SOSrvWeb::handleCommand(
	LPCTSTR /* command */,
	SOCmnStringList* /* pArgsList */,
	SOSrvWebExpandData* /* pExData */,
	SOCmnStringEx& /* result */,
	OUT SOCmnString* /* type */)
{
	return FALSE;
}

BOOL SOSrvWeb::checkUser(
	LPCTSTR /* user */,
	LPCTSTR /* password */,
	LPCTSTR /* command */,
	SOCmnStringList* /* pArgsList */,
	SOSrvWebExpandData* /* pExData */,
	SOCmnStringEx& /* result */,
	OUT SOCmnString* /* type */)
{
	return TRUE;
}

WORD SOSrvWeb::checkAccess(
	IN SOSrvHTTPRequest* /* pReq */,
	IN SOSrvHTTPHandlerData* /* pHTTPData */,
	IN LPCTSTR /* filePath */,
	OUT SOCmnString* /* httpAttributes */)
{
	return SOCMNHTTP_CODE_OK;
}


void SOSrvWeb::watchProcess(DWORD /* pid */)
{
}

BOOL SOSrvWeb::isProcessRunning(DWORD /* pid */)
{
	_ASSERTION(FALSE, "SOSrvWeb::isProcessRunning must be implemented if CGI is used");
	return FALSE;
}

SOSrvWebTrace* SOSrvWeb::getWebTrace(void)
{
	_ASSERTION(FALSE, "SOSrvWeb::getWebTrace must be implemented if WEB TRACE is used");
	return NULL;
}


SOSrvWebExpandData::SOSrvWebExpandData(SOSrvWeb* pServer)
	: SOCmnObject(SOCMNOBJECT_TYPE_OTHER)
{
	m_webServer = pServer;
	m_webServer.addRef();
	m_server = pServer->getServer();
	m_pass = 0;
	m_ppReadValues = NULL;
	m_pReadQualities = NULL;
	m_ppReadTimeStamps = NULL;
	m_pReadErrors = NULL;
	m_readCnt = 0;
	m_executeRead = FALSE;
	m_ppWriteValues = NULL;
	m_pWriteErrors = NULL;
	m_writeCnt = 0;
	m_executeWrite = FALSE;
	SOCmnListConfig cfg;
	cfg.m_type = (SOCMNLIST_TYPE_ORG_MAP | SOCMNLIST_TYPE_KEY_STRING | SOCMNLIST_TYPE_OBJ_STRING);
	cfg.m_hashTableSize = 64;
	m_readItemList.create(&cfg);
	m_writeItemList.create(&cfg);
	m_postArgsList.create(&cfg);
}

SOSrvWebExpandData::~SOSrvWebExpandData(void)
{
	DWORD i;

	if (m_ppReadValues)
	{
		for (i = 0; i < m_readCnt; i++)
		{
			if (m_ppReadValues[i])
			{
				delete m_ppReadValues[i];
			}
		}

		free(m_ppReadValues);
	}

	if (m_ppReadTimeStamps)
	{
		for (i = 0; i < m_readCnt; i++)
		{
			if (m_ppReadTimeStamps[i])
			{
				delete m_ppReadTimeStamps[i];
			}
		}

		free(m_ppReadTimeStamps);
	}

	if (m_pReadQualities)
	{
		free(m_pReadQualities);
	}

	if (m_pReadErrors)
	{
		free(m_pReadErrors);
	}

	m_readItemList.destroy();
	m_writeItemList.destroy();
	m_postArgsList.destroy();
}


void SOSrvWebExpandData::init(
	IN SOSrvHTTPRequest* /* pReq */,
	IN SOSrvHTTPHandlerData* /* pHTTPData */,
	IN SOCmnString* /* httpAttributes */)
{
}

void SOSrvWebExpandData::addReadRequest(IN LPCTSTR itemID)
{
	SOCmnPointer<SODaSEntry> engine = ::getSODaSEntry();
	SOCmnPointer<SODaSCreator> creator = engine->getCreator();
	SOCmnPointer<SODaSItemTag> itemTag;

	if (!m_readTransaction)
	{
		// create synchronous read transaction
		m_readTransaction = creator->createTransaction(NULL, SODaSTransaction::serverRead, SODaSTransaction::synchronous, 0);
		m_executeRead = TRUE;
		SOCmnList<SODaSRequest>* pReqList = m_readTransaction->getRequestList();
		pReqList->destroy();
		SOCmnListConfig cfg;
		cfg.m_type = (SOCMNLIST_TYPE_ORG_MAP | SOCMNLIST_TYPE_KEY_STRING | SOCMNLIST_TYPE_OBJ_OBJECT | SOCMNLIST_TYPE_SYNCED | SOCMNLIST_TYPE_BLOCKED);
		cfg.m_hashTableSize = 64;
		pReqList->create(&cfg);
	}

	// check if request for itemID already exists
	if (m_lastReadItemID == itemID)
	{
		return;
	}

	m_lastReadItemID = itemID;

	if (m_readItemList.findKey(itemID, NULL))
	{
		return;
	}

	m_readItemList.add(itemID, itemID);

	if (SUCCEEDED(m_server->getItemTag(itemID, itemTag)))
	{
		// create request
		SOCmnPointer<SODaSRequest> req;
		SOCmnList<SODaSRequest> reqList(m_readTransaction->getRequestList());
		req = creator->createRequest(m_server, NULL, itemTag, SODaSRequest::read, SODaSRequest::serverMaxAge);

		if (req.isNotNull())
		{
			req->setFlag(SODASREQUEST_FLAG_SOAP);
			req->setItemTag(itemTag);
			req->setServer(m_server);
			req->setMaxAge(0);
			req->setReqDatatype(VT_EMPTY);
			reqList.add(req, itemID);
		}
	}
}

void SOSrvWebExpandData::addWriteRequests(IN LPCTSTR command, SOCmnStringList* pArgsList)
{
	SOCmnPointer<SODaSEntry> engine = ::getSODaSEntry();
	SOCmnPointer<SODaSCreator> creator = engine->getCreator();
	SOCmnStringList itemList;
	SOCmnStringList checkList;
	SOCmnStringList pList;
	SOCmnString val;
	SOCmnString name;
	BOOL check;
	SOCmnString itemID;
	SOCmnString itemValue;
	SOCmnListPosition pos;
	DWORD i;
	SOCmnPointer<SODaSItemTag> itemTag;
	check = (_tcsncmp(command, _T("WRITEITEM##CHECK"), 16) == 0);

	if (check)
	{
		SOCmnListConfig cfg;
		cfg.m_type = (SOCMNLIST_TYPE_ORG_MAP | SOCMNLIST_TYPE_KEY_STRING | SOCMNLIST_TYPE_OBJ_STRING);
		cfg.m_hashTableSize = 64;
		itemList.create(&cfg);
		checkList = m_writeItemList;
		pList = checkList;
	}
	else
	{
		itemList = m_writeItemList;
		pList = itemList;
	}

	pos = pArgsList->getStartPosition();

	while (pos)
	{
		val = pArgsList->getNext(pos, &name);

		if (check)
			if (_tcsncmp(name, _T("CHECK##"), 7) == 0)
			{
				checkList.add(1, (LPCTSTR)name + 7);
			}

		if (_tcsncmp(name, _T("ITEMVALUE##"), 11) == 0)
		{
			itemList.add(val, (LPCTSTR)name + 11);
		}
	}

	if (check)
	{
		// get the values of the CHECKs
		pos = checkList.getStartPosition();

		while (pos)
		{
			checkList.getNext(pos, &itemID);

			if (itemList.findKey(itemID, &itemValue))
			{
				checkList.update(itemValue, itemID);
			}
			else
			{
				checkList.removeKey(itemID);
			}
		}
	}

	m_writeCnt = pList.getCount();

	if (m_writeCnt > 0)
	{
		m_ppWriteValues = (SOCmnVariant**)malloc(m_writeCnt * sizeof(SOCmnVariant*));
		m_pWriteErrors = (HRESULT*)malloc(m_writeCnt * sizeof(HRESULT));;

		for (i = 0; i < m_writeCnt; i++)
		{
			SOCMN_ALLOCATE_OBJECT(m_ppWriteValues[i], SOCmnVariant());
		}

		m_writeTransaction = creator->createTransaction(NULL, SODaSTransaction::serverWrite, SODaSTransaction::synchronous, 0);
		pos = pList.getStartPosition();
		i = 0;

		while (pos)
		{
			itemValue = pList.getNext(pos, &itemID);

			if (SUCCEEDED(m_server->getItemTag(itemID, itemTag)))
			{
				if ((itemTag->getAccessRights() & OPC_WRITEABLE) == OPC_WRITEABLE)
				{
					SOCmnPointer<SODaSRequest> req;
					req = creator->createRequest(m_server, NULL, itemTag, SODaSRequest::write, SODaSRequest::serverDevice);

					if (req.isNotNull())
					{
						req->setItemTag(itemTag);
						req->setServer(m_server);
						req->setResultAddress(m_pWriteErrors + i);
						req->setResult(S_OK);
						m_ppWriteValues[i]->setBSTR(itemValue);

						if (req->setWriteValue(m_ppWriteValues[i]))
						{
							m_writeTransaction->insertRequest(req);
						}
						else
						{
							m_pWriteErrors[i] = OPC_E_BADTYPE;
						}
					}
					else
					{
						m_pWriteErrors[i] = OPC_E_READONLY;
					}
				}
				else
				{
					m_pWriteErrors[i] = OPC_E_READONLY;
				}

				i++;
			}
			else
			{
				m_pWriteErrors[i] = OPC_E_UNKNOWNITEMID;
			}
		}

		m_writeCnt = i;
		m_executeWrite = TRUE;
	}
}

void SOSrvWebExpandData::execute(void)
{
	if ((m_executeRead) && (m_readTransaction.isNotNull()))
	{
		SOCmnList<SODaSRequest> reqList(m_readTransaction->getRequestList());
		SOCmnListPosition pos;
		SODaSRequest* req;
		SOCmnPointer<SODaSItemTag> itemTag;
		SOCmnEvents events;
		DWORD reqEventId = events.create();
		DWORD i;
		m_readCnt = reqList.getCount();

		if (m_readCnt > 0)
		{
			m_ppReadValues = (SOCmnVariant**)malloc(m_readCnt * sizeof(SOCmnVariant*));
			m_pReadQualities = (WORD*)malloc(m_readCnt * sizeof(WORD));
			m_ppReadTimeStamps = (SOCmnDateTime**)malloc(m_readCnt * sizeof(SOCmnDateTime*));
			m_pReadErrors = (HRESULT*)malloc(m_readCnt * sizeof(HRESULT));;

			for (i = 0; i < m_readCnt; i++)
			{
				SOCMN_ALLOCATE_OBJECT(m_ppReadValues[i], SOCmnVariant());
				SOCMN_ALLOCATE_OBJECT(m_ppReadTimeStamps[i], SOCmnDateTime());
			}

			pos = reqList.getStartPosition();
			i = 0;

			while (pos)
			{
				req = reqList.getNext(pos);
				itemTag = req->getItemTag();
				req->setResultAddress(m_pReadErrors + i);
				req->setReadValueAddress(m_ppReadValues[i]);
				req->setReadQualityAddress(m_pReadQualities + i);
				req->setReadTimeStampAddress(m_ppReadTimeStamps[i]);
				req->setCompletedEvent(SOCmnEventPointer(&events, reqEventId));
				req->setResult(S_OK);

				// check access rights
				if ((itemTag->getAccessRights() & OPC_READABLE) != OPC_READABLE)
				{
					req->setResult(OPC_E_WRITEONLY);
				}

				i++;
			}

			m_readTransaction->execute(m_server, &events, reqEventId, m_server->getDuetimeRead());
			m_executeRead = FALSE;
		}
	}

	if ((m_executeWrite) && (m_writeTransaction.isNotNull()))
	{
		SOCmnList<SODaSRequest> reqList(m_writeTransaction->getRequestList());
		SOCmnListPosition pos;
		SODaSRequest* req;
		SOCmnEvents events;
		DWORD reqEventId = events.create();

		if (!reqList.isEmpty())
		{
			pos = reqList.getStartPosition();

			while (pos)
			{
				req = reqList.getNext(pos);
				req->setCompletedEvent(SOCmnEventPointer(&events, reqEventId));
			}

			m_writeTransaction->execute(m_server, &events, reqEventId, m_server->getDuetimeWrite());
			m_executeWrite = FALSE;
		}
	}
}


void SOSrvWebExpandData::getReadValue(
	IN LPCTSTR itemID,
	IN SOCmnStringList* /* pArgs */,
	OUT SOCmnStringEx& result)
{
	if (m_readTransaction.isNotNull())
	{
		SOCmnList<SODaSRequest> reqList(m_readTransaction->getRequestList());
		SOCmnString szVal;
		SOCmnPointer<SODaSRequest> request;
		request = reqList.findKey(itemID);

		if (request.isNotNull())
		{
			if (SUCCEEDED(request->getResult()))
			{
				SOCmnVariant* pVal;
				request->getReadValueAddress(pVal);
				result = variant2string(&szVal, pVal, m_webServer->getLCID());
			}
		}
	}
}

void SOSrvWebExpandData::getReadQuality(
	IN LPCTSTR itemID,
	IN SOCmnStringList* pArgs,
	OUT SOCmnStringEx& result)
{
	SOCmnString formater;

	if (!pArgs->isEmpty())
	{
		formater = pArgs->pop();
		// set default values
		getQualityFormaterDefault(formater, result);
	}

	if (m_readTransaction.isNotNull())
	{
		SOCmnList<SODaSRequest> reqList(m_readTransaction->getRequestList());
		SOCmnPointer<SODaSRequest> request;
		request = reqList.findKey(itemID);

		if (request.isNotNull())
		{
			if (SUCCEEDED(request->getResult()))
			{
				if (formater.isEmpty())
				{
					getQualityString(*request->getReadQualityAddress(), result);
				}
				else
				{
					getQualityFormater(*request->getReadQualityAddress(), formater, result);
				}
			}
		}
	}
}


void SOSrvWebExpandData::getQualityString(WORD quality, SOCmnStringEx& result)
{
	SOCmnString szVal;
	result = quality2string(&szVal, quality, m_webServer->getLCID());
}



SOSrvHTTPRequest::SOSrvHTTPRequest(IN SOSrvHTTPHandler* pHandler, IN SOSrvHTTPHandlerData* pHTTPData, IN DWORD lenBody, IN BYTE* pBody)
	: SOCmnObject(SOCMNOBJECT_TYPE_OTHER)
{
	m_httpHandlerForResponse = pHandler;
	m_httpHandlerForResponse.addRef();
	m_httpData = pHTTPData;
	m_httpData.addRef();
	m_lenBody = lenBody;

	if (lenBody)
	{
		m_pBody = (BYTE*)malloc(lenBody);
		memcpy(m_pBody, pBody, lenBody);
	}
	else
	{
		m_pBody = NULL;
	}

	m_flags = pHTTPData->m_flags;
	m_httpURL = pHandler->getURL();
	m_httpMethod = pHandler->getHTTPMethod();
	m_httpVersion = pHandler->getHTTPVersion();
	m_attributeList.create(SOCMNLIST_TYPE_ORG_MAP | SOCMNLIST_TYPE_KEY_STRING | SOCMNLIST_TYPE_OBJ_STRING | SOCMNLIST_TYPE_BLOCKED);
	SOCmnListPosition pos;
	SOCmnString szVal, szKey;
	SOCmnStringList attrList(pHandler->getAttributeList());
	pos = attrList.getStartPosition();

	while (pos)
	{
		szVal = attrList.getNext(pos, &szKey);
		m_attributeList.add(szVal, szKey);
	}
}

SOSrvHTTPRequest::~SOSrvHTTPRequest(void)
{
	m_httpHandlerForResponse.release();
	m_httpData.release();

	if (m_pBody)
	{
		free(m_pBody);
	}
}

#endif // SOFEATURE_SOAP

#endif // SOSRV
