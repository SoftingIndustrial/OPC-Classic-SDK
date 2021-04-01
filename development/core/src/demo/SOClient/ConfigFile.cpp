#include "stdafx.h"
#include "ConfigFile.h"
#include "SOVersion.h"

SOCmnXmlTreeElement::SOCmnXmlTreeElement(LPCTSTR elementName, SOCmnXmlTreeElement* pParentElement)
	: SOCmnElement(SOCMNOBJECT_TYPE_BRANCH)
{
	m_name = elementName;

	if (pParentElement)
	{
		pParentElement->addBranch(this);
	}

	SOCmnListConfig listCfg;
	listCfg.m_type = SOCMNLIST_TYPE_ORG_MAP | SOCMNLIST_TYPE_KEY_STRING | SOCMNLIST_TYPE_OBJ_STRING | SOCMNLIST_TYPE_BLOCKED;
	listCfg.m_hashTableSize = 1;
	m_xmlAttributes.create(&listCfg);
	SOCmnString id;

	if (pParentElement)
	{
		id.format(_T("[XTE|%s|%s]"), (LPCTSTR)pParentElement->getObjTraceId(), (LPCTSTR)m_name);
	}
	else
	{
		id.format(_T("[XTE|-]"));
	}

	setObjTraceId(id);
	_TRACE(SOCMNTRACE_L_DEB, SOCMNTRACE_G_USER1, (_T("")));
}

SOCmnXmlTreeElement::~SOCmnXmlTreeElement(void)
{
	m_xmlAttributes.destroy();
}

void SOCmnXmlTreeElement::addXmlAttributes(LPCSTR* atts)
{
	USES_CONVERSION;
	_acp = CP_UTF8;   // enable use of conversion macros with code page UTF8
	LPCTSTR attName, attVal;
	DWORD i;

	for (i = 0; atts[i]; i += 2)
	{
		attName = A2CT(atts[i]);
		attVal = A2CT(atts[i + 1]);
		m_xmlAttributes.add(attVal, attName);
		_TRACE(SOCMNTRACE_L_DEB, SOCMNTRACE_G_USER1, (_T("%s = %s"), attName, attVal));
	}
}

void SOCmnXmlTreeElement::storeToString(SOCmnStringEx& data)
{
	SOCmnList<SOCmnXmlTreeElement> childList((SOCmnList<SOCmnXmlTreeElement> *)getBranchList());
	SOCmnListPosition posC;
	SOCmnXmlTreeElement* pChild;
	SOCmnStringEx hlp;
	SOCmnString name = getName();

	if (name.isEmpty())
	{
		// root
		data += _T("<?xml version=\"1.0\" encoding=\"UTF-8\"?");
	}
	else if (name == _T("OCT"))
	{
		hlp.format(_T("<OCT xmlns=\"http://www.softing.com/OCT/V%s\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xsi:schemaLocation=\"http://www.softing.com/OCT/V%s OCT.xsd\""), VERSION_TEXT, VERSION_TEXT);
		data += hlp;
	}
	else
	{
		// add myself
		hlp.format(_T("<%s"), getName());
		data += hlp;
		// add own attributes
		SOCmnStringList attributeList(getXmlAttributes());
		SOCmnString attrKey, attrVal;
		SOCmnListPosition posA;
		posA = attributeList.getStartPosition();

		while (posA)
		{
			attrVal = attributeList.getNext(posA, &attrKey);
			hlp.format(_T(" %s=\"%s\""), attrKey, attrVal);
			data += hlp;
		}
	}

	// add children
	posC = childList.getStartPosition();

	if (!posC)
	{
		data += _T("/>"); // end of myself
	}
	else
	{
		data += _T(">"); // end of myself

		while (posC)
		{
			pChild = childList.getNext(posC);
			pChild->storeToString(data);
		}

		if (!name.isEmpty())
		{
			hlp.format(_T("</%s>"), name);
		}

		data += hlp;
	}
}


SOCmnXmlTree::SOCmnXmlTree(LPCTSTR fileName)
	: SOCmnXmlFile(fileName)
{
	SOCMN_ALLOCATE_OBJECT_C(m_root, SOCmnXmlTreeElement(_T(""), NULL), (SOCmnXmlTreeElement*));
	m_pCurXmlElement = m_root;
}

SOCmnXmlTree::~SOCmnXmlTree(void)
{
	m_root.release();
}

void SOCmnXmlTree::handleStartElement(LPCSTR aName, LPCSTR* atts)
{
	USES_CONVERSION;
	SOCmnXmlFile::handleStartElement(aName, atts);
	SOCmnString name = A2CT(aName);
	removeNamespace(name);
	SOCmnPointer<SOCmnXmlTreeElement> newElement;
	SOCMN_ALLOCATE_OBJECT_C(newElement, SOCmnXmlTreeElement((LPCTSTR)name, m_pCurXmlElement), (SOCmnXmlTreeElement*));
	m_pCurXmlElement = newElement;
	newElement->addXmlAttributes(atts);
}

void SOCmnXmlTree::handleEndElement(LPCSTR aName)
{
	SOCmnXmlFile::handleEndElement(aName);

	if (m_pCurXmlElement.isNotNull())
	{
		m_pCurXmlElement = (SOCmnXmlTreeElement*)(SOCmnElement*)m_pCurXmlElement->getParentBranch();
	}
}

HRESULT SOCmnXmlTree::store(LPCTSTR aFileName, DWORD anEstimatedSize)
{
	USES_CONVERSION;
	SOCmnPointer<SOCmnXmlTreeElement> xmlRoot = getXmlRoot();
	SOCmnStringEx treeData;
	treeData.getBuffer(anEstimatedSize);
	xmlRoot->storeToString(treeData);
	// open configuration file
	HANDLE hFile = CreateFile(aFileName, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

	if (hFile == INVALID_HANDLE_VALUE)
	{
		return HRESULT_FROM_WIN32(GetLastError());
	}

	DWORD lenData = 0;

	if (!WriteFile(hFile, T2A(treeData), treeData.getLength(), &lenData, NULL))
	{
		CloseHandle(hFile);
		return HRESULT_FROM_WIN32(GetLastError());
	}

	CloseHandle(hFile);
	return S_OK;
}

